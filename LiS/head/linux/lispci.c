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

#ident "@(#) LiS lispci.c 1.8 9/24/02"

#include <sys/stream.h>		/* gets all the right LiS stuff included */
#include <sys/lispci.h>		/* LiS PCI header file */

# ifdef RH_71_KLUDGE			/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM			/* b_page has semi-circular reference */
# endif
#include <linux/pci.h>		/* kernel PCI header file */

#include <sys/osif.h>		/* LiS kernel interface */


static lis_pci_dev_t	*lis_pci_dev_list = NULL ;
static lis_pci_dev_t 	*lis_pci_dev_list_end = NULL ;


/************************************************************************
*                       lis_map_pci_device                              *
*************************************************************************
*									*
* Given a pointer to one of our pci device structures and to one of the	*
* kernel's structures, map theirs into ours.				*
*									*
************************************************************************/
void	lis_map_pci_device(lis_pci_dev_t *p, struct pci_dev *kp)
{
    int		i ;

    /*
     * Copy some data from kernel structure into ours
     */
    p->kern_ptr		= kp ;			/* to kernel's structure */
    p->bus		= kp->bus->number ;
    p->dev_fcn		= kp->devfn ;
    p->vendor		= kp->vendor ;
    p->device		= kp->device ;
    p->class		= kp->class ;
    p->hdr_type		= kp->hdr_type ;
    p->irq		= kp->irq ;

#if defined(KERNEL_2_3)

    {
	int	nn = LIS_PCI_MEM_CNT ;		/* LiS size */

	if (nn > DEVICE_COUNT_RESOURCE)		/* kernel size */
	    nn = DEVICE_COUNT_RESOURCE ;	/* reduce loop count */

	for (i = 0; i < nn; i++)
	{
	    p->mem_addrs[i] = kp->resource[i].start ;
	}
    }

#elif defined(KERNEL_2_1)

    for (i = 0; i < 6; i++)
    {
	p->mem_addrs[i] = kp->base_address[i] ;
    }

    p->mem_addrs[6] = kp->rom_address ;		/* just for fun */

#else

/* there have been plenty of syntax errors by now */

#endif

} /* lis_map_pci_device */

/************************************************************************
*                          lis_pci_find_device                          *
*************************************************************************
*									*
* Find a device of the given vendor and device id and return a pointer	*
* to a lis_pci_dev_t structure for it.					*
*									*
************************************************************************/
lis_pci_dev_t   *lis_pci_find_device(unsigned vendor, unsigned device,
                                     lis_pci_dev_t *previous_struct)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;
    struct pci_dev	*prev_kp ;

    if(previous_struct == NULL)
    {
	prev_kp = NULL;
	p = lis_pci_dev_list;
    }
    else
    {
	prev_kp = previous_struct->kern_ptr ;
	p = previous_struct->next;
    }

    while(p != NULL) 
    {
	if (p->vendor == vendor && p->device == device) return(p) ;
	p = p->next;
    }

    kp = pci_find_device(vendor, device, prev_kp) ;
    if (kp == NULL) return(NULL) ;

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    if(lis_pci_dev_list_end == NULL) /* first one */
    {
	lis_pci_dev_list = p;
	lis_pci_dev_list_end = p;
    }
    else
    {
	lis_pci_dev_list_end->next = p;		/* remember it */
    }
    p->next = NULL;
    lis_pci_dev_list_end = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_device */

/************************************************************************
*                          lis_pci_find_class                           *
*************************************************************************
*									*
* Find a class in a manner similar to finding devices.			*
*									*
************************************************************************/
lis_pci_dev_t   *lis_pci_find_class(unsigned class,
                                     lis_pci_dev_t *previous_struct)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;
    struct pci_dev	*prev_kp ;

    for (p = lis_pci_dev_list; p != NULL; p = p->next)
    {
	if (p->class == class) return(p) ;
    }

    prev_kp = previous_struct == NULL ? NULL : previous_struct->kern_ptr ;
    kp = pci_find_class(class, prev_kp) ;
    if (kp == NULL) return(NULL) ;

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    p->next = lis_pci_dev_list ;		/* remember it */
    lis_pci_dev_list = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_class */

