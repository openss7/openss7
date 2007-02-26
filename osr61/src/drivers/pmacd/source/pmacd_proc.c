/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_proc.c
// 
//  Proc interface for retrieving information about the driver.
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_parameters.h"
#include "pmacd_driver.h"
#include "pmacd_board.h"
#include "pmacd_control.h"
#include "pmacd_pmb.h"

#define PMACD_PROC_SIZE_LIMIT (PAGE_SIZE-80)
// Defined in pmacd_driver.c
//extern pmacd_driver_t pmacd_driver;
// Defined in pmacd_linux_entry.c
extern ATOMIC_T pmacd_dynamicMemoryUsage;

/*
 *  FunctionName:  pmacd_procDriverInfo()
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

////////////////////////////////////////
// PROC getinfo interface changed
//  between 2.2. and 2.4
////////////////////////////////////////
// Kernels prior to 2.4
int pmacd_procDriverInfo(char *buf, char **start, off_t offset, int len);
int pmacd_procDriverInfo(char *buf, char **start, off_t offset, int len){
////////////////////////////////////////

  pmacd_driverparms_t *parms = &(pmacd_driver.parms);
  pmacd_pcibusdevice_t *pci;
  pmacd_board_t *pBoard;
  pmacd_pmbqueue_t *pQueue;
  pmacd_stream_t *stream;
  UINT8_T *sbuf;
  int i,j;
  len=0;

  len += sprintf(buf+len,
		 "\nDriver:\n"
		   "-------\n"
		 "  numberOfBoards = %i\n"
		 "  numberOfQueues = %i\n"
		 "  queueHighWaterMark = %li(bytes)\n"
		 "  lastQueueAddressAssigned = 0x%x\n"
		 "  lastBoardIdAssigned = 0x%x\n"
		 "  dynamicMemoryUsage = %li(bytes)\n",
		 parms->boardCount,
		 ATOMIC_GET(parms->queueCount),
		 parms->queueHighWaterMark*PMACD_PMBBLOCK_SIZE,
		 (parms->lastQueueAddress | PMACD_HOSTBOARD_FLAG),
		 parms->lastBoardId,
		 ATOMIC_GET(pmacd_dynamicMemoryUsage));

  
  len += sprintf(buf+len,
		 "\nMsg Unit Parms:\n"
		 "---------------\n"
		 "  msgSize = %li(bytes)\n"
		 "  pullOption = %i(%i=AllOnHost %i=OutBoundOnHost)\n"
		 "  inbound:   transferMode = %i(%i=Batch;%i=ASAP)\n"
		 "             queueSize = %li(msgs)\n"
                 "             threshold = %li(msgs)\n"
		 "             timer = %li(ms)\n"
		 "  outbound:  transferMode = %i(%i=Batch;%i=ASAP)\n"
		 "             queueSize = %li(msgs)\n"
                 "             threshold = %li(msgs)\n"
		 "             timer = %li(ms)\n",
		 parms->msgUnitConfig.msgSize,
		 parms->msgUnitConfig.pullOption,
		 (int)pmacd_MFPullOption_AllOnHost,
		 (int)pmacd_MFPullOption_OutboundOnHost,
		 parms->msgUnitConfig.inboundTransferMode,
		 (int)pmacd_MsgTransferMode_Batch,
		 (int)pmacd_MsgTransferMode_ASAP,
		 parms->msgUnitConfig.inboundQueueSize,
		 parms->msgUnitConfig.inboundThreshold,
		 parms->msgUnitConfig.inboundTimer,
		 parms->msgUnitConfig.outboundTransferMode,
		 (int)pmacd_MsgTransferMode_Batch,
		 (int)pmacd_MsgTransferMode_ASAP,
		 parms->msgUnitConfig.outboundQueueSize,
		 parms->msgUnitConfig.outboundThreshold,
		 parms->msgUnitConfig.outboundTimer);

  len += sprintf(buf+len,
		 "\nStream Parms:\n"
		   "-------------\n"
		 "  streamBufferSize  = %li(bytes)\n"
		 "  buffersPerStream  = %li\n"
		 "  streamsPerBoard   = %li\n",
		 parms->streamConfig.bufferSize,
		 parms->streamConfig.buffersPerStream,
		 parms->streamConfig.streamsPerBoard);
  
  len += sprintf(buf+len,"\nBoards:\n"
		           "_______\n");  

  for(i = 0; i<=PMACD_MAX_BOARDID ; i++){
    pBoard = pmacd_driver.boardmap[i];
    if(pBoard !=NULL){
      
      len += sprintf(buf+len,
		     "Board#:%i Bus#:%i Slot#:%i Physical Slot#:%i Shelf#:%i BusType: PCI\n",
		     pBoard->parms.boardId,
		     pBoard->parms.busId,
		     pBoard->parms.slotId,
		     pBoard->parms.geoAddrId,
		     pBoard->parms.shelfId);

      
      pci=&(pBoard->bus);
      len += sprintf(buf+len, 
		     "  IRQ = %i\n"
		     "  CSRBaseAddr = 0x%lx\n"
		     "  ConfigBaseAddr = 0x%lx\n"
		     "  BoardState = %i\n"
		     "  CommandActive = %i (0=No 1=Yes)\n"
		     "  Command = %i\n"
		     "  Response = %i\n"
		     "  InboundMfaQueue: Size = %li(entries)\n"
		     "                   Dirty = %li(entries)\n"
		     "  InboundMFs:  LastMFRetrieved = 0x%lx\n"
		     "               LastMFReturned  = 0x%lx\n"
		     "  OutboundMFs: LastMFRetrieved = 0x%lx\n"
		     "               LastMFReturned  = 0x%lx\n"
		     "  OverflowQueue: IsQueueEmpty = %i (0=False 1=True)\n"
		     "                 IsQueueFull = %i (0=False 1=True)\n"
		     "                 WrapAround = %i (0=Wrapped)\n"
		     "                 MsgCount = %i\n"
		     "                 CleanMsgs = %i\n"
		     "                 NumberOfBlocks = %i\n"
		     "                 BlockSize = %i(bytes)\n"
		     "                 BlockHighWatermark = %i\n"
		     "                 MsgsPerBlock = %i\n"
		     "  ActiveStreamCount = %i\n",
		     
		     pci->irq,
		     pci->csrBaseAddress,
		     pci->configBaseAddress,
		     pmacd_getControlState(&(pBoard->control)),
		     pmacd_isControlCommandActive(&(pBoard->control)),
		     pBoard->control.command,
		     pBoard->control.response,
		     pBoard->inboundMfaQueue.size,
		     pBoard->inboundMfaQueue.count,
		     (ULONG_T)(pBoard->inboundMFs->lastMFRetrieved),
		     (ULONG_T)(pBoard->inboundMFs->lastMFReturned),
		     (ULONG_T)(pBoard->outboundMFs->lastMFRetrieved),
		     (ULONG_T)(pBoard->outboundMFs->lastMFReturned),
		     pmacd_isPMBQueueEmpty(pBoard->overflowQueue),
		     pmacd_isPMBQueueFull(pBoard->overflowQueue),
		     pmacd_getPMBQueueWrapAround(pBoard->overflowQueue),
		     pBoard->overflowQueue->numberOfMessages,
		     pmacd_getPMBQueueCapacity(pBoard->overflowQueue),
		     pBoard->overflowQueue->numberOfBlocks,
		     (pBoard->overflowQueue->pmbSize * 
		      pBoard->overflowQueue->pmbsPerBlock),
		     pBoard->overflowQueue->highWaterMark,
		     pBoard->overflowQueue->pmbsPerBlock,
		     ATOMIC_GET(pBoard->streams.activeStreams)); 
    }
    if(len > PAGE_SIZE-1000)return(len);
  }
  
  len += sprintf(buf+len,"\nQueues:\n"
		           "_______\n");  

  for(i = 0; i<PMACD_MAX_QUEUES; i++){
    pQueue = pmacd_driver.queues[i];
    if(pQueue != NULL){
      len += sprintf(buf+len,
		     "QueueAddress:0x%x\n"
		     "  IsQueueEmpty:%i (0=False 1=True)\n"
		     "  IsQueueFull:%i (0=False 1=True)\n"
		     "  WrapAround:%i (0=Wrapped)\n"
		     "  MsgCount = %i\n"
		     "  CleanMsgs = %i\n"
		     "  NumberOfBlocks = %i\n"
		     "  BlockSize = %i(bytes)\n"
		     "  BlockHighWatermark = %i\n"
		     "  MsgsPerBlock = %i\n",
		     (pQueue->queueAddress | PMACD_HOSTBOARD_FLAG),
		     pmacd_isPMBQueueEmpty(pQueue),
		     pmacd_isPMBQueueFull(pQueue),
		     pmacd_getPMBQueueWrapAround(pQueue),
		     pQueue->numberOfMessages,
		     pmacd_getPMBQueueCapacity(pQueue),
		     pQueue->numberOfBlocks,
		     (pQueue->pmbSize * pQueue->pmbsPerBlock),
		     pQueue->highWaterMark,
		     pQueue->pmbsPerBlock);
    }
    if(len > PAGE_SIZE-1000)return(len);
  }
    

  // Dump active streams info.
  len += sprintf(buf+len,"\nActive Streams:\n"
		         "_______\n");  

  for(j = 0; j<=PMACD_MAX_BOARDID ; j++){
    pBoard = pmacd_driver.boardmap[j];
    if(pBoard !=NULL){
      stream = pBoard->streams.streams;
      for(i=0; i< pBoard->streams.numberOfStreams; i++){
	stream = &(pBoard->streams.streams[i]);
	if(pmacd_getStreamState(stream) != pmacd_StreamState_Null){
	  sbuf = (UINT8_T *)stream->buffers;
	  len += sprintf(buf+len,
			 "BoardId #%i Stream #%i: state=%i isCmdActive=%i isMapped=%i errorCode=0x%x\n"
			 "     boardId=%i contextId=%i termId=%i streamId=%i\n"
			 "     streamFlags=0x%x bufferSize=%i numberOfBuffers=%i\n"
			 "     0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n"
			 "     0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
			 pBoard->parms.boardId,
			 i,
			 pmacd_getStreamState(stream),
			 pmacd_isStreamCommandActive(stream),
			 pmacd_isStreamMapped(stream),
			 pmacd_getStreamError(stream),
			 stream->boardId, stream->contextId, stream->termId, stream->hostStreamId,
			 stream->streamAttributes, stream->bufferSize, stream->numberOfBuffers,
			 sbuf[0], sbuf[1], sbuf[2], sbuf[3], sbuf[4], 
			 sbuf[5], sbuf[6], sbuf[7], sbuf[8], sbuf[9], 
			 sbuf[10], sbuf[11], sbuf[12], sbuf[13], sbuf[14], 
			 sbuf[15], sbuf[16], sbuf[17], sbuf[18], sbuf[19]);
	  if(len > PAGE_SIZE-1000)return(len);
	}
      }
    }
    if(len > PAGE_SIZE-1000)return(len);
  }
  
  return len;
  
}



static struct proc_dir_entry *pmacd_driverInfo_proc_entry;
static struct proc_dir_entry *pmacd_driverInfo_proc_entry;
static struct proc_dir_entry *pmacd_driverInfo_proc_entry;

/*
 *  FunctionName:  pmacd_registerProcDriverInfo()
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
void pmacd_registerProcDriverInfo(void){
  pmacd_driverInfo_proc_entry = create_proc_entry("pmac", S_IFREG | S_IRUGO, NULL);
//  pmacd_driverInfo_proc_entry = create_proc_entry("pmac", S_IFREG | S_IRUGO, &proc_root);
  pmacd_driverInfo_proc_entry->get_info = &pmacd_procDriverInfo;
}

/*
 *  FunctionName:  pmacd_unregisterProcDriverInfo()
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
void pmacd_unregisterProcDriverInfo(void){
//  remove_proc_entry("pmac", &proc_root);
  remove_proc_entry("pmac", NULL);
}







