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
* This library is free software; you can redistribute it and/or		*
* modify it under the terms of the GNU Library General Public		*
* License as published by the Free Software Foundation; either		*
* version 2 of the License, or (at your option) any later version.	*
* 									*
* This library is distributed in the hope that it will be useful,	*
* but WITHOUT ANY WARRANTY; without even the implied warranty of	*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*
* Library General Public License for more details.			*
*								 	*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the	Free Software	*
* Foundation, Inc., 59 Temple Place - Suite 330, Cambridge, MA 02139 USA*
*									*
************************************************************************/

#ident "@(#) LiS osif.c 1.41 12/18/02"

#include <sys/stream.h>
#include <linux/autoconf.h>		/* Linux config defines */

#ifdef STR
#undef STR				/* collides with irq.h */
#endif

# ifdef RH_71_KLUDGE			/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM			/* b_page has semi-circular reference */
# endif
#include <linux/pci.h>		/* PCI BIOS functions */
#include <linux/sched.h>		/* odd place for request_irq */
#include <linux/ioport.h>		/* request_region */
#include <asm/dma.h>
#include <linux/slab.h>
#include <linux/timer.h>                /* add_timer, del_timer */
#include <linux/ptrace.h>		/* for pt_regs */
#if LINUX_VERSION_CODE >= 0x020100      /* 2.2 kernel */
#include <linux/vmalloc.h>
#endif
#include <asm/io.h>			/* ioremap, virt_to_phys */
#include <asm/irq.h>			/* disable_irq, enable_irq */
#include <asm/atomic.h>			/* the real kernel routines */
#include <linux/delay.h>
#include <linux/time.h>
#if LINUX_VERSION_CODE < 0x020100       /* 2.0 kernel */
#include <linux/bios32.h>		/* old style PCI routines */
#include <linux/mm.h>			/* vremap */
#endif
#include <stdarg.h>

#define	INCL_FROM_OSIF_DRIVER		/* do not change routine names */
#include <sys/osif.h>




/************************************************************************
*                       PCI BIOS Functions                              *
*************************************************************************
*									*
* These are filtered calls to the kernel's PCI BIOS routines.		*
*									*
************************************************************************/

#if (!defined(_SPARC_LIS_) && !defined(_PPC_LIS_))

#if defined(KERNEL_2_5)		/* pcibios_xxx functions de-implemented */

static int pcibios_present(void)
{
    return(-ENOSYS) ;
}
static int pcibios_find_class(unsigned int   class_code,
			    unsigned short index,
                            unsigned char *bus,
			    unsigned char *dev_fn)
{
    return(-ENOSYS) ;
}
static int pcibios_find_device(unsigned short vendor,
			     unsigned short dev_id,
			     unsigned short index,
			     unsigned char *bus,
			     unsigned char *dev_fn)
{
    return(-ENOSYS) ;
}

#endif

int lis_pcibios_present(void)
{
    return(pcibios_present()) ;
}

/*
 * PCO BIOS init routine is not an exported symbol, but we define it
 * anyway in case some driver was calling it.
 */
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
unsigned long lis_pcibios_init(unsigned long memory_start,
			        unsigned long memory_end)
{
#if defined(CONFIG_PCI)
    return(1) ;			/* assume if configured then init'd by now */
#else
    return(0) ;			/* not configured, not initialized */
#endif
}
#else                                           /* 2.1 or 2.2 kernel */
void lis_pcibios_init(void)
{
}
#endif

int lis_pcibios_find_class(unsigned int   class_code,
			    unsigned short index,
                            unsigned char *bus,
			    unsigned char *dev_fn)
{
#if !defined(__s390__)
    return(pcibios_find_class(class_code, index, bus, dev_fn)) ;
#else
    return(-ENXIO) ;
#endif
}

int lis_pcibios_find_device(unsigned short vendor,
			     unsigned short dev_id,
			     unsigned short index,
			     unsigned char *bus,
			     unsigned char *dev_fn)
{
#if !defined(__s390__)
    return(pcibios_find_device(vendor, dev_id, index, bus, dev_fn)) ;
#else
    return(-ENXIO) ;
#endif
}

int lis_pcibios_read_config_byte(unsigned char  bus,
				  unsigned char  dev_fn,
				  unsigned char  where,
				  unsigned char *val)
{
#if LINUX_VERSION_CODE < 0x020100       /* 2.0 kernel */
    return pcibios_read_config_byte(bus, dev_fn, where, val);
#else					/* 2.1/2.2 kernel */
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
#endif
}

