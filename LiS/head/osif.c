/*****************************************************************************

 @(#) $RCSfile: osif.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/07/13 12:01:18 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/13 12:01:18 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: osif.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/07/13 12:01:18 $"

/************************************************************************
*                   Operating System Interface                          *
*************************************************************************
*									*
* The routines in this file provide a filtered interface to certain	*
* operating system routines.  These are routines that drivers are	*
* most likely to use such as PCI BIOS calls and mapping addresses.	*
*									*
* This code loads as a loadable module, but it has no open routine.	*
*									*
* Additions to this file are welcome.					*
*									*
*	Copyright (c) 1999 David Grothe <dave@gcom.com>			*
*									*
************************************************************************/

#include <sys/stream.h>

#ifdef STR
#undef STR			/* collides with irq.h */
#endif

# ifdef RH_71_KLUDGE		/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM		/* b_page has semi-circular reference */
# endif
#include <linux/pci.h>		/* PCI BIOS functions */
#include <linux/sched.h>	/* odd place for request_irq */
#include <linux/ioport.h>	/* request_region */
#include <asm/dma.h>
#include <linux/slab.h>
#include <linux/timer.h>	/* add_timer, del_timer */
#include <linux/ptrace.h>	/* for pt_regs */
#if LINUX_VERSION_CODE >= 0x020100	/* 2.2 kernel */
#include <linux/vmalloc.h>
#endif
#include <asm/io.h>		/* ioremap, virt_to_phys */
#include <asm/irq.h>		/* disable_irq, enable_irq */
#include <asm/atomic.h>		/* the real kernel routines */
#include <linux/delay.h>
#include <linux/time.h>
#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
#include <linux/bios32.h>	/* old style PCI routines */
#include <linux/mm.h>		/* vremap */
#endif
#include <stdarg.h>

#define	INCL_FROM_OSIF_DRIVER	/* do not change routine names */
#include <sys/osif.h>

/************************************************************************
*                       PCI BIOS Functions                              *
*************************************************************************
*									*
* These are filtered calls to the kernel's PCI BIOS routines.		*
*									*
************************************************************************/

/*
 * Determine which systems have this old PCI BIOS stuff.  LiS provides
 * routines anyway, but they all return errors.
 */
#if 	defined(_SPARC_LIS_) || defined(_PPC_LIS_) || \
	defined(KERNEL_2_5) || defined(__s390__)
#undef HAS_PCIBIOS
#else
#define HAS_PCIBIOS	1
#endif

int
lis_pcibios_present(void)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_present());
#else
	return (-ENOSYS);
#endif
}

/*
 * PCO BIOS init routine is not an exported symbol, but we define it
 * anyway in case some driver was calling it.
 */
void
lis_pcibios_init(void)
{
}

int
lis_pcibios_find_class(unsigned int class_code, unsigned short index, unsigned char *bus,
		       unsigned char *dev_fn)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_find_class(class_code, index, bus, dev_fn));
#else
	return (-ENXIO);
#endif
}

int
lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id, unsigned short index,
			unsigned char *bus, unsigned char *dev_fn)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_find_device(vendor, dev_id, index, bus, dev_fn));
#else
	return (-ENXIO);
#endif
}

int
lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
			     unsigned char *val)
{
#if defined(HAS_PCIBIOS)
	if (where != PCI_INTERRUPT_LINE)
		return pcibios_read_config_byte(bus, dev_fn, where, val);
	else {
		struct pci_dev *dev;

		if ((dev = pci_find_slot(bus, dev_fn)) != NULL) {
			*val = dev->irq;
			return PCIBIOS_SUCCESSFUL;
		}
	}
	*val = 0;
	return PCIBIOS_DEVICE_NOT_FOUND;
#else
	return (-ENOSYS);
#endif
}

int
lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
			     unsigned short *val)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_read_config_word(bus, dev_fn, where, val));
#else
	return (-ENOSYS);
#endif
}

int
lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned int *val)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_read_config_dword(bus, dev_fn, where, val));
#else
	return (-ENOSYS);
#endif
}

int
lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned char val)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_write_config_byte(bus, dev_fn, where, val));
#else
	return (-ENOSYS);
#endif
}

