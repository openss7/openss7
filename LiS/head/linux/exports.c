/************************************************************************
*                       STREAMS Exported Symbols			*
*************************************************************************
*									*
* Author:	David Grothe	<dave@gcom.com>				*
*									*
* Copyright (C) 2002  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS exports.c 1.13 5/19/03"

#ifdef MODVERSIONS
# ifdef LISMODVERS
#  include <sys/modversions.h>      /* /usr/src/LiS/include/sys */
# else
#  include <linux/modversions.h>
# endif
#endif
#define CONFIG_MODULES  1
#define EXPORT_SYMTAB   1
#define __NO_VERSION__	1	/* 2.2 kernel needs this */
#include <linux/module.h>


#include <sys/stream.h>
#include <sys/poll.h>
#include <sys/lislocks.h>
#include <sys/lismem.h>
#include <sys/lispci.h>
#include <sys/osif.h>

/************************************************************************
*                           Prototypes                                  *
*************************************************************************
*									*
* For things that aren't in the LiS header files.			*
*									*
************************************************************************/

void		 lis_qdetach(queue_t *, int, int, cred_t *) ;
unsigned	 lis_poll_bits(stdata_t *hd);
void		 lis_enable_intr(struct streamtab *, int, const char *);
void    	 lis_fdetach_all(void);
mblk_t		*lis_get_passfp(void);
int		 lis_can_unload(void);
void		 lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l);
const char	*lis_strm_name_from_queue(queue_t *q);
void		 lis_cmn_err(int err_lvl, char *fmt, ...);
struct stdata	*lis_file_str(struct file *f);
struct inode	*lis_file_inode(struct file *f);
void		 lis_set_file_str(struct file *f, struct stdata *s);
struct stdata	*lis_inode_str(struct inode *i);
void		 lis_set_inode_str(struct inode *i, struct stdata *s);
stdata_t	*lis_fd2str( int fd );
struct inode	*lis_old_inode( struct file *f, struct inode *i );
int		 lis_valid_mod_list(struct str_list ml);

extern int		lis_major;
extern char		lis_kernel_version[];
extern char		lis_version[] ;
extern char		lis_date[] ;
extern char		*lis_stropts_file ;
extern char		*lis_poll_file;
extern int              lis_num_cpus ;
extern lis_atomic_t     lis_in_syscall ;
extern lis_atomic_t     lis_queues_running ; 
extern volatile unsigned long lis_runq_cnts[LIS_NR_CPUS] ;
extern volatile unsigned long lis_queuerun_cnts[LIS_NR_CPUS] ;
extern volatile unsigned long lis_setqsched_cnts[LIS_NR_CPUS] ;
extern volatile unsigned long lis_setqsched_isr_cnts[LIS_NR_CPUS] ;
extern lis_atomic_t	lis_strcount ;
extern long		lis_max_mem ;
extern int        	lis_seq_cntr ;




/************************************************************************
*                             Exports                                   *
*************************************************************************
*									*
* Symbols exported by LiS.  These are in alphabetical order.		*
*									*
************************************************************************/

