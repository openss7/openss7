/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_driver.c
//
// Implementation of Functions which work with pmacd_driver_t.
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_driver.h"
#include "pmacd_linux_pci.h"
#include "pmacd_interrupt.h"

// The pmacd_driver_t is a sigleton (only one allowed in the system)
pmacd_driver_t pmacd_driver;

/*
 *  FunctionName:  pmacd_initDriver()
 *
 *        Inputs:  msgSize - Size of the pmacd messages and MF frames.
 *                 pullOption - Where the inbound MF's live. (presently only on host).
 *                 inboundQueueSize - Number of MF Frames for inbound messages.
 *                 outboundQueueSize - Number of MF Frames for outbound messages.
 *                 streamBufferSize - Size in bytes of each stream buffer.
 *                 bufferPerStream - number of buffers per stream.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Initializes the singleton driver structure.
 *                 No memory allocation, therefore this always succeedes.
 *                 
 *      Comments:  
 */
void pmacd_initDriver(ULONG_T msgSize,
		      pmacd_MFPullOptionValues_t  pullOption,
		      ULONG_T inboundQueueSize,
		      ULONG_T outboundQueueSize,
		      ULONG_T streamBufferSize,
		      ULONG_T buffersPerStream,
		      ULONG_T streamsPerBoard){
  int i;

  // Initialize all parameters that are configured only at driver load.
  pmacd_driver.parms.msgUnitConfig.msgSize           = msgSize;
  pmacd_driver.parms.msgUnitConfig.pullOption        = pullOption;
  pmacd_driver.parms.msgUnitConfig.inboundQueueSize  = inboundQueueSize;
  pmacd_driver.parms.msgUnitConfig.outboundQueueSize = outboundQueueSize;
  pmacd_driver.parms.streamConfig.bufferSize         = streamBufferSize;
  pmacd_driver.parms.streamConfig.buffersPerStream   = buffersPerStream;
  pmacd_driver.parms.streamConfig.streamsPerBoard    = streamsPerBoard;

  // Set to default values for runtime changeable parameters
  pmacd_driver.parms.msgUnitConfig.inboundTransferMode  = PMACD_TRANSFER_MODE;
  pmacd_driver.parms.msgUnitConfig.outboundTransferMode = PMACD_TRANSFER_MODE;
  pmacd_driver.parms.msgUnitConfig.inboundThreshold     = PMACD_INBOUNDMF_THRESHOLD;
  pmacd_driver.parms.msgUnitConfig.outboundThreshold    = PMACD_OUTBOUNDMF_THRESHOLD;
  pmacd_driver.parms.msgUnitConfig.inboundTimer         = PMACD_TRANSFER_TIMER;
  pmacd_driver.parms.msgUnitConfig.outboundTimer        = PMACD_TRANSFER_TIMER;
  pmacd_driver.parms.queueHighWaterMark                 = PMACD_PMBQUEUE_HIGHWATER_MARK;

  // Non-Configurable parameters
  pmacd_driver.parms.maxBoards   = PMACD_MAX_BOARDS;
  pmacd_driver.parms.maxQueues   = PMACD_MAX_QUEUES;
  pmacd_driver.parms.maxBoardId  = PMACD_MAX_BOARDID;
  pmacd_driver.parms.lastBoardId = -1;
  pmacd_driver.parms.lastQueueAddress = -1;
  pmacd_driver.parms.boardCount  = 0;
  ATOMIC_SET(pmacd_driver.parms.queueCount, 0);
  INIT_MUTEX(pmacd_driver.mutex);
  for(i = 0; i <= PMACD_MAX_BOARDID; i++){
    pmacd_driver.boardmap[i] = (pmacd_board_t *)NULL;
  }
  for(i = 0; i < PMACD_MAX_QUEUES; i++){
    pmacd_driver.queues[i] = (pmacd_pmbqueue_t *)NULL;
  }
}


/*
 *  FunctionName:  pmacd_addBoardToDriver()
 *
 *        Inputs:  pBoard - pointer to a board that should be added to the 
 *                          driver
 *
 *       Outputs:  none.
 *  
 *       Returns:  TRUE = successfully added.
 *                 FALSE = could not allocate linklist item.
 *
 *   Description:  Inserts a board structure into the pmacd_driver board list
 *                 and board map.
 *                 
 *      Comments:  
 */
BOOLEAN_T pmacd_addBoardToDriver(pmacd_board_t *pBoard){
  pmacd_driver.boardmap[pBoard->parms.boardId] = pBoard;
  pmacd_driver.parms.boardCount++;
  return(TRUE);
}

