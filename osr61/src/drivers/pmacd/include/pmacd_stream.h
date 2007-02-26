/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_steams.h
// 
//  Defines structures and functions for working with streams
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_STREAM_H
#define _PMACD_STREAM_H

#include "pmacd.h"
#include "pmacd_pmb.h"

#define PMACD_PROPID_HOST_STREAMID         0x00002001
#define PMACD_PROPVAL_HOST_STREAMID_DETACH 0xFFFFFFFF
#define PMACD_STREAM_MODE_MASK             0x03
#define PMACD_DOWNLOAD_STREAM 		   0x20

// UserCmdId in stream commands has new state and stream index embedded in
// it.  upper 16 bits = new state; lower 16 bits = stream index
#define pmacd_getStreamIdxFromUserCmdId(cmdId) ((cmdId)&0x0000FFFF)
#define pmacd_getNewStreamStateFromUserCmdId(cmdId) (((cmdId)&0xFFFF0000)>>16)
#define pmacd_setStreamIdxInUserCmdId(cmdId, idx)\
         (cmdId) = ((cmdId)&0xFFFF0000)|((idx)&0x0000FFFF)
#define pmacd_setNewStreamStateInUserCmdId(cmdId, state)\
         (cmdId) = (((state)<<16)&0xFFFF0000)|((cmdId)&0x0000FFFF)

// Structures used for dealing with streaming control messages.
#pragma pack(1)

typedef struct{
  UINT8_T descId;
  UINT8_T streamId;
  struct{
    UINT8_T streamMode:3;
    UINT8_T reserved:5;
  }flags;
  UINT8_T  numberOfPropertyIds;
  UINT32_T propertyId;
  UINT32_T value;
}pmacd_localControlDesc_t;

typedef struct{
  pmacd_msgheader_t        msgHeader;
  pmacd_commandHeader_t    cmdHeader;
  pmacd_localControlDesc_t localControlDesc;

  // TBD: Will be used for exit notification eventually
  // pmacd_srcregistration_t  srcRegistrationDesc;
}pmacd_attachRequestMsg_t, pmacd_detachRequestMsg_t;


typedef struct{
  pmacd_msgheader_t msgHeader;
  pmacd_commandHeader_t cmdHeader;
}pmacd_attachReplyMsg_t, 
  pmacd_detachReplyMsg_t, 
  pmacd_modifyReplyMsg_t;

#pragma pack()


typedef UINT8_T pmacd_streambuffer_t[];
typedef pmacd_streambuffer_t *pmacd_streambufferblock_t;

typedef enum{
  pmacd_StreamState_Null,
  pmacd_StreamState_Attached,
  pmacd_StreamState_Started,
  pmacd_StreamState_Downloaded
}pmacd_streamstate_t;

typedef enum {
  pmacd_StreamCmd_Start,
  pmacd_StreamCmd_Stop,
  pmacd_StreamCmd_Reset_Boot

  // TBD: Not supported Yet
  // pmacd_StreamCmd_Pause,
  // pmacd_StreamCmd_Resume
}pmacd_streamcommand_t; 

typedef struct{
  UINT8_T numberOfConnections;
}pmacd_streamstats_t;

typedef struct{
  UINT8_T contextId;
}pmacd_streamparms_t;

/*
 *  Stream
 *  -- this structure represents a board stream
 *  -- all streaming related activities are accessed through 
 *     this structure
 */
typedef struct{
  ATOMIC_T cmdActive;  // is a command actively working on the stream.
  ATOMIC_T state;                              // state of the stream.
  ATOMIC_T isMapped;                    // has the stream been mmaped.
  ATOMIC_T error;
  ULONG_T  numberOfBuffers;
  ULONG_T  bufferSize;
  ULONG_T  hostStreamId;
  ULONG_T  contextId;
  ULONG_T  termId;
  ULONG_T  boardId;
  ULONG_T  streamAttributes;
  void     *mmapAddress;
  struct pmacd_streamgroup *group;
  int streamIdx;
  pmacd_streambuffer_t *buffers;
  pmacd_time_sec_t responseTimeoutTimer;
  WAITQUEUE_T waitForResponse;
}pmacd_stream_t;

