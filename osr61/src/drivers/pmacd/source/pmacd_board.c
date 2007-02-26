/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_board.c
// 
//  Defines functions that deal with the pmacs at board level.
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_board.h"
#include "pmacd_interrupt.h"
#include "pmacd_linux_pci.h"
#include "21554.h"

/*
 *  FunctionName:  pmacd_allocateBoard()
 *
 *        Inputs:  boardId - boardId to allocate board with. 
 *                           (retrieved from driver)
 *                 busId - busId to allocate board with.
 *                         (retrieved from bus subsystem)
 *                 slotId - slotId to allocate board with.
 *                          (retrieved from bus subsystem)
 *                 busType - bus type of the bus the board is plugged into.
 *
 *       Outputs:  none.
 *  
 *       Returns:  pointer to the newly allocated board.
 *                 NULL upon failure to allocate board.
 *
 *   Description:  Allocates a board structure with the values initialized
 *                 as per the input parameters.
 *
 *      Comments:  
 */
pmacd_board_t *pmacd_allocateBoard(UINT8_T boardId,
				   UINT16_T busId,
				   UINT16_T slotId,
				   UINT8_T geoAddrId,
				   UINT8_T busType,
				   pmacd_msgUnitConfig_t *msgUnitConfig,
				   pmacd_streamConfig_t *streamConfig){
  pmacd_board_t *pBoard = NULL;
  
  // Allocate board structure
  pBoard =  (pmacd_board_t *)pmacd_malloc(sizeof(pmacd_board_t));
  if(pBoard == NULL){
    pmacd_printDebug("pmacd_allocateBoard: malloc failure...\n");
    return(pBoard);
  }
  // Clear board struct memory
  memset(pBoard, 0, sizeof(pmacd_board_t));

  // Allocate inbound overflow queue
  pBoard->overflowQueue = pmacd_allocatePMBQueue(msgUnitConfig->msgSize,
						 PMACD_PMBQUEUE_HIGHWATER_MARK);
  if(pBoard->overflowQueue == NULL)goto pmacd_BoardAllocation_error;

  // Allocate Inbound MFs.
  pBoard->inboundMFs = pmacd_allocateMFPool(msgUnitConfig->inboundQueueSize,
					    msgUnitConfig->msgSize);
  if(pBoard->inboundMFs == NULL)goto pmacd_BoardAllocation_error;
  
  // Allocate Outbound MFs.
  pBoard->outboundMFs = pmacd_allocateMFPool(msgUnitConfig->outboundQueueSize,
					     msgUnitConfig->msgSize);
  if(pBoard->outboundMFs == NULL)goto pmacd_BoardAllocation_error;

  // Allocate inbound MFA Queue
  if(!pmacd_initMfaQueue(&(pBoard->inboundMfaQueue), 
			 msgUnitConfig->inboundQueueSize)){
    goto pmacd_BoardAllocation_error;
  }

  // If any of the config parameters are zero streams will not be created
  if(streamConfig->streamsPerBoard &&
     streamConfig->buffersPerStream &&
     streamConfig->bufferSize){
    // Init Streams
    if(!pmacd_initStreamGroup(&(pBoard->streams), streamConfig))
      goto pmacd_BoardAllocation_error;
  }else{
    // Null/Zero out all stream parameters, because they can only be set
    // at driver load time.  One of the key parameters is zero, therefore
    // set them all to 0 to avoid any confusion that they may have been
    // allocated.
    pBoard->streams.streams = NULL;
    pBoard->streams.streamBuffers = NULL;
    streamConfig->streamsPerBoard = 0;
    streamConfig->buffersPerStream = 0;
    streamConfig->bufferSize = 0;
  }

  // Initialize the board  
  INIT_MUTEX(pBoard->mutex);
  pBoard->parms.boardId   = boardId;
  pBoard->parms.busId     = busId;
  pBoard->parms.slotId    = slotId;
  pBoard->parms.geoAddrId = geoAddrId;
  pBoard->parms.busType   = busType;
 
  init_timer(&(pBoard->sendMessagesTimer));
  pBoard->sendMessagesTimer.function = pmacd_sendMessagesToBoard;
  pBoard->sendMessagesTimer.data     = (ULONG_T)pBoard;

  init_timer(&(pBoard->resetBoardTimer));
  pBoard->resetBoardTimer.function = pmacd_hardResetBoardComplete;
  pBoard->resetBoardTimer.data     = (ULONG_T)pBoard;

#ifdef LINUX24
  tasklet_init(&(pBoard->getMessagesTask), (void *)pmacd_routeBoardMessages, (unsigned long)pBoard);
#else
  INIT_WORK(&pBoard->getMessagesTask, (void (*)(void *))pmacd_routeBoardMessages, pBoard);
#endif

  // Init control structure
  pmacd_initControl(&(pBoard->control));
  
  return(pBoard);
  
pmacd_BoardAllocation_error:
  if(pBoard != NULL){
    if(pBoard->overflowQueue != NULL)
      pmacd_freePMBQueue(pBoard->overflowQueue);
    if(pBoard->inboundMFs != NULL)
      pmacd_freeMFPool(pBoard->inboundMFs);
    if(pBoard->outboundMFs != NULL)
      pmacd_freeMFPool(pBoard->outboundMFs);
    pmacd_cleanupStreamGroup(&(pBoard->streams));
    pmacd_cleanupMfaQueue(&(pBoard->inboundMfaQueue));
    pmacd_free(pBoard, sizeof(pmacd_board_t));
    pBoard = NULL;
  }
  return(pBoard);
}