int
lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned short val)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_write_config_word(bus, dev_fn, where, val));
#else
	return (-ENOSYS);
#endif
}

int
lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
			       unsigned int val)
{
#if defined(HAS_PCIBIOS)
	return (pcibios_write_config_dword(bus, dev_fn, where, val));
#else
	return (-ENOSYS);
#endif
}

const char *
lis_pcibios_strerror(int error)
{
#if defined(HAS_PCIBIOS)
	switch (error) {
	case PCIBIOS_SUCCESSFUL:
		return ("PCIBIOS_SUCCESSFUL");
	case PCIBIOS_FUNC_NOT_SUPPORTED:
		return ("PCIBIOS_FUNC_NOT_SUPPORTED");
	case PCIBIOS_BAD_VENDOR_ID:
		return ("PCIBIOS_BAD_VENDOR_ID");
	case PCIBIOS_DEVICE_NOT_FOUND:
		return ("PCIBIOS_DEVICE_NOT_FOUND");
	case PCIBIOS_BAD_REGISTER_NUMBER:
		return ("PCIBIOS_BAD_REGISTER_NUMBER");
	case PCIBIOS_SET_FAILED:
		return ("PCIBIOS_SET_FAILED");
	case PCIBIOS_BUFFER_TOO_SMALL:
		return ("PCIBIOS_BUFFER_TOO_SMALL");
	}

	{
		static char msg[100];	/* not very re-entrant */

		sprintf(msg, "PCIBIOS Error #%d", error);
		return (msg);
	}
#else
	return ("No PCI BIOS functions");
#endif
}

/*
 * End of old PCI BIOS routines
 */

struct pci_dev *
lis_osif_pci_find_device(unsigned int vendor, unsigned int device, struct pci_dev *from)
{
	return (pci_find_device(vendor, device, from));
}

#ifdef HAVE_KFUNC_PCI_FIND_CLASS
struct pci_dev *
lis_osif_pci_find_class(unsigned int class, struct pci_dev *from)
{
	return (pci_find_class(class, from));
}
#endif

struct pci_dev *
lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn)
{
	return (pci_find_slot(bus, devfn));
}

int
lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val)
{
	return (pci_read_config_byte(dev, where, val));
}

int
lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	return (pci_read_config_word(dev, where, val));
}

int
lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	return (pci_read_config_dword(dev, where, val));
}

int
lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	return (pci_write_config_byte(dev, where, val));
}

int
lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	return (pci_write_config_word(dev, where, val));
}

int
lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	return (pci_write_config_dword(dev, where, val));
}

void
lis_osif_pci_set_master(struct pci_dev *dev)
{
	pci_set_master(dev);
}

int
lis_osif_pci_enable_device(struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < 0x020300	/* 2.0 or 2.2 kernel */
	return (0);		/* pretend success */
#else
	return (pci_enable_device(dev));
#endif
}

void
lis_osif_pci_disable_device(struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < 0x020300	/* 2.0 or 2.2 kernel */
	return;			/* pretend success */
#else
	pci_disable_device(dev);
#endif
}

#if LINUX_VERSION_CODE < 0x020300	/* 2.0 or 2.2 kernel */
int
lis_osif_pci_module_init(void *p)
{
	return (0);
}

void
lis_osif_pci_unregister_driver(void *p)
{
}
#else				/* 2.4 kernel */
int
lis_osif_pci_module_init(struct pci_driver *p)
{
	return pci_module_init(p);
}

void
lis_osif_pci_unregister_driver(struct pci_driver *p)
{
	pci_unregister_driver(p);
}
#endif

#if LINUX_VERSION_CODE >= 0x020400	/* 2.4 kernel */
#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_))

	 /***************************************
	 *        PCI DMA Mapping Fcns		*
	 ***************************************/

void *
lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size, dma_addr_t *dma_handle)
{
#ifdef CONFIG_PCI
	return (pci_alloc_consistent(hwdev, size, dma_handle));
#else
	return 0;
#endif
}

void
lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle)
{
#ifdef CONFIG_PCI
	pci_free_consistent(hwdev, size, vaddr, dma_handle);
#else
	return;
#endif
}

dma_addr_t
lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size, int direction)
{
	return (pci_map_single(hwdev, ptr, size, direction));
}

