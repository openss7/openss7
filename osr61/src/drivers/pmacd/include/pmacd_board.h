/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_board.h
// 
//  Defines structures and functions for working with pmacs at the board
//  level.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_BOARD_H
#define _PMACD_BOARD_H

#include "pmacd.h"
#include "pmacd_pmb.h"
#include "pmacd_mf.h"
#include "pmacd_stream.h"
#include "pmacd_control.h"
#include "pmacd_linux_pci.h"

typedef struct{
  UINT16_T busId;
  UINT16_T slotId;
  UINT8_T  geoAddrId;
  UINT8_T  shelfId;
  UINT8_T   boardId;
  UINT8_T   busType;
}pmacd_boardparms_t;

/*
 *  Board
 *  -- this structure represents a board in the system
 *  -- all board related activities are accessed through this
 *     structure
 */
typedef struct{
  MUTEX_T mutex;                       // Mutex for accessing this struct
  pmacd_pcibusdevice_t bus;            // PCI Bus device
  pmacd_control_t control;             // Structure for control command 
  pmacd_pmbqueue_t *overflowQueue;     // Overflow when the MF's are full
  pmacd_mfaqueue_t inboundMfaQueue;    // Queue of MFA's for inbound batching
  pmacd_mfpool_t *inboundMFs;          // inbound Msg Frames  = to the board
  pmacd_mfpool_t *outboundMFs;         // outbound Msg Frames = from the board
  pmacd_streamgroup_t streams;         // streams for this board
  struct timer_list sendMessagesTimer; // Timer used for sending msgs to the board
  struct timer_list resetBoardTimer;   // Timer for hard reseting the board
#ifdef LINUX24
#ifdef LFS
  struct tasklet_struct getMessagesTask;    // Task for retrieving messages
#else
  struct tq_struct getMessagesTask;    // Task for retrieving messages
#endif
#else
  struct work_struct getMessagesTask;    // Task for retrieving messages
#endif
  pmacd_boardparms_t parms;            // Structure of board parameters
}pmacd_board_t;

typedef struct {
unsigned short state;
pmacd_board_t *pBoard;
}pmacd_hs_t;

/*
 * Function Prototypes
 */

pmacd_board_t *pmacd_allocateBoard(UINT8_T boardId,
				   UINT16_T busId,
				   UINT16_T slotId,
				   UINT8_T geoAddrId,
				   UINT8_T busType,
				   pmacd_msgUnitConfig_t *msgUnitConfig,
				   pmacd_streamConfig_t *streamConfig);
void           pmacd_freeBoard(pmacd_board_t *pBoard);
int            pmacd_initBoard(pmacd_board_t *pBoard);
int            pmacd_configureBoard(pmacd_board_t *pBoard,
				    pmacd_msgUnitConfig_t *msgUnitConfig);
int            pmacd_resetBoard(pmacd_board_t *pBoard);
int            pmacd_startBoard(pmacd_board_t *pBoard);
int            pmacd_stopBoard(pmacd_board_t *pBoard);
int            pmacd_quiesceBoard(pmacd_board_t *pBoard);
int pmacd_hardResetBoard(pmacd_board_t *pBoard, BOOLEAN_T block);

void pmacd_moveOverflowMsgsToMfs(pmacd_mfpool_t *mfPool,
				 pmacd_mfaqueue_t *mfaQueue,
				 pmacd_pmbqueue_t *overflowQueue);

void pmacd_sendMfaQueueToBoard(pmacd_mfaqueue_t *mfaQueue,
			       pmacd_mfpool_t *mfPool);
void pmacd_hardResetBoardComplete(ULONG_T board);

int pmacd_GetFWBuffer(pmacd_board_t *pBoard);
int pmacd_StartDownloadFW(pmacd_board_t *pBoard, ULONG_T fSize);
int pmacd_ResetSystemAndEnterBoot(pmacd_board_t *pBoard);
int pmacd_DownloadFWComplete(pmacd_board_t *pBoard);
#endif /* _PMACD_BOARD_H */











