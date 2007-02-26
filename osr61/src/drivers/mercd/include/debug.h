/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : debug.h
 * Description                  : debug definitions
 *
 *
 **********************************************************************/

#include <stdarg.h>

#ifdef DEBUGPRT

/* Externs needed for calls to drv_printf() */

extern  int DebugLevel;


/* Prototype */
int DebugPrintf(char *,...);


/* 
 * Dprintf guidelines
 *
 * Use:
 * dprintf  - for any error that you want to log always
 * dprintf1 - simple
 * dprintf2 - moderate 
 * dprintf3 - lots of logging
 * dprintf4 - for any logging in Interrupt routines (potentially expensive)
 * dprintf5 - for really obnoxious uneeded logging (e.g. "Entering Init function")
 */

#define  dprintf        DebugPrintf
#define  dprintf1       if ((DebugLevel >=1 )) DebugPrintf
#define  dprintf2       if ((DebugLevel >=2 )) DebugPrintf
#define  dprintf3       if ((DebugLevel >=3 )) DebugPrintf
#define  dprintf4       if ((DebugLevel >=4 )) DebugPrintf
#define  dprintf5       if ((DebugLevel >=5 )) DebugPrintf


#endif /* DEBUGPRT*/
