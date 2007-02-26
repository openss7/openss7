/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdstrmutl.c
 * Description			: Utility/Support functions
 *
 *
 ***********************************************************************/
#include "msd.h"
#define _MSDSTRMUTL_C
#include "msdextern.h"
#undef _MSDSTRMUTL_C

/***************************************************************************
 * Function Name		: strm_fnd_free_connect_entry
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_fnd_free_connect_entry
 *				  Routine. This routine finds a free entry
 *				  from the stream table.
 * Additional comments		:
 ****************************************************************************/

MSD_HANDLE strm_fnd_free_connect_entry(pmercd_adapter_block_sT padapter)
{
	MD_HANDLE   i;

	MSD_FUNCTION_TRACE("strm_fnd_free_connect_entry", NO_PARAMETERS);

	// search the MsdStreamMapTable for a free entry 
	for(i=1; i < MSD_MAX_STREAM_ID; i++) {
		if( padapter->pstream_connection_list[i]== NULL) {
			// return the index as stream handle 
			return(i);
		 }
	}

	return(0);
}

/***************************************************************************
 * Function Name		: strm_fnd_entry_from_adapter
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_fnd_entry_from_adapter
 *				  Routine. This routine finds a stream entry
 *				  associated withspecified transaction Id.
 * Additional comments		:
 ****************************************************************************/
pmercd_stream_connection_sT strm_fnd_entry_from_adapter(pmercd_adapter_block_sT padapter, merc_uint_t  TransactionId)
{
	pmercd_stream_connection_sT   StreamBlock;

	MSD_FUNCTION_TRACE("strm_fnd_entry_from_adapter", TWO_PARAMETERS,
				(size_t)padapter, (size_t)TransactionId);


	// start with the strema block at the adapter stream list queue head
	StreamBlock = (pmercd_stream_connection_sT)
			         padapter->stream_adapter_list.QueueHead;

	// search the list until list is exhausted or a match is found 
	while(StreamBlock) {
		if(StreamBlock->TransactionId == TransactionId) {
			return(StreamBlock);
		}
		StreamBlock = StreamBlock->Next;
	}

	return(NULL);
}


/***************************************************************************
 * Function Name                : search_streamblock
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver strm_fnd_entry_from_adapter
 *                                Routine. This routine finds a stream entry
 *                                associated withspecified transaction Id.
 * Additional comments          :
 ****************************************************************************/
pmercd_stream_connection_sT search_streamblock(pmercd_open_block_sT MsdOpenBlock, MSD_HANDLE StreamHandle)
{
        pmercd_stream_connection_sT   StreamBlock;
        pmercd_bind_block_sT        BindBlock, OldBindBlock;


	MSD_FUNCTION_TRACE("search_streamblock", TWO_PARAMETERS,
				(size_t)MsdOpenBlock, (size_t)StreamHandle);

        // start with the strema block at the adapter stream list queue head
        BindBlock = (pmercd_bind_block_sT)
                    MsdOpenBlock->bind_block_q.QueueHead;

        // search the list until list is exhausted or a match is found
        while(BindBlock) {
              StreamBlock = BindBlock->stream_connection_ptr;
              if(StreamBlock) {
                if(StreamBlock->handle ==  StreamHandle) {
                        return(StreamBlock);
                  }
		}

		OldBindBlock =  BindBlock;
            
		BindBlock  = BindBlock->Next;
		
        }


        return(NULL);
}

/***************************************************************************
 * Function Name		: search_bindblock
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_fnd_entry_from_adapter
 *				  Routine. This routine finds a stream entry
 *				  associated withspecified transaction Id.
 * Additional comments		:
 ****************************************************************************/
#ifdef _8_BIT_INSTANCE
pmercd_bind_block_sT search_bindblock(pmercd_open_block_sT MsdOpenBlock, MSD_HANDLE BindHandle)
#else
pmercd_bind_block_sT search_bindblock(pmercd_open_block_sT MsdOpenBlock, MBD_HANDLE BindHandle)
#endif 

