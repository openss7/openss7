/*****************************************************************************

 @(#) $Id: osif.h,v 1.1.1.4.4.9 2005/12/18 05:41:24 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/12/18 05:41:24 $ by $Author: brian $

 *****************************************************************************/

/************************************************************************
*                      Operating System Interface                       *
*************************************************************************
*									*
* These are routines that call kernel routines indirectly.  This allows	*
* driver writers to reference the lis_... names in compiled code	*
* and the osif.c module itself will use the mangled names of the	*
* kernel routines when "modversions" is set.				*
*									*
* Include this file AFTER any linux includes.				*
*									*
* This file is only interpreted for LINUX builds.  User-level and other	*
* portable builds bypass this file.					*
*									*
************************************************************************/
#if defined(LINUX) && !defined(OSIF_H)
#define OSIF_H			/* file included */

#ident "@(#) $RCSfile: osif.h,v $ $Name:  $($Revision: 1.1.1.4.4.9 $) $Date: 2005/12/18 05:41:24 $"

#include <sys/LiS/genconf.h>

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#endif

#if LINUX_VERSION_CODE >= 0x020100	/* 2.1 kernel */
# if !defined(_SPARC_LIS_) && !defined(LINUX_PCI_H)
#  ifdef RH_71_KLUDGE		/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM		/* b_page has semi-circular reference */
#  endif
# include <linux/pci.h>		/* need struct definitions */
# endif
#endif

#ifndef _STDARG_H
#include <stdarg.h>		/* for va_list */
#endif

#ifndef _LINUX_WAIT_H
#include <linux/wait.h>		/* for struct wait_queue */
#endif

#ifndef _LINUX_TIMER_H
#include <linux/timer.h>	/* for struct timer_list */
#endif

#ifndef _LINUX_TIME_H		/* for struct timeval */
#include <linux/time.h>
#endif

#if !defined(INCL_FROM_OSIF_DRIVER)

/*
 * Redefine the "official" kernel names of these routines to the
 * "lis_..." names.
 *
 * Modversions has already defined most of these names to be the
 * version-mangled form.  So we have to test each one to see if
 * it has already been defined before redefining to the lis_... form.
 * This is messy but saves many warning messages when compiling
 * with modversions.
 */

#if (!defined(_SPARC_LIS_) && !defined(_PPC_LIS_))

#ifdef pcibios_present
#undef pcibios_present
#endif
#define	pcibios_present			lis_pcibios_present
#ifdef pcibios_init
#undef pcibios_init
#endif
#define	pcibios_init			lis_pcibios_init
#ifdef pcibios_find_class
#undef pcibios_find_class
#endif
#define	pcibios_find_class		lis_pcibios_find_class
#ifdef pcibios_find_device
#undef pcibios_find_device
#endif
#define	pcibios_find_device		lis_pcibios_find_device
#ifdef pcibios_read_config_byte
#undef pcibios_read_config_byte
#endif
#define	pcibios_read_config_byte	lis_pcibios_read_config_byte
#ifdef pcibios_read_config_word
#undef pcibios_read_config_word
#endif
#define	pcibios_read_config_word	lis_pcibios_read_config_word
#ifdef pcibios_read_config_dword
#undef pcibios_read_config_dword
#endif
#define	pcibios_read_config_dword	lis_pcibios_read_config_dword
#ifdef pcibios_write_config_byte
#undef pcibios_write_config_byte
#endif
#define	pcibios_write_config_byte	lis_pcibios_write_config_byte
#ifdef pcibios_write_config_word
#undef pcibios_write_config_word
#endif
#define	pcibios_write_config_word	lis_pcibios_write_config_word
#ifdef pcibios_write_config_dword
#undef pcibios_write_config_dword
#endif
#define	pcibios_write_config_dword	lis_pcibios_write_config_dword
#ifdef pcibios_strerror
#undef pcibios_strerror
#endif
#define	pcibios_strerror		lis_pcibios_strerror

#endif				/* ifndef _SPARC_LIS_, _PPC_LIS_ */