/*
 *  FunctionName:  pmacd_addQueueToDriver()
 *
 *        Inputs: pQueue - pointer to queue structure to add to the driver queue array.  
 *
 *       Outputs: none
 *  
 *       Returns: none
 *
 *   Description:   places a queue in the driver queue address map.  The address slot
 *                  should already be filled in the queue before calling this function.
 *                 
 *      Comments:  Does not check for address clashing.  This address should have been
 *                 retrieved from getFreeQueueAddress.  And both that call and this call
 *                 should be under Driver Mutex protection.
 */
void pmacd_addQueueToDriver(pmacd_pmbqueue_t *pQueue){
  pmacd_driver.queues[pQueue->queueAddress] = pQueue;
  ATOMIC_INC(pmacd_driver.parms.queueCount);
}


/*
 *  FunctionName:  pmacd_getFreeQueueAddress()
 *
 *        Inputs:  none
 *
 *       Outputs:  none
 *  
 *       Returns:  queueAddress - a free queue address to be assigned.
 *                 On Error == PMACD_MAX_QUEUES (no free queue addresses)
 *
 *   Description:  This function returns a free queue address that can be assigned
 *                 to a queue created by the calling client.
 *                 
 *                 
 *      Comments:  This function must be Driver mutex protected before being called.
 */
UINT8_T pmacd_getFreeQueueAddress(){
  UINT8_T queueAddress;
  int count;
  
  // Increment to new queue.
  queueAddress = pmacd_driver.parms.lastQueueAddress + 1;
  if(queueAddress == PMACD_MAX_QUEUES)queueAddress = 0;

  for(count = 0; count < PMACD_MAX_QUEUES; count++){
    if(pmacd_driver.queues[queueAddress]){
      queueAddress++;
      if(queueAddress == PMACD_MAX_QUEUES)queueAddress = 0;
    }else{
      break;
    }
  }
  pmacd_driver.parms.lastQueueAddress = queueAddress;
  if(count == PMACD_MAX_QUEUES)queueAddress = PMACD_MAX_QUEUES;
  return(queueAddress);
}

/*
 *  FunctionName:  pmacd_getFreeBoardId()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  boardId - returns a valid board id for a slot in the board map that
 *                           is not presently in use.
 *                 On Error == PMACD_MAX_BOARDS (no free boardIds)
 *
 *   Description:  Scans the board map and returns a board Id for the first
 *                 open slot.
 *                 
 *      Comments:  This function must be Driver mutex protected before being called.
 */
UINT8_T pmacd_getFreeBoardId(){
  UINT8_T boardId;
  int count;
  
  boardId = pmacd_driver.parms.lastBoardId + 1;
  if(boardId == PMACD_MAX_BOARDS)boardId = 0;
  
  for(count = 0; count <= PMACD_MAX_BOARDID; count++){
    if(pmacd_driver.boardmap[boardId] != NULL){
      boardId++;  
      if(boardId == PMACD_MAX_BOARDS)boardId = 0;
    }else{
      break;
    }
  }
  

  if(count == PMACD_MAX_BOARDS){
    // No free board Ids
    boardId = PMACD_MAX_BOARDS;
  }else{
    // Free board Id found. Set lastBoardId to boardId.
    pmacd_driver.parms.lastBoardId = boardId;
  }  
  return(boardId);
}



/*
 *  FunctionName:  pmacd_printBoards()
 *
 *        Inputs:  print information for all the boards handled by this 
 *                 driver.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Scans the board map and prints out all the present
 *                 boards information.
 *                 
 *      Comments:  
 */
void pmacd_printBoards(void){
  pmacd_board_t *pBoard;
  int i;

  MUTEX_ENTER(pmacd_driver.mutex);
  for(i = 0; i<=PMACD_MAX_BOARDID ; i++){
    pBoard = pmacd_driver.boardmap[i];
    
    if(pBoard !=NULL){
      pmacd_printDebug("Board#:%i Bus#:%i Slot#:%i BusType: PCI\n",
		       pBoard->parms.boardId,
		       pBoard->parms.busId,
		       pBoard->parms.slotId);
      pmacd_printPCIBusDev(&(pBoard->bus));
    }
  }
  MUTEX_EXIT(pmacd_driver.mutex);
}


/*
 *  FunctionName:  pmacd_freeBoardsFromDriver()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Scans the board map and frees  all boards found.
 *                 
 *      Comments:  
 */
