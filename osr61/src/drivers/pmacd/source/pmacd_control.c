/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_control.c
// 
//  Defines functions that work on the pmac control interface.
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_control.h"
#include "21554.h"

/*
 *  FunctionName:  pmacd_initControl()
 *
 *        Inputs:  *control - pointer to control structure to init.
 *
 *       Outputs:  none.
 *  
 *       Returns:  TRUE on success.
 *                 FALSE if control is NULL.
 *
 *   Description:  Initialize the control structure;
 *                 
 *      Comments:  
 */
BOOLEAN_T pmacd_initControl(pmacd_control_t *control){

  if(control == NULL)return(FALSE);

  INIT_MUTEX(control->modifyIntMutex);
  INIT_WAITQUEUE(control->waitForResponse);
  pmacd_setControlState(control, pmacd_BoardState_Null);
  pmacd_clearControlInterruptsEnabled(control);
  pmacd_clearControlCommandActive(control);
  control->responseTimeout = PMACD_CMD_RESPONSE_TIMEOUT;
  control->command         = PMACD_CMD_NULL;
  control->cmdArg1         = 0;
  control->cmdArg2         = 0;  
  control->response        = PMACD_CMD_NULL;
  control->respArg1        = 0;
  control->respArg2        = 0;  
  control->errorCode       = 0;
  control->successState    = pmacd_BoardState_Null;
  control->errorState      = pmacd_BoardState_Null;
  
  init_timer(&(control->responseTimer));
  control->responseTimer.function = pmacd_handleControlResponseTimeout;
  control->responseTimer.data     = (ULONG_T)control;
//#ifdef LINUX24
#if 1
  tasklet_init(&(((pmacd_control_t *)control)->handleResponseTask), (void *)pmacd_handleControlResponse, (unsigned long)control);
#else
  INIT_WORK(&control->handleResponseTask, (void (*)(void *))pmacd_handleControlResponse, control);
#endif
  return(TRUE);
}

/*
 *  FunctionName:  pmacd_sendSimpleControlCommand()
 *
 *        Inputs:  control - the control structure for tracking the command.
 *                 busDevice - the busDevice to perform the command on.
 *                 command - the control command to send.
 *                 arg1 - the first command argument.
 *                 arg2 - the second command argument.
 *                 newState - the state to move the board to if the 
 *                            command completes successfully.
 *
 *       Outputs:  none.
 *  
 *       Returns:  0 on success.
 *                 the appropriate error code on failure:
 *                 -EBUSY     = Command already active.
 *                 -ETIMEDOUT = Timed out waiting for response.
 *                 -EIO       = Got wrong or error response.
 *                 -EINTR     = Got signal while waiting for response.
 *
 *   Description:  This functions will send a control command that
 *                 expects the response to be the appropriate response 
 *                 with a single argument that contains the the response 
 *                 error code.  If the argument is 0 the command was 
 *                 successful.  If the command is successful then we have 
 *                 moved into the newState.  The board control structure 
 *                 will be updated with the command sent, the response
 *                 recieved, and the newState.
 *                 
 *      Comments:  The command will complete even if a signal is recieved.
 */