/*
 *  FunctionName:  pmacd_hardResetBoard()
 *
 *        Inputs:  pBoard - pointer to the board to reset.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the null state.
 *                 The board is physically reset and placed
 *                 in the pre initialization state.
 *
 *      Comments:  This resets the board through the PCI interface.  There is no way
 *                 to know when the board has completed the reset, therefore we set a timer
 *                 and assume that when the timer goes off we can move from the NULL state
 *                 to the Reset state.
 */
int pmacd_hardResetBoard(pmacd_board_t *pBoard, BOOLEAN_T block){
  
  int returnVal = 0;
  struct pci_dev *pciDev;  
  pmacd_control_t *control;
  UINT32_T ResetValue;
  ULONG_T pciSlotId;
 
  control = &(pBoard->control);
  pciDev =  pBoard->bus.pciDev;

  if(!pmacd_ifControlCommandNotActiveSetActive(control)) {
    pmacd_printDebug("pmacd_hardResetBoard: Failed to set control command active\n");
    return -EBUSY;
  }

  del_timer(&(pBoard->sendMessagesTimer));
  pmacd_disableInterrupts(pBoard);
  
  // Since it is a pci reset allow from any state.  
  pci_read_config_dword(pciDev, RESET_CONTROL_REG, &ResetValue);
  ResetValue = ResetValue | SECONDARY_RESET;
  pci_write_config_dword(pciDev, RESET_CONTROL_REG, ResetValue);
  udelay(10); // 10 microsecond delay
  pci_read_config_dword(pciDev, RESET_CONTROL_REG, &ResetValue);
  ResetValue = ResetValue & ~SECONDARY_RESET;
  pci_write_config_dword(pciDev, RESET_CONTROL_REG, ResetValue);

  // Set state to null, until the board cycles.
  pmacd_setControlState(control, pmacd_BoardState_Null);
  
  // Set timer to block any commands from being sent while the board is cycling.
  pBoard->resetBoardTimer.expires = PMACD_GET_CURRENT_TICK() + 
    (PMACD_HARD_RESET_TIMEOUT * PMACD_TICKS_PER_SEC);
  add_timer(&(pBoard->resetBoardTimer));
  
  if(block){
    // Blocking version: block until timer goes off.
    // add 1 second to make sure waiting is longer than the reset board
    // timer above.
    ALLOCATE_LOCAL_WAITQUEUE;
    ADD_TO_WAITQUEUE(control->waitForResponse);
    SLEEP_ON_WAITQUEUE_TIMEOUT_INTERRUPTIBLE(control->waitForResponse,
					     PMACD_HARD_RESET_TIMEOUT+1);
    // No need to check for signals.  If we wake up early because of a signal the timer
    // will still go off and move the board to the Reset state.
    REMOVE_FROM_WAITQUEUE((control->waitForResponse));
  }

  // Clear out messages that didn't get sent to the
  // board before the reset.
  MUTEX_ENTER(pBoard->inboundMfaQueue.mutex);  
  pmacd_emptyMfaQueue(&(pBoard->inboundMfaQueue));
  MUTEX_EXIT(pBoard->inboundMfaQueue.mutex);  
  pmacd_emptyPMBQueue(pBoard->overflowQueue);

  // Clear streams.
  pmacd_nullStreamGroup(&(pBoard->streams));

  // Get the Physical and Shelf Id
#ifdef LFS
  pciSlotId =  readl((caddr_t) pBoard->bus.csrMappedBaseAddress+SCRATCHPAD_7_REG);
#else
  pciSlotId =  readl((ULONG_T) pBoard->bus.csrMappedBaseAddress+SCRATCHPAD_7_REG);
#endif
  pBoard->parms.geoAddrId = (pciSlotId & 0xFFFF0000) >> 16;
  pBoard->parms.shelfId = (pciSlotId & 0x0000FFFF);
  pmacd_printDebug("pmacd: geoid= %#x shelfId=%#x  %#x\n",
                        pBoard->parms.geoAddrId, pBoard->parms.shelfId, pciSlotId); 
 
  return(returnVal);
}