void pmacd_freeBoardsFromDriver(void){
  pmacd_board_t *pBoard;
  int i;

  MUTEX_ENTER(pmacd_driver.mutex);
  for(i = 0; i<=PMACD_MAX_BOARDID ; i++){
    pBoard = pmacd_driver.boardmap[i];
  
    if(pBoard !=NULL){
      pmacd_freeBoard(pBoard);
      pmacd_driver.boardmap[i] = NULL;
      pmacd_driver.parms.boardCount--;
    }
  }
  MUTEX_EXIT(pmacd_driver.mutex);
}

/*
 *  FunctionName:  pmacd_freeBoardFromDriver()
 *
 *        Inputs:  pciDev - pointer to pci device of the board to free.
 *
 *       Outputs:  if the board is matched it is freed.
 *  
 *       Returns:  none.
 *
 *   Description:  Scans the board map for the given pciDev and frees it
 *                 if it is found.
 *                 
 *      Comments:  
 */
void pmacd_freeBoardFromDriver(struct pci_dev *pciDev){
  pmacd_board_t *pBoard;
  int i;
  
  MUTEX_ENTER(pmacd_driver.mutex);
  for(i = 0; i<=PMACD_MAX_BOARDID ; i++){
    pBoard = pmacd_driver.boardmap[i];
  
    if(pBoard !=NULL){
      if(pBoard->bus.pciDev == pciDev){
	
  
  if (pBoard->streams.streamBuffersBusAddr) 
	pci_unmap_single(pciDev, pBoard->streams.streamBuffersBusAddr, pmacd_driver.parms.streamConfig.bufferSize, PCI_DMA_BIDIRECTIONAL);
  if (pBoard->outboundMFs->mfBlockStartBusAddr) 
	pci_unmap_single(pciDev, pBoard->outboundMFs->mfBlockStartBusAddr, (pmacd_driver.parms.msgUnitConfig.outboundQueueSize * pmacd_driver.parms.msgUnitConfig.msgSize), PCI_DMA_BIDIRECTIONAL);
  if (pBoard->inboundMFs->mfBlockStartBusAddr) 
	pci_unmap_single(pciDev, pBoard->inboundMFs->mfBlockStartBusAddr, (pmacd_driver.parms.msgUnitConfig.inboundQueueSize * pmacd_driver.parms.msgUnitConfig.msgSize), PCI_DMA_BIDIRECTIONAL);

	pmacd_freeBoard(pBoard);
	pmacd_driver.boardmap[i] = NULL;
	pmacd_driver.parms.boardCount--;
      }
    }
  }
  MUTEX_EXIT(pmacd_driver.mutex);
}


/*
 *  FunctionName:  pmacd_reportBoards()
 *
 *        Inputs:  report->numberOfBoards - number of report board buffers
 *                 report->boardReports - array of report board buffers
 *
 *       Outputs:  report->numberOfBoards - number of report board buffers
 *                                           filled in.
 *                 report->boardReports - array of report board buffers 
 *                                           filled in.
 *       Returns:  errorcode upon failure.
 *                 0 upon success.
 *                 -EFAULT = memory access error when copying board report to user space.
 *                 -EINVAL = report buffer sent in not big enough for board list.
 *
 *   Description:  Reports all the boards present at the time of the
 *                 function call.
 *                 
 *      Comments:  
 */
int pmacd_reportBoards(pmacd_reportBoards_t *report){
  pmacd_board_t *pBoard;
  pmacd_boardreport_t *pBoardReport = report->boardReports;
  pmacd_boardreport_t boardReport;
  ULONG_T boardCount = 0;
  int retValue;
  int i;

  pmacd_memcpy_UtoK(&retValue,&boardCount,&(report->numberOfBoards), sizeof(ULONG_T));
  if(retValue)return -EFAULT;
  
  if(boardCount < pmacd_driver.parms.boardCount){
    return -EINVAL;
  }
  
  boardCount = pmacd_driver.parms.boardCount;
    
  for(i = 0; i <= PMACD_MAX_BOARDID; i++){
    MUTEX_ENTER(pmacd_driver.mutex);

    pBoard = pmacd_driver.boardmap[i];
    
    if(pBoard !=NULL){
      boardReport.boardId   = pBoard->parms.boardId;
      boardReport.busId     = pBoard->parms.busId;
      boardReport.slotId    = pBoard->parms.slotId;
      boardReport.geoAddr   = pBoard->parms.geoAddrId;
      boardReport.busType   = pBoard->parms.busType;
      boardReport.state     = pmacd_getControlState(&(pBoard->control));
      MUTEX_EXIT(pmacd_driver.mutex);
      pmacd_memcpy_KtoU(&retValue, pBoardReport, &(boardReport), sizeof(pmacd_boardreport_t));
      if(retValue){
	return -EFAULT;
      }
      pBoardReport++;
    } else {
      MUTEX_EXIT(pmacd_driver.mutex);
    }
  }
  pmacd_memcpy_KtoU(&retValue,&(report->numberOfBoards),&boardCount, sizeof(ULONG_T));
  if(retValue)return -EFAULT;
  return(0);
}


