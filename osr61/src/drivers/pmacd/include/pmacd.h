/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd.h
//
// Defines types and defines for:
// 1) Any OS/Platform mapping functions and types.
// 2) Configuring the driver.
// 3) The external interface between the driver and pdi.
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_TYPES_H
#define _PMACD_TYPES_H

/**************************************************************************
 **************************************************************************
 *
 *  OS/PLATFORM MAPPING TYPES
 *
 **************************************************************************
 *************************************************************************/

/**********************************
 *
 * Architecture independent section
 *
 **********************************/

typedef int   bool, BOOLEAN_T;
typedef unsigned long pmacd_memaddress_t;

#ifndef true
#define true  (1)
#endif

#ifndef false
#define false (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

// Short name notations
#ifndef UCHAR_T
#define UCHAR_T unsigned char
#endif

#ifndef USHORT_T
#define USHORT_T unsigned short
#endif

#ifndef ULONG_T
#define ULONG_T unsigned long
#endif

#ifndef SHORT_T
#define SHORT_T short
#endif

#ifndef UINT_T
#define UINT_T unsigned int
#endif


typedef UINT_T pmacd_time_sec_t;
typedef UINT_T pmacd_time_msec_t;
typedef UINT_T pmacd_time_usec_t;

/************************/
/* IOCTL UTILITY MACROS */
/************************/
#include <linux/ioctl.h>
#define IOCTL_PMACD_BASE       'U'
#define PMACD_IO(nr)           _IO(IOCTL_PMACD_BASE,nr)
#define PMACD_IOR(nr,size)     _IOR(IOCTL_PMACD_BASE,nr,size)
#define PMACD_IOW(nr,size)     _IOW(IOCTL_PMACD_BASE,nr,size)
#define PMACD_IOWR(nr,size)    _IOWR(IOCTL_PMACD_BASE,nr,size)

/* LINUX KERNEL MODE CODE DEFINES */
#ifdef __KERNEL__    


/********************************
 *
 * Architecture dependent section
 *
 ********************************/

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/poll.h>
#ifndef LFS
#include <linux/config.h>
#endif
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#ifdef LINUX24
#include <linux/tqueue.h>
#ifndef IRQ_NONE
typedef void irqreturn_t;
#define IRQ_NONE
#define IRQ_HANDLED
#define IRQ_RETVAL(x)
#endif
#else
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#endif
#include <linux/pci.h>
//#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include "ctimod.h"

#ifndef UINT8_T
#define UINT8_T __u8
#endif

#ifndef UINT32_T
#define UINT32_T __u32
#endif

#ifndef UINT16_T
#define UINT16_T __u16
#endif

#define PMACD_PAGE_SIZE PAGE_SIZE

#define PMACD_TICKS_PER_SEC HZ
#define PMACD_GET_CURRENT_TICK() jiffies

////////////////////////////////////////
// KERNEL compatability macros
////////////////////////////////////////

// All kernels before 2.2
#if (LINUX_VERSION_CODE < 0x020200)
#error "kernels below 2.2 are not supported."
#else

// 2.2 Kernel
#if (LINUX_VERSION_CODE < 0x020300)
#include <asm/spinlock.h>
#define pci_resource_start(ppciDev,bar)(((ppciDev)->base_address[(bar)])&(~0xF))
#define	pci_resource_region_flags(ppciDev,bar)(((ppciDev)->base_address[(bar)])&0xF)
#define WAITQUEUE_T struct wait_queue *
#define INIT_WAITQUEUE(waitqueue) (waitqueue = NULL)
#define ALLOCATE_LOCAL_WAITQUEUE struct wait_queue __localwait = {current, NULL}
#define ATOMIC_INC_AND_TEST_GREATER_ZERO(atomicVar)\
	 atomic_inc_and_test_greater_zero(&(atomicVar))
#else

// 2.3 Kernel
#if (LINUX_VERSION_CODE < 0x020400)
#error "2.3 kernels not supported."
#else

