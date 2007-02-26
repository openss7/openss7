/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdfwmgr.c
 * Description			: FirmWare Manager functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDFWMGR_C_
#include "msdextern.h"
#undef _MSDFWMGR_C_

extern int NewCanTakeProtocol;

/***************************************************************************
 * Function Name		: fwmgr_reply_mgr
 * Function Type		: manager function
 * Inputs			: padapter,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
md_status_t fwmgr_reply_mgr(pmercd_adapter_block_sT padapter,PSTRM_MSG Msg)
{
	PMDRV_MSG MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_ulong_t		MessageId; 
	PMERC_HOSTIF_MSG	FwMessage = 0;
#ifndef LFS
  	pmercd_ww_dev_info_sT   pwwDev = 0; /* WW Streaming */
  	merc_uint_t             i = 0; 	    /* WW Streaming */
#endif

	MSD_FUNCTION_TRACE("mid_fwmgr_reply_mgr", TWO_PARAMETERS, (merc_ulong_t)padapter,
						(merc_ulong_t)Msg);

	// ???????? Same function replacing ProcessDriverClass and CdProcessRecv
	// ProcessDriver class message is FW and CdProcess MDMSG ????
	// following assert fails for ConfigReply ???? Raj
	// ConfigReply moved to SendConfigReply

	MSD_ASSERT(Msg->b_cont);
	FwMessage = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;

	// read the message ID 
	MessageId = MERCURY_GET_MESSAGE_TYPE(FwMessage);


	// it is imperative that we copy out the fwmessgae fields
	// before we re-use the space for driver messages

	switch(MessageId) {
	case QCNTRL_OPEN_STR_ACK:
	{
		PHIF_OPEN_STREAM_ACK	BodyPtr;
		merc_ulong_t		TransactionId;
		merc_ulong_t		StreamId;
		PSTREAM_OPEN_ACK	Ptr;
		merc_ulong_t		CanTakeCount;
		pmercd_bind_block_sT    BindBlock;
		pmercd_stream_connection_sT StreamBlock;
		pmercd_open_block_sT  OpenBlockContext;
    		//WW Streaming Change
#ifndef LFS
    		merc_ulong_t          szInBoundMfa = 0;
    		merc_uint_t           numEntries = 0;
    		pmercd_ww_dma_descr_sT pdmaDescr = 0;
    		merc_uint_t          szBMap = 0;
		int actualBlkSize = MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER);
#endif

		BodyPtr = (PHIF_OPEN_STREAM_ACK) MERCURY_BODY_START(FwMessage);
                
                MSD_LEVEL2_DBGPRINT("QCNTRL_OPEN_STR_ACK\n");

		if (MERCURY_GET_BODY_SIZE(FwMessage) != sizeof(HIF_OPEN_STREAM_ACK)){
		    MSD_ERR_DBGPRINT("OPEN_STR_ACK Bad body size=%d should be %d\n",
		     MERCURY_GET_BODY_SIZE(FwMessage), sizeof(HIF_OPEN_STREAM_ACK));
		    MSD_FREE_MESSAGE(Msg);
		    return(MD_FAILURE);
		}

		if (BodyPtr->StreamId >= MSD_MAX_STREAM_ID) {
		    MSD_ERR_DBGPRINT("OPEN_STR_ACK Bad Stream Id 0x%x\n", BodyPtr->StreamId);
		    MSD_FREE_MESSAGE(Msg);
		    return(MD_FAILURE);
		}

		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

		StreamBlock = padapter->pstream_connection_list[BodyPtr->StreamId];

		// StreamBlock should be NULL
		if (StreamBlock == NULL) {
		    TransactionId = MERCURY_GET_TRANSACTION_ID(FwMessage);
		    StreamId = BodyPtr->StreamId;
		    CanTakeCount = BodyPtr->InitialCanTake;
		    MD_SET_MDMSG_ID(MdMsg, MID_STREAM_OPEN_ACK);
		    MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
		    
                    // Must readjust size since we're switching to a different payload 
		    MSD_ASSERT(MD_GET_MSG_SIZE(Msg)>(sizeof(MDRV_MSG)
							     +sizeof(STREAM_OPEN_ACK)));

		    MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)
						             +sizeof(STREAM_OPEN_ACK)));

	
		    // try to find a match for a stream block waiting for
		    // for an open ack.  The transaction id is used  as the 
                    // match criteria. Till this point the StreamBlock is null
		    StreamBlock = strm_fnd_entry_from_adapter(padapter, TransactionId);
	
		    // validity Stream Block checking 
		    if (StreamBlock == NULL) {
			MSD_ERR_DBGPRINT("MID_STREAM_OPEN_ACK: Cannot find matching"
	 			  " Stream Block: TransId=%d\n",Ptr->TransactionId);
			MSD_FREE_MESSAGE(Msg);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			return(MD_FAILURE);
		    }			
	
		    if (StreamBlock->state != MERCD_STREAM_STATE_OPEN_PEND) {
			MSD_FREE_MESSAGE(Msg);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			return(MD_FAILURE);
		    }
	
 		    Ptr = (PSTREAM_OPEN_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
		    Ptr->StreamId = StreamId;
		    Ptr->ActualSize = BodyPtr->ActualSize;

                    if (!(padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                        Ptr->CanTakeByteCount = ((CanTakeCount/
                                          (MERCURY_HOST_IF_BLK_SIZE+ sizeof(USER_HEADER)))
                                        *(MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)));
                    } else {
		        if (NewCanTakeProtocol &&  (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)) {
			    // for customer who can't handle lot of initial read callbacks
                            CanTakeCount = 4;
			    Ptr->CanTakeByteCount = CanTakeCount;
                        } else {
                            Ptr->CanTakeByteCount = (CanTakeCount) * 
			 	          (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
		        }
                    }

		    Ptr->TransactionId = TransactionId;
		    Ptr->ErrorCode = MD_OK;

		    MSD_FREE_MESSAGE(Msg->b_cont);
		    Msg->b_cont = NULL;

		    // init the stream block 
		    StreamBlock->id = Ptr->StreamId;
		    StreamBlock->state = MERCD_STREAM_STATE_CONNECTED;		
		    StreamBlock->qparam.snd_queue_low= MSD_STREAM_SEND_LOW;
      		    StreamBlock->qparam.cur_snd_cnt = 0; 
		    StreamBlock->isStreamHCS = 0;
		
        	    // Ensure that the can take is a multiple of 4056 
		    if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
                        if (!(padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                            // GStream Setup for SRAM
                            StreamBlock->WWMode = 0;
			    StreamBlock->can_take = (Ptr->CanTakeByteCount /
					 SRAM_BLOCK_MAX_DATA_SIZE) * (SRAM_BLOCK_MAX_DATA_SIZE);
			    if (Ptr->ActualSize > (SEND_QUEUE_LIMIT_MULTIPLIER * SRAM_BLOCK_MAX_DATA_SIZE)) {
			        StreamBlock->qparam.snd_queue_limit = ((SEND_QUEUE_LIMIT_MULTIPLIER *
				 	 SRAM_BLOCK_MAX_DATA_SIZE) / SEND_QUEUE_LIMIT_DIVISOR);
			        Ptr->CanTakeByteCount = (StreamBlock->qparam.snd_queue_limit);
			    } else {
				StreamBlock->qparam.snd_queue_limit = (Ptr->ActualSize /
				          SRAM_BLOCK_MAX_DATA_SIZE) * (SRAM_BLOCK_MAX_DATA_SIZE);
			        Ptr->CanTakeByteCount = (Ptr->ActualSize / SRAM_BLOCK_MAX_DATA_SIZE) *
						                          (SRAM_BLOCK_MAX_DATA_SIZE);
			    }
			    Ptr->BlockSize = 2072;  //SRAM_BLOCK_MAX_DATA_SIZE;
         	        } else {
                            // GStream Setup for WW
                            StreamBlock->WWMode = 1;
                            StreamBlock->writeDMAPending = 0;
                            StreamBlock->maxWWMfs = CanTakeCount;
                            StreamBlock->activeWWMfs = 0;
                            StreamBlock->qparam.snd_queue_limit =  Ptr->CanTakeByteCount;
                            Ptr->BlockSize = 2048;  //MERCURY_HOST_IF_BLK_SIZE;
                            StreamBlock->can_take = Ptr->CanTakeByteCount;
                        } 
                    } else { 
                        if (!(padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                            // BStream Setup for SRAM
                            StreamBlock->WWMode = 0;
			    // setup container size
                            if ((StreamBlock->ContainerSize == SRAM_BLOCK_MAX_DATA_SIZE) ||
                                (StreamBlock->multiplication_factor != 0)) {
                                StreamBlock->can_take = (Ptr->CanTakeByteCount / SRAM_BLOCK_MAX_DATA_SIZE) *
                                                                                 StreamBlock->ContainerSize;
                            } else {
                                StreamBlock->can_take = 0;
                            }

			    if (StreamBlock->cantakeMode == 0) {
                                StreamBlock->qparam.snd_queue_limit = SEND_QUEUE_LIMIT_MULTIPLIER *
                                                                          SRAM_BLOCK_MAX_DATA_SIZE;
                                Ptr->CanTakeByteCount = StreamBlock->qparam.snd_queue_limit /
                                                                          SEND_QUEUE_LIMIT_DIVISOR;
                            } else {
                                Ptr->CanTakeByteCount = StreamBlock->multiplication_factor *
                                                                        StreamBlock->ContainerSize;
                                StreamBlock->qparam.snd_queue_limit = Ptr->CanTakeByteCount;
                            }

			    Ptr->BlockSize = (StreamBlock->ContainerSize - sizeof(USER_HEADER));
                        } else {
                            // BStream Setup for WW
                            StreamBlock->WWMode = 1;
                            StreamBlock->writeDMAPending = 0;
                            StreamBlock->maxWWMfs = CanTakeCount;
                            StreamBlock->activeWWMfs = 0;

                            if ((StreamBlock->ContainerSize == SRAM_BLOCK_MAX_DATA_SIZE) ||
                                (StreamBlock->multiplication_factor != 0)) {
                                StreamBlock->can_take = CanTakeCount * 
					 (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
                            } else {
                                StreamBlock->can_take = 0;
                            }

                            if (StreamBlock->cantakeMode == 0) {
                                StreamBlock->qparam.snd_queue_limit =  (CanTakeCount) * 
					 (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
                                Ptr->CanTakeByteCount = (CanTakeCount) * 
					 (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
                            } else {
                                Ptr->CanTakeByteCount = StreamBlock->multiplication_factor * StreamBlock->ContainerSize;
                                StreamBlock->qparam.snd_queue_limit = Ptr->CanTakeByteCount;
                            }
			    Ptr->BlockSize = (StreamBlock->ContainerSize - sizeof(USER_HEADER));
                        }
	 	    }

		    StreamBlock->accumulated_usr_ack = 0;
		
		    // Save the stream block ptr in the adapter block streamid table
		    padapter->pstream_connection_list[Ptr->StreamId] = StreamBlock;

		    MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

		    // Now set the ptr in BindBlock.  DO NOT set before OPEN_ACK
		    // is received since that'll cause an internal close to
		    // be sent  if user closes the stream before OPEN_ACK is received.
		    MSD_ASSERT(StreamBlock->pbind_block);
		    BindBlock = StreamBlock->pbind_block;
		    MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);	
		    OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);

		    // connect bind block with the stream block 
		    BindBlock->stream_connection_ptr = StreamBlock;
		    BindBlock->stream_connections++;
	
		    // turn off the open pending flag in bind block now that open is complete
		    BindBlock->flags &= ~MSD_BIND_FLAG_STREAM_OPEN_PEND;
	
		    // must set bind handle and user context in msg 
		    MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindBlock->bindhandle);  
		    MD_SET_MDMSG_USER_CONTEXT(MdMsg, BindBlock->UserContext);

		    // New streamgroup implementation to fix 256 open limitation 
		    StreamBlock->handle = Ptr->StreamId; /* New streamgroup api changes */
 		    Ptr->StreamHandle = StreamBlock->handle;

		    MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
                    if (StreamBlock->WWMode) {
                        MD_STATUS Status = MD_SUCCESS;
                        Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
                        if (Status == MD_FAILURE) {
                            printk("Abnormal Termination in OPEN_ACK...\n");
                            return (MD_FAILURE);
                        }
                    }
#endif
		    supp_process_receive(OpenBlockContext, Msg);
                    break;
		} else {
	 	    MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
		    MSD_ASSERT(0);
		}
		
		break;
	}

	case QCNTRL_INIT_ACK:
	{
		// Is it required to send the Ack to user ?
                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_INIT_ACK\n");
                MSD_FREE_MESSAGE(Msg);
		break;
	}

	case QCNTRL_CAN_TAKE:
	{
		PHIF_CAN_TAKE BodyPtr;
		PHIF_CAN_TAKE_ENTRY EntryPtr;
		pmercd_stream_connection_sT   StreamBlock;
		int cnt;

		BodyPtr = (PHIF_CAN_TAKE) MERCURY_BODY_START(FwMessage);
		EntryPtr = (PHIF_CAN_TAKE_ENTRY)(&(BodyPtr->Entry));

                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_CAN_TAKE\n");

		MSD_ENTER_MUTEX(&padapter->snd_data_mutex);

		for (cnt=0; cnt < BodyPtr->NoCanTakes; cnt++) {

			MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

			StreamBlock = padapter->pstream_connection_list[EntryPtr->StreamId];

			if(!StreamBlock) {
				MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			}else {
			
			// Ensure it is multiple of 4056 
			if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
				StreamBlock->can_take += EntryPtr->CanTake;
			} else {
				StreamBlock->can_take += ((EntryPtr->CanTake/4056) * (StreamBlock->ContainerSize));
			}
			MSD_LEVEL2_DBGPRINT("CanTake: StreamId %d, new Can Take: %d\n", 
						EntryPtr->StreamId, StreamBlock->can_take);

			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
                     
 		        if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
                            PSTRM_MSG                       SendMsg;
                            PSTRM_MSG                       DataMsg;
                            PMERC_HOSTIF_MSG               MercMsg1;

                            SendMsg = supp_alloc_buf(sizeof(MDRV_MSG), GFP_ATOMIC);
			    if (SendMsg) {
                                DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG), GFP_ATOMIC);
				if (DataMsg) {
                                    MD_SET_MSG_TYPE(DataMsg, M_DATA);
                                    MSD_LINK_MESSAGE(SendMsg,DataMsg);

                                    MercMsg1= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

                                    MERCURY_SET_DEST_COMPINST(MercMsg1, EntryPtr->StreamId);
                                    MERCURY_SET_BODY_SIZE(MercMsg1,4);

                                    *(int *)MERCURY_BODY_START(MercMsg1) = StreamBlock->can_take;

                                    MERCURY_SET_MESSAGE_TYPE(MercMsg1,0xc00003);

                                    MERCURY_SET_MESSAGE_CLASS(MercMsg1, MERCURY_CLASS_DRIVER);

                                    if (Msg->b_cont) {
                                        MERCURY_SET_TRANSACTION_ID(MercMsg1,EntryPtr->CanTake);
                                    }

                                    supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)MercMsg1);

                                    MSD_FREE_MESSAGE(SendMsg);
			        } else {
				    printk("mercd: Unable to allocate Data Msg for drvtrace\n");
				    MSD_FREE_MESSAGE(SendMsg);
				}
		            } else {
				printk("mercd: Unable to allocate Msg for drvtrace\n");
			    }
                        }
 
			}

			EntryPtr++;

		}

		MSD_EXIT_MUTEX(&padapter->snd_data_mutex);
	
		// free the message 
		MSD_FREE_MESSAGE(Msg);
		break;
	}

	case QCNTRL_SUCCEEDED:		// Used for CLOSE complete
	{
		PHIF_SUCCESS BodyPtr;
		PSTREAM_CLOSE_ACK	Ptr;
		merc_ulong_t	StreamId;
		pmercd_open_block_sT  OpenBlockContext;
		pmercd_stream_connection_sT StreamBlock;
		pmercd_bind_block_sT		BindBlock;

                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_SUCCEEDED\n");

		if(MERCURY_GET_BODY_SIZE(FwMessage) != sizeof(HIF_SUCCESS)){
			MSD_ERR_DBGPRINT("HIF_SUCCESS: Bad body size=%d, should be %d\n", 
					MERCURY_GET_BODY_SIZE(FwMessage),sizeof(HIF_SUCCESS));
		}
		BodyPtr = (PHIF_SUCCESS) MERCURY_BODY_START(FwMessage);

		// if stream id map entry is null, then this must
		//	be an ack to the close stream cmd
		MSD_ASSERT(BodyPtr->StreamId <= MSD_MAX_STREAM_ID);
		StreamId = BodyPtr->StreamId;

		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

		// Add a check for StreamIdMapTable to avoide panic on close timing 
		if((StreamBlock = padapter->pstream_connection_list[BodyPtr->StreamId]) == NULL) {
			MSD_ERR_DBGPRINT("HIF_SUCCESS: For non existant Stream=%d\n",StreamId);
			MSD_FREE_MESSAGE(Msg);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			break;
		}

		MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

		MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_ACK);
		MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
		MSD_ASSERT(MD_GET_MSG_SIZE(Msg)>(sizeof(MDRV_MSG)
						 +sizeof(STREAM_CLOSE_ACK)));
		MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg) +sizeof(MDRV_MSG)
						+sizeof(STREAM_CLOSE_ACK)));
		Ptr = (PSTREAM_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
		Ptr->StreamId = StreamId;
		Ptr->ErrorCode = MD_OK;

		MSD_FREE_MESSAGE(Msg->b_cont);
		Msg->b_cont = NULL;
		
		// If the QCNTRL_SUCCEEDED came back in the TERMINATE_PEND
                // state then we need not free all the memory for the
                // stream as we will get a close. In this case, just send
                // back the CLOSE ACK and be done 
		if ((StreamBlock->flags & STREAM_OPEN_F_PERSISTENT) &&
		    (StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND)) {
		    StreamBlock->state = MERCD_STREAM_STATE_TERMINATE_PEND;
                    BindBlock = StreamBlock->pbind_block;
                    MSD_ASSERT(BindBlock);

                    MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);

                    OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);

                    MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
                    MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle);
                    MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);

                    MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

                    supp_process_receive(OpenBlockContext, Msg);
                    break;
                }

		// must clear all links in MsdStreamMapTable and
		// adapter block StreamIdMapTable

		MSD_ENTER_MUTEX(&padapter->snd_data_mutex);

		if(StreamBlock->SendStreamQueue){
			MSD_FREE_MESSAGE(StreamBlock->SendStreamQueue );
			StreamBlock->SendStreamQueue=NULL;
		}

		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

        // finally, remove the stream block from adapter block stream list
        if(queue_remove_from_Q(&padapter->stream_adapter_list,
		                       (PMSD_GEN_MSG)StreamBlock) != MD_SUCCESS) {
            MSD_ERR_DBGPRINT("CLOSE_ACK:RemoveFromQ failed for StreamBlock."
                    " StreamId=%d\n",StreamBlock->id);
            MSD_ASSERT(0);
        }

        padapter->pstream_connection_list[Ptr->StreamId] = NULL;

		MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

		MSD_EXIT_MUTEX(&padapter->snd_data_mutex);


     /////////////////////////////////////////////////////////////////
     //WW Streaming Change
     /////////////////////////////////////////////////////////////////
     {
        if (padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)
          {
            if(padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED)
             {
               if (StreamBlock->writeDMAPending)
                {
                  MSD_ERR_DBGPRINT("Close Strm ACK Rcvd while Write DMA Complete Pending..id: %d\n", StreamBlock->id);
                  if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
                     MSD_ERR_DBGPRINT("id : %d is a Read Stream\n", StreamBlock->id);
                  else
                     MSD_ERR_DBGPRINT("id : %d is a Write Stream\n", StreamBlock->id);
                }

               if (((StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) &&
                  (StreamBlock->pSendBitMapValue)) ||
                  ((StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) &&
                  (StreamBlock->pRcvStrmDmaDesc)))
                    mid_wwmgr_close_streams(padapter, StreamBlock, 1);
               //For an internal close like ^C, we would have sent
               //an internal close and would have overwritten the
               //StreamBlock->state from INTERNAL_CLOSE_PEND to
               //MERCD_STREAM_STATE_FREED.

               if(StreamBlock->state == MERCD_STREAM_STATE_FREED)
                             MSD_FREE_MESSAGE(Msg);
             }
          }
     }
     /////////////////////////////////////////////////////////////////

		// if stream close was internally generated, release the
		//	message and break; All other links were destroyed earlier on 
		if(StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND){

			BindBlock = StreamBlock->pbind_block;

			MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);

			if(BindBlock->stream_connections)
                	  MSD_SIGNAL_CV(&BindBlock->bind_block_termination_cv);

			BindBlock->stream_connection_ptr = NULL;
			BindBlock->stream_connections--;
			MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);	 
		
			MSD_FREE_MESSAGE(Msg);
		}
		else {

		 if(StreamBlock->state != MERCD_STREAM_STATE_FREED)  {

			BindBlock = StreamBlock->pbind_block; 

			MSD_ASSERT(BindBlock);

			MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
			
			OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);

			// close stream failed or passed shutdown connection anyways
			// must set bind handle and user context in msg 

			// LA: Fri Aug 31 14:57:06 EDT 2001
			// The order for setting the stream handle before setting the bind handle
			// is imperative for the stream group api change in the libqhost.so.
			MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
			MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle);
			MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);
			Ptr->StreamId = StreamBlock->id;

			StreamBlock->id = 0;
		
			StreamBlock->handle = 0;

			BindBlock->stream_connection_ptr = NULL;

			BindBlock->stream_connections--;

			MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

			// Not Internal Close so send msg to user 
			supp_process_receive(OpenBlockContext, Msg);

		 }
	   }	

		// free the stream block memory space 
		mercd_free(StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE);
