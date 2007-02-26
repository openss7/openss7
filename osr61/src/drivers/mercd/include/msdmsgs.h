/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdmsgs.h
 * Description                  : message structure definitions
 *
 *
 **********************************************************************/

#ifndef _MDMSGS_
#define _MDMSGS_

#include "dtrace.h"

typedef struct _MD_ACK  {
	merc_uint_t	MessageId;
	merc_uint_t	ErrorCode;
	merc_uint_t	ReturnValue;
} MD_ACK,*PMD_ACK;


typedef struct _MID_CFG_DRIVER_START  {
	merc_uint_t	MaxBind;
	merc_uint_t	MaxStreamConnections;
} CFG_DRIVER_START, *PCFG_DRIVER_START  ;


typedef struct _MID_CFG_PAM_START  {
	merc_uint_t	PamId;
	merc_uint_t	Flags;
} CFG_PAM_START ,*PCFG_PAM_START  ;

typedef struct _MID_CFG_PAM_SHUTDOWN{
	merc_uint_t	PamId;
	merc_uint_t	Flags;
} CFG_PAM_SHUTDOWN,*PCFG_PAM_SHUTDOWN;

#define MD_CONFIG_ROM_MAX_SIZE	0x100

typedef struct _CFG_GET_CONFIG_ROM{
	merc_ushort_t	ConfigId;	/* Used by Kaw for mainly dip */
	merc_uchar_t	ConfigRom[MD_CONFIG_ROM_MAX_SIZE];
} CFG_GET_CONFIG_ROM, *PCFG_GET_CONFIG_ROM;

typedef struct _CFG_MAP_BOARD {
	merc_ushort_t	ConfigId;	/* Used by Kaw for mainly dip */
	merc_ushort_t	BoardNumber;	/* Used by api */
} CFG_MAP_BOARD, *PCFG_MAP_BOARD;

typedef struct _CFG_SCAN_BOARD {
	merc_ushort_t	Slot;	/* Used by Kaw for PhysAddr */
} CFG_SCAN_BOARD, *PCFG_SCAN_BOARD;

typedef struct _CFG_GET_PHYS_BOARD_ATTR_ACK {
	merc_uint_t	BoardCount;
} CFG_GET_PHYS_BOARD_ATTR_ACK, *PCFG_GET_PHYS_BOARD_ATTR_ACK;

typedef struct _MD_PHYS_BOARD_ATTR {
	merc_ushort_t	ConfigId;
	merc_ushort_t	BusType;	/* BUS_TYPE_MERC or BUS_TYPE_PCI */
	merc_uchar_t	BusId;		/* Bus number where board is installed */
	merc_uchar_t	SlotNumber;	/* Slot number where board is installed */
	merc_uint_t	PhysSramAddr;	/* Board's shared ram physical addr */
	merc_uint_t	PhysSramSize;	/* Board's shared ram physical size */
	merc_ushort_t	IoPortStart;	/* Board's I/O registers addr; 0=not used */
	merc_ushort_t	IoPortSize;	/* Board's I/O registers size */
	merc_ushort_t	InterruptLevel;	/* PCI irq or MERC intr level */
	merc_ushort_t	InterruptVector;/* MERC Interrupt vector */
	merc_uchar_t	InterruptSharable;/* is Interrupt sharable?; 0=no, 1=yes */
	merc_char_t	DmaChannel;	/* Dma channel; -1=not used */
        //PCI Slot Number for Listboards
        merc_uchar_t    pciSlotNumber;
} MD_PHYS_BOARD_ATTR, *PMD_PHYS_BOARD_ATTR;

typedef struct _MD_GET_POWER_STATUS { 
   	merc_uchar_t   boardIndex;			/* Physcial Board Id */
   	merc_uint_t    powerGoodRegValue;  		/* Power Good Status */
   	merc_uint_t    powerOverrideRegValue;		/* Power Management Override */
   	merc_uint_t    actualPowerProvidedValue;	/* Actual Power Provided */
   	merc_uint_t    powerRequiredByBoardValue;	/* Power required by board */
   	merc_uint_t    busNumber;			/* Bus number the board is placed in */
   	merc_uint_t    slotNumber;			/* Slot Number the board is placed in */
} MD_GET_POWER_STATUS, *PMD_GET_POWER_STATUS;


typedef struct _CFG_GET_BOARD_STATE_ACK{
	merc_uint_t	BoardCount;
} CFG_GET_BOARD_STATE_ACK, *PCFG_GET_BOARD_STATE_ACK;

typedef struct _MD_BOARD_STATE {
	merc_ushort_t	ConfigId;		/* Used by Kaw for mainly dip */
	merc_ushort_t	BoardNumber;	/* Used by api */
	merc_ushort_t	BoardState;		/* see below, INITED, CONFIGED, STARTED */
	merc_ushort_t	Reserved;		/* for future use */
	merc_ushort_t	DiagRetVal1;	/* Not Used, Check the GET_BRD_DIAG_STATE */
	merc_ushort_t	DiagRetVal2;	/* Not Used, Check the GET_BRD_DIAG_STATE */
} MD_BOARD_STATE, *PMD_BOARD_STATE;

