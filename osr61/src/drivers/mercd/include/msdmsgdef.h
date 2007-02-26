/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdmsgdef.h
 * Description                  : message structure definitions
 *
 *
 **********************************************************************/

#ifndef _MDMSGDEF_
#define _MDMSGDEF_

/* Mercury Driver address struct */
typedef struct _MD_BIND_HANDLE{
#ifdef _8_BIT_INSTANCE
	MD_HANDLE	BindHandle;	/* Short */
#else
        MBD_HANDLE      BindHandle; 
#endif
	merc_uint_t	UserContext;
} MD_BIND_HANDLE, *PMD_BIND_HANDLE;

typedef union _MDRV_ADDRESS {
	MD_BIND_HANDLE	Bind;
	MD_HANDLE		StreamHandle;
} MDRV_ADDRESS, *PMDRV_ADDRESS;

/* Mercury Driver Main Message Definition */

typedef struct _MDRV_MESSAGE {
	MDRV_ADDRESS	MessageAddress;
	merc_ushort_t	MessageFlags;
	merc_ushort_t	MessageClass;
	merc_uint_t	MessageId;
	/* Was -> ULONG	Ulong1; */ /* Driver Internal */
	pmerc_void_t    MessagePtr; /* pointer to STREAM (mblk_t), changed Ulong1 to msgptr */
} MDRV_MSG, *PMDRV_MSG;

/* MDRV message flags */

#define MD_MSG_FLAG_ASYNC	0x0001
#define MD_MSG_FLAG_SYNC	0x0002
#define MD_MSG_FLAG_NO_ACK	0x0004
#define MD_MSG_FLAG_NACK	0x0008

/* Latency Improvement Flag */
#define MD_MSG_FLAG_LATENCY	0x20000000

/* MDRV_MESSAGE Message Classes */
// In New Driver there is only one Class
#define MD_CLASS_PAM		0x0001
#define MD_CLASS_CORE		0x0001
#define MD_CLASS_KAW		0x0001

//===============================================
#ifdef	ENABLE_NOT_REQUIRED_THINGS 
// In New Driver there are no SUB-CLASSES

/* Sub Classes */

/* Core Driver Class */

#define MD_SUBCLASS1_MASK	0xF0000000
#define MD_SUBCLASS1_STREAM	0x10000000
#define MD_SUBCLASS1_ADMIN	0x20000000
#define MD_SUBCLASS1_DISPATCHER	0x30000000
#define MD_SUBCLASS1_BUF_MGR	0x40000000

/* Various Admin Modules Class Ids */
#define MD_SUBCLASS2_MASK		0x0F000000
#define MD_SUBCLASS2_CONFIG		0x01000000
#define MD_SUBCLASS2_PHYSIO		0x02000000
#define MD_SUBCLASS2_MONITOR		0x03000000
#define MD_SUBCLASS2_DOWNLOAD		0x04000000

#endif	// ENABLE_NOT_REQUIRED_THINGS
//===============================================


// Ack Message Common definition 
#define MID_ACK_BITS			0x00008000

// generic ack message 
#define MID_ACK				0x0000ABCD

// Maximum messages listed below ......
// If you want to add a new messages, please increment the following counter..
// The reason why i did not put 0x42 (the max) is that, there is already a msg 
// type of 0xab, may be it never happens, better safe than to be sorry... 
#define MSD_MAX_DRV_INTER_MESSAGES	0x100

// Message Identifiers
// To achieve performance the Message Ids are allocated based on
// frequency of usage
#define MID_STREAM_SEND					0x00000001
#define MID_PAM_SEND_MERCURY			0x00000002

// DISPATCHER messages 
#define MID_DISPATCHER_BIND			0x00000003
#define MID_DISPATCHER_BIND_ACK		(MID_DISPATCHER_BIND|MID_ACK_BITS)
#define MID_DISPATCHER_UNBIND			0x00000004
#define MID_DISPATCHER_UNBIND_ACK 	(MID_DISPATCHER_UNBIND|MID_ACK_BITS)

// Bulk Stream messages
#define MID_STREAM_OPEN					0x00000005
#define MID_STREAM_CLOSE				0x00000006
#define MID_STREAM_RECEIVE				0x00000007
#define MID_STREAM_SET_PARAMETER		0x00000008
#define MID_STREAM_GET_PARAMETER		0x00000009
#define MID_STREAM_FLUSH				0x0000000A
#define MID_STREAM_CLOSE_INTERNAL	0x0000000B
#define MID_STREAM_CLOSE_SESS			0x0000000C
#define MID_STREAM_OPEN_ACK				(MID_STREAM_OPEN|MID_ACK_BITS)
#define MID_STREAM_CLOSE_ACK				(MID_STREAM_CLOSE|MID_ACK_BITS)
#define MID_STREAM_CLOSE_SESS_ACK		(MID_STREAM_CLOSE_SESS|MID_ACK_BITS)
#define MID_STREAM_SEND_ACK				(MID_STREAM_SEND|MID_ACK_BITS)
#define MID_STREAM_GET_PARAMETER_ACK	(MID_STREAM_GET_PARAMETER|MID_ACK_BITS)
#define MID_STREAM_SET_PARAMETER_ACK	(MID_STREAM_SET_PARAMETER|MID_ACK_BITS)

