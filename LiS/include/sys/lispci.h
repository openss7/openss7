/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

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
************************************************************************/

#ifndef SYS_LISPCI_H
#define SYS_LISPCI_H	1

#ident "@(#) $RCSfile: lispci.h,v $ $Name:  $($Revision: 1.1.1.3.4.6 $) $Date: 2005/12/18 05:41:24 $"

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

#define	LIS_PCI_MEM_CNT		12	/* # mem addrs */

typedef struct lis_pci_dev {
	unsigned bus;			/* bus number */
	unsigned dev_fcn;		/* device/function code */
	unsigned vendor;		/* vendor id */
	unsigned device;		/* device id */
	unsigned class;			/* class type */
	unsigned hdr_type;		/* PCI header type */
	unsigned irq;			/* IRQ number */

	unsigned long mem_addrs[LIS_PCI_MEM_CNT];

	void *user_ptr;			/* private for user */

	/* 
	 * Fields below here are forbidden for the user to view
	 */
	void *kern_ptr;			/* private for LiS */
	struct lis_pci_dev *next;	/* list ptr */

} lis_pci_dev_t;

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

lis_pci_dev_t *_RP lis_pci_find_device(unsigned vendor, unsigned device,
				       lis_pci_dev_t *previous_struct);

#ifdef HAVE_KFUNC_PCI_FIND_CLASS
#ifdef __LIS_INTERNAL__
lis_pci_dev_t *_RP lis_pci_find_class(unsigned class, lis_pci_dev_t *previous_struct);
#endif
#endif
lis_pci_dev_t *_RP lis_pci_find_slot(unsigned bus, unsigned dev_fcn);

int _RP lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char *rtn_val);
int _RP lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short *rtn_val);
int _RP lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long *rtn_val);
int _RP lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char val);
int _RP lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short val);
int _RP lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long val);
void _RP lis_pci_set_master(lis_pci_dev_t *dev);
int _RP lis_pci_enable_device(lis_pci_dev_t *dev);
void _RP lis_pci_disable_device(lis_pci_dev_t *dev);

/*
 * Internal routine, not to be called by user
 */
#ifdef __LIS_INTERNAL__
void lis_pci_cleanup(void);
#endif

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

typedef struct lis_dma_addr {
	unsigned opaque[8];		/* opaque kernel structure */
	int size;			/* saved size */
	void *vaddr;			/* cpu virtual addr */
	lis_pci_dev_t *dev;		/* PCI device */
	int direction;			/* for map/unmap single */

} lis_dma_addr_t;

/*
 * Allocate 'size' bytes and return the CPU pointer to it via return value.
 * Return the DMA address via 'dma_handle'.
 */
void *_RP lis_pci_alloc_consistent(lis_pci_dev_t *dev, size_t size, lis_dma_addr_t * dma_handle);

/*
 * Deallocate memory allocated above.  'vaddr' is the address returned
 * by alloc_consistent.  'dma_handle' points to the DMA handle structure
 * returned by the allocate routine.
 */
void *_RP lis_pci_free_consistent(lis_dma_addr_t * dma_handle);

u32 _RP lis_pci_dma_handle_to_32(lis_dma_addr_t * dma_handle);
u64 _RP lis_pci_dma_handle_to_64(lis_dma_addr_t * dma_handle);

void _RP lis_pci_map_single(lis_pci_dev_t *dev, void *ptr, size_t size, lis_dma_addr_t * dma_handle,
			    int direction);

void *_RP lis_pci_unmap_single(lis_dma_addr_t * dma_handle);

/*
 * Miscellaneous
 */
int _RP lis_pci_dma_supported(lis_pci_dev_t *dev, u64 mask);
int _RP lis_pci_set_dma_mask(lis_pci_dev_t *dev, u64 mask);

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

#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
#ifdef __LIS_INTERNAL__
void _RP lis_pci_dma_sync_single(lis_dma_addr_t * dma_handle, size_t size, int direction);
#endif
#endif

/************************************************************************
*                          Memory Barrier                               *
*************************************************************************
*									*
* This can sometimes be helpful when trying to synchronize the CPU view	*
* of memory across a number of CPUs, plus DMA accesses.			*
*									*
************************************************************************/

void _RP lis_membar(void);

#endif				/* SYS_LISPCI_H from top of file */
