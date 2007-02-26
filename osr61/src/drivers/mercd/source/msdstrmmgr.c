/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdstrmmgr.c
 * Description			: Stream Manager functions 
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDSTRMMGR_C_
#include "msdextern.h"
#undef _MSDSTRMMGR_C_
extern int NewCanTakeProtocol; 


merc_uint_t vopen = 0xffff;
merc_uint_t vopen1 = 0xffff;


/***************************************************************************
 * Function Name		: mid_strmmgr_send_low;
 * Function Type		: manager function
 * Inputs			: padapter, Streamnblock, MsdOpenBlock,	Msg
 * Outputs			: none
 * Calling functions	        :
 * Description			:
 * Additional comments	        : 
 ****************************************************************************/
void mid_strmmgr_send_low(pmercd_adapter_block_sT padapter, 
	           pmercd_stream_connection_sT StreamBlock, 
		   pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg )
{
	PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_ulong_t		ErrorCode = MD_OK;
	merc_ulong_t		ReturnValue = 0;
	merc_ulong_t	        MessageDataSize = 0;
	PSTREAM_SEND 		Ptr;
	PSTRM_MSG		DataMsg;
	MSD_HANDLE		StreamHandle;

	MSD_FUNCTION_TRACE("mid_strmmgr_send", TWO_PARAMETERS,
					(size_t)MsdOpenBlock, (size_t)Msg);

	// Init the driver with the message parameters
	Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);

	//  Get Streamhandle
	StreamHandle = MD_GET_MDMSG_STREAM_HANDLE(MdMsg);

        if( !( Ptr->Flags & STREAM_FLAG_EOS ) ) {
            if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
			 if(Msg->b_cont)
		   		 MessageDataSize = MsdMessageDataSize(Msg->b_cont);
	    }else
                MessageDataSize = StreamBlock->ContainerSize;
        }
        
#if 0 

        if ( (((StreamBlock->qparam.cur_snd_cnt + MessageDataSize ) >
             StreamBlock->qparam.snd_queue_limit)) && (MessageDataSize != 0) ) {

             if (Ptr->Flags & STREAM_FLAG_EOS) {
                 printk("MsdDownStreamDataStrategy: EOS on Nonzero block\n");
             } else {
                 printk("ID:%d CD_ERR_SEND_Q_OVERFLOW StreamBlock->qparam.cur_snd_cnt is %d, StreamBlock->qparam.snd_queue_limit is %d\n", StreamBlock->id, StreamBlock->qparam.cur_snd_cnt, StreamBlock->qparam.snd_queue_limit);
		 printk("Sending back ABCD to cheetah for stream id %d handle is %d now \n", StreamBlock->id, StreamHandle);
		 ErrorCode = CD_ERR_SEND_Q_OVERFLOW;
                 DataMsg = MD_GET_MSG_B_CONT(Msg);
                 MSD_FREE_BUFFER(DataMsg);
                 MD_SET_MSG_B_CONT(Msg, NULL);
                 goto out;
             }
        }
#endif

	// link a null block to the control msg if no data are attached
	// this is done because next code expects a data block for all stream
	// send messages
	
	if (MD_GET_MSG_B_CONT(Msg)==NULL){
	    DataMsg = mercd_streams_allocator(16, MERCD_STREAMS_BUFFER, MERCD_FORCE);
		
	    if (DataMsg == NULL) {
                cmn_err(CE_WARN, "DownStreamDataStrategy:cannot allocate data msg.\n");
		ErrorCode = CD_ERR_NO_MEMORY;
		goto out;
	    }
	    
	    MD_SET_MSG_TYPE(DataMsg,M_DATA);
	    MD_SET_MSG_WRITE_PTR(DataMsg, 
	    MD_GET_MSG_READ_PTR(DataMsg));
	    MSD_LINK_MESSAGE(Msg,DataMsg);
	}

	// link the message to this stream's send queue

	Ptr->StreamId = StreamBlock->id;

	if (Ptr->Flags & STREAM_FLAG_EOS) {
		if (Ptr->Flags & (MULTI_BLK_NO_CAN_TAKE|MULTI_BLK_SEND_CAN_TAKE)) {
			MSD_ASSERT(0);
		}
	}

	strm_put_data_strm_Q(StreamBlock, Msg);

	StreamBlock->qparam.cur_snd_cnt +=  MessageDataSize;


        ///////////////////////////////////////////////////////////
        // WW Streaming Changes
        ///////////////////////////////////////////////////////////
        if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
             padapter->flags.SendDataPending |= MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND;
        } else {
	    strm_adv_gather_snd_blks(padapter, StreamBlock);
	    padapter->flags.SendDataPending |= MERC_ADAPTER_FLAG_SEND_DATA_PEND;
        }

	return;

out:
	// send back an error message
	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg),MERCD_STREAMS_BUFFER, MERCD_FORCE);
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// for async messages, send back a async reply
	if (MD_IS_MSG_ASYNC(Msg)) {
	    // msg is freed in the send function 
	    supp_process_receive(MsdOpenBlock, Msg);
	} else {
	    // sync msg uses the sync return path
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	return;
}


