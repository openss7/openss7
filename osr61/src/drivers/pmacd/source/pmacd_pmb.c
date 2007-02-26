/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_pmb.c
// 
// Defines structures and functions for working with PMB's and PMB queues.
// (PMAC Message Blocks)
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_pmb.h"

/*
 *  FunctionName:  pmacd_allocatePMBBlock()
 *
 *        Inputs:  pmbSize - size of PMBs to allocate
 *
 *       Outputs:  numberOfPMBs - number of PMBs allocated in the block.
 *  
 *       Returns:  pointer to a block of newly allocated PMBs.
 *                 NULL - if the block of PMBS could not be allocated.
 *
 *   Description:  allocates a contiguous block of PMBs.  The block size is 
 *                 decided to be the most efficient for the platform.
 *                 
 *      Comments:  In Linux the block size is equal to a single page of memory.
 */
pmacd_pmbblock_t *pmacd_allocatePMBBlock(ULONG_T pmbSize, ULONG_T *numberOfPMBs){
  pmacd_pmbblock_t *pmbblock;
  *numberOfPMBs = PMACD_PMBBLOCK_SIZE/pmbSize;
  pmbblock = (pmacd_pmbblock_t *)pmacd_mallocPageBlock(PMACD_PMBBLOCK_SIZE);
  return(pmbblock);
}

/*
 *  FunctionName:  pmacd_freePMBBlock()
 *
 *        Inputs:  *pmbBlock - pointer to a block of PMBs to free.
 *
 *       Outputs:  none.
 *  
 *       Returns:  TRUE  - PMBBlock was freed.
 *                 FALSE - freeing PMBBlock failed.
 *
 *   Description:  free a contiguous block of PMBs, which were allocated
 *                 with pmacd_allocatePMBBlock().
 *
 *      Comments:  Only pmbBlocks allocated with pmacd_allocatePMBBlock()
 *                 should be passed into this function for freeing.
 */
void pmacd_freePMBBlock(pmacd_pmbblock_t *pmbBlock){
  pmacd_freePageBlock((pmacd_memaddress_t)pmbBlock, PMACD_PMBBLOCK_SIZE);
}

// Helper Functions
void pmacd_freePMBList(pmacd_linklist_t *plist){
  pmacd_linklist_t *plist2;
  
  // Free pmbblocks and list nodes.
  while(plist != NULL){
    if(plist->element != NULL){
      pmacd_freePMBBlock((pmacd_pmbblock_t *)plist->element);
    }
    plist2 = plist;
    plist = plist->next;
    pmacd_free(plist2, sizeof(pmacd_linklist_t));
  }
}

/*
 *  FunctionName:  pmacd_allocatePMBQueue()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  pointer to a newly allocated pmbqueue.
 *                 NULL - if the pmbqueue could not be allocated.
 *
 *   Description:  allocates a queue for storing PMBs with 1 block of PMBs
 *                 stored initially.
 *
 *      Comments:
 */
pmacd_pmbqueue_t *pmacd_allocatePMBQueue(ULONG_T pmbSize, 
					 ULONG_T highWaterMark){

  pmacd_pmbqueue_t *queue;
  pmacd_linklist_t *list;
  pmacd_pmbblock_t *pmbblock;
  ULONG_T pmbsPerBlock; 
  
  // Allocate memory for Queue.
  queue = (pmacd_pmbqueue_t *)pmacd_malloc(sizeof(pmacd_pmbqueue_t));
  list = (pmacd_linklist_t *)pmacd_malloc(sizeof(pmacd_linklist_t));
  pmbblock = pmacd_allocatePMBBlock(pmbSize, &pmbsPerBlock);
  
  // Check if the memory was allocated.
  if( (queue==NULL)||(list==NULL)||(pmbblock==NULL) ){
    if(queue != NULL) pmacd_free(queue, sizeof(pmacd_pmbqueue_t));
    if(list != NULL) pmacd_free(list, sizeof(pmacd_linklist_t));
    if(pmbblock !=NULL){
      pmacd_freePMBBlock(pmbblock);
    }
    pmacd_printError("pmacd_allocatePMBQueue:Could not allocate PMBQueue.\n");
    return(NULL);
  }
  
  // Setup pmbblock list with a single pmbblock element.
  list->element = pmbblock;
  list->next = list;
  queue->pmbSize = pmbSize;
  queue->pmbsPerBlock = pmbsPerBlock;
  queue->highWaterMark = highWaterMark;
  queue->numberOfBlocks = 1;
  queue->numberOfMessages = pmbsPerBlock;
  pmacd_setPMBQueueWrapAround(queue, 1);
  pmacd_setPMBQueueCapacity(queue, pmbsPerBlock);
  INIT_WAITQUEUE(queue->readers);
  
  // Setup queue head/tail pointers.
  queue->blockhead = queue->blocktail = list;
  queue->pmbhead = (pmacd_pmb_t *)queue->blockhead->element;  
  queue->pmbtail = queue->pmbhead;
  INIT_MUTEX(queue->readMutex);
  INIT_MUTEX(queue->writeMutex);
  return(queue);
}


