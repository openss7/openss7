/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwdpc.c
 * Description                  : Generic Deferred processing functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDDPC_C_
#include "msdextern.h"
#undef _MSDDPC_C_

typedef merc_void_t (*FUNCTION)(merc_uint_t);
#define UUFUNCTION merc_int_t (*)(pmerc_void_t)

//#define printdbg printk

/***************************************************************************
 * Function Name                : mercd_ww_generic_intr_processing
 * Function Type                : DPC function
 * Inputs                       : Context1
 * Outputs                      : none
 * Calling functions            :
 * Description                  : Streams Driver InterruptDeferredProcessing
 *                                Routine. This routine does the work for Intr.
 *                                at a later stage.
 * Additional comments          :
 ****************************************************************************/
int mercd_ww_generic_intr_processing(merc_ulong_t Context1)
{
        pmercd_adapter_block_sT    padapter;
        merc_uchar_t               cnt;
        merc_uint_t                interruptType;
	int                        entered_cnt=0;

        if (!Context1) {
	    printk("msdwwdpc: Null Context\n");
	    return (MERCD_INTR_UNCLAIMED);
        }

        // First Get the adapter code in the structure

        cnt = *(merc_uint_t * )Context1;

        padapter = MsdControlBlock->padapter_block_list[cnt];
	
	if (!padapter) {
	    return(MERCD_INTR_UNCLAIMED);
	}

        MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

        //atomic_inc(&padapter->pww_info->pww_counters->dpc_called);

        if (!(padapter->flags.RecvPending & MERC_ADAPTER_FLAG_RECEIVE_PEND ) ) {
            cmn_err(CE_WARN, "mercd_ww_generic_intrp_processing. !RCV_PEND..\n");
            //atomic_inc(&padapter->pww_info->pww_counters->dpc_notours);

	    padapter->pww_info->intr_reason = 0;

            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return(MERCD_INTR_UNCLAIMED);
        }

        //atomic_inc(&padapter->pww_info->pww_counters->dpc_ours);

        if (!( padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) ) {
            cmn_err(CE_WARN, "mercd_ww_generic_intr_processing.I2O not..\n");
            padapter->phw_info->un_flag = 0;
	    padapter->pww_info->intr_reason = 0;
            padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;

            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return(MERCD_INTR_CLAIMED);

        }

        //Filter other invalid adapter states
        //WWPENDING 3: Are we checking the other states:
        //MERCD_ADAPTER_STATE_CRASHED and MERCD_ADAPTER_STATE_OUT_OF_SYNC ?

        if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED)      ||
	    (padapter->state == MERCD_ADAPTER_STATE_OUT_OF_SERVICE)) {
            cmn_err(CE_WARN, "mercd_ww_generic_intr_processing. adpt out of srvc.\n");
            padapter->phw_info->un_flag = 0;
	    padapter->pww_info->intr_reason = 0;
            padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;

            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return(MERCD_INTR_CLAIMED);
        }

        interruptType =  padapter->pww_info->intr_reason;

	padapter->pww_info->intr_reason = 0;

	//PLEASE NOTE: Never leave the intr_mutex here as in an SMP system
	//there might be another interrupt from another cpu creeping in 
	//immediately following our exit at this point and make intr_reason 
	//non-NULL. There by we will end up in an infinite do while loop 
	//causing a system hang: Mohan. 
	entered_cnt = 0;
	do {
	 entered_cnt++;

         switch(interruptType) {

         case MERCD_WW_INIT_ACK_INTERRUPT:
            //atomic_inc(&padapter->pww_info->pww_counters->init_ack_intr);
            mid_wwmgr_post_init_intr_processing(padapter->pww_info);
            break;
         case MERCD_WW_INIT_COMPLETE_INTERRUPT:
            //Change the Adapter state to Started
            //atomic_inc(&padapter->pww_info->pww_counters->init_complete_intr);
            mid_wwmgr_post_init_msg_ready_intr_processing(padapter->pww_info);
            break;

         case MERCD_WW_MESSAGEREADY_INTERRUPT:
            //atomic_inc(&padapter->pww_info->pww_counters->msg_ready_intr);
            msdwwrcv_ww_msgready_intr_processing(padapter->pww_info);
            break;

         case MERCD_WW_RESET_INTERRUPT_ACK_FROM_BOARD:
         case MERCD_WW_CANCEL_REQUEST_INTERRUPT_ACK:
            //Acknowledgement from the board for a Cancel interrupt sent earlie
            //printk("Got a MERCD_WW_CANCEL_REQUEST_INTERRUPT_ACK..\n");
            //atomic_inc(&padapter->pww_info->pww_counters->cancel_req_intr_ack);
            mid_wwmgr_process_cancel_request_intr_ack_from_board(padapter->pww_info);
            break;
         case MERCD_WW_RESET_INTERRUPT_FROM_BOARD:
            cmn_err(CE_CONT, " Got an Reset Interrupt from Board\n");
            //atomic_inc(&padapter->pww_info->pww_counters->reset_intr_from_brd);
            // Received a reset interrupt from the board.
            mid_wwmgr_process_reset_interrupt_from_board(padapter->pww_info);
            break;
         case MERCD_WW_ERROR_INTERRUPT :
            //cmn_err(CE_CONT, " Got an Error Interrupt from Board id %d\n", padapter->adapternumber);
	    //atomic_inc(&padapter->pww_info->pww_counters->error_intrs);
	    mid_wwmgr_process_error_interrupt_from_board(padapter->pww_info);
            break;
         default:
            printk("mercd: Got an Unknown Interrupt 0x%x on Adp. %d \n", interruptType, padapter->adapternumber);
            //atomic_inc(&padapter->pww_info->pww_counters->our_intr_but_unknown_type);
            break;
         }

	 interruptType =  padapter->pww_info->intr_reason;	

	 if (!interruptType) {
	     padapter->phw_info->un_flag = 0;
             padapter->flags.RecvPending = 0;
         }
	   
	 if (entered_cnt == 5000) {
	     printk("Get Stack here probably Current interrupt type is %d \n", interruptType);
	     interruptType = 0;
	     padapter->pww_info->intr_reason = 0;
	     padapter->phw_info->un_flag = 0;
	     padapter->flags.RecvPending = 0;
	 }


        } while(interruptType>0);