/***************************************************************************
 * Function Name		: mid_strmmgr_send
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_send(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
	PMDRV_MSG			MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_ulong_t		     	ErrorCode = MD_OK;
	merc_ulong_t			ReturnValue = 0;
#ifndef LFS
	merc_ulong_t	    		MessageDataSize = 0;
#endif
	PSTREAM_SEND 			Ptr;
#ifndef LFS
	PSTRM_MSG			DataMsg;
	MSD_HANDLE			StreamHandle;
#endif
	pmercd_stream_connection_sT 	StreamBlock;
	pmercd_adapter_block_sT		padapter;
        merc_uint_t                     StreamId = 0;
        pmercd_bind_block_sT            BindBlock;
#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                      BindHandle = 0;
#else
        MBD_HANDLE                      BindHandle = 0;
#endif

	// Init the driver with the message parameters
	Ptr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
	StreamId = Ptr->StreamId;
        BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

	MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

	BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
	StreamBlock = BindBlock->stream_connection_ptr;
	
	// Validity Checking    
        if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
            printk("mid_strmmgr_send: NULL StreamBlock for id %d & bh %d\n", StreamId, BindHandle);
            ErrorCode = CD_ERR_BAD_STREAM_HANDLE;  
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);    
            goto out;  
        }

	padapter = StreamBlock->padapter_block; 

	if (padapter == NULL) {
	    MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);	
            printk("mid_strmmgr_send: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            goto out;
        }
	
	MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

	if ((StreamBlock->state == MERCD_STREAM_STATE_NOT_OPENED) || 
	    (StreamBlock->state == MERCD_STREAM_STATE_OPEN_PEND) ) {
	     printk("mid_strmmgr_send: non-open state=%d, Sid=%d\n", StreamBlock->state,StreamBlock->id);
	     ErrorCode = CD_ERR_BAD_STREAM_STATE;
	     ReturnValue = StreamBlock->state;
	     goto out;
	}

	if (((StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND) ||
	     (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND)) && 
			       (!(Ptr->Flags & STREAM_FLAG_EOS)) ) {
	    printk("mid_strmmgr_send: closed-state state=%d, Sid=%d \n", StreamBlock->state,StreamBlock->id);
	    ErrorCode = CD_ERR_BAD_STREAM_STATE;
	    ReturnValue = StreamBlock->state;
	    goto out;
	}

	if (StreamBlock->state == MERCD_STREAM_STATE_BROKEN) {
	    // Just free the message and return
	    mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, 1);
	    return;
	}

        MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	mid_strmmgr_send_low( padapter, StreamBlock, MsdOpenBlock , Msg) ;
	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex); 

	return;

out:
	// send back an error message
	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg),MERCD_STREAMS_BUFFER, MERCD_FORCE);
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// for async messages, send back a async reply
	if (MD_IS_MSG_ASYNC(Msg)) {
	    // msg is freed in the send function 
	    supp_process_receive(MsdOpenBlock, Msg);
	} else {
	    // sync msg uses the sync return path
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	return;
}


/***************************************************************************
 * Function Name		: mid_strmmgr_mblk_split_send
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
int mid_strmmgr_mblk_split_send(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg, char *buffer, size_t size)
{

	PMDRV_MSG			MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PSTREAM_MBLK_SEND 		Ptr;
	PSTREAM_MBLK_SEND_ENTRY 	EntryPtr;
	PSTRM_MSG			DataMsg;
#ifndef LFS
	PSTRM_MSG			NewDataMsg;
	MSD_HANDLE			StreamHandle;
#endif
	pmercd_stream_connection_sT	StreamBlock;
	pmercd_adapter_block_sT		padapter;
	PSTRM_MSG   			SendMsg;
	PMDRV_MSG   			SendMdMsg;
	PSTREAM_SEND 			SendPtr;
	merc_uint_t 			cnt, buffer_count = 0;
	merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
#ifndef LFS
        unsigned char                  *savedbuffer = NULL;
#endif
        merc_uint_t                     StreamId = 0;
        pmercd_bind_block_sT            BindBlock;
#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                      BindHandle = 0;
#else
        MBD_HANDLE                      BindHandle = 0;
#endif

	// Init the driver with the message parameters
	Ptr = (PSTREAM_MBLK_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
	StreamId = Ptr->StreamId;
        BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);


	MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

	BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
        StreamBlock = BindBlock->stream_connection_ptr;

        // Validity Checking
        if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
            printk("mid_strmmgr_mblk_split_send: NULL StreamBlock for id %d & bh %d\n", StreamId, BindHandle);
            ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
            goto out;
        }

        padapter = StreamBlock->padapter_block;
	
	if (padapter == NULL) {
	    MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);	
            printk("mid_strmmgr_mblk_split_send: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            goto out;
        }

	MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

	if ((StreamBlock->state == MERCD_STREAM_STATE_NOT_OPENED) || 
	    (StreamBlock->state == MERCD_STREAM_STATE_OPEN_PEND) ) {
	     printk("mid_strmmgr_mblk_split_send: non-opened state=%d, Sid=%d \n", StreamBlock->state,StreamBlock->id);
	     ErrorCode = CD_ERR_BAD_STREAM_STATE;
	     ReturnValue = StreamBlock->state;
	     goto out;
	}

        if (((StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND) ||
             (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND))) {
            printk("mid_strmmgr_mblk_split_send: closed-state state=%d, Sid=%d \n", StreamBlock->state,StreamBlock->id);
            ErrorCode = CD_ERR_BAD_STREAM_STATE;
            ReturnValue = StreamBlock->state;
            goto out;
        }

	Ptr->StreamId = StreamBlock->id;

        buffer_count = 0;

	// Split the MultiBlk message into many single block messages
	for (cnt = 1; cnt <= Ptr->BlockCount; cnt++) {

	  EntryPtr = (PSTREAM_MBLK_SEND_ENTRY)(&(Ptr->Entry[cnt-1]));
	  // Create STREAM_SEND message
   	  SendMsg = mercd_streams_allocator( sizeof(MDRV_MSG)+sizeof(STREAM_SEND), MERCD_STREAMS_BUFFER, MERCD_FORCE);

   	  if (SendMsg== NULL) {
	     printk("mid_strmmgr_mblk_split_send: alloc buffer failed.\n");
             return -EINVAL;
   	  }

#ifdef LFS
	  DataMsg = allocb(EntryPtr->ByteCount, BPRI_MED);
	  if (!DataMsg) {
		  printk("mid_strmmg_mblk_split_send: Unable to allocate buffer \n");
		  return (-1);
	  }
	  MsdCopyMemory(((char *)buffer+buffer_count), DataMsg->b_rptr, EntryPtr->ByteCount);
	  DataMsg->b_wptr += EntryPtr->ByteCount;

#else
          savedbuffer = mercd_allocator(EntryPtr->ByteCount);
          if (savedbuffer) {
              DataMsg = native_alloc_msg_desr(EntryPtr->ByteCount, (char *)savedbuffer);
   	     if (!DataMsg) {
                printk("mid_strmmgr_mblk_split_send: alloc data buffer failed \n");
		mercd_free(savedbuffer, EntryPtr->ByteCount, MERCD_FORCE);
       		return -EINVAL;
   	     }

       	     MsdCopyMemory(((char *)buffer+buffer_count), savedbuffer, EntryPtr->ByteCount);
          } else {
             printk("mid_strmmgr_mblk_split_send: Unable to allocate buffer \n");
             return(-1);
          }
#endif

   	  // now mimic a STREM_SEND using the newly allocated message
   	  SendMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(SendMsg);

	  // Copy the original MDRV_MSG to the new message
	  MsdCopyMemory((PCHAR)MdMsg, (PCHAR)SendMdMsg, sizeof(MDRV_MSG));

   	  SendPtr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(SendMdMsg);
	  SendPtr->StreamId = Ptr->StreamId;
		
	  // Copy User Header
	  if (StreamBlock->type != STREAM_OPEN_F_GSTREAM) {
    	      MsdCopyMemory((PCHAR)(&(EntryPtr->StreamUserHeader)), 
	        (PCHAR)(&(SendPtr->StreamUserHeader)), sizeof(USER_HEADER));
	  }
	   
	  if (cnt == Ptr->BlockCount) {
	      // PTR 24666 Allow sending chopped of packets from WriteMulti
	      if (EntryPtr->Flags & MULTI_BLK_SEND_CAN_TAKE)
	          SendPtr->Flags =  MULTI_BLK_SEND_CAN_TAKE;
	      else
	          SendPtr->Flags = MULTI_BLK_NO_CAN_TAKE;
			
	      // Put the block count
	      SendPtr->Flags |= (cnt & MULTI_BLK_CNT_MASK);
	  } else {
	      // Read pointer of the DataMsg, calculate Write pointer
              buffer_count += EntryPtr->ByteCount;
	      SendPtr->Flags = MULTI_BLK_NO_CAN_TAKE;
	  }

	  if (EntryPtr->Flags & MD_MSG_FLAG_LATENCY) {
              SendPtr->Flags |= MD_MSG_FLAG_LATENCY;
	  }

	  MSD_LINK_MESSAGE(SendMsg, DataMsg);
	
	  MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
   	  mid_strmmgr_send_low(padapter, StreamBlock, MsdOpenBlock,  SendMsg);
	  MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex); 	   
        }

	// Free the original message
	mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, 1);

	return(MD_SUCCESS);

out:

	// send back an error message
	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg), MERCD_STREAMS_BUFFER,MERCD_FORCE);
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// for async messages, send back a async reply
	if (MD_IS_MSG_ASYNC(Msg)) {
	    // msg is freed in the send function 
	    supp_process_receive(MsdOpenBlock, Msg);
	} else { 
	    // sync msg uses the sync return path 
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		  : mid_strmmgr_mblk_send
 * Function Type		  : manager function
 * Inputs			        : MsdOpenBlock,
 *				              Msg
 * Outputs			      : none
 * Calling functions	:
 * Description			  :			 
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_mblk_send(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg)
{

	PMDRV_MSG			MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PSTREAM_MBLK_SEND 		Ptr;
	PSTREAM_MBLK_SEND_ENTRY 	EntryPtr;
	PSTRM_MSG			DataMsg;
	PSTRM_MSG			NewDataMsg;
#ifndef LFS
	MSD_HANDLE			StreamHandle;
#endif
	pmercd_stream_connection_sT	StreamBlock;
	pmercd_adapter_block_sT		padapter;
	PSTRM_MSG   			SendMsg;
	PMDRV_MSG   			SendMdMsg;
	PSTREAM_SEND 			SendPtr;
	merc_uint_t 			cnt;
	merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
        merc_uint_t                     StreamId = 0;
        pmercd_bind_block_sT            BindBlock;
#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                      BindHandle = 0;
#else
        MBD_HANDLE                      BindHandle = 0;
#endif

	// Init the driver with the message parameters
	Ptr = (PSTREAM_MBLK_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
	StreamId = Ptr->StreamId;
        BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);
	
	MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

        BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
        StreamBlock = BindBlock->stream_connection_ptr;

        // Validity Checking
        if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
            printk("mid_strmmgr_mblk_send: NULL StreamBlock for id %d & bh %d\n", StreamId, BindHandle);
            ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
            goto out;
        }

        padapter = StreamBlock->padapter_block;
	
	if (padapter == NULL) {
	    MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);	
            printk("mid_strmmgr_mblk_send: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            goto out;
        }

	MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

	if ((StreamBlock->state == MERCD_STREAM_STATE_NOT_OPENED) || 
	    (StreamBlock->state == MERCD_STREAM_STATE_OPEN_PEND) ) {
	     printk("mid_strmmgr_mblk_send: non-opend state=%d, Sid=%d \n", StreamBlock->state,StreamBlock->id);
	     ErrorCode = CD_ERR_BAD_STREAM_STATE;
	     ReturnValue = StreamBlock->state;
	     goto out;
	}

	// INETERNAL_CLOSE cant be valid if the Data is coming down on the stream
	MSD_ASSERT(StreamBlock->state != MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND);

	Ptr->StreamId = StreamBlock->id;

	// Delink the original Datablock from original message
	DataMsg = unlinkb(Msg);

	MD_SET_MSG_B_CONT(Msg,NULL);

	// Split the MultiBlk message into many single block messages
	for (cnt = 1; cnt <= Ptr->BlockCount; cnt++) {

	     // Create STREAM_SEND message
   	     SendMsg = mercd_streams_allocator(sizeof(MDRV_MSG)+sizeof(STREAM_SEND), 
			     			 MERCD_STREAMS_BUFFER, MERCD_FORCE);

   	     if (SendMsg== NULL) {
		 printk("MsdDownStreamMultiBlk: alloc buffer failed.\n");
		 MSD_ASSERT(0);
   	     }

   	     // now mimic a STREM_SEND using the newly allocated message
   	     SendMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(SendMsg);

	     // Copy the original MDRV_MSG to the new message
	     MsdCopyMemory((PCHAR)MdMsg, (PCHAR)SendMdMsg, sizeof(MDRV_MSG));

   	     SendPtr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(SendMdMsg);

	     EntryPtr = (PSTREAM_MBLK_SEND_ENTRY)(&(Ptr->Entry[cnt-1]));
	     SendPtr->StreamId = Ptr->StreamId;
		
	     // Copy User Header
	     if (StreamBlock->type != STREAM_OPEN_F_GSTREAM)     
		 MsdCopyMemory((PCHAR)(&(EntryPtr->StreamUserHeader)), 
			(PCHAR)(&(SendPtr->StreamUserHeader)), sizeof(USER_HEADER));
		
	     // Adjust the read/write pointers in the new data buffer
	     // link new STREAM_SEND message and data buffer
	     // dupb the DataBlock, for last block use the original header
	     if (cnt == Ptr->BlockCount) {
		 MSD_ASSERT(MD_GET_MSG_SIZE(DataMsg) == EntryPtr->ByteCount);
		 MSD_LINK_MESSAGE(SendMsg, DataMsg);

		 // PTR 24666 Allow sending chopped of packets from WriteMulti
		 if (EntryPtr->Flags & MULTI_BLK_SEND_CAN_TAKE)
		     SendPtr->Flags =  MULTI_BLK_SEND_CAN_TAKE;
		 else
		     SendPtr->Flags = MULTI_BLK_NO_CAN_TAKE;
			
		 // Put the block count
		 SendPtr->Flags |= (cnt & MULTI_BLK_CNT_MASK);
	     } else {
		 NewDataMsg = dupb(DataMsg);
		 
		 if (NewDataMsg == NULL) {
		     ErrorCode = PAM_ERR_NO_MEMORY;
		     goto out;
		 }

		 // Read pointer of the DataMsg, calculate Write pointer
		 MD_SET_MSG_READ_PTR(NewDataMsg, MD_GET_MSG_READ_PTR(DataMsg));
		 MD_SET_MSG_WRITE_PTR(NewDataMsg, 
			(MD_GET_MSG_READ_PTR(NewDataMsg) + EntryPtr->ByteCount));

		 // Change the Read Pointer of the original message to point to
		 // begining of the new data area.
		 MD_SET_MSG_READ_PTR(DataMsg, 
			(MD_GET_MSG_READ_PTR(DataMsg) + EntryPtr->ByteCount));
		 MSD_LINK_MESSAGE(SendMsg, NewDataMsg);

		 SendPtr->Flags = MULTI_BLK_NO_CAN_TAKE;
	     }
	
	   MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
   	   mid_strmmgr_send_low(padapter, StreamBlock, MsdOpenBlock,  SendMsg);
	   MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex); 	   
	}

	// Free the original message
	mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, 1);

	return;

out:

	// send back an error message
	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg), MERCD_STREAMS_BUFFER,MERCD_FORCE);
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// for async messages, send back a async reply
	if (MD_IS_MSG_ASYNC(Msg)) {
	    // msg is freed in the send function 
	    supp_process_receive(MsdOpenBlock, Msg);
	} else { 
	    // sync msg uses the sync return path 
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	return;
}

/***************************************************************************
 * Function Name		: mid_strmmgr_strm_open
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_strm_open(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
        PMDRV_MSG               MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        PSTREAM_OPEN                    Ptr;
#ifdef LFS
        pmercd_adapter_block_sT         padapter = NULL;
#else
        pmercd_adapter_block_sT         padapter;
#endif

#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                      BindHandle = 0;
#else
        MBD_HANDLE      		BindHandle = 0;
#endif

#ifndef LFS
        MSD_HANDLE                      StreamHandle;
#endif
        pmercd_bind_block_sT            BindBlock;
        pmercd_stream_connection_sT     StreamBlock;
        PHIF_OPEN_STREAM        	OpenStreamMsg;
        PMERC_HOSTIF_MSG        	MercMsg;
        PMDRV_MSG               	NewMdMsg;
        PSTRM_MSG               	NewMsg;
        PSTRM_MSG               	DataMsg;
        merc_uint_t             	ErrorCode = MD_OK;
        merc_uint_t             	ReturnValue = 0;
        merc_uint_t             	Result;
        merc_uint_t             	AdapterNumber;
        merc_uchar_t            	Flag;
	int unlock=0;


        MSD_FUNCTION_TRACE("mid_strmmgr_strm_open", TWO_PARAMETERS, (merc_ulong_t)MsdOpenBlock,
                                (merc_ulong_t)Msg);

        // Get pointer to payload
        Ptr = (PSTREAM_OPEN)MD_GET_MDMSG_PAYLOAD(MdMsg);

        BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

        MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);
        BindBlock =  search_bindblock(MsdOpenBlock, BindHandle);
        MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

	if (BindBlock == NULL) {
	    printk("StreamOpen: invalid BindHandle %d\n", BindHandle);
	    ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
	    ReturnValue = Ptr->BoardNumber;
	    goto out2;
	}

        if (Ptr->BoardNumber >= MSD_MAX_BOARD_ID_COUNT) {
            printk("StreamOpen: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = Ptr->BoardNumber;
            goto out2;
        }

        AdapterNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

        if (AdapterNumber == 0xFF) {
            printk("StreamOpen: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = Ptr->BoardNumber;
            goto out2;
        }

        vopen = 0xfff3;
        padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

        if (padapter == NULL) {
            printk("StreamOpen: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = Ptr->BoardNumber;
            goto out2;
        }

        vopen = 0xfff4;
        /* -- LOCK -- */

	MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex); 
		
	/* Check for STARTED instead of READY dm3stderr   */
        if ((padapter->state != MERCD_ADAPTER_STATE_STARTED) &&
            (padapter->state != MERCD_ADAPTER_STATE_CP_STARTED) &&
            (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED)) {
             printk("StreamOpen: Adapter State not ready.\n");
             ErrorCode = CD_ERR_BAD_BOARD_STATE;
             goto out;
        }

        // If the StreamId is greater than MSD_MAX_STREAM_ID send err back
        if (Ptr->StreamId >= MSD_MAX_STREAM_ID) {
            printk("StreamOpen: Bad StreamId.\n");
            ErrorCode = PAM_ERR_BAD_PARAMETER;
            ReturnValue = Ptr->StreamId;
            goto out;
        }

        // If stream is already is opened send err to user
        // StramBlock should be NULL
        if (padapter->pstream_connection_list[Ptr->StreamId] != NULL) {
            printk("StreamOpen: Bad StreamId %d .\n", Ptr->StreamId);
            ErrorCode = PAM_ERR_BAD_PARAMETER;
            ReturnValue = Ptr->StreamId;
            goto out;
        }

        if (!(Ptr->StreamFlags & (STREAM_OPEN_F_SEND_ONLY|STREAM_OPEN_F_RECEIVE_ONLY))) {
            ErrorCode = PAM_ERR_BAD_PARAMETER;
            ReturnValue = padapter->adapternumber;
            goto out;
        }

        // get a new messgae block
        NewMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
        if (NewMsg == NULL) {
            printk("StreamOpen:Msg allocbuf failed\n");
            ErrorCode = PAM_ERR_NO_MEMORY;
            ReturnValue = padapter->adapternumber;
            goto out;
        }
	
        DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+ sizeof(HIF_OPEN_STREAM),GFP_ATOMIC);
	if (DataMsg == NULL) {
            printk("StreamOpen: Cannot alloc Data buffer\n");
            ErrorCode = PAM_ERR_NO_MEMORY;
            ReturnValue = padapter->adapternumber;
            goto out;
        }

        MD_SET_MSG_TYPE(DataMsg, M_DATA);
        MSD_LINK_MESSAGE(NewMsg,DataMsg);

        // allocate a stream block , zeroing is necessary on Linux
        mercd_zalloc( StreamBlock,
                      pmercd_stream_connection_sT,
                      sizeof(mercd_stream_connection_sT));

        if (StreamBlock == NULL) {
            printk("StreamOpen: Stream blk alloc failed.\n");
            ErrorCode = CD_ERR_NO_MEMORY;
            goto out;
        }

        //////////////////////////////////////////////////////////////////
        //WW Streaming Change:
        //The following should have been on the OPEN_ACK part of the code
        //in fwmgr.c. But linux2.4 kernel panics when you alloc kernel
        //memory recursively in the bh context. So for now this resides here.
        //The code in fwmgr.c is commented out.

        ///////////////////////////////////////////////////////////////////
        // set mutual pointers between bind and stream blocks
        StreamBlock->pbind_block = BindBlock;
 
        StreamBlock->handle = strm_fnd_free_connect_entry(padapter);