/* Board States */
#define BOARD_STATE_INITED		1
#define BOARD_STATE_MAPPED		2
#define BOARD_STATE_CONFIGED		3
#define BOARD_STATE_STARTED		4
#define BOARD_STATE_DOWNLOADED		5
#define BOARD_STATE_STOPPED		6
#define BOARD_STATE_OUT_OF_SERVICE	7
#define BOARD_STATE_CRASHED		8
#define BOARD_STATE_OUT_OF_SYNC		9
#define BOARD_STATE_SUSPENDED           10
#define BOARD_STATE_INVALID		11

// Defines for Board Diag Get command
typedef struct _CFG_GET_BRD_DIAG_STATE{
	merc_uint_t BoardNumber;
	merc_uint_t	BoardState;
	merc_uint_t	DiagRetVal1;		/* Daignostic Return Code from Kernel */
	merc_uint_t	DiagRetVal2;		/* Daignostic Return Code from Kernel */
} CFG_GET_BRD_DIAG_STATE, *PCFG_GET_BRD_DIAG_STATE;

// Defines for MID_CFG_SET_BRD_STATE_DNLD
typedef struct _CFG_SET_BRD_STATE_DNLD{
	merc_uint_t BoardNumber;
} CFG_SET_BRD_STATE_DNLD, *PCFG_SET_BRD_STATE_DNLD;

#define MD_MIN_POST_ENABLE                 1
#define MD_MAX_POST_ENABLE                 0

typedef struct _CFG_GET_BRD_POST_STATE{
        merc_uint_t ConfigId;
        merc_uint_t PostState;
} CFG_GET_BRD_POST_STATE, *PCFG_GET_BRD_POST_STATE;

typedef struct _CFG_SET_BRD_POST_STATE{
        merc_uint_t ConfigId;
        merc_uint_t PostState;
} CFG_SET_BRD_POST_STATE, *PCFG_SET_BRD_POST_STATE;


/* Actually 30 msgs are traced since 0th element of the
   array is used to store the number of elements in the
   array
*/
#define MSD_MAX_TRACE_FILTER_MSGS     31

// Defines for Driver Trace
typedef struct _CFG_ENABLE_DRV_TRACE{
  merc_uint_t BoardNumber;                               // Config id
  merc_uint_t TraceLevel;                                // not used
  merc_uint_t TraceMsgs;                                 // mem. allocated in driver depends on this
  merc_uint_t Criteria;                                  // not used
  merc_int_t SelectTrace;                                // Now using only SELECT mode
  merc_uint_t ThreshDump;                                // threshold for the driver to send trace data to app
  merc_uint_t b2dFilter[MSD_MAX_TRACE_FILTER_MSGS];      // b2d msgtype filters
  merc_uint_t d2bFilter[MSD_MAX_TRACE_FILTER_MSGS];      // d2b msgtype filters
  merc_uint_t b2dStrIdFilter[MSD_MAX_TRACE_FILTER_MSGS]; // b2d streamid filters
  merc_uint_t d2bStrIdFilter[MSD_MAX_TRACE_FILTER_MSGS]; // d2b streamid filters
  merc_uint_t SrcDesc[MSD_MAX_TRACE_FILTER_MSGS];        // src compdesc filters
  merc_uint_t DstDesc[MSD_MAX_TRACE_FILTER_MSGS];        // dest compdesc filters
  merc_uint_t dataTraceSizeLimit;                        // msg content size to be traced
} CFG_ENABLE_DRV_TRACE, *PCFG_ENABLE_DRV_TRACE;

// Defines for Driver Trace
typedef struct _CFG_SWITCH_DRV_TRACE{
        merc_uint_t BoardNumber;
        merc_uint_t TraceLevel;
        merc_uint_t Criteria;
} CFG_SWITCH_DRV_TRACE, *PCFG_SWITCH_DRV_TRACE;

typedef struct _CFG_DISABLE_DRV_TRACE{
	merc_uint_t BoardNumber;
	merc_uint_t Reserved;
} CFG_DISABLE_DRV_TRACE, *PCFG_DISABLE_DRV_TRACE;

// Defines for Trace Level
#define TRACE_LEVEL_ENTIRE_DRIVER       1

#if 0
#if 0
#define MSD_MAX_TRACE_BUF_SIZE      12288 // 3 * 4096
#define MSD_MAX_TRACE_MSGS          1755          // Max Trace Messages per board
#else
#define MSD_MAX_TRACE_BUF_SIZE      10500
#define MSD_MAX_TRACE_MSGS          1500
#endif
#endif

#define MSGS_TRACE_SELECT 1
#define MSGS_TRACE_RAW 0

