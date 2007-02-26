/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : cfmsg.h
 * Description                  : generic library message info
 *
 *
 **********************************************************************/

#ifndef __CFMSG_H__
#define __CFMSG_H__

/*
 * Config Driver IOCTLS
 */
#define CF_STARTGNDRV            0x01
#define CF_STOPGNDRV             0x02
#define CF_STARTPM               0x03
#define CF_STOPPM                0x04
#define CF_STARTBD               0x05
#define CF_STOPBD                0x06
#define CF_GETNPMS               0x07
#define CF_SETLOG                0x08
#define CF_STATUS                0x09
#define CF_DEBUG                 0x0A

#ifdef VME_LIVE_INSERTION
#define CF_MONITORERR            0x0B
#define CF_GETSTARTBDINFO        0x0C
#define CF_GETBDINFO             0x0D
#define CF_BOOTDIAGS             0x0E
#define CF_GETPMINFO             0x0F
#define CF_ADDBD                 0x10
#define CF_REMOVEBD              0x11
#define CF_RESETBD               0x12
#define CF_GETTSINFO             0x13
#define CF_SETTSINFO             0x14

#endif
#define CF_STATS                 0x15
#define CF_TESTLOG               0xFE


/*
 * Data Structures
 */
typedef struct gn_logdata {
   unsigned long gl_mode;
   unsigned long gl_rfu[3];
} GN_LOGDATA;


#endif

