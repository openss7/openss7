/*****************************************************************************

 @(#) $RCSfile: exports.c,v $ $Name:  $($Revision: 1.1.1.7.4.19 $) $Date: 2006/02/20 11:38:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/02/20 11:38:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: exports.c,v $
 Revision 1.1.1.7.4.19  2006/02/20 11:38:51  brian
 - corrections for some 64bit architectures, from patches

 *****************************************************************************/

#ident "@(#) $RCSfile: exports.c,v $ $Name:  $($Revision: 1.1.1.7.4.19 $) $Date: 2006/02/20 11:38:51 $"

/************************************************************************
*                       STREAMS Exported Symbols			*
*************************************************************************
*									*
* Author:	David Grothe	<dave@gcom.com>				*
*									*
* Copyright (C) 2002  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/

#include <sys/LiS/linux-mdep.h>	/* redefine dev_t b4 any kernel incls */

#undef  EXPORT_SYMTAB
#define EXPORT_SYMTAB 1

#include <sys/stream.h>
#ifdef LINUX_POLL
#define	USE_LINUX_POLL_H	1
#include <linux/poll.h>
#else
#include <sys/poll.h>
#endif
#include <sys/lislocks.h>
#include <sys/lismem.h>
#include <sys/lispci.h>
#include <sys/cmn_err.h>
#include <sys/osif.h>

/************************************************************************
*                           Prototypes                                  *
*************************************************************************
*									*
* For things that aren't in the LiS header files.			*
*									*
************************************************************************/

extern int lis_major;
extern char lis_kernel_version[];
extern char lis_version[];
extern char lis_date[];
extern char *lis_stropts_file;
extern char *lis_poll_file;
extern int lis_num_cpus;
extern lis_atomic_t lis_in_syscall;
extern lis_atomic_t lis_queues_running;
extern volatile unsigned long lis_runq_cnts[LIS_NR_CPUS];
extern volatile unsigned long lis_queuerun_cnts[LIS_NR_CPUS];
extern volatile unsigned long lis_setqsched_cnts[LIS_NR_CPUS];
extern volatile unsigned long lis_setqsched_isr_cnts[LIS_NR_CPUS];
extern lis_atomic_t lis_strcount;
extern long lis_max_mem;
extern int lis_seq_cntr;

/************************************************************************
*                             Exports                                   *
*************************************************************************
*									*
* Symbols exported by LiS.  These are in alphabetical order.		*
*									*
************************************************************************/

#ifndef EXPORT_SYMBOL_GPL
#define EXPORT_SYMBOL_GPL(__x) EXPORT_SYMBOL(__x)
#endif