#ifdef DRVR_STATISTICS                                                           
        MsdControlBlock->open_streams++;                                         
        padapter->streams_count++;                                               
#endif   

	if(Ptr->StreamFlags & STREAM_OPEN_F_RECEIVE_ONLY) {
                        Ptr->ContainerSize = 0;
                        Ptr->multiplier = 0;
                        Ptr->suppress_fw_cantakes = 0;
                }

        if (Ptr->ContainerSize > 0) {
            StreamBlock->ContainerSize =  (Ptr->ContainerSize+24);
            StreamBlock->cantakeMode = 1;
        } else {
            StreamBlock->cantakeMode = 0;
            StreamBlock->ContainerSize = SRAM_BLOCK_MAX_DATA_SIZE;
            Ptr->ContainerSize = 4032;
        }

        StreamBlock->multiplication_factor = 1; //Ptr->multiplier;


                if(Ptr->suppress_fw_cantakes)
                        StreamBlock->suppress_fw_cantakes = Ptr->suppress_fw_cantakes;
                else   
                        StreamBlock->suppress_fw_cantakes = 0;


                StreamBlock->firstthreecnt = 0;StreamBlock->multiplication_factor = 1; //Ptr->multiplier;


                if(Ptr->suppress_fw_cantakes)
                        StreamBlock->suppress_fw_cantakes = Ptr->suppress_fw_cantakes;
                else   
                        StreamBlock->suppress_fw_cantakes = 0;


                StreamBlock->firstthreecnt = 0;
    

        if(padapter->pstream_connection_list[StreamBlock->handle] != NULL)
           printk(" ID is already in use \n");

        // link the stream block on the padapter's streamList
        queue_put_Q(&padapter->stream_adapter_list, (PMSD_GEN_MSG)StreamBlock);

        // set the stream state to OPEN_PENDING
        StreamBlock->state = MERCD_STREAM_STATE_OPEN_PEND;
        // Flags added for later usage (Preserve the mode with which
        // the stream is being open i.e. read/write
        StreamBlock->flags = (Ptr->StreamFlags & ~(STREAM_OPEN_F_GSTREAM));
                                /* set only open mode. */
        StreamBlock->type = (Ptr->StreamFlags  & STREAM_OPEN_F_GSTREAM);
                                /* Set only B or GStreams */
        Ptr->StreamFlags = (Ptr->StreamFlags & ~(STREAM_OPEN_F_GSTREAM));
                                /* leave only open mode. */

	// save the current trans id for later matching needs

        // get pointer to adapter
        StreamBlock->padapter_block = padapter;

        MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
        MERCURY_SET_BODY_SIZE(MercMsg,sizeof(HIF_OPEN_STREAM));

        // create a host interface driver class message
        NewMdMsg=MD_EXTRACT_MDMSG_FROM_STRMMSG(NewMsg);

        Flag = MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER;
        MERCURY_SET_MESSAGE_FLAG(MercMsg, Flag);
        MERCURY_SET_MESSAGE_CLASS(MercMsg, MERCURY_CLASS_DRIVER);
        MERCURY_SET_MESSAGE_TYPE(MercMsg, QCNTRL_OPEN_STREAM);


	while(strm_fnd_entry_from_adapter(padapter,padapter->TransactionId))
          padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

        StreamBlock->TransactionId = padapter->TransactionId;

        MERCURY_SET_TRANSACTION_ID(MercMsg,padapter->TransactionId);
        padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

	// Transation id may overflow in next 1-2 quarters. 
	// Better solution will be provided. If the transation 
	// id is  wrapped around, and that id is already in open, 
	// we have a possible problem.
        if(padapter->TransactionId == 0xFFFFFFFF)
            printk("Transaction ID overflow   \n");

        MERCURY_SET_DEST_NODE(MercMsg, DEFAULT_DEST_NODE);
        MERCURY_SET_DEST_PROC(MercMsg, DEFAULT_DEST_PROC);
        MERCURY_SET_DEST_COMPINST(MercMsg, DEFAULT_DEST_COMPINST);
        MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);

        Ptr = (PSTREAM_OPEN)MD_GET_MDMSG_PAYLOAD(MdMsg);
        OpenStreamMsg = (PHIF_OPEN_STREAM)MERCURY_BODY_START(MercMsg);
        OpenStreamMsg->StreamId = Ptr->StreamId;
        OpenStreamMsg->RequestSize = Ptr->RequestSize;
        OpenStreamMsg->CanTakeLimit = MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER);

        // convert the stream open flags to FW interface spec
        if(Ptr->StreamFlags & STREAM_OPEN_F_SEND_ONLY)
                OpenStreamMsg->Mode = OPEN_STR_WRITE_ONLY;
        else if(Ptr->StreamFlags & STREAM_OPEN_F_RECEIVE_ONLY)
                OpenStreamMsg->Mode = OPEN_STR_READ_ONLY|OPEN_STR_FLUSH;

        // persistent streams (cached prompts)
        if(Ptr->StreamFlags & STREAM_OPEN_F_FLUSH)
                OpenStreamMsg->Mode |= OPEN_STR_FLUSH;
        if(Ptr->StreamFlags & STREAM_OPEN_F_PERSISTENT)
                OpenStreamMsg->Mode |= OPEN_STR_PERSISTENT;

        //  may want to save open stream msg */
       	Result = MD_FAILURE; 
	// HCS Support
	if (padapter->isHCSSupported && (Ptr->StreamId != 1) && 
	        (Ptr->RequestSize < padapter->HCS.sizeOfHCS)) {
	    // check for available HCS else use old method
	    if (padapter->HCS.numOfHCSActive < padapter->HCS.maxNumOfHCS) {
	        MERCURY_SET_MESSAGE_TYPE(MercMsg, QCNTRL_SET_STREAM_PROPERTY);
		Result = mid_strmmgr_strm_open_hcs(padapter, StreamBlock, NewMsg);
	    } else {
		Result = MD_FAILURE;
	    }
	}
	    
	// if HCS is not supported or some reason failed then use original method
	if (Result != MD_SUCCESS) {
            MERCURY_SET_MESSAGE_TYPE(MercMsg, QCNTRL_OPEN_STREAM);
            if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                Result = strm_Q_snd_msg(NewMsg, padapter);
            } else {
                if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
                    Result = strm_ww_Q_snd_msg(NewMsg, padapter);
                } else {
		    printk("mid_strmmgr_strm_open: invalid adapter state\n");
		    Result = MD_FAILURE;
                }
	    }
        }

        if (Result != MD_SUCCESS) {
            printk("StreamOpen: strm_Q_snd_msg/strm_ww_Q_snd_msg failed.\n");
            BindBlock->stream_connection_ptr = NULL;

            if (queue_remove_from_Q(&padapter->stream_adapter_list,(PMSD_GEN_MSG)StreamBlock)
                   				   != MD_SUCCESS) {

                printk("StreamOpen: RemoveFromQ failed for StreamBlock."
                                                "StreamId=%d\n", StreamBlock->id);
                MSD_ASSERT(0);
            }

            mercd_free((pmerc_void_t)StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE);
            // Added to free the new message ??????
            mercd_streams_free(NewMsg, MERCD_STREAMS_BUFFER , MERCD_FORCE);
            ErrorCode = Result;
            ReturnValue = padapter->adapternumber;
            printk("StreamOpen: strm_Q_snd_msg/strm_ww_Q_snd_msg failed. Error=%d\n", ErrorCode);
            goto out;
        }

        vopen = 0xfff5;
        // free old message
        mercd_streams_free(Msg, MERCD_STREAMS_BUFFER , MERCD_FORCE);

        vopen = 0xfff6;
        // Must record the open pending intermediate state in
        // bind block so an internal close will cause a stream
        // close to be processed.

        BindBlock->flags |= MSD_BIND_FLAG_STREAM_OPEN_PEND;

        vopen = 0xfff7;
	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

        return;

out:
	unlock=1;
