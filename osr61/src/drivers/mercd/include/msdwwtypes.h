/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwtypes.h
 * Description                  : WW data type structures
 *
 *
 **********************************************************************/

#ifndef _MSDWWTYPES_H
#define _MSDWWTYPES_H


//WW support
typedef struct _merc_ww_param{
  merc_ushort_t numberMFAInboundQ;// number of MFA's in the Inbound Q
  merc_ushort_t sizeofMFAInboundQ; // size of MFA in the Inbound Q
  merc_ushort_t numberMFAOutboundQ; // number of MFA's in the Outbound Q
  merc_ushort_t sizeofMFAOutboundQ;  // size of MFA for the outbound Q
  merc_ushort_t numberBigMsgBlocks;  // number of Big Msg blocks to be sent
                                // to the board
  merc_ushort_t sizeofBigMsgBlocks; // size of the big message blocks
  merc_ushort_t numberOfOrphanEnteries;  // number of Orphan Q enteries....

 // The following fields are of no use for the driver.
  merc_ushort_t flowControlToBoard; // Flow Control to the board
  merc_ushort_t flowControlFromBoard; // Flow COntrol from the board
  merc_ushort_t hardwareTimeout;  // Timeout interval for the hardware
  merc_ushort_t maxNumStreamMFAAllowed; // The max number of mfa's
                                              // allowed for the streams
                                              //on this board.
}merc_ww_param_sT,  *pmercd_ww_param_sT;

#define MERCD_WW_PARAM sizeof(merc_ww_param_sT)

typedef struct merc_perf_counters{
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

}merc_perf_counters_sT, *pmerc_perf_counters_sT;

#define MERCD_WW_PERF_COUNTERS sizeof(merc_perf_counters_sT)

typedef struct _merc_ww_mf_memory_struct {
  pmerc_void_t board_address;
  pmerc_void_t host_address;
  merc_ulong_t size;
  //REPLACE WTTH LINUX SPECIFIC
  //mercd_dma_handle_T dma_hdlp;
  //mercd_dev_acc_handle_T dev_hdlp;
  pmerc_void_t start_address;

  // order for pages allocated
  merc_int_t   order;
}merc_ww_mf_memory_sT, *pmerc_ww_mf_memory_sT;

typedef struct _mercd_ww_dma_descr_sT {
  struct _mercd_ww_dma_descr_sT *next;
  pmerc_void_t  board_address;
  pmerc_void_t  host_address;
  //REPLACE WTTH LINUX SPECIFIC
  //mercd_dma_handle_T dma_hdlp;
  //mercd_dev_acc_handle_T dev_hdlp;
  //0x xxxp ecsv
  merc_uchar_t flag;
  merc_ushort_t size;
}mercd_ww_dma_descr_sT, *pmercd_ww_dma_descr_sT;

#define MERCD_WW_BIGMSG_DESCR sizeof(mercd_ww_dma_descr_sT)

typedef struct _merc_ww_rcvbigmsg_memory_struct {
  mercd_mutex_T rcvbigmsg_mutex;
  merc_ushort_t avail_counter;
  merc_ushort_t free_counter;
  pmercd_ww_dma_descr_sT *pbigMsgDescr;
  pmerc_uchar_t pBitMapValue;
  merc_ushort_t  szBitMap;
}merc_ww_rcvbigmsg_memory_sT, *pmerc_ww_rcvbigmsg_memory_sT;

typedef struct _merc_ww_sndbigmsg_memory_struct {
  struct _merc_ww_sndbigmsg_memory_struct *next;
  mercd_mutex_T sndbigmsg_mutex;
  merc_ulong_t avail_counter;
  merc_ushort_t free_counter;
  pmercd_ww_dma_descr_sT *pbigMsgDescr;
  pmerc_uchar_t  pBitMapValue;
  merc_ushort_t  szBitMap;
}merc_ww_sndbigmsg_memory_sT, *pmerc_ww_sndbigmsg_memory_sT;