/*
 *  FunctionName:  pmacd_freePMBQueue()
 *
 *        Inputs:  queue - pmbqueue to free.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none. - this functions can not fail.
 *
 *   Description:  frees the memory allocated by a PMB Queue.
 *
 *      Comments: 
 */
void pmacd_freePMBQueue(pmacd_pmbqueue_t *queue){
  pmacd_linklist_t *plist;
  
  if(queue == NULL){
    pmacd_printError("pmacd_freePMBQueue:Asked to free a NULL queue.\n");
    return;
  }
  
  // Wait for any current operations to finish. No new processes should be
  // able to access this queue.  Any new messages will be dropped by the
  // router.
  MUTEX_ENTER(queue->readMutex);
  MUTEX_ENTER(queue->writeMutex);
  if(pmacd_getPMBQueueCapacity(queue) != queue->numberOfMessages){
    pmacd_printDebug(
       "pmacd_freePMBQueue:Freeing a queue with %i messages.\n",
	(queue->numberOfMessages - pmacd_getPMBQueueCapacity(queue)));
  }
  
  // Break circular list into a null terminated linear list.
  plist = queue->blockhead;
  queue->blockhead = plist->next;
  plist->next = NULL;
  
  // Free pmbblocks, list nodes, and queue.
  pmacd_freePMBList(queue->blockhead);
  pmacd_free(queue, sizeof(pmacd_pmbqueue_t));
  MUTEX_EXIT(queue->writeMutex);
  MUTEX_EXIT(queue->readMutex);

  return;
}

/*
 *  FunctionName:  pmacd_emptyPMBQueue()
 *
 *        Inputs:  queue - pmbqueue to empty.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none. - this functions can not fail.
 *
 *   Description:  drops all messages on the Pmb queue.
 *
 *      Comments:  This function must be read and write mutex protected.
 *                 Only one process should be in this function for a 
 *                 particular queue at a time.
 */
BOOLEAN_T pmacd_emptyPMBQueue(pmacd_pmbqueue_t *queue){

  if(queue == NULL){
    pmacd_printError("pmacd_emptyPMBQueue:Asked to empty a NULL queue.\n");
    return(FALSE);
  }

  // Wait for any outstanding operations to finish.
  MUTEX_ENTER(queue->readMutex);
  MUTEX_ENTER(queue->writeMutex);

  // Reinitialize wraparound.
  pmacd_setPMBQueueWrapAround(queue, 
			      queue->numberOfBlocks);

  // Mark queue Empty
  pmacd_setPMBQueueCapacity(queue, 
			    queue->pmbsPerBlock * queue->numberOfBlocks); 
  
  // Setup queue head/tail pointers.
  queue->blocktail = queue->blockhead;
  queue->pmbhead = (pmacd_pmb_t *)queue->blockhead->element;  
  queue->pmbtail = queue->pmbhead;

  MUTEX_EXIT(queue->writeMutex);
  MUTEX_EXIT(queue->readMutex);
  return(TRUE);
}
 
