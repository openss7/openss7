/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srpm.h
 * Description                  : SRAM protocol header
 *
 *
 **********************************************************************/

#ifndef __SRPM_H__
#define __SRPM_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

/*
 * SRAM PM function prototypes
 */
#ifdef LFS
extern int sraminitpm(void);
extern int sramshutpm(void);
extern int sramstartbd(GN_BOARD *);
extern int sramstopbd(GN_BOARD *);
extern int sramopen(GN_LOGDEV *);
extern int sramclose(GN_LOGDEV *, ULONG);
extern int sramsendcmd(GN_LOGDEV *, mblk_t *);
extern int srammemsz(ULONG *, ULONG *, char **);
//extern int sramdebug(GN_DEBUG *, int *);
#else
extern int sraminitpm();
extern int sramshutpm();
extern int sramstartbd();
extern int sramstopbd();
extern int sramopen();
extern int sramclose();
extern int sramsendcmd();
extern int srammemsz();
extern int sramdebug();
#endif


#endif
