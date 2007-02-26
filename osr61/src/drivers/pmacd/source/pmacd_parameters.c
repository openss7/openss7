/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_parameters.c
// 
//  Functions for accessing the pmac driver parameters from 
//  user space.
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_parameters.h"
#include "pmacd_driver.h"
#include "pmacd_board.h"
#include "pmacd_control.h"
#include "pmacd_pmb.h"
#include "21554.h"

// Defined in pmacd_driver.c
extern pmacd_driver_t pmacd_driver;

//
// Helper Macros for Parameter Tranfer Functions.
//
#define PMACD_PARM_GENERAL_SETUP() ULONG_T parmSize; void *pParm; pmacd_ioctlParm_t localParm
#define PMACD_PARM_CUSTOM_SETUP(ptype, p) ptype p; pParm = &(p); parmSize = sizeof(ptype)
#define PMACD_PARM_COPY_PARM_FROM_USER(retValue)pmacd_memcpy_UtoK(&retValue,&localParm, parm, sizeof(pmacd_ioctlParm_t))  
#define PMACD_PARM_COPY_VALUE_TO_USER(retValue)pmacd_memcpy_KtoU(&retValue, localParm.pValue, pParm, parmSize)
#define PMACD_PARM_COPY_VALUE_FROM_USER(retValue)pmacd_memcpy_UtoK(&retValue,pParm, localParm.pValue, parmSize)  

#define PMACD_BOARDPARM_GENERAL_SETUP() ULONG_T parmSize; void *pParm; pmacd_ioctlBoardParm_t localParm
#define PMACD_BOARDPARM_CUSTOM_SETUP(ptype, p) ptype p; pParm = &(p); parmSize = sizeof(ptype)
#define PMACD_BOARDPARM_COPY_PARM_FROM_USER(retValue)pmacd_memcpy_UtoK(&retValue,&localParm, parm, sizeof(pmacd_ioctlBoardParm_t))
#define PMACD_BOARDPARM_COPY_VALUE_TO_USER(retValue)pmacd_memcpy_KtoU(&retValue, localParm.pValue, pParm, parmSize)
#define PMACD_BOARDPARM_COPY_VALUE_FROM_USER(retValue)pmacd_memcpy_UtoK(&retValue,pParm, localParm.pValue, parmSize)  
#define PMACD_BOARDPARM_GET_BOARDID() localParm.boardId
/*
 *  FunctionName:  pmacd_getDriverParm()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  
 *                 
 *                 
 *
 *      Comments:  
 */  