typedef struct _CFG_GET_DRV_TRACE{
	merc_uint_t BoardNumber;
        merc_uint_t TraceCount;
        merc_int_t SelectFlag;
  //  merc_uint_t TraceStream;
} CFG_GET_DRV_TRACE, *PCFG_GET_DRV_TRACE;

typedef struct _CFG_DRV_TRACE_REPLY{
        merc_uint_t RetTraceLevel;
        merc_uint_t     Drv2BrdTraceIndex;
        merc_uint_t     Brd2DrvTraceIndex;
        merc_uint_t     Drv2BrdAllocated;
        merc_uint_t     Brd2DrvAllocated;
        merc_uint_t Brd2DrvTraceBuff[MSD_MAX_TRACE_BUF_SIZE];
        merc_uint_t Drv2BrdTraceBuff[MSD_MAX_TRACE_BUF_SIZE];
} CFG_DRV_TRACE_REPLY, *PCFG_DRV_TRACE_REPLY;

typedef struct _CFG_SELECT_TRACE_REPLY{
        merc_uint_t RetTraceLevel;
        merc_uint_t     Drv2BrdTraceIndex;
        merc_uint_t     Brd2DrvTraceIndex;
        merc_uint_t     Drv2BrdAllocated;
        merc_uint_t     Brd2DrvAllocated;
} CFG_SELECT_TRACE_REPLY, *PCFG_SELECT_TRACE_REPLY;
//=======================================

typedef struct _CFG_BOARD_CONFIG {
	merc_uint_t	PamId;
	merc_uint_t	SharedRamStart;
	merc_uint_t	SharedRamSize;
	merc_ushort_t	BoardNumber;
	merc_ushort_t	InterruptLevel;
	merc_ushort_t	InterruptVector;
	merc_ushort_t	InterruptSharable;
	merc_ushort_t	IoPortStart;
	merc_ushort_t	IoPortSize;
	merc_short_t	DmaChannel;	/* -1 = not used */
	merc_ushort_t	BusType;
} CFG_BOARD_CONFIG, *PCFG_BOARD_CONFIG;

typedef struct _CFG_GET_SRAM {
	merc_uint_t     Size;
	merc_uint_t     CfgId;
} CFG_GET_SRAM, *PCFG_GET_SRAM;

typedef struct _STREAM_SEND_TERMINATE{
        merc_uint_t     StreamId;
	merc_uint_t	Mode;
} STREAM_SEND_TERMINATE,*PSTREAM_SEND_TERMINATE;

typedef struct _STREAM_SEND_TERMINATE_ACK{
        merc_uint_t     StreamId;
        merc_uint_t     ErrorCode;
} STREAM_SEND_TERMINATE_ACK,*PSTREAM_SEND_TERMINATE_ACK;

/* Interrupt Sharable */

#define MD_CFG_INTERRUPT_NOT_SHARABLE	0
#define MD_CFG_INTERRUPT_SHARABLE	1


#define MD_CFG_DMA_DISABLE		-1
#define MD_CFG_DMA_ENABLE		1

/* Bus Types */

#define BUS_TYPE_ISA 			0x0001
#define BUS_TYPE_EISA			0x0002
#define BUS_TYPE_VME			0x0003
#define BUS_TYPE_PCI			0x0004
#define BUS_TYPE_MICRO_CHANNEL		0x0005
#define BUS_TYPE_PCMCIA			0x0006
#define BUS_TYPE_PCI_EXPRESS		0x0007

/***** Define structures for WW_SET_PARAM ************************/
 
typedef struct {
	merc_ushort_t numberMFAInboundQ;// number of MFA's in the Inbound Q
	merc_ushort_t sizeofMFAInboundQ; // size of MFA in the Inbound Q
	merc_ushort_t numberMFAOutboundQ; // number of MFA's in the Outbound Q
	merc_ushort_t sizeofMFAOutboundQ;  // size of MFA for the outbound Q
	merc_ushort_t numberBigMsgBlocks;  // number of Big Msg blocks to be sent
				     // to the board
	merc_ushort_t sizeofBigMsgBlocks; // size of the big message blocks
	merc_ushort_t numberOfOrphanEnteries;  // number of Orphan Q enteries....
	//
	// The following fields are of no use for the driver.
	merc_ushort_t flowControlToBoard; // Flow Control to the board
	merc_ushort_t flowControlFromBoard; // Flow COntrol from the board
	merc_ushort_t hardwareTimeout;  // Timeout interval for the hardware
	merc_ushort_t maxNumStreamMFAAllowed;	// The max number of mfa's
						// allowed for the streams
						//on this board.

} WW_INIT_PARAM, *PWW_INIT_PARAM; 


