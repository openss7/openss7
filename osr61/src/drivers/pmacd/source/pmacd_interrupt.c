/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_interrupt.c
//
// Defines functions that deal with pmac interrupt handling.
/////////////////////////////////////////////////////////////////////////////
#include "pmacd_interrupt.h"
#include "pmacd_driver.h"
#include "pmacd_linux_entry.h"
#include "21554.h"

//extern pmacd_driver_t pmacd_driver;

/*
 *  FunctionName:  pmacd_handleInterrupt()
 *
 *        Inputs:  pBoard - pointer to a board that the interrupt may have
 *                          occured on.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Handles pmacd interrupts for a single board.
 *                 
 *      Comments:  
 */
#ifdef LINUX24
void pmacd_handleInterrupt(int irq, void *pBoard, struct pt_regs *regs)
#else
irqreturn_t pmacd_handleInterrupt(int irq, void *pBoard, struct pt_regs *regs)
#endif
{
  pmacd_board_t *board = (pmacd_board_t *)pBoard;
  volatile UINT16_T doorbellStatus, i2oStatus, flags;
  ULONG_T baseAddr = 
    (ULONG_T)board->bus.csrMappedBaseAddress;
    
  // Check for interrupts
#ifdef LFS
  doorbellStatus  = readw((caddr_t)baseAddr+PRIMARY_CLEAR_IRQ_REG);
  i2oStatus = readw((caddr_t)baseAddr+I2O_OUTBOUND_POST_STATUS_REG);
#else
  doorbellStatus  = readw(baseAddr+PRIMARY_CLEAR_IRQ_REG);
  i2oStatus = readw(baseAddr+I2O_OUTBOUND_POST_STATUS_REG);
#endif
  

  if(!( (i2oStatus & I2O_OUTBOUND_IRQ_STATUS_BIT) ||
	(doorbellStatus & (PMACD_CMD_DOORBELL|PMACD_PANIC_DOORBELL)))){
    return IRQ_NONE; // Not our interrupt
  }
  
  // Handle command response interrupt
  if(doorbellStatus & PMACD_CMD_DOORBELL){
    pmacd_control_t *control = &(board->control);
    
#ifdef LFS
    control->response = readl((caddr_t)baseAddr+SCRATCHPAD_3_REG);
    control->respArg1 = readl((caddr_t)baseAddr+SCRATCHPAD_4_REG);
    control->respArg2 = readl((caddr_t)baseAddr+SCRATCHPAD_5_REG);
#else
    control->response = readl(baseAddr+SCRATCHPAD_3_REG);
    control->respArg1 = readl(baseAddr+SCRATCHPAD_4_REG);
    control->respArg2 = readl(baseAddr+SCRATCHPAD_5_REG);
#endif
    
    if(pmacd_isControlCommandActive(control)){
      // Clear timeout timer.
      del_timer(&(control->responseTimer));
#ifdef LINUX24
      tasklet_hi_schedule(&(control->handleResponseTask));
#else
      schedule_work(&(control->handleResponseTask));
#endif
    }else{
      pmacd_printError("Board %i: Recieved spurious response 0x%lx (0x%lx, 0x%lx)\n",
		       board->parms.boardId,
		       (ULONG_T)control->response, 
		       (ULONG_T)control->respArg1, 
		       (ULONG_T)control->respArg2);    
    }
  }
  
  // Handle panic interrupt
  if(doorbellStatus & PMACD_PANIC_DOORBELL){
    pmacd_printError("Board %i: Recieved Panic interrupt.\n",
		     board->parms.boardId);
  }
  
  // Clear interrupts If they were enabled.
  doorbellStatus &= PMACD_CMD_DOORBELL|PMACD_PANIC_DOORBELL;
#ifdef LFS
  if(doorbellStatus)writew(doorbellStatus, (caddr_t)baseAddr+PRIMARY_CLEAR_IRQ_REG);
#else
  if(doorbellStatus)writew(doorbellStatus, baseAddr+PRIMARY_CLEAR_IRQ_REG);
#endif

  
  // If there are any messages schedule BH to process and route them.
  if(i2oStatus & I2O_OUTBOUND_IRQ_STATUS_BIT){
    
    // Shut off I2O interrupt until after bottom half runs.
    flags = I2O_OUTBOUND_IRQ_MASK_BIT;
#ifdef LFS
    writew(flags, (caddr_t)baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#else
    writew(flags, baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#endif
#ifdef LINUX24
    tasklet_hi_schedule(&(board->getMessagesTask));
#else
    schedule_work(&(board->getMessagesTask));
#endif
  }    
  return IRQ_HANDLED;
}

/*
 *  FunctionName:  pmacd_routeBoardMessages()
 *
 *        Inputs:  pBoard - pointer to a board that has messages to
 *                          be processed.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Handles pmacd interrupts for a single board.
 *                 
 *      Comments:  
 */
void pmacd_routeBoardMessages(void *pBoard){
  pmacd_board_t *board = (pmacd_board_t *) pBoard;
  UINT8_T address;
  pmacd_pmbqueue_t *queue;
  pmacd_mfpool_t *mfPool;
  pmacd_mf_t *mf;
  pmacd_msgheader_t *pmbheader;
  int retValue;
  volatile UINT16_T flags;
  struct timeval tv;
 
  mfPool =  board->outboundMFs;
  mf = pmacd_getMFFromPool(mfPool);
  while(mf != (pmacd_mf_t *)NULL){
    pmbheader = (pmacd_msgheader_t *)mf;
    address = pmbheader->destNode;
    if ( pmbheader->srcNode & 0x40 ) {
	do_gettimeofday(&tv);
	pmacd_printDebug("pmacd_routeBoardMessages: pmbheader->srcNode =0x%x, %9d:%06d\n", 			pmbheader->srcNode, tv.tv_sec, tv.tv_usec); 
    }
    if(address == PMACD_DRIVER_ADDRESS){
      pmacd_decodeDriverMessage(board, (pmacd_pmb_t *)mf);
    }else{
      address &= ~PMACD_HOSTBOARD_FLAG;
      queue = pmacd_driver.queues[address];
      if(queue == NULL){
	//pmacd_printDebug("pmacd_routeBoardMessages: message dropped. Invalid queue 0x%x.\n", pmbheader->destNode);
      }else{
        pmbheader->srcNode = 
          PMACD_ENCODE_BOARDNUM(pmbheader->srcNode, board->parms.boardId);
	MUTEX_ENTER(queue->readMutex);
	MUTEX_ENTER(queue->writeMutex);
	retValue = pmacd_copyMFToPMBQueue(mf, queue); 
	MUTEX_EXIT(queue->writeMutex);
	MUTEX_EXIT(queue->readMutex);
	WAKEUP_WAITQUEUE(queue->readers);
      }
    }
    pmacd_returnMFToPool(mfPool, mf); 
    mf = pmacd_getMFFromPool(mfPool);
  }
  
  // Enable Board I2O outbound message interrupt
  flags = 0;
  writew(flags, 
  	 board->bus.csrMappedBaseAddress +
	 I2O_OUTBOUND_POST_IRQ_MASK_REG);
}
  
  
/*
 *  FunctionName:  pmacd_enableInterrupts
 *
 *        Inputs:  pBoard - pointer to the board to enable interrupts on.
 *
 *       Outputs:  interrupts are enabled.
 *  
 *       Returns:  TRUE = success.
 *                 FALSE = registering interrupt failed.
 *
 *   Description:  Enables interrupts, but verifies first that they have 
 *                 not already been enabled.
 *                 
 *      Comments:  
 */
BOOLEAN_T pmacd_enableInterrupts(pmacd_board_t *board){
  volatile UINT16_T flags;
  ULONG_T baseAddr = 
    (ULONG_T)board->bus.csrMappedBaseAddress;
  pmacd_control_t *control = &(board->control);
  
  if(pmacd_areControlInterruptsEnabled(control))return(TRUE);
  MUTEX_ENTER(control->modifyIntMutex);
  if(pmacd_areControlInterruptsEnabled(control)){
    MUTEX_EXIT(control->modifyIntMutex);
    return(TRUE);
  }

  // Register Interrupt Handler
  if(pmacd_register_irq(board)){
    // Didn't get requested interrupt.  Interrupts not enabled.
    MUTEX_EXIT(control->modifyIntMutex);
    pmacd_printError("Failed to register IRQ %i for board %i.",
		     board->bus.irq,
		     board->parms.boardId);
    return(FALSE);
  }
  pmacd_setControlInterruptsEnabled(control);
     
  // Enable Board Doorbell Interrupts
  flags = PMACD_CMD_DOORBELL|PMACD_PANIC_DOORBELL;
#ifdef LFS
  writew(flags, (caddr_t)baseAddr+PRIMARY_CLEAR_IRQ_MASK_REG);
#else
  writew(flags, baseAddr+PRIMARY_CLEAR_IRQ_MASK_REG);
#endif
  
  // Enable Board I2O outbound message interrupt
  flags = 0;
#ifdef LFS
  writew(flags, (caddr_t)baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#else
  writew(flags, baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#endif
  MUTEX_EXIT(control->modifyIntMutex);
  return(TRUE);
}

/*
 *  FunctionName:  pmacd_disableInterrupts()
 *
 *        Inputs:  pBoard - pointer to a board that should be added to the 
 *                          driver
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Disables interrupts, but verifies first that they have 
 *                 not already been disabled.
 *             
 *      Comments:  
 */
void pmacd_disableInterrupts(pmacd_board_t *board){
  volatile UINT16_T flags;
  ULONG_T baseAddr = 
    (ULONG_T)board->bus.csrMappedBaseAddress;
  pmacd_control_t *control = &(board->control);
   
  if(!pmacd_areControlInterruptsEnabled(control))return;
  MUTEX_ENTER(control->modifyIntMutex);
  if(!pmacd_areControlInterruptsEnabled(control)){
    MUTEX_EXIT(control->modifyIntMutex);
    return;
  }
  
  // Clear interrupts enabled flag
  pmacd_clearControlInterruptsEnabled(control);

  // Disable Doorbell Interrupts
  flags = PMACD_CMD_DOORBELL|PMACD_PANIC_DOORBELL;
#ifdef LFS
  writew(flags, (caddr_t)baseAddr+PRIMARY_SET_IRQ_MASK_REG);
#else
  writew(flags, baseAddr+PRIMARY_SET_IRQ_MASK_REG);
#endif
  
  // Clear any spurious doorbell interrupts
  flags = PMACD_CMD_DOORBELL|PMACD_PANIC_DOORBELL;
#ifdef LFS
  writew(flags, (caddr_t)baseAddr+PRIMARY_CLEAR_IRQ_REG);
#else
  writew(flags, baseAddr+PRIMARY_CLEAR_IRQ_REG);
#endif

  // Disable I2O outbound queue interrupt
  flags = I2O_OUTBOUND_IRQ_MASK_BIT;
#ifdef LFS
  writew(flags, (caddr_t)baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#else
  writew(flags, baseAddr+I2O_OUTBOUND_POST_IRQ_MASK_REG);
#endif
  
  // Remove Interrupt Handler
  free_irq(board->bus.irq, board);
  
  MUTEX_EXIT(control->modifyIntMutex);
}


/*
 *  FunctionName:  pmacd_sendMessagesToBoard()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *                 
 *
 *      Comments:  
 */  
void pmacd_sendMessagesToBoard(ULONG_T board){
  pmacd_board_t *pBoard = (pmacd_board_t *) board;
  pmacd_moveOverflowMsgsToMfs(pBoard->inboundMFs,
  			      &(pBoard->inboundMfaQueue),
  			      pBoard->overflowQueue);
  pmacd_sendMfaQueueToBoard(&(pBoard->inboundMfaQueue),
			    pBoard->inboundMFs);
  
  pBoard->sendMessagesTimer.expires = PMACD_GET_CURRENT_TICK() + 
    (pmacd_driver.parms.msgUnitConfig.inboundTimer*HZ)/1000; 
  add_timer(&(pBoard->sendMessagesTimer));
}












