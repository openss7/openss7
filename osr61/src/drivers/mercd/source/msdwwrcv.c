/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwrcv.c
 * Description                  : WW FRom Board Message functions 
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDWWRCV_C_
#include "msdextern.h"
#undef _MSDWWRCV_C_

merc_ulong_t vRcvFreeDesc = 0;
merc_ulong_t vSndFreeDesc = 0;

static merc_uint_t  msdwwrcvcount[2] = {0,0};

/***************************************************************************
 * Function Name                : msdwwrcv_ww_msgready_intr_processing
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                This routine Builds a small MF
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t
msdwwrcv_ww_msgready_intr_processing(pmercd_ww_dev_info_sT pwwDev)
{
  MD_STATUS                       Status;
  merc_ulong_t                   mfindex =0;
  mercd_ww_get_mfAddress_sT  mfAddressST;
  pmerc_uchar_t               pmfAddress;
  pmercd_adapter_block_sT       padapter;
  merc_uint_t                    msgType;
#ifndef LFS
  merc_uint_t                    msgType1;
#endif
  merc_uint_t                    found = 0;


  MSD_FUNCTION_TRACE("msdwwrcv_ww_msgready_intr_processing 0x%x %s #%d\n",
                      (size_t)pwwDev, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfAddressST.pwwDevi = pwwDev;

 
    while (Status == MD_SUCCESS) {

         if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
	     (!(pwwDev->mfaBaseAddressDescr.host_address)))
          {
                return(MD_SUCCESS);
          }

          mfAddressST.pmfAddress = NULL;
          Status = msdwwutl_ww_read_posted_outboundQ_for_mf_address(&mfAddressST);


          //Store the returned MF address and index
          pmfAddress = mfAddressST.pmfAddress;
          mfindex   = mfAddressST.mfIndex;


          if ((Status) == (MD_SUCCESS))
          {
            msgType = *((merc_uint_t *)(pmfAddress));

            found = 1;

            switch (msgType)
            {
               case MERCD_WW_SMALL_MSG_FROM_BOARD                 :
                    //atomic_inc(&padapter->pww_info->pww_counters->small_msgs_recd);
                    //printk("MERCD_WW_SMALL_MSG_FROM_BOARD...\n");
                    rcv_ww_process_small_msgs_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_BIG_MSG_FROM_BOARD                   :
                    //atomic_inc(&padapter->pww_info->pww_counters->big_msgs_recd);
                    //printk("MERCD_WW_BIG_MSG_FROM_BOARD...\n");
                    rcv_ww_process_big_msgs_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_BIG_MSG_READ_ACK_FROM_BOARD           :
                    //atomic_inc(&padapter->pww_info->pww_counters->bigmsg_read_acks_recd);
                    //printk("MERCD_WW_BIG_MSG_READ_ACK_FROM_BOARD...\n");
                    rcv_ww_process_big_msg_read_ack_msg_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_BIG_XMSG_READ_ACK_FROM_BOARD           :
                    //atomic_inc(&padapter->pww_info->pww_counters->bigmsg_read_acks_recd);
                    //printk("MERCD_WW_BIG_XMSG_READ_ACK_FROM_BOARD...\n");
                    rcv_ww_process_big_xmsg_read_ack_msg_from_adapter(&mfAddressST);
                    break;
               case MERCD_WW_BIG_MSG_CANCELLATION_ACK_FROM_BOARD   :
                    //atomic_inc(&padapter->pww_info->pww_counters->bigmsg_cancel_acks_recd);
                    //printk("MERCD_WW_BIG_MSG_CANCELLATION_ACK_FROM_BOARD...\n");
                    rcv_ww_process_big_msg_cancel_ack_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_WRITE_REQUEST_COMPLETION              :
                    //atomic_inc(&padapter->pww_info->pww_counters->writestr_complete_recd);
                    //printk("MERCD_WW_WRITE_REQUEST_COMPLETION...\n");
                    rcv_ww_process_write_request_complete_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_READ_REQUEST_COMPLETION                :
                    //atomic_inc(&padapter->pww_info->pww_counters->readstr_complete_recd);
                    //printk("MERCD_WW_READ_REQUEST_COMPLETION...\n");
                    rcv_ww_process_read_request_complete_from_adapter(&mfAddressST);
                     break;

               case MERCD_WW_DATA_BUFFER_REQUEST_FROM_BOARD         :
                    //atomic_inc(&padapter->pww_info->pww_counters->readstr_databufreq_recd);
                    //printk("MERCD_WW_DATA_BUFFER_REQUEST_FROM_BOARD...\n");
                    rcv_ww_process_data_buffer_request_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_STREAM_FLUSH_REQUEST_ACK_FROM_BOARD    :
                    //atomic_inc(&padapter->pww_info->pww_counters->strm_flush_req_ack_recd);
                    //printk("MERCD_WW_STREAM_FLUSH_REQUEST_ACK_FROM_BOARD...\n");
                    rcv_ww_process_stream_flush_request_ack_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_STREAM_REQ_CANCEL_ACK_FROM_BOARD       :
                    //atomic_inc(&padapter->pww_info->pww_counters->strm_cancel_req_ack_recd);
                    //printk("MERCD_WW_STREAM_REQ_CANCEL_ACK_FROM_BOARD...\n");
                    rcv_ww_process_stream_reqeust_cancel_ack_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_EOS_READ_STREAM_FROM_BOARD             :
                    //atomic_inc(&padapter->pww_info->pww_counters->readstr_eos_recd);
                    //printk("MERCD_WW_EOS_READ_STREAM_FROM_BOARD...\n");
                    rcv_ww_process_eos_read_stream_from_adapter(&mfAddressST);
                    break;

               case MERCD_WW_BIG_MSG_FROM_BOARD_INMF_START          :
                    //atomic_inc(&padapter->pww_info->pww_counters->bigmsg_inmf_start_recd);
                    //printk("MERCD_WW_BIG_MSG_FROM_BOARD_INMF_START...\n");
                    rcv_ww_process_big_msg_in_mfs(&mfAddressST);
                    break;

		case MERCD_WW_SESS_CLOSE			    :
		    //printk("WW: Got MERCD_WW_SESS_CLOSE\n");
		    rcv_ww_process_stream_session_close(&mfAddressST);
		    break;

               default:
		    //printk("WW: DEFAULT CASE %#x\n", msgType);
                    //atomic_inc(&padapter->pww_info->pww_counters->msg_type_rcvd_corrupted);
                    break;

            }
            //Bzero the outbound Q memory before giving back the MF
            MSD_ZERO_MEMORY(mfAddressST.pmfAddress, pwwDev->pww_param->sizeofMFAOutboundQ);
            //Write the MF back to outBoundQ
            MSD_ENTER_MUTEX(&pwwDev->ww_oboundQ_mutex);
            MsdWWPutoutBoundQReg(padapter, mfindex);
            MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);

            //atomic_inc(&pwwDev->pww_counters->total_outboundq_writes);
          } /* Status == MD_SUCCESS */
     }



     if (!found)
        {
           //atomic_inc(&padapter->pww_info->pww_counters->valid_msgrdy_intr_but_no_mf);
           //MSD_LEVEL2_DBGPRINT("msdwwrcv_ww_msgready_intr_processing:
           printk("msdwwrcv_ww_msgready_intr_processing:\
                                Interrupt But no MFs in FIFO\n");
        }
#ifdef LFS
     return (0);
#endif
}
 
/***************************************************************************
 * Function Name                : rcv_ww_process_small_msgs_from_adapter
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This receives small MF
 * Additional comments          :
 ****************************************************************************/
merc_uint_t
rcv_ww_process_small_msgs_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                         Status;
  merc_ulong_t                     mfindex;
  pmerc_uchar_t                 pmfAddress;
  pmerc_uchar_t                 pQMesgHeader;
  pmercd_adapter_block_sT         padapter;
  pmercd_ww_dev_info_sT             pwwDev;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                    BindHandle;
#else
#ifndef LFS
  MBD_HANDLE                    BindHandle;
#endif
#endif
  merc_uint_t                  MsgBodySize;
  PSTRM_MSG                            Msg;
  PSTRM_MSG                        DataMsg;
  PMDRV_MSG                          MdMsg;
  PMERC_HOSTIF_MSG                 MercMsg;
  PMERC_HOSTIF_MSG               FwMessage;
#ifndef LFS
  merc_uchar_t                       Class;
  merc_uint_t                    found = 0;
#endif
  mercd_ww_dispatch_rcvd_msg_sT  dispMsg = {0};

#ifndef MERCD_LINUX
  merc_ulong_t                  syncOffset;
  merc_ulong_t                    syncSize;
#endif


  MSD_FUNCTION_TRACE("rcv_ww_process_small_msgs_from_adapter 0x%x %s #%d", 
                      (size_t)pmfAddrSt, 
                      __FILE__, __LINE__
                    );

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfAddress = pmfAddrSt->pmfAddress;


  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
    {
         return(MD_FAILURE);
    }


  //Get a Pointer to the QMesg Header in the receive frame
  pQMesgHeader = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

  MercMsg = (PMERC_HOSTIF_MSG)pQMesgHeader;

  if ((MERCURY_GET_TRANSACTION_ID(MercMsg) == 0x800000) && !padapter->rtkMode)
      padapter->rcvflowcount++;

  if (MercMsg->MessageType == 0x80004f ) {
      msdwwrcvcount[padapter->rtkMode]++;
      if (msdwwrcvcount[padapter->rtkMode] && !(msdwwrcvcount[padapter->rtkMode]%50 )) {
          MSD_LEVEL2_DBGPRINT("Undelived messages received for 8Bit %d 16Bit\n",
                 msdwwrcvcount[0], msdwwrcvcount[1]);
          return(Status);
      }
  }

  //Get the Body Size. The same can also be obtained by
  //the size field of the receive MF. But let us take
  //the old approach.
  MsgBodySize = (MERCURY_GET_BODY_SIZE(MercMsg));

  if (MsgBodySize > 2048) {
      printk("msdwwrcv: MsgBodySize %d > 2048\n", MsgBodySize);
      return (MD_FAILURE);
  }

  //Now Alloc an mblk

  Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ, GFP_ATOMIC);


  if(Msg == NULL) {
           printk("rcv_ww_small_msgs_from_adapter: Cannot alloc Msg buffer\n");
           MSD_LEVEL2_DBGPRINT(
                    "Cannot alloc Msg size: 0x%x %s #%d\n",
                    sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,
                    __FILE__, __LINE__
                    );
           Status = MD_FAILURE;
           return(Status);
  }


  DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+MsgBodySize, GFP_ATOMIC);
  if(DataMsg == NULL) {
#ifdef LFS
	  /* I which GNU printf checks would recognize the %z option. */
           printk("rcv_ww_small_msgs_from_adapter: Cannot alloc Data buffer %zd\n", sizeof(MERC_HOSTIF_MSG)+MsgBodySize);
#else
           printk("rcv_ww_small_msgs_from_adapter: Cannot alloc Data buffer %d\n", sizeof(MERC_HOSTIF_MSG)+MsgBodySize);
#endif
           MSD_FREE_MESSAGE(Msg);
           Status = MD_FAILURE;
           return(Status);
   }

  MD_SET_MSG_TYPE(DataMsg, M_DATA);
  MSD_LINK_MESSAGE(Msg,DataMsg);


  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  FwMessage = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

#ifndef MERCD_LINUX
  //Sync
  syncOffset = ((pwwDev->outBoundQMfAddress) +
               (mfindex * pwwDev->pww_param->sizeofMFAOutboundQ));
  syncSize   = pwwDev->pww_param->sizeofMFAOutboundQ;
  MsdWWSyncCpu(pwwDev);
#endif

  //Copy the MF from outBound MF pool to newly allocated bufs
  MsdCopyMemory(pQMesgHeader, (pmerc_uchar_t)FwMessage, sizeof(MERC_HOSTIF_MSG)+MsgBodySize);

  //Trace Code (SMALL Msg): Copy the Message to the buffer (if required)
  if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
     supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)FwMessage);
  }

  dispMsg.padapter = (pmerc_void_t)padapter;
  dispMsg.mb = (pmerc_void_t)Msg;
  dispMsg.fwmsg = (pmerc_void_t)FwMessage;

  Status = rcv_ww_dispatch_mblk_train_up(&dispMsg);
  
  return(Status);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_big_msg_read_ack_msg_from_adapter
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This receives small MF
 * Additional comments          :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_big_msg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                     pUserContext;
  merc_uint_t                        numContexts;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                              count;
  merc_ushort_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  mercd_dhal_ww_free_desc_sT          freeDescSt;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                          bindHandle = 0;
#else
  MBD_HANDLE                          bindHandle = 0;
