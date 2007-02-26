/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdstruct.h
 * Description                  : driver data structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDSTRUCT_
#define _MSDSTRUCT_
// Mercury Driver Control block 

#include "dtrace.h"

// Control Block Related definitions
#define MERCD_CTRL_BLOCK_PRE_INI 		0x0001
#define MERCD_CTRL_BLOCK_INIT			0x0002
#define MERCD_CTRL_BLOCK_READY			0x0004

//Global Lock for Printing Messages
//mercd_mutex_T merc_prtlock, *merc_printlock;

//THIRD_ROCK Support
enum FAMILY_TYPES  {SRAM_FAMILY = 0, THIRD_ROCK_FAMILY, INVALID_BOARD_FAMILY };

//PCI Config SubSytemIds for Third rock boards.
#define SUBSYSID_21554                  0x5001
#define SUBSYSID_ROZETTA_21554          0x5002
#define SUBSYSID_80321                  0x5003 /*YAVAPAI*/

//THIRD_ROCK Support
//This structure defines the subsystem Id and the
//family to which that board belongs
typedef struct  deviceTypeArray{
        ULONG   subsystemId;
        USHORT  boardFamilyType;
} deviceTypeArray_sT;


/////////////////SRAMDMA BEGIN////////////////////////////////////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
  //So far the testing was performed with descriptors on board
  //only. Our intention is to have the descriptors both on host
  //and board, but such a scenario is never tested. Hence please
  //experiment such a scenario at your risk. This will be supported
  //in the next release
#define MAX_MERC_RX_DMA_CHAINS_ON_BOARD               120
#define MAX_MERC_TX_DMA_CHAINS_ON_BOARD               0 

#define MAX_MERC_RX_DMA_CHAINS_ON_HOST                0
#define MAX_MERC_TX_DMA_CHAINS_ON_HOST                0

#define TOTAL_MERC_RX_DMA_CHAINS                       120 
#define TOTAL_MERC_TX_DMA_CHAINS                       0

typedef struct _mercd_dma_sgt_chain_node {
   merc_uint_t   hostAddr;
   merc_uint_t   sramAddr;
   merc_uint_t   xferSize;
   merc_uint_t   nextnode;

   //!!! what replaces _sparcv9???
#if defined( __sparcv9 )
   merc_uint_t   HiHostAddr; // For 64 bit support
#endif
} mercd_dma_sgt_chain_node, *pmercd_dma_sgt_chain_node;

typedef struct _dma_desc_sT {
   // DMA handle
   //mercd_dma_handle_t dma_hdlp;

   // Pointer to sgt - gather node
   pmercd_dma_sgt_chain_node  pdma_chain;

   // Pointer to streams message block buffer
   void                    *dma_mb;

   // Size of this chain node
   merc_ushort_t   size;

   // Loc = 1 : Desc on host
   // Loc = 0 : Desc on board
   merc_char_t             location;

   // XXXX XEPV
   // E: End of linked chains
   // P: Passing physical page boundary
   // V: Validity bit
   merc_char_t             flags;
} dma_desc_sT, *pdma_desc_sT;

//Descriptors Locations
#define MERC_DESCR_ON_HOST 1
#define MERC_DESCR_ON_BOARD 0

//For Streams mblk page boundary validation
#define MERC_PAGE_VALID 1
#define MERC_PAGE_PASS 2
#define MERC_PAGE_END  4

// Timer Information Structure
typedef struct _mercd_dma_info {
   //Tx side
   merc_uchar_t *  phys_tx_chain_base;
   merc_uchar_t *  phys_tx_chain_base_on_host;
   merc_uchar_t *  virt_tx_chain_base;
   merc_uchar_t *  virt_tx_chain_base_on_host;
   //Rx side
   merc_uchar_t *  phys_rx_chain_base;
   merc_uchar_t *  phys_rx_chain_base_on_host;
   merc_uchar_t *  virt_rx_chain_base;
   merc_uchar_t *  virt_rx_chain_base_on_host;

   merc_uchar_t    total_merc_tx_dma_chains;
   merc_uchar_t    total_merc_rx_dma_chains;

   merc_uchar_t    max_merc_tx_dma_chains_on_board;
   merc_uchar_t    max_merc_rx_dma_chains_on_board;

   mercd_mutex_T   snd_dma_mutex;
   mercd_mutex_T   rcv_dma_mutex;


#ifdef POST_DMA_FREE
   merc_ulong_t  dmaRdIndex;
   merc_ulong_t  dmaWrIndex;
   merc_ulong_t  dma_blk_count;
#endif

   //mercd_dma_handle_t  phys_tx_handle;
   //mercd_dma_handle_t  phys_rx_handle;

   merc_ulong_t  tx_dma_count;
   merc_ulong_t  rx_dma_count;

   merc_ulong_t  rx_dma_started;
   merc_ulong_t  tx_dma_started;

   merc_ulong_t  rx_mbuf_count;
   merc_uint_t   dma_base_offset;

   pdma_desc_sT  prxdma_res;
   pdma_desc_sT  ptxdma_res;
   void * rxmbufdma[120]; // To be Cleaned ...

   merc_uchar_t    descr_location;

} mercd_dma_info_sT, *pmercd_dma_info_sT;