/************************************************************************
*                          lis_pci_find_slot                            *
*************************************************************************
*									*
* Find the structure by slot number.					*
*									*
************************************************************************/
lis_pci_dev_t   *lis_pci_find_slot(unsigned bus, unsigned dev_fcn)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;

    for (p = lis_pci_dev_list; p != NULL; p = p->next)
    {
	if (p->bus == bus && p->dev_fcn == dev_fcn) return(p) ;
    }

    kp = pci_find_slot(bus, dev_fcn) ;
    if (kp == NULL) return(NULL) ;

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    p->next = lis_pci_dev_list ;		/* remember it */
    lis_pci_dev_list = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_slot */


/************************************************************************
*                        lis_pci_read_config_byte                       *
*************************************************************************
*									*
* Read a byte from config space for the device.				*
*									*
************************************************************************/
int              lis_pci_read_config_byte(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned char *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_byte(dev->kern_ptr, index, rtn_val)) ;

} /* lis_pci_read_config_byte */

/************************************************************************
*                        lis_pci_read_config_word                       *
*************************************************************************
*									*
* Read a word from config space for the device.				*
*									*
************************************************************************/
int              lis_pci_read_config_word(lis_pci_dev_t  *dev,
                                          unsigned        index,
                                          unsigned short *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_word(dev->kern_ptr, index, rtn_val)) ;

} /* lis_pci_read_config_word */

/************************************************************************
*                        lis_pci_read_config_dword                      *
*************************************************************************
*									*
* Read a dword from config space for the device.			*
*									*
************************************************************************/
int              lis_pci_read_config_dword(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned long *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_dword(dev->kern_ptr, index, (u32 *) rtn_val)) ;

} /* lis_pci_read_config_dword */

/************************************************************************
*                       lis_pci_write_config_byte                       *
*************************************************************************
*									*
* Write a byte from config space for the device.			*
*									*
************************************************************************/
int              lis_pci_write_config_byte(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned char  val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_byte(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_byte */

/************************************************************************
*                        lis_pci_write_config_word                      *
*************************************************************************
*									*
* Write a word from config space for the device.			*
*									*
************************************************************************/
int              lis_pci_write_config_word(lis_pci_dev_t  *dev,
                                          unsigned        index,
                                          unsigned short  val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_word(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_word */

/************************************************************************
*                        lis_pci_write_config_dword                     *
*************************************************************************
*									*
* Write a dword from config space for the device.			*
*									*
************************************************************************/
int              lis_pci_write_config_dword(lis_pci_dev_t *dev,
                                          unsigned        index,
                                          unsigned long    val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_dword(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_dword */

/************************************************************************
*                          lis_pci_enable_device                        *
*************************************************************************
*									*
* Enable the device.  Sometimes this is needed to resolve IRQ routing	*
* differences between different machines.				*
*									*
************************************************************************/
int lis_pci_enable_device (lis_pci_dev_t *dev)
{
#if LINUX_VERSION_CODE < 0x020300               /* 2.0 or 2.2 kernel */
    return(0) ;					/* pretend success */
#else
    return (pci_enable_device (dev->kern_ptr));
#endif
}

/************************************************************************
*                         lis_pci_disable_device                        *
*************************************************************************
*									*
* Disable the device.  Undoes what lis_pci_enable_device does.		*
*									*
************************************************************************/
void lis_pci_disable_device (lis_pci_dev_t *dev)
{
#if LINUX_VERSION_CODE < 0x020300               /* 2.0 or 2.2 kernel */
    return ;					/* pretend success */
#else
    pci_disable_device (dev->kern_ptr);
#endif
}

/************************************************************************
*                         lis_pci_set_master                            *
*************************************************************************
*									*
* Set bus master capability for the device.				*
*									*
************************************************************************/
void             lis_pci_set_master(lis_pci_dev_t *dev)
{
    if (dev == NULL || dev->kern_ptr == NULL) return ;
    pci_set_master(dev->kern_ptr) ;

} /* lis_pci_set_master */

/************************************************************************
*                             lis_pci_cleanup                           *
*************************************************************************
*									*
* Called at LiS module exit to deallocate structures.			*
*									*
************************************************************************/
void	lis_pci_cleanup(void)
{
    lis_pci_dev_t	*p ;
    lis_pci_dev_t	*np ;

    for (p = lis_pci_dev_list; p != NULL; p = np)
    {
	np = p->next ;
	FREE(p) ;
    }

    lis_pci_dev_list = NULL ;

} /* lis_pci_cleanup */