/*
 *  FunctionName:  pmacd_queueDriverMessage()
 *
 *        Inputs:  boardNumber - message destination board Id.
 *                 buffer - pointer to message buffer of message to send.
 *                 size - size of message to copy from buffer.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ssize_t >= 0 number of bytes copied to the queue. (should be size)
 *                 ssize_t <  0 error condition
 *                 -EINVAL = boardNumber out of range or invalid (No board at that Id)
 *                 -ENOTTY = board not in the Running State. (Messages can only be sent when
 *                           the board is in the Running State.)
 *                 -EFAULT = Memory access error when trying to copy buffer to board queue.
 *                 
 *
 *   Description:  Copies the driver message buffer to the inbound board queue of the
 *                 the board with the BoardNumber boardId.
 *                 
 *                 
 *
 *      Comments:  
 */  
ssize_t pmacd_queueDriverMessage(pmacd_boardnumber_t boardNumber,
				 UINT8_T *buffer,
				 size_t size){
  pmacd_board_t *pBoard;
  pmacd_mfpool_t *mfPool;
  pmacd_mf_t *mf;
  pmacd_pmbqueue_t *overflowQueue;
  int retValue;
  
  if(boardNumber > PMACD_MAX_BOARDID){
    return -EINVAL;
  }
  
  pBoard = pmacd_driver.boardmap[boardNumber];
  if(pBoard == NULL)return -EINVAL;

  if(pmacd_getControlState(&(pBoard->control)) 
     != pmacd_BoardState_Running)return -ENOTTY;
  
  mfPool = pBoard->inboundMFs;
  overflowQueue = pBoard->overflowQueue;
  
  // Move any overflow queue messages to the mfs first.
  if(!pmacd_isPMBQueueEmpty(pBoard->overflowQueue)){
    MUTEX_ENTER_BH(pBoard->inboundMfaQueue.mutex);
    pmacd_moveOverflowMsgsToMfs(mfPool, 
				&(pBoard->inboundMfaQueue),
				pBoard->overflowQueue);
    MUTEX_EXIT_BH(pBoard->inboundMfaQueue.mutex);
    
  }

  // If all of the overflow queue messages were moved over
  // try and copy the new message into a MF.
  if(pmacd_isPMBQueueEmpty(pBoard->overflowQueue) &&
     ((mf = pmacd_getMFFromPool(mfPool)) != (pmacd_mf_t *)NULL)){
    // got an MFcopy directly to MF.
    pmacd_memcpy_KtoK(&retValue, mf, buffer, size);
    if(retValue)return -EFAULT;
    
    MUTEX_ENTER_BH(pBoard->inboundMfaQueue.mutex);
    pmacd_addToMfaQueue(&(pBoard->inboundMfaQueue), (pmacd_mfa_t)mf);
    MUTEX_EXIT_BH(pBoard->inboundMfaQueue.mutex);
    // If we are in ASAP mode we need to check if we have reached
    // the threshold.

    if(pmacd_driver.parms.msgUnitConfig.inboundTransferMode ==
       pmacd_MsgTransferMode_ASAP){	
      if(pmacd_driver.parms.msgUnitConfig.inboundThreshold == 
	 ATOMIC_GET(pBoard->inboundMfaQueue.count)){
	del_timer(&(pBoard->sendMessagesTimer));
	pmacd_sendMessagesToBoard((ULONG_T)pBoard);
      }
    }
  }else{
    // No more MFs copy to overflow queue.
    MUTEX_ENTER(overflowQueue->writeMutex);
    if(!pmacd_copyPMBToPMBQueue((pmacd_pmb_t *)buffer,
    				overflowQueue,
    				pmacd_KernelToKernel)){
      MUTEX_EXIT(overflowQueue->writeMutex);
      return -EFAULT;
    }
    MUTEX_EXIT(overflowQueue->writeMutex);
  }    
  return(size);
}

/*
 *  FunctionName:  pmacd_decodeDriverMessage()
 *
 *        Inputs:  board - pointer to the board the message is from.
 *                 msg - buffer containing the driver message to be decoded.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Decode all messages addressed to the driver.
 *
 *      Comments:  
 */  