// The following are the DMA flags.

#define  MERC_ADAPTER_FLAG_SND_DMA_INTR                 1
#define  MERC_ADAPTER_FLAG_RCV_DMA_INTR                 2
#define  MERC_ADAPTER_FLAG_DMA_INTR                     4

#define  MERC_ADAPTER_FLAG_SND_DMA_PENDING              1
#define  MERC_ADAPTER_FLAG_RCV_DMA_PENDING              2
#define  MERC_ADAPTER_FLAG_DMA_PENDING_MASK             3

#define MERC_DPC_OWN_SRAM                               0x1
#define MERC_DPC_REQ_SRAM                               0x2
#define MERC_TIMER_OWN_SRAM                             0x4
#define MERC_TIMER_REQ_SRAM                             0x8

#endif /*SRAM_DRVR_DMA_SUPPORT*/
/////////////////////////////SRAMDMA END////////////////////////////////////////////

// Timer Information Structure
typedef struct _mercd_timer_info {
        mercd_mutex_T       timer_mutex;
        mercd_mutex_T       timer_flag_mutex;
	merc_uchar_t        state;
	merc_uint_t         snd_timeout_interval; /* Was -> merc_ulong_t snd_timeout_interval;*/
#if defined LiS || defined LIS || defined LFS
#ifdef LFS
	toid_t        	    timeout_id; /* Was -> merc_ulong_t timeout_id; */
#else
	ssize_t        	    timeout_id; /* Was -> merc_ulong_t timeout_id; */
#endif
#else
	struct timer_list   timeout_id;
#endif /* LiS */
#ifdef LFS
        toid_t             timeout_id_sem;
#else
        ssize_t             timeout_id_sem;
#endif
	pmercd_function_T   timeout_handler;
} mercd_timer_info_sT, *pmercd_timer_info_sT;

#define  MERCD_TIMER_INFO  sizeof(mercd_timer_info_sT)

// Interrupt Information Structure : Part of Adapter Structure

typedef struct _mercd_intr_info {
	merc_uint_t               level;
	merc_uint_t               intr_vec;
	merc_uint_t               intr_number;
	pmercd_function_T         intr_handle;
	size_t                    isr_handle;
	mercd_iblock_cookie_T     intr_iblock_cookie;
	pmercd_function_T         dpc_intr_handle;
	mercd_iblock_cookie_T     dpc_iblock_cookie;
	size_t                    dpc_handle; /* Was -> merc_ulong_t        dpc_handle; */
	mercd_mutex_T             intr_mutex;
	mercd_mutex_T             soft_level_mutex;
        //////SRAMDMA BEGIN//////////////////
        mercd_mutex_T             sram_ownership_mutex;
        //////SRAMDMA END//////////////////
	mercd_iblock_cookie_T     soft_level_id;
        mercd_taskqueue_sT        dpc_task;
}mercd_intr_info_sT, *pmercd_intr_info_sT;

#define MERCD_INTR_INFO sizeof(mercd_intr_info_sT)

typedef struct merc_virt_map_sT{
	volatile merc_uint_t   regset;       /* Was -> merc_ulong_t         regset; */		
	volatile merc_uint_t   phys_addr;    /* Was -> merc_ulong_t         phys_addr; */
	volatile pmerc_uchar_t virt_addr;    /* Was -> merc_ulong_t         virt_addr; */
	volatile merc_uint_t   start_offset; /* Was -> merc_ulong_t         start_offset; */
	volatile merc_uint_t   end_offset;   /* Was -> merc_ulong_t         end_offset; */
	merc_uint_t            byte_count;   /* Was -> merc_ulong_t         byte_count; */
	mercd_dev_acc_handle_T acc_handle;
}mercd_virt_map_sT, *pmercd_virt_map_sT;