#ifdef DRVR_STATISTICS
                MsdControlBlock->open_streams--;
                padapter->streams_count--;
#endif
		break;
	}

	case QCNTRL_FAILED:		// Used for StreamOpen Failure
	{
		PHIF_FAILED BodyPtr;
		merc_ulong_t msgtype;

                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_FAILED\n");

		if(MERCURY_GET_BODY_SIZE(FwMessage) != sizeof(HIF_FAILED)){
			MSD_ERR_DBGPRINT("HIF_FAILED: Bad body size=%d, should be %d\n", 
						MERCURY_GET_BODY_SIZE(FwMessage),sizeof(HIF_FAILED));
		}
		BodyPtr = (PHIF_FAILED) MERCURY_BODY_START(FwMessage);

		// if stream id map entry is null, then this must
		//	be an ack to the open stream cmd
		msgtype = BodyPtr->Type & MERCURY_TYPE_MASK;

		MSD_ERR_DBGPRINT("QCNTRL_FAILED: Type=0x%x,Error=0x%x,StreamId=%d \n", 
			      msgtype, BodyPtr->ErrorCode,BodyPtr->StreamId);

		switch(msgtype) {
		case QCNTRL_OPEN_STREAM:
		{
			merc_ulong_t		TransactionId;
			PSTREAM_OPEN_ACK	Ptr;
			merc_ulong_t		ErrorCode;
			pmercd_stream_connection_sT StreamBlock;
	
			TransactionId = MERCURY_GET_TRANSACTION_ID(FwMessage);
			ErrorCode = BodyPtr->ErrorCode;
			MD_SET_MDMSG_ID(MdMsg, MID_STREAM_OPEN_ACK);
			MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
			MSD_ASSERT(MD_GET_MSG_SIZE(Msg)>(sizeof(MDRV_MSG)
						 +sizeof(STREAM_OPEN_ACK)));
			MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg) +sizeof(MDRV_MSG)
							+sizeof(STREAM_OPEN_ACK)));

			Ptr = (PSTREAM_OPEN_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
			Ptr->ErrorCode = PAM_ERR_STREAM_OPEN_FAILED;
			Ptr->TransactionId = TransactionId;
		
			MSD_FREE_MESSAGE(Msg->b_cont);
			Msg->b_cont = NULL;

			// try to find a match for a stream block waiting for
			// for an open ack.  The transaction id is used 
			// as the match criteria. Till this point the StreamBlock
			// is null
			
			MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

			StreamBlock = strm_fnd_entry_from_adapter(padapter, Ptr->TransactionId);

			MSD_LEVEL2_DBGPRINT("OPEN_STREAM_ACK:ERR:Id=%d, TransId=%d\n",
							Ptr->TransactionId, Ptr->StreamId);

			// validity checking 
			if(StreamBlock == NULL){
				MSD_ERR_DBGPRINT("MID_STREAM_OPEN_ACK:ERR: Cannot find matching"
						" Stream Block: TransId=%d\n",Ptr->TransactionId);

				MSD_FREE_MESSAGE(Msg);
				MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
				return(MD_SUCCESS);
			}			
	
			if(StreamBlock->state != MERCD_STREAM_STATE_OPEN_PEND) {
				MSD_ERR_DBGPRINT("MID_STREAM_OPEN_ACK:ERR:in bad state =%d \n",
								StreamBlock->state); 

				MSD_FREE_MESSAGE(Msg);
				MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
				return(MD_SUCCESS);
			}

//////////////////////////////////////////////////////////////////////////////
//Since it is a Open Failure We could never have initialized
//StreamBlock->WWMode
#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
{
	 MD_STATUS Status = MD_SUCCESS;
	 Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
	 if (Status == MD_FAILURE)
		 {
				printk("Abnormal Termination in OPEN_FAILED...\n");
				StreamBlock->pbind_block->stream_connection_ptr = NULL;
				// remove the stream block from the adapter stream list
				if(queue_remove_from_Q(&padapter->stream_adapter_list,
							 (PMSD_GEN_MSG)StreamBlock)!=MD_SUCCESS){
					printk("MID_STREAM_OPEN_ACK:RemoveFromQ failed for "
									"StreamBlock. StreamId=%d\n",
								 StreamBlock->id);
					MSD_ASSERT(0);
				}
				padapter->pstream_connection_list[Ptr->StreamId] = NULL;
				// free the stream block memory space
				MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
				MSD_FREE_KERNEL_MEMORY((pmerc_void_t)StreamBlock,
										sizeof(mercd_stream_connection_sT));
				return (MD_SUCCESS);
		 }
}
#endif
//////////////////////////////////////////////////////////////////////////////



			// process stream open ack error condition :open stream failed
			// reset the MsdStreamMapTable to free this stream handle 
	
			StreamBlock->pbind_block->stream_connection_ptr = NULL;

			// send error message to user 
			supp_process_receive(
				MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(StreamBlock->pbind_block), Msg);


			// remove the stream block from the adapter stream list 
			if(queue_remove_from_Q(&padapter->stream_adapter_list,
			                       (PMSD_GEN_MSG)StreamBlock)!=MD_SUCCESS){
				MSD_ERR_DBGPRINT("MID_STREAM_OPEN_ACK:RemoveFromQ failed for "
								"StreamBlock. StreamId=%d\n",
								StreamBlock->id);
				MSD_ASSERT(0);
			}
			padapter->pstream_connection_list[Ptr->StreamId] = NULL;

			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
		
			// free the stream block memory space 
			MSD_FREE_KERNEL_MEMORY((pmerc_void_t)StreamBlock,
			                       sizeof(mercd_stream_connection_sT));
			break;
		}

		case QCNTRL_CLOSE_STREAM:
		{
			// MSD_ASSERT(0);
			MSD_ERR_DBGPRINT("QCNTL_FAILED for QCNTRL_CLOSE_STREAM\n");
			break;
		}

		default:
			break;
		}	// Internal Switch
		break;
	}

	case QCNTRL_SESS_CLOSED:
	{
		PHIF_SESS_CLOSED BodyPtr;
		PSTREAM_SESS_CLOSE_ACK	Ptr;
		merc_ulong_t	StreamId;
		pmercd_open_block_sT  OpenBlockContext;
		pmercd_stream_connection_sT StreamBlock;
		pmercd_bind_block_sT	BindBlock;

                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_SESS_CLOSED\n");

		if(MERCURY_GET_BODY_SIZE(FwMessage) != sizeof(HIF_SESS_CLOSED)){
			MSD_ERR_DBGPRINT("HIF_SESS_CLOSED: Bad body size=%d, should be %d\n", 
					MERCURY_GET_BODY_SIZE(FwMessage),sizeof(HIF_SESS_CLOSED));
		}
		BodyPtr = (PHIF_SESS_CLOSED) MERCURY_BODY_START(FwMessage);

		StreamId = BodyPtr->StreamId;
		MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);
		MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
		MSD_ASSERT(MD_GET_MSG_SIZE(Msg)>(sizeof(MDRV_MSG)
					+sizeof(STREAM_SESS_CLOSE_ACK)));
		MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg) +sizeof(MDRV_MSG)
					+sizeof(STREAM_SESS_CLOSE_ACK)));
		Ptr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
		Ptr->StreamId = StreamId;
		MSD_LEVEL2_DBGPRINT("CLOSE_SESS_STREAM_ACK: Id=%d \n",Ptr->StreamId);

		MSD_FREE_MESSAGE(Msg->b_cont);
		Msg->b_cont = NULL;


		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
		// fetch the stream block 
		if((StreamBlock = padapter->pstream_connection_list[Ptr->StreamId])
						== NULL) {
			MSD_ERR_DBGPRINT("CLOSE_SESS_ACK: streamblock null StreamId=%d \n", 
						Ptr->StreamId);
			MSD_FREE_MESSAGE(Msg);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			return(MD_SUCCESS);
		}

		if(StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) {

			MSD_ERR_DBGPRINT("CLOSE_SESS_ACK: in bad state =%d, StreamId=%d\n", 
						StreamBlock->state,Ptr->StreamId);

			MSD_FREE_MESSAGE(Msg);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			return(MD_SUCCESS);
		}

