/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gncfd.h
 * Description                  : gncfd header
 *
 *
 **********************************************************************/

#ifndef __GNCFD_H__
#define __GNCFD_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndrv.h"
#endif


/*
 * Public functions available from Config Driver
 */
#ifdef LFS
extern GN_LOGDEV *cf_name2ldp(char *);
extern char *cf_ioccopy(mblk_t *);
extern void cf_iocput(mblk_t *, GN_DEBUG *, int);
#else
extern GN_LOGDEV *cf_name2ldp();
extern char *cf_ioccopy();
extern void cf_iocput();
#endif

#endif