// 2.4 & 2.6 Kernel
#if (LINUX_VERSION_CODE < 0x020700)
#include <linux/spinlock.h>
#define	pci_resource_region_flags(ppciDev,bar)((pci_resource_flags((ppciDev),(bar)))&0xF)
#define WAITQUEUE_T wait_queue_head_t
#define INIT_WAITQUEUE(waitqueue) (init_waitqueue_head(&(waitqueue)))
#define ALLOCATE_LOCAL_WAITQUEUE DECLARE_WAITQUEUE((__localwait),current)

#define ATOMIC_ADD_NEGATIVE(addVar, atomicVar)\
	 atomic_add_negative(addVar , &(atomicVar))

// All Kernels above 2.6
#else
#error "Only 2.2, 2.4 and 2.6 kernels are supported."
#endif /* LINUX_VERSION_CODE */
#endif /* LINUX_VERSION_CODE */
#endif /* LINUX_VERSION_CODE */
#endif /* LINUX_VERSION_CODE */
////////////////////////////////////////


// Wait Queues are implemented using the alternate unrolled method for avoiding
// the sleep race condition.  
// See "Linux Device Drivers: Chapter 9: Going to Sleep Without Races"

#define ADD_TO_WAITQUEUE(waitqueue) add_wait_queue(&(waitqueue), &__localwait);\
                                    (current->state) = TASK_INTERRUPTIBLE
#define REMOVE_FROM_WAITQUEUE(waitqueue) remove_wait_queue(&(waitqueue),&__localwait);\
                                         current->state = TASK_RUNNING
#define SLEEP_ON_WAITQUEUE_INTERRUPTIBLE(waitqueue) current->state = TASK_INTERRUPTIBLE;\
                                                    schedule() 
#define SLEEP_ON_WAITQUEUE_TIMEOUT_INTERRUPTIBLE(waitqueue, timeout)\
      current->state = TASK_INTERRUPTIBLE; schedule_timeout((timeout)*HZ)

#define WAKEUP_WAITQUEUE(waitqueue) wake_up_interruptible(&(waitqueue))
#define IS_WAITQUEUE_ACTIVE(waitqueue) waitqueue_active(&(waitqueue))

#define MUTEX_T spinlock_t
#define INIT_MUTEX(mutex) spin_lock_init(&(mutex))
#define MUTEX_ENTER(mutex) spin_lock(&(mutex))
#define MUTEX_EXIT(mutex) spin_unlock(&(mutex))
#define MUTEX_ENTER_BH(mutex) spin_lock_bh(&(mutex))
#define MUTEX_EXIT_BH(mutex) spin_unlock_bh(&(mutex))

// These versions shutoff interrupts on the present processor, 
// and save/restore processor flags.
// These should be used when protecting against ISRs/BHs (avoid preemption)
#define MUTEX_ENTER_IRQSAVE(mutex, flags)spin_lock_irqsave(&(mutex), (flags))
#define MUTEX_EXIT_IRQRESTORE(mutex, flags)spin_unlock_irqrestore(&(mutex),(flags))

#define ATOMIC_T atomic_t
#define ATOMIC_SET(atomicVar,val) atomic_set(&(atomicVar),(val))
#define ATOMIC_GET(atomicVar) atomic_read(&(atomicVar))
#define ATOMIC_INC(atomicVar) atomic_inc(&(atomicVar))
#define ATOMIC_DEC(atomicVar) atomic_dec(&(atomicVar))
#define ATOMIC_ADD(atomicVar,val) atomic_add((val), &(atomicVar))
#define ATOMIC_SUB(atomicVar,val) atomic_sub((val), &(atomicVar))
#define ATOMIC_DEC_AND_TEST(atomicVar) atomic_dec_and_test(&(atomicVar))

#define pmacd_printError(fmt, args...)printk(KERN_ERR "pmacd: ERROR: " fmt, ##args)
#define pmacd_printDebug(fmt, args...) printk(KERN_DEBUG "pmacd: DEBUG: " fmt, ##args)
#define pmacd_memcpy_UtoK(retCode, dest, src, size) { *(retCode)=copy_from_user((dest), (src), (size));}
#define pmacd_memcpy_KtoK(retCode, dest, src, size) { *(retCode)=0; memcpy((dest), (src), (size));}
#define pmacd_memcpy_KtoU(retCode, dest, src, size) { *(retCode)=copy_to_user((dest), (src), (size));}