#define MERCD_VIRT_MAP  sizeof(mercd_virt_map_sT)

// Hardware Information Structure : Part of Adapter Structure

typedef struct _mercd_hw_info {
	merc_uchar_t         board_type;
	merc_uchar_t         bus_type;
	merc_uchar_t         slot_number;
	merc_uchar_t         bus_number;
	pmercd_virt_map_sT   *virt_map_q;
	pmercd_virt_map_sT   *virt_ww_map_q;
	mercd_kcondvar_t     un_cv;
	mercd_mutex_T        un_lock;
	merc_uchar_t         un_flag;
	pmercd_intr_info_sT  intr_info;
	pmercd_timer_info_sT timer_info;

        ////SRAMDMA BEGIN/////////
#ifdef SRAM_DRVR_DMA_SUPPORT
        pmercd_dma_info_sT   pdma_info;
#endif
        ////SRAMDMA END/////////

	md_status_t 	     ret; /* Added for Linux */
        //THIRD_ROCK Support
        merc_ushort_t          boardFamilyType;
        merc_uint_t            pciSubSysId;
        //PCI Slot Number for listboards
        merc_uchar_t           pci_slot_number;
	merc_uint_t            pacific_chip;
	merc_uint_t            clash_slot_id;
        merc_ushort_t          typePCIExpress;
        merc_ulong_t           powerGoodReg;
        merc_ulong_t           powerOverrideReg;
        merc_ulong_t           actualPowerProvided;
        merc_ulong_t           powerRequiredByBoard;
}mercd_hw_info_sT, *pmercd_hw_info_sT;

#define MERCD_HW_INFO sizeof(mercd_hw_info_sT)

// Hardware definitions
#define MERCD_BUS_VME                       0x01
#define MERCD_BUS_PCI                       0x02
#define MERCD_BUS_CPCI                      0x03

// need to change it to line below 
//typedef PMSD_QUEUE pmercd_queue_sT;

typedef MSD_QUEUE pmercd_queue_sT;

typedef PSTRM_MSG pmercd_strm_msg_T; 

typedef struct _merc_adpflag {
	merc_uchar_t	SendTimeoutPending;
	merc_uchar_t	LaunchIntr;
	merc_uchar_t	SramOurs;
	merc_uchar_t	HostifConfigSent;
	merc_uchar_t	SendDataPending;
	merc_uchar_t	RecvPending;
//#ifdef SRAM_DRVR_DMA_SUPPORT
        merc_uchar_t    DMAIntrReceived;
        merc_uchar_t    RcvDmaPending; /* concurrent DMA along with send */
        merc_uchar_t    SndDmaPending; /* concurrent DMA along with rcv */
        merc_uchar_t    DPCDmaPending; /* one at a time */
        merc_uchar_t    SramFlags;
//#endif
        //WW support
        merc_uint_t    WWFlags;
        merc_uchar_t   PostFlags;
        merc_uchar_t   TraceLevelInfo; /* Driver Trace related */
}merc_adpflag_sT,  *pmercd_adpflag_sT;	

typedef struct _mercd_can_take {
    merc_int_t     nelements;
    pmerc_void_t   Node[MSD_MAX_STREAM_ID];
} mercd_can_take_sT, pmercd_can_take_sT;

#define MSD_READER      1
#define MSD_SENDER      2
#define MSD_ARRAY_SIZE  1000
#define MSD_ARRAY_SIZE_WW 512
#define MSD_ARRAY_SIZE_SRAM 256

typedef struct _MSD_ARRAY {
#ifdef LFS
             Uint32 **array;
#else
             Uint32 *array;
#endif
             Uint32 head;
             Uint32 tail;
} MSD_ARRAY, *PMSD_ARRAY;

#define MSD_ARRAY_EMPTY(A) (A.head == A.tail)

typedef struct _MSD_HCS_ARRAY {
        merc_uint_t sizeOfHCS;
        merc_uint_t firstHCSId;
        merc_uint_t numOfHCSActive;
        merc_uint_t maxNumOfHCS;
	merc_uint_t nextAvailableStreamId;
	merc_uint_t *pStreamStateArray;
} MSD_HCS_ARRAY, *PMSD_HCS_ARRAY;

#define HCS_AVAILABLE 0xDEADC0FF

