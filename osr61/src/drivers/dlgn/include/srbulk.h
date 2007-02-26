/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srbulk.h
 * Description                  : SRAM protocol - bulk definitions
 *
 *
 **********************************************************************/

#ifndef __SRBULK_H__
#define __SRBULK_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

/*
 * External function references
 */
#if LFS
extern void sr_bulkstart(mblk_t *, GN_LOGDEV *);
extern void sr_bulkdone(register mblk_t *, register GN_LOGDEV *);
extern void sr_bulkmore(register mblk_t *, register GN_LOGDEV *);
extern void sr_record(mblk_t *, GN_LOGDEV *);
extern int  sr_lastbulk(mblk_t *, GN_LOGDEV *, DEVBUFF *);
#else
extern void sr_bulkstart();
extern void sr_bulkdone();
extern void sr_bulkmore();
extern void sr_record();
extern int  sr_lastbulk();
#endif

#endif