#endif
 pmerc_uchar_t                     	pbitmap;
 pmercd_bind_block_sT 			BindBlock;



  MSD_FUNCTION_TRACE("rcv_ww_process_big_msg_read_ack_msg_from_adapter 0x%x %s #%d\n", 
                      (size_t)pmfAddrSt,
                      __FILE__, __LINE__
                     );

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
    {
         return(MD_SUCCESS);
    }


  //Get a Pointer to the user context following the Header
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

  //Get the number of user contexts
  pmfAddress += (sizeof(merc_uint_t));
  numContexts = *((pmerc_uint_t)(pmfAddress));


  //For each index at each user context, free the Msg block that
  //has been just DMAd and mark the DMA descriptor as free

  for ( count = 0; count < numContexts; count ++)
  {

    if (padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS)
      {
            return(MD_SUCCESS);
      }

#if 1
     dmaDescIndex = *((pmerc_ushort_t)(pUserContext));
     // Kernel Workaround: Null contexts were sent as 0xFF - need to decode
     if (dmaDescIndex == 0xFF) {
	 dmaDescIndex = 0;
     }
#endif

#if 0
     dmaDescIndex = (merc_ushort_t)(*((pmerc_uint_t)(pUserContext)));
#endif

     //Check if this index is being marked as used in the
     //bitmap. This is an extreme case, but should never
     //be marked as freed.
     dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
     dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
     dmaDescSt.index = dmaDescIndex;
     MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
     Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
     if (Status == MD_FAILURE) {
        //Dump the BigMsgSndMemStr.pBitMapValue;
        //i_bprintf(pwwDev->BigMsgSndMemStr.pBitMapValue, pwwDev->BigMsgSndMemStr.szBitMap);
        MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
        MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
        padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
        printk("rcv_ww_process_big_msg_read_ack: Adapter out. Desc not marked\n");
        MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
        return(MD_FAILURE);
     }

     bindHandle = *((MBD_HANDLE *)((pmerc_uchar_t)pUserContext+(sizeof(merc_ushort_t))));

     if (!bindHandle) {
         MSD_LEVEL2_DBGPRINT("rcv_ww_process_big_msg_read_ack: 0 BindHandle for %d\n", dmaDescIndex);
         //i_bprintf(pUserContext, 8);
         MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
         continue;
     }

     //We leave the first descritpor at dmaDescIndex intact,
     //and mark its corresponding bit in bitmap free.
     //But we free the DMA descriptors following the next link
     //of the first descritptor at dmaDescIndex.
     //And finally free the Streams Msg block or DMAble memory
     // that we just DMAd to Board.

     freeDescSt.pwwDevi = pwwDev;
     freeDescSt.index = dmaDescIndex;
     freeDescSt.StreamBlock = 0;
     freeDescSt.purpose = 0;

     (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC])((pmerc_void_t)&freeDescSt);
     if (freeDescSt.ret == MD_FAILURE)
     {
       printk(" rcv_ww_process_big_msg_read_\
               ack_msg_from_adapter FAILED dmaDescIndex: %d\n", dmaDescIndex);
       MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
       return(MD_SUCCESS);
     }
     //Now we are left with the only descriptor at dmaDescIndex. Mark
     //this as free in the bitmap.
     dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
     dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
     dmaDescSt.index = dmaDescIndex;

     Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
     ++pwwDev->BigMsgSndMemStr.free_counter;
     vSndFreeDesc = pwwDev->BigMsgSndMemStr.free_counter;

   //Mark the Global Message Pending BitMap if the BindBlock
   //is still alive
   if (bindHandle)
    {
      pbitmap = pwwDev->pPendMsgBitMap[bindHandle];
      BindBlock = MsdControlBlock->pbind_block_list[bindHandle];
      if (BindBlock)
       {
        dmaDescSt.pBitMap = pbitmap;
        dmaDescSt.szBitMap = pwwDev->szPendMsgBitMap;
        dmaDescSt.index = dmaDescIndex;
        MSD_LEVEL2_DBGPRINT("BiG Msg Read Ack: bindHandle: %d before free\n", bindHandle);
        //i_bprintf(pbitmap, 8);
        Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
        MSD_LEVEL2_DBGPRINT("BiG Msg Read Ack: After Free..\n");
        //i_bprintf(pbitmap, 8);
        BindBlock->WWPendReadAckForToBoardBigMsgs--;
      }
   } /*bindHandle*/
  else
   {
     MSD_LEVEL2_DBGPRINT("BigMsg Read Ack: bindHandle Should Never be NULL: INVESTIGATE\n");
   }

     MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
     pUserContext += (2*(sizeof(merc_uint_t)));
  }
 return(MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_big_xmsg_read_ack_msg_from_adapter
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This receives big Xmsg MF
 * Additional comments          :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_big_xmsg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                     pUserContext;
  merc_uint_t                        numContexts;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
#ifndef LFS
  merc_uint_t                              count;
#endif
  merc_ushort_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  mercd_dhal_ww_free_desc_sT          freeDescSt;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                          bindHandle = 0;
#else
#ifndef LFS
  MBD_HANDLE                          bindHandle = 0;
#endif
#endif
#ifndef LFS
 pmerc_uchar_t                     	pbitmap;
 pmercd_bind_block_sT 			BindBlock;
#endif


  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address))) {
      return(MD_SUCCESS);
  }

  //Get a Pointer to the user context following the Header
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

  //Get the number of user contexts
  pmfAddress += (sizeof(merc_uint_t));
  numContexts = *((pmerc_uint_t)(pmfAddress));

  //free the Msg block that has been just DMAd and mark the DMA descriptor as free
  dmaDescIndex = *((pmerc_ushort_t)(pUserContext));
 
  // Kernel Workaround: Null contexts were sent as 0xFF - need to decode
  if (dmaDescIndex == 0xFF) {
      dmaDescIndex = 0;
  }

  //Check if this index is being marked as used in the
  //bitmap. This is an extreme case, but should never
  //be marked as freed.
  dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
  dmaDescSt.index = dmaDescIndex;
  MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
  Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
  if (Status == MD_FAILURE) {
      //Dump the BigMsgSndMemStr.pBitMapValue;
      //i_bprintf(pwwDev->BigMsgSndMemStr.pBitMapValue, pwwDev->BigMsgSndMemStr.szBitMap);
      MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
      MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
      padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
      printk("rcv_ww_process_big_xmsg_read_ack: Adapter out. Desc not marked\n");
      MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
      return(MD_FAILURE);
  }

  //We leave the first descritpor at dmaDescIndex intact,
  //and mark its corresponding bit in bitmap free.
  //But we free the DMA descriptors following the next link
  //of the first descritptor at dmaDescIndex.
  //And finally free the Streams Msg block or DMAble memory
  // that we just DMAd to Board.

  freeDescSt.pwwDevi = pwwDev;
  freeDescSt.index = dmaDescIndex;
  freeDescSt.StreamBlock = 0;
  freeDescSt.purpose = 0;

  (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC])((pmerc_void_t)&freeDescSt);
  if (freeDescSt.ret == MD_FAILURE) {
      printk(" rcv_ww_process_big_xmsg_read_ack_msg_from_adapter FAILED dmaDescIndex: %d\n", dmaDescIndex);
      MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
      return(MD_SUCCESS);
  }

  //Now we are left with the only descriptor at dmaDescIndex. Mark
  //this as free in the bitmap.
  dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
  dmaDescSt.index = dmaDescIndex;

  Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
  ++pwwDev->BigMsgSndMemStr.free_counter;
  vSndFreeDesc = pwwDev->BigMsgSndMemStr.free_counter;

  MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);

  return(MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_big_msgs_from_adapter
 * Function Type                : Host FW Receive function for BIG Messages
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  We go a BIG Msg from Adapter
 * Additional comments          :
 *                                Each MF from FW correspond to one BIG msg. If the
 *                                size of the BIG msg from FW exceeds, the allocated
 *                                size of a BIG Msg Block, then the MF contains multiple
 *                                user contexts. That means, the data DMAd to the
 *                                descritpors at the respective indexes consitute
 *                                one BIG Msg from FW.
 *                                Ex: If we allocated 'numberBigMsgBlocks' number of
 *                                of BIG msg Blks on receive side, each of size
 *                                'sizeofBigMsgBlocks' (refer pww_param), then if the
 *                                amount of Data that is bein DMAed by FW is <
 *                                'sizeofBigMsgBlock', then we will have just one
 *                                index as the user context in MF. But if it exceeds
 *                                'sizeofBigMsgBlock', then there are multiple indexes
 *                                at the user context fieds of the MF. But all these
 *                                indexes will constitute one BIG Msg that is being
 *                                DMAed from FW.
 *
 *                                The next BIG msg comes in a second MF from FW.
 **************************************************************************************/
merc_uint_t
rcv_ww_process_big_msgs_from_adapter (pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                     pUserContext;
  pmerc_uchar_t                      pRepContext;
  merc_uint_t                        numContexts;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_ulong_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
#ifndef LFS
  mercd_dhal_ww_free_desc_sT          freeDescSt;
  pmercd_ww_dma_descr_sT           pbigmsg;
#endif
  PSTRM_MSG                                  Msg;
  PSTRM_MSG                      linkedDataMblks;
  PMDRV_MSG                                MdMsg;
#ifndef LFS
  PMERC_HOSTIF_MSG                       MercMsg;
#endif
  PMERC_HOSTIF_MSG                     FwMessage;
#ifndef LFS
  merc_uchar_t                             Class;
#endif
  mercd_ww_prep_mblk_train_sT          mbTrainSt;
  mercd_dhal_ww_rcv_rep_bigmsgdes_sT  RepDescrSt;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                          BindHandle;
#else
#ifndef LFS
  MBD_HANDLE                          BindHandle;
#endif
#endif
  merc_uint_t                              count;
  merc_uint_t                         totMsgSize;
#ifndef LFS
  mercd_ww_mblk_train_info_sT          trainInfo;
  merc_ulong_t                             noMFs; /* Total required MFs */
  merc_ulong_t                        numOfDescr; /* Number of Descriptors in a MF */
  merc_ulong_t                       sizeofDescr; /* size of each descriptor */
  mercd_ww_build_init_msgblkMF_sT     msgblkMfSt;
  merc_ulong_t                      mfToBePosted;
#endif
  mercd_ww_dispatch_rcvd_msg_sT          dispMsg;

#ifndef MERCD_LINUX
  merc_ulong_t                        syncOffset;
  merc_ulong_t                          syncSize;
#endif

  MSD_FUNCTION_TRACE("rcv_ww_process_big_msgs_from_adapter 0x%x %s #%d\n",
                      (size_t)pmfAddrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
     (!(pwwDev->mfaBaseAddressDescr.host_address)))
    {
      return(MD_SUCCESS);
    }


#ifndef MERCD_LINUX
   //Sync the BIG MF
  syncOffset = ((pwwDev->outBoundQMfAddress) +
               (mfindex * pwwDev->pww_param->sizeofMFAOutboundQ));
  syncSize   = pwwDev->pww_param->sizeofMFAOutboundQ;
  MsdWWSyncCpu(pwwDev);
#endif

  //Get a Pointer to the user context following the Header
  pRepContext = pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

  //Get the number of blocks to which data was DMAed
  //i.e number of user contexts
  pmfAddress += (sizeof(merc_uint_t));
  totMsgSize =  *((pmerc_uint_t)(pmfAddress));
  numContexts = ((totMsgSize)/(pwwDev->pww_param->sizeofBigMsgBlocks));

  if ((totMsgSize)%(pwwDev->pww_param->sizeofBigMsgBlocks))
            numContexts += 1;


  //Now Alloc an mblk for driver header
  Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ, GFP_DMA);
  if(Msg == NULL) {
    printk("rcv_ww_process_big_msgs_from_adapter: Cannot alloc Msg\n");
    Status = MD_FAILURE;
    return(Status);
  }

  mbTrainSt.pwwDevi = pwwDev;
  mbTrainSt.pusrContext = pUserContext;
  mbTrainSt.dataMblks= NULL;
#ifdef LFS
  mbTrainSt.numContexts = &numContexts;
#else
  mbTrainSt.numContexts = numContexts;
#endif
  mbTrainSt.pmfHead = 0;
  rcv_ww_prepare_mblk_train_to_sendup(&mbTrainSt);

  if (Status != MD_SUCCESS) {
     printk("Could not link the Data Mblks\n");
     MSD_FREE_MESSAGE(Msg);
     return(Status);
  }

  linkedDataMblks = (PSTRM_MSG)mbTrainSt.dataMblks;

  MD_SET_MSG_TYPE(linkedDataMblks, M_DATA);
  MSD_LINK_MESSAGE(Msg,linkedDataMblks);
  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  FwMessage = (PMERC_HOSTIF_MSG)linkedDataMblks->b_rptr;

  dispMsg.padapter = (pmerc_void_t)padapter;
  dispMsg.mb = (pmerc_void_t)Msg;
  dispMsg.fwmsg = (pmerc_void_t)FwMessage;

  Status = rcv_ww_dispatch_mblk_train_up(&dispMsg);

  //Time to replenish the Rcv Big Msg DMA descriptors with
  //Fresh memory at respective indexes. Here again we first
  //try to alloc Streams to avoid copying in kernel. If this
  //does not work then we allocate the DMAble memory.


  RepDescrSt.pwwDevi = (pmerc_void_t)pwwDev;
  RepDescrSt.purpose = MERCD_WW_BIGMSG_RCV_DESCR_ALLOC;

  dmaDescSt.pBitMap = pwwDev->BigMsgRcvMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgRcvMemStr.szBitMap;

  for (count = 0; count < numContexts; count++)
  {
    if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
        (!(pwwDev->mfaBaseAddressDescr.host_address)))
     {
       return(MD_SUCCESS);
     }

    dmaDescIndex = *((pmerc_ushort_t)(pRepContext));
    //printk("Replenish dmaIndex: %d numContexts: %d\n", dmaDescIndex, numContexts);
    RepDescrSt.index =  dmaDescIndex;
    (*mercd_dhal_func[MERCD_DHAL_WW_REPLENISH_RCV_DESC])((pmerc_void_t)(&RepDescrSt));
    pRepContext += (2*(sizeof(merc_uint_t)));

    //Mark the corresponding bit in bitmap free
    dmaDescSt.index = dmaDescIndex;
    MSD_ENTER_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex)
    msgutl_ww_mark_dmadescr_free(&dmaDescSt);
    ++pwwDev->BigMsgRcvMemStr.free_counter;

    //The following for debug purposes only
    vRcvFreeDesc = pwwDev->BigMsgRcvMemStr.free_counter;
    MSD_EXIT_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex)
  }

 MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
 if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_BIT))
  {
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
    msgutl_ww_build_and_send_msgblk_mf(padapter->pww_info);
  }
 else
   MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

return (Status);

}

/************************************************************************************
 * Function Name                : rcv_ww_prepare_mblk_train_to_sendup
 * Function Type                : Host FW Receive Support function for
 *                              : BIG Messages
 *
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This prepares an mblk train from Streams
 *                              : mem/DMAble kmem before the big msg is
 *                              : shipped up to the libraries.
 * Additional comments          :
 *
 *                              |---->| 0 S |
 *        Descriptor table  --->|---->| 1 D |
 *                              |---->| 2 S |
 *                              |---->| 3 D |
 * NOTE: As shown consider a case of  4 descriptors
 *       in the descriptor table, where 0 and 2 have
 *       been allocated with stream mblks, 1 and 3 have
 *       been allocated the DMAble memory. When we get a
 *       Rcv Big Msg frame with all the above indexes in
 *       the user context, then we send the data up by
 *       copying data from 1 and 3 to two new mblks, and
 *       then linking existing mblk at 0, new mblk for 1,
 *       existing mblk at 2, new mblk at 3. Note, we need
 *       to only deallocate the memory for the descriptors
 *       1 and 3.
 *
 *       Hope this helps.
 *************************************************************************************/
merc_int_t
rcv_ww_prepare_mblk_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                            offSet;
  merc_ulong_t                           lenToCp;
  merc_ulong_t                       MsgBodySize;
  merc_uint_t                        numContexts;
  pmercd_ww_dma_descr_sT                 pbigmsg;
  PSTRM_MSG                              DataMsg;
  PSTRM_MSG                             nDataMsg;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                              count;
  merc_ulong_t                      dmaDescIndex;
  PMERC_HOSTIF_MSG                       MercMsg;
  pmerc_uchar_t                     pUserContext;
#ifndef LFS
  merc_ulong_t  len;
  merc_uint_t     i;