#if LINUX_VERSION_CODE >= 0x020100	/* 2.1 kernel */
#ifdef pci_find_device
#undef pci_find_device
#endif
#define	pci_find_device			lis_osif_pci_find_device
#ifdef HAVE_KFUNC_PCI_FIND_CLASS
#ifdef pci_find_class
#undef pci_find_class
#endif
#define	pci_find_class			lis_osif_pci_find_class
#endif
#ifdef pci_find_slot
#undef pci_find_slot
#endif
#define	pci_find_slot			lis_osif_pci_find_slot
#ifdef pci_read_config_byte
#undef pci_read_config_byte
#endif
#define	pci_read_config_byte		lis_osif_pci_read_config_byte
#ifdef pci_read_config_word
#undef pci_read_config_word
#endif
#define	pci_read_config_word		lis_osif_pci_read_config_word
#ifdef pci_read_config_dword
#undef pci_read_config_dword
#endif
#define	pci_read_config_dword		lis_osif_pci_read_config_dword
#ifdef pci_write_config_byte
#undef pci_write_config_byte
#endif
#define	pci_write_config_byte		lis_osif_pci_write_config_byte
#ifdef pci_write_config_word
#undef pci_write_config_word
#endif
#define	pci_write_config_word		lis_osif_pci_write_config_word
#ifdef pci_write_config_dword
#undef pci_write_config_dword
#endif
#define	pci_write_config_dword		lis_osif_pci_write_config_dword
#ifdef pci_set_master
#undef pci_set_master
#endif
#define	pci_set_master			lis_osif_pci_set_master
#ifdef pci_enable_device
#undef pci_enable_device
#endif
#define pci_enable_device               lis_osif_pci_enable_device
#ifdef pci_disable_device
#undef pci_disable_device
#endif
#define pci_disable_device               lis_osif_pci_disable_device
#ifdef pci_module_init
#undef pci_module_init
#endif
#define pci_module_init       lis_osif_pci_module_init
#ifdef pci_unregister_driver
#undef pci_unregister_driver
#endif
#define pci_unregister_driver lis_osif_pci_unregister_driver

#endif				/* 2.1 kernel */

#if LINUX_VERSION_CODE >= 0x020400	/* 2.4 kernel */

#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_))
#ifdef pci_alloc_consistent
#undef pci_alloc_consistent
#endif
#define pci_alloc_consistent lis_osif_pci_alloc_consistent
#ifdef pci_free_consistent
#undef pci_free_consistent
#endif
#define pci_free_consistent lis_osif_pci_free_consistent
#ifdef pci_map_single
#undef pci_map_single
#endif
#define pci_map_single lis_osif_pci_map_single
#ifdef pci_unmap_single
#undef pci_unmap_single
#endif
#define pci_unmap_single lis_osif_pci_unmap_single
#ifdef pci_map_page
#undef pci_map_page
#endif
#define pci_map_page lis_osif_pci_map_page
#ifdef pci_unmap_page
#undef pci_unmap_page
#endif
#define pci_unmap_page lis_osif_pci_unmap_page
#ifdef pci_map_sg
#undef pci_map_sg
#endif
#define pci_map_sg lis_osif_pci_map_sg
#ifdef pci_unmap_sg
#undef pci_unmap_sg
#endif
#define pci_unmap_sg lis_osif_pci_unmap_sg
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
#ifdef pci_dma_sync_single
#undef pci_dma_sync_single
#endif
#define pci_dma_sync_single lis_osif_pci_dma_sync_single
#endif
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SG
#ifdef pci_dma_sync_sg
#undef pci_dma_sync_sg
#endif
#define pci_dma_sync_sg lis_osif_pci_dma_sync_sg
#endif
#ifdef pci_dma_supported
#undef pci_dma_supported
#endif
#define pci_dma_supported lis_osif_pci_dma_supported
#ifdef pci_dac_dma_supported
#undef pci_dac_dma_supported
#endif
#define pci_dac_dma_supported lis_osif_pci_dac_dma_supported
#ifdef pci_set_dma_mask
#undef pci_set_dma_mask
#endif
#define pci_set_dma_mask lis_osif_pci_set_dma_mask
#ifdef pci_dac_set_dma_mask
#undef pci_dac_set_dma_mask
#endif
#define pci_dac_set_dma_mask lis_osif_pci_dac_set_dma_mask
#ifdef pci_dac_page_to_dma
#undef pci_dac_page_to_dma
#endif
#define pci_dac_page_to_dma lis_osif_pci_dac_page_to_dma
#ifdef pci_dac_dma_to_page
#undef pci_dac_dma_to_page
#endif
#define pci_dac_dma_to_page lis_osif_pci_dac_dma_to_page
#ifdef pci_dac_dma_to_offset
#undef pci_dac_dma_to_offset
#endif
#define pci_dac_dma_to_offset lis_osif_pci_dac_dma_to_offset
#ifdef pci_dac_dma_sync_single
#undef pci_dac_dma_sync_single
#endif
#define pci_dac_dma_sync_single lis_osif_pci_dac_dma_sync_single
#ifdef pci_dac_dma_sync_single_for_cpu
#undef pci_dac_dma_sync_single_for_cpu
#endif
#define pci_dac_dma_sync_single_for_cpu lis_osif_pci_dac_dma_sync_single_for_cpu
#ifdef pci_dac_dma_sync_single_for_device
#undef pci_dac_dma_sync_single_for_device
#endif
#define pci_dac_dma_sync_single_for_device lis_osif_pci_dac_dma_sync_single_for_device
#ifdef sg_dma_address
#undef sg_dma_address
#endif
#define sg_dma_address lis_osif_sg_dma_address
#ifdef sg_dma_len
#undef sg_dma_len
#endif
#define sg_dma_len lis_osif_sg_dma_len