/*
 *  FunctionName:  pmacd_hardResetBoardComplete()
 *
 *        Inputs:  board - board that the hard reset has been performed on.
 *
 *       Outputs:  Moves the board into the reset state.
 *  
 *       Returns:  none.
 *                 
 *
 *   Description:  This function is called when the timer for a hard reset goes off.
 *                 It updates the state of the board and marks commands inactive.
 *
 *      Comments:  
 */
void pmacd_hardResetBoardComplete(ULONG_T board){
  pmacd_board_t *pBoard = (pmacd_board_t *) board;
  pmacd_control_t *control = &(pBoard->control);

  pmacd_setControlState(control, pmacd_BoardState_Reset);
  pmacd_clearControlCommandActive(control);
}

/*
 *  FunctionName:  pmacd_sendMfaQueueToBoard()
 *
 *        Inputs:  *mfaQueue -
 *                 *mfPool   -
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  
 *                 
 *                 
 *      Comments:  
 */
void pmacd_sendMfaQueueToBoard(pmacd_mfaqueue_t *mfaQueue,
			       pmacd_mfpool_t *mfPool){
  pmacd_mf_t *mf;

  MUTEX_ENTER(mfaQueue->mutex);  
  while(!pmacd_isMfaQueueEmpty(mfaQueue)){
    pmacd_getFromMfaQueue(mfaQueue, mf);
    pmacd_returnMFToPool(mfPool, mf);
  } 
  MUTEX_EXIT(mfaQueue->mutex);
}

  
/*
 *  FunctionName:  pmacd_moveOverflowMsgsToMfs()
 *
 *        Inputs:  *mfPool        - Pool of message frames to try
 *                                  and move messages to.
 *                 *mfaQueue      - List of MF address to ship to the
 *                                  board on next transfer to the board
 *                 *overflowQueue - Messages that did not fit in the MF
 *                                  pool originally, and are trying to
 *                                  be moved into the pool now.
 *
 *       Outputs:  If there is room in the pool messages will be moved
 *                 from the overflow queue to the MF's ready to be 
 *                 consumed by the board.
 *  
 *       Returns:  none.
 *
 *   Description:  This function will try to move messages in the Overflow
 *                 queue to the the Message Frames that the board reads for
 *                 consumption.
 *                 
 *                 
 *      Comments:  This function will grap the necessary MUTEXs.
 */
