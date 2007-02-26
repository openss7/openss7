/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbamsg.h
 * Description                  : SRAM board message 
 *
 *
 **********************************************************************/

#ifndef __SBDMSG_H__
#define __SBDMSG_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * SB_MESSAGE - Firmware to application message area
 *
 * The structure used by the firmware to communicate with the application
 * is SB_MESSAGE.  Memory for this structure must be allocated by the
 * application and on return from sb_dnldbd(), the values in msg_id and
 * msg_data1 must be checked and should be zero.
 * SIZE == 4 * 4 == 16 bytes
 */

typedef struct sb_message {
   unsigned short msg_id;     /* the message being returned by firmware  */
   unsigned short msg_data1;  /* Error					 */
   unsigned short msg_data2;  /* Data determined by the individual error */
   unsigned short msg_data3;  /* Data determined by the individual error */
   unsigned short msg_data4;  /* Data determined by the individual error */
   unsigned short msg_data5;  /* Data determined by the individual error */
   unsigned short msg_data6;  /* Data determined by the individual error */
   unsigned short msg_data7;  /* Data determined by the individual error */
} SB_MESSAGE;

#endif