void
lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size, int direction)
{
	pci_unmap_single(hwdev, dma_addr, size, direction);
}

int
lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	return (pci_map_sg(hwdev, sg, nents, direction));
}

void
lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	pci_unmap_sg(hwdev, sg, nents, direction);
}

#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
void
lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size,
			     int direction)
{
	pci_dma_sync_single(hwdev, dma_handle, size, direction);
}
#endif

#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SG
void
lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems, int direction)
{
	pci_dma_sync_sg(hwdev, sg, nelems, direction);
}
#endif

int
lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return (pci_dma_supported(hwdev, mask));
}

int
lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return (pci_set_dma_mask(hwdev, mask));
}

dma_addr_t
lis_osif_sg_dma_address(struct scatterlist *sg)
{
	return (sg_dma_address(sg));
}

size_t
lis_osif_sg_dma_len(struct scatterlist *sg)
{
	return (sg_dma_len(sg));
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)	/* 2.4.13 or later */

void
lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size, int direction)
{
	pci_unmap_page(hwdev, dma_address, size, direction);
}

#ifdef HAVE_KFUNC_PCI_DAC_SET_DMA_MASK
int
lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return (pci_dac_set_dma_mask(hwdev, mask));
}
#endif

dma_addr_t
lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page, unsigned long offset, size_t size,
		      int direction)
{
	return (pci_map_page(hwdev, page, offset, size, direction));
}

int
lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return (pci_dac_dma_supported(hwdev, mask));
}

dma64_addr_t
lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page, unsigned long offset,
			     int direction)
{
	return (pci_dac_page_to_dma(pdev, page, offset, direction));
}

struct page *
lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return (pci_dac_dma_to_page(pdev, dma_addr));
}

unsigned long
lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return (pci_dac_dma_to_offset(pdev, dma_addr));
}

#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
void
lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr, size_t len,
				 int direction)
{
	pci_dac_dma_sync_single(pdev, dma_addr, len, direction);
}
#endif

#if HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_CPU
void
lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev, dma64_addr_t dma_addr, size_t len,
					 int direction)
{
	pci_dac_dma_sync_single_for_cpu(pdev, dma_addr, len, direction);
}
#endif

#if HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_DEVICE
void
lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev *pdev, dma64_addr_t dma_addr, size_t len,
					    int direction)
{
	pci_dac_dma_sync_single_for_device(pdev, dma_addr, len, direction);
}
#endif

#endif				/* 2.4.13 */

#endif				/* S390 or S390X */

#endif				/* LINUX_VERSION_CODE >= 0x020400 */

/************************************************************************
*                        IRQ Routines                                   *
************************************************************************/

#if defined (KERNEL_2_5)
/*
 * Because of the register parameter passing in the 2.6 kernel we need
 * an irq routine that is compiled with the kernel's register style to
 * pass to the kernel's request_irq function.  This routine then calls
 * the LiS style irq handler using the LiS parameter passing convention.
 *
 * So we construct a "deice id" as the pointer to one of these structures
 * in which we save the user's original dev_id parameter which we then
 * pass back to the user's interrupt handler.  We are going to appropriate
 * the lis_incr_lock spin lock for our list protection.
 *
 * We make a list of these structures that can be cleaned up at LiS
 * termination time.
 */
typedef struct lis_devid {
	struct lis_devid *link;
	lis_int_handler handler;
	void *dev_id;			/* user's dev_id param */
	int irq;
} lis_devid_t;

extern lis_spin_lock_t lis_incr_lock;

lis_devid_t *lis_devid_list;

/*
 * This routine is compiled with the kernel's parameter passing convention.
 * It calls the STREAMS driver's handler using LiS parameter passing
 * convention.
 */
static irqreturn_t
lis_khandler(int irq, void *dev_id, struct pt_regs *regs)
{
	lis_devid_t *dv = (lis_devid_t *) dev_id;

	return (dv->handler(irq, dv->dev_id, regs));
}

/*
 * Called at LiS termination time to clean up the list
 */
