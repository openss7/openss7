/************************************************************************
*                          LiS PCI Interface                            *
*************************************************************************
*									*
* This interface was created to insulate the Linux STREAMS programmer	*
* from the constantly changing PCI interface provided by the Linux	*
* kernel.								*
*									*
*    Copyright (C) 2000  David Grothe, Gcom, Inc <dave@gcom.com>	*
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
* 									*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the			*
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		*
* Cambridge, MA 02139, USA.						*
*									*
************************************************************************/

#ifndef SYS_LISPCI_H
#define SYS_LISPCI_H	1

#ident "@(#) LiS lispci.h 1.9 08/24/04"

#include <sys/LiS/genconf.h>

/************************************************************************
*                       PCI Device Structure                            *
*************************************************************************
*									*
* The interface consists of returning to the user a pointer to a	*
* structure of the following form.  This is a simplification of the	*
* fields contained in the kernel's similar structure.  We have pared	*
* this structure down to its essentials to make for a more stable	*
* interface.								*
*									*
************************************************************************/

#define	LIS_PCI_MEM_CNT		12		/* # mem addrs */

typedef struct lis_pci_dev
{
    unsigned		bus ;			/* bus number */
    unsigned		dev_fcn ;		/* device/function code */
    unsigned		vendor ;		/* vendor id */
    unsigned		device ;		/* device id */
    unsigned		class ;			/* class type */
    unsigned		hdr_type ;		/* PCI header type */
    unsigned		irq ;			/* IRQ number */

    unsigned long	mem_addrs[LIS_PCI_MEM_CNT] ;

    void		*user_ptr ;		/* private for user */

    /*
     * Fields below here are forbidden for the user to view
     */
    void		*kern_ptr ;		/* private for LiS */
    struct lis_pci_dev	*next ;			/* list ptr */
    
} lis_pci_dev_t ;

/*
 * Macros to isolate the "dev" from the "fcn" part of dev_fcn values.
 */
#define	LIS_PCI_DEV(devfcn)	( ((devfcn) >> 3) & 0x1F )
#define	LIS_PCI_FCN(devfcn)	( (devfcn) & 0x07 )
#define	LIS_MK_DEV_FCN(dev,fcn)	( ((fcn) & 0x07) | (((dev) & 0x1F) << 3) )

/************************************************************************
*                       PCI Interface Routines                          *
*************************************************************************
*									*
* These are the routines that you can call to obtain pci_dev structures	*
* and perform I/O with the PCI device's configuration space.		*
*									*
* These routines perform the same functions as the Linux kernel		*
* routines of similar names.						*
*									*
* The "lis_find_device", "lis_find_class" and "lis_find_slot" routines	*
* return a pointer to a structure that the user can use.  It is OK	*
* for the user to use the "user_ptr" field of this structure to point	*
* to private data that belongs to the driver.  The assumption is that	*
* only one driver will find any one of these devices interesting.	*
*									*
************************************************************************/

lis_pci_dev_t	*lis_pci_find_device(unsigned vendor, unsigned device,
				     lis_pci_dev_t *previous_struct) _RP;
lis_pci_dev_t	*lis_pci_find_class(unsigned class,
				     lis_pci_dev_t *previous_struct) _RP;
lis_pci_dev_t	*lis_pci_find_slot(unsigned bus, unsigned dev_fcn) _RP;

int		 lis_pci_read_config_byte(lis_pci_dev_t *dev,
					  unsigned       index,
					  unsigned char *rtn_val) _RP;
int		 lis_pci_read_config_word(lis_pci_dev_t  *dev,
					  unsigned        index,
					  unsigned short *rtn_val) _RP;
int		 lis_pci_read_config_dword(lis_pci_dev_t *dev,
					  unsigned        index,
					  unsigned long  *rtn_val) _RP;
int		 lis_pci_write_config_byte(lis_pci_dev_t *dev,
					  unsigned        index,
					  unsigned char   val) _RP;
int		 lis_pci_write_config_word(lis_pci_dev_t  *dev,
					  unsigned         index,
					  unsigned short   val) _RP;
int		 lis_pci_write_config_dword(lis_pci_dev_t *dev,
					  unsigned         index,
					  unsigned long    val) _RP;
void		 lis_pci_set_master(lis_pci_dev_t *dev) _RP;
int		 lis_pci_enable_device (lis_pci_dev_t *dev) _RP;
void		 lis_pci_disable_device (lis_pci_dev_t *dev) _RP;

/*
 * Internal routine, not to be called by user
 */
void    lis_pci_cleanup(void) ;

/************************************************************************
*                         PCI Memory Allocation                         *
*************************************************************************
*									*
* These routines are used to allocate memory with contiguous physical	*
* addresses suitable for PCI DMA use.					*
*									*
* See /usr/src/linux/Documentation/DMA-mapping.txt for information on	*
* the underlying kernel routines.					*
*									*
************************************************************************/

typedef struct lis_dma_addr
{
    unsigned	   opaque[8] ;		/* opaque kernel structure */
    int		   size ;		/* saved size */
    void	  *vaddr ;		/* cpu virtual addr */
    lis_pci_dev_t *dev ;		/* PCI device */
    int		   direction ;		/* for map/unmap single */

} lis_dma_addr_t ;

/*
 * Allocate 'size' bytes and return the CPU pointer to it via return value.
 * Return the DMA address via 'dma_handle'.
 */
void	*lis_pci_alloc_consistent(lis_pci_dev_t	 *dev,
				  size_t	  size,
				  lis_dma_addr_t *dma_handle)_RP;

/*
 * Deallocate memory allocated above.  'vaddr' is the address returned
 * by alloc_consistent.  'dma_handle' points to the DMA handle structure
 * returned by the allocate routine.
 */
void	*lis_pci_free_consistent(lis_dma_addr_t *dma_handle)_RP;

u32	lis_pci_dma_handle_to_32(lis_dma_addr_t *dma_handle)_RP;
u64	lis_pci_dma_handle_to_64(lis_dma_addr_t *dma_handle)_RP;

void	lis_pci_map_single(lis_pci_dev_t *dev,
			   void		*ptr,
			   size_t	 size,
			   lis_dma_addr_t *dma_handle,
			   int		 direction)_RP;

void *lis_pci_unmap_single(lis_dma_addr_t *dma_handle)_RP;

/*
 * Miscellaneous
 */
int	lis_pci_dma_supported(lis_pci_dev_t *dev, u64 mask)_RP;
int	lis_pci_set_dma_mask(lis_pci_dev_t *dev, u64 mask)_RP;


/************************************************************************
*                       DMA Synchronization                             *
*************************************************************************
*									*
* Using the DMA handles allocated above, synchronize memory with DMA.	*
*									*
************************************************************************/

#define	LIS_SYNC_FOR_CPU	1	/* direction value */
#define LIS_SYNC_FOR_DMA	2	/* direction value */
#define LIS_SYNC_FOR_BOTH	3	/* direction value */

void lis_pci_dma_sync_single(lis_dma_addr_t	*dma_handle,
			     size_t		 size,
			     int		 direction)_RP;

/************************************************************************
*                          Memory Barrier                               *
*************************************************************************
*									*
* This can sometimes be helpful when trying to synchronize the CPU view	*
* of memory across a number of CPUs, plus DMA accesses.			*
*									*
************************************************************************/

void lis_membar(void) _RP;




#endif				/* SYS_LISPCI_H from top of file */