typedef struct {
	//Counters for Interrupt Context
	merc_ulong_t unknown_intr;
	merc_ulong_t error_intrs;
	merc_ulong_t dpc_scheduled;

	//Counters for DPC Context
	merc_ulong_t dpc_called;
	merc_ulong_t dpc_ours;
	merc_ulong_t dpc_notours;
	merc_ulong_t read_ack_intr;
	merc_ulong_t msg_ready_intr;
	merc_ulong_t reset_interrupt_ack_from_board;
	merc_ulong_t cancel_req_intr_ack;
	merc_ulong_t reset_intr_from_brd;
	merc_ulong_t our_intr_but_unknown_type;

        //WW Initialization
        merc_ulong_t init_ack_intr;
        merc_ulong_t init_complete_intr;

	//Counters for RCV Context
	merc_ulong_t small_msgs_recd;
	merc_ulong_t big_msgs_recd;
	merc_ulong_t bigmsg_read_acks_recd;
	merc_ulong_t bigmsg_cancel_acks_recd;
	merc_ulong_t writestr_complete_recd;
	merc_ulong_t readstr_complete_recd;
	merc_ulong_t readstr_databufreq_recd;
	merc_ulong_t strm_flush_req_ack_recd;
	merc_ulong_t strm_cancel_req_ack_recd;
	merc_ulong_t readstr_eos_recd;
	merc_ulong_t bigmsg_inmf_start_recd;
	merc_ulong_t bigmsg_inmf_continue_recd;
	merc_ulong_t msg_type_rcvd_corrupted;
	merc_ulong_t valid_msgrdy_intr_but_no_mf;

	//Counters for SEND Context
	merc_ulong_t small_msgs_sent;
	merc_ulong_t big_msgs_sent;
	merc_ulong_t bigmsg_blk_mfs_sent;
	merc_ulong_t cancel_strm_mfs_sent;
	merc_ulong_t cancel_bigmsg_mfs_sent;
	merc_ulong_t strm_flush_mfs_sent;
	merc_ulong_t write_strm_mfs_sent;
	merc_ulong_t write_strm_multieos_mfs_sent;
	merc_ulong_t write_strm_eos_mfs_sent;
	merc_ulong_t readstr_databuf_post_mfs_sent;

	//InBound FIFO Stats
	merc_ulong_t total_inboundq_reads;
	merc_ulong_t nomf_inboundq_reads;
	merc_ulong_t outofrange_inboundq_reads;
	merc_ulong_t valid_inboundq_reads;
	merc_ulong_t total_inboundq_writes;

	//OutBound FIFO Stats
	merc_ulong_t total_outboundq_reads;
	merc_ulong_t outofrange_outboundq_reads;
	merc_ulong_t total_outboundq_writes;

        //DOORBELL Counters
        merc_ulong_t init_intr_db;
        merc_ulong_t init_intr_db_response;
        merc_ulong_t init_msg_ready_intr_db;
        merc_ulong_t init_msg_ready_intr_db_response;
        merc_ulong_t cancel_req_intr_db;
}WW_STATS, *PWW_STATS;

//////////////////////////////////////////////////////
typedef struct _MERCD_MUTEX_INFO{
       merc_uint_t wait_count;
       merc_uchar_t waitfile[25];
       merc_uint_t waitline;

       merc_uint_t acquired_count;
       merc_uchar_t enterfile[25];
       merc_uint_t enterline;

       merc_uchar_t exitfile[25];
       merc_uint_t exitline;
} MERCD_MUTEX_INFO, *PMERCD_MUTEX_INFO;

typedef struct _GET_MUTEX_STATS_ {
        MERCD_MUTEX_INFO up_soft_mutex_stats;
        MERCD_MUTEX_INFO up_mutex_stats;
        MERCD_MUTEX_INFO smp_mutex_stats;
        MERCD_MUTEX_INFO config_map_table_mutex_stats;
        MERCD_MUTEX_INFO open_list_mutex_stats;
        MERCD_MUTEX_INFO soft_mutex_stats;
        MERCD_MUTEX_INFO mutex_stats; /* MSD_ENTER_MUTEX or MSD_EXIT_MUTEX */
        MERCD_MUTEX_INFO hw_mutex_stats;
        MERCD_MUTEX_INFO control_block_mutex_stats;
}GET_MUTEX_STATS, *PGET_MUTEX_STATS;

//////////////////////////////////////////////////////


typedef struct _WW_SET_PARAM {
	        merc_uint_t     BoardNumber;
} WW_SET_PARAM, *PWW_SET_PARAM;
 
typedef struct _WW_GET_PARAM {
	        merc_uint_t     BoardNumber;
} WW_GET_PARAM, *PWW_GET_PARAM;
 
typedef struct _WW_SET_MODE { /* Set to WW mode */
	        merc_uint_t     BoardNumber;
} WW_SET_MODE, *PWW_SET_MODE;
 
typedef struct _WW_GET_MODE { /* Get the mode */
	        merc_uint_t     BoardNumber;
} WW_GET_MODE, *PWW_GET_MODE;
 
typedef struct _WW_STREAM_START_READ { /* Start Read Stream */
	        QBindHandle	bind;
	        merc_uint_t     StreamId;
}WW_STREAM_START_READ , *PWW_STREAM_START_READ;

/****************************************************************/
                                 