typedef struct _mercd_open_block {
        struct  _mercd_open_block *Next;
        mercd_mutex_T             open_block_mutex;
        merc_ushort_t             state;
        merc_ulong_t              clone_device_id;
        /* One open block can refer to only one string am i wrong ?? */
        pmercd_strm_queue_sT      up_stream_write_q;
        pmercd_queue_sT           bind_block_q;
#if !defined LiS && !defined LIS && !defined LFS
	msgb_t                    *SavedMsg;
	//MSD_QUEUE                 readersQueue;
        MSD_ARRAY                 readersArray;
        WAITQUEUE_T               readers;
	mercd_mutex_T             readers_queue_mutex;
	void*			  kernelBuf;	
#endif /* LiS */
}mercd_open_block_sT, *pmercd_open_block_sT;

/* Driver Trace */
typedef struct  _mercd_trace_info_sT {
  merc_uint_t             level;               // trace level
  merc_uint_t             match_criteria;      // criteria - not used
  merc_uint_t             Drv2BrdMessageCnt;   // memory is allocated for storing these many d2b msgs
  merc_uint_t             Brd2DrvMessageCnt;   // memory is allocated for storing these many b2d msgs
  merc_uint_t             Drv2BrdTraceIndex;   // index for keeping track of d2b tracing
  merc_uint_t             Brd2DrvTraceIndex;   // index for keeping track of b2d tracing
  merc_ulong_t            StartTime;           
  pmercd_trace_copy_sT    Drv2BrdTraceBuf;     // for storing d2b trace info
  pmercd_trace_copy_sT    Brd2DrvTraceBuf;     // for storing b2d trace info
  merc_uint_t             Drv2BrdTotal;        // total number of d2b messages traced
  merc_uint_t             Brd2DrvTotal;        // total number of b2d messages traced
  
  merc_uint_t   dumpThreshold;                 // threshold used to send messages upstream
  merc_uint_t   Drv2BrdPrintIndex;             // used for sending messages up - MakeTraceData
  merc_uint_t   Brd2DrvPrintIndex;             // used for sending messages up - MakeTraceData
  merc_int_t   bSelect;                        // tracing mode  - now SELECT only
  pmercd_open_block_sT openblk;                // to have the stream to send upwards
  
  pmerc_uint_t            FilterToBrd;         // for storing message types filters (to board)
  pmerc_uint_t            FilterToDrv;         // for storing message types filters (to driver)
  merc_int_t              b2dFilterPresent;    // set to 1 if there are any b2d msgtype filters
  merc_int_t              d2bFilterPresent;    // set to 1 if there are any d2b msgtype filters

  merc_int_t b2dStrIdPresent;                  // set to 1 if there are any b2d streamid filters
  merc_int_t d2bStrIdPresent;                  // set to 1 if there are any d2b streamid filters
  pmerc_uint_t             StrIdFilterToBrd;   // for storing stream id filters to the board
  pmerc_uint_t             StrIdFilterToDrv;   // for storing stream id filters  to the driver

  merc_int_t SrcDescPresent;                   // set to 1 if there are any SrcDec filters
  merc_int_t DstDescPresent;                   // set to 1 if there are any DstDec filters
  pmerc_uint_t    SrcDesc;                     // for storing source comp descriptors
  pmerc_uint_t    DstDesc;                     // for storing destination comp descriptors

  merc_uint_t CurrentDataSizeLimit;            // how many bytes of message contents to be traced
  merc_int_t MsgDataTrace;                     // set to MSG_DATA_TRACE, if msg contents are traced

}mercd_trace_info_sT, *pmercd_trace_info_sT;

#define  MERCD_TRACE_INFO_LIST sizeof(mercd_trace_info_sT)

#define MSD_MAX_DATA_TRACE_SIZELIMIT 48
#define MSG_DATA_NO_TRACE 0
#define MSG_DATA_TRACE 1

// Flags for the Push and Pop trace routines
#define MSD_BRD2DRV_TRACE       1
#define MSD_DRV2BRD_TRACE       2
#define MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED 1
#define MERC_ADAPTER_FLAG_TRACE_STRUCT_NOT_ALLOCATED 0
/* Driver Trace */

typedef struct _MSD_MUTEX_INFO{
       merc_uint_t wait_count;
       merc_uchar_t waitfile[25];
       merc_uint_t waitline;

       merc_uint_t acquired_count;
       merc_uchar_t enterfile[25];
       merc_uint_t enterline;

       merc_uchar_t exitfile[25];
       merc_uint_t exitline;
} MSD_MUTEX_INFO, *PMSD_MUTEX_INFO;

