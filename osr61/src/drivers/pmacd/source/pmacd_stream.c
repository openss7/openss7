/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_steams.c
// 
//  Defines structures and functions for working with streams
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_stream.h"
#include "21554.h"
#include "pmacd_driver.h"
#include "pmacd_pmb.h"

//extern pmacd_driver_t pmacd_driver;


/*
 *  FunctionName:  pmacd_allocateStreamBufferBlock
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
pmacd_streambufferblock_t *pmacd_allocateStreamBufferBlock(ULONG_T bufferSize, 
							   ULONG_T numberOfBuffers){
  unsigned long size = bufferSize * numberOfBuffers;
  UINT8_T *pBuf = (UINT8_T *)pmacd_mallocPageBlock(size);
  
  return((pmacd_streambufferblock_t *)pBuf);
}


/*
 *  FunctionName:  pmacd_allocateStreams
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
pmacd_stream_t *pmacd_allocateStreams(ULONG_T bufferSize, 
				      ULONG_T buffersPerStream,
				      ULONG_T numberOfStreams,
				      pmacd_streamgroup_t *group,
				      pmacd_streambufferblock_t *block){
  pmacd_stream_t *streams = (pmacd_stream_t *)NULL;
  ULONG_T i;
  UINT8_T *pBuf;

  // Check input parameters
  if(block == NULL ||
     !buffersPerStream ||
     !bufferSize ||
     !numberOfStreams) return(streams);
  
  streams = (pmacd_stream_t *)pmacd_malloc(numberOfStreams*sizeof(pmacd_stream_t));
  if(streams == NULL) return(streams);

  pBuf = (UINT8_T *)block;
  for(i=0; i<numberOfStreams; i++){
    INIT_WAITQUEUE(streams[i].waitForResponse);
    pmacd_setStreamState(&streams[i], pmacd_StreamState_Null);
    pmacd_clearStreamCommandActive(&streams[i]);
    pmacd_clearStreamMapped(&streams[i]);
    pmacd_clearStreamError(&streams[i]);
    streams[i].numberOfBuffers  = buffersPerStream;
    streams[i].bufferSize       = bufferSize;
    streams[i].hostStreamId     = i;
    streams[i].contextId        = 0;
    streams[i].termId           = 0;
    streams[i].streamAttributes = 0;
    streams[i].mmapAddress      = NULL;
    streams[i].group            = group;
    streams[i].streamIdx        = i;
    streams[i].buffers          = (pmacd_streambuffer_t *)pBuf;
    pBuf                       += bufferSize*buffersPerStream;
  }

  return(streams);
}

/*
 *  FunctionName:  pmacd_freeStreams
 *
 *        Inputs:  streams - pointer to a block of streams.
 *                 numberOfStreams - number of streams in the block
 *
 *       Outputs:  the memory of the streams in the block will be freed.
 *  
 *       Returns:  none.
 *
 *   Description:  Frees the streams memory 
 *
 *      Comments: This function will protect against *streams being NULL and numberOfStreams
 *                being zero. So no need to check them before calling this function.
 */ 
void pmacd_freeStreams(pmacd_stream_t *streams, 
		       ULONG_T numberOfStreams){
  // Check input parameters
  if(streams == NULL ||
     !numberOfStreams) return;
  pmacd_free((void *)streams, numberOfStreams*sizeof(pmacd_stream_t));
}


/*
 *  FunctionName:  pmacd_freeStreamBufferBlock()
 *
 *        Inputs:  block - pointer to stream buffer block to free.
 *                 bufferSize - size of each buffer in the block.
 *                 numberOfBuffers - number of buffers in the block
 *
 *       Outputs:  memory will be freed and the memory will be unreserved from the mem_map.
 *  
 *       Returns:  none.
 *
 *   Description:  Frees the stream buffer block memory and unreserves the memory from
 *                 the mem_map.
 *
 *      Comments: This function will protect against *block being NULL. So no need
 *                to check it before calling this function.
 */