out2:
        // Only Errrors are handled here
        Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

        if (MD_GET_MSG_B_CONT(Msg)) {
            mercd_streams_free(MD_GET_MSG_B_CONT(Msg), MERCD_STREAMS_BUFFER , MERCD_FORCE);
            MD_SET_MSG_B_CONT(Msg, NULL);
        }

        // for async messages, send back a async reply
        if (MD_IS_MSG_ASYNC(Msg)) {
            // msg is freed in the send function
            supp_process_receive(MsdOpenBlock, Msg);
	} else {
	    // sync msg uses the sync return path
            supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	if (unlock)    
  	   MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
        return;

}

/***************************************************************************
 * Function Name		: mid_strmmgr_strm_close
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_strm_close(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
    	PSTREAM_CLOSE                   Ptr;
#ifdef LFS
    	pmercd_adapter_block_sT         padapter = NULL;
#else
    	pmercd_adapter_block_sT         padapter;
#endif
    	pmercd_stream_connection_sT     StreamBlock;
    	PSTRM_MSG                       SendMsg;
    	PMDRV_MSG                       SendMdMsg;
    	PSTREAM_SEND                    SendPtr;
    	PSTRM_MSG                       DataMsg;
    	PHIF_CLOSE_STREAM               CloseStreamMsg;
    	PMERC_HOSTIF_MSG                MercMsg;
    	PMDRV_MSG   MdMsg =             MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
    	merc_uint_t ErrorCode =         MD_OK;
    	merc_uint_t ReturnValue =       0;
    	merc_uint_t                     Result;
    	merc_uint_t                     StreamId=0;
    	merc_uchar_t                    Flag;
    	MSD_HANDLE                      StreamHandle;
#ifndef LFS
    	mercd_ww_flush_streamMF         flushMf;
    	merc_ushort_t                   flushIndex;
    	MD_STATUS                       Status;
#endif
    	int unlock = 0;

        MSD_DRIVER_STATE_CHECK(MERCD_CTRL_BLOCK_READY);

        // Get pointer to payload
        Ptr = (PSTREAM_CLOSE)MD_GET_MDMSG_PAYLOAD(MdMsg);
	StreamId = Ptr->StreamId;

	// perform validity checking
        StreamHandle = MD_GET_MDMSG_STREAM_HANDLE(MdMsg);

        if(StreamHandle >= MsdControlBlock->maxstreams)  {
                printk("mid_strmmgr_strm_close: bad stream handle.\n");
                ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
                goto out2;
        }

        MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

	StreamBlock = search_streamblock(MsdOpenBlock, StreamHandle);
        
        // Validity Checking
        if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
            printk("mid_strmmgr_strm_close: NULL StreamBlock for id %d & sh %d\n", StreamId, StreamHandle);
            ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
            goto out2;
        }

        padapter = StreamBlock->padapter_block;

	if (padapter == NULL) {
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
            printk("mid_strmmgr_strm_close: bad board number.\n");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            goto out2;
        }

        MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

        if ((!(StreamBlock->state == MERCD_STREAM_STATE_CONNECTED)) &&
            (!(StreamBlock->state == MERCD_STREAM_STATE_BROKEN))) {

	    // 2045 - send cancel on DMVB
	    if (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) {
 	        MSD_LEVEL2_DBGPRINT("mid_strmmgr_strm_close: cancel active stream\n");
		StreamBlock->SendCancelStreamMf = 2;
		StreamBlock->cancelFlags = Ptr->Flags;
                msgutl_ww_build_and_send_cancel_stream_mf(StreamBlock->pbind_block);
                mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
		return;
	    } 
            printk("mid_strmmgr_strm_close: non-broken state=%d, Sid=%d\n",
            StreamBlock->state,StreamBlock->id);
            ErrorCode = CD_ERR_BAD_STREAM_STATE;
            goto out2;
        }

	MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

        if ((padapter->state != MERCD_ADAPTER_STATE_READY) &&
            (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED)) {
            printk("mid_strmmgr_strm_close: Adapter State not ready. %d\n",padapter->state);
            ErrorCode = CD_ERR_BAD_BOARD_STATE;
            goto out;
        }

        // Should this be on StreamFlags ??????
        if ((Ptr->Flags & STREAM_CLOSE_FLAG_FLUSH_ALL) || (StreamBlock->isStreamHCS)) {
	    // FOR HCS - allow write stream to be close the same way as read...

            // This is a READ stream so send the CLOSE message
            // if the stream close message flag is set to FLUSH_ALL
            // then pass the stream close message to F/W, else
            // we need to craft a NULL data block with EOS flag
            // and send to firmware.  Firmware will interpret
            // the null block with EOS as a stream close command
            // StreamBlock->StreamState |= MERCD_STREAM_STATE_CLOSE_PEND;

            StreamBlock->state  = MERCD_STREAM_STATE_CLOSE_PEND;
            
	    // allocate a new mesage for our use
            SendMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
            if (SendMsg == NULL) {
                printk("mid_strmmgr_strm_close: allocbuf failed\n");
                ErrorCode = PAM_ERR_NO_MEMORY;
                ReturnValue = padapter->adapternumber;
                goto out;
            }

            DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+sizeof(HIF_CLOSE_STREAM),GFP_ATOMIC);
            if (DataMsg == NULL) {
                printk("mid_strmmgr_strm_close:: Cannot alloc Data buffer\n");
                ErrorCode = PAM_ERR_NO_MEMORY;
                ReturnValue = padapter->adapternumber;
                mercd_streams_free(SendMsg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                goto out;
            }
	    
            MD_SET_MSG_TYPE(DataMsg, M_DATA);
            MSD_LINK_MESSAGE(SendMsg,DataMsg);

            MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
            MERCURY_SET_BODY_SIZE(MercMsg,sizeof(HIF_CLOSE_STREAM));

            // create a host interface driver class message
            SendMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(SendMsg);

            StreamId = StreamBlock->id;

            Flag = MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER;
            MERCURY_SET_MESSAGE_FLAG(MercMsg,Flag);
            MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);

            // CLOSE STREAM is CLOSE_STREAM_ABORT
            MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_CLOSE_STREAM);

	    while(strm_fnd_entry_from_adapter(padapter,padapter->TransactionId))
                  padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

            MERCURY_SET_TRANSACTION_ID(MercMsg,padapter->TransactionId);
            padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

	    // Transation id may overflow in next 1-2 quarters. 
	    // Better solution will be provided. If the transation id is 
	    // wrapped around, and that id is already in open, 
	    // we have a possible problem.
            if (padapter->TransactionId == 0xFFFFFFFF)
                cmn_err(CE_WARN, "Transaction ID overflow   \n");

            MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
            MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
            MERCURY_SET_DEST_COMPINST(MercMsg,DEFAULT_DEST_COMPINST);
            MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);
            CloseStreamMsg = (PHIF_CLOSE_STREAM)MERCURY_BODY_START(MercMsg);
            CloseStreamMsg->StreamId = StreamId;

 	    // HCS Support
            if (StreamBlock->isStreamHCS) {
                // close HCS
                MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_RESET_STREAM_PROPERTY);
                Result = mid_strmmgr_strm_close_hcs(padapter, StreamBlock, SendMsg);
            } else if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
	        // WW Flags for WW enabled 1.9 boards and Third Rock boards. 
	        // e.g it does not impact existing customers.(2001)
                Result = strm_Q_snd_msg(SendMsg, padapter);
            } else {
                if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {

#if 0
                    //WW specific freeing
                    mid_wwmgr_close_streams(padapter, StreamBlock);
                    //First we send the FLUSH MF Followd by the CLose Msg
                    flushMf.streamid = StreamBlock->id;
                    flushMf.padapter = (pmerc_void_t)padapter;
                    Status = msgutl_ww_build_and_send_streams_flush_mf(&flushMf);

                    if (Status == MD_SUCCESS) {
                        StreamBlock->pWWFlushCloseMsg = NULL;
                        vopen1 = 0x1;
                        Result = strm_ww_Q_snd_msg(SendMsg, padapter);
                        Result = MD_SUCCESS;
                        vopen1 = 0x2;
                    } else {
                        StreamBlock->pWWFlushCloseMsg = SendMsg;
                        Result = MD_SUCCESS;
                    }
#endif

#if 1
                    //WW specific freeing
                    StreamBlock->pWWFlushCloseMsg = NULL;

                    //mid_wwmgr_close_streams(padapter, StreamBlock);
                    Result = strm_ww_Q_snd_msg(SendMsg, padapter);
                    Result = MD_SUCCESS;
#endif 
                } /* MERCD_ADAPTER_WW_SUCCEDED */
                  else {
                    cmn_err(CE_WARN, "Adapter out of srvc\n");
                    padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
                    goto out;
                }
            }

            if (Result != MD_SUCCESS) {
                ErrorCode = Result;
                if (vopen1 == 0x2)
                    vopen = 0x12123;
                ReturnValue = padapter->adapternumber;
                printk("StreamClose:strm_Q_snd_msg/strm_ww_Q_snd_msg failed.Err=%d\n", ErrorCode);
                mercd_streams_free(SendMsg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                goto out;
            }

            // free old message
            mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);

	    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return;

        } else {
            // This is a write stream hand craft a null block with EOS flag

            // Assert if not Write
            MSD_ASSERT(StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY);

            // Before putting this EOS block lets remove all the pending blocks
            // from the SendQ for a BROKEN stream

            if (StreamBlock->state == MERCD_STREAM_STATE_BROKEN) {
                strm_free_only_data_msgs(StreamBlock);
            }

            // Allocate a MDRV_MSG
            SendMsg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_SEND),GFP_ATOMIC);
            if (SendMsg == NULL) {
                printk("mid_strmmgr_strm_close: cannot allocate msg.\n");
                ErrorCode = CD_ERR_NO_MEMORY;
                goto out;
            }

            SendMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(SendMsg);

            // setup the MID_STREAM_SEND message
            MD_SET_MDMSG_CLASS(SendMdMsg, MD_CLASS_CORE);
            MD_SET_MDMSG_ID(SendMdMsg, MID_STREAM_SEND);
            MD_SET_MDMSG_STREAM_HANDLE(SendMdMsg, StreamHandle);
            SendMdMsg->MessageFlags |= MD_MSG_FLAG_ASYNC;

            SendPtr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(SendMdMsg);
            SendPtr->StreamId = StreamBlock->id;
            SendPtr->Flags = STREAM_FLAG_EOS;

            // must allocate a dummy data block
            DataMsg = supp_alloc_buf(16,GFP_ATOMIC);
            if (DataMsg == NULL) {
                printk("mid_strmmgr_strm_close: cannot allocate data msg.\n");
                ErrorCode = CD_ERR_NO_MEMORY;
                goto out;
            }

            MD_SET_MSG_TYPE(DataMsg,M_DATA);
            MD_SET_MSG_WRITE_PTR(DataMsg,
            MD_GET_MSG_READ_PTR(DataMsg));

            // link the empty data block to the MDRV_MSG
            MSD_LINK_MESSAGE(SendMsg,DataMsg);

            // cannot change state befpore data is sent
            // StreamBlock->StreamState |= MERCD_STREAM_STATE_CLOSE_PEND;
            StreamBlock->state = MERCD_STREAM_STATE_CLOSE_PEND;

