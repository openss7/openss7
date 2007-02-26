/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msddrvparam.h
 * Description                  : internal parameter definitions
 *
 *
 **********************************************************************/

#ifndef _MSDDRVPARAM_
#define _MSDDRVPARAM_

#define MSD_MAX_OPEN_ALLOWED        30000  // Maximum Open allowed (KAW_OPEN)
#define MSD_ABSOLUTE_MAX_BIND       MSD_MAX_OPEN_ALLOWED
#define MSD_ABSOLUTE_MAX_STREAMS    MSD_MAX_OPEN_ALLOWED

#if defined LiS || defined LIS || defined LFS
#define MSD_INITIAL_OPEN_ALLOWED    256 * 2
#else
#define MSD_INITIAL_OPEN_ALLOWED    256 * 16
#endif /* LiS */
#define MSD_INITIAL_MAX_BIND 	    MSD_INITIAL_OPEN_ALLOWED
#define MSD_INITIAL_MAX_STREAMS	    MSD_INITIAL_OPEN_ALLOWED

// This number needs to be same as MD_MAX_CONFIG_ID_COUNT
// from msdparam.h file

#define MSD_MAX_BOARD_ID_COUNT      100    // Maximum number of Boards(CONFIG_ID)
#define MSD_CONFIG_ROM_MAX_SIZE     0x100 // Config Rom Size
#define MSD_MAX_ADAPTER_COUNT  		MSD_MAX_BOARD_ID_COUNT 	
					// Same as MD_MAX_AD...

#define SEND_QUEUE_LIMIT_MULTIPLIER     16  /* for old streamconnect ack */
#define SEND_QUEUE_LIMIT_MULTIPLIER2    4   /* for setting params */
#define SEND_QUEUE_LIMIT_DIVISOR        4

// Mercury Driver Up-Stream IOCTL commands
#define MSD_REQ_GET_STREAM_DATA		0x00000001

// Mercury Driver Stream Queue definations, derived from OS queus
typedef queue_t      STRM_QUEUE;
typedef queue_t      *PSTRM_QUEUE;

#define MSD_MAX_STREAM_ID 600

// To resolve forward referencing problems
typedef struct _MSD_STREAM_CONN_BLOCK_ *PMSD_STREAM_CONN_BLOCK;
typedef struct _MERC_DEFERRED_Q_BLOCK *PMERC_DEFERRED_Q_BLOCK;

// Some macros

#define MD_GET_ADAPTER_DATA_BLOCK_SIZE(padapter) \
	(padapter->phost_info->host_config_ack.blockCount)

#define MD_GET_ADAPTER_PROTO_VERSION(padapter) \
	(padapter->phost_info->host_config_ack.blockCount)

#define MD_GET_ADAPTER_HWINT_INTERVAL(padapter) \
	(padapter->phost_info->host_config.hwIntInterval)

#define MD_GET_ADAPTER_DBLOCK_LIMT(padapter) \
	(padapter->phost_info->host_config.dataBlockLimit)

#define MD_GET_ADAPTER_EBLOCK_LIMT(padapter) \
	(padapter->phost_info->host_config.extBlockLimit)

#define MD_GET_ADAPTER_MERCINT_LEVEL(padapter) \
	(padapter->phost_info->host_config.vmeIntLevel)

#define MD_GET_ADAPTER_MERCINT_VECTOR(padapter) \
	(padapter->phost_info->host_config.vmeIntVector)

#endif // _MSDDRVPARAM_