#endif				/* S390 or S390X */
#endif

#ifdef request_irq
#undef request_irq
#endif
#define	request_irq			lis_request_irq
#ifdef free_irq
#undef free_irq
#endif
#define	free_irq			lis_free_irq
#ifdef disable_irq
#undef disable_irq
#endif
#define	disable_irq			lis_disable_irq
#ifdef enable_irq
#undef enable_irq
#endif
#define	enable_irq			lis_enable_irq
#ifdef cli
# undef cli
#endif
#define cli                   		lis_osif_cli
#ifdef sti
# undef sti
#endif
#define sti                   		lis_osif_sti

#ifdef ioremap
#undef ioremap
#endif
#define	ioremap				lis_ioremap
#ifdef ioremap_nocache
#undef ioremap_nocache
#endif
#define	ioremap_nocache			lis_ioremap_nocache
#ifdef iounmap
#undef iounmap
#endif
#define	iounmap				lis_iounmap
#ifdef vremap
#undef vremap
#endif
#define	vremap				lis_vremap
#ifdef virt_to_phys
#undef virt_to_phys
#endif
#define virt_to_phys			lis_virt_to_phys
#ifdef phys_to_virt
#undef phys_to_virt
#endif
#define phys_to_virt			lis_phys_to_virt

#ifdef check_region
#undef check_region
#endif
#define	check_region			lis_check_region
#ifdef request_region
#undef request_region
#endif
#define	request_region			lis_request_region
#ifdef release_region
#undef release_region
#endif
#define	release_region			lis_release_region

#ifdef check_mem_region
#undef check_mem_region
#endif
#define	check_mem_region		lis_check_mem_region
#ifdef request_mem_region
#undef request_mem_region
#endif
#define	request_mem_region		lis_request_mem_region
#ifdef release_mem_region
#undef release_mem_region
#endif
#define	release_mem_region		lis_release_mem_region

#ifdef add_timer
#undef add_timer
#endif
#define add_timer			lis_add_timer
#ifdef del_timer
#undef del_timer
#endif
#define del_timer			lis_del_timer

#ifdef do_gettimeofday
#undef do_gettimeofday
#endif
#define do_gettimeofday			lis_osif_do_gettimeofday
#ifdef do_settimeofday
#undef do_settimeofday
#endif
#define do_settimeofday			lis_osif_do_settimeofday

#ifdef kmalloc
#undef kmalloc
#endif
#define	kmalloc				lis_kmalloc
#ifdef kfree
#undef kfree
#endif
#define	kfree				lis_kfree
#ifdef vmalloc
#undef vmalloc
#endif
#define	vmalloc				lis_vmalloc
#ifdef vfree
#undef vfree
#endif
#define	vfree				lis_vfree

#ifdef request_dma
#undef request_dma
#endif
#define	request_dma			lis_request_dma
#ifdef free_dma
#undef free_dma
#endif
#define	free_dma			lis_free_dma

#ifdef udelay
#undef udelay
#endif
#define	udelay				lis_udelay
#ifdef jiffies
#undef jiffies
#endif
#define	jiffies				lis_jiffies()

#ifdef printk
#undef printk
#endif
#define	printk				lis_printk
#ifdef sprintf
#undef sprintf
#endif
#define sprintf				lis_sprintf
#ifdef vsprintf
#undef vsprintf
#endif
#define	vsprintf			lis_vsprintf

