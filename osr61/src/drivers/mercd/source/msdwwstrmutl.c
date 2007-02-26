/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***************************************************************************
 * File Name                    : msdwwstrmutl.c
 * Description                  : WW Stream Utility functions 
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDMSGMGR_C
#include "msdextern.h"
#undef _MSDMSGMGR_C


/***************************************************************************
 * Function Name                : strm_ww_Q_snd_msg
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver QueueSendMessage Routine.
 *                                This routine queues a send message
 * Additional comments          :
 ****************************************************************************/
md_status_t strm_ww_Q_snd_msg(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter)
{
  md_status_t                 Status;
  pmercd_ww_dev_info_sT       pwwDev;
#ifndef LFS
  PMDRV_MSG                    MdMsg;
#endif
  merc_uint_t              msgClass;
  mercd_ww_get_mfClass_sT  mClassSt;

  Status = MD_SUCCESS;

  MSD_ENTER_MUTEX(&padapter->snd_msg_mutex);

  // Send_array_pending is a functional mutex to create an order in messages 
  pwwDev = padapter->pww_info;
  mClassSt.msgSize = pwwDev->pww_param->sizeofMFAInboundQ;

  if (!test_and_set_bit(0, &padapter->send_array_pending)) {
      mClassSt.mb = (pmerc_void_t)Msg;
      Status = msgutl_ww_determine_message_class(&mClassSt);
      msgClass = mClassSt.msgClass;

      if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
          (!(pwwDev->mfaBaseAddressDescr.host_address))) {
          // Go to the timer code
      } else  if( msgClass == MERCD_WW_SMALL_MESSAGE ) {
          Status = snd_ww_small_msgs2adapter(padapter, Msg);
         if (Status == MD_SUCCESS) {
             MSD_FREE_MESSAGE(Msg);
             clear_bit(0, &padapter->send_array_pending);
             MSD_EXIT_MUTEX(&padapter->snd_msg_mutex);
             return(Status);
         }
      } else if ( msgClass == MERCD_WW_BIG_MESSAGE ) {
         Status = snd_ww_big_msgs2adapter(padapter, Msg);
         if (Status == MD_SUCCESS) {
             clear_bit(0, &padapter->send_array_pending);
             MSD_EXIT_MUTEX(&padapter->snd_msg_mutex);
             return(Status);
         } 
      } else { 
         printk("strm_ww_Q_snd_msg: Invalid Msg Class 0x%x \n", msgClass );
         clear_bit(0, &padapter->send_array_pending);
         MSD_EXIT_MUTEX(&padapter->snd_msg_mutex);
         Status = MD_FAILURE;
         return(Status);
      }
  } 

  // send the message to the timer array for processing
  queue_put_msg_Array(&padapter->snd_msg_Array, Msg, MSD_SENDER);

  MSD_EXIT_MUTEX(&padapter->snd_msg_mutex);

  return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : strm_ww_get_mblk_train_info 
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : 
 *                              
 * Additional comments          :
 ****************************************************************************/
md_status_t strm_ww_get_mblk_train_info(pmercd_ww_mblk_train_info_sT pmblkTrain)
{
 PSTRM_MSG pmblk;
 md_status_t Status;
  
 MSD_FUNCTION_TRACE("strm_ww_get_mblk_train_info 0x%x %s #%d\n", 
                     (size_t) pmblkTrain, __FILE__, __LINE__);
 Status = MD_SUCCESS;
 pmblk = ((PSTRM_MSG)(pmblkTrain->mb));

 while (pmblk)
 {
  pmblkTrain->dataSize += ((merc_ulong_t)(pmblk->b_wptr - pmblk->b_rptr));
  pmblkTrain->mblkCount++;
  pmblk = pmblk->b_cont;
 }
return (Status);
}

/***************************************************************************
 * Function Name                : strm_ww_copy_mblk_train_data 
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : 
 *                                
 * Additional comments          :
 ****************************************************************************/
md_status_t strm_ww_copy_mblk_train_data(pmercd_ww_copy_mblk_train_data_sT pmblkCopy)
{
  PSTRM_MSG pmblk;
  merc_uint_t size;
  pmerc_uchar_t virt;
  md_status_t Status;

  MSD_FUNCTION_TRACE("strm_ww_copy_mblk_train_data 0x%x %s #%d\n", 
                      (size_t) pmblkCopy, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  pmblk = ((PSTRM_MSG)(pmblkCopy->mb));

  virt = (pmerc_uchar_t)(pmblkCopy->virtaddr);

    while (pmblk)
    {
      size = (pmblk->b_wptr - pmblk->b_rptr);
      MsdCopyMemory(pmblk->b_rptr, virt, size);
      pmblk = pmblk->b_cont;
      virt += size;
    }
return (Status);
}