EXPORT_SYMBOL(lis_add_timer);
EXPORT_SYMBOL(lis_adjmsg);
EXPORT_SYMBOL(lis_alloc_atomic_fcn);
EXPORT_SYMBOL(lis_allocb);
EXPORT_SYMBOL(lis_allocb_physreq);
EXPORT_SYMBOL(lis_alloc_dma_fcn);
EXPORT_SYMBOL(lis_alloc_kernel_fcn);
EXPORT_SYMBOL(lis_allocq);
EXPORT_SYMBOL(lis_appq);
EXPORT_SYMBOL(lis_apush_get);
EXPORT_SYMBOL(lis_apushm);
EXPORT_SYMBOL(lis_apush_set);
EXPORT_SYMBOL(lis_assert_fail);
EXPORT_SYMBOL(lis_atomic_add);
EXPORT_SYMBOL(lis_atomic_dec);
EXPORT_SYMBOL(lis_atomic_dec_and_test);
EXPORT_SYMBOL(lis_atomic_inc);
EXPORT_SYMBOL(lis_atomic_read);
EXPORT_SYMBOL(lis_atomic_set);
EXPORT_SYMBOL(lis_atomic_sub);
EXPORT_SYMBOL(lis_backq);
EXPORT_SYMBOL(lis_backq_fcn);
EXPORT_SYMBOL(lis_bcanput);
EXPORT_SYMBOL(lis_bcanputnext);
EXPORT_SYMBOL(lis_bcanputnext_anyband);
EXPORT_SYMBOL(lis_bprintf);
EXPORT_SYMBOL(lis_bufcall);
EXPORT_SYMBOL(lis_can_unload);
EXPORT_SYMBOL(lis_check_guard);
EXPORT_SYMBOL(lis_check_mem);
EXPORT_SYMBOL(lis_check_q_magic);
EXPORT_SYMBOL(lis_check_region);
EXPORT_SYMBOL(lis_check_umem);
EXPORT_SYMBOL(lis_clone_major);
EXPORT_SYMBOL(lis_cmn_err);
EXPORT_SYMBOL(lis_copyb);
EXPORT_SYMBOL(lis_copyin);
EXPORT_SYMBOL(lis_copymsg);
EXPORT_SYMBOL(lis_copyout);
EXPORT_SYMBOL(lis_date);
EXPORT_SYMBOL(lis_debug_mask);
EXPORT_SYMBOL(lis_debug_mask2);
EXPORT_SYMBOL(lis_dec_mod_cnt_fcn);
EXPORT_SYMBOL(lis_del_timer);
EXPORT_SYMBOL(lis_disable_irq);
EXPORT_SYMBOL(lis_dobufcall);
EXPORT_SYMBOL(lis_doclose);
EXPORT_SYMBOL(lis_down_fcn);
EXPORT_SYMBOL(lis_dsecs);
EXPORT_SYMBOL(lis_dupb);
EXPORT_SYMBOL(lis_dupmsg);
EXPORT_SYMBOL(lis_enable_intr);
EXPORT_SYMBOL(lis_enable_irq);
EXPORT_SYMBOL(lis_esballoc);
EXPORT_SYMBOL(lis_esbbcall);
EXPORT_SYMBOL(lis_fattach);
EXPORT_SYMBOL(lis_fdetach);
EXPORT_SYMBOL(lis_fdetach_all);
EXPORT_SYMBOL(lis_fdetach_stream);
EXPORT_SYMBOL(lis_fifo_close_sync);
EXPORT_SYMBOL(lis_fifo_open_sync);
EXPORT_SYMBOL(lis_fifo_write_sync);
EXPORT_SYMBOL(lis_file_inode);			/* for connld */
EXPORT_SYMBOL(lis_file_str);			/* for connld */
EXPORT_SYMBOL(lis_find_strdev);
EXPORT_SYMBOL(lis_flushband);
EXPORT_SYMBOL(lis_flush_print_buffer);
EXPORT_SYMBOL(lis_flushq);
EXPORT_SYMBOL(lis_fmod_sw);
EXPORT_SYMBOL(lis_free);
EXPORT_SYMBOL(lis_freeb);
EXPORT_SYMBOL(lis_freedb);
EXPORT_SYMBOL(lis_free_dma);
EXPORT_SYMBOL(lis_free_irq);
EXPORT_SYMBOL(lis_free_mem_fcn);
EXPORT_SYMBOL(lis_freemsg);
EXPORT_SYMBOL(lis_free_pages_fcn);
EXPORT_SYMBOL(lis_free_passfp);
EXPORT_SYMBOL(lis_freeq);
EXPORT_SYMBOL(lis_fstr_sw);
EXPORT_SYMBOL(lis_get_fifo);
EXPORT_SYMBOL(lis_get_free_pages_atomic_fcn);
EXPORT_SYMBOL(lis_get_free_pages_fcn);
EXPORT_SYMBOL(lis_get_free_pages_kernel_fcn);
EXPORT_SYMBOL(lis_getint);
EXPORT_SYMBOL(lis_get_passfp);
EXPORT_SYMBOL(lis_get_pipe);
EXPORT_SYMBOL(lis_getq);
EXPORT_SYMBOL(lis_gettimeofday);
EXPORT_SYMBOL(lis_hitime);
EXPORT_SYMBOL(lis_inc_mod_cnt_fcn);
EXPORT_SYMBOL(lis_init_bufcall);
EXPORT_SYMBOL(lis_insq);
EXPORT_SYMBOL(lis_in_syscall);
EXPORT_SYMBOL(lis_interruptible_sleep_on);
EXPORT_SYMBOL(lis_ioc_fattach);
EXPORT_SYMBOL(lis_ioc_fdetach);
EXPORT_SYMBOL(lis_ioc_pipe);
EXPORT_SYMBOL(lis_ioremap);
EXPORT_SYMBOL(lis_ioremap_nocache);
EXPORT_SYMBOL(lis_iounmap);
EXPORT_SYMBOL(lis_jiffies);
EXPORT_SYMBOL(lis_kernel_down);
EXPORT_SYMBOL(lis_kernel_up);
EXPORT_SYMBOL(lis_kernel_version);
EXPORT_SYMBOL(lis__kfree);
EXPORT_SYMBOL(lis_kfree);
EXPORT_SYMBOL(lis_kill_pg);
EXPORT_SYMBOL(lis_kill_proc);
EXPORT_SYMBOL(lis__kmalloc);
EXPORT_SYMBOL(lis_kmalloc);
EXPORT_SYMBOL(lis_linkb);
EXPORT_SYMBOL(lis_loadable_load);
EXPORT_SYMBOL(lis_maj_min_name);
EXPORT_SYMBOL(lis_major);
EXPORT_SYMBOL(lis_malloc);
EXPORT_SYMBOL(lis_mark_mem);
EXPORT_SYMBOL(lis_max_mem);
EXPORT_SYMBOL(lis_membar);
EXPORT_SYMBOL(lis_milli_to_ticks);
EXPORT_SYMBOL(lis_mknod);
EXPORT_SYMBOL(lis_mount);
EXPORT_SYMBOL(lis_msecs);
EXPORT_SYMBOL(lis_msgdsize);
EXPORT_SYMBOL(lis_msgpullup);
EXPORT_SYMBOL(lis_msgsize);
EXPORT_SYMBOL(lis_msg_type_name);
EXPORT_SYMBOL(lis_num_cpus);
EXPORT_SYMBOL(lis_old_inode);			/* for connld */
EXPORT_SYMBOL(lis_open_cnt);
EXPORT_SYMBOL(lis_osif_cli);
EXPORT_SYMBOL(lis_osif_do_gettimeofday);
EXPORT_SYMBOL(lis_osif_do_settimeofday);
EXPORT_SYMBOL(lis_osif_pci_disable_device);
EXPORT_SYMBOL(lis_osif_pci_enable_device);
EXPORT_SYMBOL(lis_osif_pci_find_class);
EXPORT_SYMBOL(lis_osif_pci_find_device);
EXPORT_SYMBOL(lis_osif_pci_find_slot);
EXPORT_SYMBOL(lis_osif_pci_module_init);
EXPORT_SYMBOL(lis_osif_pci_read_config_byte);
EXPORT_SYMBOL(lis_osif_pci_read_config_dword);
EXPORT_SYMBOL(lis_osif_pci_read_config_word);
EXPORT_SYMBOL(lis_osif_pci_set_master);
EXPORT_SYMBOL(lis_osif_pci_unregister_driver);
EXPORT_SYMBOL(lis_osif_pci_write_config_byte);
EXPORT_SYMBOL(lis_osif_pci_write_config_dword);
EXPORT_SYMBOL(lis_osif_pci_write_config_word);
EXPORT_SYMBOL(lis_osif_sti);
EXPORT_SYMBOL(lis_own_spl);
EXPORT_SYMBOL(lis_pcibios_find_class);
EXPORT_SYMBOL(lis_pcibios_find_device);
EXPORT_SYMBOL(lis_pcibios_init);
EXPORT_SYMBOL(lis_pcibios_present);
EXPORT_SYMBOL(lis_pcibios_read_config_byte);
EXPORT_SYMBOL(lis_pcibios_read_config_dword);
EXPORT_SYMBOL(lis_pcibios_read_config_word);
EXPORT_SYMBOL(lis_pcibios_strerror);
EXPORT_SYMBOL(lis_pcibios_write_config_byte);
EXPORT_SYMBOL(lis_pcibios_write_config_dword);
EXPORT_SYMBOL(lis_pcibios_write_config_word);
EXPORT_SYMBOL(lis_pci_alloc_consistent);
EXPORT_SYMBOL(lis_pci_cleanup);
EXPORT_SYMBOL(lis_pci_disable_device);
EXPORT_SYMBOL(lis_pci_dma_handle_to_32);
EXPORT_SYMBOL(lis_pci_dma_handle_to_64);
EXPORT_SYMBOL(lis_pci_dma_supported);
EXPORT_SYMBOL(lis_pci_dma_sync_single);
EXPORT_SYMBOL(lis_pci_enable_device);
EXPORT_SYMBOL(lis_pci_find_class);
EXPORT_SYMBOL(lis_pci_find_device);
EXPORT_SYMBOL(lis_pci_find_slot);
EXPORT_SYMBOL(lis_pci_free_consistent);
EXPORT_SYMBOL(lis_pci_map_single);
EXPORT_SYMBOL(lis_pci_read_config_byte);
EXPORT_SYMBOL(lis_pci_read_config_dword);
EXPORT_SYMBOL(lis_pci_read_config_word);
EXPORT_SYMBOL(lis_pci_set_dma_mask);
EXPORT_SYMBOL(lis_pci_set_master);
EXPORT_SYMBOL(lis_pci_unmap_single);
EXPORT_SYMBOL(lis_pci_write_config_byte);
EXPORT_SYMBOL(lis_pci_write_config_dword);
EXPORT_SYMBOL(lis_pci_write_config_word);
EXPORT_SYMBOL(lis_phys_to_virt);
EXPORT_SYMBOL(lis_pipe);
EXPORT_SYMBOL(lis_poll_2_1);
EXPORT_SYMBOL(lis_poll_bits);
EXPORT_SYMBOL(lis_poll_events);
EXPORT_SYMBOL(lis_poll_file);
EXPORT_SYMBOL(lis_print_block);
EXPORT_SYMBOL(lis_print_data);
EXPORT_SYMBOL(lis_printk);
EXPORT_SYMBOL(lis_print_mem);
EXPORT_SYMBOL(lis_print_msg);
EXPORT_SYMBOL(lis_print_queue);
EXPORT_SYMBOL(lis_print_queues);
EXPORT_SYMBOL(lis_print_spl_track);
EXPORT_SYMBOL(lis_print_stream);
EXPORT_SYMBOL(lis_pullupmsg);
EXPORT_SYMBOL(lis_putbq);
EXPORT_SYMBOL(lis_putbyte);
EXPORT_SYMBOL(lis_putctl);
EXPORT_SYMBOL(lis_putctl1);
EXPORT_SYMBOL(lis_putnextctl);
EXPORT_SYMBOL(lis_putnextctl1);
EXPORT_SYMBOL(lis_putq);
EXPORT_SYMBOL(lis_qcountstrm);
EXPORT_SYMBOL(lis_qdetach);
EXPORT_SYMBOL(lis_qenable);
EXPORT_SYMBOL(lis_qprocsoff);
EXPORT_SYMBOL(lis_qprocson);
EXPORT_SYMBOL(lis_qsize);
EXPORT_SYMBOL(lis_queue_name);
EXPORT_SYMBOL(lis_queuerun_cnts);
EXPORT_SYMBOL(lis_queues_running);
EXPORT_SYMBOL(lis_recvfd);
EXPORT_SYMBOL(lis_register_strdev);
EXPORT_SYMBOL(lis_register_strmod);
EXPORT_SYMBOL(lis_release_region);
EXPORT_SYMBOL(lis_request_dma);
EXPORT_SYMBOL(lis_request_irq);
EXPORT_SYMBOL(lis_request_region);
EXPORT_SYMBOL(lis_rmvb);
EXPORT_SYMBOL(lis_rmvq);
EXPORT_SYMBOL(lis_runq_cnts);
EXPORT_SYMBOL(lis_runq_req_cnt);
EXPORT_SYMBOL(lis_rw_lock_alloc_fcn);
EXPORT_SYMBOL(lis_rw_lock_free_fcn);
EXPORT_SYMBOL(lis_rw_lock_init_fcn);
EXPORT_SYMBOL(lis_rw_read_lock_fcn);
EXPORT_SYMBOL(lis_rw_read_lock_irq_fcn);
EXPORT_SYMBOL(lis_rw_read_lock_irqsave_fcn);
EXPORT_SYMBOL(lis_rw_read_unlock_fcn);
EXPORT_SYMBOL(lis_rw_read_unlock_irq_fcn);
EXPORT_SYMBOL(lis_rw_read_unlock_irqrestore_fcn);
EXPORT_SYMBOL(lis_rw_write_lock_fcn);
EXPORT_SYMBOL(lis_rw_write_lock_irq_fcn);
EXPORT_SYMBOL(lis_rw_write_lock_irqsave_fcn);
EXPORT_SYMBOL(lis_rw_write_unlock_fcn);
EXPORT_SYMBOL(lis_rw_write_unlock_irq_fcn);
EXPORT_SYMBOL(lis_rw_write_unlock_irqrestore_fcn);
EXPORT_SYMBOL(lis_safe_canenable);
EXPORT_SYMBOL(lis_safe_enableok);
EXPORT_SYMBOL(lis_safe_noenable);
EXPORT_SYMBOL(lis_safe_OTHERQ);
EXPORT_SYMBOL(lis_safe_putmsg);
EXPORT_SYMBOL(lis_safe_putnext);
EXPORT_SYMBOL(lis_safe_qreply);
EXPORT_SYMBOL(lis_safe_RD);
EXPORT_SYMBOL(lis_safe_SAMESTR);
EXPORT_SYMBOL(lis_safe_WR);
EXPORT_SYMBOL(lis_secs);
EXPORT_SYMBOL(lis_sem_alloc);
EXPORT_SYMBOL(lis_sem_destroy);
EXPORT_SYMBOL(lis_sem_init);
EXPORT_SYMBOL(lis_sendfd);
EXPORT_SYMBOL(lis_seq_cntr);
EXPORT_SYMBOL(lis_set_file_str);		/* for connld */
EXPORT_SYMBOL(lis_setq);
EXPORT_SYMBOL(lis_setqsched);
EXPORT_SYMBOL(lis_setqsched_cnts);
EXPORT_SYMBOL(lis_setqsched_isr_cnts);
EXPORT_SYMBOL(lis_sleep_on);
EXPORT_SYMBOL(lis_spin_is_locked_fcn);
EXPORT_SYMBOL(lis_spin_lock_alloc_fcn);
EXPORT_SYMBOL(lis_spin_lock_fcn);
EXPORT_SYMBOL(lis_spin_lock_free_fcn);
EXPORT_SYMBOL(lis_spin_lock_init_fcn);
EXPORT_SYMBOL(lis_spin_lock_irq_fcn);
EXPORT_SYMBOL(lis_spin_lock_irqsave_fcn);
EXPORT_SYMBOL(lis_spin_trylock_fcn);
EXPORT_SYMBOL(lis_spin_unlock_fcn);
EXPORT_SYMBOL(lis_spin_unlock_irq_fcn);
EXPORT_SYMBOL(lis_spin_unlock_irqrestore_fcn);
EXPORT_SYMBOL(lis_spl0_fcn);
EXPORT_SYMBOL(lis_splstr_fcn);
EXPORT_SYMBOL(lis_splx_fcn);
EXPORT_SYMBOL(lis_sprintf);
EXPORT_SYMBOL(lis_stdata_cnt);
EXPORT_SYMBOL(lis_strclose);
EXPORT_SYMBOL(lis_strcount);
EXPORT_SYMBOL(lis_strgetpmsg);
EXPORT_SYMBOL(lis_strioctl);
EXPORT_SYMBOL(lis_strm_name);
EXPORT_SYMBOL(lis_strm_name_from_queue);
EXPORT_SYMBOL(lis_stropen);
EXPORT_SYMBOL(lis_stropts_file);
EXPORT_SYMBOL(lis_strputpmsg);
EXPORT_SYMBOL(lis_strqget);
EXPORT_SYMBOL(lis_strqset);
EXPORT_SYMBOL(lis_strread);
EXPORT_SYMBOL(lis_strstats);
EXPORT_SYMBOL(lis_strwrite);
EXPORT_SYMBOL(lis_testb);
EXPORT_SYMBOL(lis_thread_start);
EXPORT_SYMBOL(lis_thread_stop);
EXPORT_SYMBOL(lis_timeout_fcn);
EXPORT_SYMBOL(lis_udelay);
EXPORT_SYMBOL(lis_umount2);
EXPORT_SYMBOL(lis_unbufcall);
EXPORT_SYMBOL(lis_unlink);
EXPORT_SYMBOL(lis_unlinkb);
EXPORT_SYMBOL(lis_unregister_strdev);
EXPORT_SYMBOL(lis_unregister_strmod);
EXPORT_SYMBOL(lis_untimeout);
EXPORT_SYMBOL(lis_up_fcn);
EXPORT_SYMBOL(lis_usecs);
EXPORT_SYMBOL(lis_usectohz);
EXPORT_SYMBOL(lis_valid_mod_list);		/* for sad driver */
EXPORT_SYMBOL(lis_version);
EXPORT_SYMBOL(lis_vfree);
EXPORT_SYMBOL(lis_virt_to_phys);
EXPORT_SYMBOL(lis_vmalloc);
EXPORT_SYMBOL(lis_vremap);
EXPORT_SYMBOL(lis_vsprintf);
EXPORT_SYMBOL(lis_wake_up);
EXPORT_SYMBOL(lis_wake_up_interruptible);
EXPORT_SYMBOL(lis_xmsgsize);
EXPORT_SYMBOL(lis_zmalloc);