typedef struct _MSD_MUTEX_STATS_ {
        MSD_MUTEX_INFO up_soft_mutex_stats;
        MSD_MUTEX_INFO up_mutex_stats;
        MSD_MUTEX_INFO smp_mutex_stats;
        MSD_MUTEX_INFO config_map_table_mutex_stats;
        MSD_MUTEX_INFO open_list_mutex_stats;
        MSD_MUTEX_INFO soft_mutex_stats;
        MSD_MUTEX_INFO mutex_stats; /* MSD_ENTER_MUTEX or MSD_EXIT_MUTEX */
        MSD_MUTEX_INFO hw_mutex_stats;
        MSD_MUTEX_INFO control_block_mutex_stats;
}MSD_MUTEX_STATS, *PMSD_MUTEX_STATS;


// Adapter Block Structure 
typedef struct _mercd_adapter_block {
	mercd_mutex_T		adapter_block_mutex;
        mercd_dev_info_T        *pdevi;
	merc_ushort_t		state;
	pmerc_void_t 		pcfg_open_block;
	merc_ushort_t		adapternumber;
	merc_adpflag_sT		flags;
	pmercd_hw_info_sT	phw_info;
	merc_ulong_t        TransactionId; 
	pmercd_host_info_sT	phost_info; 
        //ww support
        pmercd_ww_dev_info_sT   pww_info; //WW Extention
	mercd_mutex_T		snd_msg_mutex;
	MSD_QUEUE		snd_msg_queue;
	MSD_ARRAY		snd_msg_Array;
#ifdef LFS
        merc_ulong_t             send_array_pending;
#else
        merc_uint_t             send_array_pending;
#endif
	mercd_mutex_T		snd_data_mutex;
	mercd_mutex_T		stream_connection_mutex;
	pmercd_queue_sT		stream_adapter_list;
	pmercd_strm_msg_T	snd_data_queue;
	merc_uchar_t		*panicDump;
        mercd_can_take_sT       stream_cantake_list;
	pmerc_void_t*		pstream_connection_list;
        //WW support
        pmercd_queue_sT     strm_pend_strmblock_list;
#ifdef DRVR_STATISTICS
        merc_ulong_t    streams_count;
	merc_ulong_t    fw_overrun_msg;
	merc_ulong_t    fw_overrun_data;
	merc_ulong_t	fw_no_sram_lock;
#endif
       pmercd_trace_info_sT      ptrace;
       merc_uchar_t              rtkMode; //16BIT Change
       merc_uint_t              sndflowcount1; 
       merc_uint_t              sndflowcount; 
       merc_uint_t              rcvflowcount;
       merc_uint_t              rcvflowcount1;

       // HCS Support
       merc_uint_t		isHCSSupported;
       MSD_HCS_ARRAY		HCS;
}mercd_adapter_block_sT, *pmercd_adapter_block_sT;
   
typedef pmercd_adapter_block_sT  PMSD_ADAPTER_BLOCK;

#define MERCD_ADAPTER_BLOCK sizeof(mercd_adapter_block_sT)
#define MERCD_ADAPTER_BLOCK_LIST MSD_MAX_BOARD_ID_COUNT*sizeof(pmercd_adapter_block_sT)

// Adapter Block Structure related definitions 

// Adapter States

#define MERCD_ADAPTER_STATE_INIT                    0x0001
#define MERCD_ADAPTER_STATE_READY                   0x0002
#define MERCD_ADAPTER_STATE_CONFIGED                0x1002
#define MERCD_ADAPTER_STATE_MAPPED                  0x2002
#define MERCD_ADAPTER_STATE_STARTED                 0x0004
#define MERCD_ADAPTER_STATE_CP_STARTED              0x1004
#define MERCD_ADAPTER_STATE_DOWNLOADED              0x0008
#define MERCD_ADAPTER_STATE_STOPPED                 0x0010
#define MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS       0x0020
#define MERCD_ADAPTER_STATE_OUT_OF_SERVICE          0x0040
#define MERCD_ADAPTER_STATE_CRASHED                 0x0080
#define MERCD_ADAPTER_STATE_OUT_OF_SYNC             0x0100
#define MERCD_ADAPTER_STATE_SEND_DATA_PEND          0x0200
#define MERCD_ADAPTER_STATE_HRAM_SEM_TIMEOUT_PEND   0x0400
#define MERCD_ADAPTER_STATE_SUSPENDED               0x0800
#define MERCD_ADAPTER_STATE_PROBED                  0x1000

// Adapter Flags
#define MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND     1	
#define MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND 2


