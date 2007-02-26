/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_driver.h
//
// Functions and Structures used to manage the Driver level actions and data.
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_DRIVER_H
#define _PMACD_DRIVER_H

#include "pmacd.h"
#include "pmacd_board.h"
#include "pmacd_pmb.h"

typedef struct{
  UINT8_T maxQueues;
  UINT8_T maxBoards;
  UINT8_T maxBoardId;
  UINT8_T lastBoardId;       // last BoardId assigned
  UINT8_T lastQueueAddress;  // last QueueAddress assigned
  UINT8_T  boardCount;       // number of boards the driver is handling
  ATOMIC_T queueCount;
  UINT32_T queueHighWaterMark;
  pmacd_streamConfig_t streamConfig;
  pmacd_msgUnitConfig_t msgUnitConfig;
}pmacd_driverparms_t;

typedef struct{
  MUTEX_T mutex;
  pmacd_driverparms_t parms;
  pmacd_pmbqueue_t *queues[PMACD_MAX_QUEUES]; // array of queues indexed by address.  
  pmacd_board_t *boardmap[PMACD_MAX_BOARDS];// Array of boards indexed by boardId.
}pmacd_driver_t;

extern pmacd_driver_t pmacd_driver;

// driver functions
void pmacd_initDriver(ULONG_T msgSize,
		      pmacd_MFPullOptionValues_t  pullOption,
		      ULONG_T inboundQueueSize,
		      ULONG_T outboundQueueSize,
		      ULONG_T streamBufferSize,
		      ULONG_T buffersPerStream,
		      ULONG_T streamsPerBoard);
UINT8_T         pmacd_getFreeBoardId(void);
UINT8_T         pmacd_getFreeQueueAddress(void);
void            pmacd_addQueueToDriver(pmacd_pmbqueue_t *pQueue);
BOOLEAN_T       pmacd_addBoardToDriver(pmacd_board_t *pBoard);
int             pmacd_reportBoards(pmacd_reportBoards_t *report);
void            pmacd_printBoards(void);
ssize_t         pmacd_queueDriverMessage(pmacd_boardnumber_t boardNumber,
					 UINT8_T *buffer,
					 size_t size);
void            pmacd_decodeDriverMessage(pmacd_board_t *board,
					  pmacd_pmb_t *msg);
void pmacd_freeBoardsFromDriver(void);
void pmacd_freeBoardFromDriver(struct pci_dev *pciDev);
int  pmacd_ExitNotification(pmacd_boardnumber_t boardNumber, UINT8_T srcNode);


#endif /* _PMACD_DRIVER_H */








