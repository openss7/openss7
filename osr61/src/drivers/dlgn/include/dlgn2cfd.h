/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgn2cfd.h
 * Description                  : dlgnc and cfd.c header
 *
 *
 **********************************************************************/

#ifndef __DLGN2CFD_H__
#define __DLGN2CFD_H__

#ifdef __STANDALONE__            /* Required header files */
#endif


/*
 * Functions provided by dlgn.c to cfd.c
 */
#ifdef LFS
extern void dlgn_drvstart(void);
extern void dlgn_drvstop(void);
extern void dlgn_addirq(void);
extern void dlgn_rmvirq(void);
#else
extern void dlgn_drvstart();
extern void dlgn_drvstop();
extern void dlgn_addirq();
extern void dlgn_rmvirq();
#endif

#endif