int pmacd_sendSimpleControlCommand(pmacd_control_t *control,
				   pmacd_pcibusdevice_t *busDevice,
				   UINT32_T command, 
				   UINT32_T arg1, 
				   UINT32_T arg2, 
				   pmacd_boardstate_t newState){ 
  
  UINT16_T flags;
  int returnVal;

  ULONG_T baseAddr = (ULONG_T)busDevice->csrMappedBaseAddress;
  ALLOCATE_LOCAL_WAITQUEUE;
  
  // Check if a command is already active if so return EBUSY,
  // if not grab the control MUTEX and mark the cmdActive.  This
  // way noone else will try and send a command.
  if(!pmacd_ifControlCommandNotActiveSetActive(control)) {
	return -EBUSY;
  }
 
  // Store command that we are sending in the control structure
  // for auditing purposes.  Set the response to NULL.  If it
  // is still NULL upon wake up we have timed out waiting for 
  // the response.
  control->command =   command;
  control->cmdArg1 =   arg1;
  control->cmdArg2 =   arg2;  
  control->response =  PMACD_CMD_NULL;
  control->respArg1 =   0;
  control->respArg2 =   0;  
  control->errorCode =  0;
  control->errorState = pmacd_BoardState_Failed;
  control->successState = newState;
  
  // Add us to the waitForResponse queue prior to sending the 
  // command to remove the race condition.  We will definitely 
  // be notified when the response comes back.
  ADD_TO_WAITQUEUE(control->waitForResponse);
  
  // Write command to the board and notify the board with a
  // Doorbell interrupt.  Then go to sleep waiting for the response.
#ifdef LFS
  writel(command, (caddr_t)baseAddr+SCRATCHPAD_0_REG);
  writel(arg1,    (caddr_t)baseAddr+SCRATCHPAD_1_REG);
  writel(arg2,    (caddr_t)baseAddr+SCRATCHPAD_2_REG);
  flags = PMACD_CMD_DOORBELL;
  writew(flags,   (caddr_t)baseAddr+SECONDARY_SET_IRQ_REG);
#else
  writel(command, baseAddr+SCRATCHPAD_0_REG);
  writel(arg1,    baseAddr+SCRATCHPAD_1_REG);
  writel(arg2,    baseAddr+SCRATCHPAD_2_REG);
  flags = PMACD_CMD_DOORBELL;
  writew(flags, baseAddr+SECONDARY_SET_IRQ_REG);
#endif
  
  // Set response timer.  Done as a timer to handle the case where
  // this process is interrupted by a signal.  We still need to timeout
  // if the process goes away because the command will still complete 
  // regardless if anyone is waiting for the response.
  control->responseTimer.expires = PMACD_GET_CURRENT_TICK() + 
    (PMACD_CMD_RESPONSE_TIMEOUT * PMACD_TICKS_PER_SEC);
  add_timer(&(control->responseTimer));    
  SLEEP_ON_WAITQUEUE_INTERRUPTIBLE(control->waitForResponse);
  
  // Got a response or a signal.  
  // Remove us from the waitqueue, and process the response.
  REMOVE_FROM_WAITQUEUE((control->waitForResponse));
  
  // If response is still NULL we were woken up by a signal 
  // or the timeout timer.
  if(control->response == PMACD_CMD_NULL){
    // Timed out waiting for a response or got a signal
    if (signal_pending(current)){
      pmacd_printDebug("pmacd_sendSimpleControlCommand: Interrupted by system Call\n");
      returnVal = -EINTR; /* a signal arrived */
      // Don't clear commandActive a command is still active.
      // Will be cleared upon response or timeout.
    }else{
      // No signal pending, timed out
      // Give up. Clear command active.
      pmacd_clearControlCommandActive(control);  
      pmacd_printDebug("pmacd_sendSimpleControlCommand: Timedout \n");
      returnVal =  -ETIMEDOUT;
    }
  }else if(control->errorCode){
    // An error occured. Report it back to the process.
    // Done sending and processing command, mark command inactive  
      pmacd_clearControlCommandActive(control);
      pmacd_printDebug("pmacd_sendSimpleControlCommand: Error= 0x%x\n", control->errorCode);
    returnVal =  -EIO;
  }else{
    // Everything looks good with the response.
    // Done sending and processing command, mark command inactive
    pmacd_clearControlCommandActive(control);  
    returnVal = 0;
  }
  return(returnVal);
}

/*
 *  FunctionName:  pmacd_handleControlResponseTimeout()
 *
 *        Inputs:  control - the control structure the response is tracked with.
 *
 *       Outputs:  handleControlResponse task is scheduled to run on the immediate queue.
 *  
 *       Returns:  none.
 *
 *   Description:  Function called when the response timeout timer has gone off.
 *                 This functions schedules the handleControlResponse Task to be run
 *                 on the immediate queue.  This task will handle responses, timeouts, and
 *                 waking a waiting process.
 *                 
 *      Comments:  The code for handling this is in the BH task to avoid a race condition between
 *                 the interrupt handler and the timeout timer.
 */
void pmacd_handleControlResponseTimeout(ULONG_T control){
  // Just schedule the bottom half. This way forcing a wake up,
  // whether an interrupt occured or not.
//#ifdef LINUX24
#if 1
  tasklet_hi_schedule(&(((pmacd_control_t *)control)->handleResponseTask));
#else
  schedule_work(&(((pmacd_control_t *)control)->handleResponseTask));
#endif
}


/*
 *  FunctionName:  pmacd_handleControlResponse()
 *
 *        Inputs:  control - the control structure the response is tracked with.
 *
 *       Outputs:  tasks waiting for the response are woken up.
 *                 the control structure is updated with any response or error information.
 *  
 *       Returns:  none.
 *
 *   Description:  Function is called by the interrupt handler to decode a command interface
 *                 response.  It is also called by the timeout timer. Both the timeout and the
 *                 response case are handled.  Any waiting tasks are woken up, and any responses
 *                 or errors are updated in the control structure.
 *                 
 *      Comments:  
 */
void pmacd_handleControlResponse(void *arg){
  pmacd_control_t *control = (pmacd_control_t *) arg;
 
  // If response is still NULL, a timeout occured.
  if(control->response != PMACD_CMD_NULL){
    if(control->response != control->command+1){
      // Got wrong response (Response is enumerated 1 greater than the command)
      control->errorCode = PMACD_CMD_WRONG_RSP;
      pmacd_setControlState(control, control->errorState);
    
    }else if(control->respArg1){
      // If the response argument1 is not zero, there was an error.
      // Got an error response
      control->errorCode = control->respArg1;
      pmacd_setControlState(control, control->errorState);
    }else{
      // Everything looks good with the response.
      pmacd_setControlState(control, control->successState);
    }
  }
  
  // If a process is still waiting for the response wake them up.
  if(IS_WAITQUEUE_ACTIVE(control->waitForResponse)){
    // Leave commandActive Mutex protection on so they can do any
    // extra processing on the response they need.
    WAKEUP_WAITQUEUE(control->waitForResponse);
  }else{
    // Done sending and processing command.
    // No one is waiting for the response, mark command inactive.
    pmacd_clearControlCommandActive(control);
  }
}
  









