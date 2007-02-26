/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwsnd.c
 * Description                  : WW To Board Messages functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDWWSND_C_
#include "msdextern.h"
#undef _MSDWWSND_C_




/***************************************************************************
 * Function Name                : snd_ww_msgs2adapter
 * Function Type                : Host FW Send function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver snd_ww_small_msgs2adapter Routine.
 *                                This routine sends small message to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_msgs2adapter(pmercd_adapter_block_sT padapter, merc_uint_t sndlimit)
{
  md_status_t                Status;
  pmercd_ww_dev_info_sT      pwwDev;
  merc_uint_t              msgClass;
  mercd_ww_get_mfClass_sT  mClassSt;
  PSTRM_MSG                     Msg;
  PMDRV_MSG                   MdMsg;


  MSD_FUNCTION_TRACE("snd_ww_msgs2adapter 0x%x 0x%x %s #%d\n",
                      (size_t)padapter, (size_t) sndlimit, __FILE__, __LINE__);

  Status = MD_SUCCESS;

  // First, find if this is a SMALL/BIG Message and try to send it
  // immediately by reading a MFA. If there are no more MFAs
  // then put it on snd_msg_queue, and start the timer
  pwwDev = padapter->pww_info;
  mClassSt.msgSize = pwwDev->pww_param->sizeofMFAInboundQ;

  Msg= queue_get_msg_Array(&padapter->snd_msg_Array, MSD_SENDER); 
  while ((Msg) != NULL)
  {

    if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
        (!(pwwDev->mfaBaseAddressDescr.host_address)))
     {
        return (MD_SUCCESS);
     }

    //Check if sndlimit flag says we need to send
    //only flush messages
    if (sndlimit == MERCD_ADAPTER_WW_SEND_MSG_FLUSH_ONLY) {
       MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
       if (MdMsg->MessageId != STREAM_CLOSE_FLAG_FLUSH_ALL)
          {
            //This message is not a flush message. So
            //put back at the head of Q, and break.
            break;
          }
    }

    mClassSt.mb = (pmerc_void_t)Msg;
    Status = msgutl_ww_determine_message_class(&mClassSt);

    if (Status == MD_FAILURE) {
        break;
    }

    msgClass = mClassSt.msgClass;


    switch(msgClass) {
       case MERCD_WW_SMALL_MESSAGE:
                    Status = snd_ww_small_msgs2adapter(padapter, Msg);
                    if (Status == MD_SUCCESS)
                      {
                        //We have successfully sent the Msg
                        //So Get Rid of the Whole Msg
                        //atomic_inc(&padapter->pww_info->pww_counters->small_msgs_sent);
                        MSD_FREE_MESSAGE(Msg);
                      }
                     else
                      {

                        //Send Has Failed. Hence we would have
                        //left the message on padapter->snd_msg_queue
                        //for the timer to take care
                        //So, do nothing

                      }
                     break;

      case MERCD_WW_BIG_MESSAGE:
                    Status = snd_ww_big_msgs2adapter(padapter, Msg);
                    if (Status == MD_SUCCESS)
                     {
                        //We have successfully sent the Msg Frame
                        //with all the dmac addresses of the Msg
                        //data blocks. We can only getrid of it after
                        //receiving a read completion interrupt
                        //atomic_inc(&padapter->pww_info->pww_counters->big_msgs_sent);
                      }
                    else
                      {
                        //Send Has Failed. Hence we would have
                        //left the message on padapter->snd_msg_queue
                        //for the timer to take care
                        //So, do nothing
                      }

                     break;
     }

     if (Status == MD_FAILURE)
       {
         //Get out of the while loop and wait
         //till the timer kicks off
         break;
       }

     queue_commit_msg_Array(&padapter->snd_msg_Array, MSD_SENDER);
     Msg= queue_get_msg_Array(&padapter->snd_msg_Array, MSD_SENDER);
  } /* while */

 return (Status = MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : snd_ww_small_msgs2adapter
 * Function Type                : Host FW Send function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver snd_ww_small_msgs2adapter Routine.
 *                                This routine sends small message to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_small_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{

 PMDRV_MSG                  MdMsg;
 MD_STATUS                  Status;
 merc_ulong_t               mfindex;
 mercd_ww_get_mfAddress_sT  mfAddressST;
 pmerc_uchar_t              pmfAddress;
 pmercd_ww_dev_info_sT      pwwDev;
 mercd_ww_build_smallMF_sT  smallSt;
#ifndef LFS
 merc_ulong_t               syncOffset;
 merc_ulong_t               syncSize;
#endif


 MSD_FUNCTION_TRACE("snd_ww_small_msgs2adapter 0x%x 0x%x %s #%d",
                     (size_t)padapter, (size_t) Msg, __FILE__, __LINE__);

 Status = MD_SUCCESS;
 pwwDev = padapter->pww_info;

 MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

 //Get an MFA from Board. If you can not get one, FAIL, so that
 //the message is appeneded to the pending send Q

   mfAddressST.pwwDevi = pwwDev;
   mfAddressST.pmfAddress = NULL;

   Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST);

 //We need to handle a situation when there are
 //no MFAs available from PLX. In case of no MFAs,
 //leave the Message back at the head of padapter's
 //snd_msg_queue and let the timer taker of its
 //sending after obtaining a MFA

    if (Status != MD_SUCCESS)
      {
        MSD_LEVEL2_DBGPRINT("snd_ww_small_msgs2adapterFailed Failed \n");

        //Still Need to return MD_FAILURE since strm_ww_Q_snd_msg
        //should not Free the Message
        Status = MD_FAILURE;
#if 0
        //Put the Message Back at the head of  padapter's snd_msg_queue
        //WWPENDING: What is threshold on this Q? Need to experiment
        //with some perf bench marking

        if (MdMsg->MessageId == STREAM_CLOSE_FLAG_FLUSH_ALL)
        {
          MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
          if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT))
             pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT);
          MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
        }
       else
        {
          MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
          if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT))
             pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT);
          MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
        }

        //MSD_ENTER_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */
        queue_put_bk_msg_Q(&padapter->snd_msg_queue, Msg);
        //MSD_EXIT_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */
#endif
        return(Status);
     }

   //Store the returned MF address and index
    pmfAddress = mfAddressST.pmfAddress;
    mfindex   = mfAddressST.mfIndex;


   //Since we got a MFA, build a small MF
   if ((MERCURY_GET_TRANSACTION_ID(((PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr))) == 0x800000) && 
									 !padapter->rtkMode)
       padapter->sndflowcount++;

   smallSt.mb = (pmerc_void_t)((Msg)->b_cont);
   smallSt.pmfAddress = pmfAddress;
   Status = msgutl_ww_build_smallmsg_mf(&smallSt);

#ifndef MERCD_LINUX
   //Sync the view of all caches with respect to the device
   syncOffset = ((pwwDev->inBoundQMfAddress) +
                (mfindex * pwwDev->pww_param->sizeofMFAInboundQ));
   syncSize   = pwwDev->pww_param->sizeofMFAInboundQ;
   MsdWWSyncDevice(pwwDev);
#endif

   //Write MFA to InBoundQ
   MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
   MsdWWPutinBoundQReg(padapter, mfindex);
   MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

   //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);

   //Trace Code (SMALL Msg): Copy the Message to the buffer (if required)
   if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
      supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
   }

   //We are done sending a small msg MF
 return(Status);
}

/***************************************************************************
 * Function Name                : snd_ww_big_msgs2adapter
 * Function Type                : Host FW Send function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver snd_ww_big_msgs2adapter Routine.
 *                                This routine sends big message to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_big_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{

#ifndef LFS
 PMDRV_MSG                  MdMsg;
#endif
 MD_STATUS                  Status;
 merc_ulong_t               mfindex;
 mercd_ww_get_mfAddress_sT  mfAddressST;
 pmerc_uchar_t              pmfAddress;
 pmercd_ww_dev_info_sT      pwwDev;
 mercd_ww_build_bigMF_sT    bigSt;
#ifndef LFS
 merc_ulong_t               syncOffset;
 merc_ulong_t               syncSize;
 merc_ulong_t               newMFneeded = 1;
#endif
 merc_uint_t                dataSize;
 merc_uint_t                numBlks;
 mercd_ww_mblk_train_info_sT mblkTrain;
 mercd_ww_dmaDscr_Index_sT  dmaDescSt;



 MSD_FUNCTION_TRACE("snd_ww_big_msgs2adapter 0x%x 0x%x %s #%d\n",
                     (size_t)padapter, (size_t) Msg, __FILE__, __LINE__);
 Status = MD_SUCCESS;
 pwwDev = padapter->pww_info;

 dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
 dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
 MSD_ENTER_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);
 Status = msgutl_ww_check_free_dmadescr(&dmaDescSt);
 MSD_EXIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);

 if (Status != MD_SUCCESS) {
     Status = MD_FAILURE;
     return(Status);
 }

 mfAddressST.pwwDevi = pwwDev;
 mfAddressST.pmfAddress = NULL;

 //Get an MFA

 Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST);



 //We need to handle a situation when there
 //are no MFAs available from PLX.  In case
 //of no MFAs, leave the Message back at the
 //head of padapter's snd_msg_queue and let
 //the timer takes care of it after obtaining
 //a MFA

 if (Status != MD_SUCCESS)
  {
    MSD_LEVEL2_DBGPRINT("snd_ww_big_msgs2adapter Failed\n");


    //Still Need to return MD_FAILURE since strm_ww_Q_snd_msg
    //should not Free the Message
    Status = MD_FAILURE;
#if 0
    //Put the Message Back at the head of  padapter's snd_msg_queue
    //WWPENDING: What is threshold on this Q? Need to experiment
    //with some perf bench marking
    MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
    if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT))
       pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT);
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

    //MSD_ENTER_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */
    queue_put_bk_msg_Q(&padapter->snd_msg_queue, Msg);
    //MSD_EXIT_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */
#endif
    //Start timer if not yet started

     return(Status);
  }

    //Store the returned MF address and index
     pmfAddress = mfAddressST.pmfAddress;
     mfindex   = mfAddressST.mfIndex;


    //Calculate the number of mblks linked by b_cont starting
    //from the 2nd mblk. All these have their own descriptor '
    //mercd_ww_dma_descr_sT' linked to each other by their
    //next pointer, but share the same index 'dmaDescIndex' in
    //the descriptor table


      mblkTrain.mb = ((pmerc_void_t)(Msg->b_cont));
      mblkTrain.dataSize = 0;
      mblkTrain.mblkCount = 0;
    strm_ww_get_mblk_train_info(&mblkTrain);


    //WWPENDING: If number of mblks on b_cont chain > 3, let's FAIL this.
    //If each mblk crosses one page boundary, then we will have min 6
    //descritptors. NOTE: This is w.r.t sending side ONLY
    //But, the descriptor space available on a MF is 112 bytes,
    //which means it can accommodate a maximum of 7 descriptors, where
    //each descriptor needs 16bytes. We need to address this issue in
    //FUTURE when a MF needs to address more than 7 descriptors.

     dataSize = mblkTrain.dataSize;
     numBlks = mblkTrain.mblkCount;

     if (numBlks > 3)
     {
      printk("snd_ww_big_msgs2adapter: Can not Handle %d > 3mblks (dropping msg)\n", numBlks);
      //For now, lets just drop the Msg by returning a success.
      Status = MD_SUCCESS;
      return (Status);
     }

    //The following is for debug purposes
    if (numBlks > 1)
      MSD_LEVEL2_DBGPRINT("msgutl_ww_build_bigmsg_mf: Num of mblks > 1 %s #%d\n", __FILE__, __LINE__);


   //Since we got a MFA, build a big MF
   bigSt.pwwDevi = pwwDev;
   bigSt.mb = (pmerc_void_t)(Msg);
   bigSt.pmfAddress = pmfAddress;


   Status = msgutl_ww_build_bigmsg_mf(&bigSt);

   //There could be a problem with descriptor allocation or
   //finding a free descritptor on the current descritptor
   //table, so leave the message on padapter->snd_msg_queue
   //and let the timer takes care of that in case of MD_FAILURE
   if (Status == MD_FAILURE)
   {
     MSD_LEVEL2_DBGPRINT("snd_ww_big_msgs2adapter Failed on DMA descriptor alloc %s #%d\n", __FILE__, __LINE__);

     //Still Need to return MD_FAILURE since strm_ww_Q_snd_msg
     //should not Free the Message
     Status = MD_FAILURE;
#if 0
     //Put the Message Back at the head of  padapter's snd_msg_queue
     //WWPENDING: What is threshold on this Q? Need to experiment
     //with some perf bench marking
     //MSD_ENTER_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */
     queue_put_bk_msg_Q(&padapter->snd_msg_queue, Msg);
     //MSD_EXIT_MUTEX(&padapter->snd_msg_mutex); /* 10.02.2002 */

     //Start timer if not yet started
    MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
    if (!(pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT))
       pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT);
    MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
#endif
     return(Status);
   }

#ifndef MERCD_LINUX
   //Sync the view of all caches with respect to the device
   syncOffset = ((pwwDev->inBoundQMfAddress) +
                (mfindex * pwwDev->pww_param->sizeofMFAInboundQ));
   syncSize   = pwwDev->pww_param->sizeofMFAInboundQ;

   MsdWWSyncDevice(pwwDev);
#endif

   //Write MFA to InBoundQ
   MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
   MsdWWPutinBoundQReg(padapter, mfindex);
   MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

   //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
   //Trace Code (BIG Msg): Copy the Message to the buffer (if required)
   if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
      supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
   }

   //We are done sending a big msg MF
 return(Status);
}

