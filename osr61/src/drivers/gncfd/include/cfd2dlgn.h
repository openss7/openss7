/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : cfd2dlgn.h
 * Description                  : dlgn.c and cfd.c header
 *
 *
 **********************************************************************/

#ifndef __CFD2DLGN_H__
#define __CFD2DLGN_H__

#ifdef __STANDALONE__            /* Required header files */
#include "gndrv.h"
#endif


/*
 * Global variables defined in cfd.c. See cfd.c for a description of 
 * each variable. NOTE: dlgn.c TREATS THESE AS READ-ONLY!
 */
extern GN_BOARD    *Gn_Boardp;
extern GN_LOGDEV   *Gn_Logdevp;
extern char        *Gn_Freep;


#endif

