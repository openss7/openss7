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

#ident "@(#) LiS lispci.h 1.4 9/24/02"

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
				     lis_pci_dev_t *previous_struct) ;
lis_pci_dev_t	*lis_pci_find_class(unsigned class,
				     lis_pci_dev_t *previous_struct) ;
lis_pci_dev_t	*lis_pci_find_slot(unsigned bus, unsigned dev_fcn) ;

int		 lis_pci_read_config_byte(lis_pci_dev_t *dev,
					  unsigned       index,
					  unsigned char *rtn_val) ;
int		 lis_pci_read_config_word(lis_pci_dev_t  *dev,
					  unsigned        index,
					  unsigned short *rtn_val) ;
int		 lis_pci_read_config_dword(lis_pci_dev_t *dev,
					  unsigned        index,
					  unsigned long  *rtn_val) ;
int		 lis_pci_write_config_byte(lis_pci_dev_t *dev,
					  unsigned        index,
					  unsigned char   val) ;
int		 lis_pci_write_config_word(lis_pci_dev_t  *dev,
					  unsigned         index,
					  unsigned short   val) ;
int		 lis_pci_write_config_dword(lis_pci_dev_t *dev,
					  unsigned         index,
					  unsigned long    val) ;
void		 lis_pci_set_master(lis_pci_dev_t *dev) ;
int		 lis_pci_enable_device (lis_pci_dev_t *dev) ;
void		 lis_pci_disable_device (lis_pci_dev_t *dev) ;

/*
 * Internal routine, not to be called by user
 */
void    lis_pci_cleanup(void) ;


#endif				/* SYS_LISPCI_H from top of file */