#endif
  pmercd_adapter_block_sT               padapter;

  MSD_FUNCTION_TRACE("rcv_ww_prepare_mblk_train_to_sendup 0x%x %s #%d\n",
                      (size_t)pmbTrainSt, __FILE__, __LINE__);

  pwwDev = pmbTrainSt->pwwDevi;
  pUserContext = pmbTrainSt->pusrContext;
#ifdef LFS
  numContexts = *pmbTrainSt->numContexts;
#else
  numContexts = pmbTrainSt->numContexts;
#endif
  padapter = pwwDev->padapter;
  

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
     (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
     return(MD_SUCCESS);
   }


  //Get the First index
  dmaDescIndex = *((pmerc_ulong_t)(pUserContext));

  //printk("prepare train at :dmaDescIndex : %d\n", dmaDescIndex);

    //Get the descritptor at dmaDescIndex
  pbigmsg= pwwDev->BigMsgRcvMemStr.pbigMsgDescr[dmaDescIndex];


  //Check for the v bit. We are little overdoing
  //in error checking. This checking may not be
  //needed. We turn-off vbit only after sending the
  //Msg portion of the descriptor up. But, if the
  //following happens, then it is serious problem.
  //Know why ? (1) Either we read a wrong
  //dmaDescIndex from a corrupted MF, or (2) we never
  //allocated the Streams Msg or DMAble Mem at
  //dmaDescIndex, but posted the index to FW. But (2)
  //can be ruled out, since we never post an index
  //when Streams Msg or DMAble Memory allocation fails.
  //That leaves the possibility of a corrupted MF.
  //So no harm in checking the vbit

  if (!(pbigmsg->flag & MERCD_WW_DMA_DESCR_VBIT))
  {
    printk("Got a Wrong DMA descriptor with vbit 0");
#ifdef LFS
    return (0);
#else
    return;
#endif
  }

  //Now look at the sbit and determine if the
  //memory at dmaDescIndex is Streams buf or
  //DMAble memory that was allocated with SCHEME2
  if (pbigmsg->flag & MERCD_WW_DMA_DESCR_SBIT)
   {
     DataMsg = (PSTRM_MSG)(pbigmsg->host_address);

     //The following is important to address some race and
     //edge conditions. Do not set pbigmsg->size and flag
     //to 0 here:
     pbigmsg->host_address = 0;
     pbigmsg->board_address = 0;

     //Get the total Big Msg Size
    MercMsg = (PMERC_HOSTIF_MSG)(DataMsg->b_rptr);
     MsgBodySize = (MERCURY_GET_BODY_SIZE(MercMsg));

     lenToCp = (MsgBodySize) + sizeof(MERC_HOSTIF_MSG);

     //printk("lenToCp: %d\n", lenToCp);

     //Calculate the offset
     if (lenToCp <= pbigmsg->size) {
       offSet = lenToCp;
       MD_SET_MSG_WRITE_PTR(DataMsg, MD_GET_MSG_READ_PTR(DataMsg) + offSet);
     } else {
       offSet = pbigmsg->size;
     }

     //Yet to be copied bytes
     lenToCp -= offSet;

   }
  else
   { /* not a streams message */

     //Since we don't know how big of a Streams mblk need to
     //be allocated for copying the data from DMAed memory,
     //first get the MsgBodySize directly.
     MercMsg = (PMERC_HOSTIF_MSG)(pbigmsg->host_address);
     MsgBodySize = (MERCURY_GET_BODY_SIZE(MercMsg));

     lenToCp = (MsgBodySize) + sizeof(MERC_HOSTIF_MSG);

     DataMsg = supp_alloc_buf(pbigmsg->size, GFP_DMA);
     if(DataMsg == NULL) {
       printk("rcv_ww_process_big_msgs_from_adapter: Cannot alloc Data buffer\n");
       MSD_ERR_DBGPRINT("rcv_ww_process_big_msgs_from_adapter: Cannot alloc Data buffer\n");
       //MSD_FREE_MESSAGE(Msg);
       Status = MD_FAILURE;
       return(Status);
       }

     //Calculate the offset
     if (lenToCp <= pbigmsg->size) {
       offSet = lenToCp;
       MD_SET_MSG_WRITE_PTR(DataMsg, (MD_GET_MSG_READ_PTR(DataMsg)) + (offSet));
     } else {
       offSet = pbigmsg->size;
     }

     //Copy the Data from DMAed memory to DataMsg
     MsdCopyMemory(pbigmsg->host_address, DataMsg->b_rptr, offSet);

     //Yet to be copied bytes
     lenToCp -= offSet;

     //Free the DMAble memory at the index,
     //and try allocating Streams Msg
     //But do this after sending the message
     //up. Need to turn off the vbit

   } /* SBIT */

  //If the Amount of data DMAed is > sizeofBigMsgBlock,
  //then we will have >1 user context, i.e multiple
  //indexes of the rcv descriptor table

  for ( count = 1; count < numContexts; count ++)
  {
    if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
        (!(pwwDev->mfaBaseAddressDescr.host_address)))
      {
       return(MD_SUCCESS);
      }

    pUserContext += (2*(sizeof(merc_uint_t)));
    dmaDescIndex = *((pmerc_ulong_t)(pUserContext));

    //Get the descritptor at dmaDescIndex
    pbigmsg= pwwDev->BigMsgRcvMemStr.pbigMsgDescr[dmaDescIndex];


    //Check the vbit
    if (!(pbigmsg->flag & MERCD_WW_DMA_DESCR_VBIT))
    {
      printk("Got a Wrong DMA descriptor with vbit 0");
#ifdef LFS
    return (0);
#else
    return;
#endif
    }

    //Now look at the sbit and determine if the memory at dmaDescIndex
    //is Streams buf or DMAble memory that was allocated with SCHEME2

    if (pbigmsg->flag & MERCD_WW_DMA_DESCR_SBIT)
     {
       nDataMsg = (PSTRM_MSG)(pbigmsg->host_address);
       //printk("dmaDescIndex: %d lenToCp: %d\n", dmaDescIndex, lenToCp);

       //The following is important to address some race and
       //edge conditions. Do not set pbigmsg->size and flag
       //to 0 here:
       pbigmsg->host_address = 0;
       pbigmsg->board_address = 0;

       //Calculate the offset
       if (lenToCp <= pbigmsg->size) {
         offSet = lenToCp;
         MD_SET_MSG_WRITE_PTR(nDataMsg, (MD_GET_MSG_READ_PTR(nDataMsg)) + (offSet));
         } else {
          offSet = pbigmsg->size;
        }
      //Yet to be copied bytes
       lenToCp -= offSet;
     }
    else
     { /* not a streams message */

       nDataMsg = supp_alloc_buf(pbigmsg->size, GFP_DMA);

       if(nDataMsg == NULL) {
         printk("rcv_ww_process_big_msgs_from_adapter: Cannot alloc Data buffer\n");
         MSD_LEVEL2_DBGPRINT(
               "rcv_ww_process_big_msgs_from_adapter: Cannot alloc Data buffer size: 0x%x\n",
                pbigmsg->size
                );
         Status = MD_FAILURE;
         return(Status);
         }

       //Calculate the offset
       if (lenToCp <= pbigmsg->size) {
             offSet = lenToCp;
             MD_SET_MSG_WRITE_PTR(nDataMsg, (MD_GET_MSG_READ_PTR(nDataMsg)) + (offSet));
           } else {
             offSet = pbigmsg->size;
        }

       //Copy the Data from DMAed memory to DataMsg
       MsdCopyMemory(pbigmsg->host_address, nDataMsg->b_rptr, offSet);

       //Yet to be copied bytes
       lenToCp -= offSet;
     }
    MSD_LINK_MESSAGE(DataMsg,nDataMsg);
    nDataMsg = NULL;
  } /* for count */

  pmbTrainSt->dataMblks= (pmerc_void_t)DataMsg;

#ifdef LFS
  Status = MD_SUCCESS;
#endif
 return (Status);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_big_msg_in_mfs
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This processes the BIG MSG that is received in MF.
 *                              :
 * Additional comments          : According the new rcv BIG Msg protocol, the BIG Msgs
 *                              : are received in MFs. The first MF carrying the BIG
 *                              : Message is of Type MERCD_WW_BIG_MSG_FROM_BOARD_INMF_START
 *                              : and subsequent MFs carrying the rest of the BIG Message
 *                              : is of Type MERCD_WW_BIG_MSG_FROM_BOARD_INMF_CONTINUE.
 *                              : These MFs are further identified by a sequence number
 *                              : in descending order and the last MF has the sequence
 *                              : number 0. That means a BIG Message will span multiple
 *                              : MFs
 *                              :
 *                              : ERROR CONDITIONS:
 *                              : 1. There is a MF of type INMF_START and there is no
 *                              : INMF_CONTINUE MF following a START MF.
 *                              : 2. There is a MF of type INMF_START with a sequence
 *                              : number 0.
 *                              :
 *                              : Since the Parent docs has no info about this, we will
 *                              : a fancy diagram about the Header Lay out:
 *                              :
 *                              : BEAUTIFUL MF HEADER:
 *                              :   64             32                  0
 *                              :   ------------------------------------
 *                              :   |  msgSize       |   mfType        |
 *                              :   ------------------------------------
 *                              :   |  reserved      |   seq num       |
 *                              :   ------------------------------------
 *                              :   |             payload              |
 *
 **************************************************************************************/
