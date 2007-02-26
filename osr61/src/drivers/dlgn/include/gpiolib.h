/********************************************************************
 *        FILE: gpiolib.h
 *
 * DESCRIPTION: This is the Dialogic Generic Port I/O Device Driver
 *              Library header file.
 *
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Intel Corporation.
 *  The copyright notice above does not evidence any actual or
 *  intended publication of such source code.
 *
 *  Copyright (c) 1996 Intel Corporation.
 *  All Rights Reserved
 ********************************************************************/


#ifndef __GPIOLIB_H__
#define __GPIOLIB_H__

#ifdef __STANDALONE__            /* Required header files */
#endif


/*
 * Error codes for Generic Port I/O Driver
 */
#define EGPIO_SUCCESS   0x0000
#define EGPIO_SYSTEM    0x0001
#define EGPIO_OPENFAIL  0x0002
#define EGPIO_BADPARAM  0x0003
#define EGPIO_CCMIBF    0x0004


/*
 * Data Structure to enable/disable console message logging.
 */
typedef struct gpio_logdata {
   unsigned short mode;
   unsigned short flags;
} GPIO_LOGDATA;


/*
 * Library Function Prototypes
 */
#ifdef __cplusplus
extern "C" {             // C Plus Plus function bindings
#define extern
#endif

#if (defined(__STDC__) || defined(__cplusplus))
void           gpio_perror(char *);
int            gpio_setlog(GPIO_LOGDATA *),
               inp(int),
               outp(int, int),
               gpio_readmemory(unsigned long, void *, unsigned long),
               gpio_writememoryUCHAR(unsigned long, unsigned char),
               gpio_writememoryUSHORT(unsigned long, unsigned short),
               gpio_writememory(unsigned long, void *, unsigned long),
               gpio_locateboards(unsigned long, unsigned char *),
               gpio_writeccmdata(unsigned short, void *, unsigned long),
               gpio_mapboard(unsigned long, unsigned char),
               gpio_enableinterrupts(void),
               gpio_disableinterrupts(void),
               gpio_getinterruptcount(void),
               gpio_resetinterruptcount(void);
unsigned char  gpio_readmemoryUCHAR(unsigned long);
unsigned short gpio_readmemoryUSHORT(unsigned long);
unsigned char  gpio_GetPciBoardAddress (unsigned long, DLGC_PCIBRD_REC *);
#ifdef LFS
unsigned char  gpio_GetNumPciBoards (void);
#else
unsigned char  gpio_GetNumPciBoards ();
#endif
unsigned long  gpio_GetPciBoardBusAndSlot (unsigned long, unsigned short *, 
					   unsigned short *);
#ifdef LFS
char          *gpio_LastErrorMsg(void);
int            gpio_LastError(void);
unsigned char  gpio_CloseDriver(void);
#else
char          *gpio_LastErrorMsg();
int            gpio_LastError();
unsigned char  gpio_CloseDriver();
#endif
unsigned char  gpio_GetBoardPowerStatus(pPOWER_STATUS_STRUCT);
#else
void           gpio_perror();
int            gpio_setlog(),
               inp(),
               outp(),
               gpio_readmemory(),
               gpio_writememoryUCHAR(),
               gpio_writememoryUSHORT(),
               gpio_writememory(),
               gpio_locateboards(),
               gpio_writeccmdata(),
               gpio_mapboard(),
               gpio_enableinterrupts(),
               gpio_disableinterrupts(),
               gpio_getinterruptcount(),
               gpio_resetinterruptcount();
unsigned char  gpio_readmemoryUCHAR();
unsigned short gpio_readmemoryUSHORT();
unsigned char  gpio_GetPciBoardAddress ();
unsigned char  gpio_GetNumPciBoards ();
unsigned long  gpio_GetPciBoardBusAndSlot ();
char          *gpio_LastErrorMsg();
int            gpio_LastError();
unsigned char  gpio_CloseDriver();
unsigned char  gpio_GetBoardPowerStatus();
#endif

#ifdef __cplusplus
}                        // C Plus Plus function bindings
#undef extern
#endif


#endif

