/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srdbg.h
 * Description                  : SRAM protocol debug
 *
 *
 **********************************************************************/

#ifndef __SRDBG_H__
#define __SRDBG_H__

#ifdef __STANDALONE__
#include "typedefs.h"
#include "gndefs.h"
#endif

/*
 * Commands for the SRAM PM's debug function
 */
#define SRDBG_DUMPCMD         0x0001


/*
 * SRDBG_CMD: Command header structure for SRAM PM debug commands.
 */
typedef struct {
   char  dbg_bdname[LD_NAMELEN];       /* Logical device name */
   ULONG dbg_cmd;                      /* Debug command ID */
   ULONG dbg_rfu[3];
} SRDBG_CMD;


/*
 * SRDBG_DUMP: Data structure for SRDBG_DUMPCMD command.
 */
typedef struct {
   unsigned long ds_offset;          /* SRAM offset to start dump */
   unsigned long ds_length;          /* # bytes to dump */
   unsigned long ds_rfu[2];          /* paragaph align */
#ifndef VME_SPAN
   unsigned char ds_srbuf[BUFSZ_8K]; /* Entire SRAM for one board */
#else
   unsigned char ds_srbuf[BUFSZ_64K]; /* Entire SRAM for one board */
#endif /* VME_SPAN */
} SRDBG_DUMP;

#endif