extern inline pmacd_memaddress_t pmacd_malloc(int size){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  ATOMIC_ADD(pmacd_dynamicMemoryUsage, size);
  return((pmacd_memaddress_t)kmalloc((size), GFP_ATOMIC));
}

extern inline void pmacd_free(void *memptr, int size){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  ATOMIC_SUB(pmacd_dynamicMemoryUsage, size);
  kfree(memptr, size);
}

extern inline pmacd_memaddress_t pmacd_mallocPage(void){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  ATOMIC_ADD(pmacd_dynamicMemoryUsage, PMACD_PAGE_SIZE);
  return((pmacd_memaddress_t)__get_free_page(GFP_ATOMIC));
}

extern inline void pmacd_freePage(pmacd_memaddress_t memptr){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  ATOMIC_SUB(pmacd_dynamicMemoryUsage, PMACD_PAGE_SIZE);
  free_page((unsigned long)memptr);
}

//
// ALLOCATING BLOCKS OF PAGES NOTE:
// -------------------------------
// Blocks of contigious pages must be allocated with an order number (n).
// 2^n pages will be allocated.  Various notes around the net suggest
// that you should not allocate more than 128K of pages which is 
// 32 pages (4k page size) on a 32 bit architecture or order 5.
// A 64 bit architecture (8k page size) max order 4.
// The best way to convert from bytes to page order is to shift
// right counting the number of bit places.
// ie.  while(size_in_bytes>>=1 && page_order<12)page_order++;
// The page_order<12 is to bound the shifting in case of a bad byte
// value.

extern inline pmacd_memaddress_t pmacd_mallocPageBlock(int size){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  int page_order = 0;
  //int osize = size;
  pmacd_memaddress_t startAddr;
  
  ATOMIC_ADD(pmacd_dynamicMemoryUsage, size);
  size /= PAGE_SIZE;
  while(size>>=1 && page_order<12)page_order++;
  // We need to allocate with GFP_ATOMIC, because we may call this from a Bottom Half.
  startAddr = (pmacd_memaddress_t) __get_free_pages(GFP_ATOMIC, page_order);
  return(startAddr);
}

extern inline BOOLEAN_T pmacd_freePageBlock(pmacd_memaddress_t startAddr, int size){
  extern ATOMIC_T pmacd_dynamicMemoryUsage;
  int page_order = 0;

  ATOMIC_SUB(pmacd_dynamicMemoryUsage, size);  
  size /= PAGE_SIZE;
  while(size>>=1 && page_order<12)page_order++;
  free_pages(startAddr, page_order);
  return(TRUE);
}


/* LINUX USER MODE CODE DEFINES */
#else

/********************************
 *
 * Architecture dependent section
 *
 ********************************/

// IA32 defines
#define UINT8_T  unsigned char
#define UINT32_T unsigned int
#define UINT16_T unsigned short

#endif

/***************************************************************************
 ***************************************************************************
 *
 *  DRIVER SPECIFIC TYPES
 *
 ***************************************************************************
 **************************************************************************/
#define PMACD_PROBED 1
#define PMACD_RESUMED 2
#define PMACD_SUSPENDED 3

// Major Number Default(0) is to dynamically allocate the major number.  
// This can be changed at driver load time.
#define PMACD_MAJOR         0
#define PMACD_CONTROL_MINOR 0
#define PMACD_QUEUES_MINOR  1
#define PMACD_STREAMS_MINOR 2

// Vendor=DEC, Device=21554, SubVendor=Dialogic, SubDevice=PMAC
#define PMACD_VENDOR_ID_21555        0x8086
#define PMACD_VENDOR_ID_21554        0x1011
#define PMACD_DEVICE_ID_21554        0x46
#define PMACD_DEVICE_ID_21555        0xB555
#define PMACD_SUBSYS_VENDOR_ID 0x12c7
#define PMACD_SUBSYS_DEVICE_ID 0x4011