#ifdef HAVE_KFUNC_SLEEP_ON
#ifdef sleep_on
#undef sleep_on
#endif
#define	sleep_on			lis_sleep_on
#endif
#ifdef HAVE_KFUNC_INTERRUPTIBLE_SLEEP_ON
#ifdef interruptible_sleep_on
#undef interruptible_sleep_on
#endif
#define	interruptible_sleep_on		lis_interruptible_sleep_on
#endif
#ifdef HAVE_KFUNC_SLEEP_ON_TIMEOUT
#ifdef sleep_on_timeout
#undef sleep_on_timeout
#endif
#define	sleep_on_timeout		lis_sleep_on_timeout
#endif
#ifdef interruptible_sleep_on_timeout
#undef interruptible_sleep_on_timeout
#endif
#define	interruptible_sleep_on_timeout	lis_interruptible_sleep_on_timeout
#ifdef wait_event
#undef wait_event
#endif
#define	wait_event			lis_wait_event
#ifdef wait_event_interruptible
#undef wait_event_interruptible
#endif
#define	wait_event_interruptible	lis_wait_event_interruptible
#ifdef wake_up
#undef wake_up
#endif
#define	wake_up				lis_wake_up
#ifdef wake_up_interruptible
#undef wake_up_interruptible
#endif
#define	wake_up_interruptible		lis_wake_up_interruptible

#endif				/* !defined(INCL_FROM_OSIF_DRIVER) */

/*
 * PCI BIOS routines
 */

int _RP lis_pcibios_present(void);

#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
unsigned long _RP lis_pcibios_init(unsigned long memory_start, unsigned long memory_end);
#else				/* 2.1 or 2.2 kernel */
void _RP lis_pcibios_init(void);
#endif
int _RP lis_pcibios_find_class(unsigned int class_code, unsigned short index, unsigned char *bus,
			       unsigned char *dev_fn);
int _RP lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id, unsigned short index,
				unsigned char *bus, unsigned char *dev_fn);
int _RP lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
				     unsigned char *val);
int _RP lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
				     unsigned short *val);
int _RP lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
				      unsigned int *val);
int _RP lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
				      unsigned char val);
int _RP lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
				      unsigned short val);
int _RP lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
				       unsigned int val);
const char *_RP lis_pcibios_strerror(int error);

/*
 * New style PCI interface
 */
struct pci_dev *_RP lis_osif_pci_find_device(unsigned int vendor, unsigned int device,
					     struct pci_dev *from);

#ifdef HAVE_KFUNC_PCI_FIND_CLASS
#ifdef __LIS_INTERNAL__
struct pci_dev
*_RP lis_osif_pci_find_class(unsigned int class, struct pci_dev *from);
#endif
#endif
struct pci_dev *_RP lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn);

int _RP lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val);
int _RP lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val);
int _RP lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val);
int _RP lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val);
int _RP lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val);
int _RP lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val);
void _RP lis_osif_pci_set_master(struct pci_dev *dev);
int _RP lis_osif_pci_enable_device(struct pci_dev *dev);
void _RP lis_osif_pci_disable_device(struct pci_dev *dev);

#if LINUX_VERSION_CODE >= 0x020400	/* 2.4 kernel */
int _RP lis_osif_pci_module_init(struct pci_driver *p);
void _RP lis_osif_pci_unregister_driver(struct pci_driver *p);
#else				/* older kernel */
int _RP lis_osif_pci_module_init(void *p);
void _RP lis_osif_pci_unregister_driver(void *p);
#endif

#if LINUX_VERSION_CODE >= 0x020400	/* 2.4 kernel */

extern void *_RP lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
					       dma_addr_t *dma_handle);
extern void _RP lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr,
					     dma_addr_t dma_handle);
extern dma_addr_t _RP lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size,
					      int direction);
extern void _RP lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size,
					  int direction);
extern int _RP lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				   int direction);
extern void _RP lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				      int direction);

#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
#ifdef __LIS_INTERNAL__
extern void
_RP lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size,
				 int direction);
#endif
#endif
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SG
#ifdef __LIS_INTERNAL__
extern void
_RP lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems,
			     int direction);
#endif
#endif
extern int _RP lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask);
extern int _RP lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern dma_addr_t _RP lis_osif_sg_dma_address(struct scatterlist *sg);
extern size_t _RP lis_osif_sg_dma_len(struct scatterlist *sg);

