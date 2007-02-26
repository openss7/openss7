/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_pmb.h
// 
// Defines structures and functions for working with PMB's and PMB queues.
// (PMAC Message Blocks)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_PMB_H
#define _PMACD_PMB_H

#include "pmacd.h"

/* 
 *  Driver/Board PMBs
 *  -- These are the raw PMBs sent between the board and the host.
 *  -- Includes an extra type for handling blocks of PMBs.
 *  -- Includes a type for building variable lengthed structs.
 */
typedef UINT8_T pmacd_pmb_t[];
typedef UINT8_T pmacd_pmbblock_t[];
typedef UINT8_T pmacd_pmbstart_t;

#pragma pack(1)
/* 
 *  User Level PMBs
 *  -- PMBs in user space need an extra header for board routing.
 */
typedef struct{
  pmacd_boardnumber_t boardNumber;
}pmacd_userpmb_header_t;

/* 
 *  PMB header
 *  -- The pmb header that the driver needs.
 */
typedef struct{
  struct{ 
    UINT8_T msgType:2;
    UINT8_T emergency:1;
    UINT8_T priority:2;
    UINT8_T extensionHdr:1;
    UINT8_T version:2;
  }flags;

  UINT8_T  length;
  UINT8_T  srcNode;
  UINT8_T  destNode;
  UINT32_T userCmdId;
  UINT16_T contextId;
  UINT8_T  reserved;
  UINT8_T  numberOfCommands;
}pmacd_msgheader_t;

//
// PMAC Command Header
//
typedef struct{
  UINT8_T  command;
  UINT8_T  length;
  UINT8_T  numberOfTerms;
  UINT8_T  numberOfDescs;
  UINT32_T termId;
}pmacd_commandHeader_t;


// PMAC Message identifiers need for building messages that
// the driver must deal with.
#define PMACD_CALLCONTROL_NODE             0x00
#define PMACD_CMD_MODIFY_REQUEST           0x03
#define PMACD_CMD_MODIFY_RESPONSE          0x83
#define PMACD_CMD_EXITNOTIFY_REQUEST       0x09
#define PMACD_CMD_EXITNOTIFY_RESPONSE      0x89
#define PMACD_DESCID_LOCAL_CONTROL         0x06
#define PMACD_DESCID_ERROR                 0xFF
#define PMACD_DESCID_SRCNODE_REGISTER      0x19

// Register Event Type
#define PMACD_REG_EXITNOTIFY_BEHAVIOR    0x02

#define PMACD_NULL_CONTEXT_ID 	 	 0x0
#define PMACD_CHOOSE_CONTEXT_ID 	 0xFFFE
#define PMACD_ALL_CONTEXT_ID 	 	 0xFFFF
// Memorize Flag
#define PMACD_SRCNODEREG_CLEARALL     ((UINT8_T)0)  /* Clear all reg nodes */
#define PMACD_SRCNODEREG_CLEARIND     ((UINT8_T)1)  /* Clear this node */
#define PMACD_SRCNODEREG_SET          ((UINT8_T)2)  /* Reg this node */

//
// Source Node Registration Descriptor
//
typedef struct{
  UINT8_T descId;
  UINT8_T registerEvtType;
  UINT8_T memorize;
  UINT8_T numberOfPropertyIds;
}pmacd_srcregistration_t;

//
// PMAC Error Descriptor
//
typedef struct{
  UINT8_T  descId;
  UINT8_T  reserved;
  UINT16_T errorCode;
  UINT8_T  errorStringLength;
}pmacd_errorDesc_t;

#pragma pack()

/* 
 *  Copy Mode
 *  -- used to decide if memcopys are crossing user/kernel boundaries.
 *  -- if they are special copy functions need to be used.
 */
typedef enum {
  pmacd_UserToKernel,
  pmacd_KernelToKernel,
  pmacd_KernelToUser  
}pmacd_copymode_t;

/* 
 *  PMB Queue
 *  -- queue structure that contains all the information for managing
 *  -- a single PMB queue.
 */
typedef struct{
  MUTEX_T readMutex;              // Used to protect against multiple readers
  MUTEX_T writeMutex;             // Used to protect against multiple writers
  WAITQUEUE_T readers;            // processes waiting for a message

  // Used to protect readers from writers and vice versa
  ATOMIC_T msgCapacity;           // Used to track how much the queue can hold
  ATOMIC_T wrapAround;            // Used for detecting when the tail pointer 
                                  // wrapped around into the head pointer's block
                                  // we must grow the queue on this condition.

  // numOfBlocks and numOfMessages are only changed during queue growing.
  // Queue growing must be both read and write mutex protected.
  int numberOfBlocks;           // total number of blocks allocated
  int numberOfMessages;         // total number of messages allocated

  // The following 4 fields are readonly set at allocation
  ULONG_T pmbSize;                // size of a message
  ULONG_T pmbsPerBlock;           // messages per block
  int highWaterMark;              // max number of blocks we will grow the queue
  UINT8_T queueAddress;   // Used for routing messages from boards
  
  pmacd_linklist_t *blockhead;
  pmacd_linklist_t *blocktail;
  pmacd_pmb_t *pmbhead;
  pmacd_pmb_t *pmbtail;
}pmacd_pmbqueue_t;