merc_uint_t
rcv_ww_process_big_msg_in_mfs(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
#ifndef LFS
  pmerc_uchar_t                     pUserContext;
#endif
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  PSTRM_MSG                                  Msg;
  PSTRM_MSG                      linkedDataMblks;
  PMDRV_MSG                                MdMsg;
#ifndef LFS
  PMERC_HOSTIF_MSG                       MercMsg;
#endif
  PMERC_HOSTIF_MSG                     FwMessage;
#ifndef LFS
  merc_uchar_t                             Class;
#endif
  mercd_ww_prep_mf_train_sT            mfTrainSt;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                        BindHandle;
#else
#ifndef LFS
  MBD_HANDLE                        BindHandle;
#endif
#endif
#ifndef LFS
  merc_uint_t                              count;
  mercd_ww_mblk_train_info_sT          trainInfo;
#endif
  merc_uint_t                          seqNumber;
  mercd_ww_dispatch_rcvd_msg_sT         dispMsg;

  MSD_FUNCTION_TRACE("rcv_ww_process_big_msg_in_mfs 0x%x %s #%d\n", 
                      (size_t)pmfAddrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
     (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
     return(MD_SUCCESS);
   }


  //Validate the Sequence Number
  seqNumber =  *((pmerc_uint_t)(pmfAddress + MERCD_WW_MF_HEADER_SIZE));

  if ( seqNumber <= 0 ) {
      printk("BIG Msg in MF has Invalid Sequence Number %d\n", seqNumber);
      Status = MD_FAILURE;
      return(Status);
  }


  //Now Alloc an mblk for driver header
  Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ, GFP_DMA);
  if(Msg == NULL) {
    printk("rcv_ww_process_big_msg_in_mfs: Cannot alloc Msg buffer\n");
    MSD_LEVEL2_DBGPRINT(
              "rcv_ww_process_big_msg_in_mfs: Cannot alloc Msg size: 0x%x\n",
               sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ
               );
    Status = MD_FAILURE;
    return(Status);
  }

  mfTrainSt.pwwDevi = pwwDev;
  mfTrainSt.pmfAddress = pmfAddress;
  mfTrainSt.dataMblks = NULL;
  Status = rcv_ww_prepare_mf_chain_to_sendup(&mfTrainSt);

  if (Status != MD_SUCCESS) {
     printk("Could not copy data from MFs to mblks\n");
     MSD_LEVEL2_DBGPRINT(
             "Could not copy data from MFs to mblks: pwwDev: 0x%x pmfAddress: 0x%x\n",
              pwwDev, pmfAddress 
              );

     MSD_FREE_MESSAGE(Msg);
     return(Status);
  }

  linkedDataMblks = (PSTRM_MSG)mfTrainSt.dataMblks;

  MD_SET_MSG_TYPE(linkedDataMblks, M_DATA);


  MSD_LINK_MESSAGE(Msg,linkedDataMblks);


  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);


  FwMessage = (PMERC_HOSTIF_MSG)linkedDataMblks->b_rptr;

  dispMsg.padapter = (pmerc_void_t)padapter;
  dispMsg.mb = (pmerc_void_t)Msg;
  dispMsg.fwmsg = (pmerc_void_t)FwMessage;

  Status = rcv_ww_dispatch_mblk_train_up(&dispMsg);

  return (MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_prepare_mf_chain_to_sendup
 * Function Type                : Host FW Receive Support function for
 *                              : BIG Messages following the BIG Msg in MF Protocol
 *
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 *                              :
 * Additional comments          : The RTK has sent the BIG Msg in mutiple MFs and
 *                              : we already have the MF of type INMF_START. We
 *                              : validated its sequence number. Now we need to
 *                              : copy the payload in this MF to newly allocated mblk
 *                              :
 *                              : After copying the INMF_START MF's paytload to
 *                              : newly allocated mblk, we read the outBoundQ
 *                              : port to see if there are anymore MFs with type
 *                              : INMF_CONTINUE. Atleast there must be one since
 *                              : INMF_START MF never carries a sequence number 0.
 *                              : We copy the payload of each subsequent INMF_CONTINUE
 *                              : MF to the new mblk until:
 *                              :
 *                              : 1. we got all the INMF_CONTINUE MFs and copied their
 *                              :    payload to newly alloced mblk, and we never
 *                              :    ran out of space in new mblk. Also we
 *                              :    get out of this loop when we receive a INMF_CONTINUE
 *                              :    MF with seqNumber == 0. We write the index of each
 *                              :    INMF_CONTINUE MF back to inBoundQ. But, we leave
 *                              :    the index of INMF_START to be written back by the
 *                              :    main loop in msdwwrcv_ww_msgready_intr_processing.
 *                              :
 *                              : 2. We ran out of space in newly allocated mblk while
 *                              :    while copying the payload of INMF_CONTINUE MF with
 *                              :    seqNumber > 0. In that case we send up our user
 *                              :    buffer i.e the new mblk,   and we stop
 *                              :    extracting the subsequent MFs with INMF_CONTINUE
 *                              :    type in the current loop. We will leave this to the
 *                              :    main loop in msdwwrcv_ww_msgready_intr_processing.
*************************************************************************************/

merc_uint_t
rcv_ww_prepare_mf_chain_to_sendup(pmercd_ww_prep_mf_train_sT pmfTrainSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           lenToCp;
  pmercd_ww_dev_info_sT                   pwwDev;
#ifndef LFS
  merc_uint_t                              count;
#endif
#ifdef LFS
  merc_uint_t                            mfIndex = 0;
#else
  merc_uint_t                            mfIndex;
#endif
  pmerc_uchar_t                     pUserContext;
  pmerc_uchar_t                      pRepContext;
  merc_uint_t                      PayloadSzInMF;
  merc_uint_t                    BigMsgSizeToCopy;
  pmerc_uchar_t                       pmfAddress;
  PMERC_HOSTIF_MSG                       MercMsg;
  merc_uint_t                    UsrBufSpaceLeft;
  merc_uint_t                     MaxMsgPossible;
  merc_uint_t                            msgType;
  mercd_ww_get_mfAddress_sT          mfAddressST;
  PSTRM_MSG                              DataMsg;
  pmercd_adapter_block_sT               padapter;
  merc_uint_t                          seqNumber;

  merc_uint_t                          mfcnt = 0;


  MSD_FUNCTION_TRACE("rcv_ww_prepare_mf_chain_to_sendup 0x%x %s #%d\n", 
                      (size_t)pmfTrainSt, __FILE__, __LINE__);

  pwwDev = pmfTrainSt->pwwDevi;
  pmfAddress = pmfTrainSt->pmfAddress;
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);;

  //Get a Pointer to the user context following the Header
  pRepContext = pUserContext = (pmfAddress + (2 * MERCD_WW_MF_HEADER_SIZE));

  //We need to find out what is the size of the BIG Msg that is
  //being sent in multiple MFs. This is equivalent to:
  //MERC_HOSTIF_MSG.BodySize + sizeof (MERC_HOSTIF_MSG);
  //We can find MERC_HOSTIF_MSG in the payload of the first
  //MF with MF type INMF_START
  MercMsg = (PMERC_HOSTIF_MSG)(pUserContext);
  BigMsgSizeToCopy = (MercMsg->BodySize + sizeof(MERC_HOSTIF_MSG));

  if (MercMsg->BodySize < 100)
      MSD_ERR_DBGPRINT("MercMsg->BodySize: %d Supposed to be 100\n", MercMsg->BodySize);

  //Now we need to allocate a memory of size equivalent to the
  //biggest message size possible which is equivalent to:
  //MERCD_WW_BIGMSG_MAX_BODY_SIZE + sizeof (MERC_HOSTIF_MSG);
  //Now if BigMsgSizeToCopy > MERCD_WW_BIGMSG_MAX_BODY_SIZE +
  //                               sizeof (MERC_HOSTIF_MSG),
  //then for sure, before we receive a MF with INMF_CONTINUE
  //with seqNumber == 0, we will run out of space in
  //in the new allocated mblk . Lets just capture it and notify
  //it for now, should such a thing happen.

  UsrBufSpaceLeft = MaxMsgPossible = (MERCD_WW_BIGMSG_MAX_BODY_SIZE +
                                   sizeof (MERC_HOSTIF_MSG));
  if (BigMsgSizeToCopy > MaxMsgPossible)
  {
     MSD_LEVEL2_DBGPRINT(
            "Expect to run out of space BigMsgSizeToCopy: %d MaxMsgPossible: %d\n",
             BigMsgSizeToCopy, MaxMsgPossible
             );
     //We will continue
  }

 
  //Allocate a Streams buffer of Size MaxBufSpace
  pmfTrainSt->dataMblks = DataMsg = supp_alloc_buf(MaxMsgPossible, GFP_DMA);

  if (DataMsg == NULL)
  {
   printk("PrepareMFChainToSendUp ERROR: Alloc for DataMsg Failed\n");
   return(MD_FAILURE);
  }

  //For our convenience make b_wptr == b_rptr.
  //NOTE: supp_alloc_buf bumps b_wptr by MaxMsgPossible
  DataMsg->b_wptr = DataMsg->b_rptr;

  //Peek at the Msg type
  msgType = *((pmerc_uint_t)(pmfAddress));

  Status = MD_SUCCESS;

  while ( Status == MD_SUCCESS)
   {
       if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
         (!(pwwDev->mfaBaseAddressDescr.host_address)))
        {
          return(MD_SUCCESS);
        }

      mfcnt++;


       //Get the payload  Size that is carried by th current MF.
       //It is equivalent to  = (Size of MF - (2 * MERCD_WW_MF_HEADER_SIZE))
       PayloadSzInMF =  *((pmerc_uint_t)(pmfAddress + (sizeof(merc_uint_t))));

       lenToCp = PayloadSzInMF;
       BigMsgSizeToCopy -= lenToCp;
       UsrBufSpaceLeft -= lenToCp;

      //Copy the payload from pUserContext to DataMsg->b_wptr
      MsdCopyMemory(pUserContext, DataMsg->b_wptr, lenToCp);

      //Bump the b_wptr
      DataMsg->b_wptr += lenToCp;

      if (msgType != MERCD_WW_BIG_MSG_FROM_BOARD_INMF_START)
      {
        //Write the MF index back to the inBoundQ
        MSD_ENTER_MUTEX(&pwwDev->ww_oboundQ_mutex);
        MsdWWPutoutBoundQReg(padapter, mfIndex);
        MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);

        //atomic_inc(&pwwDev->pww_counters->total_outboundq_writes);
      }
     //Check if we just processed a INMF_CONTINUE MF with seqNumber == 0
     seqNumber =  *((pmerc_uint_t)(pmfAddress + MERCD_WW_MF_HEADER_SIZE));

     if ( seqNumber == 0)
     {
       MSD_LEVEL2_DBGPRINT("Done Copying Payload. Processed All the CONTINUE MFs..\n");
       Status = MD_SUCCESS;
       return(Status);
     }

    //Check if we have copied all the BIG Msg payload
     if ((UsrBufSpaceLeft == 0) && (BigMsgSizeToCopy > 0))
     {
       printk(" Ran Out of User Buffer Space. Still Big Msg Payload to Copy\n");
       MSD_LEVEL2_DBGPRINT(
            "Ran Out of User Buffer Space, UsrBufSpaceLeft: 0x%x, BigMsgSizeToCopy: 0x%x\n",
             UsrBufSpaceLeft,
             BigMsgSizeToCopy
             );
       Status = MD_SUCCESS;
       return(Status);
     }


    //Get the Next MF from the outBoundQ
    mfAddressST.pwwDevi = pwwDev;
    mfAddressST.pmfAddress = NULL;
    Status = msdwwutl_ww_read_posted_outboundQ_for_mf_address(&mfAddressST);

    if(Status != MD_SUCCESS)
     {
        MSD_FREE_MESSAGE(DataMsg);
        pmfTrainSt->dataMblks = NULL;
        return(Status);
     }

    pmfAddress = mfAddressST.pmfAddress;
    mfIndex    = mfAddressST.mfIndex;

    //Peek at the Msg type
    msgType = *((pmerc_uint_t)(pmfAddress));

   if (msgType != MERCD_WW_BIG_MSG_FROM_BOARD_INMF_CONTINUE)
      {
        printk("We  have seen a different mf type before INMF_CONTINUE with seq#0\n");
        MSD_LEVEL2_DBGPRINT(
             "Seeing different mf type msgType: 0x%x\n", msgType
              );
        //Write the MF index back to the inBoundQ
        MSD_ENTER_MUTEX(&pwwDev->ww_oboundQ_mutex);
        MsdWWPutoutBoundQReg(padapter, mfIndex);
        MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);

        //atomic_inc(&pwwDev->pww_counters->total_outboundq_writes);
        Status = MD_FAILURE;
        MSD_FREE_MESSAGE(DataMsg);
        pmfTrainSt->dataMblks = NULL;
        return(Status);
      }
    pUserContext = (pmfAddress + (2 * MERCD_WW_MF_HEADER_SIZE));
   } /* while */

   //printk("mfcnt: %d\n", mfcnt);

return(Status);
}

/************************************************************************************
 * Function Name                : rcv_ww_dispatch_mblk_train_up
 * Function Type                : Host FW Receive Support function for
 *                              : SMALL and BIG Messages
 *
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This sends the received message up. This could be
 *                              : a multiple of mblks.
 * Additional comments          :
***************************************************************************************/
merc_uint_t
rcv_ww_dispatch_mblk_train_up(pmercd_ww_dispatch_rcvd_msg_sT  pdispMsg)
{
 pmercd_adapter_block_sT         padapter;
 merc_uchar_t                    Class;
 PSTRM_MSG                       Msg = NULL;
 PMDRV_MSG                       MdMsg = NULL;
 PMERC_HOSTIF_MSG                FwMessage;
 MD_STATUS                       Status;
#ifdef _8_BIT_INSTANCE
 MSD_HANDLE                      BindHandle = 0;
#else
 MBD_HANDLE                      BindHandle = 0;
#endif
#ifndef LFS
 PMERC_HOSTIF_MSG                pHdr;
 pmerc_uchar_t                   ptr = NULL;
#endif
 merc_uint_t                     destinationBoardNumber;
 pmercd_adapter_block_sT         pXadapter;
 merc_uint_t                     XAdapterNumber;

  MSD_FUNCTION_TRACE("rcv_ww_dispatch_mblk_train_up 0x%x %s #%d\n", 
                      (size_t)pdispMsg, __FILE__, __LINE__);
  padapter = (pmercd_adapter_block_sT)pdispMsg->padapter;
  Msg = (PSTRM_MSG)pdispMsg->mb;
  FwMessage = (PMERC_HOSTIF_MSG)pdispMsg->fwmsg;
  Status = MD_SUCCESS;

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(padapter->pww_info->mfaBaseAddressDescr.host_address))) {
      return(MD_SUCCESS);
  }

  if (!(Msg) || !(FwMessage)) {
      printk("rcv_ww_dispatch_mblk_train_up: Null  Message\n");
      return (MD_FAILURE);
  }


  //Useful Debug: GetKernelVersion TimeOut during the download
  //time is a common problem on a mis-match. The following
  //is useful for dumping the header info, and varfield in the
  //payload
#if 0
  pHdr = FwMessage;
  ptr = (pmerc_uchar_t)FwMessage;

  printk("Printing the Header\n");
  i_bprintf(pHdr, 24);
  printk("Byte dump of the varfields\n");
  if (*((pmerc_uint_t)(ptr+24))) {
     printk("Found some data in varfield:..\n");
      i_bprintf(ptr+24, 72);
  }
  
  //The MessageTypes for GetKernelVersion
  //command and response are 0x80011b and 0x80011c.
  if (pHdr->MessageType == 0x80011c)
      printk("Got Kernel Version response.My Favorite Problem..\n");
#endif

  //Trace Code: Copy the Message to the buffer (if required)
  if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
      supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)FwMessage);
  }


  //Fetch the message class
  Class = MERCURY_GET_MESSAGE_CLASS(FwMessage);
  

    //16BIT Change: Extract bind handle from destination address
#ifdef _8_BIT_INSTANCE
    BindHandle = MERCURY_GET_DEST_COMPINST(FwMessage);
#else
  if (padapter->rtkMode == 0) {
    //We need to access 8BIT Macros
    //BindHandle = (FwMessage->SourceDestComponent & 0xFFFF0000) >> 16;
    BindHandle = MERCURY_GET_DEST_COMPINST_8BITMODE(FwMessage);
  }
  else
  {
    //We need to access 16BIT Macros
    BindHandle = MERCURY_GET_DEST_COMP(FwMessage);
    BindHandle = BindHandle << 8;
    BindHandle |= MERCURY_GET_DEST_INST(FwMessage);
  }
#endif

  //The following is left commented out, since
  //we no longer support Rozetta. Also, the following
  //is creating panics some times by throwing us into
  //streaming code of fwmgr while messaging on a dti16.
  //10.15.02
  //Fix for Rozetta: where Class is resolving to
  //MERCURY_CLASS_DRIVER
  
#if 0
  if (BindHandle == 0)  {
     Class = MERCURY_CLASS_DRIVER;
   } else {
     Class = MERCURY_CLASS_SYSTEM;
  }
#endif

  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  switch(Class) {
     case MERCURY_CLASS_DRIVER: // to be procss locally
          if(fwmgr_reply_mgr(padapter,Msg)==MD_FAILURE) {
               MSD_LEVEL2_DBGPRINT("rcv_rceive_msg: ProcDriverClass failed.\n");
               Status = MD_FAILURE;
               return(Status);
            }
           break;

     case MERCURY_CLASS_SYSTEM: // send to user with msgloop
     case MERCURY_CLASS_RESOURCE:
     case MERCURY_CLASS_RESOURCE_INT:        /* PTR1489 */
	// Check for undelivered message
 	if (FwMessage->MessageType == 0x80004f) {
            MSD_ERR_DBGPRINT("rcv_rceive_msg: Undelivered message \n");
            MSD_FREE_MESSAGE(Msg);
            return (MD_FAILURE);
        }

        // Check id this is an Xmsg
        destinationBoardNumber = MERCURY_GET_DEST_BOARD(FwMessage);

        // In case of Linux, 0 is not a valid board Id. So the check for an
        // XMsg is if the destinationBoardNumber != 0. 0 is the host board
        // Id for Linux
        if (destinationBoardNumber) {
            if (destinationBoardNumber >=  MsdControlBlock->maxadapters) {
                MSD_ERR_DBGPRINT("mercd: adapter number from the firmware\n");
                MSD_FREE_MESSAGE(Msg);
                return(MD_FAILURE);
            }
            // grab the adapter number 
            XAdapterNumber = mercd_adapter_log_to_phy_map_table[destinationBoardNumber];
            if (XAdapterNumber != mercd_adapter_map[padapter->adapternumber]) {
                // 0xFF is not a valid adapter number
                if (XAdapterNumber == 0xFF) {
                    MSD_FREE_MESSAGE(Msg);
                    break;
                }

                pXadapter = MsdControlBlock->padapter_block_list[XAdapterNumber];
                if (pXadapter == NULL) {
                    MSD_FREE_MESSAGE(Msg);
                    break;
                }

                // validity checking
                if ((pXadapter->state & MERCD_ADAPTER_STATE_STARTED) ||
                    (pXadapter->state == MERCD_ADAPTER_STATE_DOWNLOADED)) {
                    merc_uint_t     Result;

	            if ((pXadapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	  		(pXadapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
                        MSD_ENTER_MUTEX(&pXadapter->snd_msg_mutex);
		        Result = strm_Q_snd_msg(Msg, pXadapter);
                        MSD_EXIT_MUTEX(&pXadapter->snd_msg_mutex);
                    } else {
                        Result = strm_ww_Q_snd_msg(Msg, pXadapter);
                    }

                    if (Result != MD_SUCCESS) {
                        MSD_ERR_DBGPRINT("MsdDownMessageStrategy:strm_Q_snd_msg failed.");
                        MSD_ERR_DBGPRINT(" Error=%d\n", Result);
                        MSD_FREE_MESSAGE(Msg);
                    }
                    break;

                } else {
                    MSD_FREE_MESSAGE(Msg);
                    break;
                }
            }
        }

        //Must ensure handle is kopacetic
        if(BindHandle == 0 || BindHandle >= MsdControlBlock->maxbind) {
               MSD_LEVEL2_DBGPRINT( "rcv_ww_dispatch_mblk_train_up BindHandle: %d, maxbind: %d\n",
                   BindHandle, MsdControlBlock->maxbind);
               MSD_FREE_MESSAGE(Msg);
               Status = MD_FAILURE;
               return(Status);
         }
        // test to see if handle is valid.  It coiuld've been
        //      closed just before this message is received

        if (MsdControlBlock->pbind_block_list == NULL) {
                MSD_LEVEL2_DBGPRINT("rcv_ww_dispatch_mblk_train_up: BIND BLOCK LIST NULL\n");
                MSD_FREE_MESSAGE(Msg);
                Status = MD_FAILURE;
                return(Status);
        }

        if(MsdControlBlock->pbind_block_list[BindHandle] == NULL) {
                MSD_LEVEL2_DBGPRINT("BindBlock NULL\n");
                MSD_FREE_MESSAGE(Msg);
                Status = MD_FAILURE;
                return(Status);
         }

        MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
        MD_SET_MDMSG_USER_CONTEXT(MdMsg, MD_MAP_BIND_TO_USER_CONTEXT(BindHandle));

        MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)) +sizeof(MDRV_MSG));


        if (supp_dispatch_up_Q(padapter,Msg)==MD_FAILURE) {
                MSD_LEVEL2_DBGPRINT("rcv_rceive_msg: ProcSystemClass failed.\n");
                Status = MD_FAILURE;
                return(Status);
        }
           break;

        default:
                MSD_ERR_DBGPRINT("mercd:ERROR:MsdReceiveMessage:Invalid Message Class (%d)",
                                        Class);
                MSD_FREE_MESSAGE(Msg);
                return(MD_FAILURE);
        }
  return (Status);
}