typedef struct _GET_BOARD_CONFIG {
	merc_uint_t	BoardNumber;
	merc_uint_t	Rfu;
} GET_BOARD_CONFIG, *PGET_BOARD_CONFIG;

typedef struct _GET_BOARD_CONFIG_ACK {
	merc_uint_t	BoardNumber;
	merc_uint_t	ErrorCode;
} GET_BOARD_CONFIG_ACK, *PGET_BOARD_CONFIG_ACK;

/* Defines for Flag field in the CFG_BOARD_SHUTDOWN structure */
#define MID_NORMAL_SHUTDOWN	0
#define MID_FORCED_SHUTDOWN	1

typedef struct _CFG_BOARD_SHUTDOWN {
	merc_uint_t		BoardNumber;
	merc_uint_t		Flag;
} CFG_BOARD_SHUTDOWN, *PCFG_BOARD_SHUTDOWN;

typedef struct _CFG_PARAMETER_ENTRY{
	merc_uint_t	Key;
	merc_uint_t	ErrorCode;
	union {
		merc_char_t	CharValue[MD_MAX_PARAMETER_CHAR_VALUE];
		merc_uint_t	UlongValue[MD_MAX_PARAMETER_ULONG_VALUE];
	} Value;
} CFG_PARAMETER_ENTRY, *PCFG_PARAMETER_ENTRY;

typedef struct _CFG_GET_PARAMETER{
	merc_uint_t	Count;
	merc_uint_t	Rfu;
} CFG_GET_PARAMETER, *PCFG_GET_PARAMETER;

typedef struct _CFG_GET_PARAMETER_ACK{
	merc_uint_t	Count;
	merc_uint_t	ErrorCode;
} CFG_GET_PARAMETER_ACK, *PCFG_GET_PARAMETER_ACK;

//#ifdef DRVR_STATISTICS
typedef struct _GET_STATISTICS{
        merc_uint_t     boardNum;
        merc_uint_t     Rfu;
} GET_STATISTICS, *PGET_STATISTICS;

typedef struct _GET_STATISTICS_ACK{
        merc_uint_t     boardNum;
        merc_uint_t     ErrorCode;
} GET_STATISTICS_ACK, *PGET_STATISTICS_ACK;

typedef struct _GET_STATISTICS_ENTRY{
        merc_uint_t     Key;
        merc_uint_t     ErrorCode;
        merc_ulong_t     bnHandles;
        merc_ulong_t     opHandles;
        merc_ulong_t     stHandles;
        merc_ulong_t    totalStHandles;
} GET_STATISTICS_ENTRY, *PGET_STATISTICS_ENTRY;
//#endif

/* Key definitions */
#define CFG_KEY_DRIVER_VERSION	0x00000001
#define CFG_KEY_BLOCK_SIZE	0x00000002
#define CFG_KEY_PROTO_VERSION	0x00000003
#define CFG_KEY_HWINT_INTERVAL	0x00000004
#define CFG_KEY_DBLOCK_LIMIT	0x00000005
#define CFG_KEY_EBLOCK_LIMIT	0x00000006
#define CFG_KEY_VMEINT_LEVEL	0x00000007
#define CFG_KEY_VMEINT_VECTOR	0x00000008
#define CFG_KEY_MAX_BIND	0x00000009
#define CFG_KEY_MAX_STRMS	0x0000000A

typedef struct _CFG_SET_EVENT {
	merc_uint_t	Command;
	merc_uint_t	Events;
	merc_uint_t	EventData;
} CFG_SET_EVENT, *PCFG_SET_EVENT;

#define MD_CFG_ADD_EVENT		0x00000001
#define MD_CFG_DELETE_EVENT		0x00000002
#define MD_CFG_ACTIVATE_ALL_EVENTS	0x00000003
#define MD_CFG_DEACTIVATE_ALL_EVENTS	0x00000004


/* 
EventData is set to the board number.  An asynchronous  
MID_EVENT_BOARD_DOWN message is sent to the bind instance 
upon board failure.
*/
#define MD_CFG_EVENT_BOARD_DOWN 	0x00000001

/* Need to OR all possible events for the ALL_EVENTS def */
#define MD_CFG_EVENT_ALL_EVENTS		(MD_CFG_EVENT_BOARD_DOWN)

typedef struct _CFG_GET_EVENT_MAKS_ACK {
	merc_uint_t		Events;
} CFG_GET_EVENT_MASK_ACK, *PCFG_GET_EVENT_MASK_ACK;

typedef struct _DISPATCHER_BIND {
	merc_uint_t	BindFlags;
	merc_uint_t	UserContext;
} DISPATCHER_BIND, *PDISPATCHER_BIND;

typedef struct _DISPATCHER_UNBIND {
	merc_uint_t	UnbindFlags;
} DISPATCHER_UNBIND, *PDISPATCHER_UNBIND;

/* Defines for Flag field in the DISPATCHER_UNBIND structure */
#define MID_NORMAL_UNBIND		0
#define MID_SEND_DEATHMSG		1