/***************************************************************************
 * Function Name                : snd_ww_data2adapter
 * Function Type                : WW Send Data function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW Driver snd_data2adapter Routine.
 *                                This routine sends data to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_data2adapter(pmercd_adapter_block_sT padapter)
{
 pmercd_stream_connection_sT  StreamBlock;
 pmercd_stream_connection_sT  tmpStreamBlock;
 PSTRM_MSG                            Msg;
#ifndef LFS
 PSTRM_MSG                         TmpMsg;
#endif
 PMDRV_MSG                          MdMsg;
 PSTREAM_SEND                         Ptr;
#ifndef LFS
 merc_uint_t                  MsgDataSize;
#endif
 mercd_ww_send_streamData    StreamDataSt;
 merc_uint_t stream_blk_snd_status = MD_SUCCESS;
 MD_STATUS Status = MD_SUCCESS;

 MSD_FUNCTION_TRACE("snd_ww_data2adapter 0x%x 0x%x %s #%d", 
                             (size_t) padapter, __FILE__, __LINE__);

 //Sanity checking
 MSD_ASSERT(padapter);
 if (!(padapter->flags.SendDataPending & MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND))
  {
    MSD_LEVEL2_DBGPRINT("snd_ww_data2adapter is SCHEDULED, But SendDataPending is not SET\n");
    cmn_err(CE_WARN, "Adapter Out of Service(snd_ww_data2adapter)\n");
    padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
    return(MD_FAILURE);
  }
   
  StreamBlock = ((pmercd_stream_connection_sT)
                 padapter->stream_adapter_list.QueueHead);

  //Since snd_ww_data2adapter is scheduled...
  //MSD_ASSERT(StreamBlock);

  while (StreamBlock)
   {
       //If the StreamBlock is opened for RECEIVE_ONLY..
       if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
        {
          StreamBlock = StreamBlock->Next;
          continue;
        }

        if (StreamBlock->SendStreamQueue== NULL) {
           MSD_LEVEL2_DBGPRINT("StreamBlock SendStreamQueue NULL\n");
           StreamBlock = StreamBlock->Next;
           continue;
        }

	//Send the data down for this StreamBlock
        //But, check if there are any MFs for this
        //Stream Available
	
        //Check for EOS - can be sent on another MF
        Msg = StreamBlock->SendStreamQueue;
        MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

        if (StreamBlock->activeWWMfs >= StreamBlock->maxWWMfs)
         {
            if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
                //No MFs for the current StreamBlock. Go to Next...
                StreamBlock = StreamBlock->Next;
                continue;
            }
	    //This block contains EOS so send even is active MF is maxed out
         }

        //Check if we are in the MERCD_STREAM_STATE_CLOSE_PEND state. In
        //this case, we need to free back the memory back to the user
        if ((StreamBlock->state ==  MERCD_STREAM_STATE_CLOSE_PEND) ||
	    (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND)) {
            if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
                strm_free_only_data_msgs(StreamBlock);
            }
  
            //Process the EOS now ..
            Msg = StreamBlock->SendStreamQueue;
            if (Msg != NULL) {
                MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
                Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
                if (!(Ptr->Flags & STREAM_FLAG_EOS))
                    MSD_LEVEL2_DBGPRINT(" How can this be. No EOS %d \n", StreamBlock->id);
            } else {
                MSD_LEVEL2_DBGPRINT(" Did not find EOS with CLOSE_PEND \n");
                StreamBlock = StreamBlock->Next;
                continue;
            }
        } 


        StreamDataSt.dataCount = 0; 
	StreamDataSt.padapter = (pmerc_void_t)padapter;
	StreamDataSt.StreamBlock = (pmerc_void_t)StreamBlock;
	Status = snd_ww_strm_data2adapter(&StreamDataSt);

        if ((Status == MD_SUCCESS) && (StreamBlock->SendStreamQueue == NULL)) {
            if (StreamBlock->can_take >= StreamDataSt.dataCount)
                StreamBlock->can_take -= StreamDataSt.dataCount;
            StreamBlock->qparam.cur_snd_cnt -= StreamDataSt.dataCount;
        } else {
            //Send has failed for this StreamBlock, Hence
            //left the rest of messages on StreamBlock->SendStreamQueue,
            //for the timer to take care. We have to just mark it
            //to start the timer after we are done.
            stream_blk_snd_status = MD_FAILURE;
        }

       
        StreamBlock->stats.snd_count += 1;
	tmpStreamBlock = StreamBlock;
        StreamBlock = StreamBlock->Next;

	if (tmpStreamBlock->SendStreamQueue != NULL) {
 	    // there are more messages on the recent StreamBlock
 	    // so move it to back of queue to give others a chance
	    queue_remove_from_Q(&padapter->stream_adapter_list, (PMSD_GEN_MSG)tmpStreamBlock);
	    queue_put_Q(&padapter->stream_adapter_list, (PMSD_GEN_MSG)tmpStreamBlock);
	}
   } /* StreamBlock */

   //Reset the data pending flag onlyif we've read all the stream blocks
   //We can just use the padapter->stream_adapter_list on which StreamBlocks
   //placed during stream open time.


  if( ((strm_cal_snd_cnt(padapter->stream_adapter_list.QueueHead)) == 0) &&
       (stream_blk_snd_status == MD_SUCCESS))  
    {
          MSD_LEVEL2_DBGPRINT("snd_ww_data2adapter: No more to Send..DOne..\n");
          // update the stats
          MsdControlBlock->MsdStatisticsBlock.TotalBlocksSent += 1;
          //MSD_ENTER_MUTEX(&padapter->snd_data_mutex); /* 10.02.2002 */
          padapter->flags.SendDataPending &= ~(MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND);
          //MSD_EXIT_MUTEX(&padapter->snd_data_mutex); /* 10.02.2002 */
    }

  if (stream_blk_snd_status == MD_FAILURE)
    {
      //Send failed for atleast one StreamBlock. Restart the timer
      //WWPENDING: !!!!!!!Data with EOS or Data with NO EOS
      //and accordingly we need to set WWMsgPendingQFlag
      //MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
      //pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_DATA_WITH_EOS_BIT);
      //pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_DATA_WITH_NOEOS_BIT);
      //MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
      //Start timer if not yet started
      MSD_LEVEL2_DBGPRINT("snd_ww_data2adapter: Failed sedning for a StreamBlock..\n");
      return(Status);
    }
