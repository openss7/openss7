/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdbndmgr.c
 * Description			: Bind Manager functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDBNDMGR_C_
#include "msdextern.h"
#undef _MSDBNDMGR_C_

merc_uint_t vmsdbndmgr = 0xffff;

extern int SendExitNotify;
/***************************************************************************
 * Function Name	: mid_bndmgr_bind
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	:
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_bndmgr_bind(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg)
{
	PDISPATCHER_BIND_ACK	BindAckPtr;
	PDISPATCHER_BIND	BindPtr;
	PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

#ifdef _8_BIT_INSTANCE
	MD_HANDLE   BindHandle;
#else
        MBD_HANDLE  BindHandle = 0;
#endif

	pmercd_bind_block_sT    BindBlock;
	merc_uint_t ErrorCode = MD_OK;
	merc_uint_t ReturnValue = 0;
        merc_uint_t      szBMap = 0;

        MSD_FUNCTION_TRACE("mid_bndmgr_bind", TWO_PARAMETERS,
                            (size_t) MsdOpenBlock, (size_t) Msg);

	MSD_ASSERT(MsdOpenBlock);
	MSD_ASSERT(Msg);

	if ( MsdControlBlock->pbind_block_list == NULL ) { //panic fix listbrds 
	    ErrorCode = CD_ERR_NO_BIND_HANDLE;
	    goto out;
	} 


	// find a free entry in the BindMapTable
	if( (BindHandle = supp_fnd_free_bnd_entry()) != 0 )
	{
		mercd_zalloc(BindBlock, pmercd_bind_block_sT, sizeof(mercd_bind_block_sT));

		if( BindBlock != NULL)
		{ 
			// initialize the bind block 
			
			MSD_INIT_MUTEX(&BindBlock->bind_block_mutex,"Bind Block Mutex", NULL);
			MSD_INIT_CV(&BindBlock->bind_block_termination_cv);
#ifdef DRVR_STATISTICS                                                           
                        MsdControlBlock->bind_count++;
#endif       
			BindPtr = (PDISPATCHER_BIND)MD_GET_MDMSG_PAYLOAD(MdMsg);
			BindBlock->bindhandle = BindHandle;

			BindBlock->state = MERCD_BIND_STATE_OPENED;

			BindBlock->popen_block = MsdOpenBlock;

			BindBlock->UserContext = BindPtr->UserContext;

                         //WW Messaging: For sending Cancel Big Msg MFs
                        BindBlock->WWPendReadAckForToBoardBigMsgs = 0;
                        szBMap = ((MSD_MAX_BOARD_ID_COUNT)/(8 * sizeof(merc_uint_t)));
                        if ((MSD_MAX_BOARD_ID_COUNT)%((8*sizeof(merc_uint_t))))
                           szBMap += 1;

                        mercd_zalloc(BindBlock->pWWMsgsToAdapters, pmerc_uchar_t, (szBMap)*(sizeof(merc_uint_t)));

                        BindBlock->szWWMsgsToAdapters = MSD_MAX_BOARD_ID_COUNT;
      BindBlock->NormalUnBind = 1;
      BindBlock->flags = 0;
		
			if(SendExitNotify)
            			BindBlock->ExitNotifyBindBoard = 0xFFFF;
			else
            			BindBlock->ExitNotifyBindBoard = 0;

			MsdControlBlock->pbind_block_list[BindHandle] = BindBlock;

			queue_put_Q(&MsdOpenBlock->bind_block_q, (PMSD_GEN_MSG) BindBlock);

			// make the current message id MID_BIND_ACK 
			MSD_ASSERT((MD_GET_MSG_DATABUF_LIMIT(Msg) - MD_GET_MSG_READ_PTR(Msg)) >=
			           sizeof(MDRV_MSG)+sizeof(DISPATCHER_BIND_ACK)); 

			MdMsg->MessageId = MID_DISPATCHER_BIND_ACK;

			BindAckPtr = (PDISPATCHER_BIND_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
   
                        MSD_ZERO_MEMORY((pmerc_char_t)(BindAckPtr), sizeof(DISPATCHER_BIND_ACK));

			BindAckPtr->BindHandle = BindHandle;

			MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
				  sizeof(MDRV_MSG)+sizeof(DISPATCHER_BIND_ACK)); 

			// return the message to user via sync path 
			supp_process_sync_receive(MsdOpenBlock, Msg);

			// Break only for error codes
			return;
		}else {
			MSD_ERR_DBGPRINT( "Bind: Bind blk alloc failed.\n");
			ErrorCode = CD_ERR_NO_MEMORY;
		}
	}else {
		MSD_ERR_DBGPRINT( "Bind: Bind handle alloc failed.\n", MsdControlBlock->maxbind);
		ErrorCode = CD_ERR_NO_BIND_HANDLE;
	}

	// Only Errors are returned thr' this path.
out:

   	if(MD_IS_MSG_NO_ACK(Msg))
		return;

   	// send back an error message 
   	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

   	if(MD_IS_MSG_ASYNC(Msg)) 
   		// for async messages, send back a async reply 
		supp_process_receive(MsdOpenBlock, Msg);
   	else 
   		// sync msg uses the sync return path 
		supp_process_sync_receive(MsdOpenBlock, Msg);
	return;
}

/***************************************************************************
 * Function Name		: mid_bndmgr_unbind
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/

void mid_bndmgr_unbind(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg)
{
   PDISPATCHER_UNBIND          UnBindPtr;
   PMDRV_MSG                   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
#ifdef _8_BIT_INSTANCE
   MD_HANDLE                   BindHandle;
#else
   MBD_HANDLE                  BindHandle;
#endif
   pmercd_stream_connection_sT StreamBlock;
#ifndef LFS
   mercd_ww_dmaDscr_Index_sT   dmaDescSt= {0};
#endif
   pmercd_adapter_block_sT     padapter;
   pmercd_bind_block_sT        BindBlock;
   merc_uint_t 		       ErrorCode = MD_OK;
   merc_uint_t 		       ReturnValue = 0;
#ifdef LFS
   merc_uint_t                 index;
#else
   merc_uint_t                 i,index;
   merc_uint_t                 id;
   merc_uint_t                 AdapterNumber;
#endif
   MD_STATUS 		       Status = MD_SUCCESS;
   int 			       szBMap;

   if ((!MsdOpenBlock) || !(Msg)) {
       printk("mid_bndmgr_unbind: null MsdOpenBlock/Msg\n");
   }

   UnBindPtr = (PDISPATCHER_UNBIND)MD_GET_MDMSG_PAYLOAD(MdMsg);

   // grab the bind handle and map it to a BindBlock
   BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

   if ((BindHandle <= MsdControlBlock->maxbind) && (BindHandle != 0)) {
       if ((BindBlock = MsdControlBlock->pbind_block_list[BindHandle]) != NULL ) {
           if (BindBlock->state == MERCD_BIND_STATE_OPENED ) {
               // Always try t0 Send Death notice if required
               bnd_snd_death_msg2brd(MSD_ONLY_ONE_BIND_DEATH, BindBlock);

               MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
               if (BindBlock->flags & MERCD_WW_FLAG_FORCED_UNBIND) {
                   //printk("MERCD_WW_FLAG_FORCED_UNBIND found..\n");
                   BindBlock->flags &= ~(MERCD_WW_FLAG_FORCED_UNBIND);
                   BindBlock->NormalUnBind = 0;
               }
               
               if (BindBlock->stream_connections != 0) {
                   StreamBlock = BindBlock->stream_connection_ptr;
                   BindBlock->state = MERCD_BIND_STATE_RELEASE_PEND;

                   if (StreamBlock) {
                       mercd_clock_t cur_ticks, to;
                       merc_ushort_t   HWTimer;
                       pmercd_ww_dev_info_sT    pwwDev;
                       padapter = StreamBlock->padapter_block;
                       pwwDev = padapter->pww_info;
                       Status = MD_SUCCESS;

                       if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
                           if ((pwwDev->state == MERCD_ADAPTER_WW_SUCCEDED) && 
						(!BindBlock->NormalUnBind))  {
			       // WW boards
                               MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)

			       // close streams 
                               BindBlock->WWCancelStrmAckPending = 0;
                               bnd_dispatch_close_strm(BindBlock);
                               StreamBlock->state = MERCD_STREAM_STATE_FREED;
                               BindBlock->stream_connections = 0;
                               BindBlock->stream_connection_ptr = NULL;
                               BindBlock->flags &= ~MSD_BIND_FLAG_STREAM_OPEN_PEND;

                               MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);
                               queue_remove_from_Q(&MsdOpenBlock->bind_block_q,
                                                    (PMSD_GEN_MSG) BindBlock );
                               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
                          
  		               MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

                               //Now return
                               if (MD_IS_MSG_NO_ACK(Msg)) {
                                   //printk("Unbind: early return for WW\n");
                                   //In WW mode: we just return
                                   MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);
                                   return;
                               }
                           } /* MERCD_ADAPTER_WW_SUCCEDED */
                       } else {
  			   // Sram boards...
                           cur_ticks = MSD_SYSTEM_TICK;
                           bnd_dispatch_close_strm(BindBlock);
                           HWTimer = StreamBlock->padapter_block->phw_info->timer_info->snd_timeout_interval;
                           // to = cur_ticks + drv_usectohz(HWTimer * 1000) * 3;
                           to = cur_ticks + MsdMsecToTicks(HWTimer * 1000) * 3;

                           if (MSD_CV_TIMED_WAIT(&BindBlock-> bind_block_termination_cv,
                                                &BindBlock->bind_block_mutex, to) == -1) {
                               MSD_LEVEL2_DBGPRINT("Unbind Timer is Expired \n");
                               StreamBlock->state = MERCD_STREAM_STATE_FREED;
                           } else {
                               MSD_LEVEL2_DBGPRINT("Unbind Timer is Signalled \n");
                           }
                           BindBlock->stream_connections = 0;
                           BindBlock->stream_connection_ptr = NULL;
                           BindBlock->flags &= ~MSD_BIND_FLAG_STREAM_OPEN_PEND;
                       } /* I20 */
                   } /* StreamBlock */
                   
 		   BindBlock->state = MERCD_BIND_STATE_RELEASED;
               }