/************************************************************************************
 * Function Name          : rcv_ww_process_write_request_complete_from_adapter
 * Function Type          : Host FW Receive function
 * Inputs                 :
 * Outputs                :
 * Calling functions      :
 * Description            : This Write Complete MF
 * Additional comments    :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_write_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  merc_uint_t                        numContexts;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                          count = 0;
  merc_ushort_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  mercd_dhal_ww_free_desc_sT          freeDescSt;
  pmerc_uchar_t                     pUserContext;
#ifdef LFS
  pmercd_stream_connection_sT        StreamBlock = NULL;
#else
  pmercd_stream_connection_sT        StreamBlock;
#endif
  merc_ushort_t                     streamid = 0;
#ifndef LFS
  merc_uint_t                          index = 0;
#endif
  merc_uint_t                        blkSize = 0;
  merc_uint_t                     maxCanTake = 0;
  pmercd_ww_dma_descr_sT 	  dmaDescptr = 0;
  merc_uint_t              snd_broken_stream = 0;
  merc_uint_t           stream_eval_can_take = 0;


  MSD_FUNCTION_TRACE("rcv_ww_process_write_request_complete_from_adapter 0x%x %s #%d\n", 
                      (size_t)pmfAddrSt,
                      __FILE__, __LINE__
                     );

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
  {
      return(MD_SUCCESS);
  }



  //Get a Pointer to the user context following the Header
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

  //Get the number of user contexts
  pmfAddress += (sizeof(merc_uint_t));
  numContexts = *((pmerc_uint_t)(pmfAddress));


  //For each index at each user context, free the Msg block that
  //has been just DMAd and mark the DMA descriptor as free

  MSD_LEVEL2_DBGPRINT("Rcv Write Completion: pmfAddress: 0x%x, numContexts: %d\n", pmfAddress, numContexts);


  for ( count = 0; count < numContexts; count ++)
  {
    if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
     {
        return(MD_SUCCESS);
     }

     dmaDescIndex = *((pmerc_uint_t)(pUserContext));
     pUserContext += (sizeof(merc_uint_t));
     streamid = *((pmerc_uint_t)(pUserContext));
     pUserContext += (sizeof(merc_uint_t));

     //Check the sanity of StreamId
     if (streamid >= MSD_MAX_STREAM_ID)
       {
         MSD_ERR_DBGPRINT("rcv_ww_process_write_request_complete_from_adapter:\
			  StreamId: %d >= MSD_MAX_STREAM_ID for Adapater %d\n",
			  streamid, padapter->adapternumber);
	 continue;
       } 

     MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
     StreamBlock = padapter->pstream_connection_list[streamid];
     MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

     if (StreamBlock == NULL)
      {
        MSD_ERR_DBGPRINT("rcv_ww_process_write_request_complete_from_adapter:\
	 pstream_connection_list has NULL for StreamId: %d\n", streamid);
	continue;
      }

     MSD_LEVEL2_DBGPRINT("Write Complete MF: streamid: %d descIndex: %d StreamBlock: 0x%x\n",
               streamid, dmaDescIndex, StreamBlock);

     // check for broken stream
     dmaDescptr = StreamBlock->pSendStrmDmaDesc[dmaDescIndex];
     if (dmaDescptr->flag & 0x80) {
	 snd_broken_stream = 1;
	 dmaDescptr->flag &= 0x7f;
     }

     //Check if this index is being marked as used in the
     //bitmap. This is an extreme case, but should never
     //be marked as freed.
     dmaDescSt.pBitMap = StreamBlock->pSendBitMapValue;
     dmaDescSt.szBitMap = StreamBlock->szSendBitMap;
     dmaDescSt.index = dmaDescIndex;
     MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
      Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
      if (Status == MD_FAILURE)
       {
        //Dump the BigMsgSndMemStr.pBitMapValue;
        MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
        MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
        //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
        //printk(
        //"rcv_ww_process_write_request_complete_from_adapter:Adapter out.Desc not marked\n");
        //i_bprintf(dmaDescSt.pBitMap, 4);
#if 0
        ///////////////////////////////////////////////////
        //test only...
        //////////////////////////////////////////////////
        {
         pmercd_ww_dma_descr_sT ptr = 0;
         ptr = StreamBlock->pSendStrmDmaDesc[dmaDescIndex];
         if (ptr != NULL)
          {
           if (ptr->host_address)
              MSD_LEVEL2_DBGPRINT("There is a Mblk at this dmadescindex..size: %d\n", ptr->size);
           if (ptr->board_address)
              MSD_LEVEL2_DBGPRINT("There is a board_address at this dmadescindex..add: 0x%x\n",
                     ptr->board_address);
          }

         }
         //////////////////////////////////////////////////
         //test ends
         //////////////////////////////////////////////////
#endif
        MSD_LEVEL2_DBGPRINT("%s #%d\n", __FILE__, __LINE__);
	//printk("rcv_ww_process_write_request_complete_from_adapter: sid %d not used\n", streamid);
        MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
        //return(MD_FAILURE);
	continue;
       }
       StreamBlock->writeDMAPending = 0;

     //We leave the first descritpor at dmaDescIndex intact,
     //and mark its corresponding bit in bitmap free.
     //But we free the DMA descriptors following the next link
     //of the first descritptor at dmaDescIndex.
     //And finally free the Streams Msg block or DMAble memory
     // that we just DMAd to Board.

     freeDescSt.pwwDevi = pwwDev;
     freeDescSt.index = dmaDescIndex;
     freeDescSt.StreamBlock = (pmerc_void_t)StreamBlock;
     freeDescSt.purpose = MERCD_WW_STRM_WRITE_COMPLETE;


     (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC])((pmerc_void_t)&freeDescSt);
     if (freeDescSt.ret == MD_FAILURE)
     {
       printk(" rcv_ww_process_big_msg_read_ack_msg_from_adapter FAILED\n");
       MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
       return(MD_SUCCESS);
     }

     //Now we are left with the only descriptor at dmaDescIndex. Mark
     //this as free in the bitmap.
     dmaDescSt.pBitMap = StreamBlock->pSendBitMapValue;
     dmaDescSt.szBitMap = StreamBlock->szSendBitMap;
     dmaDescSt.index = dmaDescIndex;

     Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
     MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex)

     MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
     //Available MFs for the StreamBlock is more by 1
     StreamBlock->activeWWMfs--;

     //Check if can take is required
     blkSize = MERCURY_HOST_IF_BLK_SIZE+sizeof(USER_HEADER);
     maxCanTake = StreamBlock->maxWWMfs * blkSize;

     if ( (StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND) || 
	  (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND) ||
          (StreamBlock->can_take == maxCanTake) ) {
         MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
         continue;
     } else {
	 stream_eval_can_take = 1;
     }
     MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);

  }


  if (snd_broken_stream) {
      // send borken stream to user to improve latency
      strm_sndbrokenstream2usr(StreamBlock);
      push_trace_msg(padapter, StreamBlock->pbind_block, 0x5a02);
  } else if (stream_eval_can_take) {
     //It's required so send a can take to host
     MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
     if ((StreamBlock->can_take+blkSize) <= maxCanTake) {
         StreamBlock->can_take += blkSize;
         StreamBlock->accumulated_usr_ack = MERCURY_HOST_IF_BLK_SIZE;
     } else {
         //Last block
         if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) 
             StreamBlock->accumulated_usr_ack = maxCanTake - StreamBlock->can_take;
 	 else
	     StreamBlock->accumulated_usr_ack = maxCanTake - StreamBlock->can_take - sizeof(USER_HEADER);
         StreamBlock->can_take = maxCanTake;
     }
     MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);

     if (strm_ack2usr(StreamBlock, StreamBlock->accumulated_usr_ack) != MD_SUCCESS) {
         printk("Unable to send Can Take (strm_ack2usr failed)\n");
     }
  } 
 
  return(MD_SUCCESS);
}

/************************************************************************************
 * Function Name          : rcv_ww_process_big_msg_cancel_ack_from_adapter
 * Function Type          : Host FW Receive function
 * Inputs                 :
 * Outputs                :
 * Calling functions      :
 * Description            : Cancel Msg Ack
 * Additional comments    :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_big_msg_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uint_t                        pmfAddress;
  pmerc_uchar_t                          pmfHead;
#ifndef LFS
  pmerc_uchar_t                     pUserContext;
#endif
  merc_uint_t                        numContexts;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
#ifndef LFS
  merc_ushort_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  mercd_dhal_ww_free_desc_sT          freeDescSt;
#endif
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                          bindHandle;
#else
#ifndef LFS
  MBD_HANDLE                          bindHandle;
#endif
#endif
#ifndef LFS
  merc_uint_t                          count = 0;
#endif


  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
#ifdef LFS
  pmfHead = pmfAddrSt->pmfAddress;
  pmfAddress = (pmerc_uint_t) pmfAddrSt->pmfAddress;
#else
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;
#endif
 
  pmfAddress = (pmerc_uint_t)(pmfAddress + (sizeof (merc_uint_t)));
  numContexts = *((pmerc_uint_t)pmfAddress);

  if (numContexts == 0) {
    printk(" BIG MSG CANCEL ACK with numContext = 0 \n");
    return (Status);
  }
  printk(" Got a big message cancel ack and not doing anything for %d contexts \n", numContexts);

#if 0
  // Move up to the first Context
  pmfAddress = (pmerc_uint_t)(pmfAddress + (sizeof (merc_uint_t)));

  dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
  dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;

  MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);

  for (i=0;i< numContexts;i++) {

      context = *((pmerc_uint_t)pmfAddress);

      if (context == 0xFF)
          context = 0;

      // Check if the dmadescriptors are being used 
      dmaDescSt.index = context;

      status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
      if (status == MD_FAILURE) {
          printk("BigMsgCancelAck: Context = used, DMA DESCR NOT for %d \n", context);
          continue;
      }

      bindHandle = *((MBD_HANDLE *)((pmerc_uchar_t)pmfAddress+(sizeof(merc_ushort_t))));

      if (!bindHandle) {
          printk("BigMsgCancelAck: 0 BindHandle for dmaDescIndex %d\n", dmaDescIndex);
          //i_bprintf(pUserContext, 8);
          continue;
      }


      // We leave the first descritpor at dmaDescIndex intact and mark its 
      // corresponding bit in bitmap free.  But we free the DMA descriptors 
      // following the next link of the first descritptor at dmaDescIndex.
      // And finally free the Streams Msg block or DMAble memory
      // that we just DMAd to Board.

      freeDescSt.pwwDevi = pwwDev;
      freeDescSt.index = context;
      freeDescSt.StreamBlock = 0;
      freeDescSt.purpose = 0;

      (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC])((pmerc_void_t)&freeDescSt);
      if (freeDescSt.ret == MD_FAILURE) {
          printk("BigMsgCancelAck: WW_DEALLOC_SND_BIGMSG_DESC failed for %d\n", context);
          MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
          return(MD_SUCCESS);
      }
 
      // Now we are left with the only descriptor at dmaDescIndex.
      // Mark this as free in the bitmap.
      dmaDescSt.index = context;

      Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
      ++pwwDev->BigMsgSndMemStr.free_counter;
      vSndFreeDesc = pwwDev->BigMsgSndMemStr.free_counter;

      // Mark the Global Message Pending BitMap if the BindBlock is alive
      if (bindHandle) {
          pbitmap = pwwDev->pPendMsgBitMap[bindHandle];
          BindBlock = MsdControlBlock->pbind_block_list[bindHandle];
          if (BindBlock) {
              dmaDescSt.pBitMap = pbitmap;
              dmaDescSt.szBitMap = pwwDev->szPendMsgBitMap;
              dmaDescSt.index = context;
              MSD_LEVEL2_DBGPRINT("BigMsgCancelAck: bindHandle: %d before free\n", bindHandle);
              //i_bprintf(pbitmap, 8);
              Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
              //i_bprintf(pbitmap, 8);
              BindBlock->WWPendReadAckForToBoardBigMsgs--;
          } else {
              // Unbind for this BindHandle might have already taken place. 
              // Just bzero the pbitmap at bindHandle in pPendMsgBitMap.
              MSD_ZERO_MEMORY(pbitmap, pwwDev->szPendMsgBitMap);
          }
      } else {
	  printk("BigMsgCancelAck:  Should not happen since we already check bindhandle\n");
      }
  }

  MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex)
#endif
 return(Status);
}

/************************************************************************************
 * Function Name          : rcv_ww_process_stream_reqeust_cancel_ack_from_adapter
 * Function Type          : Host FW Receive function
 * Inputs                 :
 * Outputs                :
 * Calling functions      :
 * Description            : Cancel Stream Ack
 * Additional comments    :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_stream_reqeust_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  PSTRM_MSG 				     Msg;
  PMDRV_MSG   				   MdMsg;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                     pUserContext;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                           StreamId;
  merc_uint_t                             Reason;
  pmercd_stream_connection_sT        StreamBlock;

  MSD_LEVEL2_DBGPRINT("rcv_ww_process_stream_reqeust_cancel_ack_from_adapter...\n");

  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address))) {
      return(MD_SUCCESS);
  }

  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);
  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress));
  pmfAddress += (sizeof(merc_uint_t));
  Reason = *((pmerc_uint_t)(pmfAddress));

  // Check the sanity of StreamId
  if (StreamId >= MSD_MAX_STREAM_ID) {
     MSD_ERR_DBGPRINT("rcv_ww_process_stream_reqeust_cancel_ack_from_adapter...\n");
     MSD_ERR_DBGPRINT("StreamId: %d >= MSD_MAX_STREAM_ID for Adapater %d\n",
                                         StreamId, padapter->adapternumber);
     return(MD_FAILURE);
  }

  // Get the StreamBlock
  MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
  StreamBlock = padapter->pstream_connection_list[StreamId];
  MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

  if (StreamBlock == NULL) {
     printk("rcv_ww_process_stream_reqeust_cancel_ack_from_adapter...\n");
     printk("pstream_connection_list has NULL for StreamId: %d\n", StreamId);
     return(MD_FAILURE);
  }

  // The reason should be vaild because the cancel was sent on an active stream
  if (Reason) {
      if (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) {
	  rcv_ww_process_stream_session_close(pmfAddrSt);
      } else {
	  rcv_ww_process_eos_read_stream_from_adapter(pmfAddrSt);
      }
  }

  // Normal regular or internal close...
  if (StreamBlock->SendCancelStreamMf == 1) {
      PSTREAM_CLOSE_INTERNAL Ptr;

      Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_CLOSE_INTERNAL), GFP_ATOMIC);
      if (Msg == NULL) {
	  printk("rcv_ww_process_stream_reqeust_cancel_ack_from_adapter...\n");
          printk("memory allocation failed\n");
	  return(MD_FAILURE);
      }
      MSD_LEVEL2_DBGPRINT("2045: CancelAck on internal...\n");
      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
      MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_INTERNAL);
      MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
      Ptr = (PSTREAM_CLOSE_INTERNAL)MD_GET_MDMSG_PAYLOAD(MdMsg);
      Ptr->StreamBlock = StreamBlock;
      StreamBlock->state = MERCD_STREAM_STATE_BROKEN;
      mid_strmmgr_strm_close_internal(StreamBlock->pbind_block->popen_block, Msg);
  } else {
      PSTREAM_CLOSE Ptr;

      Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_CLOSE), GFP_ATOMIC);
      if (Msg == NULL) {
	  printk("rcv_ww_process_stream_reqeust_cancel_ack_from_adapter...\n");
          printk("memory allocation failed\n");
	  return(MD_FAILURE);
      }
      MSD_LEVEL2_DBGPRINT("2045: CancelAck on normal...\n");
      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
      MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE);
      MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
      Ptr = (PSTREAM_CLOSE)MD_GET_MDMSG_PAYLOAD(MdMsg);
      Ptr->StreamId = StreamBlock->id;
      Ptr->Flags = StreamBlock->cancelFlags;
      StreamBlock->state = MERCD_STREAM_STATE_BROKEN;
      MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
      mid_strmmgr_strm_close(StreamBlock->pbind_block->popen_block, Msg);
  }
 
  StreamBlock->SendCancelStreamMf = 0;
  return (MD_SUCCESS);

}

/************************************************************************************
 * Function Name          : rcv_ww_process_stream_flush_request_ack_from_adapter
 * Function Type          : Host FW Receive function
 * Inputs                 :
 * Outputs                :
 * Calling functions      :
 * Description            : Flush Stream Ack
 * Additional comments    :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_stream_flush_request_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  pmerc_uchar_t                       pmfAddress;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                           StreamId;
  pmercd_stream_connection_sT        StreamBlock;

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);

  pmfAddress = pmfAddrSt->pmfAddress;

  MSD_LEVEL2_DBGPRINT("rcv_ww_process_stream_flush_request_ack_from_adapter...\n");

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
         return(MD_SUCCESS);
   }

  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress)); 

  if (StreamId > MSD_MAX_STREAM_ID)
  {
    MSD_LEVEL2_DBGPRINT("Got INVALID StreamId: %d in FLUSH AKCK MF From FW: INVESTIGATE\n", StreamId);
    return(MD_SUCCESS);
  }

  StreamBlock = padapter->pstream_connection_list[StreamId];

  if (!(StreamBlock->pbind_block->WWFlushAckPending))
   {
    MSD_LEVEL2_DBGPRINT("Got FLUSH ACK for the Stream for which no WWFlushAckPending: INVESTIGATE\n");
    return(MD_SUCCESS);
   }
  else
   {
    MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
     StreamBlock->pbind_block->WWFlushAckPending = 0;
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
    }
return (MD_SUCCESS);
}

/************************************************************************************
 * Function Name          : rcv_ww_process_data_buffer_request_from_adapter
 * Function Type          : Host FW Receive function
 * Inputs                 :
 * Outputs                :
 * Calling functions      :
 * Description            :  Board is asking for More Data Buffers
 * Additional comments    :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_data_buffer_request_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  pmerc_uchar_t                       pmfAddress;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_uint_t                           StreamId;
  pmercd_stream_connection_sT        StreamBlock;
  merc_uint_t                     TotalDataOnStr;
  mercd_ww_dmaDscr_Index_sT      dmaDescSt = {0};

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);

  pmfAddress = pmfAddrSt->pmfAddress;

  MSD_LEVEL2_DBGPRINT("rcv_ww_process_data_buffer_request_from_adapter...\n");

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
         return(MD_SUCCESS);
   }

  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress)); 

  //Amount of Data on Stream
  pmfAddress += (sizeof(merc_uint_t));
  TotalDataOnStr = *((pmerc_uint_t)(pmfAddress));
  MSD_LEVEL2_DBGPRINT("Amount of data on Stream: %d StreamId: %d\n", TotalDataOnStr, StreamId);

  if (StreamId > MSD_MAX_STREAM_ID)
  {
    MSD_LEVEL2_DBGPRINT("Got INVALID StreamId: %d in MORE DATABUF  MF From FW: INVESTIGATE\n", StreamId);
    return(MD_SUCCESS);
  }

  StreamBlock = padapter->pstream_connection_list[StreamId];

   if (StreamBlock == NULL)
    {
      MSD_LEVEL2_DBGPRINT("Board Asks for More DATABUFs but StreamBlock is NULL\n");
      return(MD_SUCCESS);
    }

   if (StreamBlock->id != StreamId)
    {
       MSD_LEVEL2_DBGPRINT("StreamBlock->id != StreamId id: %d StreamId: %d\n",
               StreamBlock->id, StreamId);
       return(MD_SUCCESS);
    }

   //By default, the Receive Stream DATA Bufs are allocated during 
   //set_param time. We should never allocate it here in the dpc
   //context
   if (!(StreamBlock->WWRcvStrDataBufAllocd))
     {
        MSD_LEVEL2_DBGPRINT("buffer_request_from_adapter: calling mid_wwmgr_allocate_mem_for_rcvdatablks\n");
        Status = mid_wwmgr_allocate_mem_for_rcvdatablks(StreamBlock, padapter);
      
        if (Status == MD_FAILURE)
          {
             MSD_LEVEL2_DBGPRINT("rcv_ww_process_data_buffer_request_from_adapter: Could not\
                          alloc the Rcv Strm Datablocks\n");
             return(MD_SUCCESS);
          }
     }

 if (StreamBlock->WWRcvStrDataBufPostMethod == MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED)
 {
   MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
   dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
   dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
   dmaDescSt.index = StreamBlock->id;
   Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
   pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
   StreamBlock->WWRcvStrDataBufPostPending = 1;
   MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
 }
else //MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE
  Status = mid_wwmgr_send_rcvstrm_datablocks_to_board(padapter, StreamBlock);

return (MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_read_request_complete_from_adapter
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : We Got the Rcv DATA DMA Complete from Board
 *                              :
 * Additional comments          : 
 *                              : LAYOUT of the this MF, Look at the following 
 *                              : MF HEADER:
 *                              :   
 *                              :   64             32                  0
 *                              :   -------------------------------------
 *                              :   |  streamId       |   mfType        |
 *                              :   -------------------------------------
 *                              :   |            Usr Context            |
 *                              :   -------------------------------------
 *                              :   |            StreamFlags            |
 *                              :   -------------------------------------
 *                              :   | bytesTransferred | sequenceNumber |
 *                              :   -------------------------------------
 *                              :
 **************************************************************************************/