void pmacd_moveOverflowMsgsToMfs(pmacd_mfpool_t *mfPool,
				  pmacd_mfaqueue_t *mfaQueue,
				  pmacd_pmbqueue_t *overflowQueue){
  pmacd_mf_t *mf;

  while(!pmacd_isPMBQueueEmpty(overflowQueue) && 
	(mf = pmacd_getMFFromPool(mfPool)) != (pmacd_mf_t *)NULL){
    pmacd_copyPMBQueueToMF(mf, overflowQueue);
    pmacd_addToMfaQueue(mfaQueue, (pmacd_mfa_t)mf);
  }
}

/*
 *  FunctionName:  pmacd_ResetSystemAndEnterBoot()
 *
 *        Inputs:  pBoard - pointer to the board to init.
 *
 *       Outputs:  none.
 *
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *                 -EIO failed to register irq
 *                 -ENOTTY wrong state
 *
 *   Description:  Move the board into the initialized state.
 *                 Also enables interrupts for the board, and
 *                 makes sure the configuration rom is upto date.
 *
 *      Comments:
 */
int pmacd_ResetSystemAndEnterBoot(pmacd_board_t *pBoard){
  int returnVal;

  pmacd_boardstate_t state
    = pmacd_getControlState(&(pBoard->control));

  if(!(state == pmacd_BoardState_Null  ||
       state == pmacd_BoardState_Reset)) {
       pmacd_printDebug("pmacd_ResetSystemAndEnterBoot: Invalid state = 0x%x\n", state);
       return -ENOTTY;
  }

  pmacd_updatePCIConfigRom(&(pBoard->bus));  
  if(!pmacd_enableInterrupts(pBoard)) {
       pmacd_printDebug("pmacd_ResetSystemAndEnterBoot: Failed to enable the interrupts\n");
       return -EIO;
 } 
  if((returnVal =
      pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus),
                                     PMACD_CMD_ENTER_BOOT, 0, 0,
                                     pmacd_BoardState_Boot))){
      pmacd_printDebug("pmacd_ResetSystemAndEnterBoot: Failed...\n");
      pmacd_disableInterrupts(pBoard);
      return returnVal;
  }
  return returnVal;
}

/*
 *  FunctionName:  pmacd_initBoard()
 *
 *        Inputs:  pBoard - pointer to the board to init.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *                 -EIO failed to register irq
 *                 -ENOTTY wrong state
 *
 *   Description:  Move the board into the initialized state.
 *                 Also enables interrupts for the board, and
 *                 makes sure the configuration rom is upto date.
 *
 *      Comments:  
 */
int pmacd_initBoard(pmacd_board_t *pBoard){
  int returnVal;
  
  pmacd_boardstate_t state 
    = pmacd_getControlState(&(pBoard->control));
  
  if(!(state == pmacd_BoardState_Null  ||
       state == pmacd_BoardState_Reset ||
       state == pmacd_BoardState_Stopped )) {
       pmacd_printDebug("pmacd_initBoard: Invalid state = 0x%x\n", state);
       return -ENOTTY;
  } 
 
  pmacd_updatePCIConfigRom(&(pBoard->bus));  
  if(!pmacd_enableInterrupts(pBoard)) {
       pmacd_printDebug("pmacd_initBoard: Failed to enable the interrupts\n");
       return -EIO;
 } 

  if((returnVal = 
      pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
				     PMACD_CMD_INIT, 0, 0,
				     pmacd_BoardState_Initialized))){
    pmacd_disableInterrupts(pBoard);
  }
  return returnVal;
}


/*
 *  FunctionName:  pmacd_configureBoard()
 *
 *        Inputs:  pBoard - pointer to the board to configure.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the configured state.
 *                 The configuration block is copied into the board
 *                 shared RAM prior to the command being sent, and
 *                 copied from shared RAM upon completion of the
 *                 command.
 *
 *      Comments:  
 */