typedef struct pmacd_streamgroup{
  ATOMIC_T activeStreams;
  ULONG_T  numberOfStreams;
  ULONG_T  buffersPerStream;
  ULONG_T  bufferSize;
  pmacd_stream_t  *streams;
  pmacd_streambufferblock_t *streamBuffers;
  dma_addr_t streamBuffersBusAddr;
}pmacd_streamgroup_t;



/*
 * Function Prototypes
 */
pmacd_streambufferblock_t *pmacd_allocateStreamBufferBlock(ULONG_T bufferSize, 
							   ULONG_T numberOfBuffers);
pmacd_stream_t            *pmacd_allocateStreams(ULONG_T bufferSize, 
						 ULONG_T buffersPerStream,
						 ULONG_T numberOfStreams,
						 pmacd_streamgroup_t *group,
						 pmacd_streambufferblock_t *block);

void                      pmacd_freeStreams(pmacd_stream_t *streams, 
					    ULONG_T numberOfStreams);
void                      pmacd_freeStreamBufferBlock(pmacd_streambufferblock_t *block,
						      ULONG_T bufferSize,
						      ULONG_T numberOfBuffers);

BOOLEAN_T                 pmacd_initStreamGroup(pmacd_streamgroup_t  *group,
						pmacd_streamConfig_t *config);

void                      pmacd_nullStreamGroup(pmacd_streamgroup_t *group);

void                      pmacd_cleanupStreamGroup(pmacd_streamgroup_t *group);

int                       pmacd_attachStream(pmacd_boardId_t boardId,
					     ULONG_T contextId,
					     ULONG_T termId,
					     ULONG_T streamAttributes,
					     pmacd_streamgroup_t  *group,
					     pmacd_stream_t **attachedStream);

int                       pmacd_detachStream(pmacd_stream_t *stream);
int                       pmacd_startStream(pmacd_stream_t *stream);
void                      pmacd_clearStreamBuffer(pmacd_stream_t *stream);
//int 			  pmacd_ResetSystemAndEnterBoot(pmacd_stream_t *stream);

//
// pmacd_ifStreamCommandNotActiveSetActive(ptrToControlStruct)
//
// Returns TRUE if you were the one to set the flag, otherwise
// it returns false
#define pmacd_ifStreamCommandNotActiveSetActive(stream)\
     ATOMIC_DEC_AND_TEST((stream)->cmdActive)

#define pmacd_isStreamCommandActive(stream)\
     (ATOMIC_GET((stream)->cmdActive) < 1)

#define pmacd_setStreamCommandActive(stream)\
     ATOMIC_SET((stream)->cmdActive, 0)

#define pmacd_clearStreamCommandActive(stream)\
     ATOMIC_SET((stream)->cmdActive, 1);\
     WAKEUP_WAITQUEUE((stream)->waitForResponse)

#define pmacd_getStreamState(stream)\
     ATOMIC_GET((stream)->state)

#define pmacd_setStreamState(stream, newState)\
     ATOMIC_SET((stream)->state, (newState))

#define pmacd_ifStreamNotMappedMarkMapped(stream)\
     ATOMIC_DEC_AND_TEST((stream)->isMapped)

#define pmacd_isStreamMapped(stream)\
     (ATOMIC_GET((stream)->isMapped) < 1)

#define pmacd_setStreamMapped(stream)\
     ATOMIC_SET((stream)->isMapped, 0)

#define pmacd_clearStreamMapped(stream)\
     ATOMIC_SET((stream)->isMapped, 1)

#define pmacd_getStreamError(stream)\
     ATOMIC_GET((stream)->error)

#define pmacd_setStreamError(stream, err)\
     ATOMIC_SET((stream)->error, (err))

#define pmacd_clearStreamError(stream)\
     ATOMIC_SET((stream)->error, 0)




#endif /* _PMACD_STREAM_H */


































