/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwmsgutl.c
 * Description                  : Messaging and Utilities
 *
 *
 ***********************************************************************/

#include "msd.h"
#define _MSDMSGMGR_C_
#include "msdextern.h"
#undef _MSDMSGMGR_C_



/***************************************************************************
 * Function Name                : msgutl_ww_determine_message_class
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Determines the Size of the Message
 * Additional comments          : This caters to sending side
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_determine_message_class(pmercd_ww_get_mfClass_sT pClassSt)
{
  MD_STATUS     Status;
  PSTRM_MSG     Msg;
#ifndef LFS
  PSTRM_MSG     NMsg;
#endif
  merc_ulong_t  payloadsz;
  merc_ulong_t  headersz;
  merc_ulong_t  totalsz;


 MSD_FUNCTION_TRACE(
           "msgutl_ww_determine_message_class 0x%x %s #%d\n", 
            (size_t)pClassSt, 
            __FILE__, 
            __LINE__
         );

  Status = MD_SUCCESS;
  
  if ( !(pClassSt) ) {
      printk("msgutl_ww_determine_message_class: Null pClassSt\n");
      return MD_FAILURE;
  }

  Msg = (PSTRM_MSG)(pClassSt->mb);

  if ( !(Msg) || !(Msg->b_cont) ) {
      printk("msgutl_ww_determine_message_class: Null Msg or b_cont\n");
      return MD_FAILURE;
  }
	
  payloadsz = MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr));
  headersz = sizeof(MERC_HOSTIF_MSG);
  // The following is as good as
  // same as :- totalsz == ((Msg)->b_cont->b_wptr - (Msg)->b_cont->b_rptr)

  totalsz = headersz + payloadsz;

  if ((totalsz) <= (pClassSt->msgSize - MERCD_WW_MF_HEADER_SIZE))
   {
    pClassSt->msgClass = MERCD_WW_SMALL_MESSAGE;
   }
  else
   {
    pClassSt->msgClass = MERCD_WW_BIG_MESSAGE;
   }

 return(Status);

}

/***************************************************************************
 * Function Name                : msgutl_ww_build_init_mf
 * Function Type                : Support Function for WW Msg Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds the init MF
 * Additional comments          : This caters to WW init time
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_build_init_mf(pmercd_ww_build_initMF_sT pinitMfSt)
{

 MD_STATUS Status;
#ifndef LFS
 merc_ulong_t physOffset;
#endif
 merc_ww_init_mf_sT init_mf;
 pmercd_ww_dev_info_sT pwwDev;

 MSD_FUNCTION_TRACE("msgutl_ww_build_init_mf 0x%x %s #%d\n", 
                     (size_t) pinitMfSt, __FILE__, __LINE__);
 Status = MD_SUCCESS;
 pwwDev =  pinitMfSt->pwwDevi;

 //Initialize init_mf
 init_mf.msgType = MERCD_WW_CONFIGURATION_MSG_TO_BOARD | sizeof(merc_ww_init_mf_sT);
 init_mf.msgSize = sizeof(merc_ww_init_mf_sT) - sizeof(merc_ulong_t);
 init_mf.outBoundMFSize = pwwDev->pww_param->sizeofMFAOutboundQ;
 init_mf.inBoundMFSize = pwwDev->pww_param->sizeofMFAInboundQ;
 init_mf.flowControlToBoard = pwwDev->pww_param->flowControlToBoard;
 init_mf.flowControlFromBoard = pwwDev->pww_param->flowControlFromBoard;
 init_mf.sizeofBigMsgBlocks = pwwDev->pww_param->sizeofBigMsgBlocks;
 init_mf.hardwareTimeout = pwwDev->pww_param->hardwareTimeout;
 init_mf.maxNumStreamMFAAllowed = pwwDev->pww_param->maxNumStreamMFAAllowed;
 init_mf.maxNumBigMsgBlocks = pwwDev->pww_param->numberBigMsgBlocks;

 // We need to send the outBoundQPhysicalAddress
#ifdef LFS
 init_mf.outBoundQPhysicalAddress = (pmerc_uchar_t)pinitMfSt->outBoundQPhysAddr; /* inBoundQ Phys Addr + Offset */
#else
 init_mf.outBoundQPhysicalAddress = pinitMfSt->outBoundQPhysAddr; /* inBoundQ Phys Addr + Offset */
#endif

 //3. Copy the init_mf to MF Mem Pool for inBoundQ
 MsdCopyMemory(&init_mf, pinitMfSt->pmfAddress, sizeof(merc_ww_init_mf_sT));

 return (Status);
}


/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_msgblk_mf
 * Function Type                : Support Function for WW Msg Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds the msgblk MF to FW
 *                                with the available RCV BIG MSG block
 *                                descriptors for the receive DMA operations
 *
 * Additional comments          : This is for advertising the available
 *                                RCV descriptors
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_and_send_msgblk_mf(pmercd_ww_dev_info_sT  pwwDev)
{
 MD_STATUS                               Status;
 merc_ulong_t          msgblk_descr_table_index;
#ifdef LFS
 merc_ulong_t                           mfindex = 0;
#else
 merc_ulong_t                           mfindex;
#endif
 mercd_ww_get_mfAddress_sT          mfAddressST;
#ifdef LFS
 pmerc_uchar_t                       pmfAddress = 0;
 pmerc_uchar_t                          pmfHead = 0;
#else
 pmerc_uchar_t                       pmfAddress;
 pmerc_uchar_t                          pmfHead;
#endif
 merc_ulong_t                        numBigMsgs;
#ifdef LFS
 merc_ulong_t                numofBigMsgsPosted = 0;
#else
 merc_ulong_t                numofBigMsgsPosted;
#endif
 merc_uint_t                    mfFilledInSize;
 merc_uint_t                       msgType = 0;
 merc_ulong_t                          szBigMsg;
 merc_ulong_t                      szInBoundMfa;
 pmercd_adapter_block_sT               padapter;
 pmercd_ww_dma_descr_sT                 pbigmsg;
 pmerc_ww_rcvbigmsg_memory_sT        pbigmsgmem;
 merc_uint_t                      newMFRequired; /* If we ran out of the MF size */
 merc_ulong_t                      host_address;
 merc_ulong_t                     board_address;
#ifndef LFS
 merc_ulong_t                               ret;
#endif
 mercd_ww_dmaDscr_Index_sT            dmaDscrSt;
#ifndef MERCD_LINUX
 merc_ulong_t                        syncOffset;
 merc_ulong_t                          syncSize;
#endif
#ifndef LFS
 mercd_osal_timeout_start_sT timeoutinfo = { 0 };
 merc_uint_t                     mfTypeAndIndex;
#endif


 MSD_FUNCTION_TRACE(
        "msgutl_ww_build_and_send_msgblk_mf 0x%x %s #%d\n", 
         (size_t)pwwDev, 
         __FILE__, __LINE__
         );

 Status = MD_SUCCESS;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;


  //Get the Number and Size of Big Message Blocks
 numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
 szBigMsg = pwwDev->pww_param->sizeofBigMsgBlocks;
 szInBoundMfa = pwwDev->pww_param->sizeofMFAInboundQ;


 //Get a ref to RCV BIG Msg Descriptor table
 pbigmsgmem = &(pwwDev->BigMsgRcvMemStr);

 //Other misc structures
 dmaDscrSt.pBitMap  = pwwDev->BigMsgRcvMemStr.pBitMapValue;
 dmaDscrSt.szBitMap = pwwDev->BigMsgRcvMemStr.szBitMap;

 newMFRequired = 1;

 for (msgblk_descr_table_index=0;
      msgblk_descr_table_index < numBigMsgs;
      msgblk_descr_table_index++)
  {
    dmaDscrSt.index = msgblk_descr_table_index;
    MSD_ENTER_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex)
    Status = msgutl_ww_check_if_dmadescr_used(&dmaDscrSt);
    if (Status == MD_SUCCESS)
        {
           MSD_EXIT_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex)
           continue;
        }
    //Mark the corresponding bit in Bit Map as used
    msgutl_ww_mark_dmadescr_used(&dmaDscrSt);
    --pwwDev->BigMsgRcvMemStr.free_counter;
    vRcvFreeDesc = pwwDev->BigMsgRcvMemStr.free_counter;
    MSD_EXIT_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex)

    pbigmsg = pbigmsgmem->pbigMsgDescr[msgblk_descr_table_index];
    host_address  =  msgblk_descr_table_index;
    board_address =  (merc_ulong_t)(pbigmsg->board_address);


  //Check if we need a New MFA
  if ( newMFRequired )
    {
      //Get a MFA
      mfAddressST.pwwDevi = pwwDev;
      mfAddressST.pmfAddress = NULL;

      Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST );
      if (Status != MD_SUCCESS)
       {
         MSD_LEVEL2_DBGPRINT(
                              "msgutl_ww_build_and_send_msgblk_mf Failed %s #%d\n",
                               __FILE__,
                               __LINE__
                             );
         MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
         pwwDev->WWMsgPendingQFlag |= MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_BIT;
         MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
         Status = MD_FAILURE;
         return(Status);
       }

      //Store the returned MF address and index
      pmfAddress = mfAddressST.pmfAddress;
      mfindex   = mfAddressST.mfIndex;

      //Leave the same in pmfHead for pointer arith manipulation
      pmfHead = pmfAddress;

      //Store the MF Type in the MF
      *((pmerc_ulong_t)(pmfAddress)) = MERCD_WW_MSG_BLOCK_INIT_MSG;
      numofBigMsgsPosted = 0;
      pmfAddress += (2 * sizeof(merc_uint_t));
   } /* newMFRequired */

    newMFRequired = 0;

    *((pmerc_ulong_t)(pmfAddress)) = board_address;
    pmfAddress += (2 * sizeof(merc_uint_t));
    *((pmerc_ulong_t)(pmfAddress)) = host_address;
    pmfAddress += (2 * sizeof(merc_uint_t));
    numofBigMsgsPosted++;

    //Check if we ran out of MF space
    Status = msgutl_ww_msgblk_mf_full(szInBoundMfa, numofBigMsgsPosted);

    if (Status == MD_SUCCESS)
    {
     //MF is now Full. Time to Send down. But need to fill in the Type and
     //the numofBigMsgsPosted in the MF
      msgType = *((pmerc_uint_t)(pmfHead));
      mfFilledInSize =  ((2*sizeof(merc_uint_t)) +
                        ((numofBigMsgsPosted) * (4 * sizeof (merc_uint_t))));
      msgType = (msgType | mfFilledInSize);
      *((pmerc_uint_t)(pmfHead)) = msgType;
      pmfHead += sizeof(merc_uint_t);
      *((pmerc_uint_t)(pmfHead))= numofBigMsgsPosted;
     //Post the MF to the inBoundQ
#ifndef MERCD_LINUX
      syncOffset = ((pwwDev->inBoundQMfAddress) +
                  ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
      syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
      MsdWWSyncDevice(pwwDev);
#endif
      MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
      //In Future: We may need to write (msgType | mfindex) to
      //inBoundQ : higher 16bits --> msgType
      //         : lower  16bits --> mfindex
      //mfTypeAndIndex = (msgType | mfindex);
      MsdWWPutinBoundQReg(padapter, mfindex);
      MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);
      //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
      //atomic_inc(&pwwDev->pww_counters->bigmsg_blk_mfs_sent);

      //We need a New MF
      newMFRequired = 1;
    }

 }/* For numBigMsgs */


  //We are done Filling the Last MF. This could be partially filled.
  //Send it down after filling the Type and numofBigMsgsPosted in the MF

   Status = MD_SUCCESS;

  if(!(newMFRequired))
  {
     msgType = *((pmerc_uint_t)(pmfHead));
     mfFilledInSize =  ((2*sizeof(merc_uint_t)) +
                        ((numofBigMsgsPosted) *
                        (4 * sizeof (merc_uint_t))));
     msgType = (msgType | mfFilledInSize);
     *((pmerc_uint_t)(pmfHead)) = msgType;
     pmfHead += sizeof(merc_uint_t);
     *((pmerc_uint_t)(pmfHead))= numofBigMsgsPosted;

     //Post the MF to the inBoundQ
#ifndef MERCD_LINUX
     syncOffset = ((pwwDev->inBoundQMfAddress) +
                  ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
     syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
     MsdWWSyncDevice(pwwDev);
#endif
     MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
     //In Future: We may need to write (msgType | mfindex) to
     //inBoundQ : higher 16bits --> msgType
     //         : lower  16bits --> mfindex
     //mfTypeAndIndex = (msgType | mfindex);
     MsdWWPutinBoundQReg(padapter, mfindex);
     MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
     //atomic_inc(&pwwDev->pww_counters->bigmsg_blk_mfs_sent);
  }


return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_msgblk_mf_full
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Check if the MF is FULL
 *
 * Additional comments          :
 ****************************************************************************/

int msgutl_ww_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted)
{
 MD_STATUS Status;
 int maxNumOfBlocks;

 MSD_FUNCTION_TRACE("msgutl_ww_msgblk_mf_full 0x%x 0x%x %s #%d", (size_t)mfsize, 
                     (size_t)numMsgBlksPosted, __FILE__, __LINE__);
  // check to see whether we have filled up the Msg block initialization MF or not.

   mfsize = mfsize - MERCD_WW_MF_HEADER_SIZE;

  // calculate the max num of blocks that can fit
   maxNumOfBlocks = mfsize/(4 * sizeof(merc_uint_t));


   return ((numMsgBlksPosted == maxNumOfBlocks) ? (Status = MD_SUCCESS) : (Status = MD_FAILURE)
);
}