merc_uint_t
rcv_ww_process_read_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmerc_uchar_t                     pUserContext;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  merc_ulong_t                     dmaDescIndex;
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  PSTRM_MSG                                  Msg;
#ifndef LFS
  PMDRV_MSG                                MdMsg;
  PMERC_HOSTIF_MSG                       MercMsg;
#endif
  mercd_ww_prep_mblk_train_sT          mbTrainSt;
  mercd_dhal_ww_rcv_rep_bigmsgdes_sT  RepDescrSt;

  merc_uint_t                           StreamId;
  merc_uint_t                        StreamFlags;
#ifdef LFS
  pmercd_ww_dma_descr_sT               pdmadescr = NULL;
#else
  pmercd_ww_dma_descr_sT               pdmadescr;
#endif
  pmercd_ww_dma_descr_sT              npdmadescr;
  pmercd_stream_connection_sT        StreamBlock;

  MSD_FUNCTION_TRACE("rcv_ww_process_read_request_complete_from_adapter 0x%x %s #%d\n",
                      (size_t)pmfAddrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;
  
  //i_bprintf(pmfHead, 128);

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
    {
      return(MD_SUCCESS);
    }

  //Get a Pointer to the user context following the Header
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);

 //Get the StreamId
  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress));

 //Get the StreamFlags
 pmfAddress += (sizeof(merc_uint_t));
 pmfAddress += (2 * sizeof(merc_uint_t));
 StreamFlags = *((pmerc_uint_t)(pmfAddress));

 //Check the sanity of StreamId
 if (StreamId >= MSD_MAX_STREAM_ID)
  {
    MSD_ERR_DBGPRINT("rcv_ww_process_read_request_complete_from_adapter:\
          StreamId: %d >= MSD_MAX_STREAM_ID for Adapater %d\n", 
          StreamId, padapter->adapternumber);
    return(MD_SUCCESS);
  }

  //Get the StreamBlock
  MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
  StreamBlock = padapter->pstream_connection_list[StreamId];
  MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

  if (StreamBlock == NULL)
   {
     MSD_ERR_DBGPRINT("rcv_ww_process_read_request_complete_from_adapter:\
          pstream_connection_list has NULL for StreamId: %d\n", StreamId);
     return(MD_SUCCESS);
   }

//#ifdef ABNORMAL_TERMINATION
 ////////////////////////////////////////////////////////////

 if (StreamBlock->pbind_block == NULL)
  {
   printk("read_request: BindBlock is NULL for this StreamBlock.\n");
   return(MD_SUCCESS);
  }

