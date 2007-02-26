/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srchk.h
 * Description                  : header for srchk.c
 *
 *
 **********************************************************************/

#ifndef __SRCHK_H__
#define __SRCHK_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


#ifdef LFS
extern int sr_cfgchk(GN_BOARD *, ULONG);
#else
extern int sr_cfgchk();
#endif

#endif