/* Driver restrictions
   --------------------
   1) Max 31 addressable boards (5 bits) 
      (b000000XX - b011111XX)
   2) Max 4 addressable board components per board (2 bits) 
      (bOXXXXX00 - b0XXXXX11)
   4) Max 128 addressable host queues (7 bits) 
      (b10000000 - b11111111)
   5) MSB is a flag for distinguishing host address from board addresses
   6) 127 queues can be used for applications. 1 queue address is saved
      for driver messages.
   6) PMB queue management is page based.  Be very careful if you want to 
      change the PMBBLOCK_SIZE.  Read through pmacd_pmb.c for impact.  
      Multi(Integral)-pages may work, but subpages is asking for trouble, 
      and is inefficient.
*/
#define PMACD_BOARDNUM_MASK       0x7C
#define PMACD_BOARDCOMP_MASK      0x03

/* Bit identifies address as a host or board address. 1=host 0=board */
#define PMACD_HOSTBOARD_FLAG      0x80

#define PMACD_MAX_QUEUES          127
#define PMACD_MAX_BOARDS          31
#define PMACD_MAX_BOARDID         30
#define PMACD_PMBBLOCK_SIZE       PAGE_SIZE
#define PMACD_DRIVER_ADDRESS      0xFF

//
// DEFAULT VALUES
//
#define PMACD_STREAM_BUFFER_SIZE   2048
#define PMACD_BUFFERS_PER_STREAM   2
#define PMACD_STREAMS_PER_BOARD    672
#define PMACD_MF_SIZE              256
#define PMACD_MF_COUNT             256
#define PMACD_INBOUNDMF_THRESHOLD  50
#define PMACD_OUTBOUNDMF_THRESHOLD 50
#define PMACD_TRANSFER_MODE        pmacd_MsgTransferMode_Batch

// 21554 Paging Base Address Register Configuration
// The paging unit is hard coded to 64K in the configuration
// at this time.  If this changes or is configureable the code
// 
#define PMACD_MEMORYUNIT_PAGE_SIZE 65536

// TRANSFER_TIMER is in milliseconds.  Be careful about precision. Most
// 32 bit architectures do not have a timer prescion below 10ms.  If you
// set the timer below 10 ms, you will be setting the timer to 0.
#define PMACD_TRANSFER_TIMER       50

// Time to wait before moving from NULL to RESET during a hard reset. (In seconds)
// Presently it takes the board around 30 seconds in .6 firmware.
#define PMACD_HARD_RESET_TIMEOUT  30

// CMD_RESPONSE_TIMOUT is in seconds 
#define PMACD_CMD_RESPONSE_TIMEOUT 120

// Set highwater mark at 4Meg. (or at least the integral PMBBLOCK equivelant)
// For 256 byte messages that is 16K messages
#define PMACD_PMBQUEUE_HIGHWATER_MARK (4096*1024/(PMACD_PMBBLOCK_SIZE))

// Firmware related constants.
#define PMACD_CSR_BAR           0
#define PMACD_CSR_SIZE          4096
#define PMACD_CONFIGBLOCK_BAR   3
#define PMACD_CONFIGBLOCK_SIZE  8192

// Board Led could be turned on/off by setparm
#define PMACD_BOARD_LED_OFF 	0
#define PMACD_BOARD_LED_ON 	1

// Encoding board number for routing through the driver.
#define PMACD_DECODE_BOARDNUM(srcAdd) (((srcAdd)&(PMACD_BOARDNUM_MASK))>>2)
#define PMACD_ENCODE_BOARDNUM(srcAdd, boardNum)\
              (((boardNum)<<2) | ((srcAdd)&(~(PMACD_BOARDNUM_MASK))))


typedef struct pmacd_linklist_t{
  void *element;
  struct pmacd_linklist_t *next;
}pmacd_linklist_t;

typedef UINT8_T pmacd_boardnumber_t;
typedef ULONG_T pmacd_boardId_t;

typedef enum {
  pmacd_BoardState_Null,
  pmacd_BoardState_Initialized,
  pmacd_BoardState_Reset,
  pmacd_BoardState_Failed,
  pmacd_BoardState_Configured,
  pmacd_BoardState_Running,
  pmacd_BoardState_Quiesced,
  pmacd_BoardState_Stopped,
  pmacd_BoardState_Boot,
  pmacd_BoardState_Downloaded
}pmacd_boardstate_t;