int pmacd_configureBoard(pmacd_board_t *pBoard,
			 pmacd_msgUnitConfig_t *msgUnitConfig){
  pmacd_boardConfigBlock_t config;
  UINT8_T *bPtr;
  int i, returnVal;
  ULONG_T baseAddr;
  pmacd_boardstate_t state 
    = pmacd_getControlState(&(pBoard->control));

  if(state != pmacd_BoardState_Initialized) {
       pmacd_printDebug("pmacd_configureBoard: Invalid state = 0x%x\n", state);
       return -ENOTTY;
  }
  pmacd_setupPciBar2(&(pBoard->bus), 
		     pBoard->inboundMFs, 
		     pBoard->outboundMFs,
		     pBoard->streams.streamBuffersBusAddr,
		     pBoard->streams.bufferSize *
		     pBoard->streams.buffersPerStream *
		     pBoard->streams.numberOfStreams);
 
  config.inBoundMode      = msgUnitConfig->inboundTransferMode;
  config.hwOutBoundMode   = msgUnitConfig->outboundTransferMode;
  config.muPullOption     = msgUnitConfig->pullOption;
  config.hwOutBoundTimer  = msgUnitConfig->outboundTimer;
  config.hwAsapThreshold  = msgUnitConfig->outboundThreshold;
  config.muMsgFrameSize   = msgUnitConfig->msgSize;
  config.muFifoSize       = msgUnitConfig->outboundQueueSize;
  config.cntrUpdateRate   = 10;
  config.strmBuffersPer   = pBoard->streams.buffersPerStream;
  config.uNumStreams      = pBoard->streams.numberOfStreams;
  config.strmBufferSize   = pBoard->streams.bufferSize;

  /* Copy to shared ram */
  i = sizeof(pmacd_boardConfigBlock_t);
  bPtr = (UINT8_T *)&(config);
  baseAddr = (ULONG_T)pBoard->bus.configMappedBaseAddress;
  while(i){
#ifdef LFS
    writeb(*bPtr, (caddr_t)baseAddr);
#else
    writeb(*bPtr, baseAddr);
#endif
    baseAddr++;
    bPtr++;
    i--;
  }

  /* Copy from shared ram */
  i = sizeof(pmacd_boardConfigBlock_t);
  bPtr = (UINT8_T *)&(config);
  baseAddr = (ULONG_T)pBoard->bus.configMappedBaseAddress;
  while(i){
#ifdef LFS
    *bPtr = readb((caddr_t)baseAddr);
#else
    *bPtr = readb(baseAddr);
#endif
    baseAddr++;
    bPtr++;
    i--;
  }
  
  returnVal = pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
					PMACD_CMD_CONFIG, 0, 0,
					pmacd_BoardState_Configured);
  if(!returnVal){
    // Board is in the running state, the board can now accept messages,
    // start the send message timer for sending messages to the board.
    add_timer(&(pBoard->sendMessagesTimer));
  }
  return(returnVal);
}

/*
 *  FunctionName:  pmacd_resetBoard()
 *
 *        Inputs:  pBoard - pointer to the board to reset.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the reset state.
 *
 *      Comments:  
 */
int pmacd_resetBoard(pmacd_board_t *pBoard){
  pmacd_boardstate_t state 
    = pmacd_getControlState(&(pBoard->control));
  
  if(!(state == pmacd_BoardState_Initialized ||
       state == pmacd_BoardState_Stopped)) {
       pmacd_printDebug("pmacd_resetBoard: Invalid state = 0x%x\n", state);
       return -ENOTTY;
  } 
  return(pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
					PMACD_CMD_RESET, 0, 0,
					pmacd_BoardState_Reset));
}

/*
 *  FunctionName:  pmacd_startBoard()
 *
 *        Inputs:  pBoard - pointer to the board to start.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the running state.
 *                 All tasks on the board will be spawned on this command.
 *
 *      Comments:  
 */
