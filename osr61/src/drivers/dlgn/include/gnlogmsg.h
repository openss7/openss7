/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnlogmsg.h
 * Description                  : console message control
 *
 *
 **********************************************************************/

#ifndef __GNLOGMSG_H__
#define __GNLOGMSG_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Defines
 */
#define LF_CONT                  0x0001
#define LF_NOTE                  0x0002
#define LF_WARN                  0x0004
#define LF_PANIC                 0x0008

#define LM_ENABLE                0x0000
#define LM_DISABLE               0x1000

#define LF_DEBUG                 0x00010000
#define LF_HASBUG                0x00020000


#endif