void
lis_free_devid_list(void)
{
	lis_devid_t *dv = lis_devid_list;
	lis_devid_t *nxt;

	lis_devid_list = NULL;

	while (dv != NULL) {
		nxt = dv->link;
		if (dv->handler && dv->dev_id && dv->irq) {
			printk("LiS freeing IRQ%u\n", dv->irq);
			free_irq(dv->irq, dv);
		}
		FREE(dv);
		dv = nxt;
	}
}

#endif

#if defined (KERNEL_2_5)

int lis_irqreturn_handled = IRQ_HANDLED;
int lis_irqreturn_not_handled = IRQ_NONE;

#else

int lis_irqreturn_handled = 1;
int lis_irqreturn_not_handled = 0;

#endif

int
lis_request_irq(unsigned int irq, lis_int_handler handler, unsigned long flags, const char *device,
		void *dev_id)
{
#if !defined(__s390__)
#if defined (KERNEL_2_5)
	int ret;
	lis_devid_t *dv;
	lis_flags_t psw;

	dv = ALLOC(sizeof(*dv));
	if (dv == NULL)
		return (-ENOMEM);

	dv->handler = handler;
	dv->dev_id = dev_id;
	dv->irq = irq;

	ret = request_irq(irq, lis_khandler, flags, device, dv);
	if (ret == 0) {
		lis_spin_lock_irqsave(&lis_incr_lock, &psw);
		dv->link = lis_devid_list;
		lis_devid_list = dv;
		lis_spin_unlock_irqrestore(&lis_incr_lock, &psw);
	} else
		FREE(dv);

	return (ret);
#else
	typedef void (*khandler) (int, void *, struct pt_regs *);
	khandler kparam = (khandler) handler;

	return (request_irq(irq, kparam, flags, device, dev_id));
#endif
#endif
}

void
lis_free_irq(unsigned int irq, void *dev_id)
{
#if !defined(__s390__)
#if defined (KERNEL_2_5)
	lis_devid_t *dv = lis_devid_list;
	lis_flags_t psw;

	lis_spin_lock_irqsave(&lis_incr_lock, &psw);
	for (; dv != NULL; dv = dv->link) {
		if (dv->dev_id == dev_id) {
			dv->handler = NULL;
			dv->dev_id = NULL;
			dv->irq = 0;
			lis_spin_unlock_irqrestore(&lis_incr_lock, &psw);
			free_irq(irq, dv);
			return;
		}
	}
	lis_spin_unlock_irqrestore(&lis_incr_lock, &psw);
#else
	free_irq(irq, dev_id);
#endif
#endif
}

void
lis_enable_irq(unsigned int irq)
{
#if !defined(__s390__)
	enable_irq(irq);
#endif
}

void
lis_disable_irq(unsigned int irq)
{
#if !defined(__s390__)
	disable_irq(irq);
#endif
}

void
lis_osif_cli(void)
{
#if defined(KERNEL_2_5)
	lis_splstr();
#else
	cli();
#endif

}

void
lis_osif_sti(void)
{
#if defined(KERNEL_2_5)
	lis_spl0();
#else
	sti();
#endif
}

/************************************************************************
*                       Memory Mapping Routines                         *
*************************************************************************
*									*
* These are a subset of what is available.  Feel free to add more.	*
*									*
* The 2.0.x routines will call 2.2.x routines if we are being compiled	*
* for 2.2.x, and vice versa.  This gives drivers something in the way	*
* of version independence.  Wish the kernel guys would tend to that	*
* a bit more.								*
*									*
************************************************************************/

void *
lis_ioremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
	return (vremap(offset, size));
#elif !defined(__s390__)
	return (ioremap(offset, size));
#else
	return (NULL);
#endif
}

void *
lis_ioremap_nocache(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
	return (vremap(offset, size));
#elif !defined(__s390__)
	return (ioremap_nocache(offset, size));
#else
	return (NULL);
#endif
}

void
lis_iounmap(void *ptr)
{
#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
	vfree(ptr);
#elif !defined(__s390__)
	iounmap(ptr);
#endif
}

void *
lis_vremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100	/* 2.0 kernel */
	return (vremap(offset, size));
#elif !defined(__s390__)
	return (ioremap_nocache(offset, size));
#else
	return (NULL);
#endif
}

unsigned long
lis_virt_to_phys(volatile void *addr)
{
	return (virt_to_phys(addr));
}