// else
//  if (StreamBlock->pbind_block->WWCancelStrmAckPending)
//   {
//    return(MD_SUCCESS);
//   }
////////////////////////////////////////////////////////////
//#endif /* ABNORMAL_TERMINATION */

  dmaDescIndex = (merc_ushort_t)(*((pmerc_uint_t)(pUserContext)));
  // Kernel Workaround: Null contexts were sent as 0xFF - need to decode
  if (dmaDescIndex == 0xFF) {
      dmaDescIndex = 0;
  }

  //Check the Sanity:
  //Check if this index is being marked as used in the
  //bitmap. This is an extreme case, but should never
  //be marked as freed.

  dmaDescSt.pBitMap = StreamBlock->pRcvBitMapValue;
  dmaDescSt.szBitMap = StreamBlock->szRcvBitMap;
  dmaDescSt.index = dmaDescIndex;

  MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
  Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
  if (Status == MD_FAILURE)
    {
      //Dump the BitMap if you need
      //i_bprintf(StreamBlock->pRcvBitMapValue, StreamBlock->szRcvBitMap);
      MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
      MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
      //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;

#if 0
      ///////////////////////////////////////////////////
      //test only...
      //////////////////////////////////////////////////
      {
        pmercd_ww_dma_descr_sT ptr = 0;
        ptr = StreamBlock->pRcvStrmDmaDesc[dmaDescIndex];

        if (ptr != NULL)
         {
           if (ptr->host_address) 
              MSD_LEVEL2_DBGPRINT("There is a Mblk at this dmadescindex..size: %d\n", ptr->size);
           if (ptr->board_address)
              MSD_LEVEL2_DBGPRINT("There is a board_address at this dmadescindex..add: 0x%x\n", 
                     ptr->board_address);
         }

      }
      //////////////////////////////////////////////////
#endif

      MSD_LEVEL2_DBGPRINT("%s #%d\n", __FILE__, __LINE__);
      MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
      return(MD_FAILURE);
    }
   MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);

   //Check to see if the FW is completing the request as a result
   // of a prior CANCEL STREAMS sent by us earlier. If so, this data
   //is invalid, so discard it

   ///////////////////////////////////////////////////////////////////
   //Now that the DMA Descriptor is Marked, get to it.
   if (!(StreamFlags & MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL))
    {
      pdmadescr = StreamBlock->pRcvStrmDmaDesc[dmaDescIndex];

      if (!pdmadescr)
      {
#ifdef LFS
        printk("rcv_ww_process_read_request_complete_from_adapter: pdmadescr is NULL at dmaDescIndex: %lu\n", dmaDescIndex);
#else
        printk("rcv_ww_process_read_request_complete_from_adapter: pdmadescr is NULL at dmaDescIndex: %d\n", dmaDescIndex);
#endif
        //MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
        //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
        //MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
        return(MD_FAILURE);
      }

     //New Phys Address
     //Do the pci_unmap before sending the dataup.
     if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
      {
#ifdef LFS
        pci_unmap_single(padapter->pdevi,
                        (dma_addr_t)(long)pdmadescr->board_address,
                        pdmadescr->size,
                        PCI_DMA_FROMDEVICE);
#else
        pci_unmap_single(padapter->pdevi,
                        pdmadescr->board_address,
                        pdmadescr->size,
                        PCI_DMA_FROMDEVICE);
#endif
      }
     else
      {
#ifdef LFS
        pci_unmap_single(padapter->pdevi,
                        (dma_addr_t) (long)pdmadescr->board_address,
                        pdmadescr->size,
                        PCI_DMA_FROMDEVICE);
            
        pci_unmap_single(padapter->pdevi,
                        (dma_addr_t) (long)pdmadescr->next->board_address,
                        pdmadescr->next->size,
                        PCI_DMA_FROMDEVICE);
#else
        pci_unmap_single(padapter->pdevi,
                        pdmadescr->board_address,
                        pdmadescr->size,
                        PCI_DMA_FROMDEVICE);
            
        pci_unmap_single(padapter->pdevi,
                        pdmadescr->next->board_address,
                        pdmadescr->next->size,
                        PCI_DMA_FROMDEVICE);
#endif
      }
    }
   else /* StreamFlags & MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL */
    {
      //Make this dmaDescIndex valid.
#ifdef LFS
       PSTRM_MSG tmpMsg = NULL;
#else
       PSTRM_MSG tmpMsg;
#endif

       //Even before this happened, wwmgr_abnormal routine would
       //have cleared the pRcvStrmDmaDesc memory

       if (StreamBlock->pRcvStrmDmaDesc)
          pdmadescr = StreamBlock->pRcvStrmDmaDesc[dmaDescIndex];
          
       if (pdmadescr) {
           tmpMsg = pdmadescr->host_address;
           //Do not set the pdmadescr->host_address or
	   //pdmadescr->board_address to zero. Let the
	   //abnormal routine take care of freeing them
	   //in close_streams
       }


       /////////////////////////////////////////////////////////////
       //New Sanity Checks
       ////////////////////////////////////////////////////////////
       if ( padapter->pstream_connection_list[StreamId] == NULL)
        {
          //printk("MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL\n 
                  //StreamBlock is already deallocated\n");
          return(MD_SUCCESS);
        }

        if ((StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND) ||
	   (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND))
        {
          //printk("MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL\n 
                  //StreamBlock state is CLOSE_PEND");
          return(MD_SUCCESS);
        }
       ////////////////////////////////////////////////////////////

       if (tmpMsg)
         {
           MSD_ZERO_MEMORY(tmpMsg->b_rptr,
                           (tmpMsg->b_wptr - tmpMsg->b_rptr));
           if (tmpMsg->b_cont)
             {
             MSD_ZERO_MEMORY(tmpMsg->b_cont->b_rptr, 
                            (tmpMsg->b_cont->b_wptr - tmpMsg->b_cont->b_rptr));
             }
           else
             {
                MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
                printk("MERCD_ADAPTER_STATE_OUT_OF_SERVICE\n");
 		printk("WW: MAOOS %s %d\n", __FILE__, __LINE__);
                padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
                MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
                return(MD_FAILURE);
              }
         }
       else
         { /* tmpMsg == NULL */
           //Already Deallocated
           return(MD_SUCCESS);
         }

         //For a BStream there is a next descriptor, do the sanity check
         if ((!(StreamBlock->type == STREAM_OPEN_F_GSTREAM))&&(pdmadescr))
           {
              if (!(pdmadescr->next))
               {
                MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
                padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
                printk("MERCD_ADAPTER_STATE_OUT_OF_SERVICE\n");
 		printk("WW: MAOOS %s %d\n", __FILE__, __LINE__);
                MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
                return(MD_FAILURE);
               }

           }

         //Mark this dmaDescrIndex as free and get out
         MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
         Status =  msgutl_ww_mark_dmadescr_free(&dmaDescSt);
         MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
         return(MD_SUCCESS);
   } /* MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL */
   

  //This msg has a b_cont; First mblk has the UserHdr
  //the next mblk on b_cont chain has the Data
  Msg = pdmadescr->host_address;

  //The following is needed to avoid some edge conditions
  pdmadescr->host_address = 0;
  pdmadescr->board_address = 0;

  // Mark all the host_addess as 0  
  npdmadescr = pdmadescr->next;
  while (npdmadescr) {
     npdmadescr->host_address = 0;
     npdmadescr->board_address = 0;
     npdmadescr = npdmadescr->next;
  }  

  mbTrainSt.pwwDevi = pwwDev;
#ifdef LFS
  mbTrainSt.pusrContext = (pmerc_uchar_t) pdmadescr;
#else
  mbTrainSt.pusrContext = pdmadescr;
#endif
  mbTrainSt.dataMblks= Msg;
  mbTrainSt.numContexts = 0;
  mbTrainSt.pmfHead = pmfHead;
  mbTrainSt.streamType = StreamBlock->type;                                             
  Status = rcv_ww_prepare_mblk_strm_train_to_sendup(&mbTrainSt);

  if (Status != MD_SUCCESS) {
     MSD_LEVEL2_DBGPRINT("rcv_ww_process_read_request_complete_from_adapter: Could not prepare mblk DATA train\n");
     MSD_FREE_MESSAGE(Msg);
     return(Status);
  }

  //Send the Receive Stream mblks up
  fwmgr_msg_reply_mgr(Msg, padapter);

  //Time to replenish the Rcv Stream Msg DMA descriptors with
  //Fresh memory at respective indexes. 
  //In FUTURE: If this does not work then we allocate the DMAble 
  //memory.

  RepDescrSt.pwwDevi = (pmerc_void_t)StreamBlock;
  RepDescrSt.index =  dmaDescIndex;
  RepDescrSt.purpose = MERCD_WW_STRMDATA_RCV_DESCR_ALLOC;

  //printk("Replenish dmaIndex: %d numContexts: %d\n", dmaDescIndex, numContexts);

  (*mercd_dhal_func[MERCD_DHAL_WW_REPLENISH_RCV_DESC])((pmerc_void_t)(&RepDescrSt));

  // prevent null descriptor from going to firmware
  if (RepDescrSt.ret != MD_SUCCESS) {
      printk("msdwwrcv: unable replenish rev descriptos\n");
      return (Status);
  }

  //Mark the corresponding bit in bitmap free
  dmaDescSt.index = dmaDescIndex;
  MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
  msgutl_ww_mark_dmadescr_free(&dmaDescSt);
   //++StreamBlock->free_descr_counter;
   //The following for debug purposes only
   //vRcvFreeDesc = StreamBlock->free_descr_counter
  MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);

  MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
  //Available MFs for the StreamBlock is more by 1
  StreamBlock->activeWWMfs--;
  MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);

 ////////////////////////////////////////////////////////////////////////
  if (StreamBlock->WWRcvStrDataBufPostMethod == MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED)
  {
    MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
    dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
    dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
    dmaDescSt.index = StreamBlock->id;
    Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
    pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
    StreamBlock->WWRcvStrDataBufPostPending = 1;
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
  }
else
 { //MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE
   MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
   if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT))
   {
     MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
     msgutl_ww_build_and_send_recv_stream_datablk_mf(padapter->pww_info,
                                                    StreamBlock);
   }
  else
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
 } //MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE
 ////////////////////////////////////////////////////////////////////////

 return (Status);
}

/************************************************************************************
 * Function Name                : rcv_ww_prepare_mblk_strm_train_to_sendup
 * Function Type                : Host FW Receive Support function for
 *                              : Rcv Data 
 *
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : This prepares an Rcv mblk DATA to Train to send up
************************************************************************************/
merc_int_t
rcv_ww_prepare_mblk_strm_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt)
{
#ifndef LFS
  MD_STATUS                               Status;
#endif
#ifdef LFS
  PSTRM_MSG                         Msg, DataMsg = NULL;
#else
  PSTRM_MSG                         Msg, DataMsg;
#endif
  pmercd_ww_dev_info_sT                   pwwDev;
#ifndef LFS
  PMERC_HOSTIF_MSG                       MercMsg;
  pmerc_uchar_t                     pUserContext;
#endif
  pmercd_ww_dma_descr_sT               pdmadescr;
  pmercd_adapter_block_sT               padapter;
  pmerc_uchar_t                          pmfHead;
  PMDRV_MSG                                MdMsg;
  PSTREAM_RECEIVE                            Ptr;
  merc_uint_t                         StreamType;
  merc_uint_t                           DataSize;
  
  //Look at the MF Layout
  merc_uint_t               streamIdOffset   = 4;  
  merc_uint_t              sequenceOffset   = 24;  
  merc_uint_t               bytesXferOffset = 28;  

  MSD_FUNCTION_TRACE("rcv_ww_prepare_mblk_data_train_to_sendup 0x%x %s #%d\n",
                      (size_t)pmbTrainSt, __FILE__, __LINE__);

  pwwDev = pmbTrainSt->pwwDevi;
  pdmadescr = (pmercd_ww_dma_descr_sT)pmbTrainSt->pusrContext;
  Msg = pmbTrainSt->dataMblks;
  pmfHead = pmbTrainSt->pmfHead;
  StreamType = pmbTrainSt->streamType;
  padapter = pwwDev->padapter;
  

 if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
     return(MD_SUCCESS);
   }

  //Check for the v bit. We are little overdoing
  //in error checking. This checking may not be
  //needed. We turn-off vbit only after sending the
  //Msg portion of the descriptor up. But, if the
  //following happens, then it is serious problem.
  //Know why ? (1) Either we read a wrong
  //dmaDescIndex from a corrupted MF, or (2) we never
  //allocated the Streams Msg or DMAble Mem at
  //dmaDescIndex, but posted the index to FW. But (2)
  //can be ruled out, since we never post an index
  //when Streams Msg or DMAble Memory allocation fails.
  //That leaves the possibility of a corrupted MF.
  //So no harm in checking the vbit

  if (!(pdmadescr->flag & MERCD_WW_DMA_DESCR_VBIT))
   {
     MSD_LEVEL2_DBGPRINT("rcv_ww_prepare_mblk_data_train_to_sendup:\
                         Got a Wrong DMA descriptor with vbit 0 flag: 0x%x", 
                         pdmadescr->flag);
     return(MD_FAILURE);
   }

  //For B Stream: Threre is a next descriptor
 if (pdmadescr->next)
   {
     //Do the same Check
     if (!(pdmadescr->flag & MERCD_WW_DMA_DESCR_VBIT))
      {
        MSD_LEVEL2_DBGPRINT("rcv_ww_prepare_mblk_data_train_to_sendup:\
                    Looks like a B Stream Descriptor: But,\
                    Got a Wrong Next DMA descriptor with vbit 0 flag: 0x%x\n",
                    pdmadescr->next->flag);
        return(MD_FAILURE);
      }
   }

  //Check the S-Bit: In future we could implement
  //a scheme with non-streams based memory for
  //DMA purpose. Adujust the Write pointer based
  //on the size of data that is DMAd
  if (pdmadescr->flag & MERCD_WW_DMA_DESCR_SBIT)
   {
     DataMsg = (PSTRM_MSG)(Msg->b_cont);

     //Get the total DATA DMAd
     DataSize = *((pmerc_uint_t)(pmfHead + bytesXferOffset));
     MSD_LEVEL2_DBGPRINT("Data DMAed: %d\n", DataSize);

     //Calculate the offset
     if (!(StreamType & STREAM_OPEN_F_GSTREAM))
      {
         DataSize -= sizeof(USER_HEADER);
         if (DataSize <= pdmadescr->next->size) {
             MD_SET_MSG_WRITE_PTR(DataMsg, MD_GET_MSG_READ_PTR(DataMsg) + DataSize);
         } else {
	     /* DataSize > Allowed Size */
             MSD_LEVEL2_DBGPRINT("BSTREAM: The size of DATA DMAd is More than ALLOWED: %d\n", DataSize);
         }
      }
     else
      { /* GSTREAM */

         //No user header for GStream
         if (DataSize <= pdmadescr->size) 
         {
          MD_SET_MSG_WRITE_PTR(DataMsg, MD_GET_MSG_READ_PTR(DataMsg) + DataSize);
         } 
        else
         { /* DataSize > Allowed Size */
           MSD_LEVEL2_DBGPRINT("GSTREAM: The size of DATA DMAd is More than ALLOWED: %d\n", DataSize);
         }
      }
   }
  else
   { /* not a streams message */
    ////////////////////////////////////////////////////
    //This is not implemented yet
    ////////////////////////////////////////////////////
   } /* SBIT */

  //Set the protocol specific information
  MD_SET_MSG_TYPE(DataMsg,M_DATA);
  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
  MD_SET_MDMSG_ID(MdMsg, MID_STREAM_RECEIVE);
  Ptr = (PSTREAM_RECEIVE)MD_GET_MDMSG_PAYLOAD(MdMsg);
  Ptr->StreamId = *((pmerc_uint_t)(pmfHead + streamIdOffset));
  Ptr->Flags = 0;
  Ptr->Sequence = *((pmerc_uint_t)(pmfHead + sequenceOffset));
 
  MSD_LEVEL2_DBGPRINT("read request complete: sequencenum: %d\n", Ptr->Sequence);

#if 0
  ////////////////////////////////////////////////////////////////////////
  //The following is useful for Debug: This dumps the 24 byte UserHeader
  //which is part of STREAM_RECEIVE in the first Mblk. If the flag
  //is non zero, it could be that, there is EOS flag with the datablock.
  ///////////////////////////////////////////////////////////////////////
  {
    pmerc_uchar_t ptr;
    merc_uchar_t flag;
    ptr = &Ptr->StreamUserHeader.UserHeaderBytes[0];
    flag = (*(pmerc_uchar_t)(ptr+4));
     if (flag)
     {
       //printk("flag: Non zero : 0x%x\n", flag);
       //i_bprintf(ptr, 24);
     }
  }
  ////////////////////////////////////////////////////////////////////////
