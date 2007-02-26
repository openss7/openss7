/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gndbg.h
 * Description                  : generic driver debug 
 *
 *
 **********************************************************************/

#ifndef __GNDBG_H__
#define __GNDBG_H__

#ifdef __STANDALONE__            /* Required header files */
#endif


typedef struct gn_debug {
   unsigned char gd_pmname[8];
   unsigned long gd_msgsz;
   unsigned long gd_rfu[1];
} GN_DEBUG;

typedef struct pm_debug {
   GN_DEBUG *gn_debug;
   unsigned long size;
} PM_DEBUG;

typedef struct pm_memsz {
   unsigned long bdsz;
   unsigned long ldsz;
   unsigned char *pmname;
} PM_MEMSZ;

#endif