typedef struct _merc_ww_dev_info{
  merc_uint_t state;

  //Structure for storing MFA Base Address
  merc_ww_mf_memory_sT mfaBaseAddressDescr;

  //Structure for MFA InBound Q
  pmerc_uchar_t inBoundQMfAddress;

  //Structure for MFA OutBound Q
  pmerc_uchar_t outBoundQMfAddress;

  //Structure for Receive BIG Msg Block Base Address
  merc_ww_rcvbigmsg_memory_sT BigMsgRcvMemStr;  /* From Bd Mem Structure */

  //Structure for Sending BIG Msg Blocks
  merc_ww_sndbigmsg_memory_sT BigMsgSndMemStr;  /* To Bd Mem Structure */

  //For new BIGMSG Rcv protocol: RTK gives
  //multiple MFs for a Big Msg, where hdr+data
  //is directly available in MFs
  PSTRM_MSG BigMsgRcvdInMFs;
 
  //PLX inBound Q Reg Address
  pmerc_void_t inBoundQRegAddress;

  //PLX outBound Q Reg Address
  pmerc_void_t outBoundQRegAddress;

  //WW Param Structure
  pmercd_ww_param_sT      pww_param;
  pmerc_void_t  phw_info;

  //Back Reference  to Adapter Structure pmercd_adapter_block_sT

  pmerc_void_t padapter;
 
  //Mutex for WW struct
  mercd_mutex_T  ww_struct_mutex;
 
  //For DPC use
  merc_uint_t intr_reason;

  //Performance Counters
  pmerc_perf_counters_sT pww_counters;

  //Mutex for WW performance counters
  mercd_mutex_T ww_counters_mutex;

  //PLX mapping in WW mode
  //NO USE OF THIS IN DTI16
  pmerc_void_t pww_plx_space_tree;

  //Message Pending Q priority Flag
  merc_ushort_t WWMsgPendingQFlag;

  //Mutex for Message Pending Q
  mercd_mutex_T  ww_msgpendq_mutex;
  mercd_mutex_T  ww_iboundQ_mutex;
  mercd_mutex_T  ww_oboundQ_mutex;
  mercd_mutex_T  ww_eospendq_mutex;
 
  //WW Stream: EOS Pending Q
  PSTRM_MSG pww_eos_msgq;

  //BitMap that gives us the pending messages
  //per BindBlock
  pmerc_uchar_t *pPendMsgBitMap;
  merc_uint_t   szPendMsgBitMap;  /* == Max BigMsgs */

  //A global bit for all BindBlocks with cancel Msg MF pending
  pmerc_uchar_t  pPendBindCancelMsgMFBitMap;
  merc_uint_t    szPendBindCancelMsgMFBitMap;

  //A global bit for all BindBlocks with cancel Strm MF pending
  pmerc_uchar_t  pPendBindCancelStrMFBitMap;
  merc_uint_t    szPendBindCancelStrMFBitMap;

  //There is a one time Cancel BigMsg MF.
  //To tell the board not to touch the host
  //mem.
  merc_uchar_t WWCancelBigMsgAckPending;

  //To check for streams for which we never
  //received the Stream CLOSE ACKs
  merc_uchar_t WWCloseAckToutCheck;

  //A bit map, that tells what streams still need to
  //receive a flush mf.
  pmerc_uchar_t pPendFlushStrBitMap;
  merc_uint_t   szPendFlushStrBitMap;

  //A bit map, that tells the timer what StreamBlocks
  //need Rcv Stream DATA Bufs. Only for StreamBlocks
  //of type == RCV_ONLY
  pmerc_uchar_t pPendReqRcvStrDataBufBitMap;
  merc_uint_t   szPendReqRcvStrDataBufBitMap;

  merc_uint_t	dpc_loop_cnt;

  //Private data; Driver uses this store to
  //some context specific processing for storing
  //a OS mblock during setmode etc, etc.
  pmerc_void_t  private_data1;
  pmerc_void_t  private_data2;

  merc_uint_t   timeWaitForCancelAck;
}merc_ww_dev_info_sT, *pmercd_ww_dev_info_sT;

#define MERCD_WW_DEV_INFO sizeof(merc_ww_dev_info_sT)


//Miscellaneous Utility Structures For....
typedef struct _mercd_ww_gen_doorbell {
        merc_ww_dev_info_sT *pwwDevi;
        merc_ulong_t value;
}mercd_ww_gen_doorbell_sT, *pmercd_ww_gen_doorbell_sT;

typedef struct _mercd_ww_get_mfAddress {
        merc_ww_dev_info_sT *pwwDevi;
        pmerc_uchar_t pmfAddress;
        merc_ulong_t  mfIndex;
}mercd_ww_get_mfAddress_sT, *pmercd_ww_get_mfAddress_sT;

typedef struct _mercd_ww_get_mfClass{
        merc_void_t  *mb;
        merc_ulong_t msgSize;
        merc_uint_t msgClass;
}mercd_ww_get_mfClass_sT, *pmercd_ww_get_mfClass_sT;

typedef struct pmercd_ww_build_initMF{
        merc_ww_dev_info_sT *pwwDevi;
        pmerc_uchar_t pmfAddress;
        merc_ulong_t  outBoundQPhysAddr;
}mercd_ww_build_initMF_sT, *pmercd_ww_build_initMF_sT;

