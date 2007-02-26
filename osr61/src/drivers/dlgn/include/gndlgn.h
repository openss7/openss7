/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gndlgn.h
 * Description                  : exported sync module functions
 *
 *
 **********************************************************************/

#ifndef __GNDLGN_H__
#define __GNDLGN_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#endif


/*
 * Prototypes for public Sync Module functions provided for PM use.
 */
#ifdef LFS
extern mblk_t  *dlgn_copymp(register mblk_t *, register unsigned long);
extern mblk_t  *dlgn_findreply(GN_LOGDEV *, unsigned short, unsigned long);
extern int     dlgn_loopback(GN_LOGDEV *, mblk_t *);
extern void    dlgn_reply(GN_LOGDEV *, mblk_t *);
extern void    dlgn_busywait(int);
extern void    dlgn_wqreply(queue_t *, register mblk_t *, int);
extern long    dlgn_getpeak(void);
extern void    dlgn_msg(int, const char *, ...) __attribute__ ((__format__(__printf__, 2, 3)));
#else
extern mblk_t  *dlgn_copymp();
extern mblk_t  *dlgn_findreply();
extern int     dlgn_loopback();
extern void    dlgn_reply();
extern void    dlgn_busywait();
extern void    dlgn_wqreply();
extern long    dlgn_getpeak();
extern void    dlgn_msg();
#endif


/*
 * Prototypes for public Sync Module functions provided for
 * backward compatability.
 */
#ifdef LFS
extern void    dl_setintr(int(*)(void));
extern void    d4_casintr(void);
#else
extern void    dl_setintr();
extern void    d4_casintr();
#endif

/* Global data provided for backward compatability */
extern unsigned short cascade_eoi;


#endif