#if 0 
            if ((((StreamBlock->qparam.cur_snd_cnt + MsdMessageDataSize(SendMsg) ) >
                   StreamBlock->qparam.snd_queue_limit)) && (MsdMessageDataSize(SendMsg) != 0) ) {

                printk("MsdDownStreamDataStrategy: Send q overflow.discarding send data\n");
                printk("MsdDownStreamDataStrategy: SendCount %x, MSgSize %x,"
                                 "Limit %x\n",StreamBlock->qparam.cur_snd_cnt,
                                 MsdMessageDataSize(SendMsg), StreamBlock->qparam.snd_queue_limit);

                if (Ptr->Flags & STREAM_FLAG_EOS) {
                    printk("MsdDownStreamDataStrategy: EOS on Nonzero block\n");
                } else {
                    ErrorCode = CD_ERR_SEND_Q_OVERFLOW;
                    DataMsg = MD_GET_MSG_B_CONT(SendMsg);
                    MSD_FREE_BUFFER(DataMsg);
                    MD_SET_MSG_B_CONT(SendMsg, NULL);
                    goto out;
                }
            }
#endif

            // link a null block to the control msg if no data are attached
            // this is done because next code expects a data block for all stream
            // send messages

            if (MD_GET_MSG_B_CONT(SendMsg)==NULL) {
                DataMsg = mercd_streams_allocator(16, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                if (DataMsg == NULL) {
                    printk("mid_strmmgr_strm_close: cannot allocate data msg.\n");
                    ErrorCode = CD_ERR_NO_MEMORY;
                    goto out;
                }
                MD_SET_MSG_TYPE(DataMsg,M_DATA);
                MD_SET_MSG_WRITE_PTR(DataMsg,
                MD_GET_MSG_READ_PTR(DataMsg));
                MSD_LINK_MESSAGE(SendMsg,DataMsg);
            }

            // link the message to this stream's send queue

            Ptr->StreamId = StreamBlock->id;

            if (Ptr->Flags & STREAM_FLAG_EOS) {
                if (Ptr->Flags & (MULTI_BLK_NO_CAN_TAKE|MULTI_BLK_SEND_CAN_TAKE)) {
                    MSD_ASSERT(0);
                }
            }

            ////////////////////////////////////////////////////////////////////
            // WW Streaming Change:
            ////////////////////////////////////////////////////////////////////
#if 1
            strm_put_data_strm_Q(StreamBlock, SendMsg);
            StreamBlock->qparam.cur_snd_cnt +=  MsdMessageDataSize(SendMsg);

            if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
                padapter->flags.SendDataPending |= MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND;
                StreamBlock->qparam.cur_snd_cnt += MsdMessageDataSize(SendMsg);
            } else {
                // update the send count
                StreamBlock->qparam.cur_snd_cnt +=  MsdMessageDataSize(SendMsg);
                strm_adv_gather_snd_blks(padapter, StreamBlock);
                padapter->flags.SendDataPending |= MERC_ADAPTER_FLAG_SEND_DATA_PEND;
            }
#endif
            // free old message
            mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);

	    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return;
        }

out:
	unlock=1;
out2:
        // Only Errrors are handled here
        Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

        if (MD_GET_MSG_B_CONT(Msg)) {
            mercd_streams_free(MD_GET_MSG_B_CONT(Msg),MERCD_STREAMS_BUFFER, MERCD_FORCE);
             MD_SET_MSG_B_CONT(Msg, NULL);
        }

        // for async messages, send back a async reply
        if (MD_IS_MSG_ASYNC(Msg)) {
            // msg is freed in the send function
            supp_process_receive(MsdOpenBlock, Msg);
	} else {
            // sync msg uses the sync return path
            supp_process_sync_receive(MsdOpenBlock, Msg);
	}

	if(unlock)
  	   MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
        return;
}

/***************************************************************************
 * Function Name		: mid_strmmgr_strm_flush
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_strm_flush(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{

    PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
    merc_uint_t ErrorCode = MD_OK;
    merc_uint_t ReturnValue = 0;

    MSD_FUNCTION_TRACE("mid_strmmgr_strm_flush", TWO_PARAMETERS,
                      (size_t)MsdOpenBlock, (size_t)Msg);

	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
 	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg), MERCD_STREAMS_BUFFER, MERCD_FORCE );
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// for async messages, send back a async reply 
	if (MD_IS_MSG_ASYNC(Msg)) {
	    // msg is freed in the send function
	    supp_process_receive(MsdOpenBlock, Msg);
	} else { 
	    // sync msg uses the sync return path 
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}

	return;
}

/***************************************************************************
 * Function Name		: mid_strmmgr_strm_close_internal
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_strm_close_internal(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
    PSTREAM_CLOSE_INTERNAL Ptr;
    pmercd_adapter_block_sT padapter;
    pmercd_stream_connection_sT StreamBlock;
    PHIF_CLOSE_STREAM   CloseStreamMsg;
    PMERC_HOSTIF_MSG    MercMsg;
    PMDRV_MSG   NewMdMsg;
    PSTRM_MSG   NewMsg;
    PSTRM_MSG   DataMsg;
    PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
#ifdef LFS
    merc_uint_t Result = 0;
#else
    merc_uint_t Result;
#endif
    merc_uint_t StreamId;
    merc_uint_t hcs_write_flag;
    merc_uchar_t    Flag;
    merc_uint_t ErrorCode = MD_OK;
    merc_uint_t ReturnValue = 0;
    PSTRM_MSG   strmEosMsg;
    PSTREAM_SEND  streamSendPtr;

    MSD_FUNCTION_TRACE("mid_strmmgr_strm_close_internal", TWO_PARAMETERS,
                        (size_t) MsdOpenBlock, (size_t) Msg);

        Ptr = (PSTREAM_CLOSE_INTERNAL)MD_GET_MDMSG_PAYLOAD(MdMsg);

        // get the stream block from the message
        StreamBlock = Ptr->StreamBlock;

        MSD_ASSERT(StreamBlock);
	
#ifdef LFS
        // get pointer to adapter
        padapter = StreamBlock->padapter_block;
#endif

        // perform validity checking
        if ((StreamBlock->state==MERCD_STREAM_STATE_CLOSE_PEND) ||
	    (StreamBlock->state==MERCD_STREAM_STATE_TERMINATE_PEND)) {
            // If internal close then app died while waiting for Close ack
            StreamBlock->state = MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND;

	    // 2045 - send cancel on DMVB
            if (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) {
                MSD_LEVEL2_DBGPRINT("internal_close cancel active stream\n");
		StreamBlock->SendCancelStreamMf = 1;
                msgutl_ww_build_and_send_cancel_stream_mf(StreamBlock->pbind_block);
                mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                return;
            }
            mercd_streams_free(Msg,MERCD_STREAMS_BUFFER, MERCD_FORCE);
            return;
        }

        if (StreamBlock->state==MERCD_STREAM_STATE_NOT_OPENED) {
            printk("StreamManager: STREAM_CLOSE_INTERNAL in "
               "bad state =%d, StreamId=%d \n", StreamBlock->state,StreamBlock->id);
            mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
            return;
        }

        if ((StreamBlock->state != MERCD_STREAM_STATE_CONNECTED)  &&
            (StreamBlock->state != MERCD_STREAM_STATE_BROKEN)     &&
            (StreamBlock->state != MERCD_STREAM_STATE_CLOSE_PEND) &&
	    (StreamBlock->state != MERCD_STREAM_STATE_TERMINATE_PEND) &&
            (StreamBlock->state != MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND)) {
	    // 2045 - send cancel on DMVB
            if (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) {
                MSD_LEVEL2_DBGPRINT("internal_close cancel active stream\n");
		StreamBlock->SendCancelStreamMf = 1;
                msgutl_ww_build_and_send_cancel_stream_mf(StreamBlock->pbind_block);
                mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                return;
            }
            printk("mid_strmmgr_strm_close_internal:in bad state =%d, StreamId=%d",
                                 StreamBlock->state, StreamBlock->id);
            mercd_streams_free(Msg,MERCD_STREAMS_BUFFER, MERCD_FORCE);
            return;
        }
	
        // get pointer to adapter
        padapter = StreamBlock->padapter_block;

	/* Check for STARTED instead of READY dm3stderr   */
        if ((padapter->state != MERCD_ADAPTER_STATE_STARTED) &&
            (padapter->state != MERCD_ADAPTER_STATE_CP_STARTED) &&
            (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED)) {
             printk("StreamManager: StremClose:Board State not ready, but %x.\n",
			     						  padapter->state);
             mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
             return;
        }

	//In WW mode, we would have called close_internal from 
	//abnormal routine after acquiring intr_mutex. So no
	//need to acquire it here.
        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
	   MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	
        // must set proper stream handle
        MD_SET_MDMSG_STREAM_HANDLE(MdMsg,StreamBlock->handle);

	// check for broken stream on a write HCS
        hcs_write_flag = 0;
        if (StreamBlock->isStreamHCS && (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)) {
            if ((StreamBlock->state == MERCD_STREAM_STATE_BROKEN) ||
                (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND)) {
                MSD_LEVEL2_DBGPRINT("int_close write bypass on %d state %d\n", StreamBlock->id, StreamBlock->state);
                hcs_write_flag = 1;
            }
        }

        // set streamblock state to close pending
        StreamBlock->state = MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND;

        // free any messages on send queue
        if (StreamBlock->SendStreamQueue) {
            mercd_streams_free(StreamBlock->SendStreamQueue, MERCD_STREAMS_BUFFER, MERCD_FORCE);
            StreamBlock->SendStreamQueue=NULL;
        }

        // Also free messages from the AdapterList
        strm_free_srm_msgs(padapter, StreamBlock);

#if 0
        //Free the WW Specific Streaming
        mid_wwmgr_close_streams(padapter, StreamBlock);