/*
 *  FunctionName:  pmacd_growPMBQueue()
 *
 *        Inputs:  queue - queue to add a new block to.
 *
 *       Outputs:  none.
 *  
 *       Returns:  TRUE  - on success
 *                 FALSE - on failure
 *
 *   Description:  grows the queue by one block of messages.
 *
 *      Comments:  This function must be read and write mutex protected.
 *                 Only one process should be in this function for a 
 *                 particular queue at a time.
 *
 */
BOOLEAN_T pmacd_growPMBQueue(pmacd_pmbqueue_t *queue){
  pmacd_linklist_t *newlist = NULL;
  pmacd_pmbblock_t *pmbblock = NULL;
  ULONG_T pmbsPerBlock;
  
  if(queue->numberOfBlocks == queue->highWaterMark){
    pmacd_printDebug("pmacd_growPMBQueue: Reached highwater mark, queue is full.\n");
    return(FALSE);
  }
  
  // Build new list Segment.
  newlist =(pmacd_linklist_t *)pmacd_malloc(sizeof(pmacd_linklist_t));
  pmbblock = pmacd_allocatePMBBlock(queue->pmbSize, &pmbsPerBlock);
  if( (newlist==NULL)||(pmbblock==NULL) ) {
    pmacd_printError("pmacd_growPMBQueue:Not enough memory to grow.");
    if(newlist != NULL) pmacd_free(newlist, sizeof(pmacd_linklist_t));
    if(pmbblock !=NULL)pmacd_freePMBBlock(pmbblock);
    return(FALSE);
  }
  
  // Add segment to the tail of the queue.
  newlist->element = pmbblock;
  newlist->next = queue->blocktail->next;
  queue->blocktail->next = newlist;
  
  // Update message and block counts.
  queue->numberOfBlocks++;
  queue->numberOfMessages += pmbsPerBlock;
  pmacd_increasePMBQueueCapacity(queue, pmbsPerBlock);
  pmacd_incrPMBQueueWrapAround(queue);
  return(TRUE);
}

/*
 *  FunctionName:  pmacd_copyPMBToPMBQueue()
 *
 *        Inputs:  pmb   - pointer to pmb to copy
 *                 queue - pointer to pmbqueue to copy into
 *                 mode  - mode to copy in (UserToKernel ...)
 *
 *       Outputs:  None.
 *  
 *       Returns:  TRUE  - copy was successful.
 *                 FALSE - copy failed.
 *
 *   Description:  Copy a PMB into a PMBQueue.
 *
 *      Comments:  The function must be mutex protected from 
 *                 multiple writers accessing the queue.  If 
 *                 the queue is grown it will grab the read
 *                 mutex to keep out readers also.
 */
BOOLEAN_T pmacd_copyPMBToPMBQueue(pmacd_pmb_t *pmb, pmacd_pmbqueue_t *queue, pmacd_copymode_t mode){
  pmacd_pmb_t *pmbtail;
  int retValue;
  
  if(queue == NULL || pmb == NULL || mode > pmacd_KernelToKernel){
    pmacd_printDebug("pmacd_copyPMBToPMBQueue:Invalid parameter passed in to copy.\n");
    return(FALSE);
  }
  
  pmbtail = queue->pmbtail;
  if(!pmacd_tryPMBQueueDelMsgFromCapacity(queue)){
    // No clean message buffers, queue is full.
    pmacd_printError("pmacd_copyPMBToPMBQueue:Queue is full. Could not copy message to queue.\n");
    return(FALSE);
  }
  
  // Copy pmb into queue  
  if(mode == pmacd_KernelToKernel){
    pmacd_memcpy_KtoK(&retValue, pmbtail, pmb, queue->pmbSize);
  }else{
    pmacd_memcpy_UtoK(&retValue, pmbtail, pmb, queue->pmbSize);
  }
  
  if(retValue){
    // If the return value is not zero, the copy failed. Return error and leave tail 
    // pointer where it was.  
    pmacd_addMsgToPMBQueueCapacity(queue);
    pmacd_printError("pmacd_copyPMBToPMBQueue:Copy message to queue failed.\n");
    return(FALSE);
  }
  
  // Move tail pointers and check if we need to grow the queue.
  // Check if we are at the end of a block.
  if(pmbtail == (pmacd_pmb_t *)((UINT8_T *)queue->blocktail->element + PMACD_PMBBLOCK_SIZE - queue->pmbSize)){
    if(pmacd_decrPMBQueueWrapAroundAndTestForZero(queue)){
      // About to move into a new block. Queue almost full (won't be able to
      // grow queue if tail and head are in the same block with tail before
      // head. Must try and grow it now.)
      //pmacd_printDebug("pmacd_copyPMBToPMBQueue:Queue almost full after copy, growing queue.\n");
      
      // If this fails and pmbtail catches pmbhead, msgs will be lost.
      // This error will be caught before the message is written above.
      // We must block readers before we grow. 
      // (We should already be blocking writers.
      pmacd_growPMBQueue(queue);
    }
    queue->blocktail = queue->blocktail->next;
    pmbtail = (pmacd_pmb_t *)queue->blocktail->element;
  }else{
    pmbtail = (pmacd_pmb_t *)((UINT8_T *)pmbtail + queue->pmbSize);
  }
  
  queue->pmbtail = pmbtail;
  return(TRUE);
}