#define MERC_ADAPTER_FLAG_CONFIGED              1	
#define MERC_ADAPTER_FLAG_HOSTIF_CONFIG_SENT    2
#define MERC_ADAPTER_FLAG_RECEIVE_PEND			3
#define MERC_ADAPTER_FLAG_SEND_DATA_PEND		4
#define MERC_ADAPTER_FLAG_LAUNCH_INTR			5
#define MERC_ADAPTER_FLAG_SRAM_IS_OURS			6
#define MERC_ADAPTER_FLAG_ON_DPC_Q              7
#define MERC_ADAPTER_FLAG_WAIT_INTR             8
#define MERC_ADAPTER_FLAG_DPC_INSERTED          9
#define MERC_ADAPTER_FLAG_DPC_DMA_PENDING       0xA

#define UNIT_BUSY  1

// Virtual Map Information Structure : Part of Adapter Structure

#define MERCD_OPEN_BLOCK sizeof(mercd_open_block_sT )
#define MERCD_OPEN_BLOCK_LIST   MSD_MAX_OPEN_ALLOWED * sizeof(pmercd_open_block_sT)

#define MERCD_INITIAL_OPEN_BLOCK_LIST   MSD_INITIAL_OPEN_ALLOWED * sizeof(pmercd_open_block_sT)

typedef  pmercd_open_block_sT  PMSD_OPEN_BLOCK;

#define MERCD_OPEN_STATE_NOT_OPENED		0x0001
#define MERCD_OPEN_STATE_OPENED			0x0002
#define MERCD_OPEN_STATE_CLOSE_PEND		0x0003
#define MERCD_OPEN_STATE_CLOSE_PROGRESS		0x0004
#define MERCD_OPEN_STATE_CLOSED			0x0005
#define MERCD_OPEN_STATE_FREE			0x0006

// Redundant Host Info
#define MERCD_PROBED 1
#define MERCD_RESUMED 2
#define MERCD_SUSPENDED 3

typedef struct {
    unsigned short 		state;
    pmercd_adapter_block_sT     padapter;
} mercd_hs_t;

// Bind Block Structure 
typedef struct _mercd_bind_block {
	struct _mercd_bind_block 	*Next;
	mercd_mutex_T			bind_block_mutex;
#ifdef _8_BIT_INSTANCE
	merc_ushort_t			bindhandle;
#else
        merc_uint_t                     bindhandle;
#endif
	merc_ushort_t			state;
	merc_ushort_t			flags;
	mercd_kcondvar_t		bind_block_termination_cv;
	merc_ushort_t			ExitNotifyBindBoard;
	merc_ulong_t			UserContext;
	pmercd_open_block_sT    popen_block;
	merc_ulong_t			stream_connections;
	pmerc_void_t			stream_connection_ptr;

        //WW Streaming Change
        merc_uchar_t                   WWCancelStrmMFPending;
        merc_uchar_t                   WWCancelStrmAckPending;
        merc_uchar_t                   WWCancelBigMsgMFPending;
        merc_uchar_t                   WWCancelBigMsgAckPending;

        //What all adapters this BindBlock is used to send
        //messages to. WWReadAckForToBoardBigMsgs are the
	//the number of BigMsgs to Board requests waiting
	//for an ack from board. These two fields are required
	//for sending the Cancel BigMsgs MF to board.
	pmerc_uchar_t                  pWWMsgsToAdapters;
	merc_uint_t                    szWWMsgsToAdapters;
	merc_uint_t                    WWPendReadAckForToBoardBigMsgs;
       
        //Is unbind happening from a ^C
        merc_uint_t                   NormalUnBind;

        //Flush MF, ACK Pending
        merc_uchar_t                   WWFlushMFPending;
        merc_uchar_t                   WWFlushAckPending;
}mercd_bind_block_sT, *pmercd_bind_block_sT;

#define MERCD_BIND_BLOCK sizeof(mercd_bind_block_sT)
#define MERCD_BIND_BLOCK_LIST MSD_ABSOLUTE_MAX_BIND*sizeof(pmercd_bind_block_sT)
#define MERCD_INITIAL_BIND_BLOCK_LIST MSD_INITIAL_MAX_BIND*sizeof(pmercd_bind_block_sT)

// Core driver bind map table entry
typedef struct _MSD_BIND_MAP_ENTRY {
        pmercd_bind_block_sT         BindBlock;
} MSD_BIND_MAP_ENTRY, *PMSD_BIND_MAP_ENTRY;

typedef pmercd_bind_block_sT PMSD_BIND_BLOCK;