#if LFS
  Status = MD_SUCCESS;
  return (Status);
#endif

}

/***************************************************************************
 * Function Name                : snd_ww_strm_data2adapter
 * Function Type                : WW Send Data Support function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW Driver snd_ww_strm_data2adapter Routine.
 *                                This routine sends data to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_strm_data2adapter(pmercd_ww_send_streamData pStreamDataSt)
{
 PSTRM_MSG                               Msg;
 PSTRM_MSG                            TmpMsg;
#ifndef LFS
 PSTRM_MSG                           NextMsg;
#endif
 PMDRV_MSG                             MdMsg;
 PSTREAM_SEND				 Ptr;
 MD_STATUS                            Status;
 merc_ulong_t                        mfindex;
 mercd_ww_get_mfAddress_sT       mfAddressST;
 pmerc_uchar_t                    pmfAddress;
 pmercd_ww_dev_info_sT                pwwDev;
#ifndef LFS
 merc_ulong_t                     syncOffset;
 merc_ulong_t                       syncSize;
 merc_ulong_t                newMFneeded = 1;
 merc_uint_t                        dataSize;
 merc_uint_t                         numBlks;
 mercd_ww_mblk_train_info_sT       mblkTrain;
#endif
 pmercd_stream_connection_sT     StreamBlock;
 pmercd_adapter_block_sT            padapter;
 merc_uint_t                 MsgDataSize = 0;
#ifndef LFS
 merc_uint_t            numOfMblksPosted = 0;
 merc_uint_t                AvailableMFs = 0;
#endif
 mercd_ww_dmaDscr_Index_sT         dmaDescSt;
 merc_ushort_t                dmaDescIndex=0;
 merc_uchar_t                   *Buffer=NULL;


 Status = MD_SUCCESS;

 padapter = (pmercd_adapter_block_sT)pStreamDataSt->padapter;
 StreamBlock = (pmercd_stream_connection_sT)pStreamDataSt->StreamBlock;
 pwwDev = padapter->pww_info;

 MSD_LEVEL2_DBGPRINT("snd_ww_strm_data2adapter..StreamBlock: 0x%x type: 0x%x\n", StreamBlock, StreamBlock->type);


 ////////////////////////////////////////////////////////////
 // WWPENDING: !!!! We need to calculate the number of mblks
 // linked by b_cont starting from the 2nd mblk.
 // All these have their own descriptor mercd_ww_dma_descr_sT
 // linked to each other by their next pointer.
 // But share the same index 'dmaDescIndex', we can expect 
 // this kind for G-Streams. So lets take care of it later.
 ////////////////////////////////////////////////////////////


 Msg = StreamBlock->SendStreamQueue;
 // To prevent starvation send one block per Stream
 if (Msg) {
      MsgDataSize = 0;

      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
      Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

      if (StreamBlock->activeWWMfs >= StreamBlock->maxWWMfs) {
	  //Check for EOS - can be sent on another MF
          if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
              //No MFs for the current StreamBlock. Just return FAIL
              return(MD_FAILURE);
          }
	  //This message contains EOS so send even is active MF is maxed out
      }


      ///////////////////////////////////////////////////////
      //StreamBock Sanity Checking Begin
      ///////////////////////////////////////////////////////

      //If this is EOS, send the respective EOS Message
      if (Ptr->Flags & STREAM_FLAG_EOS) {
          //Remove the message first
          StreamBlock->SendStreamQueue = Msg->b_next;
          if (StreamBlock->SendStreamQueue != NULL)
             printk("Seen EOS, but SendStreamQueue not NULL!!!!\n");
          MSD_ENTER_MUTEX(&padapter->pww_info->ww_eospendq_mutex);
          Status = snd_ww_eos2adapter(padapter, Msg, 0);
          MSD_EXIT_MUTEX(&padapter->pww_info->ww_eospendq_mutex);
          return(MD_SUCCESS);
      }

      ///////////////////////////////////////////////
      // WWPENDING: This IF Block is not TESTED!!!!!!
      // FLUSH only applies to READ StreamBlocks.
      // Why we need this for Write StreamBlock???
      ///////////////////////////////////////////////
      if ((StreamBlock->pbind_block->WWFlushMFPending) ||
          (StreamBlock->pbind_block->WWFlushAckPending)) {
          return(MD_FAILURE);
      }

      if ((StreamBlock->state == MERCD_STREAM_STATE_BROKEN) ||
          (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND)) {
          if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
              strm_free_only_data_msgs(StreamBlock);
	      if (StreamBlock->SendStreamQueue == NULL) {
	 	  MSD_LEVEL2_DBGPRINT( "StreamGSndBlocks: SendStreamQueue NULL\n");
                  return(MD_FAILURE);
	      }
	       
	      Msg = StreamBlock->SendStreamQueue;
	      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	      Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
              
              //Send EOS MF for this Msg
              MSD_ENTER_MUTEX(&pwwDev->ww_eospendq_mutex);
              Status = snd_ww_eos2adapter(padapter, Msg, 0);
              MSD_EXIT_MUTEX(&pwwDev->ww_eospendq_mutex);
              Status = MD_SUCCESS;
              return(Status);
	  }
      } /* PTR 1979 */

      //Only EOS block does not have user header
      if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
          //Assert if data size > 4056 for non-EOS blocks
          if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
              MsgDataSize = MsdMessageDataSize(Msg);
              if (MsgDataSize > (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)))
                  MsgDataSize = (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
          } else {
              MsgDataSize = MsdMessageDataSize(Msg->b_cont);
              if (MsgDataSize > StreamBlock->ContainerSize)
                  MsgDataSize = StreamBlock->ContainerSize;
          }

          Msg->b_cont->b_wptr = Msg->b_cont->b_rptr + MsgDataSize;

          // Concatanation need for >4K blocks - saves MFs
          if ((StreamBlock->cantakeMode) && (MsgDataSize < MERCURY_HOST_IF_BLK_SIZE) && (Msg->b_next)) {
              TmpMsg = Msg;
              MsgDataSize = 0;
              while (TmpMsg) {
                 if (!Buffer) {
                     mercd_zalloc(Buffer, merc_uchar_t*,MERCURY_HOST_IF_BLK_SIZE);
                     if (Buffer == NULL) {
                         printk("WWStreamSendData: Unable to allocate memory\n");
#ifdef LFS
			 Status = MD_FAILURE;
			 return (Status);
#else
                         return;
#endif
                     }
                 }

                 if (!TmpMsg->b_cont) {
                     MSD_LEVEL2_DBGPRINT("WWStreamSendData: No data on this message\n");
                     TmpMsg = TmpMsg->b_next;
                     continue;
                 }

                 // do no concat end of stream
                 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(TmpMsg);
                 Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
                 if (Ptr->Flags & MD_MSG_FLAG_LATENCY) {
                     break;
                 }

                 memcpy(Buffer+MsgDataSize, TmpMsg->b_cont->b_datap->db_base, TmpMsg->b_cont->b_datap->db_size);
                 MsgDataSize += TmpMsg->b_cont->b_datap->db_size;

                 // do not free the first message - since it will be the main message
                 if (TmpMsg != Msg) {
                     Msg->b_next = TmpMsg->b_next;
                     MSD_FREE_MESSAGE(TmpMsg);
                     TmpMsg = Msg->b_next;
                 } else {
                     TmpMsg = TmpMsg->b_next;
                 }
              }

              MSD_FREE_KERNEL_MEMORY(Msg->b_cont->b_datap->db_base, Msg->b_cont->b_datap->db_size);
              Msg->b_cont->b_datap->db_base = (unsigned char *)Buffer;
              Msg->b_cont->b_datap->db_lim = (char *)Buffer + MsgDataSize;
              Msg->b_cont->b_datap->db_size = MsgDataSize;
              Msg->b_cont->b_rptr = (unsigned char *) Buffer;
              Msg->b_cont->b_wptr = (unsigned char *) Msg->b_cont->b_rptr + MsgDataSize;

          }

          if (MsgDataSize != MsdMessageDataSize(Msg->b_cont))
              MSD_LEVEL2_DBGPRINT("Error..%d %d\n", MsgDataSize, MsdMessageDataSize(Msg));
      }

      ///////////////////////////////////////////////////////
      //StreamBlock Sanity Checking End
      ///////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////
      //Get the next available DMA descriptor:
      //This code used to be in msgutl_ww_build_write_stream_mf
      //What if after getting an MF, we try to acquire the next avalble
      //DMA descr and it is not there ?? We cannot write the MF index
      //back to the inBound Q
      ////////////////////////////////////////////////////////
      dmaDescSt.pBitMap = StreamBlock->pSendBitMapValue;
      dmaDescSt.szBitMap = StreamBlock->szSendBitMap;

      //Get next available free dma descriptor
      //MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
      Status = msgutl_ww_get_nextfree_dmadescr(&dmaDescSt);
      if (Status == MD_FAILURE) {
          //WWPENDING: No Free DMA descriptors. Are we going to create another
          //descriptor table and link it with the first one. This needs to be
          //addressed.
          MSD_LEVEL2_DBGPRINT("snd_ww_strm_data2adapter: No Free DMA Descriptors\n");

          //MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
          return (Status);
      }

      dmaDescIndex = dmaDescSt.index;
      //Now mark ths DMA descr at  dmaDescSt.index as used
      dmaDescSt.pBitMap = StreamBlock->pSendBitMapValue;
      dmaDescSt.szBitMap = StreamBlock->szSendBitMap;

      Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);

      if (Status == MD_FAILURE) {
          MSD_LEVEL2_DBGPRINT("snd_ww_strm_data2adapter: Unable to mark DMA Descr\n");
          //MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
          return (Status);
      }
      StreamBlock->writeDMAPending = 1;
      //MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);

      ////////////////////////////////////////////////////////
      //Get a MF
      mfAddressST.pwwDevi = pwwDev;
      mfAddressST.pmfAddress = NULL;
      mfAddressST.mfIndex = 0;

      Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST);

      //We need to handle a situation when there are no MFAs available from PLX.  
      //In case of no MFAs, leave the Message back at the head of StreamBlock's 
      //StreamSendQ and let the timer take care of it after obtaining a MFA
      if (Status != MD_SUCCESS) {
         //Need to return MD_FAILURE
         //MSD_ENTER_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
         msgutl_ww_mark_dmadescr_free(&dmaDescSt);
         //MSD_EXIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
         Status = MD_FAILURE;
         //No need to Put the Message Back at
         //the head of  StreamBlock->SendStreamQueue
         //because we never removed it from
         //StreamBlock->SendStreamQueue
         return (Status);
      } else {
         StreamBlock->activeWWMfs++;
      }

      //Store the returned MF address and index
      pmfAddress = mfAddressST.pmfAddress;
      mfindex   = mfAddressST.mfIndex;
      //////////////////////////////////////////////////////