void pmacd_freeStreamBufferBlock(pmacd_streambufferblock_t *block,
				 ULONG_T bufferSize,
				 ULONG_T numberOfBuffers){

#ifndef LFS
  unsigned long size = bufferSize * numberOfBuffers;
#endif
  UINT8_T *pBuf = (UINT8_T *)block;
  
  if (pBuf != NULL) {
    pmacd_freePageBlock((pmacd_memaddress_t)block, bufferSize*numberOfBuffers);
  }
}


/*
 *  FunctionName:  pmacd_initStreamGroup()
 *
 *        Inputs:  group - pointer to stream group to be initialized
 *                 config - pointer to stream configuration to configure group with.
 *
 *       Outputs:  group will be updated and stream structures and buffers will be allocated.
 *  
 *       Returns:  TRUE - initialization succeeded.
 *                 FALSE - memory allocation failed.
 *
 *   Description:  Initializes the stream group based on the configuration information.
 *                 Stream buffers and stream structures will be allocated.
 *
 *      Comments:  
 */
BOOLEAN_T pmacd_initStreamGroup(pmacd_streamgroup_t  *group,
				pmacd_streamConfig_t *config){
  ATOMIC_SET(group->activeStreams, 0);
  group->numberOfStreams  = config->streamsPerBoard;
  group->buffersPerStream = config->buffersPerStream;
  group->bufferSize       = config->bufferSize;
  group->streamBuffers    = 
    pmacd_allocateStreamBufferBlock(config->bufferSize,
				    config->buffersPerStream *
				    config->streamsPerBoard);
  if(group->streamBuffers == NULL)return(FALSE);
  
  group->streams = pmacd_allocateStreams(config->bufferSize,
					 config->buffersPerStream,
					 config->streamsPerBoard,
					 group,
					 group->streamBuffers);
  if(group->streams == NULL){
    pmacd_freeStreamBufferBlock(group->streamBuffers,
				config->bufferSize,
				config->buffersPerStream *
				config->streamsPerBoard);
    return(FALSE);
  }
  
  return(TRUE);
}

/*
 *  FunctionName:  pmacd_nullStreamGroup
 *
 *        Inputs:  group - group of streams to modify
 *                 
 *
 *       Outputs:  all streams in the group will be moved back to the NULL state.
 *  
 *       Returns:  none.
 *
 *   Description:  Sets all streams in the group back to the Null state, and clears
 *                 all of the structure values.
 *
 *      Comments: This is used for board reset.
 *                
 */ 
void pmacd_nullStreamGroup(pmacd_streamgroup_t *group){
  int i;
  pmacd_stream_t *streams;
  
  if(group == NULL) return;
  streams = group->streams;
  if(streams == NULL) return;
  
  // Force entire stream group to the null state.
  for(i=0; i < group->numberOfStreams ; i++){
    pmacd_setStreamCommandActive(&streams[i]);
    pmacd_setStreamState(&streams[i], pmacd_StreamState_Null);
    pmacd_clearStreamMapped(&streams[i]);
    pmacd_clearStreamError(&streams[i]);
    streams[i].hostStreamId     = i;
    streams[i].contextId        = 0;
    streams[i].termId           = 0;
    streams[i].streamAttributes = 0;
    streams[i].mmapAddress      = NULL;
    pmacd_clearStreamCommandActive(&streams[i]);
  }
}