/***************************************************************************
 * Function Name                : msgutl_ww_maxdata_in_readwrite_mf
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *
 * Additional comments          : Returns the Max data that can be shipped in
 *                              : one MF
 ****************************************************************************/

int msgutl_ww_maxdata_in_readwrite_mf(merc_ulong_t mfsize,
                                  pmercd_stream_connection_sT StreamBlock)
{
#ifndef LFS
 MD_STATUS Status;
 int maxData;
#endif
 int maxDataBlocks = 0;

 MSD_FUNCTION_TRACE(
                 "msgutl_ww_maxdata_in_readwrite_mf 0x%x 0x%x %s #%d", (size_t)mfsize,
                  (size_t)StreamBlock,
                  __FILE__,
                  __LINE__);

  if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
   {
    //Total Header size for a Write MF is 32 Bytes
    //Type + StreamId                    = 8
    //Context                            = 8
    //StreamFlags                        = 8
    //Bytes Transferred + Container Size = 8
    mfsize = mfsize - MERCD_WW_WRITE_MF_HEADER_SIZE;

    //Calculate the max num of blocks that can fit
    //Each Phys Address Entry has: Address : 8
    //                             size    : 8
    maxDataBlocks = mfsize/(4 * sizeof(merc_uint_t));
    MSD_LEVEL2_DBGPRINT("maxDataBlocks in Write MF: %d\n", maxDataBlocks);
   }
  else
   {
    //BStream: Can send one 4032 DataBlock
    maxDataBlocks = 1;
   }

   return(maxDataBlocks);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_small_mf
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds a small Msg MF
 * Additional comments          : This MF caters to sending side
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_build_smallmsg_mf(pmercd_ww_build_smallMF_sT psmallSt)
{
  MD_STATUS           Status = MD_SUCCESS;
  PSTRM_MSG              Msg;
  pmerc_uchar_t   pmfAddress;
  pmerc_uchar_t   payLoadptr;
  merc_ulong_t    totalsz;
  merc_ulong_t    headersz;
  merc_ulong_t    payloadsz;

  MSD_FUNCTION_TRACE("msgutl_ww_build_smallmsg_mf 0x%x %s #%d\n", (size_t)psmallSt, __FILE__, __LINE__);

  Msg = (PSTRM_MSG)psmallSt->mb;
  pmfAddress = psmallSt->pmfAddress;


  //Store the MF Type in the MF
    *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_SMALL_MSG_TO_BOARD;

    pmfAddress += (sizeof(merc_uint_t));

  //Store the Total Size in MF: (Header size + Data size)
  //same as :- totalsz = ((Msg)->b_wptr - (Msg)->b_rptr);

    payloadsz = MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG)Msg->b_rptr));
    headersz = sizeof(MERC_HOSTIF_MSG);
    totalsz = payloadsz + headersz;


   *((pmerc_uint_t)(pmfAddress)) = totalsz;

    pmfAddress += (sizeof(merc_uint_t));

  //Now Copy the 24Byte Header in the next 24 Bytes of MF
    MsdCopyMemory((Msg)->b_rptr, pmfAddress, headersz);

    pmfAddress += headersz;

  //Now copy the Data Portion. !! We can copy 96Bytes of Data
    payLoadptr = (pmerc_uchar_t)((Msg)->b_rptr+headersz);
    MsdCopyMemory(payLoadptr, pmfAddress, payloadsz);

  return(Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_bigmsg_mf
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds a BIG Msg MF
 * Additional comments          : This MF caters to sending side
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_build_bigmsg_mf(pmercd_ww_build_bigMF_sT pbigSt)
{
  MD_STATUS                              Status;
  PSTRM_MSG                              Msg;
  PMDRV_MSG                              MdMsg;
  pmerc_uchar_t                          pmfAddress;
  pmerc_uchar_t                          pmfType;
  merc_uint_t                            mfFilledInSize;
  pmerc_uchar_t                          pmfAddressSz;
#ifndef LFS
  pmerc_uchar_t                          payLoadptr;
  merc_ulong_t                           totalsz;
  merc_ulong_t                           headersz;
  merc_ulong_t                           payloadsz;
#endif
  pmercd_ww_dev_info_sT                  pwwDev;
#ifndef LFS
  merc_uint_t                            count;
#endif
  mercd_ww_dmaDscr_Index_sT              dmaDescSt;
  merc_ushort_t                          dmaDescIndex;
#ifndef LFS
  merc_uint_t                            numDescriptors;
#endif
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                             bindHandle = 0;
#else
  MBD_HANDLE                             bindHandle = 0;
#endif
  pmercd_ww_dma_descr_sT                 pbigMsgDmaDescr;
  pmercd_ww_dma_descr_sT                 npbigMsgDmaDescr;
  mercd_dhal_ww_snd_stream_mblk_sT       dir_copy;
  PMERC_HOSTIF_MSG                       FwMessage;
  merc_uint_t                            xMsg;
  merc_uint_t                            destNum;
  merc_uint_t                            sourNum;

#ifndef LFS
  pmerc_uint_t                           pbmap;
#endif

  MSD_FUNCTION_TRACE("msgutl_ww_build_bigmsg_mf 0x%x %s #%d\n", (size_t)pbigSt, __FILE__, __LINE__);
  Status = MD_SUCCESS;
  pwwDev = pbigSt->pwwDevi;
  Msg = (PSTRM_MSG)pbigSt->mb;
  pmfAddress = pbigSt->pmfAddress;

  //Get the bindHandle
  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  bindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

  if (!bindHandle)
     MSD_LEVEL2_DBGPRINT("BisgMsg Send: Got BindHandle 0..\n");

  //Obtain the next available DMA descriptor Table index from
  //pwwDev->BigMsgSndMemStr.pbigMsgDescr table

 //Get the FwMessage to determine XMsg
  xMsg = 0;
  if (Msg->b_cont && Msg->b_cont->b_rptr) {
      FwMessage = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;
      destNum = MERCURY_GET_DEST_BOARD(FwMessage);
      sourNum = MERCURY_GET_SOURCE_BOARD(FwMessage);
      if ((sourNum) && (destNum) &&
          (destNum < MsdControlBlock->maxadapters) &&
          (mercd_adapter_log_to_phy_map_table[destNum] != 0xFF) &&
          (mercd_adapter_log_to_phy_map_table[destNum] !=
                  mercd_adapter_log_to_phy_map_table[sourNum]))  {
          MSD_LEVEL2_DBGPRINT("SndXmsg: dest=%d src=%d \n", destNum, sourNum);
          xMsg = 1;
      } 
  }

  dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
  MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
  Status = msgutl_ww_get_nextfree_dmadescr(&dmaDescSt);
  if (Status == MD_FAILURE)
  {
   //WWPENDING: No Free DMA descriptors. Are we going to create another
   //descriptor table and link it with the first one. This needs to be
   //addressed.
   //That means we need to allocate a new
   //pwwDev->BigMsgSndMemStr.next to create another descriptor table.
   MSD_LEVEL2_DBGPRINT("msgutl_ww_build_bigmsg_mf: No Free DMA Descriptors\n");
   //The following is useful for debug purpose
   vSndFreeDesc = pwwDev->BigMsgSndMemStr.free_counter;
   MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
   return (Status);
  }
  dmaDescIndex = dmaDescSt.index;
  //Now mark ths DMA descr at  dmaDescSt.index as used
  dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
  Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
  --pwwDev->BigMsgSndMemStr.free_counter;
  vSndFreeDesc = pwwDev->BigMsgSndMemStr.free_counter;

  if (!xMsg) {
    pmerc_uchar_t pbitmap;
    pmercd_bind_block_sT BindBlock;
    pbitmap = pwwDev->pPendMsgBitMap[bindHandle];
    dmaDescSt.pBitMap = pbitmap;
    dmaDescSt.szBitMap = pwwDev->szPendMsgBitMap;
    dmaDescSt.index = dmaDescIndex;
    Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
    MSD_LEVEL2_DBGPRINT("Big Msg Send: bindHandle: %d\n", bindHandle);
    //i_bprintf(pbitmap, 8);
    BindBlock = MsdControlBlock->pbind_block_list[bindHandle];
    if (BindBlock == NULL) {
       MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
       return(MD_FAILURE);
    }
    BindBlock->WWPendReadAckForToBoardBigMsgs++;
  }


  if (Status == MD_FAILURE)
   {
#ifndef LFS
    cmn_err(CE_WARN, "Could not mark DMA Descr as used %s #%d\n");
#endif
    MSD_LEVEL2_DBGPRINT("msgutl_ww_build_bigmsg_mf: Could not mark DMA Descr as used %s #%d\n", __FILE__, __LINE__);
    MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
    return (Status);
   }

  //Now we need to constuct the  descriptors at index
  //'dmaDescIndex' in our descriptor table. All these
  //descriptors in the table at dmaDescIndex are linked
  //by their next pointers. With in each mblk if (wp-rp)
  //cross a page boundary, then we need to allocate
  //additional descriptors. Also, if there
  //is an mblk on a b_cont chain, then also we alloc
  //additional descriptors

  dir_copy.pwwDevi = pwwDev;
  dir_copy.pdmaDescr = pwwDev->BigMsgSndMemStr.pbigMsgDescr[dmaDescIndex];
  dir_copy.mb = ((pmerc_void_t)(Msg));
  dir_copy.ret = MD_SUCCESS;
  dir_copy.purpose = 0;

  (*mercd_dhal_func[MERCD_DHAL_WW_SND_DIRCOPY_FROM_KERNEL])((pmerc_void_t)(&dir_copy));
  Status = dir_copy.ret;

  MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);

  if(Status == MD_FAILURE)
  {
   //Lets Think about the SCHEME2 Later
   //dir_copy.ret = MD_SUCCESS;
   //(*mercd_dhal_func[MERCD_DHAL_WW_SND_COPY_FROM_DMAABLE_BUFFERS])((pmerc_void_t)(&dir_copy));
   //Status = dir_copy.ret;
   if (Status != MD_SUCCESS)
    {
      Status = MD_FAILURE;
      return(Status);
    }
  }

  /////////////////////////////////////////////////////////
  //We can post a maximum of 7 descriptors in one MF. In the
  //worst case scenario, even if each mblk crosses one page
  //boundary, and there are 3 mblks on the b_cont chain, this
  //MF can cater to all the 3 mblks i.e 6 descriptors. If there
  //is a  4th mblk that do not cross the page boundary, then the
  //MF  can cater its descriptor too, as the 7th one.

  //Example: If the Msg train has 2 mblks on its b_cont chain,
  //where each mblk's b_wptr crosses the page boundary, then we
  //will have 4 descriptors at the smae index.
  //
  //  dmaDescIndex : |0|->|1|->|2|->|3|
  //
  //Ex:If there are 4 descriptors at dmaDescIndex, then 0 and 1
  //might correspond to the first mblk (this mblk data might
  //have crossed the page boundary), similarly 2 and 3 might
  //correspond to the next mblk on the b_cont chain
  //All these 4 descriptors can conveniently be shipped in one MF

  //Now we go by the assumption that we will never have to deal
  //with more than 1 mblk on sending side. In future we will have
  //to deal with multiple mblks on b_cont chain, for which one MF
  //with 112 bytes for descriptor space is not enough

  //WWPENDING: we need to find a work around where we have to deal
  //with > 7 descriptors at a specified index. We can not ship
  //all of them in a single MF though.
  ///////////////////////////////////////////////////////////////

  npbigMsgDmaDescr = pbigMsgDmaDescr =
              pwwDev->BigMsgSndMemStr.pbigMsgDescr[dmaDescIndex];

  //Store the MF Type in the MF
  if (xMsg) {
      pmfType = pmfAddress;
      *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_BIG_XMSG_TO_BOARD;
  } else {
      pmfType = pmfAddress;
      *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_BIG_MSG_TO_BOARD;
  }

  pmfAddress += (sizeof(merc_uint_t));

  //Lets come back and write the size of mblk later
  pmfAddressSz = pmfAddress;
  *((pmerc_uint_t)(pmfAddressSz)) = 0;
  pmfAddress += (sizeof(merc_uint_t));

#if 1 
///////////////////////////////////////////////////
  //Store the index
  // Null contexts sent as 0xFF - need to decode in ack
  if (!dmaDescIndex) {
      *((pmerc_ushort_t)(pmfAddress)) = 0xFF;
  } else {
      *((pmerc_ushort_t)(pmfAddress)) = dmaDescIndex;
  }

  //Store the BindHandle
  pmfAddress += (sizeof(merc_ushort_t));

#ifdef _8_BIT_INSTANCE
  *((pmerc_ushort_t)(pmfAddress)) = bindHandle;
#else
  *((pmerc_uint_t)(pmfAddress)) = bindHandle;
#endif
  //Now write the physical address and size from the descriptors,
  //But bump pmfAddress by 6 bytes(NOT 4bytes)
  pmfAddress += (sizeof(merc_ushort_t));
  pmfAddress += (sizeof(merc_uint_t));
///////////////////////////////////////////////////
#endif

#if 0
///////////////////////////////////////////////////
  //Store the index
  *((pmerc_uint_t)(pmfAddress)) = dmaDescIndex;
  //Store the BindHandle
  pmfAddress += (sizeof(merc_uint_t));
#ifdef _8_BIT_INSTANCE
  *((pmerc_uint_t)(pmfAddress)) = bindHandle;
#else
  *((pmerc_uint_t)(pmfAddress)) = bindHandle;
#endif

 //Now write the physical address and size from the descriptors,
 //But bump pmfAddress by 6 bytes(NOT 4bytes)
 pmfAddress += (sizeof(merc_uint_t));

///////////////////////////////////////////////////
#endif

  //We need to OR the MF type with the filled in size. Will do
  //it after filling the rest of MF
  mfFilledInSize = (4*sizeof(merc_uint_t));


  while (npbigMsgDmaDescr)
   {
    *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)(npbigMsgDmaDescr->board_address);
    pmfAddress += (2 * sizeof(merc_uint_t));
    mfFilledInSize += (2 * sizeof(merc_uint_t));
    *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)(npbigMsgDmaDescr->size);
    pmfAddress += (2 * sizeof(merc_uint_t));
    mfFilledInSize += (2 * sizeof(merc_uint_t));
    *((pmerc_uint_t)(pmfAddressSz)) += npbigMsgDmaDescr->size;
    npbigMsgDmaDescr = npbigMsgDmaDescr->next;
  } /* while npbigMsgDmaDescr */
 //Now write the (mfType | mfFilledInSize) to the
 //first 4bytes of MF, which is "Message Type" field.
  *((pmerc_ulong_t)(pmfType)) =  (*((pmerc_ulong_t)(pmfType)) | mfFilledInSize);


    //Now time to ship the current MF
 return (Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_cancel_stream_mf
 * Function Type                : Support Function for WW Msg Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This builds and sends a cancel stream MF to
 *                                FW.
 *
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_and_send_cancel_stream_mf(pmercd_bind_block_sT BindBlock)
{
 MD_STATUS                                    Status;
 pmercd_ww_dev_info_sT                        pwwDev;
 pmercd_adapter_block_sT                    padapter;
 merc_uint_t                                streamid;
 pmerc_uchar_t                            pmfAddress;
 pmerc_uchar_t                               pmfHead;
 merc_ulong_t                                mfindex;
 mercd_ww_get_mfAddress_sT               mfAddressST;
 pmercd_stream_connection_sT             StreamBlock;
 merc_uint_t                                  mfType;
#ifndef MERCD_LINUX
 mercd_ww_dmaDscr_Index_sT                 dmaDescSt;

 merc_ulong_t                             syncOffset;
 merc_ulong_t                               syncSize;
#endif

 MSD_FUNCTION_TRACE("msgutl_ww_build_and_send_cancel_stream_mf 0x%x %s #%d\n",
                                   (size_t)BindBlock, __FILE__, __LINE__);

 Status = MD_SUCCESS;

 StreamBlock =  BindBlock->stream_connection_ptr;

 if(StreamBlock == NULL)
  {
    MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_send_cancel_stream_mf: This Should Never Happen..\n");
  }

 padapter = StreamBlock->padapter_block;
 pwwDev = padapter->pww_info;
 streamid = StreamBlock->id;


  //Get a MFA
  mfAddressST.pwwDevi = pwwDev;
  mfAddressST.pmfAddress = NULL;


  Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST );


  if (Status != MD_SUCCESS)
   {
     //cmn_err(CE_CONT, "msgutl_ww_build_and_cancel_stream_mf Failed\n");
     MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_cancel_stream_mf Failed\n");
     //Need to think about no MFA situation!!!!!
     //Start the timer that schedules this function at a later time
     //For Cancel alone big mutex: So removed this:
     pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_CANCEL_STREAM_MF_BIT);
     BindBlock->WWCancelStrmAckPending = 0;
     BindBlock->WWCancelStrmMFPending = 1;
     Status = MD_FAILURE;
     return(Status);
   }
  else
   {
     BindBlock->WWCancelStrmMFPending = 0;
   }

  //Store the returned MF address and index
  pmfHead = pmfAddress = mfAddressST.pmfAddress;
  mfindex   = mfAddressST.mfIndex;

  //Store the MF Type in the MF
  *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_STREAM_REQ_CANCEL_TO_BOARD;


  //Go to the next field
  pmfAddress += (sizeof(merc_uint_t));

  //Store the stream id
  *((pmerc_uint_t)(pmfAddress)) = streamid;

   pmfAddress += (sizeof(merc_uint_t));

  //Store the stream mode
  if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
     *((pmerc_uint_t)(pmfAddress)) = 0x1; /*kernel wants a 1 for read stream */
  else
     *((pmerc_uint_t)(pmfAddress)) = 0x2;

   pmfAddress += (sizeof(merc_uint_t));