/////////////////////////////////////////////////////////////////////////////////
#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
                if (StreamBlock->WWMode)
                {
                  MD_STATUS Status = MD_SUCCESS;
                  Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
                  if (Status == MD_FAILURE)
                   {
                     printk("Abnormal Termination in FAILE in QCNTRL_SESS_CLOSED...\n");
                     return (MD_FAILURE);
                    }
                }
#endif
/////////////////////////////////////////////////////////////////////////////////

		BindBlock = StreamBlock->pbind_block;
		MSD_ASSERT(BindBlock);

		MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);

		OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);

		// must set bind handle and user context in msg 
		MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);

		// LA: Fri Aug 31 14:57:06 EDT 2001
		// The order for setting the stream handle before setting the bind handle
		// is imperative for the stream group api change in the libqhost.so.
		MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindBlock->bindhandle);

		MD_SET_MDMSG_USER_CONTEXT(MdMsg, BindBlock->UserContext);

		Ptr->StreamId = StreamBlock->id;

		StreamBlock->state = MERCD_STREAM_STATE_BROKEN;

                MSD_LEVEL2_DBGPRINT("MERCD_STREAM_STATE_BROKEN...\n");

		// free any messages on send queue for Write stream : NEW  ###########
		if(StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) {
			if(StreamBlock->SendStreamQueue){
				// Free only till EOS ......
				strm_free_only_data_msgs(StreamBlock);
			}
		}

		MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

		// Is it a CLOSE_SESS for write stream NEW ######### 
		// Write stream can receive CLOSE SESS Message
		// if(StreamBlock->StreamFlags & STREAM_OPEN_F_SEND_ONLY)
		//		MSD_ASSERT(0);

		// Send to user 
		// No more teaching to cheetah guys.....
		supp_process_receive(OpenBlockContext, Msg);

		MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
		break;
	}

	case	QCNTRL_PROC_DTH_ACK:
	{
		// Free the Bind Block
                MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: QCNTRL_PROC_DTH_ACK\n");
		MSD_FREE_MESSAGE(Msg);
		break;
	}

        case QCNTRL_INIT_HCS:
        {
                merc_int_t i;
                PHIF_INIT_HCS BodyPtr;

                BodyPtr = (PHIF_INIT_HCS) MERCURY_BODY_START(FwMessage);

                if (BodyPtr->numOfStreams < MSD_MAX_STREAM_ID) {
                    padapter->HCS.maxNumOfHCS = BodyPtr->numOfStreams;
                } else {
                    MSD_LEVEL2_DBGPRINT("fwmgr_reply_mgr: FW (%d) > Driver (%d)\n",
                                 BodyPtr->numOfStreams, padapter->HCS.maxNumOfHCS);
                    padapter->HCS.maxNumOfHCS = MSD_MAX_STREAM_ID;
                }

                // allocate array for stream states
                mercd_zalloc(padapter->HCS.pStreamStateArray, Uint32 *, (sizeof(Uint32)*padapter->HCS.maxNumOfHCS));
                if (padapter->HCS.pStreamStateArray == NULL) {
                    printk("QCNTRL_INIT_HCS: unable to allocate memory - HCS disabled\n");
                    padapter->isHCSSupported = 0;
                    MSD_FREE_MESSAGE(Msg);
                    break;
                }
                // initialize the array
                for (i=0; i<padapter->HCS.maxNumOfHCS; i++) {
                     padapter->HCS.pStreamStateArray[i] = HCS_AVAILABLE;
                }

                padapter->HCS.firstHCSId = BodyPtr->startStreamId;
                padapter->HCS.sizeOfHCS = BodyPtr->streamSize;
                padapter->HCS.numOfHCSActive = 0;
                padapter->isHCSSupported = 1;
                padapter->HCS.nextAvailableStreamId = 0;

                printk("HCS Enabled on Board %d\n", padapter->adapternumber);
                MSD_LEVEL2_DBGPRINT("        maxNumOfHCSSupported %d\n", padapter->HCS.maxNumOfHCS);
                MSD_LEVEL2_DBGPRINT("        firstHCSStreamId     %d\n", padapter->HCS.firstHCSId);
                MSD_LEVEL2_DBGPRINT("        sizeOfHCSSupported   %d\n", padapter->HCS.sizeOfHCS);
                MSD_FREE_MESSAGE(Msg);

                break;
        }

	default:
		MSD_LEVEL2_DBGPRINT("Unknown message type from fw. 0x%x\n",MessageId);
                MSD_FREE_MESSAGE(Msg);
		break;

	} // Main Switch

	return(MD_SUCCESS);
}