// WrapAround Algorithm:
// --------------------
// wrapAround is initialized to numberOfBlocks;
// Any time a new block is added wrapAround is incremented;
// When head enters a new block wrapAround is incremented;
// When tail enters a new block wrapAround is decremented;
// If wrapAround == 0 we have wrapped; either grow or risk queue filling
//
// Operations dealing with wraparound management
//
#define pmacd_decrPMBQueueWrapAroundAndTestForZero(queue)\
         ATOMIC_DEC_AND_TEST((queue)->wrapAround)
#define pmacd_setPMBQueueWrapAround(queue, size)\
         (ATOMIC_SET((queue)->wrapAround, (size)))
#define pmacd_getPMBQueueWrapAround(queue)\
         (ATOMIC_GET((queue)->wrapAround))
#define pmacd_incrPMBQueueWrapAround(queue)\
         (ATOMIC_INC((queue)->wrapAround))
#define pmacd_decrPMBQueueWrapAround(queue)\
         (ATOMIC_DEC((queue)->wrapAround))

//
// Queue Capacity Algorithm:
// ------------------------
// Queue capacity is set to the # of messages that a queue can hold.
// If the queue is grown, the number of messages is added to the capacity.
// If a process wants a message to fill in it trys to delete a msg from the
// capacity atomically.  If the capacity has dropped below zero, on return from the
// atomic delete, the process did not get a message.  It must try again after atomically
// incrementing the capacity.  If the capacity is zero or greater then the process has
// gotten a message.  Upon consumption of the message the capcity is incremented.
//
// Operations dealing with queue message capacity
//
// 2.2 Kernels have ATOMIC_INC_AND_TEST_GREATER_ZERO
// (This reverses the logic.  Capacity starts negative and is empty when the capacity
//   is greater than zero)
#ifdef ATOMIC_INC_AND_TEST_GREATER_ZERO
extern inline BOOLEAN_T pmacd_tryPMBQueueDelMsgFromCapacity(pmacd_pmbqueue_t *queue){
  if(ATOMIC_INC_AND_TEST_GREATER_ZERO(queue->msgCapacity)){
    // Reached capacity. Remove increment.
    ATOMIC_DEC(queue->msgCapacity);
    return(0);
  }
  return 1;
}
#define pmacd_setPMBQueueCapacity(queue, size) ATOMIC_SET((queue)->msgCapacity, -((size)))
#define pmacd_getPMBQueueCapacity(queue) -(ATOMIC_GET((queue)->msgCapacity))
#define pmacd_increasePMBQueueCapacity(queue, size)ATOMIC_SUB((queue)->msgCapacity, (size))
#define pmacd_addMsgToPMBQueueCapacity(queue) ATOMIC_DEC((queue)->msgCapacity)
#define pmacd_isPMBQueueFull(queue)(ATOMIC_GET((queue)->msgCapacity) >= 0)
#define pmacd_isPMBQueueEmpty(queue)\
         ((-ATOMIC_GET((queue)->msgCapacity)) == (queue)->numberOfMessages)
#else 

// 2.4 Kernels have ATOMIC_INC_AND_TEST_GREATER_ZERO
// This starts with the capacity positive and follows the algorithm.
#ifdef ATOMIC_ADD_NEGATIVE
extern inline BOOLEAN_T pmacd_tryPMBQueueDelMsgFromCapacity(pmacd_pmbqueue_t *queue){
  if(ATOMIC_ADD_NEGATIVE((-1),queue->msgCapacity)){
    // Reached capacity. Remove increment.
    ATOMIC_DEC(queue->msgCapacity);
    return(0);
  }
  return 1;
}
#define pmacd_setPMBQueueCapacity(queue, size) ATOMIC_SET((queue)->msgCapacity, (size))
#define pmacd_getPMBQueueCapacity(queue) ATOMIC_GET((queue)->msgCapacity)
#define pmacd_increasePMBQueueCapacity(queue, size)ATOMIC_ADD((queue)->msgCapacity, (size))
#define pmacd_addMsgToPMBQueueCapacity(queue) ATOMIC_INC((queue)->msgCapacity)
#define pmacd_isPMBQueueFull(queue)(ATOMIC_GET((queue)->msgCapacity) <= 0)
#define pmacd_isPMBQueueEmpty(queue)\
         (ATOMIC_GET((queue)->msgCapacity) == (queue)->numberOfMessages)