{
	pmercd_bind_block_sT        BindBlock, OldBindBlock;

	MSD_FUNCTION_TRACE("search_bindblock", TWO_PARAMETERS,
				(size_t)MsdOpenBlock, (size_t)BindHandle);

	// start with the strema block at the adapter stream list queue head
	BindBlock = (pmercd_bind_block_sT)
			         MsdOpenBlock->bind_block_q.QueueHead;

	// search the list until list is exhausted or a match is found 
	while(BindBlock) {
	      if(BindBlock->bindhandle == BindHandle) {
			return(BindBlock);
		}

		OldBindBlock =  BindBlock;
           
        	BindBlock  = BindBlock->Next;

	}

	return(NULL);
}



/***************************************************************************
 * Function Name		: strm_gather_snd_blks
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_gather_snd_blks
 *				  Routine. This routine is responsible for
 *				  queueing stream blocks on the padapter
 *				  queue. This routine checks the Stream Can
 *				  Take value and queues blocks to be sent to the
 *				  Adapter. It is also resposible for doing round
 *				  robin.
 * Additional comments		:
 ****************************************************************************/
void strm_gather_snd_blks(pmercd_adapter_block_sT padapter)
{
        pmercd_stream_connection_sT  StreamBlock;
        pmercd_stream_connection_sT  TempStreamBlock;
        PSTRM_MSG       CurrentTail;
        PSTRM_MSG       Msg;
        PSTRM_MSG       TmpMsg;
#ifndef LFS
        PSTRM_MSG       CurMsg;
#endif
        PMDRV_MSG       MdMsg;
        PSTREAM_SEND    Ptr;
        merc_uint_t     MsgDataSize;
#ifndef LFS
        merc_uint_t     AllowCount = 5;
#endif
	merc_uchar_t    *Buffer=NULL;

        MSD_FUNCTION_TRACE("strm_gather_snd_blks", ONE_PARAMETER, (size_t)padapter);

	// search the adapter stream list and link all the outgoing blocks

	CurrentTail = strm_fnd_msg_lst_tail(padapter->snd_data_queue);

	StreamBlock = (pmercd_stream_connection_sT)padapter->stream_adapter_list.QueueHead;


	if (StreamBlock == NULL)  {
		return;
	}

	MSD_LEVEL2_DBGPRINT("StreamGatherSendBlocks: CanTake %d Id %d\n",
		 StreamBlock->can_take, StreamBlock->id);

	while(StreamBlock) {
		if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) {	
			StreamBlock = StreamBlock->Next;
			continue;
		}
		if (StreamBlock->SendStreamQueue == NULL) {
			StreamBlock = StreamBlock->Next;
			MSD_LEVEL2_DBGPRINT(
				"StreamGatherSendBlocks: SendStreamQueue NULL\n");
			continue;
		}
		// must be in connected state for sending 
		if( (StreamBlock->state == MERCD_STREAM_STATE_NOT_OPENED)  
		   || (StreamBlock->state == MERCD_STREAM_STATE_OPEN_PEND) ) {
			printk("StreamGatherSendBlocks: StreamId=%d, "
					"bad state =%d, Not sending\n",
					StreamBlock->id,StreamBlock->state);
			StreamBlock = StreamBlock->Next;
			continue;
		}

		MSD_LEVEL2_DBGPRINT("StreamGatherSendBlocks: Sending StreamId %d\n",
			StreamBlock->id);

		// get the first Msg check size and pass to snd_data_queue 
		Msg = StreamBlock->SendStreamQueue;

		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
		Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

		if ((StreamBlock->state == MERCD_STREAM_STATE_BROKEN) 
		/* || (StreamBlock->StreamState == MERCD_STREAM_STATE_CLOSE_PEND)  PTR 1979 */

		 || (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND) ){
			if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
				strm_free_only_data_msgs(StreamBlock);
				if (StreamBlock->SendStreamQueue == NULL) {
					StreamBlock = StreamBlock->Next;
					MSD_LEVEL2_DBGPRINT(
						"StreamGSndBlocks: SendStreamQueue NULL\n");
					continue;
				}
				Msg = StreamBlock->SendStreamQueue;

				MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
				Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
			}
     		}

	// Only EOS block does not have user header.
        if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
            if(StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
                MsgDataSize = MsdMessageDataSize(Msg);
                MSD_ASSERT(MsgDataSize <= 4056);
            }else
                MsgDataSize = StreamBlock->ContainerSize; //4056;
        } else {
            MsgDataSize = 0;
        }

		// Assert if data size > 4056
		MSD_ASSERT(MsgDataSize <= 4056);

		MSD_LEVEL2_DBGPRINT("StreamGatherSendBlocks:Before:CanTakes Datasize %d,"
			"CanTake %d\n", MsgDataSize, 
			StreamBlock->can_take);

        if ( ((!(Ptr->Flags & STREAM_FLAG_EOS))
         && (MsgDataSize > StreamBlock->can_take)) || !StreamBlock->can_take) { 
            StreamBlock = StreamBlock->Next;
            continue;
        }

	// Concatanation need for >4K blocks - speeds up streaming
	if ((StreamBlock->cantakeMode) && (MsdMessageDataSize(Msg) < MERCURY_HOST_IF_BLK_SIZE) && (Msg->b_next)) {
              TmpMsg = Msg;
              MsgDataSize = 0;
              while (TmpMsg) {
                 if (!Buffer) {
                     mercd_zalloc(Buffer, merc_uchar_t*,MERCURY_HOST_IF_BLK_SIZE);
                     if (Buffer == NULL) {
                         printk("StreamSendData: Unable to allocate memory\n");
                         return;
                     }
                 }

                 if (!TmpMsg->b_cont) {
                     MSD_LEVEL2_DBGPRINT("StreamSendData: No data on this message\n");
                     TmpMsg = TmpMsg->b_next;
                     continue;
                 }

                 // do no concat end of stream
                 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(TmpMsg);
                 Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
                 if (Ptr->Flags & MD_MSG_FLAG_LATENCY) {
                     break;
                 }

		 // need to add a check for data size > block size
		 if ((TmpMsg->b_cont->b_datap->db_size + MsgDataSize) > 4056) {
		     printk("concat: too much data on this stream - do it later");
		     break;     
	  	 } else {
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
              }

              MSD_FREE_KERNEL_MEMORY(Msg->b_cont->b_datap->db_base, Msg->b_cont->b_datap->db_size);
              Msg->b_cont->b_datap->db_base = (unsigned char *)Buffer;
              Msg->b_cont->b_datap->db_lim = (char *)Buffer + MsgDataSize;
              Msg->b_cont->b_datap->db_size = MsgDataSize;
              Msg->b_cont->b_rptr = (unsigned char *) Buffer;
              Msg->b_cont->b_wptr = (unsigned char *) Msg->b_cont->b_rptr + MsgDataSize;
          }

		// Isolate the message from the SendStreamQueue
		TmpMsg = Msg->b_next;
		Msg->b_next = NULL;
		StreamBlock->SendStreamQueue = TmpMsg;

		StreamBlock->qparam.cur_snd_cnt -= MsgDataSize;

		if (!(Ptr->Flags & STREAM_FLAG_EOS))
            	    StreamBlock->can_take -= MsgDataSize;
		
		// Put that Message on the AdapterSend queue
		if(CurrentTail == NULL) {
			padapter->snd_data_queue = Msg;
		} else {
			CurrentTail->b_next = Msg;
		}

		CurrentTail = strm_fnd_msg_lst_tail(padapter->snd_data_queue);

		MSD_LEVEL2_DBGPRINT("StreamGatherSendBlocks:After: CanTakes Datasize %d,"
					" CanTake %d\n", MsgDataSize, 
					StreamBlock->can_take);

		if ((int)StreamBlock->can_take < 0) {
#ifdef LFS
			printk("DispatchCloseStream: CurrentSendCount %lu, "
				"BlockSize %d, CanTake %d\n", 
				StreamBlock->qparam.cur_snd_cnt, 
				MERCURY_HOST_IF_BLK_SIZE, 
				StreamBlock->can_take);
#else
			printk("DispatchCloseStream: CurrentSendCount %d, "
				"BlockSize %d, CanTake %d\n", 
				StreamBlock->qparam.cur_snd_cnt, 
				MERCURY_HOST_IF_BLK_SIZE, 
				StreamBlock->can_take);
#endif
		}

		// update the stats
		StreamBlock->stats.snd_count += 1;
		MsdControlBlock->MsdStatisticsBlock.TotalBlocksSent += 1;

		TempStreamBlock = StreamBlock;

		MSD_LEVEL2_DBGPRINT("Before: ASL %x\n", padapter->snd_data_queue);

		// Rotate the snd_data_queue 

		queue_remove_from_Q(&padapter->stream_adapter_list, 
				(PMSD_GEN_MSG)TempStreamBlock);

		queue_put_Q(&padapter->stream_adapter_list, 
			(PMSD_GEN_MSG)TempStreamBlock);



		StreamBlock = 
			(pmercd_stream_connection_sT) padapter->stream_adapter_list.QueueHead;

		MSD_LEVEL2_DBGPRINT("After: ASL %x\n", padapter->stream_adapter_list);
	} // While

	// reset the data pendign flag only if we've read all the stream blocks
	if( (strm_cal_snd_cnt(padapter->stream_adapter_list.QueueHead)) == 0)  {
		padapter->flags.SendDataPending &= ~MERC_ADAPTER_FLAG_SEND_DATA_PEND;
	}

}