#define MID_PAM_BOARD_START			0x00000010
#define MID_PAM_BOARD_START_ACK 		(MID_PAM_BOARD_START|MID_ACK_BITS)
#define MID_CFG_DRIVER_START			0x00000011
#define MID_CFG_DRIVER_SHUTDOWN		0x00000012
#define MID_CFG_PAM_START				0x00000013
#define MID_CFG_PAM_SHUTDOWN			0x00000014
#define MID_CFG_BOARD_CONFIG			0x00000015
#define MID_CFG_BOARD_SHUTDOWN		0x00000016
#define MID_CFG_BOARD_SET_EVENT		0x00000017
#define MID_CFG_GET_EVENT_MASK		0x00000018
#define MID_CFG_GET_EVENT_MASK_ACK	(MID_CFG_GET_EVENT_MASK|MID_ACK_BITS)
#define MID_CFG_GET_PARAMETER			0x00000019
#define MID_CFG_GET_PARAMETER_ACK	(MID_CFG_GET_PARAMETER|MID_ACK_BITS)
#define MID_GET_BOARD_CONFIG			0x0000001A
#define MID_GET_BOARD_CONFIG_ACK		(MID_GET_BOARD_CONFIG|MID_ACK_BITS)
#define MID_CFG_MAP_BOARD				0x0000001B
#define MID_CFG_GET_PHYS_BOARD_ATTR	0x0000001C
#define MID_CFG_GET_PHYS_BOARD_ATTR_ACK\
												(MID_CFG_GET_PHYS_BOARD_ATTR|MID_ACK_BITS)
#define MID_CFG_GET_BOARD_STATE		0x0000001D
#define MID_CFG_GET_BOARD_STATE_ACK\
												(MID_CFG_GET_BOARD_STATE|MID_ACK_BITS)
#define MID_CFG_GET_CONFIG_ROM		0x0000001E
#define MID_CFG_GET_CONFIG_ROM_ACK\
												(MID_CFG_GET_CONFIG_ROM|MID_ACK_BITS)
#define MID_CFG_SCAN_BOARD				0x0000001F

// Monitor module messages 
#define MID_MON_GET_STATUS				0x00000020
#define MID_MON_GET_STATUS_ACK		(MID_MON_GET_STATUS|MID_ACK_BITS)


// Physical Io Messages 
#define MID_PHYSIO_BOARD_OPEN			0x00000021
#define MID_PHYSIO_BOARD_READ			0x00000022
#define MID_PHYSIO_BOARD_READ_ACK	(MID_PHYSIO_BOARD_READ|MID_ACK_BITS)
#define MID_PHYSIO_BOARD_WRITE		0x00000023
#define MID_PHYSIO_READ_INTR_COUNT	0x00000024
#define MID_PHYSIO_WAIT_INTR			0x00000025
#define MID_PHYSIO_WAIT_INTR_ACK		(MID_PHYSIO_WAIT_INTR|MID_ACK_BITS)
#define MID_PHYSIO_BOARD_CLOSE		0x00000026


#define MID_PAM_RECEIVE_MERCURY				0x00000030
#define MID_PAM_GET_PANIC_DUMP				0x00000031
#define MID_PAM_GET_PANIC_DUMP_ACK			(MID_PAM_GET_PANIC_DUMP|MID_ACK_BITS)
#define MID_PAM_SET_BOOT_HOST_RAM_BIT		0x00000032
#define MID_PAM_GET_BOOT_HOST_RAM_BIT		0x00000033
#define MID_PAM_GET_BOOT_HOST_RAM_BIT_ACK\
											 (MID_PAM_GET_BOOT_HOST_RAM_BIT|MID_ACK_BITS)

// Exit Notification related bind command
#define MID_EXIT_NOTIFY_BIND			0x00000034

// Multiple block send
#define MID_STREAM_MBLK_SEND			0x00000035

// Debug Port Read Command seperated from MID_CFG_GET_BOARD_STATE
#define MID_CFG_GET_BRD_DIAG_STATE	0x00000036
#define MID_CFG_GET_BRD_DIAG_STATE_ACK\
                                    (MID_CFG_GET_BRD_DIAG_STATE|MID_ACK_BITS)

// Downloader will set the Driver state after the download is complete
#define MID_CFG_SET_BRD_STATE_DNLD	0x00000037
#define MID_CFG_SET_BRD_STATE_DNLD_ACK\
                                    (MID_CFG_SET_BRD_STATE_DNLD|MID_ACK_BITS)

// Driver trace enabling and disabling IOCTLs
#define MID_ENABLE_DRV_TRACE			0x00000038
#define MID_ENABLE_DRV_TRACE_ACK\
                                    (MID_ENABLE_DRV_TRACE|MID_ACK_BITS)