#define MERCD_BIND_STATE_OPENED			0x0001
#define MERCD_BIND_STATE_RELEASE_PEND		0x0002
#define MERCD_BIND_STATE_RELEASED		0x0003
#define MERCD_BIND_STATE_FREE			0x0004	

// Flags for Death Notification routine
#define MSD_ONLY_ONE_BIND_DEATH 	1
#define MSD_ALL_BINDS_DEATH         2

// Exit Notification flag
#define MSD_EXIT_NOTIFY_BIND_BOUND  1

#define MSD_BIND_FLAG_STREAM_OPEN_PEND  0x00000001
#define MSD_BIND_FLAG_EXIT_WOMUTEX      0x00000002

// STREAM Structures 


// Definitions
#define	MERCD_STREAM_STATE_NOT_OPENED		0x0001
#define MERCD_STREAM_STATE_OPEN_PEND		0x0002
#define MERCD_STREAM_STATE_OPEN			0x0003
#define	MERCD_STREAM_STATE_CONNECTED		0x0004
#define MERCD_STREAM_STATE_BROKEN		0x0005
#define MERCD_STREAM_STATE_CLOSE_PEND		0x0006
#define MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND	0x0007
#define MERCD_STREAM_STATE_CLOSED		0x0008
#define MERCD_STREAM_STATE_DISCONNECTED		0x0010
#define MERCD_STREAM_STATE_FREED		0x0011
#define MERCD_STREAM_STATE_TERMINATE_PEND	0x0012

#define MERCD_STREAM_FLAG_WRITE_ONLY		0x0001
#define	MERCD_STREAM_FLAG_READ_ONLY		0x0002
#define MERCD_STREAM_FLAG_SEND_FLOW_OFF		0x0003
#define MERCD_STREAM_FLAG_RECEIVE_FLOW_OFF	0x0004
#define MERCD_STREAM_FLAG_GSTREAM		0x0005


#define MSD_STREAM_SEND_LIMIT		4*4032
#define MSD_STREAM_SEND_LOW			4032

#define ALLOW_COUNT				5 /* Might be removed */

#define DUMMY_DATA_BLOCK_SIZE	16

// Stream Queue Parameters : Part of Streams structure
typedef struct _mercd_stream_queue_parm {
	merc_ulong_t	snd_queue_limit;
	merc_ulong_t	snd_queue_low;
	merc_ulong_t	cur_snd_cnt;
}mercd_stream_queue_parm_sT, *pmercd_stream_queue_parm_sT;


typedef struct _mercd_stream_stats {
	merc_ulong_t	snd_count;
	merc_ulong_t	rcv_count;
        merc_uint_t     ww_rcv_dblk_sequence_num;
}mercd_stream_stats_sT, *pmercd_stream_stats_sT;

typedef struct _MERC_DEFERRED_Q_BLOCK {
	size_t    DpcHandle;
	MSD_QUEUE DpcServiceQ;
} MERC_DEFERRED_Q_BLOCK, *PMERC_DEFERRED_Q_BLOCK_;

// STREAM Structures 

// Stream Connection Structure 