/***************************************************************************
 * Function Name		: fwmgr_msg_reply_mgr
 * Function Type		: manager function
 * Inputs			: Msg,
 *				  padapter
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
md_status_t fwmgr_msg_reply_mgr(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter)
{
	PMDRV_MSG	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	pmercd_stream_connection_sT	StreamBlock;

	MSD_FUNCTION_TRACE("mid_fwmgr_msg_reply_mgr", TWO_PARAMETERS, (merc_ulong_t)Msg,
						(merc_ulong_t)padapter);

	// Decode the message ID and process 

	switch(MD_GET_MDMSG_ID(MdMsg)) {
	case   MID_STREAM_RECEIVE:
	{
		PSTREAM_RECEIVE	Ptr;

                MSD_LEVEL2_DBGPRINT("fwmgr_msg_reply_mgr: MID_STREAM_RECEIVE\n");

		Ptr = (PSTREAM_RECEIVE)MD_GET_MDMSG_PAYLOAD(MdMsg);

		MSD_ASSERT(Ptr->StreamId < MSD_MAX_STREAM_ID);

		// fetch the stream block using the stream id as index
		// into the StreamIdMapTable

		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
		StreamBlock = padapter->pstream_connection_list[Ptr->StreamId];
		MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

		// if stream block is null then just free the msg and return 
		// this normally means stream was closed
		if(StreamBlock == NULL) {
			MSD_FREE_MESSAGE(Msg);
			return(MD_SUCCESS);
		}

		if(StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) {
			MSD_FREE_MESSAGE(Msg);
			return(MD_SUCCESS);
		}

		if(StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY){
			MSD_LEVEL2_DBGPRINT(
			 "mercd:mid_fwmgr_msg_reply_mgr: Data msg rx'ed on WRITE_ONLY stream: Adapter %d, stream Id %d.",
					padapter->adapternumber,
					StreamBlock->id);
			MSD_FREE_MESSAGE(Msg);
			return(MD_SUCCESS);
		}

///////////////////////////////////////////////////////////////////////////////
#ifdef _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
     if (StreamBlock->WWMode)
      {
        MD_STATUS Status = MD_SUCCESS;
        Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
        if (Status == MD_FAILURE)
         {
           printk("Abnormal Termination in MID_STREAM_RECEIVE...\n");
           return (MD_FAILURE);
         }
      }
#endif
/* _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS */
///////////////////////////////////////////////////////////////////////////////

		// need to check the size of the up-stream receive queue
		// before passing the message to the user.  

		// set the MDRV_MSG class, stream handle, and user context
		// in the message 
		MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);

		// LA: Fri Aug 31 14:57:06 EDT 2001
		// The order for setting the stream handle before setting the bind handle
		// is imperative for the stream group api change in the libqhost.so.
		MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
		MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle);
		MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);

		if(Ptr->Sequence != StreamBlock->stats.rcv_count &&
			!(Ptr->Flags & STREAM_FLAG_EOS)) {
		}

		
		// update some stats
		StreamBlock->stats.rcv_count++;

		if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
                    PSTRM_MSG               SendMsg;
                    PSTRM_MSG               DataMsg;
                    PMERC_HOSTIF_MSG        MercMsg;

                    SendMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
		    if (SendMsg) {
                        DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG),GFP_ATOMIC);
			if (DataMsg) {
                            MD_SET_MSG_TYPE(DataMsg, M_DATA);
                            MSD_LINK_MESSAGE(SendMsg,DataMsg);

                            MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

                            MERCURY_SET_DEST_COMPINST(MercMsg, Ptr->StreamId);
                            MERCURY_SET_BODY_SIZE(MercMsg,4);
                            *(int *)MERCURY_BODY_START(MercMsg) = Ptr->StreamId;
                            MERCURY_SET_MESSAGE_TYPE(MercMsg,(MID_STREAM_RECEIVE|0x8000));

                            if (Msg->b_cont) {
                                MERCURY_SET_TRANSACTION_ID(MercMsg,(Msg->b_cont->b_wptr-Msg->b_cont->b_rptr));
                            }

                            supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)MercMsg);

                            MSD_FREE_MESSAGE(SendMsg);
		        } else {
                            printk("mercd: Unable to allocate Data Msg for drvtrace\n");
                            MSD_FREE_MESSAGE(SendMsg);
                        }
                    } else {
                        printk("mercd: Unable to allocate Msg for drvtrace\n");
                    }
                }

		supp_process_receive(MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(StreamBlock->pbind_block), Msg);

		break;
	}

	case   MID_STREAM_CLOSE_SESS_ACK: // For Read Streams
	{
		PSTREAM_SESS_CLOSE_ACK	Ptr;
		pmercd_open_block_sT  OpenBlockContext;
       	        pmercd_stream_connection_sT StreamBlock;
       	        pmercd_bind_block_sT        BindBlock;

                MSD_LEVEL2_DBGPRINT("fwmgr_msg_reply_mgr: MID_STREAM_CLOSE_SESS_ACK\n");

		Ptr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);

		MSD_ASSERT(Ptr->StreamId < MSD_MAX_STREAM_ID);

		MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);

		// fetch the stream block 
		if((StreamBlock = padapter->pstream_connection_list[Ptr->StreamId])==NULL) {
			MSD_ERR_DBGPRINT("mid_fwmgr_msg_reply_mgr: streamblock null "
							"StreamId=%d\n", Ptr->StreamId);
			MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
			MSD_FREE_MESSAGE(Msg);
			return(MD_SUCCESS);
		}

		MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);

		if((StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) &&
		   ((StreamBlock->state != MERCD_STREAM_STATE_CLOSE_PEND) ||
		    (StreamBlock->state != MERCD_STREAM_STATE_TERMINATE_PEND))) { 
			MSD_FREE_MESSAGE(Msg);
			return(MD_SUCCESS);
		}