int lis_pcibios_read_config_word(unsigned char   bus,
				  unsigned char   dev_fn,
				  unsigned char   where,
				  unsigned short *val)
{
    return(pcibios_read_config_word(bus, dev_fn, where, val)) ;
}

int lis_pcibios_read_config_dword(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned int  *val)
{
    return(pcibios_read_config_dword(bus, dev_fn, where, val)) ;
}

int lis_pcibios_write_config_byte(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned char  val)
{
    return(pcibios_write_config_byte(bus, dev_fn, where, val)) ;
}

int lis_pcibios_write_config_word(unsigned char   bus,
				   unsigned char   dev_fn,
				   unsigned char   where,
				   unsigned short  val)
{
    return(pcibios_write_config_word(bus, dev_fn, where, val)) ;
}

int lis_pcibios_write_config_dword(unsigned char  bus,
				    unsigned char  dev_fn,
				    unsigned char  where,
				    unsigned int   val)
{
    return(pcibios_write_config_dword(bus, dev_fn, where, val)) ;
}

const char *lis_pcibios_strerror(int error)
{
#if LINUX_VERSION_CODE < 0x020100       /* 2.0 kernel */
    return(pcibios_strerror(error)) ;
#else					/* 2.1 and beyond */
    switch (error)
    {
    case PCIBIOS_SUCCESSFUL:		return("PCIBIOS_SUCCESSFUL") ;
    case PCIBIOS_FUNC_NOT_SUPPORTED:	return("PCIBIOS_FUNC_NOT_SUPPORTED") ;
    case PCIBIOS_BAD_VENDOR_ID:		return("PCIBIOS_BAD_VENDOR_ID") ;
    case PCIBIOS_DEVICE_NOT_FOUND:	return("PCIBIOS_DEVICE_NOT_FOUND") ;
    case PCIBIOS_BAD_REGISTER_NUMBER:	return("PCIBIOS_BAD_REGISTER_NUMBER") ;
    case PCIBIOS_SET_FAILED:		return("PCIBIOS_SET_FAILED") ;
    case PCIBIOS_BUFFER_TOO_SMALL:	return("PCIBIOS_BUFFER_TOO_SMALL") ;
    }

    {
	static char msg[100] ;	/* not very re-entrant */
	sprintf(msg, "PCIBIOS Error #%d", error) ;
	return(msg) ;
    }
#endif
}

#endif /* ifndef _SPARC_LIS_, _PPC_LIS_ */


struct pci_dev  *lis_osif_pci_find_device(unsigned int vendor,
				 unsigned int device,
                                 struct pci_dev *from)
{
    return(pci_find_device(vendor, device, from)) ;
}

struct pci_dev  *lis_osif_pci_find_class(unsigned int class,
					 struct pci_dev *from)
{
    return(pci_find_class(class, from)) ;
}

struct pci_dev  *lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn)
{
    return(pci_find_slot(bus, devfn)) ;
}

int     lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 *val)
{
    return(pci_read_config_byte(dev, where, val)) ;
}

int     lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 *val)
{
    return(pci_read_config_word(dev, where, val)) ;
}

int     lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 *val)
{
    return(pci_read_config_dword(dev, where, val)) ;
}

int     lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
    return(pci_write_config_byte(dev, where, val)) ;
}

int     lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
    return(pci_write_config_word(dev, where, val)) ;
}

int     lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
    return(pci_write_config_dword(dev, where, val)) ;
}

void    lis_osif_pci_set_master(struct pci_dev *dev)
{
    pci_set_master(dev) ;
}

int lis_osif_pci_enable_device (struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < 0x020300               /* 2.0 or 2.2 kernel */
    return(0) ;					/* pretend success */
#else
    return (pci_enable_device (dev));
#endif
}

void lis_osif_pci_disable_device (struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < 0x020300               /* 2.0 or 2.2 kernel */
    return ;					/* pretend success */
#else
    pci_disable_device (dev);
#endif
}

#if LINUX_VERSION_CODE < 0x020300               /* 2.0 or 2.2 kernel */
int lis_osif_pci_module_init( void *p )
{
    return ( 0 );
}

void lis_osif_pci_unregister_driver( void *p )
{
}
#else						/* 2.4 kernel */
int lis_osif_pci_module_init( struct pci_driver *p )
{
    return pci_module_init( p );
}

void lis_osif_pci_unregister_driver( struct pci_driver *p )
{
    pci_unregister_driver( p );
}
#endif