/***************************************************************************
 * Function Name		: strm_adv_gather_snd_blks
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_adv_gather_snd_blks
 *				  Routine. This routine is responsible for
 *				  queueing stream blocks on the padapter queue.
 *				  This routine checks the Stream Can Take value and
 *				  queues blocks to be sent to the Adapter. This
 *				  routine is called when the Block comes down from
 *				  the use and hence does handle that particluar 
 * 				  Adapter and StreamBlock
 * Additional comments		:
 ****************************************************************************/
void strm_adv_gather_snd_blks(pmercd_adapter_block_sT padapter, 
pmercd_stream_connection_sT StreamBlock)
{
    PSTRM_MSG   CurrentTail;
    PSTRM_MSG   Msg;
    PSTRM_MSG   TmpMsg;
    PMDRV_MSG   MdMsg; 
    PSTREAM_SEND Ptr;
    merc_uint_t MsgDataSize;
    merc_uint_t AllowCount = 5;

    MSD_FUNCTION_TRACE("strm_adv_gather_snd_blks", TWO_PARAMETERS,
            (size_t)padapter, (size_t)StreamBlock);

	MSD_ASSERT(padapter);
	MSD_ASSERT(StreamBlock);
 	MSD_ASSERT(!(StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY));

	MSD_LEVEL2_DBGPRINT("StreamGatherAdvanceSendBlocks: CanTake %d Id %d\n",
			 StreamBlock->can_take, StreamBlock->id);

	if (StreamBlock->SendStreamQueue== NULL) {
		return;
	}

	// must be in connected state for sending 
	if( (StreamBlock->state == MERCD_STREAM_STATE_NOT_OPENED)  
	   || (StreamBlock->state == MERCD_STREAM_STATE_OPEN_PEND) ) {
		MSD_LEVEL2_DBGPRINT("StreamGatherSendBlocks: StreamId=%d, "
				"bad state =%d, Not sending\n",
			StreamBlock->id,StreamBlock->state);
		return;
	}

	CurrentTail = strm_fnd_msg_lst_tail(padapter->snd_data_queue);

	MSD_LEVEL2_DBGPRINT("StreamAdvanceGatherSendBlocks: Sending StreamId %d\n",
			StreamBlock->id);

	// get the first Msg check size and pass to snd_data_queue 
	while( (Msg = StreamBlock->SendStreamQueue) && (--AllowCount) ) {

		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
		Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
	
		// Only EOS block does not have user header.
        if ( Ptr->Flags & STREAM_FLAG_EOS ) {
            MsgDataSize = 0; // ptr19424 fixes
        }
        else  {

            if(StreamBlock->type == STREAM_OPEN_F_GSTREAM)
                MsgDataSize = MsdMessageDataSize(Msg);
            else
                MsgDataSize = StreamBlock->ContainerSize; // ptr19424 fixes

            // Assert if data size > 4056
            MSD_ASSERT(MsgDataSize <= 4056);
        }

		// Assert if data size > 4056
		MSD_ASSERT(MsgDataSize <= 4056);

		if ((!(Ptr->Flags & STREAM_FLAG_EOS)) &&
			(MsgDataSize > StreamBlock->can_take) ) {
			return;
		}

		MSD_LEVEL2_DBGPRINT("StreamAdvanceGatherSendBlocks:Before:CanTakes "
			"Datasize %d, CanTake %d\n", MsgDataSize, 
			 StreamBlock->can_take);

		// Isolate the message from the SendStreamQueue
		TmpMsg = Msg->b_next;
		Msg->b_next = NULL;
		StreamBlock->SendStreamQueue = TmpMsg;

		StreamBlock->qparam.cur_snd_cnt -= MsgDataSize;


		StreamBlock->can_take -= MsgDataSize;

		// Put that Message on the AdapterSend queue
		if(CurrentTail == NULL) {
			padapter->snd_data_queue = Msg;
		} else {
			CurrentTail->b_next = Msg;
		}

		CurrentTail = strm_fnd_msg_lst_tail(padapter->snd_data_queue);

		MSD_LEVEL2_DBGPRINT("StreamAdvanceGatherSendBlocks:After: CanTakes "
			"Datasize %d, CanTake %d\n", MsgDataSize, 
			StreamBlock->can_take);

		if ((int)StreamBlock->can_take < 0) {
#ifdef LFS
			printk("DispatchCloseStream: CurrentSendCount %lu, "
				"BlockSize %d, CanTake %d\n", 
				StreamBlock->qparam.cur_snd_cnt, 
				MERCURY_HOST_IF_BLK_SIZE, 
				StreamBlock->can_take);
#else
			printk("DispatchCloseStream: CurrentSendCount %d, "
				"BlockSize %d, CanTake %d\n", 
				StreamBlock->qparam.cur_snd_cnt, 
				MERCURY_HOST_IF_BLK_SIZE, 
				StreamBlock->can_take);
#endif
		}

		// update the stats
		StreamBlock->stats.snd_count += 1;
		MsdControlBlock->MsdStatisticsBlock.TotalBlocksSent += 1;

	} // While
	return;
}

