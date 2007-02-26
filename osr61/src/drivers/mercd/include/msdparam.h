/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdparam.h
 * Description                  : parameter definitions
 *
 *
 **********************************************************************/

#ifndef _MDPARAM_
#define _MDPARAM_

#define MD_SUCCESS	0
#define MD_FAILURE	1
#define MD_MEM_FAILURE  2
#define MD_PENDING      3

#ifdef PCI
// This number needs to be same as MSD_MAX_BOARD_ID_COUNT
// from msddrvparam.h file
#define MD_MAX_CONFIG_ID_COUNT	21	/* physical boards */
#define MD_MAX_ADAPTER_COUNT	256	/* api board numbers */
#else
#define MD_MAX_ADAPTER_COUNT	21	/* api numbers and physical boards */
#endif
#define MD_MAX_PAMS		1

// Moved to msddrvparam.h
// #define MD_MAX_STREAM_ID	300

#define MD_MAX_INTR_LEVEL	7
#define MD_MAX_INTR_VECTOR	0x000000ff

#define MD_MAX_STREAM_SEND_BYTE_COUNT	0x10000
#define MD_MIN_STREAM_SEND_BYTE_COUNT	0x1000
#define MD_MAX_STREAM_PARAM_COUNT	2

#define MD_MAX_CAN_TAKE_BLOCK_COUNT	140

#define MD_MAX_GET_PARAM_COUNT	2
#define MD_MAX_PARAMETER_CHAR_VALUE	50
#define MD_MAX_PARAMETER_ULONG_VALUE (MD_MAX_PARAMETER_CHAR_VALUE>>2)

#define MERC_MERCURY_PAM_ID	0

/* Msg levels */
#define MD_LEV1		0x0000000f
#define MD_LEV2		0x00000007
#define MD_LEV3		0x00000003
#define MD_LEV4		0x00000001

#define MD_MAX_DRIVER_BODY_SZ	48

#define MERC_BUFFER_UNAVAIL_FAIL 2   


#endif


