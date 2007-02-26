/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dtrace.h
 * Description                  : driver trace definitions
 *
 *
 **********************************************************************/

#ifndef _DTRACE_
#define _DTRACE_

#include "msdtypes.h"

#define MSD_MAX_TRACE_BUF_SIZE      10500
#define MSD_MAX_TRACE_MSGS          1000
#define MSD_MAX_TRACE_FILTER_MSGS     31
#define MSD_DUMPTHRESHOLD_FACTOR 3
#define NO_STREAMID -1

typedef struct _mercd_trace_copy_sT {

    merc_uint_t     FlagTransactionId;
    merc_uint_t     MessageType;
    merc_uint_t     Source;
    merc_uint_t     Destination;
    merc_uint_t     SourceDestComponent;
    merc_uint_t     BodySize;
    merc_uint_t     Reserved;
    merc_ulong_t    TimeStamp;
    merc_ulong_t    TimeUSec;
}mercd_trace_copy_sT, *pmercd_trace_copy_sT;

#endif