/***************************************************************************
 * Function Name		: strm_ack2usr
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_ack2usr Routine.
 *				  This routine sends a can take to User in 
 *				  form of a SendAck.
 * Additional comments		:
 ****************************************************************************/
md_status_t strm_ack2usr(pmercd_stream_connection_sT StreamBlock, merc_uint_t BytesSent)
{

	PSTRM_MSG   Msg;
    PMDRV_MSG   MdMsg;
    PSTREAM_SEND_ACK    Ptr;

    MSD_FUNCTION_TRACE("strm_ack2usr", TWO_PARAMETERS,
                    (size_t)StreamBlock,
                    (size_t)BytesSent);


	if(StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) 
	   return(MD_SUCCESS);

//////////////////////////////////////////////////////////////////////
#ifdef ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS
     if (StreamBlock->WWMode)
      {
        MD_STATUS Status = MD_SUCCESS;
        Status = mid_wwmgr_check_abnormal_termination(StreamBlock);
        if (Status == MD_FAILURE)
         {
           printk("strm_ack2usr: Abnormal Termination in ...\n");
           return (MD_FAILURE);
         }
      }
#endif /* ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS */
//////////////////////////////////////////////////////////////////////

	// allocate a MDRV_MSG message 
	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_SEND_ACK),GFP_ATOMIC);
	if(Msg == NULL) {
		printk("strm_ack2usr:cannot allocate msg.\n");
		return(MD_FAILURE);
	}

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

	// setup the MID_STREAM_SEND_ACK message 
	MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
	MD_SET_MDMSG_ID(MdMsg, MID_STREAM_SEND_ACK);

	// LA: Fri Aug 31 14:57:06 EDT 2001
	// The order for setting the stream handle before setting the bind handle
	// is imperative for the stream group api change in the libqhost.so.
	MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
	MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle );

	MSD_ASSERT(StreamBlock->pbind_block);
	MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);
	MdMsg->MessageFlags |= MD_MSG_FLAG_ASYNC;

	Ptr = (PSTREAM_SEND_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);

	// Ensure multiple of 4056 
	Ptr->CanTakeByteCount = BytesSent;

	// send it to the user
	supp_process_receive(MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(StreamBlock->pbind_block),
 				Msg);
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: strm_sndbrokenstream2usr
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_sndbrokenstream2usr Routine.
 *				  This routine sends a borken stream to User  
 * Additional comments		:
 ****************************************************************************/