#if 0
  //What user context we need to give !!!!
  //!!!!!!!!!!!!
  *((pmerc_ulong_t)(pmfAddress)) = BindBlock;
#endif 

  //Fill in the total filled in size
  mfType = *((pmerc_uint_t)(pmfHead));
  mfType = ((mfType) | (3 * sizeof(merc_uint_t)));
  *((pmerc_uint_t)(pmfHead)) = mfType;

  //Post the MF to the inBoundQ
#ifndef MERCD_LINUX
  syncOffset = ((pwwDev->inBoundQMfAddress) +
                ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
  syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
  MsdWWSyncDevice(pwwDev);
#endif

  MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_send_cancel_stream_mf...bef index\n");
  MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
  MsdWWPutinBoundQReg(padapter, mfindex);
  MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

  //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
  //atomic_inc(&pwwDev->pww_counters->cancel_strm_mfs_sent);

  MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_send_cancel_stream_mf...aft index\n");
 return (Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_cancel_bigmsg_mf
 * Function Type                : Support Function for WW Msg Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This builds and sends a cancel RCV BIG MSG MF
 *                                to FW.
 *
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_and_send_cancel_bigmsg_mf(pmercd_ww_dev_info_sT pwwDev,
                                          pmercd_bind_block_sT  BindBlock)
{
 MD_STATUS                               Status;
 merc_ushort_t                            index;
#ifdef LFS
 merc_ulong_t                           mfindex = 0;
#else
 merc_ulong_t                           mfindex;
#endif
 mercd_ww_get_mfAddress_sT          mfAddressST;
#ifdef LFS
 pmerc_uchar_t                       pmfAddress = 0;
 pmerc_uchar_t                          pmfHead = 0;
#else
 pmerc_uchar_t                       pmfAddress;
 pmerc_uchar_t                          pmfHead;
#endif
 merc_ulong_t                        numBigMsgs;
#ifdef LFS
 merc_ulong_t                numofCancelsPosted = 0;
#else
 merc_ulong_t                numofCancelsPosted;
#endif
 merc_ulong_t                    mfFilledInSize;
 merc_ulong_t                       msgType = 0;
 merc_ulong_t                          szBigMsg;
 merc_ulong_t                      szInBoundMfa;
 pmercd_adapter_block_sT               padapter;
 pmercd_ww_dma_descr_sT                 pbigmsg;
 pmerc_uchar_t                          pbitmap;
 merc_uint_t                      newMFRequired; /* If we ran out of the MF size */
#ifndef MERCD_LINUX
 merc_ulong_t                               ret;
#endif
 mercd_ww_dmaDscr_Index_sT            dmaDscrSt;
 PSTRM_MSG                                  Msg;
 PMDRV_MSG                                MdMsg;

#ifdef _8_BIT_INSTANCE
 MSD_HANDLE                         bindHandle;
#else
 MBD_HANDLE                         bindHandle;
#endif

#ifndef MERCD_LINUX
 mercd_osal_timeout_start_sT timeoutinfo = { 0 };
#endif

#ifndef MERCD_LINUX
 merc_ulong_t                        syncOffset;
 merc_ulong_t                          syncSize;
#endif

 MSD_FUNCTION_TRACE("msgutl_ww_build_and_send_cancel_bigmsg_mf 0x%x %s #%d\n", 
                     (size_t)pwwDev, __FILE__, __LINE__); 

 Status = MD_SUCCESS;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;


 //Get the Number and Size of Big Message Blocks
 numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
 szBigMsg = pwwDev->pww_param->sizeofBigMsgBlocks;
 szInBoundMfa = pwwDev->pww_param->sizeofMFAInboundQ;


 //Other misc structures
 pbitmap = dmaDscrSt.pBitMap = pwwDev->pPendMsgBitMap[BindBlock->bindhandle];
 dmaDscrSt.szBitMap  = pwwDev->szPendMsgBitMap;

 //Cancel MF Fix
 bindHandle = BindBlock->bindhandle;
 

 newMFRequired = 1;


  for (index=0; index < pwwDev->szPendMsgBitMap; index++)
  {
    dmaDscrSt.index = index;
    MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex)
    Status = msgutl_ww_check_if_dmadescr_used(&dmaDscrSt);
    if (Status == MD_FAILURE)
        {
           MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex)
           continue;
        }
    MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex)

    pbigmsg = pwwDev->BigMsgSndMemStr.pbigMsgDescr[index];
    if (!(pbigmsg->flag & MERCD_WW_DMA_DESCR_VBIT))
     {
        continue;
     }
 
    //Do the double checking: if the Msg is on the same BindBlock
    Msg = (PSTRM_MSG)pbigmsg->host_address;
    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

    if (bindHandle != MD_GET_MDMSG_BIND_HANDLE(MdMsg))
     {
       MSD_LEVEL2_DBGPRINT("cancel_bigmsg_mf: This Msg is not on the same BindBlock for CANCEL\n");
       //What needs to be done for this!!!!!
       //continue for now
       continue;
     }

      //Check if we need a New MFA
     if ( newMFRequired )
     {
      //Get a MFA
      mfAddressST.pwwDevi = pwwDev;
      mfAddressST.pmfAddress = NULL;

      Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST );

      if (Status != MD_SUCCESS)
       {
         cmn_err(CE_CONT, "msgutl_ww_build_and_send_cancel_bigmsg_mf Failed\n");
         MSD_LEVEL2_DBGPRINT("%s #%d\n", __FILE__, __LINE__);
         //Removed the mutex for the cancel alone:
         //MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
         pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_CANCEL_FROM_BD_BIGMSGBUF_MF_BIT);
         BindBlock->WWCancelBigMsgMFPending = 1;
         BindBlock->WWCancelBigMsgAckPending = 0;
         //MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
         Status = MD_FAILURE;
         return(Status);
       }

      //Store the returned MF address and index
      pmfAddress = mfAddressST.pmfAddress;
      mfindex   = mfAddressST.mfIndex;

      //Leave the same in pmfHead for pointer arith manipulation
      pmfHead = pmfAddress;

      //Store the MF Type in the MF
      *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_BIG_MSG_CANCELLATION_TO_BOARD;
      numofCancelsPosted = 0;
      pmfAddress += (2 * sizeof(merc_uint_t));
     } /* newMFRequired */

    newMFRequired = 0;

