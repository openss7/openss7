/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_mf.h
// 
// Defines structures and functions for working with MF's. (Message Frames)
// MFA (Message Frame Address) Queues, and MF Pools 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_MF_H
#define _PMACD_MF_H

#include "pmacd.h"

#define PMACD_INBOUND_MFA_OFFSET 0x40;
#define PMACD_OUTBOUND_MFA_OFFSET 0x44;

/* 
 *  MFs
 *  -- These are the Message Frames where the PMBs are placed for
 *     transfer between the board and host.
 *  -- It is a shared memory area (between the board and host)
 *  -- Includes an extra type for handling blocks of MFs.
 */
typedef UINT8_T pmacd_mf_t[];
typedef pmacd_mf_t *pmacd_mfblock_t;

typedef pmacd_memaddress_t pmacd_mfa_t;

/*
 *  MFPool
 *  -- A pool of MFs for transferring PMB's to/from the boards.  
 *     Used for encapsalating the MF transfer protocol.
 */
typedef struct{
  int size;
  int count;
  pmacd_mf_t *lastMFRetrieved;
  pmacd_mf_t *lastMFReturned;
  pmacd_mf_t *mfBlockStart;
  dma_addr_t  mfBlockStartBusAddr;
  pmacd_memaddress_t mfaQueueAddress;
}pmacd_mfpool_t;


typedef struct{
  MUTEX_T mutex;
  int size;             // Size of queue set at init time.
  ATOMIC_T count;       // Number of dirty entries
  pmacd_mfa_t *block;
  pmacd_mfa_t *tail;
  pmacd_mfa_t *head;
}pmacd_mfaqueue_t;

/*
 * Function Prototypes
 */
pmacd_mfpool_t    *pmacd_allocateMFPool(int numberOfMFs, 
					int sizeOfMFs); 
void              pmacd_freeMFPool(pmacd_mfpool_t *mfpool);
void              pmacd_returnMFToPool(pmacd_mfpool_t *mfPool, 
				       pmacd_mf_t *mf);
pmacd_mf_t        *pmacd_getMFFromPool(pmacd_mfpool_t *mfPool);


extern inline BOOLEAN_T pmacd_initMfaQueue(pmacd_mfaqueue_t *queue,
					    int size){
  ATOMIC_SET(queue->count, 0);
  queue->size  = size;
  queue->head  = queue->tail = queue->block =
    (pmacd_mfa_t *)pmacd_malloc(sizeof(pmacd_mfa_t)*size);
  if(queue->head == NULL)return(FALSE);
  INIT_MUTEX(queue->mutex);
  return(TRUE);
}

#define pmacd_cleanupMfaQueue(queue)\
  if((queue)->block != NULL){\
   pmacd_free((queue)->block, sizeof(pmacd_mfa_t)*((queue)->size));} 
 
#define pmacd_isMfaQueueEmpty(queue)\
  (ATOMIC_GET((queue)->count) == 0)

#define pmacd_isMfaQueueFull(queue)\
  (ATOMIC_GET((queue)->count) == (queue)->size)

#define pmacd_addToMfaQueue(queue, mfa)\
  *((queue)->tail) = (pmacd_mfa_t)(mfa);\
  if(((queue)->tail) == &(((queue)->block)[((queue)->size)-1]))\
      ((queue)->tail)=((queue)->block);\
  else ((queue)->tail)++;\
  ATOMIC_INC((queue)->count)

#ifdef LFS
#define pmacd_getFromMfaQueue(queue, mfa)\
  (mfa) = (pmacd_mf_t *)(*((queue)->head));\
  if(((queue)->head) == &(((queue)->block)[((queue)->size)-1]))\
      ((queue)->head)=((queue)->block);\
  else ((queue)->head)++;\
  ATOMIC_DEC((queue)->count)
#else
#define pmacd_getFromMfaQueue(queue, mfa)\
  (mfa) = (pmacd_mfa_t)(*((queue)->head));\
  if(((queue)->head) == &(((queue)->block)[((queue)->size)-1]))\
      ((queue)->head)=((queue)->block);\
  else ((queue)->head)++;\
  ATOMIC_DEC((queue)->count)
#endif

#define pmacd_emptyMfaQueue(queue)\
  (queue)->head = (queue)->tail = (queue)->block;\
  ATOMIC_SET((queue)->count, 0)

#endif /* _PMACD_MF_H */