#ifdef DEBUG
      mblkTrain.mb = ((pmerc_void_t)(Msg->b_cont));
      mblkTrain.dataSize = 0;
      mblkTrain.mblkCount = 0;

      strm_ww_get_mblk_train_info(&mblkTrain);

      dataSize = mblkTrain.dataSize;
      numBlks = mblkTrain.mblkCount;

      //The following is for debug purposes
      if (numBlks > 1)
         MSD_LEVEL2_DBGPRINT("msgutl_ww_build_write_stream_mf: Num of mblks > 1\n");
#endif
//////////////////////////////////////////////////////
      //Just Allocate it locall and Free it locally,
      //so that we dont need to manage it exclusively.
     
      pStreamDataSt->Msg = (PSTRM_MSG) Msg;
      pStreamDataSt->padapter = (pmerc_void_t)padapter;
      pStreamDataSt->pmfAddress = (pmerc_uchar_t)pmfAddress;
      pStreamDataSt->StreamBlock = (pmerc_void_t)StreamBlock;
#ifdef LFS
      pStreamDataSt->dmaDescIndex = dmaDescIndex;
#else
      pStreamDataSt->dmaDescIndex = (pmerc_void_t)dmaDescIndex;
#endif

      Status = msgutl_ww_build_write_stream_mf(pStreamDataSt);

      //There could be a problem with descriptor allocation or
      //finding a free descritptor on the current descritptor
      //table.
      if (Status == MD_FAILURE) {
          MSD_LEVEL2_DBGPRINT("snd_ww_strm_data2adapter: Could be a problem on DMA descr alloc\n");
	  //Start timer if not yet started
          return (Status);
       }