/*
 *  FunctionName:  pmacd_cleanupStreamGroup
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
void pmacd_cleanupStreamGroup(pmacd_streamgroup_t  *group){
  if(group->streamBuffers != NULL)
    pmacd_freeStreamBufferBlock(group->streamBuffers,
				group->bufferSize,
				group->buffersPerStream *
				group->numberOfStreams);
  if(group->streams != NULL)
    pmacd_freeStreams(group->streams,
		      group->numberOfStreams);
}

/*
 *  FunctionName:  pmacd_sendStreamCommand
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
int pmacd_sendStreamCommand(pmacd_stream_t *stream, 
			    pmacd_streamcommand_t command){
  int msgSize = pmacd_driver.parms.msgUnitConfig.msgSize;
  UINT8_T msgBuf[msgSize];
  pmacd_attachRequestMsg_t *attachMsg = (pmacd_attachRequestMsg_t *)msgBuf;
  int retVal = 0;

  attachMsg->msgHeader.flags.msgType    = 0x00;
  attachMsg->msgHeader.flags.emergency  = 0x00;
  attachMsg->msgHeader.flags.priority   = 0x00;
  attachMsg->msgHeader.flags.extensionHdr  = 0x00;
  attachMsg->msgHeader.flags.version    = 0x00;
  attachMsg->msgHeader.length           = sizeof(pmacd_attachRequestMsg_t);
  attachMsg->msgHeader.srcNode          = PMACD_DRIVER_ADDRESS;
  attachMsg->msgHeader.destNode         = PMACD_CALLCONTROL_NODE;
  attachMsg->msgHeader.contextId        = stream->contextId;
  attachMsg->msgHeader.reserved         = 0x00;
  attachMsg->msgHeader.numberOfCommands = 0x01;
  attachMsg->cmdHeader.command          = PMACD_CMD_MODIFY_REQUEST;
  attachMsg->cmdHeader.length           = sizeof(pmacd_attachRequestMsg_t)-
					  sizeof(pmacd_msgheader_t);
  attachMsg->cmdHeader.numberOfTerms    = 0x01;
  attachMsg->cmdHeader.numberOfDescs    = 0x01;
  attachMsg->cmdHeader.termId           = stream->termId;
  
  attachMsg->localControlDesc.descId    = PMACD_DESCID_LOCAL_CONTROL;
  attachMsg->localControlDesc.streamId  = 0x01;
  attachMsg->localControlDesc.flags.streamMode = PMACD_STREAM_MODE_MASK & 
                                                 stream->streamAttributes;
  attachMsg->localControlDesc.flags.reserved   = 0x00;
  attachMsg->localControlDesc.numberOfPropertyIds = 0x01;
  attachMsg->localControlDesc.propertyId       = PMACD_PROPID_HOST_STREAMID;

  /*
  TBD:  This code should be used when Exit Notification is implemented

  attachMsg->srcRegistrationDesc.descId        = PMACD_DESCID_SRCNODE_REGISTER;
  attachMsg->srcRegistrationDesc.registerEvtType = PMACD_REG_EXITNOTIFY_BEHAVIOR;
  attachMsg->srcRegistrationDesc.memorize      = PMACD_SRCNODEREG_SET;
  attachMsg->srcRegistrationDesc.numberOfPropertyIds = 0x00;
  */

  // Set stream index in user command id for routing in the response handler.
  pmacd_setStreamIdxInUserCmdId(attachMsg->msgHeader.userCmdId, stream->streamIdx);
  
  if(command == pmacd_StreamCmd_Stop){
    attachMsg->localControlDesc.value = PMACD_PROPVAL_HOST_STREAMID_DETACH;
    pmacd_setNewStreamStateInUserCmdId(attachMsg->msgHeader.userCmdId, 
				       pmacd_StreamState_Null);
  } else {
    attachMsg->localControlDesc.value     = stream->streamIdx;
    pmacd_setNewStreamStateInUserCmdId(attachMsg->msgHeader.userCmdId, 
				       pmacd_StreamState_Started);
  }

  // Clear the stream error to be able to detect errors on this response.
  pmacd_clearStreamError(stream);

  if((retVal = pmacd_queueDriverMessage(stream->boardId, 
				       msgBuf, 
					msgSize))
     < 0){
    return(retVal);
  }
  return(0);
}

