/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srcmd.h
 * Description                  : header for srcmd.c
 *
 *
 **********************************************************************/

#ifndef __SRCMD_H__
#define __SRCMD_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

/*
 * External function references
 */
#ifdef LFS
extern int sr_send2bd(GN_LOGDEV *, DL_DRVCMD *, int *, int);
extern int sr_control(GN_LOGDEV *, mblk_t *);
extern int sr_device(GN_LOGDEV *, mblk_t *);
extern int sr_virt(GN_LOGDEV *, mblk_t *);
extern int sr_bulk(GN_LOGDEV *, mblk_t *);
#else
extern int sr_send2bd();
extern int sr_control();
extern int sr_device();
extern int sr_virt();
extern int sr_bulk();
#endif

#endif