typedef enum {
  pmacd_ControlCmd_None,
  pmacd_ControlCmd_Reset,
  pmacd_ControlCmd_Init,
  pmacd_ControlCmd_Configure,
  pmacd_ControlCmd_Start,
  pmacd_ControlCmd_Quiesce,
  pmacd_ControlCmd_Stop
}pmacd_controlcmd_t;

typedef enum {
  pmacd_BusType_Pci
}pmacd_bustype_t;

typedef enum {
  // Driver Parms
  pmacd_Parm_DriverParmsStart = 0x100,
  pmacd_Parm_NumberOfBoards,
  pmacd_Parm_MsgSize,
  pmacd_Parm_MsgUnitInboundMode,
  pmacd_Parm_MsgUnitOutboundMode,
  pmacd_Parm_MsgUnitInboundThreshold,
  pmacd_Parm_MsgUnitOutboundThreshold,
  pmacd_Parm_MsgUnitInboundTimer,
  pmacd_Parm_MsgUnitOutboundTimer,  
  pmacd_Parm_QueueHighWaterMark,
  pmacd_Parm_StreamBufferSize,
  pmacd_Parm_BuffersPerStream,
  pmacd_Parm_StreamsPerBoard,
  pmacd_Parm_DriverParmsStop,

  // Driver Parm Groupings
  pmacd_Parm_DriverParmGroupsStart = 0x200,
  pmacd_Parm_MsgUnitConfig,
  pmacd_Parm_StreamConfig,
  pmacd_Parm_DriverParmGroupsStop,
  
  // Board Parms
  pmacd_Parm_BoardParmsStart = 0x300,
  pmacd_Parm_BoardState,
  pmacd_Parm_BoardParmsStop,

  // Board Parm Groupings
  pmacd_Parm_BoardParmGroupsStart = 0x400,
  pmacd_Parm_PhysicalSlotInfo,
  pmacd_Parm_BoardParmGroupsStop,
  pmacd_Parm_BoardLed
}pmacd_parmId_t;


/*******************/
/* IOCTL DEFINES   */
/*******************/
typedef enum {
  // Driver Stats
  pmacd_Stats_DriverStatsStart = 0x10,
  pmacd_Stats_NumberOfQueuesAllocated,
  pmacd_Stats_DriverStatsStop,

  // Board Stats
  pmacd_Stats_BoardStatsStart = 0x40,
  pmacd_Stats_NumberOfMessagesSent,
  pmacd_Stats_BoardStatsStop
}pmacd_statsId_t;

typedef struct{
  ULONG_T boardId;
  ULONG_T busId;
  ULONG_T slotId;
  ULONG_T geoAddr;
  pmacd_bustype_t busType;
  pmacd_boardstate_t state;
}pmacd_boardreport_t;

typedef struct{
  ULONG_T busId;
  ULONG_T slotId;
  ULONG_T geoAddr;
  ULONG_T irq;
  pmacd_bustype_t busType; 
}pmacd_physicalSlotInfo_t;

typedef struct{
  ULONG_T numberOfBoards;
  pmacd_boardreport_t *boardReports;
}pmacd_reportBoards_t;

typedef struct{
  pmacd_boardId_t boardId;
  ULONG_T fileSize;
}pmacd_dnld_t;
    
typedef struct{
  pmacd_boardId_t boardId;
  ULONG_T contextId;
  ULONG_T termId;
  ULONG_T streamAttributes;
  ULONG_T bufferSize;
  ULONG_T numberOfBuffers;
}pmacd_attachStream_t;
    
typedef struct{
  ULONG_T bufferSize;
  ULONG_T numberOfBuffers;
  void    *startAddress;
}pmacd_detachStream_t;

typedef struct{
  pmacd_boardId_t boardId;
  pmacd_parmId_t parmId;
  void *pValue;
}pmacd_ioctlBoardParm_t;

typedef struct{
  pmacd_parmId_t parmId;
  void *pValue;
}pmacd_ioctlParm_t;

typedef struct{
  pmacd_boardId_t boardId;
  pmacd_statsId_t statsId;
  void *pValue;
}pmacd_getDiagCounts_t;

typedef struct{
  pmacd_boardId_t boardId;
  pmacd_statsId_t statsId;
}pmacd_resetDiagCounts_t;

