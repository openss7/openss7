/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : include.h
 * Description                  : main include
 *
 *
 **********************************************************************/

#if (defined(UNIX) || defined(WNT_ANT))
#define far
#define huge
#define near
#endif
#include "antares.h"
#include "anterror.h"
#include "typedef.h"

#ifdef ANT_CNVRTR_BRD
#define FORCE_16_BITS
#endif