#if  BITS_PER_LONG == 64
typedef u64 dma64_addr_t;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)	/* 2.4.13 or later */
#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_))
extern void _RP lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size,
					int direction);
extern int _RP lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern dma_addr_t _RP lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page,
					    unsigned long offset, size_t size, int direction);
extern int _RP lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask);
extern dma64_addr_t _RP lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
						     unsigned long offset, int direction);
extern struct page *_RP lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr);
extern unsigned long _RP lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
							dma64_addr_t dma_addr);

#ifdef __LIS_INTERNAL__
extern void _RP lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr,
						 size_t len, int direction);
#endif
extern void _RP lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev,
							 dma64_addr_t dma_addr, size_t len,
							 int direction);
extern void _RP lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev *pdev,
							    dma64_addr_t dma_addr, size_t len,
							    int direction);
#endif				/* S390 or S390X */
#endif				/* 2.4.13 */

#endif				/* LINUX_VERSION_CODE >= 0x020400 */

/*
 * IRQ routines
 */
#if defined(_LINUX_PTRACE_H)
#define	OSIF_REGS_T	struct pt_regs
#else
#define	OSIF_REGS_T	void
#endif

/*
 * Pre 2.6 kernels use a void routine for interrupt handling.  In 2.6
 * it is an int routine.  For portability in LiS we will always use
 * an int handler.  It will be called as a void in earlier kernels.
 * This should allow you do move your driver back and forth between
 * 2.4 and 2.6 without recompilation.
 *
 * If you need to, use one of the typedefs here to cast your routine
 * pointer to the proper type to keep the compiler from squawking.
 */
typedef int (*lis_int_handler) (int, void *, OSIF_REGS_T *);
typedef void (*lis_void_handler) (int, void *, OSIF_REGS_T *);

/*
 * In 2.6 the kernel wants the irq handler to return 1 if it handled the
 * interrupt and 0 if not.  LiS will provide a pair of variables that
 * hold these values for portability.  The externs are here and the
 * variables are in osif.c.  The variables will exist even on a 2.4
 * kernel so your handler can be portable.
 */
extern int lis_irqreturn_handled;
extern int lis_irqreturn_not_handled;

int _RP lis_request_irq(unsigned int irq, lis_int_handler handler, unsigned long flags,
			const char *device, void *dev_id);
void _RP lis_free_irq(unsigned int irq, void *dev_id);
void _RP lis_disable_irq(unsigned int irq);
void _RP lis_enable_irq(unsigned int irq);
void _RP lis_osif_cli(void);
void _RP lis_osif_sti(void);

#if defined(_SPARC_LIS_) || defined(_SPARC64_LIS_)

/*
 * On the sparc we have the whole physical IO address space mapped at all
 * times, so ioremap() and ioremap_nocache() do not need to do anything.
 */
#undef ioremap
#undef ioremap_nocache
#undef iounmap

extern __inline__ void *
sparc_ioremap(unsigned long offset, unsigned long size)
{
	return __va(offset);
}

#define	ioremap				sparc_ioremap
#define ioremap_nocache(offset, size)	sparc_ioremap((offset), (size))
#define iounmap(ptr)		/* nothing to do */

#endif

/*
 * Memory mapping routines
 */
void *_RP lis_ioremap(unsigned long offset, unsigned long size);
void *_RP lis_ioremap_nocache(unsigned long offset, unsigned long size);
void _RP lis_iounmap(void *addr);
void *_RP lis_vremap(unsigned long offset, unsigned long size);
unsigned long _RP lis_virt_to_phys(volatile void *addr);
void *_RP lis_phys_to_virt(unsigned long addr);

/*
 * I/O port routines <linux/ioport.h>
 */
#ifdef __LIS_INTERNAL__
int lis_check_region(unsigned int from, unsigned int extent);
#endif
void _RP lis_request_region(unsigned int from, unsigned int extent, const char *name);
void _RP lis_release_region(unsigned int from, unsigned int extent);

#ifdef __LIS_INTERNAL__
int lis_check_mem_region(unsigned int from, unsigned int extent);
void lis_request_mem_region(unsigned int from, unsigned int extent, const char *name);
void lis_release_mem_region(unsigned int from, unsigned int extent);
#endif

/*
 * Memory allocator <linux/malloc.h>
 */