#ifndef MERCD_LINUX
      //Sync the view of all caches with respect to the device
      syncOffset = ((pwwDev->inBoundQMfAddress) +
	       (mfindex * pwwDev->pww_param->sizeofMFAInboundQ));
      syncSize   = pwwDev->pww_param->sizeofMFAInboundQ;
      MsdWWSyncDevice(pwwDev);
#endif

      ///////////////////////////////////////////////////////
      //Write MFA to InBoundQ
      ///////////////////////////////////////////////////////
      MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
      MsdWWPutinBoundQReg(padapter, mfindex);
      MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

      //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
      //atomic_inc(&pwwDev->pww_counters->write_strm_mfs_sent);

      //We are done sending a Write Stream MF
 
      if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
          pStreamDataSt->dataCount += MsgDataSize; 
      } else {
          pStreamDataSt->dataCount += (MsgDataSize + sizeof(USER_HEADER));
      } 

      //Since we successfully placed the message on the MF, remove it from SendStreamQueue
      Msg = StreamBlock->SendStreamQueue = Msg->b_next;
 } /* Msg */

 return(Status);

}

/***************************************************************************
 * Function Name                : snd_ww_eos2adapter
 * Function Type                : WW Send Data Support function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW Driver snd_ww_strm_data2adapter Routine.
 *                                This routine sends data to adapter.
 * Additional comments          :
 ****************************************************************************/