int pmacd_getDriverParm(pmacd_ioctlParm_t *parm){
  int retValue; 
  PMACD_PARM_GENERAL_SETUP();
  PMACD_PARM_COPY_PARM_FROM_USER(retValue);
  if(retValue)return -EFAULT;  
    
  switch(localParm.parmId){
    
  case  pmacd_Parm_MsgUnitInboundMode:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_msgTransferMode_t, inboundMode);
    MUTEX_ENTER(pmacd_driver.mutex);
    inboundMode = 
      pmacd_driver.parms.msgUnitConfig.inboundTransferMode;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_MsgUnitOutboundMode:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_msgTransferMode_t, outboundMode);
    MUTEX_ENTER(pmacd_driver.mutex);
    outboundMode = 
      pmacd_driver.parms.msgUnitConfig.outboundTransferMode;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_MsgUnitInboundThreshold:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, inboundThreshold);
    MUTEX_ENTER(pmacd_driver.mutex);
    inboundThreshold = 
      pmacd_driver.parms.msgUnitConfig.inboundThreshold;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case  pmacd_Parm_MsgUnitOutboundThreshold:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, outboundThreshold);
    MUTEX_ENTER(pmacd_driver.mutex);
    outboundThreshold = 
      pmacd_driver.parms.msgUnitConfig.outboundThreshold;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case  pmacd_Parm_MsgUnitInboundTimer:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, inboundTimer);
    MUTEX_ENTER(pmacd_driver.mutex);
    inboundTimer = pmacd_driver.parms.msgUnitConfig.inboundTimer;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_MsgUnitOutboundTimer:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, outboundTimer);
    MUTEX_ENTER(pmacd_driver.mutex);
    outboundTimer = pmacd_driver.parms.msgUnitConfig.outboundTimer;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case pmacd_Parm_MsgSize:{
    PMACD_PARM_CUSTOM_SETUP(size_t, msgSize);
    msgSize = pmacd_driver.parms.msgUnitConfig.msgSize;
    break;
  }

  case pmacd_Parm_NumberOfBoards:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, numOfBoards);
    MUTEX_ENTER(pmacd_driver.mutex);
    numOfBoards = pmacd_driver.parms.boardCount;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_QueueHighWaterMark:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, highWaterMark);
    MUTEX_ENTER(pmacd_driver.mutex);
    highWaterMark = 
      pmacd_driver.parms.queueHighWaterMark/PMACD_PMBBLOCK_SIZE;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case pmacd_Parm_StreamBufferSize:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, streamBufferSize);
    streamBufferSize = pmacd_driver.parms.streamConfig.bufferSize;
    break;
  }

  case pmacd_Parm_BuffersPerStream:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, buffersPerStream);
    buffersPerStream = pmacd_driver.parms.streamConfig.buffersPerStream;
    break;
  }

  case pmacd_Parm_StreamsPerBoard:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, streamsPerBoard);
    streamsPerBoard = pmacd_driver.parms.streamConfig.streamsPerBoard;
    break;
  }

  case  pmacd_Parm_MsgUnitConfig:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_msgUnitConfig_t, config);
    MUTEX_ENTER(pmacd_driver.mutex);
    config = pmacd_driver.parms.msgUnitConfig;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_StreamConfig:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_streamConfig_t, config);
    MUTEX_ENTER(pmacd_driver.mutex);
    config = pmacd_driver.parms.streamConfig;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  default:  
    return -EINVAL;
  }

  PMACD_PARM_COPY_VALUE_TO_USER(retValue);
  if(retValue)return -EFAULT;    
  return(0);
}

/*
 *  FunctionName:  pmacd_setDriverParm()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  
 *                 
 *                 
 *
 *      Comments:  
 */  
int pmacd_setDriverParm(pmacd_ioctlParm_t *parm){
  int retValue; 
  PMACD_PARM_GENERAL_SETUP();
  PMACD_PARM_COPY_PARM_FROM_USER(retValue);
  if(retValue)return -EFAULT;  
   
  switch(localParm.parmId){
    
  case  pmacd_Parm_MsgUnitInboundMode:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_msgTransferMode_t, inboundMode);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.inboundTransferMode = inboundMode;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_MsgUnitOutboundMode:{
    PMACD_PARM_CUSTOM_SETUP(pmacd_msgTransferMode_t, outboundMode);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.outboundTransferMode = outboundMode;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case  pmacd_Parm_MsgUnitInboundThreshold:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, inboundThreshold);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.inboundThreshold =
      inboundThreshold;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case  pmacd_Parm_MsgUnitOutboundThreshold:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, outboundThreshold);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.outboundThreshold =
      outboundThreshold;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case  pmacd_Parm_MsgUnitInboundTimer:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, inboundTimer);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.inboundTimer = 
      inboundTimer;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }

  case  pmacd_Parm_MsgUnitOutboundTimer:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, outboundTimer);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.msgUnitConfig.outboundTimer = 
      outboundTimer;
    MUTEX_EXIT(pmacd_driver.mutex);
    break; 
  }

  case  pmacd_Parm_QueueHighWaterMark:{
    PMACD_PARM_CUSTOM_SETUP(ULONG_T, highWaterMark);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;    
    MUTEX_ENTER(pmacd_driver.mutex);
    pmacd_driver.parms.queueHighWaterMark =
      highWaterMark;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case pmacd_Parm_MsgSize:
  case pmacd_Parm_NumberOfBoards:
  case pmacd_Parm_MsgUnitConfig:
    // READ ONLY PARAMETERS.
    return -EPERM;
    
  default:  
    return -EINVAL;
  }
  
  return(0);
}


/*
 *  FunctionName:  pmacd_getBoardParm()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  
 *                 
 *                 
 *
 *      Comments:  
 */  