#if LINUX_VERSION_CODE >= 0x020400		/* 2.4 kernel */

	 /***************************************
	 *        PCI DMA Mapping Fcns		*
	 ***************************************/

void *lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
	                                  dma_addr_t *dma_handle)
{
    return(pci_alloc_consistent(hwdev, size, dma_handle));
}

void lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size,
	                                void *vaddr, dma_addr_t dma_handle)
{
    pci_free_consistent(hwdev, size, vaddr, dma_handle);
}

dma_addr_t lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr,
	                                        size_t size, int direction)
{
    return(pci_map_single(hwdev, ptr, size, direction)) ;
}

void lis_osif_pci_unmap_single(struct pci_dev *hwdev,
			    dma_addr_t dma_addr, size_t size, int direction)
{
    pci_unmap_single(hwdev, dma_addr, size, direction) ;
}

int lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
	                             int nents, int direction)
{
    return(pci_map_sg(hwdev, sg, nents, direction)) ;
}

void lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
	                                int nents, int direction)
{
    pci_unmap_sg(hwdev, sg, nents, direction) ;
}

void lis_osif_pci_dma_sync_single(struct pci_dev *hwdev,
			   dma_addr_t dma_handle, size_t size, int direction)
{
    pci_dma_sync_single(hwdev, dma_handle, size, direction) ;
}

void lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev,
			   struct scatterlist *sg, int nelems, int direction)
{
    pci_dma_sync_sg(hwdev, sg, nelems, direction) ;
}

int lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
    return(pci_dma_supported(hwdev, mask)) ;
}

int lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
    return(pci_set_dma_mask(hwdev, mask)) ;
}

dma_addr_t lis_osif_sg_dma_address(struct scatterlist *sg)
{
    return(sg_dma_address(sg)) ;
}

size_t lis_osif_sg_dma_len(struct scatterlist *sg)
{
    return(sg_dma_len(sg)) ;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)      /* 2.4.13 or later */

void lis_osif_pci_unmap_page(struct pci_dev *hwdev,
				dma_addr_t dma_address, size_t size,
				int direction)
{
    pci_unmap_page(hwdev, dma_address, size, direction) ;
}

int lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
    return(pci_dac_set_dma_mask(hwdev, mask)) ;
}

dma_addr_t lis_osif_pci_map_page(struct pci_dev *hwdev,
				struct page *page, unsigned long offset,
				size_t size, int direction)
{
    return(pci_map_page(hwdev, page, offset, size, direction)) ;
}

int lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)
{
    return(pci_dac_dma_supported(hwdev, mask)) ;
}


dma64_addr_t lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev,
			struct page *page, unsigned long offset,
			int direction)
{
    return(pci_dac_page_to_dma(pdev, page, offset, direction)) ;
}

struct page *lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev,
					dma64_addr_t dma_addr)
{
    return(pci_dac_dma_to_page(pdev, dma_addr)) ;
}

unsigned long lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
					dma64_addr_t dma_addr)
{
    return(pci_dac_dma_to_offset(pdev, dma_addr)) ;
}

void lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev,
			    dma64_addr_t dma_addr, size_t len, int direction)
{
    pci_dac_dma_sync_single(pdev, dma_addr, len, direction) ;
}

#endif                                  /* 2.4.13 */


#endif		/* LINUX_VERSION_CODE >= 0x020400 */

/************************************************************************
*                        IRQ Routines                                   *
************************************************************************/

int lis_request_irq(unsigned int  irq,
	             void        (*handler)(int, void *, struct pt_regs *),
	             unsigned long flags,
		     const char   *device,
		     void         *dev_id)
{
    return(request_irq(irq, handler, flags, device, dev_id)) ;
}

void lis_free_irq(unsigned int irq, void *dev_id)
{
    free_irq(irq, dev_id) ;
}

void lis_enable_irq(unsigned int irq)
{
#if !defined(__s390__)
    enable_irq(irq) ;
#endif
}

void lis_disable_irq(unsigned int irq)
{
#if !defined(__s390__)
    disable_irq(irq) ;
#endif
}

void lis_osif_cli( void )
{
#if defined(KERNEL_2_5)
    lis_splstr() ;
#else
    cli( );
#endif

}
void lis_osif_sti( void )
{
#if defined(KERNEL_2_5)
    lis_spl0() ;
#else
    sti( );
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

void *lis_ioremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
    return(ioremap(offset, size)) ;
#else
    return(NULL) ;
#endif
}

