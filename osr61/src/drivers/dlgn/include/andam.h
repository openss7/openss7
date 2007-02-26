/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : andam.h
 * Description                  : antares device access module header
 *
 *
 **********************************************************************/

#ifndef __ANDAM_H__
#define __ANDAM_H__

/***** Antares Device Access Module entry points for DLGN and CFD *****/
#if defined(_SCO_) || defined(LINUX)
int anstartpm(GN_BOARD *, char *, ULONG, ULONG, ULONG);
int anstoppm(GN_BOARD *, ULONG);
#else
extern int aninitpm();
extern int anshutpm();
#endif /* _SCO_ */
int anstartbd(GN_BOARD *);
int anstopbd(GN_BOARD *);
int anopen(GN_LOGDEV *);
int anclose(GN_LOGDEV *, ULONG);
int ansendcmd(GN_LOGDEV *, mblk_t *);
#ifdef _SCO_
#ifdef LFS
extern int anrecvcmd(int);
#else
extern int anrecvcmd();
#endif
#endif /* _SCO_ */
int anmemsz(ULONG *, ULONG *, char **);
int andebug(mblk_t **, int *);
#ifdef _STATS_
int anstats(mblk_t **);
#endif /* _STATS_ */
#endif