int pmacd_getBoardParm(pmacd_ioctlBoardParm_t *parm){
  int retValue; 
  pmacd_boardId_t boardId;
  pmacd_board_t *pBoard;

  PMACD_BOARDPARM_GENERAL_SETUP();
  PMACD_BOARDPARM_COPY_PARM_FROM_USER(retValue);
  if(retValue)return -EFAULT;  
  
  boardId = PMACD_BOARDPARM_GET_BOARDID();
  pBoard =  pmacd_driver.boardmap[boardId];
  if(pBoard == NULL) return -EINVAL;
  
  switch(localParm.parmId){
    
  case  pmacd_Parm_BoardState:{
    PMACD_BOARDPARM_CUSTOM_SETUP(pmacd_boardstate_t, state);
    state = pmacd_getControlState(&(pBoard->control));
    PMACD_BOARDPARM_COPY_VALUE_TO_USER(retValue);
    if(retValue)return -EFAULT;    
    break;
  }
  
  case pmacd_Parm_PhysicalSlotInfo:{
    PMACD_BOARDPARM_CUSTOM_SETUP(pmacd_physicalSlotInfo_t, info);
    info.busId   = pBoard->parms.busId;
    info.slotId  = pBoard->parms.slotId;
    info.geoAddr = pBoard->parms.geoAddrId;
    info.busType = pBoard->parms.busType;
    info.irq     = pBoard->bus.irq;
    PMACD_BOARDPARM_COPY_VALUE_TO_USER(retValue);
    if(retValue)return -EFAULT;    
    break;
  }
  
  default:  
    return -EINVAL;
  }
  
  return(0);
}

/*
 *  FunctionName:  pmacd_setBoardParm()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  
 *                 
 *                 
 *
 *      Comments:  
 */  
int pmacd_setBoardParm(pmacd_ioctlBoardParm_t *parm){
  int retValue; 
  pmacd_boardId_t boardId;
  pmacd_board_t *pBoard;
  unsigned char Value ;

  PMACD_BOARDPARM_GENERAL_SETUP();
  PMACD_BOARDPARM_COPY_PARM_FROM_USER(retValue);
  if(retValue)return -EFAULT;  
  
  boardId = PMACD_BOARDPARM_GET_BOARDID();
  pBoard =  pmacd_driver.boardmap[boardId];
  if(pBoard == NULL) return -EINVAL;
  
  switch(localParm.parmId){
   
  case pmacd_Parm_BoardLed: 
  {
    PMACD_PARM_CUSTOM_SETUP(UCHAR_T, parameter);
    PMACD_PARM_COPY_VALUE_FROM_USER(retValue);
    if(retValue)return -EFAULT;  

       switch(parameter){
	case PMACD_BOARD_LED_OFF:
        pci_read_config_byte(pBoard->bus.pciDev, HOTSWAP_CONTROL_REG, &Value);
	Value &=~HSCSR_LED_LO;
        pci_write_config_byte(pBoard->bus.pciDev, HOTSWAP_CONTROL_REG, Value);
        pci_read_config_byte(pBoard->bus.pciDev, HOTSWAP_CONTROL_REG, &Value);
  	break;

	case PMACD_BOARD_LED_ON:
        pci_read_config_byte(pBoard->bus.pciDev, HOTSWAP_CONTROL_REG, &Value);
	Value |= HSCSR_LED_LO;
        pci_write_config_byte(pBoard->bus.pciDev, HOTSWAP_CONTROL_REG, Value);
  	break;
   	
	default:

    		return -EINVAL;
	}	
    }
	break;

  case pmacd_Parm_BoardState:
  case pmacd_Parm_PhysicalSlotInfo:
    // READ ONLY PARAMETERS
    return -EPERM;
  
  case  pmacd_Parm_BoardParmsStop:{
    // This is here as an example it will fall through
    // to the default error case.  It is also here to keep
    // the compiler from complaining, since we don't have
    // any board parameters that can be set at this time.
    ULONG_T realparm;
    PMACD_BOARDPARM_CUSTOM_SETUP(ULONG_T, highWaterMark);
    //PMACD_BOARDPARM_COPY_VALUE_FROM_USER(retValue);
    //if(retValue)return -EFAULT;    
    // MUTEX_ENTER(pmacd_driver.mutex);
    //pmacd_driver.parms.queueHighWaterMark =
    realparm = highWaterMark;
    // MUTEX_EXIT(pmacd_driver.mutex);
  }

  default:  
    return -EINVAL;
  }
 
  return(0);
}