#else
#error "ATOMIC_ADD_NEGATIVE or ATOMIC_INC_AND_TEST_GREATER_ZERO must be defined."
#endif
#endif

/* 
 *  Get Board Number From UPMB
 *  -- retrieves the board number field from the User level pmb into
 *  -- kernel space.
 */
#define pmacd_getBoardNumberFromUPMB(retVal,pUPMB, pBoardNumber)\
  pmacd_memcpy_UtoK((retVal),\
                     (pBoardNumber),\
		     (&(((pmacd_userpmb_header_t *)pUPMB)->boardNumber)),\
                     (sizeof(pmacd_boardnumber_t)))

/* 
 *  Set Board Number In UPMB
 *  -- sets the board number of a user level pmb from kernel space.
 */
#define pmacd_setBoardNumberInUPMB(retVal, pUPMB, pBoardNumber)\
  pmacd_memcpy_KtoU((retVal),\
                     (&(((pmacd_userpmb_header_t *)pUPMB)->boardNumber)),\
		     (pBoardNumber),\
                     (sizeof(pmacd_boardnumber_t)))

/* 
 *  Copy UPMB to MF
 *  -- copy a user level PMB into a kernel message frame.
 */
#define pmacd_copyUPMBToMF(retVal, pUPMB, pMF, mfSize)\
  pmacd_memcpy_UtoK((retVal),\
		    (void *)(pMF),\
		    (void *)(((ULONG_T)pUPMB + sizeof(pmacd_userpmb_header_t))),\
                    (mfSize))

/* 
 *  Get PMB Head From PMB Queue
 *  -- retrieve a pointer to the pmb element presently on the PMB queue head.
 *  -- the element will remain in the queue. This allows for modification of
 *  -- of the element right after it has been copied to the queue.
 */   
#define pmacd_getPMBHeadFromPMBQueue(queue) ((queue)->pmbhead)

/* 
 *  Get PMB Tail From PMB Queue
 *  -- retrieve a pointer to the pmb element presently on the PMB queue tail.
 *  -- the element will remain in the queue. This allows for modification of
 *  -- of the element right before it is copied from the queue.
 */   
#define pmacd_getPMBTailFromPMBQueue(queue) ((queue)->pmbtail)

#define pmacd_copyUPMBToPMBQueue(pUPMB, pPMBQueue)\
  pmacd_copyPMBToPMBQueue(((pmacd_pmb_t *)((ULONG_T)pUPMB + sizeof(pmacd_userpmb_header_t))),\
                          (pPMBQueue),\
                          (pmacd_UserToKernel))

/* Passing kerenl buffer */
#define pmacd_copyPMBQueueToUPMB(pUPMB, pPMBQueue)\
  pmacd_copyPMBQueueToPMB(((pmacd_pmb_t *)((ULONG_T)pUPMB + sizeof(pmacd_userpmb_header_t))),\
                          (pPMBQueue),\
                          (pmacd_KernelToKernel))

#define pmacd_copyMFToPMBQueue(pMF, pPMBQueue)\
  pmacd_copyPMBToPMBQueue((pmacd_pmb_t *)(pMF),\
                          (pPMBQueue),\
                          (pmacd_KernelToKernel))

#define pmacd_copyPMBQueueToMF(pMF, pPMBQueue)\
  pmacd_copyPMBQueueToPMB((pmacd_pmb_t *)(pMF),\
                          (pPMBQueue),\
                          (pmacd_KernelToKernel))

/*
 * Function Prototypes
 */

pmacd_pmbblock_t   *pmacd_allocatePMBBlock(ULONG_T pmbSize, 
					   ULONG_T *numberOfPMBs);

void               pmacd_freePMBBlock(pmacd_pmbblock_t *pmbBlock);

pmacd_pmbqueue_t   *pmacd_allocatePMBQueue(ULONG_T pmbSize,
					   ULONG_T highWaterMark);

void               pmacd_freePMBQueue(pmacd_pmbqueue_t *queue);
BOOLEAN_T          pmacd_emptyPMBQueue(pmacd_pmbqueue_t *queue);

BOOLEAN_T          pmacd_growPMBQueue(pmacd_pmbqueue_t *queue);

BOOLEAN_T          pmacd_copyPMBToPMBQueue(pmacd_pmb_t *pmb, 
					   pmacd_pmbqueue_t *queue, 
					   pmacd_copymode_t mode);

BOOLEAN_T          pmacd_copyPMBQueueToPMB(pmacd_pmb_t *pmb, 
					   pmacd_pmbqueue_t *queue, 
					   pmacd_copymode_t mode);

ssize_t pmacd_readPMBQueue(pmacd_pmbqueue_t *queue, 
		       size_t size, 
		       char *buffer, 
		       BOOLEAN_T dontBlock);

void pmacd_printMsg(void *ppmb);
#endif /* _PMACD_PMB_H */









