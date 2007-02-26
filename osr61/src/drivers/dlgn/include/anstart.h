/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : anstart.h
 * Description                  : antares startup/shutdown
 *
 *
 **********************************************************************/

#ifndef __ANSTART_H__
#define __ANSTART_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

typedef struct an_startpm {
    SYS_INFO sys;
    CFG_INFO cfg;
    BOARDDATA p_board[NBOARDS];
} AN_STARTPM;

/*
 * AN_INITBD: Device-dependent board initialization structure. 
 */
typedef struct an_initbd {
   unsigned long  btype;     /* Board type code:  ANBT_..  */
   unsigned short ioport;    /* Board IO port address */
} AN_INITBD;

/*
 * Antares Board Types
 */
#define ANBT_MASTER    1   /* master board */
#define ANBT_SLAVE     0   /* Slave board  */

#endif