#ifndef LFS
out2:
#endif
	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
        return(MERCD_INTR_CLAIMED);
}


/***************************************************************************
 * Function Name                : mercd_ww_generic_timeout_processing
 * Function Type                : DPC function
 * Inputs                       : padapter
 * Outputs                      : none
 * Calling functions            :
 * Description                  : WW driver
 *                                Routine. This routine is called on system
 *                                timeout to send data and messages
 * Additional comments          :
 ****************************************************************************/
void mercd_ww_generic_timeout_processing(pmercd_adapter_block_sT padapter)
{
  MD_STATUS                                  Status;
  MD_STATUS                                retCode1;
#ifndef LFS
  pmercd_stream_connection_sT           StreamBlock;
#endif
  pmercd_ww_dev_info_sT                      pwwDev;
#ifndef LFS
  merc_uint_t	                         retVal = 0;
#endif

  if (!padapter) {
      printk("msdwwdpc: null padapter\n");
      return;
  }
  
  pwwDev = (pmercd_ww_dev_info_sT)padapter->pww_info;

  if (!pwwDev) {
      printk("msdwwdpc: null pwwDwv\n");
      return;
  }
  
  MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

  //Check for an undesirable state
  if (padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) {
      // We are in the shutdown state. In this state we are waiting on a
      // cancel ack back from the board. We can either get a cancel ack
      // back or the kernel died and cannot respond back with a cancel ack.
      // In this case we need to simulate a cancel ack behavior and return
      if (pwwDev->timeWaitForCancelAck ==  MERCD_WW_MAX_RESET_TIME) {
          printk("msdwwdpc: In Shutdown state - Simulating cancel ack behavior\n");
          mid_wwmgr_process_cancel_request_intr_ack_from_board(pwwDev);
          padapter->flags.SendTimeoutPending = 0;
  	  MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
          return;
      }

      pwwDev->timeWaitForCancelAck++;
      padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;
      time_ww_chk_snd_timer(padapter);
      MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
      return;
  }
  
  if ((padapter->state == MERCD_ADAPTER_STATE_MAPPED) ||
        (!(pwwDev->mfaBaseAddressDescr.host_address))) {
      MSD_ERR_DBGPRINT("mercd_timeout_dpc:Ret %d: %d\n",
          MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS, padapter->state);
      padapter->flags.SendTimeoutPending = 0;
      MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
      return;
  }


  //////////////////////////////////////////////
  //First Close the Unwanted Streams
  //User might have done a ^C
  //////////////////////////////////////////////
  if (pwwDev->WWCancelBigMsgAckPending) {
      pwwDev->WWCancelBigMsgAckPending++;
#ifdef LFS
      if (pwwDev->WWCancelBigMsgAckPending > 200) {
#else
      if (pwwDev->WWCancelBigMsgAckPending == 500) {
#endif
          pwwDev->WWCancelBigMsgAckPending = 0;
          mid_wwmgr_process_abnormal_termination(pwwDev);
          pwwDev->WWCloseAckToutCheck = 1;
      }
  }

#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
  /////////////////////////////////////////////////
  //Clean up all those streams for which we
  //never heard the CLOSE ACK from Board
  /////////////////////////////////////////////////
  if (pwwDev->WWCloseAckToutCheck) {
      pwwDev->WWCloseAckToutCheck++;
      //printk("Timer live after a ^C..%d\n", pwwDev->WWCloseAckToutCheck);
      if ((pwwDev->WWCloseAckToutCheck) > (200)) {
          pwwDev->WWCloseAckToutCheck = 0;
          mid_wwmgr_cleanup_unacked_closed_streams(pwwDev);
      }
  }
#endif /* _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS */


  //Send the Messages in their order of priority

  //1. Check for cancel bigmsg MF
  if (pwwDev->WWMsgPendingQFlag & (MERCD_ADAPTER_WW_CANCEL_FROM_BD_BIGMSGBUF_MF_BIT)) {
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_CANCEL_FROM_BD_BIGMSGBUF_MF_BIT);
      mid_wwmgr_send_pending_cancel_bigmsg_mfs(pwwDev);
  }

  //2. Check for cancel stream MF
  if (pwwDev->WWMsgPendingQFlag & (MERCD_ADAPTER_WW_CANCEL_STREAM_MF_BIT)) {
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_CANCEL_STREAM_MF_BIT);
      mid_wwmgr_send_pending_cancel_stream_mfs(pwwDev);
  }

  //3. Check for flush MF
  if (pwwDev->WWMsgPendingQFlag & (MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT)) {
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT);
      mid_wwmgr_send_pending_flush_stream_mfs(pwwDev);
  }

  //4. Now post FROM BOARD BIG Msg buffers
  if (pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_BIT) {
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_BIT);
      msgutl_ww_build_and_send_msgblk_mf(pwwDev);
  } else {
  }

  //5. Now send normal messages SMALL and BIG
  if (!MSD_ARRAY_EMPTY(padapter->snd_msg_Array) ) {
      retCode1 = snd_ww_msgs2adapter(padapter, MERCD_ADAPTER_WW_SEND_MSG_ALL);
      if ((retCode1 == MD_SUCCESS) && (MSD_ARRAY_EMPTY(padapter->snd_msg_Array))) {
          clear_bit(0, &padapter->send_array_pending);
      }
  }

  //6. WWPENDING: Now post FROM BOARD DATA Buffers
  if (pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT) {
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
      mid_wwmgr_send_pending_recv_streams_databuf_mfs(pwwDev);
  }

  //7 & 8. Now send the data on padapter->snd_data_quueue.
  if (pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT) {
      //Here first we send the data with EOS followed by regular data.
      pwwDev->WWMsgPendingQFlag &= ~(MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT);
      //If the Q is not Empty
      if(pwwDev->pww_eos_msgq) {
        if (pwwDev->pww_eos_msgq->b_next == NULL) {
            snd_ww_eos2adapter(padapter, pwwDev->pww_eos_msgq, 1);
        } else {
            msgutl_ww_build_and_send_multi_eos_mf(padapter);
        }
      }
  } else {
  }

  if (padapter->flags.SendDataPending & MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND) {
      Status = snd_ww_data2adapter(padapter); /*WW Streaming*/
  }

  padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;

  time_ww_chk_snd_timer(padapter);

  MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
}