EXPORT_SYMBOL_GPL(lis_adjmsg);
EXPORT_SYMBOL_GPL(lis_alloc_atomic_fcn);
EXPORT_SYMBOL_GPL(lis_allocb);
EXPORT_SYMBOL_GPL(lis_allocb_physreq);
EXPORT_SYMBOL_GPL(lis_alloc_dma_fcn);
EXPORT_SYMBOL_GPL(lis_alloc_kernel_fcn);
EXPORT_SYMBOL_GPL(lis_allocq);
EXPORT_SYMBOL_GPL(lis_appq);
EXPORT_SYMBOL_GPL(lis_apush_get);	/* for sad */
EXPORT_SYMBOL_GPL(lis_apush_set);	/* for sad */
EXPORT_SYMBOL_GPL(lis_apush_vml);	/* for sad */
EXPORT_SYMBOL_GPL(lis_assert_fail);
EXPORT_SYMBOL_GPL(lis_atomic_add);
EXPORT_SYMBOL_GPL(lis_atomic_dec);
EXPORT_SYMBOL_GPL(lis_atomic_dec_and_test);
EXPORT_SYMBOL_GPL(lis_atomic_inc);
EXPORT_SYMBOL_GPL(lis_atomic_read);
EXPORT_SYMBOL_GPL(lis_atomic_set);
EXPORT_SYMBOL_GPL(lis_atomic_sub);
EXPORT_SYMBOL_GPL(lis_backenable);
EXPORT_SYMBOL_GPL(lis_backq);
EXPORT_SYMBOL_GPL(lis_backq_fcn);
EXPORT_SYMBOL_GPL(lis_bcanput);
EXPORT_SYMBOL_GPL(lis_bcanputnext);
EXPORT_SYMBOL_GPL(lis_bcanput_anyband);
EXPORT_SYMBOL_GPL(lis_bufcall);
EXPORT_SYMBOL_GPL(lis_clone_major);
EXPORT_SYMBOL_GPL(lis_cmn_err);
EXPORT_SYMBOL_GPL(lis_vcmn_err);
EXPORT_SYMBOL_GPL(lis_copyb);
EXPORT_SYMBOL_GPL(lis_copyin);
EXPORT_SYMBOL_GPL(lis_copymsg);
EXPORT_SYMBOL_GPL(lis_copyout);
EXPORT_SYMBOL_GPL(lis_date);
EXPORT_SYMBOL_GPL(lis_debug_mask);
EXPORT_SYMBOL_GPL(lis_debug_mask2);
EXPORT_SYMBOL_GPL(lis_disable_irq);
EXPORT_SYMBOL_GPL(lis_down_fcn);
EXPORT_SYMBOL_GPL(lis_down_nosig_fcn);
EXPORT_SYMBOL_GPL(lis_dsecs);
EXPORT_SYMBOL_GPL(lis_dupb);
EXPORT_SYMBOL_GPL(lis_dupmsg);
EXPORT_SYMBOL_GPL(lis_enable_irq);
EXPORT_SYMBOL_GPL(lis_esballoc);
EXPORT_SYMBOL_GPL(lis_esbbcall);
EXPORT_SYMBOL_GPL(lis_flushband);
EXPORT_SYMBOL_GPL(lis_findmod);
EXPORT_SYMBOL_GPL(lis_flushq);
EXPORT_SYMBOL_GPL(lis_fmod_sw);	/* for sad */
EXPORT_SYMBOL_GPL(lis_fstr_sw);	/* for sad */
EXPORT_SYMBOL_GPL(lis_free);
EXPORT_SYMBOL_GPL(lis_freeb);
EXPORT_SYMBOL_GPL(lis_free_dma);
EXPORT_SYMBOL_GPL(lis_free_irq);
EXPORT_SYMBOL_GPL(lis_free_mem_fcn);
EXPORT_SYMBOL_GPL(lis_freemsg);
EXPORT_SYMBOL_GPL(lis_free_pages_fcn);
EXPORT_SYMBOL_GPL(lis_freeq);
EXPORT_SYMBOL_GPL(lis_freezestr);
EXPORT_SYMBOL_GPL(lis_get_free_pages_atomic_fcn);
EXPORT_SYMBOL_GPL(lis_get_free_pages_fcn);
EXPORT_SYMBOL_GPL(lis_get_free_pages_kernel_fcn);
EXPORT_SYMBOL_GPL(lis_getint);
EXPORT_SYMBOL_GPL(lis_getq);
EXPORT_SYMBOL_GPL(lis_gettimeofday);
EXPORT_SYMBOL_GPL(lis_hitime);
EXPORT_SYMBOL_GPL(lis_insq);
EXPORT_SYMBOL_GPL(lis_in_interrupt);
#ifdef HAVE_KFUNC_INTERRUPTIBLE_SLEEP_ON
EXPORT_SYMBOL_GPL(lis_interruptible_sleep_on);
#endif
EXPORT_SYMBOL_GPL(lis_interruptible_sleep_on_timeout);
EXPORT_SYMBOL_GPL(lis_ioremap);
EXPORT_SYMBOL_GPL(lis_ioremap_nocache);
EXPORT_SYMBOL_GPL(lis_iounmap);
EXPORT_SYMBOL_GPL(lis_irqreturn_handled);
EXPORT_SYMBOL_GPL(lis_irqreturn_not_handled);
EXPORT_SYMBOL_GPL(lis_jiffies);
EXPORT_SYMBOL_GPL(lis_kernel_down);
EXPORT_SYMBOL_GPL(lis_kernel_up);
EXPORT_SYMBOL_GPL(lis_kernel_version);
EXPORT_SYMBOL_GPL(lis_kfree);
EXPORT_SYMBOL_GPL(lis_kmalloc);
EXPORT_SYMBOL_GPL(lis_linkb);
EXPORT_SYMBOL_GPL(lis_getmajor);
EXPORT_SYMBOL_GPL(lis_getminor);
EXPORT_SYMBOL_GPL(lis_makedevice);
EXPORT_SYMBOL_GPL(lis_major);
EXPORT_SYMBOL_GPL(lis_malloc);
EXPORT_SYMBOL_GPL(lis_mark_mem_fcn);
#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_) && !defined(_PPC64_LIS_) && !defined(_X86_64_LIS))
EXPORT_SYMBOL_GPL(lis_membar);
#endif				/* S390 or S390X or HPPA or PPC64 or X86_64 */
EXPORT_SYMBOL_GPL(lis_milli_to_ticks);
EXPORT_SYMBOL_GPL(lis_mknod);
EXPORT_SYMBOL_GPL(lis_msecs);
EXPORT_SYMBOL_GPL(lis_msgdsize);
EXPORT_SYMBOL_GPL(lis_msgpullup);
EXPORT_SYMBOL_GPL(lis_msgsize);
EXPORT_SYMBOL_GPL(lis_msg_type_name);
EXPORT_SYMBOL_GPL(lis_num_cpus);
EXPORT_SYMBOL_GPL(lis_osif_cli);
EXPORT_SYMBOL_GPL(lis_osif_do_gettimeofday);
EXPORT_SYMBOL_GPL(lis_osif_do_settimeofday);
EXPORT_SYMBOL_GPL(lis_osif_pci_disable_device);
EXPORT_SYMBOL_GPL(lis_osif_pci_enable_device);
#ifdef HAVE_KFUNC_PCI_FIND_CLASS
EXPORT_SYMBOL_GPL(lis_osif_pci_find_class);
#endif
EXPORT_SYMBOL_GPL(lis_osif_pci_find_device);
EXPORT_SYMBOL_GPL(lis_osif_pci_find_slot);
EXPORT_SYMBOL_GPL(lis_osif_pci_module_init);
EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_byte);
EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_dword);
EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_word);
EXPORT_SYMBOL_GPL(lis_osif_pci_set_master);
EXPORT_SYMBOL_GPL(lis_osif_pci_unregister_driver);
EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_byte);
EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_dword);
EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_word);
EXPORT_SYMBOL_GPL(lis_osif_sti);
EXPORT_SYMBOL_GPL(lis_own_spl);
#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_) && !defined(_PPC64_LIS_))
EXPORT_SYMBOL_GPL(lis_pcibios_find_class);
EXPORT_SYMBOL_GPL(lis_pcibios_find_device);
EXPORT_SYMBOL_GPL(lis_pcibios_init);
EXPORT_SYMBOL_GPL(lis_pcibios_present);
EXPORT_SYMBOL_GPL(lis_pcibios_read_config_byte);
EXPORT_SYMBOL_GPL(lis_pcibios_read_config_dword);
EXPORT_SYMBOL_GPL(lis_pcibios_read_config_word);
EXPORT_SYMBOL_GPL(lis_pcibios_strerror);
EXPORT_SYMBOL_GPL(lis_pcibios_write_config_byte);
EXPORT_SYMBOL_GPL(lis_pcibios_write_config_dword);
EXPORT_SYMBOL_GPL(lis_pcibios_write_config_word);
EXPORT_SYMBOL_GPL(lis_pci_alloc_consistent);
EXPORT_SYMBOL_GPL(lis_pci_disable_device);
EXPORT_SYMBOL_GPL(lis_pci_dma_handle_to_32);
EXPORT_SYMBOL_GPL(lis_pci_dma_handle_to_64);
EXPORT_SYMBOL_GPL(lis_pci_dma_supported);
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
EXPORT_SYMBOL_GPL(lis_pci_dma_sync_single);
#endif
EXPORT_SYMBOL_GPL(lis_pci_enable_device);
#ifdef HAVE_KFUNC_PCI_FIND_CLASS
EXPORT_SYMBOL_GPL(lis_pci_find_class);
#endif
EXPORT_SYMBOL_GPL(lis_pci_find_device);
EXPORT_SYMBOL_GPL(lis_pci_find_slot);
EXPORT_SYMBOL_GPL(lis_pci_free_consistent);
EXPORT_SYMBOL_GPL(lis_pci_map_single);
EXPORT_SYMBOL_GPL(lis_pci_read_config_byte);
EXPORT_SYMBOL_GPL(lis_pci_read_config_dword);
EXPORT_SYMBOL_GPL(lis_pci_read_config_word);
EXPORT_SYMBOL_GPL(lis_pci_set_dma_mask);
EXPORT_SYMBOL_GPL(lis_pci_set_master);
EXPORT_SYMBOL_GPL(lis_pci_unmap_single);
EXPORT_SYMBOL_GPL(lis_pci_write_config_byte);
EXPORT_SYMBOL_GPL(lis_pci_write_config_dword);
EXPORT_SYMBOL_GPL(lis_pci_write_config_word);
#endif				/* S390 or S390X or HPPA or PPC64 */
EXPORT_SYMBOL_GPL(lis_phys_to_virt);
EXPORT_SYMBOL_GPL(lis_print_block);
EXPORT_SYMBOL_GPL(lis_print_data);
EXPORT_SYMBOL_GPL(lis_printk);
EXPORT_SYMBOL_GPL(lis_print_mem);
EXPORT_SYMBOL_GPL(lis_print_msg);
EXPORT_SYMBOL_GPL(lis_print_queue);
EXPORT_SYMBOL_GPL(lis_pullupmsg);
EXPORT_SYMBOL_GPL(lis_putbq);
EXPORT_SYMBOL_GPL(lis_putbqf);
EXPORT_SYMBOL_GPL(lis_putbyte);
EXPORT_SYMBOL_GPL(lis_putctl);
EXPORT_SYMBOL_GPL(lis_putctl1);
EXPORT_SYMBOL_GPL(lis_putnextctl);
EXPORT_SYMBOL_GPL(lis_putnextctl1);
EXPORT_SYMBOL_GPL(lis_putq);
EXPORT_SYMBOL_GPL(lis_putqf);
EXPORT_SYMBOL_GPL(lis_qcountstrm);
EXPORT_SYMBOL_GPL(lis_qenable);
EXPORT_SYMBOL_GPL(lis_qprocsoff);
EXPORT_SYMBOL_GPL(lis_qprocson);
EXPORT_SYMBOL_GPL(lis_qsize);
EXPORT_SYMBOL_GPL(lis_queue_name);
EXPORT_SYMBOL_GPL(lis_register_strdev);
EXPORT_SYMBOL_GPL(lis_register_strmod);
EXPORT_SYMBOL_GPL(lis_register_driver_qlock_option);
EXPORT_SYMBOL_GPL(lis_register_module_qlock_option);
EXPORT_SYMBOL_GPL(lis_release_region);
EXPORT_SYMBOL_GPL(lis_request_dma);
EXPORT_SYMBOL_GPL(lis_request_irq);
EXPORT_SYMBOL_GPL(lis_request_region);
EXPORT_SYMBOL_GPL(lis_rmvb);
EXPORT_SYMBOL_GPL(lis_rmvq);
EXPORT_SYMBOL_GPL(lis_rw_lock_alloc_fcn);
EXPORT_SYMBOL_GPL(lis_rw_lock_free_fcn);
EXPORT_SYMBOL_GPL(lis_rw_lock_init_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_lock_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_lock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_lock_irqsave_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_unlock_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_unlock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_rw_read_unlock_irqrestore_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_lock_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_lock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_lock_irqsave_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_unlock_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_unlock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_rw_write_unlock_irqrestore_fcn);
EXPORT_SYMBOL_GPL(lis_safe_canenable);
EXPORT_SYMBOL_GPL(lis_safe_enableok);
EXPORT_SYMBOL_GPL(lis_safe_noenable);
EXPORT_SYMBOL_GPL(lis_safe_OTHERQ);
EXPORT_SYMBOL_GPL(lis_safe_putmsg);
EXPORT_SYMBOL_GPL(lis_safe_putnext);
EXPORT_SYMBOL_GPL(lis_safe_qreply);
EXPORT_SYMBOL_GPL(lis_safe_RD);
EXPORT_SYMBOL_GPL(lis_safe_SAMESTR);
EXPORT_SYMBOL_GPL(lis_safe_WR);
EXPORT_SYMBOL_GPL(lis_secs);
EXPORT_SYMBOL_GPL(lis_sem_alloc);
EXPORT_SYMBOL_GPL(lis_sem_destroy);
EXPORT_SYMBOL_GPL(lis_sem_init);
EXPORT_SYMBOL_GPL(lis_setq);	/* for clone and friends */
#ifdef HAVE_KFUNC_SLEEP_ON
EXPORT_SYMBOL_GPL(lis_sleep_on);
#endif
#ifdef HAVE_KFUNC_SLEEP_ON_TIMEOUT
EXPORT_SYMBOL_GPL(lis_sleep_on_timeout);
#endif
EXPORT_SYMBOL_GPL(lis_wait_event);
EXPORT_SYMBOL_GPL(lis_wait_event_interruptible);
EXPORT_SYMBOL_GPL(lis_spin_is_locked_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_alloc_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_free_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_init_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_spin_lock_irqsave_fcn);
EXPORT_SYMBOL_GPL(lis_spin_trylock_fcn);
EXPORT_SYMBOL_GPL(lis_spin_unlock_fcn);
EXPORT_SYMBOL_GPL(lis_spin_unlock_irq_fcn);
EXPORT_SYMBOL_GPL(lis_spin_unlock_irqrestore_fcn);
EXPORT_SYMBOL_GPL(lis_spl0_fcn);
EXPORT_SYMBOL_GPL(lis_splstr_fcn);
EXPORT_SYMBOL_GPL(lis_splx_fcn);
EXPORT_SYMBOL_GPL(lis_sprintf);
EXPORT_SYMBOL_GPL(lis_strm_name);
EXPORT_SYMBOL_GPL(lis_strm_name_from_queue);
EXPORT_SYMBOL_GPL(lis_stropts_file);
EXPORT_SYMBOL_GPL(lis_strqget);
EXPORT_SYMBOL_GPL(lis_strqset);
EXPORT_SYMBOL_GPL(lis_testb);
EXPORT_SYMBOL_GPL(lis_thread_start);
EXPORT_SYMBOL_GPL(lis_thread_stop);
EXPORT_SYMBOL_GPL(lis_timeout_fcn);
EXPORT_SYMBOL_GPL(lis_udelay);
EXPORT_SYMBOL_GPL(lis_unbufcall);
EXPORT_SYMBOL_GPL(lis_unfreezestr);
EXPORT_SYMBOL_GPL(lis_unlink);
EXPORT_SYMBOL_GPL(lis_unlinkb);
EXPORT_SYMBOL_GPL(lis_unregister_strdev);
EXPORT_SYMBOL_GPL(lis_unregister_strmod);
EXPORT_SYMBOL_GPL(lis_untimeout);
EXPORT_SYMBOL_GPL(lis_up_fcn);
EXPORT_SYMBOL_GPL(lis_usecs);
EXPORT_SYMBOL_GPL(lis_usectohz);
EXPORT_SYMBOL_GPL(lis_version);
EXPORT_SYMBOL_GPL(lis_vfree);
EXPORT_SYMBOL_GPL(lis_virt_to_phys);
EXPORT_SYMBOL_GPL(lis_vmalloc);
EXPORT_SYMBOL_GPL(lis_vremap);
EXPORT_SYMBOL_GPL(lis_vsprintf);
EXPORT_SYMBOL_GPL(lis_wake_up);
EXPORT_SYMBOL_GPL(lis_wake_up_interruptible);
EXPORT_SYMBOL_GPL(lis_xmsgsize);
EXPORT_SYMBOL_GPL(lis_zmalloc);