typedef struct{
  pmacd_boardId_t boardId;
  ULONG_T busId;
  ULONG_T slotId;
}pmacd_mapBoard_t;

typedef struct{
  ULONG_T    bufferSize;
  ULONG_T    buffersPerStream;
  ULONG_T    streamsPerBoard;
}pmacd_streamConfig_t;

// Driver Level Commands
#define IOCTL_PMACD_REPORT_BOARDS      PMACD_IOR( 0x01, pmacd_reportBoards_t)
#define IOCTL_PMACD_GET_PARM           PMACD_IOWR( 0x02, pmacd_ioctlParm_t)
#define IOCTL_PMACD_SET_PARM           PMACD_IOWR( 0x03, pmacd_ioctlParm_t)
#define IOCTL_PMACD_GET_DIAG_COUNTS    PMACD_IOWR( 0x04, pmacd_getDiagCounts_t)
#define IOCTL_PMACD_RESET_DIAG_COUNTS  PMACD_IOWR( 0x05, pmacd_resetDiagCounts_t)

// Board Commands
#define IOCTL_PMACD_RESET_BOARD       PMACD_IOW( 0x10, pmacd_boardId_t)
#define IOCTL_PMACD_INIT_BOARD        PMACD_IOW( 0x11, pmacd_boardId_t)
#define IOCTL_PMACD_START_BOARD       PMACD_IOW( 0x12, pmacd_boardId_t)
#define IOCTL_PMACD_QUIESCE_BOARD     PMACD_IOW( 0x13, pmacd_boardId_t)
#define IOCTL_PMACD_STOP_BOARD        PMACD_IOW( 0x14, pmacd_boardId_t)
#define IOCTL_PMACD_CONFIGURE_BOARD   PMACD_IOW( 0x15, pmacd_boardId_t)
#define IOCTL_PMACD_GET_BOARD_PARM    PMACD_IOWR( 0x16, pmacd_ioctlBoardParm_t)
#define IOCTL_PMACD_SET_BOARD_PARM    PMACD_IOWR( 0x17, pmacd_ioctlBoardParm_t)
#define IOCTL_PMACD_HARD_RESET_BOARD  PMACD_IOW( 0x18, pmacd_boardId_t)
#define IOCTL_PMACD_START_DOWNLOAD_FW PMACD_IOW( 0x19, pmacd_boardId_t)
#define IOCTL_PMACD_COMPLETE_DOWNLOAD_FW  PMACD_IOW( 0x1A, pmacd_boardId_t)
#define IOCTL_PMACD_RSAEBIMBSCM       PMACD_IOW( 0x1B, pmacd_boardId_t)
#define IOCTL_PMACD_GET_FW_BUFFER     PMACD_IOW( 0x1C, pmacd_boardId_t)

// Stream Commands
#define IOCTL_PMACD_ATTACH_STREAM     PMACD_IOWR( 0x21, pmacd_attachStream_t)
#define IOCTL_PMACD_START_STREAM      PMACD_IO( 0x22 )
#define IOCTL_PMACD_DETACH_STREAM     PMACD_IOWR( 0x23, pmacd_detachStream_t)
/******************/


typedef enum {
  pmacd_MsgTransferMode_Batch,
  pmacd_MsgTransferMode_ASAP
}pmacd_msgTransferMode_t;

typedef char pmacd_MFPullOption_t;
typedef enum {
  pmacd_MFPullOption_AllOnHost,
  pmacd_MFPullOption_OutboundOnHost
}pmacd_MFPullOptionValues_t;


// Messaging Unit configuration structure.
typedef struct{
  ULONG_T                    msgSize;
  pmacd_MFPullOptionValues_t pullOption;
  pmacd_msgTransferMode_t    inboundTransferMode;
  pmacd_msgTransferMode_t    outboundTransferMode;
  ULONG_T                    inboundQueueSize;
  ULONG_T                    outboundQueueSize;
  ULONG_T                    inboundThreshold;
  ULONG_T                    outboundThreshold;
  ULONG_T                    inboundTimer;
  ULONG_T                    outboundTimer;
}pmacd_msgUnitConfig_t;


#endif /* _PMACD_TYPES_H */