typedef struct _mercd_stream_connection {
	struct _mercd_stream_connection *Next;
	pmercd_bind_block_sT	pbind_block;
	pmercd_adapter_block_sT padapter_block;
	merc_ushort_t	id;
	MSD_HANDLE      handle;
	merc_ushort_t	state;
	merc_ushort_t	flags;
	merc_ushort_t	type;
	merc_ushort_t	cancelFlags;
	/* next three is for chxfer changes */
	merc_uint_t             ContainerSize;
        merc_uint_t     multiplication_factor;
        merc_uint_t     firstthreecnt;
	 merc_uint_t    suppress_fw_cantakes;
	merc_uint_t	can_take;	/* Was -> merc_ulong_t          can_take; */
	size_t		TransactionId;	/* Was -> merc_ulong_t          TransactionId; */
	merc_uint_t	accumulated_usr_ack; /* Was -> merc_ulong_t     accumulated_usr_ack; */
	mercd_stream_queue_parm_sT	qparam;
	pmercd_strm_msg_T		SendStreamQueue;	

        //WW support
        mercd_mutex_T            stream_block_global_mutex;

        //WW Max MFs allowed per Stream
        merc_uint_t                     maxWWMfs;
        merc_uint_t                     activeWWMfs;

        //WWSend Stream DMA descriptor table
        mercd_mutex_T            mercd_ww_snd_strm_block_mutex;
        pmercd_ww_dma_descr_sT   *pSendStrmDmaDesc;
        pmerc_uchar_t             pSendBitMapValue;
        merc_ushort_t                 szSendBitMap;

        //WWRcv Stream DMA descriptor table
        mercd_mutex_T            mercd_ww_rcv_strm_block_mutex;
        pmercd_ww_dma_descr_sT   *pRcvStrmDmaDesc;
        pmerc_uchar_t             pRcvBitMapValue;
        merc_ushort_t                 szRcvBitMap;
       
	// Stats are to be declared in the future
	mercd_stream_stats_sT	stats;

        //Pending Cancel St MF flag
        merc_uchar_t SendCancelStreamMf;

        //If this StreamBlock for WW mode of operation
        merc_uchar_t WWMode;
        merc_uint_t WWRcvBufSize;
        merc_uint_t WWRcvNumBufs;

        //If the Receive Stream Data Bufs are allocd already 
        //By default, we do that during set_param time after
        //strm_open_ack
        merc_uint_t WWRcvStrDataBufAllocd;

	//If this StreamBlock needs more host specific buffers
	//for receive ring
        merc_uint_t WWRcvStrDataBufPostPending;

	//Nature of Databuf posting to the receive ring
	//Immediate or deferred
        merc_uint_t WWRcvStrDataBufPostMethod;

        //Do we want to post the receive databuffers 
        //during Stream Open for Receive Stream ?
        //This is to characterize the profile for 
        //posting the buffers well in advance vs 
        //posting them on demand.
        merc_uint_t WWRcvStrDataBufPostInAdvance;

        //Pending FLUSH + READ CLOSE Msg for this stream
        PSTRM_MSG    pWWFlushCloseMsg;

        //PreAllocd EOS Msg for Read Streams
        PSTRM_MSG pWWReadEosMsg;

        merc_uint_t writeDMAPending;

	// DefaultMode or Speedy Mode
        merc_uchar_t     cantakeMode;

	// HCS Support
        merc_uint_t	isStreamHCS;
}mercd_stream_connection_sT, *pmercd_stream_connection_sT;

#define MERCD_STREAMS_BLOCK_LIST MSD_MAX_STREAM_ID*sizeof(pmercd_stream_connection_sT)

typedef struct _STRAM_CLOSE_INTERNAL {
	pmercd_stream_connection_sT  StreamBlock;
} STREAM_CLOSE_INTERNAL, *PSTREAM_CLOSE_INTERNAL;


typedef struct _MSD_STREAM_MAP_ENTRY {
	pmercd_stream_queue_parm_sT          StreamBlock;
} MSD_STREAM_MAP_ENTRY, *PCD_STREAM_MAP_ENTRY;

// Mercury Driver Control block 

typedef MSD_STATISTICS_BLOCK  mercd_statistics_sT;

#define MSD_MAX_OPENTOBE_FREED_INDEX  50
#define MSD_MAX_BINDTOBE_FREED_INDEX  50 

typedef struct _mercd_control_block {
	mercd_mutex_T			  mercd_ctrl_block_mutex;
	merc_ushort_t			  MsdState;
	merc_ushort_t			  maxbind;
	merc_ushort_t			  maxopen;
	merc_ushort_t			  maxstreams;
	merc_ushort_t			  open_streams;
	merc_ushort_t			  maxadapters;
	merc_ushort_t			  adapter_count;
	pmercd_adapter_map_T	  mercd_adapter_map;
	/* These are going to be removed later */
	/* An array of adapter block pointers indexed by the adapter number */
	pmercd_adapter_block_sT*  padapter_block_list; 
	merc_uint_t               open_block_count; /* Was -> merc_ulong_t open_block_count; */
	pmercd_open_block_sT*     popen_block_queue;
	pmercd_open_block_sT*     popen_free_block_queue;
	merc_uint_t               bind_count;       /* Was -> merc_ulong_t bind_count; */
	/* An array of bind block pointers indexed by the bind handle */

	int BindFreeIndex;
        int OpenFreeIndex;
	int sramDumpCfgId;
	int sramDumpSz;
 	int arraySz;

	pmercd_bind_block_sT*     pbind_block_list; 
	pmercd_bind_block_sT*     pbind_free_block_list;
	mercd_statistics_sT       MsdStatisticsBlock;
}mercd_control_block_sT, *pmercd_control_block_sT;

#define MERCD_CONTROL_BLOCK_STRUCT sizeof(mercd_control_block_sT)

#endif	// _MSDSTRUCT_ 
