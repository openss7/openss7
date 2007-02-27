/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_control.h
// 
//  Defines functions that work on the pmac control interface.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_CONTROL_H
#define _PMACD_CONTROL_H

#include "pmacd.h"
#include "pmacd_linux_pci.h"

// Board Configuration Structure
//
#pragma pack(1)

// This is the shared memory structure on the board.  It comes from the NT driver.
// Only a few of the fields are actually used by the board see the pmacd_configBoard command
// for which fields are used.
typedef struct
{
  unsigned long             boardNumber;      /* Assinged by PPD                      */
  pmacd_boardstate_t        boardState;       /* Present board state                  */
  pmacd_msgTransferMode_t   inBoundMode;      /* Defines the inbound message mode.    */
                                              /*  User assigned.                      */
  unsigned long             inBoundTimer;     /* Defines the period of the inbound    */
                                              /* timer for the PPD. User Assigned     */
  pmacd_msgTransferMode_t   hwOutBoundMode;   /* Defines the outbounf message mode    */
                                              /*  User assigned.                      */
  unsigned long       hwOutBoundTimer;        /* Defines the period of the outbound   */
                                              /* timer for the PMAC. User assigned    */
  unsigned long       asapThreshold;          /* User assigned                        */
  unsigned long       hwAsapThreshold;        /* User assigned -- used by PMAC        */
  unsigned long       orphanageMsgTimeout;    /* in seconds. User defined             */
  unsigned long        muMsgFrameSize;        /* Size of each I20 message             */
  pmacd_MFPullOption_t muPullOption;          /* 1 = All MFAs alloc'd on host. 0=     */
                                              /*  Outbound MFAs on host inbound MFAs  */
                                              /*  alloc'd on PMAC. Read from registry */
  void*               muInBoundAddr;          /* Start address of Inbound MFAs        */
  void*               muOutBoundAddr;         /* Start address of outbound MFAs       */
  unsigned long       muFifoSize;             /* Number of MFAs in MU queues.         */                                 
  unsigned long       cntrUpdateRate;         /* Host I/F perf. counter update rate   */
  unsigned long       strmBuffersPer;         /* Number of ping/pong buffers per      */
                                              /*  stream                              */
  unsigned short      uNumStreams;            /* Total number of streams. Read from   */
                                              /*  registry                            */
  unsigned long       strmBufferSize;         /* Defines the size of a stream buffer  */
  unsigned long       strmBuffers;            /* Number of stream addresses that      */
                                              /*  follow.                             */
  /* unsigned long   strmAdds[MAX_STREAM_BFRS];  Contains the stream buffer           */
                                              /* addresses                            */
} pmacd_boardConfigBlock_t;
#pragma pack()


typedef struct{
  ATOMIC_T state;
  ATOMIC_T intEnabled;      // Interrupts Enabled Flag
  ATOMIC_T cmdActive;       // Command Active Flag
  MUTEX_T  modifyIntMutex;  // Used for modifying interrupts

  // cmdActive acts as a mutex protection
  UINT32_T command;     // Last/Present command sent
  UINT32_T cmdArg1;     // Last/Present command argument 1
  UINT32_T cmdArg2;     // Last/Present command argument 2
  UINT32_T response;    // Last response recieved
  UINT32_T respArg1;    // Last response argument 1
  UINT32_T respArg2;    // Last response argument 2
  UINT32_T errorCode;   // Last ErrorCode recieved. 
                        // 0 = last command was successful or in progress
  UINT8_T successState; // Upon a successful response what state to move to.
  UINT8_T errorState;   // Upon an errored response what state to move to.
  struct timer_list responseTimer;     // Timer for waiting for response.
#ifdef LINUX24
#ifdef LFS
  struct tasklet_struct handleResponseTask; // Task to process responses when they arrive.
#else
  struct tq_struct handleResponseTask; // Task to process responses when they arrive.
#endif
#else
  struct work_struct handleResponseTask; // Task to process responses when they arrive.
#endif
  pmacd_time_sec_t responseTimeout;    // Max time to wait for a response.
  WAITQUEUE_T waitForResponse;         // Wait queue for the process to wait for response
}pmacd_control_t;

/*
 * Function Prototypes
 */
#define pmacd_getControlState(control)\
     ATOMIC_GET((control)->state)

#define pmacd_setControlState(control, newState)\
     ATOMIC_SET((control)->state, (newState))

//
// pmacd_ifControlCommandNotActiveSetActive(ptrToControlStruct)
//
// Returns TRUE if you were the one to set the flag, otherwise
// it returns false
#define pmacd_ifControlCommandNotActiveSetActive(control)\
     ATOMIC_DEC_AND_TEST((control)->cmdActive)

#define pmacd_isControlCommandActive(control)\
     (ATOMIC_GET((control)->cmdActive) < 1)

#define pmacd_setControlCommandActive(control)\
     ATOMIC_SET((control)->cmdActive, 0)

#define pmacd_clearControlCommandActive(control)\
     ATOMIC_SET((control)->cmdActive, 1)


#define pmacd_areControlInterruptsEnabled(control)\
     ATOMIC_GET((control)->intEnabled)

#define pmacd_setControlInterruptsEnabled(control)\
     ATOMIC_SET((control)->intEnabled, 1)

#define pmacd_clearControlInterruptsEnabled(control)\
     ATOMIC_SET((control)->intEnabled, 0)

int pmacd_sendSimpleControlCommand(pmacd_control_t *control,
				   pmacd_pcibusdevice_t *busDevice,
				   UINT32_T command, 
				   UINT32_T arg1, 
				   UINT32_T arg2, 
				   pmacd_boardstate_t newState);

void pmacd_cleanupControl(pmacd_control_t *control);
BOOLEAN_T pmacd_initControl(pmacd_control_t *control);
void pmacd_handleControlResponse(void *control);
void pmacd_handleControlResponseTimeout(ULONG_T control);
#endif /* _PMACD_CONTROL_H */