md_status_t strm_sndbrokenstream2usr(pmercd_stream_connection_sT StreamBlock)
{

        PSTRM_MSG   Msg;
        PMDRV_MSG   MdMsg;
        PSTREAM_SESS_CLOSE_ACK  Ptr;

        if (StreamBlock->state != MERCD_STREAM_STATE_CONNECTED) {
           return(MD_SUCCESS);
        }

        // allocate a MDRV_MSG message
        Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_SESS_CLOSE_ACK),GFP_ATOMIC);
        if (Msg == NULL) {
            printk("strm_sndbrokenstream2usr :cannot allocate msg.\n");
            return(MD_FAILURE);
        }

        MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

        // setup the MID_STREAM_SEND_ACK message
        MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
        MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);

        // LA: Fri Aug 31 14:57:06 EDT 2001
        // The order for setting the stream handle before setting the bind handle
        // is imperative for the stream group api change in the libqhost.so.
        MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
        MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle );

        MSD_ASSERT(StreamBlock->pbind_block);
        MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);
        MdMsg->MessageFlags |= MD_MSG_FLAG_ASYNC;

        Ptr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);

        // Ensure multiple of 4056
        Ptr->StreamId = StreamBlock->id;

        StreamBlock->state = MERCD_STREAM_STATE_BROKEN;

     	supp_process_receive(MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(StreamBlock->pbind_block), Msg);

	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: strm_cal_snd_cnt
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_cal_snd_cnt Routine. This
 *				  routine calculates send count.
 * Additional comments		:
 ****************************************************************************/