/*
 *  FunctionName:  pmacd_copyPMBQueueToPMB()
 *
 *        Inputs:  pmb   - pointer to pmb buffer to copy into
 *                 queue - pointer to pmbqueue to copy from
 *                 mode  - mode to copy in (UserToKernel ...)
 *
 *       Outputs:  None.
 *  
 *       Returns:  TRUE  - copy was successful.
 *                 FALSE - copy failed.
 *
 *   Description:  Copy a PMB from a PMBQueue.
 *
 *      Comments:  The function must be mutex protected from 
 *                 multiple readers accessing the queue.
 */
BOOLEAN_T pmacd_copyPMBQueueToPMB(pmacd_pmb_t *pmb, pmacd_pmbqueue_t *queue, pmacd_copymode_t mode){
  pmacd_pmb_t *pmbhead;
  int retValue;
  
  if(queue == NULL || pmb == NULL || mode < pmacd_KernelToKernel){
    pmacd_printDebug("pmacd_copyPMBQueueToPMB:NULL parameter passed in to copy.\n");
    return(FALSE);
  }
  
  pmbhead = queue->pmbhead;
  
  // Check if queue is empty
  if(pmacd_isPMBQueueEmpty(queue)){
    pmacd_printDebug("pmacd_copyPMBQueueToPMB:Tried to copy from an empty queue.\n");
    return(FALSE);
  }
  
  // Copy pmb from queue and move the head pointers.
  if(mode == pmacd_KernelToKernel){
    pmacd_memcpy_KtoK(&retValue, pmb, pmbhead, queue->pmbSize);
  }else{
    pmacd_memcpy_KtoU(&retValue, pmb, pmbhead, queue->pmbSize);
  }
  
  if(retValue){
    pmacd_printError("pmacd_copyPMBQueueToPMB:Copying from queue failed.\n");
    return(FALSE);
  }
  
  
  if(pmbhead == (pmacd_pmb_t *)((UINT8_T *)(queue->blockhead->element) + PMACD_PMBBLOCK_SIZE - queue->pmbSize)){
    // We are at the end of a block
    pmacd_incrPMBQueueWrapAround(queue);
    
    // Increment head pointers
    queue->blockhead = queue->blockhead->next;
    pmbhead = (pmacd_pmb_t *)queue->blockhead->element;
    
  }else{
    // In the middle or beginning of a block
    // Increment Head pointers
    pmbhead = (pmacd_pmb_t *)((UINT8_T *)pmbhead + queue->pmbSize);   
  }
  queue->pmbhead = pmbhead;
  pmacd_addMsgToPMBQueueCapacity(queue);
  return(TRUE);
}


/*
 *  FunctionName:  pmacd_readQueue()
 *
 *        Inputs:  queue - pmbqueue to read from.
 *                 size - size of buffer to be copied into.
 *                 dontBlock - if queue is empty should we block.
 *
 *       Outputs:  buffer - buffer to copy head pmb to.
 *  
 *       Returns:  ssize_t >= 0 number of bytes copied from the queue. (should be size)
 *                 ssize_t <  0 error condition
 *                 -EAGAIN = Queue is empty and the function was asked not to block.
 *                 -EINTR  = While blocked waiting for a message the function 
 *                           was interrupted by
 *                           a signal.  No message will be returned.
 *                 -EFAULT = Memory access error when trying to copy msg into user buffer.
 *
 *   Description:  reads the head pmb from the pmbqueue into a user 
 *                 pmb buffer.
 *
 *      Comments: 
 */