#endif

        /* For a board running the WW protocol and if this is a write
         * stream then we should not be sending a QCNTRL_CLOSE_ABORT. In
         * this case we need to send an EOS MF down to the board */

        if (((padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY) &&
                (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)) && (!hcs_write_flag)) {
            /* This is a WW board and it is a write stream. In this case we
             * need to send an EOS MF to the board and not a CLOSE_ ABORT
             * */

            strmEosMsg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_SEND), GFP_ATOMIC);
            if(strmEosMsg== NULL) {
                MSD_ERR_DBGPRINT("Failed to allocate memory while sending EOS to board.\n");
                mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
                if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
                    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
                return;
            }

            // now mimic a STREM_CLOSE using the newly allocated message
            // set the message body
            MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(strmEosMsg);
            streamSendPtr = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
            streamSendPtr->StreamId = StreamBlock->id;

            // Invoke the function to send the eos mf now
	    if (!StreamBlock->isStreamHCS) {
                snd_ww_eos2adapter(padapter, strmEosMsg,0);
	    } else {
	        // send a cancel stream for HSC streams on WW
                MSD_LEVEL2_DBGPRINT("HCS: sending a cancel stream on write..\n");
		StreamBlock->SendCancelStreamMf = 1;
                msgutl_ww_build_and_send_cancel_stream_mf(StreamBlock->pbind_block);
            }
        } else {	
            // allocate a new mesage for our use
            NewMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
            if (NewMsg == NULL) {
                ErrorCode = PAM_ERR_NO_MEMORY;
                ReturnValue = padapter->adapternumber;
	        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
	            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
                return;
            }

            DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+ sizeof(HIF_CLOSE_STREAM),GFP_ATOMIC);
            if (DataMsg == NULL) {
                printk("CLOSE_INTERNAL:Cannot alloc Data buffer\n");
                ErrorCode = PAM_ERR_NO_MEMORY;
                ReturnValue = padapter->adapternumber;
                mercd_streams_free(NewMsg,MERCD_STREAMS_BUFFER, MERCD_FORCE);
                mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
	        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
	           MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
                return;
            }
	
            MD_SET_MSG_TYPE(DataMsg, M_DATA);
            MSD_LINK_MESSAGE(NewMsg,DataMsg);

            MercMsg = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
            MERCURY_SET_BODY_SIZE(MercMsg,sizeof(HIF_CLOSE_STREAM));

            // create a host interface driver class message
            NewMdMsg=MD_EXTRACT_MDMSG_FROM_STRMMSG(NewMsg);

            StreamId = StreamBlock->id;

            Flag = MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER;
            MERCURY_SET_MESSAGE_FLAG(MercMsg,Flag);
            MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);

            // CLOSE STREAM is CLOSE_STREAM_ABORT
            MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_CLOSE_STREAM);

	    while(strm_fnd_entry_from_adapter(padapter,padapter->TransactionId))
              padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

            MERCURY_SET_TRANSACTION_ID(MercMsg,padapter->TransactionId);
            padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF); 

	    // Transation id may overflow in next 1-2 quarters. 
	    // Better solution will be provided. If the transation id is 
	    // wrapped around, and that id is already in open, we have a possible problem.
            if (padapter->TransactionId == 0xFFFFFFFF)
                printk("Transaction ID  overflow  \n");

            MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
            MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
            MERCURY_SET_DEST_COMPINST(MercMsg,DEFAULT_DEST_COMPINST);
            MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);
            CloseStreamMsg = (PHIF_CLOSE_STREAM)MERCURY_BODY_START(MercMsg);
            CloseStreamMsg->StreamId = StreamId;

            /**********************************************************/

	    // HCS Support
            if (StreamBlock->isStreamHCS) {
                // close HCS
                MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_RESET_STREAM_PROPERTY);
                Result = mid_strmmgr_strm_close_hcs(padapter, StreamBlock, NewMsg);
            } else if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
               Result = strm_Q_snd_msg(NewMsg, padapter);
            } else {
               if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
                   Result = strm_ww_Q_snd_msg(NewMsg, padapter);
               } else {
                   cmn_err(CE_WARN, "Adapter out of srvc\n");
                   padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
               }
            }

            if (Result != MD_SUCCESS) {
                ErrorCode = Result;
                ReturnValue = padapter->adapternumber;
                printk("INT_CLOSE: strm_Q_snd_msg/strm_ww_Q_snd_msg failed. Error=%d\n",
                        ErrorCode);
                mercd_streams_free(Msg,MERCD_STREAMS_BUFFER, MERCD_FORCE);
                mercd_streams_free(NewMsg,MERCD_STREAMS_BUFFER, MERCD_FORCE);
	        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
	            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
                return;
            }
	}

        mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
	   MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
   
	return;

}

/***************************************************************************
 * Function Name                : mid_strmmgr_get_param
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_strmmgr_get_param(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg)
{

	PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        PSTREAM_GET_PARAMETER 		Ptr;
        PSTREAM_GET_PARAMETER_ACK 	AckPtr;
        PSTREAM_PARAMETER_ENTRY 	EntryPtr;
        pmercd_stream_connection_sT  	StreamBlock;
        merc_int_t  			i;
        merc_uint_t 			ErrorCode = MD_OK;
        merc_uint_t 			ReturnValue = 0;
        MSD_HANDLE      		StreamHandle;

        MSD_FUNCTION_TRACE("mid_strmmgr_get_param", TWO_PARAMETERS,
                (size_t)MsdOpenBlock, (size_t)Msg);

        // check the current driver state
        Ptr = (PSTREAM_GET_PARAMETER)MD_GET_MDMSG_PAYLOAD(MdMsg);
        AckPtr = (PSTREAM_GET_PARAMETER_ACK)Ptr;
        AckPtr->ErrorCode = MD_OK;

        // check the current state
        if (MsdControlBlock->MsdState != MERCD_CTRL_BLOCK_READY) {
            printk("mid_strmmgr_get_param: MsdState: 0x%x\n", 
                             MsdControlBlock->MsdState);
            ErrorCode = CD_ERR_BAD_STATE;
            ReturnValue = MsdControlBlock->MsdState;
	    goto out;
        }
	
	// fetch the stream handle from message
        StreamHandle = MD_GET_MDMSG_STREAM_HANDLE(MdMsg);
	
        if (StreamHandle >= MsdControlBlock->maxstreams || StreamHandle == 0 ) {
            printk("StreamProcGetParam: bad stream handle %d\n", StreamHandle);
            AckPtr->ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
            goto out;
        }

        // fetch the stream block using the stream handle
	MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);
	StreamBlock = search_streamblock(MsdOpenBlock, StreamHandle);
	MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

        // perform validity checking
        if (StreamBlock == NULL) {
            printk("StreamProcGetParam: Stream handle bad.\n");
            AckPtr->ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
            goto out;
        }

        if (Ptr->Count > MD_MAX_STREAM_PARAM_COUNT){
            printk("StreamProcGetParam: count: %d > max :%d\n", 
                                  Ptr->Count,  MD_MAX_STREAM_PARAM_COUNT);
            AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
            goto out;
        }

        // process the individual get parm entries.
        // User may request more than one entry
        EntryPtr = (PSTREAM_PARAMETER_ENTRY)(Ptr+1);

	for (i=0;i<Ptr->Count;i++) {
             switch (EntryPtr->Key) {
                case STREAM_KEY_SEND_Q_LIMIT: // get send queue limit
                        EntryPtr->ErrorCode = MD_OK;
                        EntryPtr->Value.UlongValue[0] =
                        (StreamBlock->qparam.snd_queue_limit / 4); // Divide by 4
                        break;
                case STREAM_KEY_SEND_Q_LOW:     // get send queue low water
                        EntryPtr->ErrorCode = MD_OK;
                        EntryPtr->Value.UlongValue[0] =
                            StreamBlock->qparam.snd_queue_low;
                        break;
		 case STREAM_KEY_CONTAINER_SIZE:
                        EntryPtr->ErrorCode = MD_OK;
                        EntryPtr->Value.UlongValue[0]=StreamBlock->ContainerSize;
                        EntryPtr->Value.UlongValue[1]=StreamBlock->multiplication_factor;

                        break;
                default:
                        EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
                        break;

             }
             EntryPtr++;
        }
out:
        // since all structs are identical, just change the msg
        // id and send the msg back
        MdMsg->MessageId = MID_STREAM_GET_PARAMETER_ACK;

        // return the message to user via sync path
        //supp_process_sync_receive(MsdOpenBlock, Msg);
        Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

        if (MD_GET_MSG_B_CONT(Msg)) {
            MSD_FREE_MESSAGE(MD_GET_MSG_B_CONT(Msg));
            MD_SET_MSG_B_CONT(Msg, NULL);
        }

        // sync msg uses the sync return path
        supp_process_sync_receive(MsdOpenBlock, Msg);

        return;

}


		

/***************************************************************************
 * Function Name		: mid_strmmgr_set_param
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_strmmgr_set_param(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg)
{

        PMDRV_MSG  MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        PSTREAM_SET_PARAMETER       Ptr;
        PSTREAM_SET_PARAMETER_ACK   AckPtr;
        PSTREAM_PARAMETER_ENTRY     EntryPtr;
        merc_uint_t                 StreamId = 0;
        pmercd_stream_connection_sT StreamBlock;
        pmercd_bind_block_sT        BindBlock;
        merc_int_t                  i;
        merc_uint_t                 ErrorCode = MD_OK;
        merc_uint_t                 ReturnValue = 0;
#ifndef LFS
        MD_STATUS  		    Status = MD_SUCCESS;
#endif
#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                  BindHandle = 0;
#else
        MBD_HANDLE                  BindHandle = 0;
#endif

	Ptr = (PSTREAM_SET_PARAMETER)MD_GET_MDMSG_PAYLOAD(MdMsg); 
	StreamId = Ptr->StreamId; 
	BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);
	AckPtr = (PSTREAM_SET_PARAMETER_ACK)Ptr;
	AckPtr->ErrorCode = MD_OK;

	if (MsdControlBlock->MsdState != MERCD_CTRL_BLOCK_READY) {
            printk("mid_strmmgr_set_param: MsdState: 0x%x\n", 
                                     MsdControlBlock->MsdState);
	    AckPtr->ErrorCode = CD_ERR_BAD_STATE;
	    ReturnValue = MsdControlBlock->MsdState;
	    goto out;
	}


        BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
	StreamBlock = BindBlock->stream_connection_ptr;

	if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
            printk("mid_strmmgr_set_param: NULL StreamBlock for id %d & bh %d\n", StreamId, BindHandle);
	    AckPtr->ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
	    goto out;
  	}

	// validity check 
	if (Ptr->Count > MD_MAX_STREAM_PARAM_COUNT){
            printk("StreamProcGetParam: Count: %d > max: %d\n",
                                  Ptr->Count, MD_MAX_STREAM_PARAM_COUNT);
	     AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
	     goto out;
	}

	// process individual entries.  There could be multiple of these
	 
	EntryPtr = (PSTREAM_PARAMETER_ENTRY)(Ptr+1);
	for (i=0;i<Ptr->Count;i++) {
	     switch(EntryPtr->Key) {
		case STREAM_KEY_SEND_Q_LIMIT: // send queue limit 

     		   if (!StreamBlock->WWMode) {
		      if ((EntryPtr->Value.UlongValue[0] > MD_MAX_STREAM_SEND_BYTE_COUNT) || 
			  (EntryPtr->Value.UlongValue[0]<MD_MIN_STREAM_SEND_BYTE_COUNT))  {
#ifdef LFS
			   printk("Bad Entry %p=%u.\n", &EntryPtr->Value.UlongValue[0], EntryPtr->Value.UlongValue[0]);
#else
			   printk("Bad Entry %x=%d.\n", &EntryPtr->Value.UlongValue[0], EntryPtr->Value.UlongValue[0]);
#endif
			   EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
			   AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
			   break;
		      }

		      // Ensure it is multiple of 4056  Comes to 64k
		      StreamBlock->qparam.snd_queue_limit= ((((EntryPtr->Value.UlongValue[0])/StreamBlock->ContainerSize) 
				      				       * StreamBlock->ContainerSize)*4); // multiply by 4
       		   } else { 
         	      //WWMode: We just give the same snd_queue_limit
         	      //Store the wwrcvBufSize
         	      StreamBlock->qparam.snd_queue_limit = 
                        ((((4*(sizeof(USER_HEADER)+MERCURY_HOST_IF_BLK_SIZE))/4056)*4056)*4);

         	      if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) {
            		  StreamBlock->WWRcvBufSize = (EntryPtr->Value.UlongValue[0])/4056;

                          //We can do the calculation: But, snd_queue_limit is 
                          //around 64k, and we can not afford to preallocate
                          //16 x 4k buffers per stream.
                          //In anycase, FW can only process 4 receive buffers
                          //any given time and hence we limit our preallocated
                          //receive buffers to 4 for the read stream buffers.
                          //We keep replenishing them as when the need arise.
 
           		  StreamBlock->WWRcvNumBufs = (MERCD_WW_MAX_PRE_ALLOCATED_RECV_STREAM_BUFFERS);
     	                  StreamBlock->szRcvBitMap = StreamBlock->WWRcvNumBufs;

			  //Two methods of posting the receive stream buffers in WWMode
			  //of operation: MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED
			  //              MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE
			  StreamBlock->WWRcvStrDataBufPostMethod = MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED;
			  StreamBlock->WWRcvStrDataBufPostInAdvance = 1;
			  StreamBlock->WWRcvStrDataBufPostPending = 0;
			  StreamBlock->WWRcvStrDataBufAllocd = 0;
          	      } /* STREAM_OPEN_F_RECEIVE_ONLY */
         	        else {
		          //None of the following applies for Write Stream Block
            		  StreamBlock->WWRcvBufSize = 0;
                          StreamBlock->WWRcvNumBufs = 0;
                          StreamBlock->szRcvBitMap = 0;
			  StreamBlock->WWRcvStrDataBufPostMethod = 0;
			  StreamBlock->WWRcvStrDataBufPostInAdvance = 0;
			  StreamBlock->WWRcvStrDataBufPostPending = 0;
			  StreamBlock->WWRcvStrDataBufAllocd = 0;
                      }
                   }
		   break;
		   
		case STREAM_KEY_SEND_Q_LOW: // send queue low water mark 
			if (EntryPtr->Value.UlongValue[0] > MD_MAX_STREAM_SEND_BYTE_COUNT) {
			    EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
			    AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
			}
			StreamBlock->qparam.snd_queue_low= EntryPtr->Value.UlongValue[0]; 
			break;

	        case STREAM_KEY_CONTAINER_SIZE:
                   StreamBlock->ContainerSize = EntryPtr->Value.UlongValue[0];
                   StreamBlock->can_take = ((EntryPtr->Value.UlongValue[0] / StreamBlock->ContainerSize ) * StreamBlock->ContainerSize);
                   StreamBlock->multiplication_factor = EntryPtr->Value.UlongValue[1];

                   break;
			
		default:
		   EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
		   AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
		   break;
	     }

  	     EntryPtr++;
	} /* for */