void pmacd_decodeDriverMessage(pmacd_board_t *board,
				  pmacd_pmb_t *msg){
  int i;

  // Setup msgHeader pointer
  pmacd_msgheader_t *msgHeader = (pmacd_msgheader_t *)msg;
  
  // Setup commandHeader to point to the 
  // first command after the msgHeader
  pmacd_commandHeader_t *cmdHeader = 
    (pmacd_commandHeader_t *)(((UINT8_T *)msg)+sizeof(pmacd_msgheader_t));
  
  for(i = msgHeader->numberOfCommands; i ; i--){
    switch(cmdHeader->command){
    
    case PMACD_CMD_MODIFY_RESPONSE:{
      // Stream command response
      pmacd_stream_t *stream = 
	&(board->streams.streams[pmacd_getStreamIdxFromUserCmdId(msgHeader->userCmdId)]);

      if(cmdHeader->numberOfDescs != 0){
	// An error occurred, decode it; Move past cmdHeader to descriptors.
	pmacd_errorDesc_t *error = 
	  (pmacd_errorDesc_t *)(((UINT8_T *)cmdHeader)+sizeof(pmacd_commandHeader_t));
	
	// Store the error
	pmacd_setStreamError(stream, error->errorCode);
      }else{
	// Command was successful. Move stream to next state.
	int state = pmacd_getNewStreamStateFromUserCmdId(msgHeader->userCmdId);
	
	if(state == pmacd_StreamState_Null){
	  // Clear attached parameters
	  stream->boardId          = 0;
	  stream->contextId        = 0;
	  stream->termId           = 0;
	  stream->streamAttributes = 0;
	  stream->mmapAddress      = NULL;
	  pmacd_clearStreamMapped(stream);
	  pmacd_clearStreamError(stream);
	  pmacd_clearStreamCommandActive(stream);
	  if(stream->group != NULL){
	    ATOMIC_DEC(stream->group->activeStreams);
	  }

	}
	
	pmacd_setStreamState(stream, state);
      }
      
      // Mark stream command inactive. (got a response, whether errored or not)
      pmacd_clearStreamCommandActive(stream);
      
    }      
    break;
      
    case PMACD_CMD_EXITNOTIFY_RESPONSE:
      break;
      
    default:
      break;
    }
    
    cmdHeader++;
  }
}
  

/*
 *  FunctionName:  pmacd_ExitNotification
 *
 *        Inputs:  boardNumber
 * 		   Source Node                
 *
 *       Outputs:  None 
 *  
 *       Returns:  Success, if 0  
 *
 *   Description:  Called by Device close() 
 *
 *      Comments:  Fails if the board is not in Running state.
 *                 Invalid boardnumber  
 */ 
int pmacd_ExitNotification(pmacd_boardnumber_t boardNumber, UINT8_T srcNode)
{
  int msgSize = pmacd_driver.parms.msgUnitConfig.msgSize;
  UINT8_T msgBuf[msgSize];
  pmacd_attachRequestMsg_t *attachMsg = (pmacd_attachRequestMsg_t *)msgBuf;
  int retVal = 0;

  attachMsg->msgHeader.flags.msgType    = 0x00;
  attachMsg->msgHeader.flags.emergency  = 0x00;
  attachMsg->msgHeader.flags.priority   = 0x00;
  attachMsg->msgHeader.flags.extensionHdr  = 0x00;
  attachMsg->msgHeader.flags.version    = 0x00;
  attachMsg->msgHeader.length           = sizeof(pmacd_commandHeader_t) +
					  sizeof(pmacd_msgheader_t);
  attachMsg->msgHeader.srcNode          = srcNode;//PMACD_DRIVER_ADDRESS;
  attachMsg->msgHeader.destNode         = PMACD_CALLCONTROL_NODE;
  attachMsg->msgHeader.contextId        = PMACD_ALL_CONTEXT_ID;
  attachMsg->msgHeader.reserved         = 0x00;
  attachMsg->msgHeader.numberOfCommands = 0x01;
  attachMsg->cmdHeader.command          = PMACD_CMD_EXITNOTIFY_REQUEST;
  attachMsg->cmdHeader.length           = sizeof(pmacd_commandHeader_t);
  attachMsg->cmdHeader.numberOfTerms    = 0x00;
  attachMsg->cmdHeader.numberOfDescs    = 0x00;
  attachMsg->cmdHeader.termId    	= 0x00;

  if ((retVal = pmacd_queueDriverMessage(boardNumber, msgBuf, msgSize)) < 0){
     pmacd_printDebug(" pmacd_ExitNotification: Failed\n");
  }
  return(retVal);
}