/*
 *  FunctionName:  pmacd_attachStream
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
int pmacd_attachStream(pmacd_boardId_t boardId,
		       ULONG_T contextId,
		       ULONG_T termId,
		       ULONG_T streamAttributes,
		       pmacd_streamgroup_t  *group,
		       pmacd_stream_t **attachedStream){
  pmacd_stream_t *stream;

  if(!contextId ||
     group == NULL ||
     contextId > group->numberOfStreams){
     pmacd_printDebug("pmacd_attachStream: Error contextId=0x%x, group=0x%x,  streams=0x%x\n", contextId, group, group->numberOfStreams);
    return -EINVAL;
  }
  
  // The firmware presently expects the stream buffer
  // to be buffer at index = (contextId - 1)
  if ( streamAttributes & 0x20 ) {
      stream = &(group->streams[0]);
  } else {
      stream = &(group->streams[contextId-1]);
  }
  if(stream == NULL) {
	pmacd_printDebug("pmacd_attachStream: Error stream=0x%x\n", stream);
	return -EINVAL;
  }
  
  // Check if the stream has a command 
  // actively being performed on it.
  if(!pmacd_ifStreamCommandNotActiveSetActive(stream)) {
	pmacd_printDebug("pmacd_attachStream: Error activate\n");
    return -EBUSY;
  } 
  // Check if stream is in the Null state
  if(pmacd_getStreamState(stream) != pmacd_StreamState_Null){
    pmacd_clearStreamCommandActive(stream);
    pmacd_printDebug("pmacd_attachStream: pmacd_getStreamState failed\n");
    return -ENOTTY;
  }
  
  // Move stream to the attached state.
  pmacd_setStreamState(stream, pmacd_StreamState_Attached);
  ATOMIC_INC(group->activeStreams);

  // Store attached stream attributes.
  stream->boardId          = boardId;
  stream->contextId        = contextId;
  stream->termId           = termId;
  stream->streamAttributes = streamAttributes;

  // Setup output parm: pointer to attached stream.
  *attachedStream = stream; 

  //  pmacd_clearStreamBuffer(stream);
  pmacd_clearStreamCommandActive(stream);
  return(0);
}

/*
 *  FunctionName:  pmacd_clearStreamBuffer
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
void pmacd_clearStreamBuffer(pmacd_stream_t *stream){
  memset(stream->buffers, 0, 
	 stream->numberOfBuffers * stream->bufferSize);
}

/*
 *  FunctionName:  pmacd_startStream
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
int pmacd_startStream(pmacd_stream_t *stream){
  
  // Check if the stream has a command 
  // actively being performed on it.
  if(!pmacd_ifStreamCommandNotActiveSetActive(stream)){
    pmacd_printDebug("pmacd_startStream: pmacd_ifStreamCommandNotActiveSetActive failed\n");
    return -EBUSY;
  } 
  // Check if stream is in the correct state for start (attached).
  if(pmacd_getStreamState(stream) != pmacd_StreamState_Attached){
    pmacd_clearStreamCommandActive(stream);
    pmacd_printDebug("pmacd_startStream: pmacd_getStreamState failed\n");
    return -ENOTTY;
  }
 
  pmacd_sendStreamCommand(stream, pmacd_StreamCmd_Start); 
  return(0);
}

/*
 *  FunctionName:  pmacd_detachStream
 *
 *        Inputs:  
 *                 
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *
 *      Comments: 
 *                
 */ 
int pmacd_detachStream(pmacd_stream_t *stream){
  pmacd_streamstate_t state;

  // Check if the stream has a command 
  // actively being performed on it.
  if(!pmacd_ifStreamCommandNotActiveSetActive(stream))
    return -EBUSY;
  
  // Check if stream state
  state = pmacd_getStreamState(stream);
#ifdef LFS
  switch((int)state){
#else
  switch(state){
#endif

  case pmacd_StreamState_Null:
    pmacd_clearStreamCommandActive(stream);
    return -ENOTTY;

  case pmacd_StreamState_Attached:
    // Clear attached parameters
    stream->boardId          = 0;
    stream->contextId        = 0;
    stream->termId           = 0;
    stream->streamAttributes = 0;
    stream->mmapAddress      = NULL;
    pmacd_clearStreamMapped(stream);
    pmacd_clearStreamError(stream);
    pmacd_setStreamState(stream, pmacd_StreamState_Null);
    pmacd_clearStreamCommandActive(stream);
    if(stream->group != NULL){
      ATOMIC_DEC(stream->group->activeStreams);
    }
    break;
    
  case pmacd_StreamState_Started:
    pmacd_sendStreamCommand(stream, pmacd_StreamCmd_Stop);
    break;
  }
  
  return(0);
}