int pmacd_startBoard(pmacd_board_t *pBoard){
  int returnVal;
  pmacd_boardstate_t state 
    = pmacd_getControlState(&(pBoard->control));
  
  if(!(state == pmacd_BoardState_Configured ||
       state == pmacd_BoardState_Stopped)) {
       pmacd_printDebug("pmacd_startBoard: Invalid state = 0x%x\n", state);
       return -ENOTTY;
  }

  returnVal = pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
					     PMACD_CMD_START, 0, 0,
					     pmacd_BoardState_Running);
  return(returnVal);
}

/*
 *  FunctionName:  pmacd_stopBoard()
 *
 *        Inputs:  pBoard - pointer to the board to stop.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the stopped state.
 *                 All tasks on the board will be stopped.
 *
 *      Comments:  
 */
int pmacd_stopBoard(pmacd_board_t *pBoard){
  
  if(pmacd_getControlState(&(pBoard->control)) 
     !=  pmacd_BoardState_Quiesced) {
       pmacd_printDebug("pmacd_stopBoard: getControlState failure..\n"); 
       return -ENOTTY;
  } 
  return(pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
					PMACD_CMD_STOP, 0, 0,
					pmacd_BoardState_Stopped));
}

/*
 *  FunctionName:  pmacd_quiesceBoard()
 *
 *        Inputs:  pBoard - pointer to the board to quiesce.
 *
 *       Outputs:  none.
 *  
 *       Returns:  error code upon failure.
 *                 0 upon success.
 *
 *   Description:  Move the board into the quiesced state.
 *                 All tasks on the board will be quiesced.
 *
 *      Comments:  
 */
int pmacd_quiesceBoard(pmacd_board_t *pBoard){
  int returnVal;
    
  if(pmacd_getControlState(&(pBoard->control)) 
     !=  pmacd_BoardState_Running) {
       pmacd_printDebug("pmacd_quiesceBoard: getControlstate failed..\n");
       return -ENOTTY;
  } 
  returnVal = pmacd_sendSimpleControlCommand(&(pBoard->control), &(pBoard->bus), 
					     PMACD_CMD_QUIESCE, 0, 0,
					     pmacd_BoardState_Quiesced);
  if(!returnVal){
    del_timer(&(pBoard->sendMessagesTimer));
    // Clear out messages that didn't get sent to the
    // board before the quiesce command completed.
    MUTEX_ENTER(pBoard->inboundMfaQueue.mutex);  
    pmacd_emptyMfaQueue(&(pBoard->inboundMfaQueue));
    MUTEX_EXIT(pBoard->inboundMfaQueue.mutex);  
    pmacd_emptyPMBQueue(pBoard->overflowQueue);

    if(pBoard->streams.streams != NULL){
      // Clean up streams if there are any
      pmacd_nullStreamGroup(&(pBoard->streams));
    }
  }
  
  return(returnVal);
}


/*
 *  FunctionName:  pmacd_freeBoard()
 *
 *        Inputs:  pBoard - pointer to the board to free.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Frees a board structure and resources allocated
 *                 for that board.
 *
 *      Comments:  
 */
void pmacd_freeBoard(pmacd_board_t *pBoard){
  
  MUTEX_ENTER(pBoard->mutex);
  
  pmacd_disableInterrupts(pBoard);
 
  pmacd_updatePCIConfigRom(&(pBoard->bus));
    
  // Unmap board shared memory
  iounmap(pBoard->bus.configMappedBaseAddress);
 
  iounmap(pBoard->bus.csrMappedBaseAddress);

  del_timer(&(pBoard->sendMessagesTimer));

  del_timer(&(pBoard->resetBoardTimer));

  pmacd_freeMFPool(pBoard->inboundMFs);

  pmacd_freeMFPool(pBoard->outboundMFs);

  pmacd_freePMBQueue(pBoard->overflowQueue);  
  pmacd_cleanupStreamGroup(&(pBoard->streams));

  pmacd_cleanupMfaQueue(&(pBoard->inboundMfaQueue));

  pmacd_free(pBoard, sizeof(pmacd_board_t));

  MUTEX_EXIT(pBoard->mutex);
}