typedef struct pmercd_ww_build_init_msgblkMF{
        merc_ww_dev_info_sT             *pwwDevi;
        pmerc_ww_rcvbigmsg_memory_sT  pbigmsgmem;
        merc_uint_t               inBoundMfIndex;
        merc_uint_t                 mfToBePosted;
        merc_uint_t                 maxNumOfDescr;
        merc_uint_t                    numBigMsgs;
        merc_uint_t                       purpose;
}mercd_ww_build_init_msgblkMF_sT, *pmercd_ww_build_init_msgblkMF_sT;

typedef struct pmercd_ww_build_smallMF{
        merc_void_t  *mb;
        pmerc_uchar_t pmfAddress;
}mercd_ww_build_smallMF_sT, *pmercd_ww_build_smallMF_sT;

typedef struct pmercd_ww_build_bigMF{
        merc_ww_dev_info_sT *pwwDevi;
        merc_void_t  *mb;
        pmerc_uchar_t pmfAddress;
}mercd_ww_build_bigMF_sT, *pmercd_ww_build_bigMF_sT;

typedef struct pmercd_ww_dmaDscr_Index{
        merc_uchar_t *pBitMap;
        merc_ushort_t szBitMap;
        merc_uint_t   index;
}mercd_ww_dmaDscr_Index_sT, *pmercd_ww_dmaDscr_Index_sT;

typedef struct pmercd_ww_mblk_train_info{
        merc_void_t  *mb;
        merc_ulong_t dataSize;
        merc_ulong_t mblkCount;
}mercd_ww_mblk_train_info_sT, *pmercd_ww_mblk_train_info_sT;
typedef struct pmercd_ww_copy_mblk_train_data{
        merc_void_t  *mb;
        merc_void_t  *virtaddr;
}mercd_ww_copy_mblk_train_data_sT, *pmercd_ww_copy_mblk_train_data_sT;

typedef struct pmercd_ww_prep_mblk_train{
        merc_ww_dev_info_sT *pwwDevi;
        merc_void_t      *dataMblks;
        pmerc_uchar_t    pusrContext;
        pmerc_uint_t   numContexts;
        pmerc_void_t   pmfHead;
        merc_uint_t    streamType;
}mercd_ww_prep_mblk_train_sT, *pmercd_ww_prep_mblk_train_sT;

typedef struct pmercd_ww_prep_mf_train{
        merc_ww_dev_info_sT *pwwDevi;
        pmerc_uchar_t    pmfAddress;
        merc_void_t      *dataMblks;
}mercd_ww_prep_mf_train_sT, *pmercd_ww_prep_mf_train_sT;

typedef struct pmercd_ww_dispatch_rcvd_msg{
        pmerc_void_t     padapter;
        pmerc_void_t     mb;
        pmerc_void_t     fwmsg;
} mercd_ww_dispatch_rcvd_msg_sT, *pmercd_ww_dispatch_rcvd_msg_sT;

typedef struct pmercd_ww_cancel_streamMF{
        pmerc_void_t  StreamBlock;
        pmerc_void_t  padapter;
}mercd_ww_cancel_streamMF, *pmercd_ww_cancel_streamMF;

typedef struct pmercd_ww_flush_streamMF{
        merc_uint_t streamid;
        pmerc_void_t padapter;
}mercd_ww_flush_streamMF, *pmercd_ww_flush_streamMF;

typedef struct pmercd_ww_send_streamdata{
        pmerc_void_t  Msg;
        pmerc_void_t  padapter;
        pmerc_void_t  StreamBlock;
        pmerc_void_t  pmfAddress;
        merc_ulong_t  dataCount;
        merc_ushort_t dmaDescIndex;
}mercd_ww_send_streamData, *pmercd_ww_send_streamData;


//Initialization MF structure
typedef struct _merc_ww_init_mf_struct {
 merc_ulong_t msgType;
 merc_ulong_t msgSize;
 merc_ulong_t outBoundMFSize;
 merc_ulong_t inBoundMFSize;
 merc_ulong_t flowControlToBoard;
 merc_ulong_t flowControlFromBoard;
 merc_ulong_t sizeofBigMsgBlocks;
 merc_ulong_t hardwareTimeout;
 merc_ulong_t maxNumStreamMFAAllowed;
 merc_ulong_t maxNumBigMsgBlocks;
 pmerc_uchar_t outBoundQPhysicalAddress;
}merc_ww_init_mf_sT, *pmercd_ww_init_mf_sT;


//EOS specific structure
typedef struct mercd_ww_build_eos_struct{
  merc_uint_t id;
  pmerc_uchar_t pmfAddress;
} mercd_ww_build_eos_sT, *pmercd_ww_build_eos_sT;




//Some Globals for DEBUG purpose
extern merc_ulong_t vSndFreeDesc;
extern merc_ulong_t vRcvFreeDesc;

#endif //_MSDWWDEFS_H_