#if 0
                 else {
		   /* This BindBlock is for messaging */
                   //Are there any BigMsgs to Board waiting for an ack from Board
                   //if ((BindBlock->WWPendReadAckForToBoardBigMsgs) && (!BindBlock->NormalUnBind))
                   if ((!BindBlock->NormalUnBind)) {
                       dmaDescSt.pBitMap = BindBlock->pWWMsgsToAdapters;
                       dmaDescSt.szBitMap =  BindBlock->szWWMsgsToAdapters;
                       for (i=0;i<MSD_MAX_BOARD_ID_COUNT;i++) {
                          dmaDescSt.index = i;
                          Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
            
                          if (Status == MD_FAILURE)
                              continue;

                          AdapterNumber = mercd_adapter_log_to_phy_map_table[i];
                          //printk("AdapterNumber: 0x%x\n", AdapterNumber);
                          if (AdapterNumber != 0xFF) {
                              padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

                              if (padapter == NULL) {
                                  continue;
                              } else {
                                  if (padapter->pww_info == NULL)
                                      continue;
                              }

                              MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

                              //printk("i : %d mask: 0x%x\n", i, (MSD_EXIT_NOTIFY_BIND_BOUND << i));
                              //printk("Found an Adapter...i: %d\n", i);
                              if ((padapter->state == MERCD_ADAPTER_STATE_READY) ||
                                  (padapter->state == MERCD_ADAPTER_STATE_DOWNLOADED)) {
                                  if (!BindBlock->WWCancelBigMsgAckPending) {
                                      if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
                                          if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
                                              //printk("cancel_bigmsg_mf: Messaging..\n");
                                              Status = MD_SUCCESS;
#if 0
                                              Status = msgutl_ww_build_and_send_cancel_bigmsg_mf
                                                                 (padapter->pww_info, BindBlock);
#endif
                                              if (Status == MD_SUCCESS) {
                                                  //printk("BndMgr: cancel_bigmsg_mf sent Succes.Mesging.\n");
                                                  BindBlock->WWCancelBigMsgMFPending = 0;
                                                  BindBlock->WWCancelBigMsgAckPending = 1;
                                                  padapter->pww_info->WWCancelBigMsgAckPending= 1;
                                              }

                                              MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);
                                              queue_remove_from_Q(&MsdOpenBlock->bind_block_q,
                                                                   (PMSD_GEN_MSG) BindBlock );
                                              MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

                                              //Now return
                                              if (MD_IS_MSG_NO_ACK(Msg)) {
                                                  msgutl_ww_mark_dmadescr_free(&dmaDescSt);
                                                  //printk("Unbind: early return for WW
                                                  //        CancelMsgs for Messaging\n");
                                                  MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
     			                          MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);
                                                  return;
                                              }
                                          }
                                      }
                                  }
                              }
                              
		              MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
                          }
                       }
                   } /* WWPendReadAckForToBoardBigMsgs */

               }  /* if BindBlock for messaging */