merc_uint_t strm_cal_snd_cnt(pmercd_stream_connection_sT StreamBlockQueue)
{
    pmercd_stream_connection_sT   StreamBlock;
    merc_uint_t    SendCount= 0;

	MSD_FUNCTION_TRACE("strm_cal_snd_cnt", ONE_PARAMETER, (size_t)StreamBlockQueue);

	StreamBlock = (pmercd_stream_connection_sT)StreamBlockQueue;

	while(StreamBlock != NULL){
		if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) {
			StreamBlock = StreamBlock->Next;
			continue;
		}else  {
			SendCount += StreamBlock->qparam.cur_snd_cnt;
		}
		StreamBlock = StreamBlock->Next;
	}
	return(SendCount);
}


/***************************************************************************
 * Function Name		: strm_put_data_strm_Q
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_put_data_strm_Q Routine.
 *				  This routine queues the Message on the
 *				  StreamBlock
 * Additional comments		:
 ****************************************************************************/
void strm_put_data_strm_Q(pmercd_stream_connection_sT StreamBlock,PSTRM_MSG Msg)
{
	PSTRM_MSG	Tail; 

	 MSD_FUNCTION_TRACE("StreamBlock", TWO_PARAMETERS,
                    (size_t)StreamBlock,
                    (size_t)Msg);

	if((Tail=StreamBlock->SendStreamQueue)==NULL){
		StreamBlock->SendStreamQueue = Msg;
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
 * Function Name		: strm_free_only_data_msgs
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_free_only_data_msgs Routine.
 *				  This routine removes all data messages from the
 *				  queue and keeps only the EOS message on the queue.
 * Additional comments		:
 ****************************************************************************/
void strm_free_only_data_msgs(pmercd_stream_connection_sT StreamBlock)
{
	PSTRM_MSG    Msg;
    PSTRM_MSG    TmpMsg;
    PMDRV_MSG    MdMsg;
    PSTREAM_SEND Ptr;

    MSD_FUNCTION_TRACE("strm_free_only_data_msgs", ONE_PARAMETER,
            (size_t)StreamBlock);

	if(StreamBlock->SendStreamQueue==NULL){
		return;
	}

	while ((Msg = StreamBlock->SendStreamQueue)) {
		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
		Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

		if (Ptr->Flags & STREAM_FLAG_EOS)
			break;
		
		TmpMsg = Msg->b_next;
		Msg->b_next = NULL;
		StreamBlock->SendStreamQueue = TmpMsg;
		MSD_FREE_MESSAGE(Msg);
	}
	return;
}


/***************************************************************************
 * Function Name		: strm_free_srm_msgs
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_free_srm_msgs Routine.
 *				  This routine removes all data messages from the
 *				  queue and keeps only the EOS message on the queue.
 * Additional comments		:
 ****************************************************************************/
void strm_free_srm_msgs(pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock)
{

	PSTRM_MSG   Msg;
    PSTRM_MSG   Last = NULL;
    PSTRM_MSG   Next = NULL;
    PMDRV_MSG   MdMsg;
    PSTREAM_SEND Ptr;

    MSD_FUNCTION_TRACE("strm_free_srm_msgs", TWO_PARAMETERS,
        (size_t)padapter, (size_t)StreamBlock);

	if(padapter->snd_data_queue==NULL)
		return;
 
	Msg = padapter->snd_data_queue;

	while (Msg) {
		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
		Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
		if (Ptr->StreamId != StreamBlock->id) {
			Last = Msg;
			Msg = Msg->b_next;
			continue;
		}
		Next = Msg->b_next;
		if (Last)
			Last->b_next = Next;
		Msg->b_next = NULL;
		MSD_FREE_MESSAGE(Msg);
		Msg = Next;
	}
}

/***************************************************************************
 * Function Name		: strm_Q_snd_msg
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver QueueSendMessage Routine.
 *				  This routine queues a send message
 * Additional comments		:
 ****************************************************************************/
md_status_t strm_Q_snd_msg(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter)
{

    	MSD_FUNCTION_TRACE("strm_Q_snd_msg", TWO_PARAMETERS,
                (size_t)Msg,
                (size_t)padapter);

	if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
	    return(MD_FAILURE);
	}

	// adapter mutext already obtained by caller 
	// add message to the send queue 
	queue_put_msg_Q(&padapter->snd_msg_queue, Msg);

	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: strm_fnd_msg_lst_tail
 * Function Type		: Support Function for Stream Manager
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver strm_fnd_msg_lst_tail Routine.
 *			 	  This routine finds the tail of a list specified
 * Additional comments		:
 ****************************************************************************/
PSTRM_MSG strm_fnd_msg_lst_tail(PSTRM_MSG List)
{

	MSD_FUNCTION_TRACE("strm_fnd_msg_lst_tail", ONE_PARAMETER, (size_t)List);

	if(List == NULL)
		return(NULL);

	while(List->b_next != NULL) 
		List = List->b_next;
	return(List);
}
/***************************************************************************
 * Function Name                : 
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : 
 *
 * Additional comments          :
 ****************************************************************************/
void  stream_add_cantake_queue (pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock)
{

    pmercd_stream_connection_sT   NodeStreamBlock;
    int nelements,i;

    MSD_FUNCTION_TRACE("stream_add_cantake_queue", TWO_PARAMETERS, 
                        (size_t)padapter, (size_t) StreamBlock);

    nelements = padapter->stream_cantake_list.nelements;

    for(i=0;i<nelements;i++) {

      NodeStreamBlock = (pmercd_stream_connection_sT)padapter->stream_cantake_list.Node[i];

      if(StreamBlock == NodeStreamBlock)
        return;
     }

    if(i < (MSD_MAX_STREAM_ID - 2)  ) {
      padapter->stream_cantake_list.Node[nelements] = StreamBlock;
      padapter->stream_cantake_list.nelements = nelements + 1;
    }  else {
      //      printk("Stream is FULL **** NOTICE ....... Loosing the cantake...  Streamblock id is %d cantake is %d \n", StreamBlock->id, StreamBlock->accumulated_usr_ack);
     }

}


void strm_snd_can_takes(pmercd_adapter_block_sT padapter)
{
   pmercd_stream_connection_sT   StreamBlock;
   int i, nelements;

  MSD_FUNCTION_TRACE("strm_snd_can_takes", ONE_PARAMETER, 
                        (size_t)padapter );
   nelements = padapter->stream_cantake_list.nelements;

   for(i=0;i<nelements;i++) {

      StreamBlock = (pmercd_stream_connection_sT)padapter->stream_cantake_list.Node[i];

      if(StreamBlock->accumulated_usr_ack)  {
               if (strm_ack2usr(StreamBlock, StreamBlock->accumulated_usr_ack) != MD_SUCCESS)
                    printk("strm_ack2usr failed.\n");


        StreamBlock->accumulated_usr_ack = 0;   /* clean it out. */
      }

     padapter->stream_cantake_list.Node[i] = NULL;
 }

    padapter->stream_cantake_list.nelements = 0;
}

/***************************************************************************
 * Function Name                : push_trace_msg
 * Function Type                : manager function
 * Inputs                       : Msg,
 *                                padapter
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void push_trace_msg(pmercd_adapter_block_sT padapter, pmercd_bind_block_sT BindBlock, merc_uint_t MsgType) {
      PSTRM_MSG        SendMsg;
      PSTRM_MSG        DataMsg;
      PMERC_HOSTIF_MSG MercMsg;

      if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {

         SendMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
         if (!SendMsg) {
             printk("push_trace_msg: unable to allocate memory for trace msg %#x\n", MsgType);
             return;
         }
         DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+4,GFP_ATOMIC);
         if (!DataMsg) {
             MSD_FREE_MESSAGE(SendMsg);
             printk("push_trace_msg: unable to allocate memory for trace msg %#x\n", MsgType);
             return;
         }
         MD_SET_MSG_TYPE(DataMsg, M_DATA);
         MSD_LINK_MESSAGE(SendMsg,DataMsg);
         MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
         MERCURY_SET_BODY_SIZE(MercMsg,4);
         MERCURY_SET_DEST_COMPINST(MercMsg, 0);
         MERCURY_SET_MESSAGE_TYPE(MercMsg,MsgType);
         MERCURY_SET_TRANSACTION_ID(MercMsg,BindBlock->bindhandle);
         *(int *)MERCURY_BODY_START(MercMsg) = 0x5a5a;
         supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)MercMsg);
         MSD_FREE_MESSAGE(SendMsg);
       }

}