typedef struct _DISPATCHER_BIND_ACK {
#ifdef _8_BIT_INSTANCE
	merc_ushort_t	BindHandle;
#else
        merc_uint_t     BindHandle;
#endif
} DISPATCHER_BIND_ACK, *PDISPATCHER_BIND_ACK;

// Message foe Exit Notification Bind
typedef struct _MSD_EXIT_NOTIFY_ENTRY {
	merc_uint_t	BoardNumber;
	merc_uint_t	Flags;
} MSD_EXIT_NOTIFY_ENTRY, *PMSD_EXIT_NOTIFY_ENTRY;

typedef struct _MSD_DEATH_NOTIFY_ENTRY {
	merc_uint_t     BoardNumber;
	merc_uint_t     Flags;
} MSD_DEATH_NOTIFY_ENTRY, *PMSD_DEATH_NOTIFY_ENTRY;

typedef struct _DWNLD_START {
	merc_uint_t	BoardNumber;
	merc_uint_t	LoadAddressStart;
} DWNLD_START, *PDWNLD_START;

typedef struct _DWNLD_DATA{
	merc_uint_t	ByteCount;
} DWNLD_DATA, *PDWNLD_DATA;

typedef struct _DWNLD_DATA_ACK {
	merc_uint_t	CanTakeByteCount;
} DWNLD_DATA_ACK, *PDWNLD_DATA_ACK;

typedef struct _DWNLD_STOP {
	merc_uint_t	StopFlags;
} DWNLD_STOP, *PDWNLD_STOP;

typedef struct _DWNLD_END {
	merc_uint_t	EndFlags;
} DWNLD_END, *PDWNLD_END;

typedef struct _EVENT_LOST_BIND {
	merc_uint_t	Reason;
	merc_uint_t	Data;
} EVENT_LOST_BIND, *PEVENT_LOST_BIND;

#define REASON_BOARD_SHUTDOWN	0x00000001
#define REASON_PAM_SHUTDOWN	0x00000002
#define REASON_DRIVER_SHUTDOWN	0x00000003
#define REASON_BOARD_FAILURE	0x00000004

typedef struct _MON_GET_STATUS {
	merc_uint_t	TargetObject;
	merc_uint_t	TargetId;
} MON_GET_STATUS, *PMON_GET_STATUS;

#define MON_STATUS_BIND		0x00000001
#define MON_STATUS_BOARD	0x00000002

typedef struct _MON_GET_STATUS_ACK {
	merc_uint_t	Status;
} MON_GET_STATUS_ACK, *PMON_GET_STATUS_ACK;


typedef struct _PHYSIO_BOARD_OPEN{
	merc_uint_t	BoardNumber;
} PHYSIO_BOARD_OPEN, *PPHYSIO_BOARD_OPEN;


typedef struct _PHYSIO_BOARD_READ{
	merc_uint_t	BoardHandle;
	merc_uint_t	Offset;
	merc_uint_t	Size;
	merc_uint_t	Type;
} PHYSIO_BOARD_READ, *PPHYSIO_BOARD_READ;


#define MD_PHYSIO_MEMORY	1
#define MD_PHYSIO_IO_PORT	2


typedef struct _PHYSIO_BOARD_READ_ACK{
	merc_uint_t	BoardHandle;
	merc_uint_t	Offset;
	merc_uint_t	BytesRead;
	merc_uint_t	Type;
} PHYSIO_BOARD_OPEN_ACK, *PPHYSIO_BOARD_OPEN_ACK;


typedef struct _PHYSIO_BOARD_WRITE{
	merc_uint_t	BoardHandle;
	merc_uint_t	Offset;
	merc_uint_t	Size;
	merc_uint_t	Type;
} PHYSIO_BOARD_WRITE, *PPHYSIO_BOARD_WRITE;

typedef struct _PHYSIO_READ_INTR_COUNT{
	merc_uint_t	BoardHandle;
	merc_uint_t	Flag;
} PHYSIO_READ_INTR_COUNT, *PPHYSIO_READ_INTR_COUNT;

#define PHYSIO_FLAG_CLEAR_INTR_COUNT	0x00000001

typedef struct _PHYSIO_WAIT_INTR{
	merc_uint_t	BoardHandle;
} PHYSIO_WAIT_INTR, *PPHYSIO_WAIT_INTR;


typedef struct _PHYSIO_WAIT_INTR_ACK{
	merc_uint_t	BoardHandle;
} PHYSIO_WAIT_INTR_ACK, *PPHYSIO_WAIT_INTR_ACK;

typedef struct _PHYSIO_BOARD_CLOSE{
	merc_uint_t	BoardHandle;
} PHYSIO_BOARD_CLOSE, *PPHYSIO_BOARD_CLOSE;


typedef struct _PAM_BOARD_START {
	merc_uint_t	BoardNumber;
	merc_uint_t	PamTimeoutInterval;
	merc_uint_t	PamMessageSize;
} PAM_BOARD_START, *PPAM_BOARD_START;