#endif

               MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

               MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);
               queue_remove_from_Q(&MsdOpenBlock->bind_block_q, (PMSD_GEN_MSG) BindBlock );
               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

               szBMap = ((MSD_MAX_BOARD_ID_COUNT)/(8 * sizeof(merc_uint_t)));
     
               if ((MSD_MAX_BOARD_ID_COUNT)%((8*sizeof(merc_uint_t))))
	                     szBMap += 1; 

	       if (BindBlock->pWWMsgsToAdapters)
                   mercd_free(BindBlock->pWWMsgsToAdapters, (szBMap)*(sizeof(merc_uint_t)),MERCD_FORCE);

#ifdef DRVR_STATISTICS
               MsdControlBlock->bind_count--;
#endif

               BindBlock->flags=0x8000;

               index = MsdControlBlock->BindFreeIndex;


               if (MsdControlBlock->pbind_free_block_list[index]) {
                   mercd_free(MsdControlBlock->pbind_free_block_list[index], 
					     MERCD_BIND_BLOCK, MERCD_FORCE);
               }

               MsdControlBlock->pbind_free_block_list[index] = BindBlock;

               MsdControlBlock->BindFreeIndex++;

               MsdControlBlock->BindFreeIndex %= MSD_MAX_BINDTOBE_FREED_INDEX;

               MsdControlBlock->pbind_block_list[BindHandle] = NULL;

               MSD_DESTROY_MUTEX(&BindBlock->bind_block_mutex);

           } else {
               MSD_ERR_DBGPRINT("mercd:Unbind: Invalid Bind Block State %d \n", BindBlock->state );
               ReturnValue = CD_ERR_BAD_PARAMETER;  /* CD_ERR_INV_BND_STATE; */
           }
       } else {
           MSD_ERR_DBGPRINT("mercd:Unbind: Invalid Bind handle %d maxbind %d\n", BindHandle, 
                                                                  MsdControlBlock->maxbind);
           ReturnValue =  CD_ERR_BAD_PARAMETER; /* CD_ERR_INV_BND_HANDLE; */
       }
   } else {
       MSD_ERR_DBGPRINT("mercd:Unbind: Invalid Bind handle %d maxbind %d \n", BindHandle, 
					                       MsdControlBlock->maxbind);
       ReturnValue =  CD_ERR_BAD_PARAMETER; /* CD_ERR_INV_BND_HANDLE; */
   } 


   //Now return
   if (MD_IS_MSG_NO_ACK(Msg))
       return;

   // make current message a MD_ACK
   Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

   if (MD_IS_MSG_ASYNC(Msg)) {
       // for async messages, send back a async reply
       supp_process_receive(MsdOpenBlock, Msg);
   } else {
       // sync msg uses the sync return path
       supp_process_sync_receive(MsdOpenBlock, Msg);
   }
   return;
}

