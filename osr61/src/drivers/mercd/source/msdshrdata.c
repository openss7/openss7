/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdshrdata.c
 * Description			: Shared data across the Driver
 *
 *
 **********************************************************************/

#include "msd.h"

#define _MSDSHRDATA_C_
// In this module all the global variables are declared

//pmercd_control_block_sT MsdControlBlock;

merc_char_t   MsdDriverVersion[100];
mercd_mutex_T config_map_table_mutex;
mercd_mutex_T mercd_open_list_mutex;

MSD_BIND_MAP_ENTRY      MsdBindMapTable[MSD_ABSOLUTE_MAX_BIND];
merc_uint_t             mercd_adapter_map[MSD_MAX_ADAPTER_COUNT+1];
merc_uint_t             mercd_adapter_log_to_phy_map_table[MSD_MAX_ADAPTER_COUNT+1];

MSD_QUEUE               MsdOpenList;

// Array to store profiles information

typedef void (*MERCD_ABSTARCT_FUNC)(void *rcvPtr);
MERCD_INTERFACE_FUNC  mercd_osal_mid_func[MAX_INTERFACE_FUNCTION];
MERCD_ABSTRACT_FUNC mercd_osal_func[MAX_OSAL_FUNC];
MERCD_ABSTRACT_FUNC mercd_dhal_func[MAX_DHAL_FUNC];

//PMSD_MUTEX_STATS pmutex_stats;

#undef _MSDSHRDATA_C_