typedef struct _MDRV_MERC_HOSTIF_CONFIG {
	merc_uint_t	ProtocolRevision;
	merc_uint_t	CheckSum;
	merc_uint_t	HostAreaSize;
	merc_uint_t	MaxHostMessages;
	merc_uint_t	MaxFwMessages;
	merc_uint_t	DataAlignment;
	merc_uint_t	MaxHostDataXfer;
	merc_uint_t	MaxFwDataXfer;
	merc_uint_t	HostConfigMask;
	merc_uint_t	HwTimeoutInterval;
	merc_uint_t	DataBlockLimit;
	merc_uint_t	ExtBlockLimit;
	merc_uint_t	BoardNumber;
	merc_uint_t	NodeNumber;
	merc_uint_t	MercIntLevel;
	merc_uint_t	MercIntVector;
} MDRV_MERC_HOST_IF_CONFIG, *PMDRV_MERC_HOST_IF_CONFIG;


/*

MERCURY_HOST_IF_CONFIG notes

ProtoclRevision specifies the Mercury host interface revision number.

CheckSum specifies the checksum used for debugging purposes.

HostAreaSize specifies the size in bytes of the shared ram which can 
be used by host interface.

MaxHostMessages specifies the size in entries of the host-to-firmware 
circular message buffer.

MaxFwMessage specifies the size in entries of the firmware-to-host 
circular message buffer.

DataAlignment specifies the desired alignment for the host interface.

MaxHostDataXfer specifies the size in entries of the host-to-firmware 
data circular buffer.

MaxFwDataXfer specifies the size in entries of the firmware-to-host 
data circular buffer.

HostConfigMask specifies revision dependent attributes.

HwTimeoutInterval specifies the time out value in milliseconds which 
the firmware will wait before sending messages and data to the host.   
The timeout allows the firmware to accumulate multiple messages before 
interrupting the host.

BoardNumber specifies the board number of the card.

NodeNumber specifies the Node number of the card.

MercIntLevel specifies the level on which the kernel will interrupt.

MercIntVector specifies the intr. vector used by the card.

Please refer to "Mercury Platform FW Host Interface Protocol"[4] 
for more details on the host interface configuration handshake.

*/


typedef struct _PAM_BOARD_START_ACK {
	merc_uint_t	BoardNumber;
	merc_uint_t	ErrorCode;
} PAM_BOARD_START_ACK, *PPAM_BOARD_START_ACK;

typedef struct _BRDID_TO_CFGID {
        merc_uint_t     BoardNumber;
        merc_uint_t     CfgId;
} BRDID_TO_CFGID, BRDID_TO_CFGID_ACK,  *PBRDID_TO_CFGID, *PBRDID_TO_CFGID_ACK;

/* 
Valid error codes 
	MD_OK
	PAM_ERR_BOARD_START_FAILED
*/ 

typedef struct _STREAM_OPEN {
        merc_uint_t     BoardNumber;
        merc_uint_t     StreamId;
        merc_uint_t     StreamFlags;
        merc_uint_t     RequestSize;
        merc_uint_t ContainerSize;
        merc_uint_t multiplier;
        merc_uint_t suppress_fw_cantakes;
} STREAM_OPEN, *PSTREAM_OPEN;

#define STREAM_OPEN_F_SEND_ONLY		0x00000001
#define STREAM_OPEN_F_RECEIVE_ONLY	0x00000002
#define STREAM_OPEN_F_FLUSH		0x00000004
#define STREAM_OPEN_F_PERSISTENT	0x00000008
#define STREAM_OPEN_F_GSTREAM		0x00000010


typedef struct _STREAM_OPEN_ACK {
	merc_uint_t	StreamHandle;
	merc_uint_t	StreamId;
	merc_uint_t	CanTakeByteCount;
	merc_uint_t	BlockSize;
	merc_uint_t	ErrorCode;
	merc_uint_t	TransactionId;
	merc_uint_t	ActualSize;
}STREAM_OPEN_ACK, *PSTREAM_OPEN_ACK;

typedef struct _STRAM_CLOSE {
	merc_uint_t	StreamId;
	merc_uint_t	Flags;
} STREAM_CLOSE, *PSTREAM_CLOSE;

#define STREAM_CLOSE_FLAG_FLUSH_ALL	0x00000001
#define STREAM_CLOSE_FLAG_NO_FLUSH	0x00000002


typedef struct _STRAM_CLOSE_ACK {
	merc_uint_t		StreamId;
	merc_uint_t		ErrorCode;
	merc_uint_t		TransactionId;
} STREAM_CLOSE_ACK, *PSTREAM_CLOSE_ACK;

typedef struct _STRAM_SESS_CLOSE_ACK {
	merc_uint_t    StreamId;
} STREAM_SESS_CLOSE_ACK, *PSTREAM_SESS_CLOSE_ACK;

typedef struct _USER_HEADER{
	merc_uchar_t	UserHeaderBytes[24];
} USER_HEADER, *PUSER_HEADER;

