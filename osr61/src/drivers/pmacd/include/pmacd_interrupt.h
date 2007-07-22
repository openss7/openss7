/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_interrupt.h
//
// Defines functions that deal with pmac interrupt handling.
/////////////////////////////////////////////////////////////////////////////
#ifndef _PMACD_INTERRUPT_H
#define _PMACD_INTERRUPT_H

#include "pmacd.h"
#include "pmacd_board.h"

#ifdef HAVE_KTYPE_IRQ_HANDLER_2ARGS
irqreturn_t pmacd_handleInterrupt(int irq, void *pBoard);
#else
#ifdef HAVE_KTYPE_IRQRETURN_T
irqreturn_t pmacd_handleInterrupt(int irq, void *pBoard, struct pt_regs *regs);
#else
void pmacd_handleInterrupt(int irq, void *pBoard, struct pt_regs *regs);
#endif
#endif
BOOLEAN_T pmacd_enableInterrupts(pmacd_board_t *board);
void pmacd_disableInterrupts(pmacd_board_t *board);
void pmacd_routeBoardMessages(void *pBoard);
void pmacd_sendMessagesToBoard(ULONG_T board);

#endif /* _PMACD_INTERRUPT_H */