out:
	// since all structs are identical, just change the msg
	// id and send the msg back 

	MdMsg->MessageId = MID_STREAM_SET_PARAMETER_ACK;

	// return the message to user via sync path 
	// supp_process_sync_receive(MsdOpenBlock, Msg);

	Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

	if (MD_GET_MSG_B_CONT(Msg)) {
	    mercd_streams_free(MD_GET_MSG_B_CONT(Msg),MERCD_STREAMS_BUFFER, MERCD_FORCE );
	    MD_SET_MSG_B_CONT(Msg, NULL);
	}

	// sync msg uses the sync return path 
	supp_process_sync_receive(MsdOpenBlock, Msg);
	return;
}

/***************************************************************************
 * Function Name                : mid_strmmgr_strm_terminate
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_strmmgr_strm_terminate(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
    PSTREAM_SEND_TERMINATE                          Ptr;
#ifdef LFS
    pmercd_adapter_block_sT                        padapter = NULL;
#else
    pmercd_adapter_block_sT                        padapter;
#endif
    pmercd_stream_connection_sT                    StreamBlock;
    PSTRM_MSG                                      SendMsg;
    PMDRV_MSG                                      SendMdMsg;
#ifndef LFS
    PSTREAM_SEND                                   SendPtr;
#endif
    PSTRM_MSG                                      DataMsg;
    PSTREAM_SEND_TERMINATE                         TerminateStreamMsg;
    PMERC_HOSTIF_MSG                               MercMsg;
    PMDRV_MSG                                      MdMsg =  MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
    merc_uint_t                                    ErrorCode =  MD_OK;
    merc_uint_t                                    ReturnValue = 0;
    merc_uint_t                                    Result;
    merc_uint_t                                    StreamId = 0;
    merc_uchar_t                                   Flag;
    MSD_HANDLE                                     StreamHandle;

    MSD_DRIVER_STATE_CHECK(MERCD_CTRL_BLOCK_READY);

    // Get pointer to payload
    Ptr = (PSTREAM_SEND_TERMINATE)MD_GET_MDMSG_PAYLOAD(MdMsg);
    StreamId = Ptr->StreamId;

    // perform validity checking
    StreamHandle = MD_GET_MDMSG_STREAM_HANDLE(MdMsg);

    if (StreamHandle >= MsdControlBlock->maxstreams)  {
        printk("mid_strmmgr_strm_terminate: bad stream handle %d id %d\n", StreamHandle, StreamId);
        ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
        goto out2;
    }

    MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

    StreamBlock = search_streamblock(MsdOpenBlock, StreamHandle);

    // Validity Checking
    if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
        printk("mid_strmmgr_strm_terminate: NULL StreamBlock for id %d & sh %d\n", StreamId, StreamHandle);
        ErrorCode = CD_ERR_BAD_STREAM_HANDLE;
        MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
        goto out2;
    }

    padapter = StreamBlock->padapter_block;

    if (padapter == NULL) {
        MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
        printk("mid_strmmgr_strm_terminate: bad board number.\n");
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        goto out2;
    }

    MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

    if (!(StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND)) {
        printk("mid_strmmgr_strm_terminate: Stream in bad state =%d StreamId=%d \n",
                StreamBlock->state,StreamBlock->id);
        ErrorCode = CD_ERR_BAD_STREAM_STATE;
        goto out2;
    }

    MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

    if ((padapter->state != MERCD_ADAPTER_STATE_READY) &&
        (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED)) {
        printk("mid_strmmgr_strm_terminate: Adapter State not ready.\n");
        ErrorCode = CD_ERR_BAD_BOARD_STATE;
        goto out;
    }

    // Change the state to indicate a TERMINATE PEND. Once we have sent the
    // terminate, we should only allow for a close to go down on that stream
    //StreamBlock->state = MERCD_STREAM_STATE_TERMINATE_PEND;

    // In the case of a terminate we formulate a QCNTRL_TERMINATE
    // and send it down to the firmware. The firmware then responds
    // with a QCNTRL_SUCCEDED or a QCNTRL FAILED for this request 

    // allocate a new mesage for our use
    SendMsg = supp_alloc_buf(sizeof(MDRV_MSG),GFP_KERNEL);
    if (SendMsg == NULL) {
        printk("mid_strmmgr_strm_terminate: allocbuf failed\n");
        ErrorCode = PAM_ERR_NO_MEMORY;
        ReturnValue = padapter->adapternumber;
        goto out;
    }

    DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+sizeof(STREAM_SEND_TERMINATE),GFP_KERNEL);
    if (DataMsg == NULL) {
        printk("mid_strmmgr_strm_terminate: Cannot alloc Data buffer\n");
        ErrorCode = PAM_ERR_NO_MEMORY;
        ReturnValue = padapter->adapternumber;
        mercd_streams_free(SendMsg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
        goto out;
    }

    MD_SET_MSG_TYPE(DataMsg, M_DATA);
    MSD_LINK_MESSAGE(SendMsg,DataMsg);

    MercMsg= (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
    MERCURY_SET_BODY_SIZE(MercMsg,sizeof(STREAM_SEND_TERMINATE));

    // create a host interface driver class message
    SendMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(SendMsg);

    StreamId = StreamBlock->id;

    Flag = MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER;
    MERCURY_SET_MESSAGE_FLAG(MercMsg,Flag);
    MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);

    // CLOSE STREAM is CLOSE_STREAM_ABORT
    MERCURY_SET_MESSAGE_TYPE(MercMsg,QCNTRL_TERMINATE);

    while(strm_fnd_entry_from_adapter(padapter,padapter->TransactionId))
          padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

    MERCURY_SET_TRANSACTION_ID(MercMsg,padapter->TransactionId);
    padapter->TransactionId= ((padapter->TransactionId + 1)%0xFFFF);

    // Transation id may overflow in next 1-2 quarters.
    // Better solution will be provided. If the transation id is
    // wrapped around, and that id is already in open,
    // we have a possible problem.
    if (padapter->TransactionId == 0xFFFFFFFF)
        printk("mid_strmmgr_strm_terminate: Transaction ID overflow\n");

    MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
    MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
    MERCURY_SET_DEST_COMPINST(MercMsg,DEFAULT_DEST_COMPINST);
    MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);

    TerminateStreamMsg = (PSTREAM_SEND_TERMINATE)MERCURY_BODY_START(MercMsg);
    TerminateStreamMsg->StreamId = StreamId;
    TerminateStreamMsg->Mode = Ptr->Mode;

    if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
            Result = strm_Q_snd_msg(SendMsg, padapter);
    } else {
        if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
            Result = strm_ww_Q_snd_msg(SendMsg, padapter);
            Result = MD_SUCCESS;
        } else {
            printk("mid_strmmgr_strm_terminate: Adapter out of srvc (invalid WW state)\n");
            padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
            goto out;
        }
    }

    if (Result != MD_SUCCESS) {
        ErrorCode = Result;
        ReturnValue = padapter->adapternumber;
        printk("mid_strmmgr_strm_terminate:Q_snd_msg failed.Err=%d\n", ErrorCode);
        mercd_streams_free(SendMsg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
        goto out;
    }

    // free old message
    mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);

    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
    return;

out:
    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
out2:
    // Only Errrors are handled here
    Msg = supp_drvr_mk_ack(Msg,MD_GET_MDMSG_ID(MdMsg),ErrorCode,ReturnValue);

    if (MD_GET_MSG_B_CONT(Msg)) {
        mercd_streams_free(MD_GET_MSG_B_CONT(Msg),MERCD_STREAMS_BUFFER, MERCD_FORCE);
        MD_SET_MSG_B_CONT(Msg, NULL);
    }

    // for async messages, send back a async reply
    if (MD_IS_MSG_ASYNC(Msg)) {
        // msg is freed in the send function
        supp_process_receive(MsdOpenBlock, Msg);
    } else {
        // sync msg uses the sync return path
        supp_process_sync_receive(MsdOpenBlock, Msg);
    }

    return;
}


/***************************************************************************
 * Function Name                : mid_strmmgr_strm_open_hcs
 * Function Type                : manager function
 * Inputs                       : padapter, StreamBlock
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  : open a host controlled stream
 * Additional comments          :
 ****************************************************************************/
merc_uint_t mid_strmmgr_strm_open_hcs(pmercd_adapter_block_sT padapter, 
		    pmercd_stream_connection_sT StreamBlock, PSTRM_MSG NewMsg)
{
   PSTRM_MSG			Msg;	
#ifndef LFS
   PSTRM_MSG                    DataMsg;
#endif
   PMDRV_MSG 			MdMsg;
   merc_ulong_t			streamId;
   PSTREAM_OPEN_ACK		Ptr;
   PHIF_OPEN_STREAM             OpenStreamMsg;
   PMERC_HOSTIF_MSG             MercMsg;
   merc_ulong_t			CanTakeCount;
#ifndef LFS
   merc_ulong_t          	szInBoundMfa = 0;
#endif
   merc_uint_t                  Result;
#ifndef LFS
   merc_uint_t           	numEntries = 0;
#endif
   pmercd_bind_block_sT    	BindBlock;
   pmercd_open_block_sT         OpenBlockContext;
#ifndef LFS
   pmercd_ww_dma_descr_sT 	pdmaDescr = 0;
   merc_uint_t          	szBMap = 0;
   merc_int_t i, actualBlkSize = MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER);
#else
   merc_int_t i;