typedef struct _STREAM_SEND{
	merc_uint_t	Flags;
	merc_uint_t	StreamId;
	USER_HEADER	StreamUserHeader;
} STREAM_SEND, *PSTREAM_SEND;

typedef struct _STREAM_MBLK_SEND_ENTRY{
	merc_uint_t	Flags;
	merc_uint_t	ByteCount;
	USER_HEADER	StreamUserHeader;
} STREAM_MBLK_SEND_ENTRY, *PSTREAM_MBLK_SEND_ENTRY;

typedef struct _STREAM_MBLK_SEND{
	merc_uint_t		BlockCount;
	merc_uint_t		StreamId;
	STREAM_MBLK_SEND_ENTRY	Entry[1];
} STREAM_MBLK_SEND, *PSTREAM_MBLK_SEND;

typedef struct _STREAM_RECEIVE{
	merc_uint_t		Flags;
	merc_uint_t		StreamId;
	merc_uint_t		Sequence;
	USER_HEADER StreamUserHeader;
} STREAM_RECEIVE, *PSTREAM_RECEIVE;

#define STREAM_FLAG_EOS				0x80000000
#define MULTI_BLK_NO_CAN_TAKE		0x10000000
#define MULTI_BLK_SEND_CAN_TAKE	0x40000000
#define MULTI_BLK_CNT_MASK			0x000000FF
// Note : Lower byte of the Flag field indicates BlockCount in Multi
// block write, used to send proper can take back to user


typedef struct _STREAM_SEND_ACK  {
	merc_uint_t	CanTakeByteCount;
} STREAM_SEND_ACK, *PSTREAM_SEND_ACK;

typedef struct _STREAM_SET_PARAMETER {
	merc_uint_t	Count;
	merc_uint_t	StreamId;
} STREAM_SET_PARAMETER, *PSTREAM_SET_PARAMETER;


typedef struct _STREAM_SET_PARAMETER_ACK{
	merc_uint_t	Count;
	merc_uint_t	ErrorCode;
} STREAM_SET_PARAMETER_ACK, *PSTREAM_SET_PARAMETER_ACK;

/*
STREAM_KEY_SEND_Q_LIMIT is the driver stream send queue limit, 
applicable per stream instance.   If the driver receives data blocks 
and the aggregate number of blocks is greater than this number, 
driver discards the overflowing blocks.  The Value field specifies 
the block count ,
*/

#define STREAM_KEY_SEND_Q_LIMIT 	0x00000001

/*
STREAM_KEY_SEND_Q_LOW is the per instance driver stream send queue 
low water mark.  This value instructs the driver to send a 
MID_STREAM_SEND_ACK when the queue length is equal to or lower 
than this value.
*/
#define STREAM_KEY_SEND_Q_LOW 0x00000002

#define STREAM_KEY_CONTAINER_SIZE 0x00000004

typedef struct _STREAM_FLUSH {
	merc_uint_t		FlushFlags;
} STREAM_FLUSH, *PSTREAM_FLUSH;

typedef struct _STREAM_PARAMETER_ENTRY{
	merc_uint_t	Key;
	merc_uint_t	ErrorCode;
	union {
		merc_char_t	CharValue[MD_MAX_PARAMETER_CHAR_VALUE];
		merc_uint_t	UlongValue[MD_MAX_PARAMETER_ULONG_VALUE];
	} Value;
} STREAM_PARAMETER_ENTRY, *PSTREAM_PARAMETER_ENTRY;

typedef struct _STREAM_GET_PARAMETER{
	merc_uint_t		Count;
	merc_uint_t		Rfu;
} STREAM_GET_PARAMETER, *PSTREAM_GET_PARAMETER;

typedef struct _STREAM_GET_PARAMETER_ACK{
	merc_uint_t		Count;
	merc_uint_t		ErrorCode;
} STREAM_GET_PARAMETER_ACK, *PSTREAM_GET_PARAMETER_ACK;

#define MD_PANIC_DUMP_MAX_SIZE	0x100

typedef struct _PAM_GET_PANIC_DUMP{
	merc_ushort_t	BoardNumber;
	merc_uchar_t	PanicDump[MD_PANIC_DUMP_MAX_SIZE];
} PAM_GET_PANIC_DUMP, *PPAM_GET_PANIC_DUMP;

typedef struct _PAM_BOOT_HOST_RAM_BIT{
	merc_ushort_t	BoardNumber;
	merc_uchar_t	BootHostRamBit;	/* see below */
} PAM_BOOT_HOST_RAM_BIT, *PPAM_BOOT_HOST_RAM_BIT;

#define MD_SET_BOOT_FROM_FLASH			0x0	/* read/write */
#define MD_SET_BOOT_FROM_HOST_RAM		0x1	/* read/write */

/* for big msgs */
#define MD_MIN_BODY_SIZE	25
#define MD_MAX_BODY_SIZE	2048
#define MERC_INVALID_STREAMID 0xFFFFFF
#endif