void *
lis_phys_to_virt(unsigned long addr)
{
	return (phys_to_virt(addr));
}

/************************************************************************
*                       I/O Ports Routines                              *
************************************************************************/

int
lis_check_region(unsigned int from, unsigned int extent)
{
#if defined(KERNEL_2_5)
	if (request_region(from, extent, "LiS-checking")) {
		release_region(from, extent);
		return (0);
	}
	return (-EBUSY);
#else
	return (check_region(from, extent));
#endif
}

void
lis_request_region(unsigned int from, unsigned int extent, const char *name)
{
	request_region(from, extent, name);
}

void
lis_release_region(unsigned int from, unsigned int extent)
{
	release_region(from, extent);
}

int
lis_check_mem_region(unsigned int from, unsigned int extent)
{
	return (check_mem_region(from, extent));
}

void
lis_request_mem_region(unsigned int from, unsigned int extent, const char *name)
{
	request_mem_region(from, extent, name);
}

void
lis_release_mem_region(unsigned int from, unsigned int extent)
{
	release_mem_region(from, extent);
}

/************************************************************************
*                    Memory Allocation Routines                         *
************************************************************************/

void *
lis_kmalloc(size_t nbytes, int type)
{
	return (kmalloc(nbytes, type));
}

void
lis_kfree(const void *ptr)
{
	kfree((void *) ptr);
}

void *
lis_vmalloc(unsigned long size)
{
	return (vmalloc(size));
}

void
lis_vfree(void *ptr)
{
	vfree(ptr);
}

/************************************************************************
*                        DMA Routines                                   *
************************************************************************/

int
lis_request_dma(unsigned int dma_nr, const char *device_id)
{
#if defined(MAX_DMA_CHANNELS)
	return (request_dma(dma_nr, device_id));
#else
	return (-ENXIO);
#endif
}

void
lis_free_dma(unsigned int dma_nr)
{
#if defined(MAX_DMA_CHANNELS)
	free_dma(dma_nr);
#endif
}

/************************************************************************
*                         Delay Routines                                *
************************************************************************/

void
lis_udelay(long micro_secs)
{
	udelay(micro_secs);
}

unsigned long
lis_jiffies(void)
{
	return (jiffies);
}

/************************************************************************
*                         Time Routines                                 *
************************************************************************/
void
lis_osif_do_gettimeofday(struct timeval *tp)
{
	do_gettimeofday(tp);
}

void
lis_osif_do_settimeofday(struct timeval *tp)
{
#if defined(KERNEL_2_5)		/* yes, of course we have to change a standard system routine
				   interface call... */
	struct timespec ts;

	ts.tv_sec = tp->tv_sec;
	ts.tv_nsec = tp->tv_usec * 1000;
	do_settimeofday(&ts);

#elif LINUX_VERSION_CODE >= 0x020300	/* 2.4 kernel */

	do_settimeofday(tp);

#endif				/* not in earlier kernels */
}

/************************************************************************
*                         Printing Routines                             *
************************************************************************/
__attribute__ ((format(printf, 1, 2)))
	int lis_printk(const char *fmt, ...)
{
	extern char lis_cmn_err_buf[];
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vsprintf(lis_cmn_err_buf, fmt, args);
	va_end(args);

	printk("%s", lis_cmn_err_buf);
	return (ret);
}

__attribute__ ((format(printf, 2, 3)))
	int lis_sprintf(char *bfr, const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vsprintf(bfr, fmt, args);
	va_end(args);

	return (ret);
}

int
lis_vsprintf(char *bfr, const char *fmt, va_list args)
{
	return (vsprintf(bfr, fmt, args));
}

/************************************************************************
*                      Sleep/Wakeup Routines                            *
************************************************************************/
#ifdef HAVE_KFUNC_SLEEP_ON
void
lis_sleep_on(OSIF_WAIT_Q_ARG)
{
	sleep_on(wq);
}
#endif

#ifdef HAVE_KFUNC_INTERRUPTIBLE_SLEEP_ON
void
lis_interruptible_sleep_on(OSIF_WAIT_Q_ARG)
{
	interruptible_sleep_on(wq);
}
#endif

