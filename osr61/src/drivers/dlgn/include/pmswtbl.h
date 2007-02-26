/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : pmswtbl.h
 * Description                  : PM switch table
 *
 *
 **********************************************************************/

#ifndef __PMSWTBL_H__
#define __PMSWTBL_H__

/*
 * PM_SWTBL: The PM_SWTBL is used by the Sync Module & Config Driver
 * to access the PM functions. A Protocol Module ID is used to index 
 * the PM switch table. Each PM must provide the functions listed in
 * the structure; these functions must be named xxxinit, xxxopen, 
 * xxxclose, etc. where xxx is the prefix used for the PM (such as
 * vox, dti, and so on).
 */
typedef struct pm_swtbl {
#ifdef LFS
   int (*startpm)(GN_BOARD *, char *, ULONG, ULONG, ULONG);          /* PM startup routine */
   int (*stoppm)(GN_BOARD *, ULONG);           /* PM shutdown routine */
   int (*startbd)(GN_BOARD *);          /* PM board startup routine */
   int (*stopbd)(GN_BOARD *);           /* PM board shutdown routine */
   int (*open)(GN_LOGDEV *);             /* PM open routine */
   int (*close)(GN_LOGDEV *, ULONG);            /* PM close routine */
#ifdef sun
   int (*ioctl)(void);            /* PM ioctl handler */
   int (*sendcmd)(GN_LOGDEV *, mblk_t *);          /* PM command handler */
   int (*recvcmd)(void);          /* PM command handler */
   int (*intr)(int);             /* PM interrupt routine */
#ifdef VME_LIVE_INSERTION
   int (*addbd)(void);            /* PM add board handler */
   int (*removebd)(void);         /* PM remove board handler */
#endif /* VME_LIVE_INSERTION  */
   int (*ioccmd)(void);           /* PM ioctl command handler */
#else
   int (*sendcmd)(GN_LOGDEV *, mblk_t *);          /* PM command handler */
   int (*intr)(int);             /* PM interrupt routine */
   int (*memsz)(ULONG *, ULONG *, char **);            /* PM dev-dep memory routine */
   int (*debug)(GN_DEBUG *, int *);            /* PM debug routine */
#ifdef _STATS_
   int (*stats)(PM_STATS *);            /* PM statistics routine */
#endif /* _STATS_ */
#endif /*  sun */
#else
   int (*startpm)();          /* PM startup routine */
   int (*stoppm)();           /* PM shutdown routine */
   int (*startbd)();          /* PM board startup routine */
   int (*stopbd)();           /* PM board shutdown routine */
   int (*open)();             /* PM open routine */
   int (*close)();            /* PM close routine */
#ifdef sun
   int (*ioctl)();            /* PM ioctl handler */
   int (*sendcmd)();          /* PM command handler */
   int (*recvcmd)();          /* PM command handler */
   int (*intr)();             /* PM interrupt routine */
#ifdef VME_LIVE_INSERTION
   int (*addbd)();            /* PM add board handler */
   int (*removebd)();         /* PM remove board handler */
#endif /* VME_LIVE_INSERTION  */
   int (*ioccmd)();           /* PM ioctl command handler */
#else
   int (*sendcmd)();          /* PM command handler */
   int (*intr)();             /* PM interrupt routine */
   int (*memsz)();            /* PM dev-dep memory routine */
   int (*debug)();            /* PM debug routine */
#ifdef _STATS_
   int (*stats)();            /* PM statistics routine */
#endif /* _STATS_ */
#endif /*  sun */
#endif
} PM_SWTBL;

/*
 * Create a reference for the PM_SWTBL structure to be used by the
 * various driver modules. The actual declaration is in the Space.c file
 * for the Generic Driver. This file is built, compiled, and linked at
 * install time according to the information in the .mastercfg file
 * (which is also updated at install time). See the Dialogic UNIX
 * installation utilities, addpm and delpm for more info.
 */
extern PM_SWTBL pmswtbl[];

/*
 * Create a reference for the Gn_Maxpm variable that is used by the
 * Config Driver & Sync Module to determine the number of PM's that have
 * been built into the PM_SWTBL. The actual declaration is in the Space.c
 * file for the Generic Driver. This file is built, compiled, and linked
 * at install time according to the information in the .mastercfg file
 * (which is also updated at install time). See the Dialogic UNIX
 * installation utilities, addpm and delpm for more info.
 */
extern unsigned long Gn_Maxpm;


#endif