#if defined(KERNEL_2_3)			/* only for 2.4 or newer kernels */

EXPORT_SYMBOL(lis_osif_pci_alloc_consistent);
EXPORT_SYMBOL(lis_osif_pci_dma_supported);
EXPORT_SYMBOL(lis_osif_pci_dma_sync_sg);
EXPORT_SYMBOL(lis_osif_pci_dma_sync_single);
EXPORT_SYMBOL(lis_osif_pci_free_consistent);
EXPORT_SYMBOL(lis_osif_pci_map_sg);
EXPORT_SYMBOL(lis_osif_pci_map_single);
EXPORT_SYMBOL(lis_osif_pci_set_dma_mask);
EXPORT_SYMBOL(lis_osif_pci_unmap_sg);
EXPORT_SYMBOL(lis_osif_pci_unmap_single);
EXPORT_SYMBOL(lis_osif_sg_dma_address);
EXPORT_SYMBOL(lis_osif_sg_dma_len);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)      /* 2.4.13 or later */
EXPORT_SYMBOL(lis_osif_pci_dac_set_dma_mask);
EXPORT_SYMBOL(lis_osif_pci_dac_dma_supported);
EXPORT_SYMBOL(lis_osif_pci_unmap_page);
EXPORT_SYMBOL(lis_osif_pci_map_page);
EXPORT_SYMBOL(lis_osif_pci_dac_dma_sync_single);
EXPORT_SYMBOL(lis_osif_pci_dac_dma_to_offset);
EXPORT_SYMBOL(lis_osif_pci_dac_dma_to_page);
EXPORT_SYMBOL(lis_osif_pci_dac_page_to_dma);
#endif                                  /* 2.4.13 */

#endif					/* defined(KERNEL_2_3) */