#ifdef HAVE_KFUNC_SLEEP_ON_TIMEOUT
void
lis_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout)
{
	sleep_on_timeout(wq, timeout);
}
#endif

void
lis_interruptible_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout)
{
	interruptible_sleep_on_timeout(wq, timeout);
}

void
lis_wait_event(OSIF_WAIT_E_ARG, int condition)
{
	wait_event(wq, condition);
}

void
lis_wait_event_interruptible(OSIF_WAIT_E_ARG, int condition)
{
	wait_event_interruptible(wq, condition);
}

void
lis_wake_up(OSIF_WAIT_Q_ARG)
{
	wake_up(wq);
}

void
lis_wake_up_interruptible(OSIF_WAIT_Q_ARG)
{
	wake_up_interruptible(wq);
}

/************************************************************************
*                             Timer Routines                            *
************************************************************************/
void
lis_add_timer(struct timer_list *timer)
{
	add_timer(timer);
}

int
lis_del_timer(struct timer_list *timer)
{
	return del_timer(timer);
}

/*
 * Prototype in dki.h (with other timer stuff), not osif.h
 */
unsigned
lis_usectohz(unsigned usec)
{
	if (((1000 / HZ) * HZ) == 1000)	/* ie: HZ == 100 */
		return (usec / (1000000 / HZ));
	else			/* ie: HZ == 1024 */
		return ((usec * HZ) / 1000000);	/* 32bits overflow ??? */
}

/************************************************************************
*                        Wrapped Functions                              *
************************************************************************/

#define __real_strcpy	strcpy
#define __real_strncpy	strncpy
#define __real_strcat	strcat
#define __real_strncat	strncat
#define __real_strcmp	strcmp
#define __real_strncmp	strncmp
#define __real_strnicmp	strnicmp
#define __real_strchr	strchr
#define __real_strrchr	strrchr
#define __real_strstr	strstr
#define __real_strlen	strlen
#define __real_memset	memset
#define __real_memcpy	memcpy
#define __real_memcmp	memcmp

char *
__wrap_strcpy(char *d, const char *s)
{
	return (__real_strcpy(d, s));
}

char *
__wrap_strncpy(char *d, const char *s, __kernel_size_t l)
{
	return (__real_strncpy(d, s, l));
}

char *
__wrap_strcat(char *d, const char *s)
{
	return (__real_strcat(d, s));
}

char *
__wrap_strncat(char *d, const char *s, __kernel_size_t l)
{
	return (__real_strncat(d, s, l));
}

int
__wrap_strcmp(const char *a, const char *b)
{
	return (__real_strcmp(a, b));
}

int
__wrap_strncmp(const char *a, const char *b, __kernel_size_t l)
{
	return (__real_strncmp(a, b, l));
}

int
__wrap_strnicmp(const char *a, const char *b, __kernel_size_t l)
{
	return (__real_strnicmp(a, b, l));
}

char *
__wrap_strchr(const char *s, int c)
{
	return (__real_strchr(s, c));
}

char *
__wrap_strrchr(const char *s, int c)
{
	return (__real_strrchr(s, c));
}

char *
__wrap_strstr(const char *a, const char *b)
{
	return (__real_strstr(a, b));
}

__kernel_size_t
__wrap_strlen(const char *s)
{
	return (__real_strlen(s));
}

void *
__wrap_memset(void *d, int v, __kernel_size_t l)
{
	return (__real_memset(d, v, l));
}

void *
__wrap_memcpy(void *d, const void *s, __kernel_size_t l)
{
	return (__real_memcpy(d, s, l));
}

int
__wrap_memcmp(const void *a, const void *b, __kernel_size_t l)
{
	return (__real_memcmp(a, b, l));
}

int
__wrap_sprintf(char *p, const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vsprintf(p, fmt, args);
	va_end(args);
	return (ret);
}

int
__wrap_snprintf(char *p, size_t len, const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vsnprintf(p, len, fmt, args);
	va_end(args);
	return (ret);
}

int
__wrap_vsprintf(char *p, const char *fmt, va_list args)
{
	return (vsprintf(p, fmt, args));
}

int
__wrap_vsnprintf(char *p, size_t len, const char *fmt, va_list args)
{
	return (vsnprintf(p, len, fmt, args));
}
