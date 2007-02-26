/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdshrdata.h
 * Description                  : SRAM data structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDSHRDATA_
#define _MSDSHRDATA_

#define SRAM_BLOCK_MAX_DATA_SIZE (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER))

extern merc_uint_t	mercd_data_intr_count;  

#define MSD_FREE_MESSAGE(msg)		freemsg(msg)

#define MSD_FREE_BUFFER(msg)		abstract_freeb(msg)

#define MSD_LINK_MESSAGE(Msg1,Msg2)	linkb(Msg1,Msg2)

// return data portion size
#define MsdMessageDataSize(Msg)		msgdsize(Msg)

#define MsdAssert(x)	ASSERT(x)


#ifdef MERCD_LINUX 
#define BZERO 			supp_bzero
#define BCOPY			supp_bcopy
#else
#define BZERO 			bzero
#define BCOPY 			bcopy
#endif

#define MSD_ZERO_MEMORY(Address,Size)    bzero(Address,Size)


#ifndef MERCD_LINUX

#define MERCD_GENERIC_INTR_PROCESSING_ADDR mercd_generic_intr_processing
#define MERCD_GENERIC_INTR_PROCESSING_FUNC( adapterBlock ) \
	mercd_generic_intr_processing( (pmercd_void_t) (adapterBlock))

#if defined LiS || defined LIS || defined LFS
#define MSD_FREE_KERNEL_MEMORY(Address,Size)  kmem_free(Address,Size)
#define MSD_ALLOCATE_KERNEL_MEMORY(Size)  kmem_zalloc(Size,KM_NOSLEEP)
#else
#define MSD_FREE_KERNEL_MEMORY(Address,Size)  kfree(Address,Size)
#define MSD_ALLOCATE_KERNEL_MEMORY(Size)  kmalloc((Size), GFP_ATOMIC)
#endif /* LiS */
	
#define MsdMsecToTicks(msec)  drv_usectohz((long)(msec*1000))
#define MsdBusyDelayMilliSec(msec)  drv_usecwait((long)(msec*1000))

#define MERCD_MAKE_DEVICE( devp, dev ) \
        (*devp) = makedevice( getemajor( (*devp) ), dev);

#define MERCD_GET_MAJOR_NODE( node ) 

#define MERCD_INTR_CLAIMED    DDI_INTR_CLAIMED
#define MERCD_INTR_UNCLAIMED  DDI_INTR_UNCLAIMED

#else

#ifdef LFS
#define KALLOC(n,c)             kmalloc(n,c)
#define KFREE(ptr,size)              kfree(ptr)
#else
#define KALLOC(n,c)             kmalloc(n,c)
#define KFREE(ptr,size)              kfree(ptr,size)
#endif

#define MERCD_GENERIC_INTR_PROCESSING_ADDR NULL
#define MERCD_GENERIC_INTR_PROCESSING_FUNC( adapterBlock ) ;

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
#define MSD_FREE_KERNEL_MEMORY(Address,Size)  kmem_free(Address,Size)
#define MSD_ALLOCATE_KERNEL_MEMORY(Size)  kmem_zalloc(Size,KM_SLEEP)
#define MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(Size)  kmem_zalloc(Size,KM_NOSLEEP)
#else
#define MSD_FREE_KERNEL_MEMORY(Address,Size)  lis_free_mem(Address)
#define MSD_ALLOCATE_KERNEL_MEMORY(Size)  lis_alloc_kernel(Size)
#define MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(Size)  lis_alloc_atomic(Size)
#endif
#else
#define MSD_FREE_KERNEL_MEMORY(Address,Size)  kfree(Address,Size)
#define MSD_ALLOCATE_KERNEL_MEMORY(Size)  kmalloc((Size), GFP_ATOMIC)
#define MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(Size)  kmalloc((Size), GFP_ATOMIC)
#endif /* LiS */
	
#ifdef LFS
#define MsdMsecToTicks(msec)  drv_usectohz((long)(msec*1000))
#define MsdBusyDelayMilliSec(msec)  __udelay((long)(msec*1000))
#else
#define MsdMsecToTicks( msec ) ( (msec) / HZ )  
#define MsdBusyDelayMilliSec(msec)  native_udelay( (size_t)msec * 1000)
#endif

#define MERCD_MAKE_DEVICE( devp, dev ) \
         (*devp) = makedevice(MAJOR(*devp), dev);

#define MERCD_GET_MAJOR_NODE( node ) \
{\
	extern MercdMajorTable majorTable; \
	int major = node / 256; \
\
	if ( major ) {\
		if ( major > MERCD_MAX_MAJOR_DEV ) { \
			MSD_EXIT_CONTROL_BLOCK_MUTEX(); \
			printk( "mercd_open: Exhausted major numbers allocated\n" ); \
			return (ENODEV); \
		} \
		/* get next major number allocated */ \
		*devp = majorTable.major[ major ] << 8; \
	}\
}

#define MERCD_INTR_CLAIMED    1
#define MERCD_INTR_UNCLAIMED  0

#endif

#define MsdCopyMemory(From,To,Size)    bcopy(From,To,Size)

#define MsdStringCopy(From,To)	strcpy(To,From)

#define MsdStringLength(String)	strlen(String)


#define MsdRegReadUchar(DeviceAddress) \
	*(volatile merc_uchar_t * const)(DeviceAddress)

// May need to flush buffer after device write
#define MsdRegWriteUchar(DeviceAddress,Uchar) \
	*(volatile merc_uchar_t * const)(DeviceAddress) = Uchar

#define MsdRegReadUcharBuffer( DeviceAddress, UcharBuffer, Size) { \
	bcopy((caddr_t)(DeviceAddress), \
		(caddr_t)(UcharBuffer), \
		(size_t)(Size)); \
	}

#define MsdRegWriteUcharBuffer( DeviceAddress, UcharBuffer, Size) { \
	bcopy((caddr_t)(UcharBuffer), \
		(caddr_t)(DeviceAddress), \
		(size_t)(Size)); \
	}

typedef void (*MERCD_INTERFACE_FUNC)(PMSD_OPEN_BLOCK MsdOpenBlock, PSTRM_MSG Msg);
typedef void (*MERCD_ABSTRACT_FUNC)(void *rcvPtr);

extern  MERCD_INTERFACE_FUNC  mercd_osal_mid_func[MAX_INTERFACE_FUNCTION];
extern  MERCD_ABSTRACT_FUNC mercd_osal_func[MAX_OSAL_FUNC];
extern  MERCD_ABSTRACT_FUNC mercd_dhal_func[MAX_DHAL_FUNC];

#ifdef MERCD_PCI
extern volatile MSD_PROBE_BOARD_CONFIG msd_probe_brd_cfg[];
extern volatile merc_ushort_t currentboard;
extern volatile merc_ushort_t board_type_list[];
#endif /* MERCD_PCI */

#endif // _MSDSHRDATA_