///////////////////////////////////////////////////////////////////////////////
#ifdef  _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
                if (StreamBlock->WWMode)
                {
                  MD_STATUS Status = MD_SUCCESS;
                  Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
                  if (Status == MD_FAILURE)
                   {
                     printk("Abnormal Termination in MID_STREAM_CLOSE_SESS_ACK...\n");
                     return (MD_FAILURE);
                   }
                }
#endif
/* _ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS */
///////////////////////////////////////////////////////////////////////////////


		BindBlock = StreamBlock->pbind_block;
		MSD_ASSERT(BindBlock);
		OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);

		// must set bind handle and user context in msg 
		// Fix for record 
		MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);

		// LA: Fri Aug 31 14:57:06 EDT 2001
		// The order for setting the stream handle before setting the bind handle
		// imperative for the stream group api change in the libqhost.so.
		MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle);
		MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);
		Ptr->StreamId = StreamBlock->id;

		StreamBlock->state = MERCD_STREAM_STATE_BROKEN;

		if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
                    PSTRM_MSG        SendMsg;
                    PSTRM_MSG        DataMsg;
                    PMERC_HOSTIF_MSG MercMsg;

                    SendMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
		    if (SendMsg) {
                        DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+4,GFP_ATOMIC);
			if (DataMsg) {
                            MD_SET_MSG_TYPE(DataMsg, M_DATA);
                            MSD_LINK_MESSAGE(SendMsg,DataMsg);
                            MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
                            MERCURY_SET_BODY_SIZE(MercMsg,4);
                            MERCURY_SET_DEST_COMPINST(MercMsg, Ptr->StreamId);
                            MERCURY_SET_MESSAGE_TYPE(MercMsg,MID_STREAM_CLOSE_SESS_ACK);
                            MERCURY_SET_TRANSACTION_ID(MercMsg,BindBlock->bindhandle);
                            *(int *)MERCURY_BODY_START(MercMsg) = Ptr->StreamId;
                            supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)MercMsg);
                            MSD_FREE_MESSAGE(SendMsg);
			} else {
                            printk("mercd: Unable to allocate Data Msg for drvtrace\n");
                            MSD_FREE_MESSAGE(SendMsg);
                        }
                    } else {
                        printk("mercd: Unable to allocate Msg for drvtrace\n");
                    }
                }

		// Is it a CLOSE_SESS for write stream 
		// This is a CLOSE_SESS ACK Message, Not Valid for Write Stream
		if(StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)
			MSD_ASSERT(0);


		// Send to user 
		supp_process_receive(OpenBlockContext, Msg);

		break;
	}

	default:
		MSD_LEVEL2_DBGPRINT("mid_fwmgr_msg_reply_mgr: Invalid Message\n");
		MSD_ASSERT(0);

	} // Switch
	return(MD_SUCCESS);

}