#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_) && !defined(_PPC64_LIS_))

EXPORT_SYMBOL_GPL(lis_osif_pci_alloc_consistent);
EXPORT_SYMBOL_GPL(lis_osif_pci_dma_supported);
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SG
EXPORT_SYMBOL_GPL(lis_osif_pci_dma_sync_sg);
#endif
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
EXPORT_SYMBOL_GPL(lis_osif_pci_dma_sync_single);
#endif
EXPORT_SYMBOL_GPL(lis_osif_pci_free_consistent);
EXPORT_SYMBOL_GPL(lis_osif_pci_map_sg);
EXPORT_SYMBOL_GPL(lis_osif_pci_map_single);
EXPORT_SYMBOL_GPL(lis_osif_pci_set_dma_mask);
EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_sg);
EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_single);
EXPORT_SYMBOL_GPL(lis_osif_sg_dma_address);
EXPORT_SYMBOL_GPL(lis_osif_sg_dma_len);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)	/* 2.4.13 or later */
#ifdef HAVE_KFUNC_PCI_DAC_SET_DMA_MASK
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_set_dma_mask);
#endif
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_supported);
EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_page);
EXPORT_SYMBOL_GPL(lis_osif_pci_map_page);
#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_sync_single);
#endif
#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_CPU
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_sync_single_for_cpu);
#endif
#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_DEVICE
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_sync_single_for_device);
#endif
#ifdef HAVE_KFUNC_PCI_DAC_DMA_TO_OFFSET
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_to_offset);
#endif
#ifdef HAVE_KFUNC_PCI_DAC_DMA_TO_PAGE
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_to_page);
#endif
#ifdef HAVE_KFUNC_PCI_DAC_PAGE_TO_DMA
EXPORT_SYMBOL_GPL(lis_osif_pci_dac_page_to_dma);
#endif
#endif				/* 2.4.13 */

#if (!defined(_X86_64_LIS))
/*
 * Wrapper functions
 */
EXPORT_SYMBOL_GPL(__wrap_strcpy);
EXPORT_SYMBOL_GPL(__wrap_strncpy);
EXPORT_SYMBOL_GPL(__wrap_strcat);
EXPORT_SYMBOL_GPL(__wrap_strncat);
EXPORT_SYMBOL_GPL(__wrap_strcmp);
EXPORT_SYMBOL_GPL(__wrap_strncmp);
EXPORT_SYMBOL_GPL(__wrap_strnicmp);
EXPORT_SYMBOL_GPL(__wrap_strchr);
EXPORT_SYMBOL_GPL(__wrap_strrchr);
EXPORT_SYMBOL_GPL(__wrap_strstr);
EXPORT_SYMBOL_GPL(__wrap_strlen);
EXPORT_SYMBOL_GPL(__wrap_memset);
EXPORT_SYMBOL_GPL(__wrap_memcpy);
EXPORT_SYMBOL_GPL(__wrap_memcmp);
#endif				/* X86_64 */

#endif				/* S390 or S390X or HPPA or PPC64 */