int snd_ww_eos2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg, 
                       merc_uchar_t wwqueued)
{
 MD_STATUS                  Status;
 merc_ulong_t               mfindex;
 mercd_ww_get_mfAddress_sT  mfAddressST;
 pmerc_uchar_t              pmfAddress;
 pmerc_uchar_t              pmfHead;
 pmercd_ww_dev_info_sT      pwwDev;
#ifndef LFS
 merc_ulong_t               syncOffset;
 merc_ulong_t               syncSize;
#endif
 PMDRV_MSG                  MdMsg;
 PSTREAM_SEND               Ptr;
 mercd_ww_build_eos_sT      eosmfSt = {0};
 pmercd_stream_connection_sT StreamBlock;

 MSD_FUNCTION_TRACE(" snd_ww_eos2adapter 0x%x 0x%x %d %s #%d\n",
                     (size_t)padapter, (size_t)Msg, (size_t)wwqueued, __FILE__, __LINE__);
 Status = MD_SUCCESS;
 pwwDev = padapter->pww_info;


 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
 Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

 mfAddressST.pwwDevi = pwwDev;
 mfAddressST.pmfAddress = NULL;

 StreamBlock = padapter->pstream_connection_list[Ptr->StreamId];

 if (StreamBlock->SendStreamQueue == NULL)
  { 
     Status = msdwwutl_ww_read_free_inboundQ_for_mf_address(&mfAddressST);

     if (Status == MD_SUCCESS)
       {
         //Store the returned MF address and index
         pmfHead = pmfAddress = mfAddressST.pmfAddress;
         mfindex   = mfAddressST.mfIndex;
         eosmfSt.id = Ptr->StreamId;
         eosmfSt.pmfAddress = pmfAddress;
         Status = msgutl_ww_build_and_send_eos_mf(&eosmfSt);
         //Ship the MF
         MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
         MsdWWPutinBoundQReg(padapter, mfindex);
         MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

         //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
         //atomic_inc(&pwwDev->pww_counters->write_strm_eos_mfs_sent);
         //Free the message
         MSD_FREE_MESSAGE(Msg);
         //Now remove the msg if it is queued.
         if(wwqueued)
           pwwDev->pww_eos_msgq = NULL;

       }
     else
       {
         //Put the Msg back on pww_eos_msgq
         if (!wwqueued)
          strm_ww_put_eos_Q(pwwDev, Msg);
         MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
         pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT);
         MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
       }
  } /* SendStreamQueue */
 else
   { //There is data that needs to be sent 
     //Let the cart not go ahead of horse..

     //Put the Msg back on pww_eos_msgq
       if (!wwqueued)
        strm_ww_put_eos_Q(pwwDev, Msg);
       MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
       pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT);
       MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)
   }

#ifdef ENABLE_LATER
    //Trace Code (EOS Msg): Copy the Message to the buffer (if required)
    if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
      supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
    }
#endif
    push_trace_msg(padapter, StreamBlock->pbind_block, 0x5a01);

 return(MD_SUCCESS);
}