#if 0
    *((pmerc_uint_t)(pmfAddress)) = index;
    pmfAddress += (sizeof(merc_uint_t));
    *((pmerc_uint_t)(pmfAddress)) = BindBlock->bindhandle;
    numofCancelsPosted++;
    pmfAddress += (sizeof(merc_uint_t));
#endif

    *((pmerc_ushort_t)(pmfAddress)) = index;
    pmfAddress += (sizeof(merc_ushort_t));
    *((pmerc_ushort_t)(pmfAddress)) = BindBlock->bindhandle;
    numofCancelsPosted++;
    pmfAddress += (sizeof(merc_ushort_t));
    pmfAddress += (sizeof(merc_uint_t));

    //No need to check pbigmsg->next as there are no
    //no next pointers on sending descriptors

    //Check if we ran out of MF space
    Status = msgutl_ww_cancel_msgblk_mf_full(szInBoundMfa, numofCancelsPosted);

    if (Status == MD_SUCCESS)
    {
     //MF is now Full. Time to Send down. But need to fill in the Type and
     //the numofCancelsPosted in the MF
      msgType = *((pmerc_uint_t)(pmfHead));
      mfFilledInSize =  ((2*sizeof(merc_uint_t)) +
                        ((numofCancelsPosted) * (2 * sizeof (merc_uint_t))));
      msgType = (msgType | mfFilledInSize);
      *((pmerc_uint_t)(pmfHead)) = msgType;
      pmfHead += sizeof(merc_uint_t);
      *((pmerc_uint_t)(pmfHead))= numofCancelsPosted;

      //Post the MF to the inBoundQ

#ifndef MERCD_LINUX
      syncOffset = ((pwwDev->inBoundQMfAddress) +
                  ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
      syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
      MsdWWSyncDevice(pwwDev);
#endif
      MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
      MSD_LEVEL2_DBGPRINT("Sending one FULL cancel BigMsg MF...\n");
      MsdWWPutinBoundQReg(padapter, mfindex);
      MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

      //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);

      //We need a New MF
      newMFRequired = 1;
    }

 }/* For numBigMsgs on this BindBlock*/


   //We are done Filling the Last MF. This could be partially filled.
  //Send it down after filling the Type and numofBigMsgsPosted in the MF

   Status = MD_SUCCESS;

  if(!(newMFRequired))
  {
     msgType = *((pmerc_uint_t)(pmfHead));
     mfFilledInSize =  ((2*sizeof(merc_uint_t)) +
                        ((numofCancelsPosted) *
                        (2 * sizeof (merc_uint_t))));
     msgType = (msgType | mfFilledInSize);
     *((pmerc_uint_t)(pmfHead)) = msgType;
     pmfHead += sizeof(merc_uint_t);
     *((pmerc_uint_t)(pmfHead))= numofCancelsPosted;

     //Post the MF to the inBoundQ
#ifndef MERCD_LINUX
     syncOffset = ((pwwDev->inBoundQMfAddress) +
                  ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
     syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
     MsdWWSyncDevice(pwwDev);
#endif
     MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
     MSD_LEVEL2_DBGPRINT("Sending one PARTIAL cancel BigMsg MF...\n");
     MsdWWPutinBoundQReg(padapter, mfindex);
     MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
     //atomic_inc(&pwwDev->pww_counters->cancel_bigmsg_mfs_sent);
  }

 return (Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_cancel_msgblk_mf_full
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Check if the MF is FULL
 *
 * Additional comments          :
 ****************************************************************************/

int msgutl_ww_cancel_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted)
{
 MD_STATUS Status;
 int maxNumOfBlocks;

 MSD_FUNCTION_TRACE("msgutl_ww_cancel_msgblk_mf_full 0x%x 0x%x %s #%d",
                     (size_t)mfsize, (size_t)numMsgBlksPosted, __FILE__, __LINE__); 
  // check to see whether we have filled up the Msg block initialization MF or not.

   mfsize = mfsize - MERCD_WW_MF_HEADER_SIZE;

  // calculate the max num of blocks to be
  // cancelled that can fit
   maxNumOfBlocks = mfsize/(2 * sizeof(merc_uint_t));


 return ((numMsgBlksPosted == maxNumOfBlocks) ? (Status = MD_SUCCESS) : (Status = MD_FAILURE));
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_streams_flush_mf
 * Function Type                : Support Function for WW Msg Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This builds and sends a Streams FLUSH MF
 *                                to FW.
 *
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_and_send_streams_flush_mf(pmercd_ww_flush_streamMF pflushMf)
{
 MD_STATUS                               Status;
 merc_ulong_t                           mfindex;
 mercd_ww_get_mfAddress_sT          mfAddressST;
 pmerc_uchar_t                       pmfAddress;
#ifndef MERCD_LINUX
 pmerc_uchar_t                          pmfHead;
 merc_ulong_t                       msgType = 0;
 merc_ulong_t                      szInBoundMfa;
#endif
 pmercd_adapter_block_sT               padapter;
#ifndef MERCD_LINUX
 merc_ulong_t                               ret;
#endif
 merc_uint_t                           streamid;
 pmercd_ww_dev_info_sT                   pwwDev;
 pmercd_stream_connection_sT        StreamBlock;
 mercd_ww_dmaDscr_Index_sT          flushDscrSt;


#ifndef MERCD_LINUX
 merc_ulong_t                        syncOffset;
 merc_ulong_t                          syncSize;
#endif

 MSD_FUNCTION_TRACE("msgutl_ww_build_and_send_streams_flush_mf 0x%x %s #%d\n",
                     (size_t)pflushMf, __FILE__, __LINE__); 

 Status = MD_SUCCESS;

 padapter = (pmercd_adapter_block_sT)pflushMf->padapter;
 streamid = pflushMf->streamid;
 pwwDev = padapter->pww_info;

  //Get a MFA
  mfAddressST.pwwDevi = pwwDev;
  mfAddressST.pmfAddress = NULL;

  Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST );

  if (Status != MD_SUCCESS)
   {
     cmn_err(CE_CONT, "msgutl_ww_build_and_send_streams_flush_mf Failed\n");
     MSD_LEVEL2_DBGPRINT("%s #%d\n", __FILE__, __LINE__);

     //No need of Mutex: Since this  routine is called 
     //after acquiring pwwDev->ww_msgpendq_mutex
     pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT);
     //Set the proper bit in the global bitmap for stremas for pending
     //stream flush MFs
     flushDscrSt.pBitMap = pwwDev->pPendFlushStrBitMap;
     flushDscrSt.szBitMap = pwwDev->szPendFlushStrBitMap;
     flushDscrSt.index =  streamid; 
     Status = msgutl_ww_check_if_dmadescr_used(&flushDscrSt);
     if (Status == MD_SUCCESS)
      {
       //Restart the timer and return
       Status = MD_FAILURE;
       return(Status);
       }
     else
      {
        Status =  msgutl_ww_mark_dmadescr_used(&flushDscrSt);
      }
     StreamBlock = padapter->pstream_connection_list[streamid];
     StreamBlock->pbind_block->WWFlushMFPending = 1;
     StreamBlock->pbind_block->WWFlushAckPending = 0;
     Status = MD_FAILURE;
     return(Status);
   }
  else
   {
    StreamBlock = padapter->pstream_connection_list[streamid];
    StreamBlock->pbind_block->WWFlushMFPending = 0;
    StreamBlock->pbind_block->WWFlushAckPending = 1;
   }

  //Store the returned MF address and index
  pmfAddress = mfAddressST.pmfAddress;
  mfindex   = mfAddressST.mfIndex;

  //Store the MF Type in the MF
  *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_STREAM_FLUSH_REQUEST_TO_BOARD;

  //Go to the next field
  pmfAddress += (sizeof(merc_uint_t));

  //Store the stream id
  *((pmerc_uint_t)(pmfAddress)) = streamid;

  //Post the MF to the inBoundQ
#ifndef MERCD_LINUX
  syncOffset = ((pwwDev->inBoundQMfAddress) +
                ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
  syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
  MsdWWSyncDevice(pwwDev);
#endif

  MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
  MsdWWPutinBoundQReg(padapter, mfindex);
  MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

  //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
  //atomic_inc(&pwwDev->pww_counters->strm_flush_mfs_sent);

 return (Status);

}

/***************************************************************************
 * Function Name                : msgutl_ww_build_write_stream_mf
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds a BIG Msg MF
 * Additional comments          : This MF caters to sending side
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_write_stream_mf(pmercd_ww_send_streamData pStreamDataSt)
{
  MD_STATUS                              Status;
  PSTRM_MSG                              Msg;
  PMDRV_MSG                              MdMsg;
  PSTREAM_SEND                           Ptr;
  pmerc_uchar_t                          pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                          pmfAddressActSz;
  pmerc_uchar_t                          pmfAddressContainerSz;
  pmercd_ww_dev_info_sT                  pwwDev;
#ifndef MERCD_LINUX
  mercd_ww_dmaDscr_Index_sT              dmaDescSt;
#endif
  merc_ushort_t                          dmaDescIndex;
  merc_uint_t                            msgType;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                             bindHandle;
#else
#ifndef MERCD_LINUX
  MBD_HANDLE                             bindHandle;
#endif
#endif
  pmercd_ww_dma_descr_sT                 pDmaDescr;
  pmercd_ww_dma_descr_sT                 npDmaDescr;
  mercd_dhal_ww_snd_stream_mblk_sT      dir_copy;
  pmercd_adapter_block_sT                padapter;
  pmercd_stream_connection_sT            StreamBlock;
  merc_uint_t                            maxDataBlocks;
  merc_uint_t                            maxdataLength;
  merc_uint_t                            mfFilledInSize = 0;
  merc_uint_t                            numDescr = 0;
  merc_uint_t                            szInBoundMfa;
 



  MSD_FUNCTION_TRACE("msgutl_ww_build_write_stream_mf 0x%x %s #%d\n", 
                     (size_t)pStreamDataSt, __FILE__, __LINE__); 


  Status = MD_SUCCESS;
  Msg = (PSTRM_MSG)pStreamDataSt->Msg;
  padapter = (pmercd_adapter_block_sT)pStreamDataSt->padapter;
  StreamBlock = (pmercd_stream_connection_sT)pStreamDataSt->StreamBlock;
  pmfAddress = (pmerc_uchar_t)pStreamDataSt->pmfAddress;

  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

  pwwDev = padapter->pww_info;
  //Obtain the next available DMA descriptor Table index from

  ///////////////////////////////////////////////////////
  //Getting the next DMA Descr Index has been moved to
  //snd_ww_strm_data2adapter
  /////////////////////////////////////////////////////
   dmaDescIndex = pStreamDataSt->dmaDescIndex;
  /////////////////////////////////////////////////////

  //Now we need to constuct the  descriptors at index
  //'dmaDescIndex' in our descriptor table. All these
  //descriptors in the table at dmaDescIndex are linked
  //by their next pointers. With in each mblk if (wp-rp)
  //cross a page boundary, then we need to allocate
  //additional descriptors. Also, if there
  //is an mblk on a b_cont chain, then also we alloc
  //additional descriptors

  dir_copy.pdmaDescr = (pmercd_ww_dma_descr_sT)(StreamBlock->pSendStrmDmaDesc[dmaDescIndex]);
  dir_copy.pwwDevi = pwwDev;
  dir_copy.mb = ((pmerc_void_t)(Msg));
  dir_copy.ret = MD_SUCCESS;

  if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
   {
    dir_copy.purpose = MERCD_WW_GSTRM_WRITE_REQUEST;
   }
  else
   {
    dir_copy.purpose = MERCD_WW_BSTRM_WRITE_REQUEST;
   }

  /////////////////////////////////////////////////////////
  //Testing.....
  /////////////////////////////////////////////////////////
  {
    if ((dir_copy.pdmaDescr->host_address) ||
       (dir_copy.pdmaDescr->board_address) ||
       (dir_copy.pdmaDescr->size))
    {
      MSD_LEVEL2_DBGPRINT("msgutl_ww_build_write_stream_mf: Got dmaDescIndex: %d, but\
              this looks to be already allocd to a previous write..\n", dmaDescIndex);
      MSD_LEVEL2_DBGPRINT("host_address: 0x%x board_address: 0x%x size: %d\n",
              dir_copy.pdmaDescr->host_address, dir_copy.pdmaDescr->board_address,
              dir_copy.pdmaDescr->size);
    }

  }

  {
     if (((size_t)(Msg->b_rptr)| (PAGE_SIZE - 1)) > (size_t) (Msg->b_wptr - 1))
      {
      }
     else
      {
        MSD_LEVEL2_DBGPRINT("msgutl_ww_build_write_stream_mf:\
                First Msg crossed the page bndary..\n");
      }
   }
  ////////////////////////////////////////////////////////
  //Testing End....
  ////////////////////////////////////////////////////////
  MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
  (*mercd_dhal_func[MERCD_DHAL_WW_SND_DIRCOPY_FROM_KERNEL])((pmerc_void_t)(&dir_copy));
  MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex)
  Status = dir_copy.ret;

  if(Status == MD_FAILURE)
  {
    MSD_LEVEL2_DBGPRINT
            (
              "SND_DIRCOPY_FROM_KERNEL Failed. Calling Scheme2 %s #%d\n", 
                        __FILE__,
                        __LINE__
             );
   dir_copy.ret = MD_SUCCESS;
   (*mercd_dhal_func[MERCD_DHAL_WW_SND_COPY_FROM_DMAABLE_BUFFERS])((pmerc_void_t)(&dir_copy));
   Status = dir_copy.ret;
   if (Status != MD_SUCCESS)
    {
      Status = MD_FAILURE;
      return(Status);
    }
  }

  npDmaDescr = pDmaDescr = StreamBlock->pSendStrmDmaDesc[dmaDescIndex];

  if (Ptr->Flags & MD_MSG_FLAG_LATENCY) {
      pDmaDescr->flag |= 0x80;
  }

  //Hold onto MF Head
  pmfHead = pmfAddress;

  //Store the MF Type in the MF
  *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_WRITE_REQUEST;

  pmfAddress += (sizeof(merc_uint_t));

  //Store the Stream Id
  *((pmerc_uint_t)(pmfAddress)) = StreamBlock->id;

  pmfAddress += (sizeof(merc_uint_t));

  //Store the Write Stream Context
  *((pmerc_uint_t)(pmfAddress)) = dmaDescIndex;
  pmfAddress += (sizeof(merc_uint_t));
  *((pmerc_uint_t)(pmfAddress)) = StreamBlock->id;

  //Store the StreamBlock flags field
  pmfAddress += (sizeof(merc_uint_t));
  *((pmerc_ushort_t)(pmfAddress)) = StreamBlock->flags;

  pmfAddress += (2 * sizeof(merc_uint_t));


  //Actual Bytes to Transfer.
  pmfAddressActSz = pmfAddress;
  *((pmerc_uint_t)(pmfAddressActSz))= 0;    
  pmfAddress += (sizeof(merc_uint_t));

  //Container Size:
  //Fill it depending on the type of stream
  pmfAddressContainerSz = pmfAddress;
  *((pmerc_uint_t)(pmfAddressContainerSz)) = 0;
  pmfAddress += (sizeof(merc_uint_t));


  //Calculate the MaxDataBlocks that can be shipped in one Write MF
  szInBoundMfa = pwwDev->pww_param->sizeofMFAInboundQ;
  maxDataBlocks= msgutl_ww_maxdata_in_readwrite_mf(szInBoundMfa, StreamBlock);
 
  if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
   {
     maxdataLength = ((maxDataBlocks) * 
                     (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)));
   }
  else
   {
     maxdataLength = ((maxDataBlocks) * (MERCURY_HOST_IF_BLK_SIZE));
   }

 while(npDmaDescr)
  {
    *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)(npDmaDescr->board_address);
    pmfAddress += (2 * sizeof(merc_uint_t));
    *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)(npDmaDescr->size);
    pmfAddress += (2 * sizeof(merc_uint_t));
    *((pmerc_uint_t)(pmfAddressActSz))+= (merc_ulong_t)(npDmaDescr->size);
    numDescr++;

    if ((numDescr-1) > (maxDataBlocks)) {
     //The only way this could happen when the data
     //crosses the page boundary. Could happen with
     //mutli-block write. In multiblock write there
     //could be upto 3 descriptors with one page 
     //bounday cross.
     //But the max data limited to: 24+4032
     //                            =4056
    } 

    if ((*((pmerc_uint_t)(pmfAddressActSz))) > ((maxdataLength)+sizeof(USER_HEADER)))
     {
       MSD_LEVEL2_DBGPRINT("numDescr: %d maxDataBlocks: %d ActSize: %d maxdataLength: %d\n",
               numDescr, maxDataBlocks, *((pmerc_uint_t)(pmfAddressActSz)), maxdataLength);

       MSD_LEVEL2_DBGPRINT("This is UNUSUAL...INVESTIGATE..Max Data Should not exceed: 4056\n");
       Status = MD_FAILURE;
       return(Status);

     }
    npDmaDescr = npDmaDescr->next;
  }

 if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
  {
    //GStream: Container Size is the same as the
    //Bytes Transferred
    *((pmerc_uint_t)(pmfAddressContainerSz)) = *((pmerc_uint_t)(pmfAddressActSz));
  }
 else
  {
     *((pmerc_uint_t)(pmfAddressContainerSz)) =
                   MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER);
  }

  //Stuff the filledInSize in msgType
  mfFilledInSize = ((MERCD_WW_WRITE_MF_HEADER_SIZE) + 
                      ((numDescr) * (4 * sizeof(merc_uint_t))));

  msgType = *((pmerc_uint_t)(pmfHead));
  msgType = (msgType | mfFilledInSize);
  *((pmerc_uint_t)(pmfHead)) = msgType;


 //Now time to ship the current MF
 return (Status);
}

/***************************************************************************
 * Function Name                : strm_ww_put_eos_Q
 * Function Type                : Support Function for WW Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams WW Driver strm_ww_put_eos_Q Routine.
 *                              : This routine queues the Message on the
 *                              : pww_eos_msgq
 * Additional comments          :
 ****************************************************************************/
void strm_ww_put_eos_Q(pmercd_ww_dev_info_sT pwwDev, PSTRM_MSG Msg)
{
  PSTRM_MSG       Tail;

  MSD_FUNCTION_TRACE("strm_ww_put_eos_Q", TWO_PARAMETERS,
	    (size_t)pwwDev,
	    (size_t)Msg);

  if((Tail=pwwDev->pww_eos_msgq)==NULL){
	pwwDev->pww_eos_msgq= Msg;
	return;
  }

 // find the tail of the stream block SendStreamQueue
 while(Tail->b_next)
     Tail = Tail->b_next;

 // link the new message to the end of the send queue
 Tail->b_next = Msg;
 return;
}

/***************************************************************************
 * Function Name                : strm_ww_get_eos_Q
 * Function Type                : Support Function for WW Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams WW Driver strm_ww_get_eos_Q Routine.
 *                              : This routine removes the next Message on the
 *                              : pww_eos_msgq
 * Additional comments          :
 ****************************************************************************/
PSTRM_MSG strm_ww_get_eos_Q(pmercd_ww_dev_info_sT pwwDev)
{
 PSTRM_MSG    Msg;
 PSTREAM_SEND Ptr;
 PMDRV_MSG    MdMsg;

 MSD_FUNCTION_TRACE("strm_free_only_data_msgs", ONE_PARAMETER, (size_t)pwwDev);

 
 if (pwwDev->pww_eos_msgq == NULL)
    return NULL;

 Msg = pwwDev->pww_eos_msgq;
 //Additional Check: We should have only EOS Msgs on this Q
 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
 Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
 if (!(Ptr->Flags & STREAM_FLAG_EOS))
 {
  MSD_LEVEL2_DBGPRINT("EOS Pend Q Has non EOS Messages\n");
  return(NULL);
 }
 pwwDev->pww_eos_msgq = Msg->b_next;
 Msg->b_next = NULL;
 return (Msg);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_multi_eos_mf
 * Function Type                : WW Send Data Support function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW Driver snd_ww_strm_data2adapter Routine.
 *                                This routine sends data to adapter.
 * Additional comments          :
 ****************************************************************************/
int msgutl_ww_build_and_send_multi_eos_mf(pmercd_adapter_block_sT padapter)
{
 MD_STATUS                  Status;
#ifdef LFS
 merc_ulong_t               mfindex =0;
#else
 merc_ulong_t               mfindex;
#endif
 mercd_ww_get_mfAddress_sT  mfAddressST;
#ifdef LFS
 pmerc_uchar_t              pmfAddress = 0;
 pmerc_uchar_t              pmfHead = 0;
#else
 pmerc_uchar_t              pmfAddress;
 pmerc_uchar_t              pmfHead;
#endif
 pmercd_ww_dev_info_sT      pwwDev;
#ifndef MERCD_LINUX
 mercd_ww_build_eos_sT      eosmfSt;
 merc_ulong_t               syncOffset;
 merc_ulong_t               syncSize;
#endif
 PSTRM_MSG                  Msg;
 PMDRV_MSG                  MdMsg;
 PSTREAM_SEND               Ptr;
 merc_uint_t                newMFRequired = 1;
 merc_uint_t                 szInBoundMfa;
 merc_uint_t                 numEntries = 0;
 merc_uint_t                 maxNumEntries = 0;
 merc_uint_t                 mfShipped = 0;
 PSTRM_MSG                   eos_not_ready = 0;
 PSTRM_MSG                   eos_not_ready_head = 0;
 pmercd_stream_connection_sT StreamBlock;
 merc_uint_t needTimer = 0;
 merc_uint_t mfReady = 0;


 MSD_FUNCTION_TRACE(" msgutl_ww_build_and_send_multi_eos_mf 0x%x %s #%d\n",
                     (size_t)padapter, __FILE__, __LINE__);
 Status = MD_SUCCESS;
 pwwDev = padapter->pww_info;


 //Check EOS Pending Q is not Empty

 if (pwwDev->pww_eos_msgq == NULL) {
    MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_send_multi_eos_mf SHEDULED, But Pend Q is Empty\n");
    Status = MD_FAILURE;
    return(Status);
  }

 szInBoundMfa = pwwDev->pww_param->sizeofMFAInboundQ;
 //Calculate the Number of Entries
 maxNumEntries = (szInBoundMfa)/(sizeof(merc_uint_t));
 //Total number of streamId entries are less 2
 maxNumEntries -= 2;
 MSD_LEVEL2_DBGPRINT("Max Number of EOS Entries: %d\n", maxNumEntries);

 
 Msg = strm_ww_get_eos_Q(pwwDev);

 while(Msg)
  {
    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
    Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
  
    StreamBlock = padapter->pstream_connection_list[Ptr->StreamId];
    if (StreamBlock->SendStreamQueue != NULL)
     {
        if (eos_not_ready_head == NULL)
          {
            eos_not_ready_head = eos_not_ready = Msg;
          }
        else
          {
            eos_not_ready = eos_not_ready_head;
            while(eos_not_ready->b_next)
                  eos_not_ready = eos_not_ready->b_next;
             eos_not_ready->b_next = Msg;
          }
        Msg = strm_ww_get_eos_Q(pwwDev);
        needTimer = 1;
        continue;
    } /* SendStreamQueue */

    if (newMFRequired) 
    {
      mfAddressST.pwwDevi = pwwDev;
      mfAddressST.pmfAddress = NULL;
      Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST);

      if (Status != MD_SUCCESS)
      {
       MSD_LEVEL2_DBGPRINT("msgutl_ww_build_and_send_multi_eos_mf Failed \n");
       Status = MD_FAILURE;
       //Put the Msg back on pww_eos_msgq and start the timer
       strm_ww_put_eos_Q(pwwDev, Msg);
       MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
       pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT);
       MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
       return(Status);
      }
     else
      {
        mfShipped = 0;
        mfReady = 1;
        //Store the returned MF address and index
        pmfHead = pmfAddress = mfAddressST.pmfAddress;
        mfindex   = mfAddressST.mfIndex;
        //Adjust the pmfAddress leaving the first 8BYTES
        pmfAddress += (2 * sizeof(merc_uint_t));
      }
    }
    newMFRequired = 0;


    //Fill the MF
    *((pmerc_uint_t)(pmfAddress)) = Ptr->StreamId; 
    pmfAddress += (sizeof(merc_uint_t));
    numEntries++;
    
    if (numEntries == maxNumEntries)
     {
       //Ship the MF
       *((pmerc_uint_t)(pmfHead)) = ((MERCD_WW_MULTIPLE_STREAM_EOS_TO_BOARD_MSG) |
                                    ((maxNumEntries+2)*(sizeof(merc_uint_t))));
       *((pmerc_uint_t)(pmfHead + sizeof(merc_uint_t))) = maxNumEntries;
      MSD_LEVEL2_DBGPRINT("Shipping the full MF..\n");
      MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
      MsdWWPutinBoundQReg(padapter, mfindex);
      MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

      //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);

      newMFRequired = 1;
      numEntries = 0;
      mfShipped = 1;
      mfReady = 0;
     }

#ifdef ENABLE_LATER
    //Trace Code (EOS Msg): Copy the Message to the buffer (if required)
    if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
      supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
    }
#endif

    MSD_FREE_MESSAGE(Msg);
    Msg = strm_ww_get_eos_Q(pwwDev);
  } /* While for Msg */

  if ((!mfShipped) && (mfReady)) {
    //We have a partial MF, this needs to be shipped
    //Ship the MF
    *((pmerc_uint_t)(pmfHead)) = ((MERCD_WW_MULTIPLE_STREAM_EOS_TO_BOARD_MSG) |
                                  ((numEntries+2)*(sizeof(merc_uint_t))));
    *((pmerc_uint_t)(pmfHead + sizeof(merc_uint_t))) = numEntries;
    MSD_LEVEL2_DBGPRINT("Shipping the partial MF..\n");
    MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
    MsdWWPutinBoundQReg(padapter, mfindex);
    MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

    //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
    //atomic_inc(&pwwDev->pww_counters->write_strm_multieos_mfs_sent);
  }

   //Put the left out Msgs on pwwDev->pww_eos_msgq
   pwwDev->pww_eos_msgq = eos_not_ready_head; 

   if (needTimer) {
     MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
     pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT);
     MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
   }

   //We are done sending a EOS MF
 return(Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_build_and_send_eos_mf
 * Function Type                : WW Send Data Support function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW Driver snd_ww_strm_data2adapter Routine.
 *                                This routine sends data to adapter.
 * Additional comments          :
 ****************************************************************************/
int msgutl_ww_build_and_send_eos_mf(pmercd_ww_build_eos_sT peosMfSt)
{

 merc_uint_t streamId;
 pmerc_uchar_t pmfAddress;
 MD_STATUS Status;

 Status = MD_SUCCESS;

 streamId = peosMfSt->id;
 pmfAddress = peosMfSt->pmfAddress;

 *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_EOS_MSG_TO_BOARD;
 pmfAddress += (sizeof(merc_uint_t));
 *((pmerc_uint_t)(pmfAddress)) = streamId;

 return(Status);
}

/***************************************************************************
 * Function Name           : msgutl_ww_build_and_send_recv_stream_datablk_mf
 * Function Type           : Support Function for WW Msg Manager
 * Inputs                  :
 * Outputs                 :
 * Calling functions       :
 * Description             : This builds and sends a Rcv Stream DATABLOCK MF
 *
 * Additional comments     :
 *
 ****************************************************************************/
merc_uint_t
msgutl_ww_build_and_send_recv_stream_datablk_mf(    pmercd_ww_dev_info_sT pwwDev,
                                         pmercd_stream_connection_sT  StreamBlock)
{
 MD_STATUS                               Status;
 merc_ushort_t                            index;
 merc_ulong_t                           mfindex;
 mercd_ww_get_mfAddress_sT          mfAddressST;
 pmerc_uchar_t                       pmfAddress;
 pmerc_uchar_t                          pmfHead;
 merc_ulong_t                numDataBlocksPerMF;
 merc_ulong_t         numofDataBlocksPosted = 0;
#ifdef LFS
 merc_ulong_t                    mfFilledInSize = 0;
#else
 merc_ulong_t                    mfFilledInSize;
#endif
 merc_ulong_t                       msgType = 0;
 pmercd_adapter_block_sT               padapter;
#ifndef MERCD_LINUX
 PSTRM_MSG                                  Msg;
#endif
 mercd_ww_dmaDscr_Index_sT            dmaDscrSt;
 pmercd_ww_dma_descr_sT               pdmadescr;
 pmerc_uchar_t                    pmfTotalBytes;
 mercd_ww_dmaDscr_Index_sT      dmaDescSt = {0};

#ifndef MERCD_LINUX
 merc_ulong_t                        syncOffset;
 merc_ulong_t                          syncSize;
#endif

 MSD_FUNCTION_TRACE("msgutl_ww_build_and_send_recv_stream_datablk_mf 0x%x 0x%x %s #%d\n", 
                     (size_t)pwwDev, StreamBlock, __FILE__, __LINE__); 

 Status = MD_SUCCESS;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;


 ////////////////////////////////////////////////////////////////////
 //Whether it is B/G Stream, We Post One Datablock per MF.
 //For BStream we post 2 Descriptors(original and its next)  per MF.
 //For GStream we post 1 Descriptor per MF. There is no Next
 ////////////////////////////////////////////////////////////////////
 numDataBlocksPerMF = 1;

 //Other misc structures
 dmaDscrSt.pBitMap = StreamBlock->pRcvBitMapValue;
 dmaDscrSt.szBitMap  = StreamBlock->szRcvBitMap;

 for (index=0; index < StreamBlock->szRcvBitMap; index++)
  {
    dmaDscrSt.index = index;

    /////////////////////////////////////////////////////////////
    //Validate the descriptor at index
    /////////////////////////////////////////////////////////////
    MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
    Status = msgutl_ww_check_if_dmadescr_used(&dmaDscrSt);
    if (Status == MD_SUCCESS)
        {
           MSD_LEVEL2_DBGPRINT("build_and_send_recv_stream_datablk_mf: descr@index: %d\n", index);
           MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
           continue;
        }
    pdmadescr = StreamBlock->pRcvStrmDmaDesc[index];
    if (!(pdmadescr->flag & MERCD_WW_DMA_DESCR_VBIT))
     {
         MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
         MSD_LEVEL2_DBGPRINT("build_and_send_recv_stream_datablk_mf: descr@index:\
                 %d is vbit no set\n", index);
        continue;
     }

    //Do the double checking:
    if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
      {
        if (pdmadescr->next)
          {
            MSD_LEVEL2_DBGPRINT("recv_stream_datablk_mf: Rcv GSTREAM has only one Descriptor:\
                 INVESTIGATE\n");
            MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
            return(MD_FAILURE);
           }
      }
    else
      {
       if(pdmadescr->next== NULL)
          {
            MSD_LEVEL2_DBGPRINT("recv_stream_datablk_mf: Rcv BSTREAM has two Descriptor:\
                 INVESTIGATE\n");
            MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
            return(MD_FAILURE);
          }
      }

    //Other Checks for Descriptors:
    if (!(pdmadescr->board_address) || (!pdmadescr->host_address))
      {
        MSD_LEVEL2_DBGPRINT("Rcv Data Descriptor Validated, But\n");
        MSD_LEVEL2_DBGPRINT("recv_stream_datablk_mf: board_address or\
                host_address NULL: INVESTIGATE\n");
        MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
        continue;
      }

    ///////////////////////////////////////////////////////////////////
    //No need to check this for Rcv StreamBlock.This limit applies
    //to only on the write side.
    ///////////////////////////////////////////////////////////////////
#if 0
    //Check per stream allowd number of MFs
    //MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
    if ((StreamBlock->activeWWMfs >= StreamBlock->maxWWMfs))
     {
      //Anyway we will start the timer
      //MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
      MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
      goto out;
     }
    //MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
#endif
    ///////////////////////////////////////////////////////////////////

    //Now mark this as used
    Status = msgutl_ww_mark_dmadescr_used(&dmaDscrSt);
    MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);

    ////////////////////////////////////////////////////////////////
    //Get a MFA
    ////////////////////////////////////////////////////////////////
    mfAddressST.pwwDevi = pwwDev;
    mfAddressST.pmfAddress = NULL;

    Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST );

    if (Status != MD_SUCCESS)
     {
       MSD_LEVEL2_DBGPRINT("%s #%d\n", __FILE__, __LINE__);
       //Mark the BitMap bit free
       MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
       Status = msgutl_ww_mark_dmadescr_free(&dmaDscrSt);
       MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
       goto out;
     }
    else
      {
        MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
        StreamBlock->activeWWMfs++;
        MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
      }

    //Store the returned MF address and index
    pmfAddress = mfAddressST.pmfAddress;
    mfindex   = mfAddressST.mfIndex;

    //Leave the same in pmfHead for pointer arith manipulation
    pmfHead = pmfAddress;

    //Store the MF Type in the MF
    *((pmerc_uint_t)(pmfAddress)) = MERCD_WW_READ_REQUEST;
    pmfAddress += (sizeof(merc_uint_t));

    //Stores StreamId
    *((pmerc_uint_t)(pmfAddress)) = StreamBlock->id;
    pmfAddress += (sizeof(merc_uint_t));
   
    //Store the User Context
    // Null contexts sent as 0xFF - need to decode in ack
    if (!index) {
	*((pmerc_uint_t)(pmfAddress)) = 0xFF;
    } else {
        *((pmerc_uint_t)(pmfAddress)) = index;
    }
    pmfAddress += (2 * sizeof(merc_uint_t));

   //Store the Stream Specific Flags
   *((pmerc_uint_t)(pmfAddress)) = StreamBlock->flags;
   pmfAddress += (2 * sizeof(merc_uint_t));

   //Store the Sequence Number
    *((pmerc_uint_t)(pmfAddress)) = StreamBlock->stats.ww_rcv_dblk_sequence_num;
   pmfAddress += (sizeof(merc_uint_t));

   //Bump the rcv_dblk_sequence_num - FW will do sequencing
   //StreamBlock->stats.ww_rcv_dblk_sequence_num++;
   
   //Fill the Total Bytes Later
   pmfTotalBytes = pmfAddress;
   *((pmerc_uint_t)(pmfTotalBytes)) = 0;

   //Jump to fill the Physical Address
   pmfAddress += (sizeof(merc_uint_t));
#ifdef LFS
   *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)pdmadescr->board_address; 