void *lis_ioremap_nocache(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
    return(ioremap_nocache(offset, size)) ;
#else
    return(NULL) ;
#endif
}

void lis_iounmap(void *ptr)
{
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
    vfree(ptr) ;
#elif !defined(__s390__)
    iounmap(ptr) ;
#endif
}

void *lis_vremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
    return(ioremap_nocache(offset, size)) ;
#else
    return(NULL) ;
#endif
}

unsigned long lis_virt_to_phys(volatile void *addr)
{
    return(virt_to_phys(addr)) ;
}

void *lis_phys_to_virt(unsigned long addr)
{
    return(phys_to_virt(addr)) ;
}


/************************************************************************
*                       I/O Ports Routines                              *
************************************************************************/

int lis_check_region(unsigned int from, unsigned int extent)
{
    return(check_region(from, extent)) ;
}

void lis_request_region(unsigned int from,
			 unsigned int extent,
			 const char  *name)
{
    request_region(from, extent, name) ;
}

void lis_release_region(unsigned int from, unsigned int extent)
{
    release_region(from, extent) ;
}

/************************************************************************
*                    Memory Allocation Routines                         *
************************************************************************/

void *lis_kmalloc(size_t nbytes, int type)
{
    return(kmalloc(nbytes, type)) ;
}

void  lis_kfree(const void *ptr)
{
    kfree((void *)ptr) ;
}

void *lis_vmalloc(unsigned long size)
{
    return(vmalloc(size)) ;
}

void  lis_vfree(void *ptr)
{
    vfree(ptr) ;
}

/************************************************************************
*                        DMA Routines                                   *
************************************************************************/

int  lis_request_dma(unsigned int dma_nr, const char *device_id)
{
#if defined(MAX_DMA_CHANNELS)
    return(request_dma(dma_nr, device_id)) ;
#else
    return(-ENXIO) ;
#endif
}

void lis_free_dma(unsigned int dma_nr)
{
#if defined(MAX_DMA_CHANNELS)
    free_dma(dma_nr) ;
#endif
}

/************************************************************************
*                         Delay Routines                                *
************************************************************************/

void lis_udelay(long micro_secs)
{
    udelay(micro_secs) ;
}

unsigned long lis_jiffies(void)
{
    return(jiffies) ;
}

/************************************************************************
*                         Time Routines                                 *
************************************************************************/
void lis_osif_do_gettimeofday( struct timeval *tp )
{
    do_gettimeofday(tp) ;
}

void lis_osif_do_settimeofday( struct timeval *tp )
{
#if LINUX_VERSION_CODE >= 0x020300		/* 2.4 kernel */
    do_settimeofday(tp) ;
#endif						/* not in earlier kernels */
}


/************************************************************************
*                         Printing Routines                             *
************************************************************************/
int lis_printk(const char *fmt, ...)
{
    extern char	    lis_cmn_err_buf[];
    va_list	    args;
    int		    ret ;

    va_start (args, fmt);
    ret = vsprintf (lis_cmn_err_buf, fmt, args);
    va_end (args);

    printk("%s", lis_cmn_err_buf) ;
    return(ret) ;
}

int lis_sprintf(char *bfr, const char *fmt, ...)
{
    va_list	    args;
    int		    ret ;

    va_start (args, fmt);
    ret = vsprintf (bfr, fmt, args);
    va_end (args);

    return(ret) ;
}

int lis_vsprintf(char *bfr, const char *fmt, va_list args)
{
    return(vsprintf (bfr, fmt, args));
}

/************************************************************************
*                      Sleep/Wakeup Routines                            *
************************************************************************/
void lis_sleep_on(OSIF_WAIT_Q_ARG)
{
    sleep_on(wq) ;
}

void lis_interruptible_sleep_on(OSIF_WAIT_Q_ARG)
{
    interruptible_sleep_on(wq) ;
}

void lis_wake_up(OSIF_WAIT_Q_ARG)
{
    wake_up(wq) ;
}

void lis_wake_up_interruptible(OSIF_WAIT_Q_ARG)
{
    wake_up_interruptible(wq) ;
}

/************************************************************************
*                             Timer Routines                            *
************************************************************************/
void 
lis_add_timer(struct timer_list * timer)
{
    add_timer(timer);
}

int  
lis_del_timer(struct timer_list * timer)
{
    return del_timer(timer);
}

/*
 * Prototype in dki.h (with other timer stuff), not osif.h
 */
unsigned lis_usectohz(unsigned usec)
{
    return( usec / (1000000/HZ) ) ;
}