/***************************************************************************
 * Function Name        : mid_bndmgr_bind_exit_markup
 * Function Type        : manager function
 * Inputs               : MsdOpenBlock,
 *                      Msg
 * Outputs              : none
 * Calling functions    :
 * Description          :
 * Additional comments  :
 ****************************************************************************/
void mid_bndmgr_bind_exit_markup(PMSD_OPEN_BLOCK MsdOpenBlock, PSTRM_MSG Msg)
{
    PMSD_EXIT_NOTIFY_ENTRY ExitNtfyPtr;
    PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

#ifdef _8_BIT_INSTANCE
    MD_HANDLE   BindHandle;
#else
    MBD_HANDLE   BindHandle; 
#endif

    pmercd_bind_block_sT    BindBlock;
    merc_ulong_t ErrorCode = MD_OK;
    merc_ulong_t ReturnValue = 0;

    MSD_ASSERT(MsdOpenBlock);
    MSD_ASSERT(Msg);

    MSD_FUNCTION_TRACE("mid_bnd_exit_notify_bind", TWO_PARAMETERS,
                       (size_t)MsdOpenBlock, (size_t)(Msg));

    ExitNtfyPtr = (PMSD_EXIT_NOTIFY_ENTRY)MD_GET_MDMSG_PAYLOAD(MdMsg);

    // grab the bind handle and map it to a BindBlock
    BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

    if( (BindHandle <= MsdControlBlock->maxbind) && (BindHandle != 0) )
    {

        if( (BindBlock = MsdControlBlock->pbind_block_list[BindHandle]) != NULL )
        {

			if(ExitNtfyPtr->Flags)
                BindBlock->ExitNotifyBindBoard |= (MSD_EXIT_NOTIFY_BIND_BOUND << ExitNtfyPtr->BoardNumber);
			else
                BindBlock->ExitNotifyBindBoard &= ~(MSD_EXIT_NOTIFY_BIND_BOUND << ExitNtfyPtr->BoardNumber);

                if(MD_IS_MSG_NO_ACK(Msg))
                    return;

                // make current message a MD_ACK
                Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

                // return the message to user via sync path
                supp_process_sync_receive(MsdOpenBlock, Msg);

                // Break only for error codes
                return;
        }else {
            MSD_ERR_DBGPRINT("ExitNtfy: BIND handle bad.\n");
            ErrorCode = CD_ERR_BAD_PARAMETER;  /* CD_ERR_BAD_BND_HANDLE; */
            ReturnValue = BindHandle;
        }
    }else {
		MSD_ERR_DBGPRINT("ExitNtfy: BIND handle too large. %d\n",BindHandle);
        ErrorCode = CD_ERR_BAD_PARAMETER; /* CD_ERR_INV_BND_HANDLE; */
        ReturnValue = BindHandle;
    }

#ifndef LFS
out:
#endif
    // Only Errors are returned thr' this path.
	if(MD_IS_MSG_NO_ACK(Msg))
		return;

    // send back an error message
    Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

    if(MD_IS_MSG_ASYNC(Msg)) // for async messages, send back a async reply
		supp_process_receive(MsdOpenBlock, Msg);
    else // sync msg uses the sync return path
		supp_process_sync_receive(MsdOpenBlock, Msg);

    return;

}