#endif

   // Ensure the nextAvailableStreamId is available
   if (padapter->HCS.pStreamStateArray[padapter->HCS.nextAvailableStreamId] != HCS_AVAILABLE) {
       for (i=0; i<padapter->HCS.maxNumOfHCS; i++) {
	    if (padapter->HCS.pStreamStateArray[i] == HCS_AVAILABLE) {
		padapter->HCS.nextAvailableStreamId = i;
	       	break;
	    }
       }
       // debug check
       if (i >= padapter->HCS.maxNumOfHCS) {
	   printk("mid_strmmgr_strm_open_hcs: maxed out HCSs\n");
	   return (MD_FAILURE);
       }
   }

   // save the stream state
   streamId = padapter->HCS.nextAvailableStreamId;
   MercMsg= (PMERC_HOSTIF_MSG)NewMsg->b_cont->b_rptr;
   OpenStreamMsg = (PHIF_OPEN_STREAM)MERCURY_BODY_START(MercMsg);
   padapter->HCS.pStreamStateArray[streamId] = OpenStreamMsg->Mode;

   // update the nextAvailableStreamId
   padapter->HCS.nextAvailableStreamId = (padapter->HCS.nextAvailableStreamId+1) % padapter->HCS.maxNumOfHCS;

   // The actual stream Id is offset by the starting stream Id
   streamId += padapter->HCS.firstHCSId;
   OpenStreamMsg->StreamId = streamId;
   
   // send the QCNTRL_SET_STREAM_PROPERTY w/ updated StreamId for FW
   if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
       Result = strm_Q_snd_msg(NewMsg, padapter);
   } else {
       if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
           Result = strm_ww_Q_snd_msg(NewMsg, padapter);
       } else {
           printk("mid_strmmgr_strm_open_hcs: invalid adapter state\n");
           Result = MD_FAILURE;
       }
   }

   // Check for errors
   if (Result == MD_FAILURE) {
       return (Result);
   }

   // Form a new message for host (OPEN ACK)
   Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_OPEN_ACK), GFP_ATOMIC);
   if (Msg == NULL) {
       printk("mid_strmmgr_strm_open_hcs: memory allocation failed\n");
       return (MD_FAILURE);
   }

   Msg->b_cont = NULL;
   MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)+sizeof(STREAM_OPEN_ACK)));

   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   MD_SET_MDMSG_ID(MdMsg, MID_STREAM_OPEN_ACK);
   MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);

   Ptr = (PSTREAM_OPEN_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
   Ptr->StreamId = streamId;
   Ptr->ActualSize = padapter->HCS.sizeOfHCS;
   Ptr->TransactionId = StreamBlock->TransactionId;
   Ptr->ErrorCode = MD_OK;
   CanTakeCount = MERCD_WW_MAX_MFS_PER_STREAM;
   if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
       Ptr->CanTakeByteCount = CanTakeCount;
   } else {
       if (NewCanTakeProtocol &&  (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)) {
           // for customer who can't handle lot of initial read callbacks
           CanTakeCount = 4;
           Ptr->CanTakeByteCount = CanTakeCount;
       } else {
           Ptr->CanTakeByteCount = CanTakeCount * (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
       }
   }

   // initialize the StreamBlock
   StreamBlock->id = Ptr->StreamId;
   StreamBlock->state = MERCD_STREAM_STATE_CONNECTED;
   StreamBlock->qparam.snd_queue_low= MSD_STREAM_SEND_LOW;
   StreamBlock->qparam.cur_snd_cnt = 0;

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
               StreamBlock->can_take = (Ptr->CanTakeByteCount/SRAM_BLOCK_MAX_DATA_SIZE) *
                                                               StreamBlock->ContainerSize;
           } else {
               StreamBlock->can_take = 0;
           }   
   
           if (StreamBlock->cantakeMode == 0) {
               StreamBlock->qparam.snd_queue_limit = SEND_QUEUE_LIMIT_MULTIPLIER * SRAM_BLOCK_MAX_DATA_SIZE;
               Ptr->CanTakeByteCount = StreamBlock->qparam.snd_queue_limit / SEND_QUEUE_LIMIT_DIVISOR;
           } else {
               Ptr->CanTakeByteCount = StreamBlock->multiplication_factor * StreamBlock->ContainerSize;
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
               StreamBlock->can_take = CanTakeCount * (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
           } else {
               StreamBlock->can_take = 0;
           }

           if (StreamBlock->cantakeMode == 0) {
               StreamBlock->qparam.snd_queue_limit =  (CanTakeCount) * (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
               Ptr->CanTakeByteCount = (CanTakeCount) * (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
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

   // Now set the ptr in BindBlock.  DO NOT set before OPEN_ACK
   // is received since that'll cause an internal close to
   // be sent  if user closes the stream before OPEN_ACK is received.
   BindBlock = StreamBlock->pbind_block;
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

   // HCS Support - using HCS method
   StreamBlock->isStreamHCS = 1;
   padapter->HCS.numOfHCSActive++;

   supp_process_receive(OpenBlockContext, Msg);

   return (MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_strmmgr_strm_close_hcs
 * Function Type                : manager function
 * Inputs                       : padapter, StreamBlock
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  : close a host controlled stream
 * Additional comments          :
 ****************************************************************************/
merc_uint_t mid_strmmgr_strm_close_hcs(pmercd_adapter_block_sT padapter,
                    pmercd_stream_connection_sT StreamBlock, PSTRM_MSG SendMsg)
{
   PSTRM_MSG                    Msg;
#ifndef LFS
   PSTRM_MSG                    DataMsg;
#endif
   PMDRV_MSG                    MdMsg;
   merc_uint_t                  Result;
   merc_ulong_t                 StreamId;
   PSTREAM_CLOSE_ACK       	Ptr;
   pmercd_bind_block_sT         BindBlock;
   pmercd_open_block_sT  	OpenBlockContext;

   StreamId = StreamBlock->id - padapter->HCS.firstHCSId;

   // check for valid stream Id
   if ((StreamId < 0) || (StreamId > padapter->HCS.maxNumOfHCS)) {
#ifdef LFS
       printk("mid_strmmgr_strm_close_hcs: out of range stream id %lu\n", StreamId);
#else
       printk("mid_strmmgr_strm_close_hcs: out of range stream id %d\n", StreamId);
#endif
       return (MD_FAILURE);
   }

   // check if the HCS stream was opened
   if (padapter->HCS.pStreamStateArray[StreamId] == HCS_AVAILABLE) {
#ifdef LFS
       printk("mid_strmmgr_strm_close_hcs: stream id %lu not opened\n", StreamId);
#else
       printk("mid_strmmgr_strm_close_hcs: stream id %d not opened\n", StreamId);
#endif
       return (MD_FAILURE);
   }

   // send the QCNTRL_RESET_STREAM_PROPERTY 
   if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
       Result = strm_Q_snd_msg(SendMsg, padapter);
   } else {
       if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
           Result = strm_ww_Q_snd_msg(SendMsg, padapter);
       } else {
           printk("mid_strmmgr_strm_close_hcs: invalid adapter state\n");
           Result = MD_FAILURE;
       }
   }

   // Check for errors
   if (Result == MD_FAILURE) {
       return (Result);
   }

   // For internal close just return
   if (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND) {
       // mark the HCS as available
       padapter->HCS.pStreamStateArray[StreamId] = HCS_AVAILABLE;
       padapter->HCS.numOfHCSActive--;
       return (MD_SUCCESS);
   }

   // Form a new message for host (CLOSE ACK)
   Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_CLOSE_ACK), GFP_ATOMIC);
   if (Msg == NULL) {
       printk("mid_strmmgr_strm_close_hcs: memory allocation failed\n");
       return (MD_FAILURE);
   }
   Msg->b_cont = NULL;
   MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)+sizeof(STREAM_CLOSE_ACK)));
   
   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg); 
   MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_ACK);
   MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);

   Ptr = (PSTREAM_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
   Ptr->StreamId = StreamBlock->id;
   Ptr->ErrorCode = MD_OK; 

   // take care of Persistent stream
   if (StreamBlock->flags & STREAM_OPEN_F_PERSISTENT) {
       StreamBlock->state = MERCD_STREAM_STATE_TERMINATE_PEND;
       BindBlock = StreamBlock->pbind_block;

       MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
       OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);
       MD_SET_MDMSG_STREAM_HANDLE(MdMsg, StreamBlock->handle);
       MD_SET_MDMSG_BIND_HANDLE(MdMsg, StreamBlock->pbind_block->bindhandle);
       MD_SET_MDMSG_USER_CONTEXT(MdMsg, StreamBlock->pbind_block->UserContext);
       MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);

       supp_process_receive(OpenBlockContext, Msg);
       return (MD_SUCCESS);
   }

   // must clear all links in MsdStreamMapTable and adapter block StreamIdMapTable
   MSD_ENTER_MUTEX(&padapter->snd_data_mutex);
   if (StreamBlock->SendStreamQueue){
       MSD_FREE_MESSAGE(StreamBlock->SendStreamQueue );
       StreamBlock->SendStreamQueue=NULL;
   }

   MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
   // finally, remove the stream block from adapter block stream list
   if (queue_remove_from_Q(&padapter->stream_adapter_list, (PMSD_GEN_MSG)StreamBlock) != MD_SUCCESS) {
       printk("mid_strmmgr_strm_close_hcs: RemoveFromQ failed for StreamId=%d\n",StreamBlock->id);
       return (MD_FAILURE);
   }

   padapter->pstream_connection_list[Ptr->StreamId] = NULL;
   MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
   MSD_EXIT_MUTEX(&padapter->snd_data_mutex);

   if (padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
       if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
           if (StreamBlock->writeDMAPending) {
               MSD_ERR_DBGPRINT("Close Strm ACK Rcvd while Write DMA Complete Pending..id: %d\n", StreamBlock->id);
               if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
                   MSD_ERR_DBGPRINT("id : %d is a Read Stream\n", StreamBlock->id);
               else
                   MSD_ERR_DBGPRINT("id : %d is a Write Stream\n", StreamBlock->id);
           }

           if (((StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) && (StreamBlock->pSendBitMapValue)) ||
               ((StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) && (StreamBlock->pRcvStrmDmaDesc))) {
                    mid_wwmgr_close_streams(padapter, StreamBlock, 1);
	   }

           // For an internal close like ^C, we would have sent an internal close and  would have 
	   // overwritten the StreamBlock->state from INTERNAL_CLOSE_PEND to MERCD_STREAM_STATE_FREED.
           if (StreamBlock->state == MERCD_STREAM_STATE_FREED) {
               MSD_FREE_MESSAGE(Msg);
           }
       }
   }

   // if stream close was internally generated, release the
   // message and break; All other links were destroyed earlier on
   if (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND){
       BindBlock = StreamBlock->pbind_block;
       
       MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
       if (BindBlock->stream_connections) {
           MSD_SIGNAL_CV(&BindBlock->bind_block_termination_cv);
       }

       BindBlock->stream_connection_ptr = NULL;
       BindBlock->stream_connections--;
       MSD_EXIT_MUTEX(&BindBlock->bind_block_mutex);
       MSD_FREE_MESSAGE(Msg);
    } else {
       if (StreamBlock->state != MERCD_STREAM_STATE_FREED) {
           BindBlock = StreamBlock->pbind_block;

           MSD_ENTER_MUTEX(&BindBlock->bind_block_mutex);
           OpenBlockContext = MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock);
           // close stream failed or passed shutdown connection anyways
           // must set bind handle and user context in msg

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

   // mark the HCS as available
   padapter->HCS.pStreamStateArray[StreamId] = HCS_AVAILABLE;
   padapter->HCS.numOfHCSActive--;

   // free the stream block memory space
   mercd_free(StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE);

   return (MD_SUCCESS);
}
