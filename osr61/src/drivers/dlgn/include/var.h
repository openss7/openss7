/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : var.h
 * Description                  : interface global variables
 *
 *
 **********************************************************************/

#if defined(_SCO_) || defined(LINUX)
PRC far *p_data;  /* process connected to this opened bd_channel */
#else
PRC *p_data;  /* process connected to this opened bd_channel */
#endif /* _SCO_ */

SHORT d_nboards;  /* Number of boards in system */