#endif

 return (MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_eos_read_stream_from_adapter
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : We got End of Read Streams MF from the Board
 *                              :
 * Additional comments          : 
 *                              :
 *                              : 
*************************************************************************************/
merc_int_t
rcv_ww_process_eos_read_stream_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  merc_ulong_t                           mfindex;
  pmerc_uchar_t                       pmfAddress;
  pmerc_uchar_t                          pmfHead;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  PSTRM_MSG                         Msg, DataMsg;
  PMDRV_MSG                                MdMsg;

  merc_uint_t                           StreamId;
  pmercd_ww_dma_descr_sT               pdmadescr;
  pmercd_ww_dma_descr_sT              npdmadescr;
  pmercd_stream_connection_sT        StreamBlock;
  PSTREAM_SESS_CLOSE_ACK                ClosePtr;
  mercd_ww_dmaDscr_Index_sT      dmaDescSt = {0};
  merc_uint_t             i, index, numOfContext;
#ifndef LFS
  mercd_dhal_ww_dealloc_dmadescr_sT    descrFree;
#endif
  merc_int_t             count = 0, prevSize = 0;

  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  mfindex = pmfAddrSt->mfIndex;
  pmfHead = pmfAddress = pmfAddrSt->pmfAddress;

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
      return(MD_SUCCESS);
   }

  //Get the StreamId
  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress));

  // Get the number of context
  pmfAddress += (sizeof(merc_uint_t));
  numOfContext = *((pmerc_uint_t)(pmfAddress));

  MSD_LEVEL2_DBGPRINT("rcv_ww_process_eos_read_stream_from_adapter: StreamId: %d\n", StreamId);

  //Check the sanity of StreamId
  if (StreamId >= MSD_MAX_STREAM_ID)
   {
     MSD_ERR_DBGPRINT("rcv_ww_process_eos_read_stream_from_adapter: StreamId: %d >= MSD_MAX_STREAM_ID for Adapater %d\n",
          StreamId, padapter->adapternumber);
     return(MD_SUCCESS);
   }

  //Get the StreamBlock
  MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
  StreamBlock = padapter->pstream_connection_list[StreamId];
  MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

  if (StreamBlock == NULL)
   {
     MSD_ERR_DBGPRINT("rcv_ww_process_eos_read_stream_from_adapter: pstream_connection_list has NULL for StreamId: %d\n", StreamId);
     return(MD_SUCCESS);
   }

  if (StreamBlock->pWWReadEosMsg)
   {
    Msg = StreamBlock->pWWReadEosMsg;
    StreamBlock->pWWReadEosMsg = NULL;
   }
  else
   {
    MSD_LEVEL2_DBGPRINT("Eos alread Recvd for this StreamBlock. One more???..\n");
    return(MD_SUCCESS);
   }
 
  //Turn off the Read Stream DATABUF MF Bit, so timer would not post
  //any DATABUFs
  MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
  dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
  dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
  dmaDescSt.index = StreamBlock->id;
  Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
  StreamBlock->WWRcvStrDataBufPostPending = 0;
  MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

  if (numOfContext > 0) {
      for(i=0; i<numOfContext; i++) {
          // Get to the first context
          pmfAddress += (2 * sizeof(merc_uint_t));

          index = (merc_uint_t)*pmfAddress;
          if (index == 0xFF)
              index = 0;

          dmaDescSt.pBitMap = StreamBlock->pRcvBitMapValue;
          dmaDescSt.szBitMap = StreamBlock->szRcvBitMap;
          dmaDescSt.index = index;

          Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
          if (Status == MD_FAILURE)
              continue;

          pdmadescr = StreamBlock->pRcvStrmDmaDesc[index];
          if (pdmadescr == NULL) {
              MSD_LEVEL2_DBGPRINT("EOS Stream %d. Index %d used but DMA DESCR NULL \n", StreamBlock->id, i);
              continue;
          }

          count = 0;
          npdmadescr = pdmadescr->next;
          prevSize = pdmadescr->size;

          while (npdmadescr != NULL)  {
             if (count > 0)
                MSD_LEVEL2_DBGPRINT(" EOS. Size = %d,p Size %d Count = %d, PD Size %d \n",
                                       npdmadescr->size,prevSize, count, pdmadescr->size);

#ifdef LFS
             pci_unmap_single(padapter->pdevi, (dma_addr_t) (long)npdmadescr->board_address,
                                   npdmadescr->size, PCI_DMA_FROMDEVICE);
#else
             pci_unmap_single(padapter->pdevi, npdmadescr->board_address,
                                   npdmadescr->size, PCI_DMA_FROMDEVICE);
#endif

             if (npdmadescr->host_address) {
                 npdmadescr->host_address = 0;
             }

             npdmadescr->board_address = 0;
             prevSize = npdmadescr->size;
             npdmadescr = npdmadescr->next;
             count++;
          } 
         
          if (count > 1) {
              MSD_LEVEL2_DBGPRINT("EOS - how can this be. %d \n", count);
          } 

#ifdef LFS
          pci_unmap_single(padapter->pdevi, (dma_addr_t) (long)pdmadescr->board_address,
                            pdmadescr->size, PCI_DMA_FROMDEVICE);
#else
          pci_unmap_single(padapter->pdevi, pdmadescr->board_address,
                            pdmadescr->size, PCI_DMA_FROMDEVICE);
#endif
  
          if (pdmadescr->host_address) 
	      MSD_FREE_MESSAGE(pdmadescr->host_address);

          pdmadescr->host_address = 0;
          pdmadescr->board_address = 0;

          Status =  msgutl_ww_mark_dmadescr_free(&dmaDescSt);
          if (Status == MD_FAILURE) {
              MSD_LEVEL2_DBGPRINT(" On EOS - mark free failed \n");
          }
      }
  }

  DataMsg = Msg->b_cont;
  if (DataMsg != NULL) {
      MD_SET_MSG_TYPE(DataMsg, M_DATA);

      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
      MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
      MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);
      MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg) +sizeof(MDRV_MSG)
                       +sizeof(STREAM_SESS_CLOSE_ACK)));
      ClosePtr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
      ClosePtr->StreamId = StreamId;

      fwmgr_msg_reply_mgr(Msg, padapter);
  } else {
      MSD_LEVEL2_DBGPRINT(" EOS BCONT NULL for %d of type %d \n",
                                    StreamBlock->id, StreamBlock->type);
  }

  return(MD_SUCCESS);
}

/************************************************************************************
 * Function Name                : rcv_ww_process_stream_session_close
 * Function Type                : Host FW Receive function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : We got End of Write Streams MF from the Board
 *                              :
 * Additional comments          : 
 *                              :
 *************************************************************************************/
merc_int_t
rcv_ww_process_stream_session_close(pmercd_ww_get_mfAddress_sT pmfAddrSt)
{
  MD_STATUS                               Status;
  pmerc_uchar_t                       pmfAddress;
  pmercd_adapter_block_sT               padapter;
  pmercd_ww_dev_info_sT                   pwwDev;
  PSTRM_MSG                                  Msg;
  PSTRM_MSG                              DataMsg;
  PMDRV_MSG                                MdMsg;
  PMERC_HOSTIF_MSG                       MercMsg;
  merc_uint_t                           StreamId;
  PSTREAM_SESS_CLOSE_ACK                ClosePtr;
  PHIF_CLOSE_STREAM               CloseStreamMsg;
  pmercd_open_block_sT          OpenBlockContext;
  pmercd_bind_block_sT                 BindBlock;
  pmercd_stream_connection_sT        StreamBlock;
#ifdef LFS
  pmercd_ww_dma_descr_sT               pdmaDescr, npdmaDescr;
#else
  mercd_dhal_ww_free_desc_sT          freeDescSt;
  pmercd_ww_dma_descr_sT               *resource;
  pmercd_ww_dma_descr_sT               pdmaDescr, npdmaDescr, nnpdmaDescr;
#endif
  mercd_ww_dmaDscr_Index_sT            dmaDescSt;
  merc_ushort_t                     dmaDescIndex;
  pmerc_uchar_t                     pUserContext;
  merc_uint_t                              count;
#ifndef LFS
  merc_ulong_t                          numDescr;
#endif
  merc_int_t			 i, prevSize = 0;


  Status = MD_SUCCESS;
  pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddrSt->pwwDevi));
  padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);
  pmfAddress = pmfAddrSt->pmfAddress;
  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);
  dmaDescIndex = *((pmerc_uint_t)(pUserContext));

  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address))) {
      return(MD_SUCCESS);
  }

  pUserContext = (pmfAddress + MERCD_WW_MF_HEADER_SIZE);
  pmfAddress += (sizeof(merc_uint_t));
  StreamId = *((pmerc_uint_t)(pmfAddress));

  // Check the sanity of StreamId
  if (StreamId >= MSD_MAX_STREAM_ID) {
     MSD_ERR_DBGPRINT("rcv_ww_process_stream_session_close: ");
     MSD_ERR_DBGPRINT("StreamId: %d >= MSD_MAX_STREAM_ID for Adapater %d\n",
          				 StreamId, padapter->adapternumber);
     return(MD_SUCCESS);
  }

  // Get the StreamBlock
  MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
  StreamBlock = padapter->pstream_connection_list[StreamId];
  MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

  if (StreamBlock == NULL) {
     printk("rcv_ww_process_stream_session_close:");
     printk("pstream_connection_list has NULL for StreamId: %d\n", StreamId);
     return(MD_SUCCESS);
  }

  // HCS: on an internal close we need to send a RESET_PROPERTY to FW
  if ((StreamBlock->isStreamHCS) && (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND)) {
      Msg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
      if (Msg == NULL) {
          printk("rcv_ww_process_stream_session_close: allocbuf failed\n");
          return (MD_FAILURE);
      }

      DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+sizeof(HIF_CLOSE_STREAM),GFP_ATOMIC);
      if (DataMsg == NULL) {
          printk("rcv_ww_process_stream_session_close: Cannot alloc Data buffer\n");
          mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
          return (MD_FAILURE);
      }

      MD_SET_MSG_TYPE(DataMsg, M_DATA);
      MSD_LINK_MESSAGE(Msg,DataMsg);

      MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
      MERCURY_SET_BODY_SIZE(MercMsg,sizeof(HIF_CLOSE_STREAM));

      MERCURY_SET_MESSAGE_FLAG(MercMsg,MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER);
      MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);
      MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_RESET_STREAM_PROPERTY);

      while(strm_fnd_entry_from_adapter(padapter,padapter->TransactionId))
                  padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

      MERCURY_SET_TRANSACTION_ID(MercMsg,padapter->TransactionId);
      padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

      MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
      MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
      MERCURY_SET_DEST_COMPINST(MercMsg,DEFAULT_DEST_COMPINST);
      MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);
      CloseStreamMsg = (PHIF_CLOSE_STREAM)MERCURY_BODY_START(MercMsg);
      CloseStreamMsg->StreamId = StreamBlock->id;

      MSD_LEVEL2_DBGPRINT("HCS: on EOS via internal close %d\n", StreamBlock->id);
      mid_strmmgr_strm_close_hcs(padapter, StreamBlock, Msg);

      return(MD_SUCCESS);
  }

  if (StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) {
      // new latency improvement already send a broken stream up
      return(MD_SUCCESS);
  }

  // If it is already broken, don't send broken back to the user again
  if (StreamBlock->state != MERCD_STREAM_STATE_BROKEN) {

     // Send a Broken Stream UP
     Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
     if(Msg == NULL) {
        printk("rcv_ww_process_stream_session_close: Cannot alloc buffer\n");
        return(MD_SUCCESS);
     }

     Msg->b_cont=NULL;

     MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
     MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
     MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);
     MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg) +sizeof(MDRV_MSG)
                                        +sizeof(STREAM_SESS_CLOSE_ACK)));
     ClosePtr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
     ClosePtr->StreamId = StreamId;

#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
     if (StreamBlock->WWMode) {
         Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
         if (Status == MD_FAILURE) {
             printk("Abnormal Termination in FAILED in rcv_ww_process_stream_session_close\n");
             MSD_FREE_MESSAGE(Msg);
             return (MD_FAILURE);
         }
     }
#endif

     BindBlock = StreamBlock->pbind_block;
     MSD_ASSERT(BindBlock);

     MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
     OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);
  
     // must set bind handle and user context in msg
     MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
     MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindBlock->bindhandle);
     MD_SET_MDMSG_USER_CONTEXT(MdMsg, BindBlock->UserContext);
  
     StreamBlock->state = MERCD_STREAM_STATE_BROKEN;

     // Send to user
     supp_process_receive(OpenBlockContext, Msg);
  }

  dmaDescSt.pBitMap = StreamBlock->pSendBitMapValue;
  dmaDescSt.szBitMap = StreamBlock->szSendBitMap;

  // Need to mark all the dma descriptors as free also
  MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);

  for (i=0; i<StreamBlock->szSendBitMap; i++) {
       // Check if this index is being marked as used in the bitmap. 
       // This is an extreme case, but should never be marked as freed.
       dmaDescSt.index = i;
       Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
       if (Status == MD_FAILURE)
           continue;

       pdmaDescr = StreamBlock->pSendStrmDmaDesc[i];
       npdmaDescr = pdmaDescr->next;
       count =0;
       prevSize = pdmaDescr->size;
       while (npdmaDescr != NULL) {
          count++;

          if (count > 1) {
              MSD_LEVEL2_DBGPRINT("Session Close. Count %d,C size %d, P Size %d \n",
                                              count, npdmaDescr->size, prevSize);
          }

          npdmaDescr->host_address = 0;
          if (npdmaDescr->board_address) {
#ifdef LFS
              pci_unmap_single(padapter->pdevi, (dma_addr_t)(long)npdmaDescr->board_address,
                                                  npdmaDescr->size, PCI_DMA_TODEVICE);
#else
              pci_unmap_single(padapter->pdevi, (dma_addr_t)npdmaDescr->board_address,
                                                  npdmaDescr->size, PCI_DMA_TODEVICE);
#endif
              npdmaDescr->board_address = 0;
          }
            
          prevSize = npdmaDescr->size;
          npdmaDescr = npdmaDescr->next;
       }
   
       if (count > 1) {
           MSD_LEVEL2_DBGPRINT("Session close - how can this be. %d \n", count);
       }

       if (pdmaDescr->host_address) {
           MSD_FREE_MESSAGE(pdmaDescr->host_address);
           pdmaDescr->host_address = 0;
       }

       if (pdmaDescr->board_address) {
#ifdef LFS
           pci_unmap_single(padapter->pdevi, (dma_addr_t)(long)pdmaDescr->board_address,
                                               pdmaDescr->size, PCI_DMA_TODEVICE);
#else
           pci_unmap_single(padapter->pdevi, (dma_addr_t)pdmaDescr->board_address,
                                               pdmaDescr->size, PCI_DMA_TODEVICE);
#endif
           pdmaDescr->board_address = 0;
       }

       Status =  msgutl_ww_mark_dmadescr_free(&dmaDescSt);
       if (Status == MD_FAILURE) {
           printk("Free failed on Session for id %d \n", StreamBlock->id);
       }
  }

  MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);

  // free any messages on send queue for Write stream : NEW  ###########
  if ((StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) && (StreamBlock->SendStreamQueue)) {
       strm_free_only_data_msgs(StreamBlock);
  }
  MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

  return(MD_SUCCESS);
}
