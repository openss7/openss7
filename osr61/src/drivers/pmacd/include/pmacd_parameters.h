/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_parameters.h
// 
//  Functions for accessing the pmac driver parameters from 
//  user space.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_PARAMETERS_H
#define _PMACD_PARAMETERS_H

#include "pmacd.h"

int                 pmacd_getDriverParm(pmacd_ioctlParm_t *parm);
int                 pmacd_getBoardParm(pmacd_ioctlBoardParm_t *parm);

int                 pmacd_setDriverParm(pmacd_ioctlParm_t *parm);
int                 pmacd_setBoardParm(pmacd_ioctlBoardParm_t *parm);

#endif /* _PMACD_PARAMETERS_H */
