#else
   *((pmerc_ulong_t)(pmfAddress)) = pdmadescr->board_address; 
#endif

   //Fill in the size for this DMA Descriptor
   pmfAddress += (2 * sizeof(merc_uint_t)); 
   *((pmerc_uint_t)(pmfAddress)) = pdmadescr->size;

   //Update the Total Bytes Field
   *((pmerc_uint_t)(pmfTotalBytes)) += pdmadescr->size;

   numofDataBlocksPosted++;

   if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
    {
     //There is no pdmadescr->next for GSTREAM
     //This MF can be shipped
     mfFilledInSize = 
           ((2*(sizeof(merc_uint_t))) +   /* Type + StreamId*/
            (2*(sizeof(merc_uint_t))) +   /* User Context */
            (2*(sizeof(merc_uint_t))) +   /* StreamFlags */
            (sizeof(merc_uint_t))     +   /* Sequence Num */
            (sizeof(merc_uint_t))     +   /*Total Bytes Xfred */
            (numofDataBlocksPosted * (4*(sizeof(merc_uint_t))))); /* PysicalAddr+Size */
    }
   else
    {
     if (pdmadescr->next)
      {
        //Fill the second descriptor for actual data
    
        //Physical Address
        pmfAddress += (2 * sizeof(merc_uint_t));
#ifdef LFS
        *((pmerc_ulong_t)(pmfAddress)) = (merc_ulong_t)pdmadescr->next->board_address;
#else
        *((pmerc_ulong_t)(pmfAddress)) = pdmadescr->next->board_address;
#endif

        //Size
        pmfAddress += (2 * sizeof(merc_uint_t));
        *((pmerc_uint_t)(pmfAddress)) = pdmadescr->next->size;

       //Update the Total Bytes Field
       *((pmerc_uint_t)(pmfTotalBytes)) += pdmadescr->next->size;

       numofDataBlocksPosted++;

       mfFilledInSize = 
           ((2*(sizeof(merc_uint_t))) +   /* Type + StreamId*/
            (2*(sizeof(merc_uint_t))) +   /* User Context */
            (2*(sizeof(merc_uint_t))) +   /* StreamFlags */
            (sizeof(merc_uint_t))     +   /* Sequence Num */
            (sizeof(merc_uint_t))     +   /*Total Bytes Xfred */
            (numofDataBlocksPosted * (4*(sizeof(merc_uint_t))))); /* PysicalAddr+Size */

       //This MF can be shipped
      }
    } 


     //MF is now Full. Time to Send down. But need to fill in the Type and
     //the numofCancelsPosted in the MF
      msgType = *((pmerc_uint_t)(pmfHead));
      msgType = (msgType | mfFilledInSize);
      *((pmerc_uint_t)(pmfHead)) = msgType;

      //Post the MF to the inBoundQ

#ifndef MERCD_LINUX
      syncOffset = ((pwwDev->inBoundQMfAddress) +
                  ((mfindex) * (pwwDev->pww_param->sizeofMFAInboundQ)));
      syncSize = pwwDev->pww_param->sizeofMFAInboundQ;
      MsdWWSyncDevice(pwwDev);
#endif
      MSD_LEVEL2_DBGPRINT("Posted StremId: %d descr index: %d\n", StreamBlock->id, index);
      //i_bprintf(pmfHead, 32);
      MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
      MsdWWPutinBoundQReg(padapter, mfindex);
      MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

      //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
      //atomic_inc(&pwwDev->pww_counters->readstr_databuf_post_mfs_sent);

 }/* For */

 return (Status);

out:
    //Set the pending bit for timer
    //Either start_streams or generic_timeout would have
    //have already acquried ww_msgpendq_mutex. So no need
    //to acquire it.
    //MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
    dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
    dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
    dmaDescSt.index = StreamBlock->id;
    Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
    pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
    StreamBlock->WWRcvStrDataBufPostPending = 1;

    Status = MD_FAILURE;
    return(Status);
}