#define MID_DISABLE_DRV_TRACE			0x00000039
#define MID_DISABLE_DRV_TRACE_ACK\
                                    (MID_DISABLE_DRV_TRACE|MID_ACK_BITS)

#define MID_GET_DRV_TRACE       		0x0000003A
#define MID_GET_DRV_TRACE_ACK			(MID_GET_DRV_TRACE|MID_ACK_BITS)


//WW support
#define MID_SET_WW_PARAM                    0x0000003B
#define MID_SET_WW_PARAM_ACK             (MID_ACK_BITS | MID_SET_WW_PARAM)
#define MID_GET_WW_PARAM                    0x0000003C
#define MID_GET_WW_PARAM_ACK             (MID_ACK_BITS | MID_GET_WW_PARAM)
#define MID_SET_WW_MODE                     0x0000003D
#define MID_SET_WW_MODE_ACK              (MID_ACK_BITS  | MID_SET_WW_MODE)
#define MID_GET_WW_MODE_OPERATIONAL                 0x0000003E
#define MID_GET_WW_MODE_OPERATIONAL_ACK  (MID_ACK_BITS | MID_GET_WW_MODE_OPERATIONAL)
#define MID_GET_WW_MODE                             0x0000003F
#define MID_GET_WW_MODE_ACK              (MID_ACK_BITS | MID_GET_WW_MODE)



//#ifdef DRVR_STATISTICS
#define MID_GET_STATISTICS      0x00000040
#define MID_GET_STATISTICS_ACK  (MID_ACK_BITS | MID_GET_STATISTICS)
//#endif

#define MID_SWITCH_DRV_TRACE                    0x00000041
#define MID_SWITCH_DRV_TRACE_ACK                (MID_SWITCH_DRV_TRACE|MID_ACK_BITS)

#define MID_SELECT_TRACE                        0x00000042
#define MID_SELECT_TRACE_ACK                    (MID_SELECT_TRACE|MID_ACK_BITS)

//WWSupport: Streaming Change
#define MID_START_WW_STREAM_READ           0x00000043
#define MID_START_WW_STREAM_READ_ACK       (MID_ACK_BITS | MID_START_WW_STREAM_READ)   

#define MID_CFG_SET_POST_STATE                  0x00000044
#define MID_CFG_SET_POST_STATE_ACK              (MID_CFG_SET_POST_STATE|MID_ACK_BITS)

#define MID_CFG_GET_POST_STATE                  0x00000045
#define MID_CFG_GET_POST_STATE_ACK              (MID_CFG_GET_POST_STATE|MID_ACK_BITS)

//WWSupport: DEBUG Only
#define MID_START_WW_STRM_DEBUG  0x00000046

#define MID_BRDID_TO_CFGID  0x00000047
#define MID_BRDID_TO_CFGID_ACK (MID_BRDID_TO_CFGID | MID_ACK_BITS)

#define MID_GET_SRAM        0x00000048
#define MID_GET_SRAM_ACK    (MID_GET_SRAM | MID_ACK_BITS)

#define MID_STREAM_SEND_TERMINATE               0x00000049
#define MID_STREAM_SEND_TERMINATE_ACK           (MID_STREAM_SEND_TERMINATE|MID_ACK_BITS)

#define MID_GET_POWER_STATUS        0x00000050
#define MID_GET_POWER_STATUS_ACK    (MID_GET_POWER_STATUS | MID_ACK_BITS)

//========================================================================

// IOCTL Command Message Type 
#define MD_COMMAND_IOCTL		0x000000ab

//	All error messages are moved to qdrverr.h, and that file is
//	included here.
#include "qdrverr.h"

// Macros to move a 16bit bind ID into two 8bit parts of the component
// descriptor and to get the value back again

#ifdef _8_BIT_INSTANCE
#define  COMP2BIND(b,c)    (b=((c)->component));\
                           (b=(b<<8));\
                           (b|=((c)->instance));

#define  BIND2COMP(b,c) ((c)->component=(b>>8));\
                        ((c)->instance=(b&0x00FF));\
                        ((c)->board=0);\
                        ((c)->node=0);\
                        ((c)->processor=0)
#else
#define  COMP2BIND(b,c)    (b=((c)->component));\
                           (b=(b<<16));\
                           (b|=((c)->instance));

#define  BIND2COMP(b,c) ((c)->component=((b>>16) & 0x00FF));\
                        ((c)->instance=(b&0xFFFF));\
                        ((c)->board=0);\
                        ((c)->node=0);\
                        ((c)->processor=0)
#endif

// Defining the Secondary board id's
#ifdef MERCD_MERC
#define MD_SCP_ID_1               MD_VMSCP_ID
#define MD_SCP_ID_2               MD_VMSCP_ID
#endif

#ifdef MERCD_PCI
#define MD_SCP_ID_1               MD_PMSCP_ID_1
#define MD_SCP_ID_2               MD_PMSCP_ID_2
#endif

#endif