ssize_t pmacd_readPMBQueue(pmacd_pmbqueue_t *queue, size_t size, 
			   char *buffer, BOOLEAN_T dontBlock){
  
    pmacd_boardnumber_t boardNumber;
    pmacd_msgheader_t *pmbheader;
    int retValue = 0;
    char *kerBuffer;
    ALLOCATE_LOCAL_WAITQUEUE;
   
    // Allocate a kernel buffer
    kerBuffer = (char*)pmacd_malloc(PMACD_MF_SIZE*2);
    if (kerBuffer == NULL) {
	pmacd_printError("pmacd_read: Unable to allocate kernel memory\n");
	return -EAGAIN;
    }
 
    // Check if queue is empty prior to grabbing mutex to avoid 
    // the overhead of competing for the MUTEX. (spinning, sleeping, etc.)
    while(1){
      // Add to wait queue first. To avoid race condition when going to
      // sleep. 
      //(Example of bad race: 
      // GotEmpty, QueueFilled, GotNotification, Sleep(Missed Notification))
      ADD_TO_WAITQUEUE(queue->readers);
      if(!pmacd_isPMBQueueEmpty(queue)){
	REMOVE_FROM_WAITQUEUE(queue->readers);
	if(!pmacd_isPMBQueueEmpty(queue)){
	  break;
	}
	ADD_TO_WAITQUEUE(queue->readers);
      }
      // Queue is empty go to sleep or return.
      if(dontBlock){
	REMOVE_FROM_WAITQUEUE(queue->readers);
        pmacd_free(kerBuffer, PMACD_MF_SIZE*2);
	return -EAGAIN;
      }
      SLEEP_ON_WAITQUEUE_INTERRUPTIBLE(queue->readers);
      
      REMOVE_FROM_WAITQUEUE(queue->readers);
      if (signal_pending(current)){
        pmacd_free(kerBuffer, PMACD_MF_SIZE*2);
	return -EINTR; /* a signal arrived */
      }
    }
    
    MUTEX_ENTER_BH(queue->readMutex);
    pmbheader = (pmacd_msgheader_t *)pmacd_getPMBHeadFromPMBQueue(queue);
    boardNumber = PMACD_DECODE_BOARDNUM(pmbheader->srcNode);
    pmbheader->srcNode &= PMACD_BOARDCOMP_MASK;
    
    if(!pmacd_copyPMBQueueToUPMB(kerBuffer, queue)) {
	retValue = -EFAULT;
 	MUTEX_EXIT_BH(queue->readMutex);
    } else {
	MUTEX_EXIT_BH(queue->readMutex);
	pmacd_memcpy_KtoU(&retValue, buffer, kerBuffer, queue->pmbSize);
    }
    
    if(!retValue)pmacd_setBoardNumberInUPMB(&retValue, buffer, &boardNumber);
    if(retValue)retValue = -EFAULT;
    else retValue = size;
        
    pmacd_free(kerBuffer, PMACD_MF_SIZE*2);
    return(retValue);
}


/*
 *  FunctionName:  pmacd_printMsg()
 *
 *        Inputs:  ppmb - pointer to a message to print.
  *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Prints a hex dump of a messsage
 *
 *      Comments: 
 */

void pmacd_printMsg(void *ppmb){
  unsigned char *pmbMsg;
  int i;

  pmbMsg = (unsigned char *)ppmb;
  i = ((pmacd_msgheader_t *)(pmbMsg))->length;
  pmacd_printDebug("\n");
  while(i){
    pmacd_printDebug("%02x ", (unsigned int)*pmbMsg);
    if(!(i%8))pmacd_printDebug("\n");
    pmbMsg++;
    i--;
  }
  pmacd_printDebug("\n\n");
}