void *_RP lis_kmalloc(size_t nbytes, int type);
void _RP lis_kfree(const void *ptr);
void *_RP lis_vmalloc(unsigned long size);
void _RP lis_vfree(void *ptr);

/*
 * DMA routines <asm/dma.h>
 */
int _RP lis_request_dma(unsigned int dma_nr, const char *device_id);
void _RP lis_free_dma(unsigned int dma_nr);

/*
 * Delay routine in <linux/delay.h> and <asm/delay.h>
 */
void _RP lis_udelay(long micro_secs);
unsigned long _RP lis_jiffies(void);

/*
 * Printing routines.
 */
int _RP lis_printk(const char *fmt, ...) __attribute__ ((__format__(__printf__, 1, 2)));
int _RP lis_sprintf(char *bfr, const char *fmt, ...) __attribute__ ((__format__(__printf__, 2, 3)));
int _RP lis_vsprintf(char *bfr, const char *fmt, va_list args)
    __attribute__ ((__format__(__printf__, 2, 0)));

#ifdef __LIS_INTERNAL__
/*
 * Timer routines.
 */
void lis_add_timer(struct timer_list *timer);
int lis_del_timer(struct timer_list *timer);
#endif

/*
 * Time routines in <linux/time.h>
 */
void _RP lis_osif_do_gettimeofday(struct timeval *tp);
void _RP lis_osif_do_settimeofday(struct timeval *tp);

/*
 * Sleep/wakeup routines
 *
 * Note: It it only legitimate to use these in open and close
 * routines in STREAMS.  DO NOT sleep in put or service routines.
 */
#define	OSIF_WAIT_Q_ARG		wait_queue_head_t *wq
#define	OSIF_WAIT_E_ARG		wait_queue_head_t  wq
#ifdef HAVE_KFUNC_SLEEP_ON
#ifdef __LIS_INTERNAL__
void _RP lis_sleep_on(OSIF_WAIT_Q_ARG);
#endif
#endif
#ifdef HAVE_KFUNC_INTERRUPTIBLE_SLEEP_ON
#ifdef __LIS_INTERNAL__
void _RP lis_interruptible_sleep_on(OSIF_WAIT_Q_ARG);
#endif
#endif
#ifdef HAVE_KFUNC_SLEEP_ON_TIMEOUT
#ifdef __LIS_INTERNAL__
void _RP lis_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout);
#endif
#endif
void _RP lis_interruptible_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout);
void _RP lis_wait_event(OSIF_WAIT_E_ARG, int condition);
void _RP lis_wait_event_interruptible(OSIF_WAIT_E_ARG, int condition);
void _RP lis_wake_up(OSIF_WAIT_Q_ARG);
void _RP lis_wake_up_interruptible(OSIF_WAIT_Q_ARG);

/*
 * Wrapped functions.
 *
 * These are some common functions that drivers might use and want to wrap
 * so that LiS can change the parameter passing convention prior to calling
 * the kernel's version of the function.
 */
extern char *_RP __wrap_strcpy(char *, const char *);
extern char *_RP __wrap_strncpy(char *, const char *, __kernel_size_t);
extern char *_RP __wrap_strcat(char *, const char *);
extern char *_RP __wrap_strncat(char *, const char *, __kernel_size_t);
extern int _RP __wrap_strcmp(const char *, const char *);
extern int _RP __wrap_strncmp(const char *, const char *, __kernel_size_t);
extern int _RP __wrap_strnicmp(const char *, const char *, __kernel_size_t);
extern char *_RP __wrap_strchr(const char *, int);
extern char *_RP __wrap_strrchr(const char *, int);
extern char *_RP __wrap_strstr(const char *, const char *);
extern __kernel_size_t _RP __wrap_strlen(const char *);
extern void *_RP __wrap_memset(void *, int, __kernel_size_t);
extern void *_RP __wrap_memcpy(void *, const void *, __kernel_size_t);
extern int _RP __wrap_memcmp(const void *, const void *, __kernel_size_t);

#ifdef __LIS_INTERNAL__
extern int __wrap_sprintf(char *, const char *, ...);
extern int __wrap_snprintf(char *, size_t, const char *, ...);
extern int __wrap_vsprintf(char *, const char *, va_list);
extern int __wrap_vsnprintf(char *, size_t, const char *, va_list);
#endif

#endif				/* from top of file */
