/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdsnd.c
 * Description			: Host FW Send functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDSND_C
#include "msdextern.h"
#undef _MSDSND_C
extern int NewCanTakeProtocol;

/***************************************************************************
 * Function Name		: snd_msgs2adapter
 * Function Type		: Host FW Send function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver snd_msgs2adapter Routine.
 *				  This routine sends message to adapter.
 * Additional comments		:
 ****************************************************************************/
int snd_msgs2adapter(pmercd_adapter_block_sT padapter)
{

   PMDRV_MSG               MdMsg;
   PSTRM_MSG               Msg;
   pmercury_host_info_sT   HostIfPtr;
   PMERC_CIR_BUFFER        CirBuffer= NULL;
   PMERC_HOSTIF_MSG        MercMsg;
   PMERC_DATA_BLK_HDR      KvDataBlockPtr;
   PMERC_DATA_BLK_HDR      DataBlkPtr;
   mercd_dhal_mem_copy_sT  mem_copyinfo = { 0 } ;
   pmerc_char_t            SramDataPtr;
   size_t                  NextWrite;
   size_t                  ReadIndex;
   size_t                  NextMessageAddress;
   merc_uint_t             MaxMessages;
   merc_uint_t             MsgBodySize;


   HostIfPtr=&padapter->phost_info->merc_host_info;
   CirBuffer=(PMERC_CIR_BUFFER)HostIfPtr->host_to_fw_msg_start;
   MaxMessages = padapter->phost_info->host_config.maxHostMsgs;
	
   NextWrite = CirBuffer->Write;
   ReadIndex = CirBuffer->Read;

   // If sram is corrupt, let the sanity function take care of it
   if (!(ReadIndex < MaxMessages)||!(NextWrite < MaxMessages)){
       printk("snd_msgs2adapter\n");
       msd_sram_sanity_dump(padapter);
       return(MD_FAILURE);
   }

   if ((NextWrite+1)%MaxMessages == ReadIndex)  {
#ifdef DRVR_STATISTICS
       padapter->fw_overrun_msg++;
#endif
        return(MD_FAILURE);
   }

   // get a message from the send queue
   if ((Msg = queue_get_msg_Q(&padapter->snd_msg_queue)) == NULL) {
       MSD_ERR_DBGPRINT("snd_msgs2adapter: Sendqueue empty.\n");
       return(MD_FAILURE);
   }
	                                                  
   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	
   // keep writing to cir buf until full 
   while (Msg && ((NextWrite+1)%MaxMessages != ReadIndex)) {
       MSD_ASSERT(Msg->b_cont);
       MercMsg = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;

       if (MERCURY_GET_TRANSACTION_ID(MercMsg) == 0x800000) {
           padapter->sndflowcount++;
       }
	  
       NextMessageAddress = (merc_uint_t)HostIfPtr->host_to_fw_msg_start+
           sizeof(MERC_CIR_BUFFER)+((sizeof(MERC_HOSTIF_MSG)+MD_OLD_BODY_SIZE)*NextWrite);

       // Big message?
       if ((MsgBodySize=MERCURY_GET_BODY_SIZE(MercMsg)) > MD_OLD_BODY_SIZE) {
           // get a free data node from sram
           KvDataBlockPtr = snd_alloc_Hif_data_blk(padapter);
           if (KvDataBlockPtr == NULL) {
       	       MSD_ERR_DBGPRINT("snd_msgs2adapter:Free list empty.\n"); 
	       // put back msg to list 
	       queue_put_bk_msg_Q(&padapter->snd_msg_queue,Msg);
	       goto out;
	   }

	   DataBlkPtr=(PMERC_DATA_BLK_HDR)KvDataBlockPtr;

	   if (!DataBlkPtr->DataBlockOffset){
	       //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
               printk("snd_msgs2adapter: Sram Corrupt \n");
	       queue_put_bk_msg_Q(&padapter->snd_msg_queue,Msg);
	       snd_release_Hif_data_blk(padapter, (PMERC_DATA_BLK_HDR)DataBlkPtr);
	       return(MD_FAILURE);
	   }
	
 	   SramDataPtr = (pmerc_char_t)TO_KV_ADDRESS(padapter, DataBlkPtr->DataBlockOffset);
	   CHECK_SRAM_ADDRESS(SramDataPtr, "snd_msgs2adapter1", padapter);

	   mem_copyinfo.src = (pmerc_char_t)MercMsg;
	   mem_copyinfo.dest = (pmerc_char_t)SramDataPtr;
	   mem_copyinfo.size = sizeof(MERC_HOSTIF_MSG) + MsgBodySize;
	   mem_copyinfo.datatype = MSD_INT_DATA;
	   mem_copyinfo.dir = MSD_TO_SRAM;

	   (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

	   // Buffer data count set to body plus hdr 
	   DataBlkPtr->DataCount= sizeof(MERC_HOSTIF_MSG)+MsgBodySize;
	   DataBlkPtr->BufFlags = 0;
	   DataBlkPtr->Sequence = 0;

	   MercMsg = (PMERC_HOSTIF_MSG)NextMessageAddress;

	   // save the body start offset in the merc msg buffer field 
	   MercMsg->BodySize = TO_HIF_OFFSET(padapter,DataBlkPtr);

       } else {
	   // Small message
	   CHECK_SRAM_ADDRESS(NextMessageAddress, "snd_msgs2adapter2", padapter);

	   mem_copyinfo.src = (pmerc_char_t)Msg->b_cont->b_rptr;
	   mem_copyinfo.dest = (pmerc_char_t)NextMessageAddress;
	   mem_copyinfo.size = sizeof(MERC_HOSTIF_MSG) + MsgBodySize;
	   mem_copyinfo.datatype = MSD_INT_DATA;
	   mem_copyinfo.dir = MSD_TO_SRAM;

	   (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

       }

       // Trace Code: Copy the Message to the buffer (if required)
       if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
           supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
       }

       // point to next msg in the cir queue 
       NextWrite = (NextWrite+1)%MaxMessages;

       MSD_FREE_MESSAGE(Msg);

       if ((Msg = queue_get_msg_Q(&padapter->snd_msg_queue)) == NULL) {
	   // no more msgs to process
	   break;
       }
       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   }

   if (Msg!=NULL && ((NextWrite+1)%MaxMessages == ReadIndex)) {
       // Full Queue in SRAM
       queue_put_bk_msg_Q(&padapter->snd_msg_queue,Msg);
   }

out:	
   CirBuffer->Write = NextWrite;
   // indicate the need for interrupting the board
   padapter->flags.LaunchIntr |=  MERC_ADAPTER_FLAG_LAUNCH_INTR;
   return(MD_SUCCESS);
}


/***************************************************************************
 * Function Name		: snd_data2adapter
 * Function Type		: Host FW Send function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver snd_data2adapter Routine.
 *				  This routine sends data to adapter.
 * Additional comments		:
 ****************************************************************************/
int snd_data2adapter(pmercd_adapter_block_sT padapter)
{

   PSTRM_MSG                            Msg;
   PSTRM_MSG                            TmpMsg;
   PMDRV_MSG                            MdMsg;
   pmercury_host_info_sT                HostIfPtr;
   PMERC_CIR_BUFFER                     CirBuffer= NULL;
   PMERC_DATA_BLK_HDR                   KvDataBlockPtr;
   PMERC_DATA_BLK_HDR                   DataBlkPtr;
   PMERC_DATA_CIR_BUF_ENTRY             CirBufEntry= NULL;
   PSTREAM_SEND                         Ptr;
   PUSER_HEADER                         UserHeaderPtr;
   mercd_dhal_mem_copy_sT               mem_copyinfo = { 0 } ;
   size_t                               NextWrite;
   size_t                               ReadIndex;
   size_t                               NextMessageAddress;
   pmerc_uchar_t                        MsgDataPtr;
   pmerc_char_t                         SramDataPtr;
   merc_uint_t                          SentCount=0;
   merc_uint_t                          MsgDataSize;
   merc_uint_t                          MaxMessages;
   MSD_HANDLE  				StreamHandle;
   merc_ushort_t 			Flags;
   pmercd_stream_connection_sT  	StreamBlock;


   // Before proceeding Collect the Data to be sent
   strm_gather_snd_blks(padapter);

   MSD_LEVEL2_DBGPRINT("snd_data2adapter: Msg1->b_next=%x\n",Msg->b_next);

   HostIfPtr=&padapter->phost_info->merc_host_info;
   CirBuffer = (PMERC_CIR_BUFFER) HostIfPtr->host_to_fw_data_start;
   MaxMessages = padapter->phost_info->host_config.maxHostDataXfer;

   NextWrite = CirBuffer->Write;
   ReadIndex = CirBuffer->Read;


   if ((NextWrite+1)%MaxMessages == ReadIndex)  {
#ifdef DRVR_STATISTICS
       padapter->fw_overrun_data++;
#endif
       return(MD_FAILURE);
   }	

   // If sram is corrupt, let the sanity function take care of it
   if (!(ReadIndex < MaxMessages)||!(NextWrite < MaxMessages)){
       printk("snd_data2adapter\n");
       msd_sram_sanity_dump(padapter);
       padapter->snd_data_queue= NULL;
       return(MD_FAILURE);
   }

   if ((Msg = padapter->snd_data_queue) == NULL){
       MSD_LEVEL2_DBGPRINT("snd_data2adapter: Sendqueue empty.\n"); 
       return(MD_FAILURE);
   } 

   // start sending messages and data blocks 
   while (Msg && ((NextWrite+1)%MaxMessages != ReadIndex)) {
       // Msg must have the data portion attached 
       if (!(MD_GET_MSG_B_CONT(Msg))) {
           printk("snd_data2adapter: No data porion attached in the message\n");
       }
       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

       // Following code is to send user ack from here
       StreamHandle = MD_GET_MDMSG_STREAM_HANDLE(MdMsg);

       Ptr= (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
       StreamBlock = padapter->pstream_connection_list[Ptr->StreamId];

       if (StreamBlock == NULL) {
	   MSD_FREE_MESSAGE(Msg);
	   padapter->snd_data_queue= NULL;
	   return(MD_FAILURE);
       }
			
       UserHeaderPtr = &Ptr->StreamUserHeader;

#ifdef SKIP 
       // No Tracing For Data yet .... VISIT LATER
       // Trace Code: Copy the Message to the buffer (if required)
       if (padapter->TraceLevel) {
 	   supp_push_trace_msg(padapter, MSD_DRV2BRD_TRACE, (PMDRV_MSG)MdMsg);
       }
#endif 

       MSD_LEVEL1_DBGPRINT("snd_data2adapter: In Outer loop.\n"); 

       while ((TmpMsg = (PSTRM_MSG)MD_GET_MSG_B_CONT(Msg)) && 
		  ((NextWrite+1)%MaxMessages != ReadIndex) &&
	          padapter->pstream_connection_list[Ptr->StreamId]) {

	   MSD_LEVEL1_DBGPRINT("snd_data2adapter: In Inner loop. TmpMsg=%x\n",TmpMsg);

	   // get a free data node from sram 
	   KvDataBlockPtr = snd_alloc_Hif_data_blk(padapter);
	   if (KvDataBlockPtr == NULL) {
	       MSD_LEVEL1_DBGPRINT("snd_data2adapter: Free list empty.\n");
	       // put back msg to list 
	       padapter->snd_data_queue = Msg;
	       goto out;
	   }
	   
	   DataBlkPtr=(PMERC_DATA_BLK_HDR)KvDataBlockPtr;
 	   if (!DataBlkPtr->DataBlockOffset) {
	       //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
               printk("snd_data2adapter: Null DataBlockOffset \n");
	       MSD_FREE_MESSAGE(Msg);
	       padapter->snd_data_queue= NULL;
	       return(MD_FAILURE);
	   }

	   SramDataPtr = (pmerc_char_t) TO_KV_ADDRESS(padapter, DataBlkPtr->DataBlockOffset);

	   // calculate local data block ptr and size 
	   MsgDataPtr = MD_GET_MSG_READ_PTR(TmpMsg);

	   // cannot assume data are in 4K block 
	   MsgDataSize = MD_GET_MSG_WRITE_PTR(TmpMsg) - MD_GET_MSG_READ_PTR(TmpMsg);

	   if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
	       if (MsgDataSize > (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)))
	 	   MsgDataSize = (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));
	   } else {
	       if (MsgDataSize > MERCURY_HOST_IF_BLK_SIZE)
	 	   MsgDataSize = MERCURY_HOST_IF_BLK_SIZE;
	   }

	   MD_SET_MSG_READ_PTR(TmpMsg, MD_GET_MSG_READ_PTR(TmpMsg)+MsgDataSize);

 	   if ((Ptr->Flags & STREAM_FLAG_EOS) && (MsgDataSize)) {
               MSD_ERR_DBGPRINT("snd_data2adapter: Copying nothing, zero byte EOS\n");
	   } else if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
	       CHECK_SRAM_ADDRESS(SramDataPtr, "MsdSendDataToAdapterGS", padapter);

	       mem_copyinfo.src = (pmerc_char_t)MsgDataPtr;
	       mem_copyinfo.dest = (pmerc_char_t)SramDataPtr;
	       mem_copyinfo.size = MsgDataSize;
	       mem_copyinfo.datatype = MSD_SYS_DATA;
	       mem_copyinfo.dir = MSD_TO_SRAM;
	       (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);
	   } else {
 	       CHECK_SRAM_ADDRESS(SramDataPtr, "snd_data2adapter1", padapter);

	       mem_copyinfo.src = (pmerc_char_t)UserHeaderPtr;
	       mem_copyinfo.dest = (pmerc_char_t)SramDataPtr;
	       mem_copyinfo.size = sizeof(USER_HEADER);
	       mem_copyinfo.datatype = MSD_SYS_DATA;
	       mem_copyinfo.dir = MSD_TO_SRAM;
	       (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

	       CHECK_SRAM_ADDRESS((SramDataPtr + sizeof(USER_HEADER)), "snd_data2adapter2", padapter);

	       mem_copyinfo.src = (pmerc_char_t)MsgDataPtr;
	       mem_copyinfo.dest = (pmerc_char_t)(SramDataPtr + sizeof(USER_HEADER));
	       mem_copyinfo.size = MsgDataSize;
	       mem_copyinfo.datatype = MSD_SYS_DATA;
	       mem_copyinfo.dir = MSD_TO_SRAM;
 	       (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);
	   }

	   if ((Ptr->Flags & STREAM_FLAG_EOS) && !(MsgDataSize)) {
               DataBlkPtr->DataCount = MsgDataSize;
           } else {
               // Setup System header
               // Whatever was the data size make it 4056
               if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
                   DataBlkPtr->DataCount = MsgDataSize;
               else              
                   DataBlkPtr->DataCount = 4056;
           }

	   MSD_LEVEL2_DBGPRINT("snd_data2adapter: flags %x, size %d, setsize %d.\n", 
				    Ptr->Flags, MsgDataSize, DataBlkPtr->DataCount);
	   Flags = 0;
	   // set the data block flags 
	   if (Ptr->Flags & STREAM_FLAG_EOS) {
	       Flags |= HIF_STREAM_F_EOS;
	   }

	   DataBlkPtr->BufFlags = Flags;

	   MSD_LEVEL2_DBGPRINT("snd_data2adapter:SB %d	0x%x %d	%d\n", Ptr->StreamId,
		  DataBlkPtr->BufFlags, DataBlkPtr->DataCount, DataBlkPtr->Sequence);
	   SentCount++;

	   // Here Send an Ack to user :Send new Can Take to the user
	   StreamBlock->accumulated_usr_ack += MsgDataSize;

           if (!NewCanTakeProtocol) {
	       if (((StreamBlock->state != MERCD_STREAM_STATE_CLOSE_PEND) || 
		   (StreamBlock->state != MERCD_STREAM_STATE_TERMINATE_PEND)) &&
		   (StreamBlock->accumulated_usr_ack)) {

	           // send broken to prevent latency
		   if (Ptr->Flags & MD_MSG_FLAG_LATENCY) {
		       strm_sndbrokenstream2usr(StreamBlock);
		   } else {
		       if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
		           if ((Ptr->Flags & MULTI_BLK_SEND_CAN_TAKE) ||
			       !(Ptr->Flags & MULTI_BLK_NO_CAN_TAKE))  {
			       if (strm_ack2usr(StreamBlock, StreamBlock->accumulated_usr_ack) != MD_SUCCESS) {
			           MSD_ERR_DBGPRINT("snd_data2adapter: strm_ack2usr failed.\n");
			       }
			       StreamBlock->accumulated_usr_ack = 0;   /* clean it out. */
			   }
		       } 
                   }
	       } 
	   } else { 
	       // The following will only be executed with New Can Take feature 
               if (((StreamBlock->state != MERCD_STREAM_STATE_CLOSE_PEND) ||
		   (StreamBlock->state != MERCD_STREAM_STATE_TERMINATE_PEND)) &&
                   (StreamBlock->accumulated_usr_ack)) {
                   if (!(Ptr->Flags & STREAM_FLAG_EOS)) {
                       if ((Ptr->Flags & MULTI_BLK_SEND_CAN_TAKE) || 
			   !(Ptr->Flags & MULTI_BLK_NO_CAN_TAKE))  {
                           stream_add_cantake_queue(padapter, StreamBlock);
                       } 
                   }
               }
	   }

	   // calc the next message start 
	   NextMessageAddress = (merc_uint_t)HostIfPtr->host_to_fw_data_start
		+sizeof(MERC_CIR_BUFFER) +(sizeof(MERC_DATA_CIR_BUF_ENTRY)*NextWrite);

	   CirBufEntry = (PMERC_DATA_CIR_BUF_ENTRY)NextMessageAddress;
	   CirBufEntry->DataHeaderOffset = TO_HIF_OFFSET(padapter,KvDataBlockPtr);
	   CirBufEntry->StreamId = Ptr->StreamId;
	   MSD_LEVEL2_DBGPRINT("snd_data2adapter: StreamId %d\n", CirBufEntry->StreamId);

	   NextWrite = (NextWrite+1)%MaxMessages;

	   if (MD_GET_MSG_WRITE_PTR(TmpMsg) <= MD_GET_MSG_READ_PTR(TmpMsg)) {
	       // should free just the data blk here 
	       Msg->b_cont = TmpMsg->b_cont;
	       MSD_FREE_BUFFER(TmpMsg);
	   }
       } /* inner while */
		
       if (!TmpMsg) {
	   TmpMsg = Msg->b_next;
	   MSD_FREE_MESSAGE(Msg);	
	   Msg = TmpMsg;
	   padapter->snd_data_queue = Msg;
	   MSD_LEVEL2_DBGPRINT("snd_data2adapter: NewMsg=%x,NextWr=%d,ReadInd=%d, MaxMsg=%d \n", 
						          Msg,NextWrite,ReadIndex, MaxMessages);
       } else {
	   break;
       }
   } /* outer while */

   if (Msg !=NULL && ((NextWrite+1)%MaxMessages == ReadIndex)) {
       // Queue full - return msg queue to adapter block 
       MSD_LEVEL2_DBGPRINT("snd_data2adapter: Re-inserting msg on Adapter send queue.Msg=%x\n", Msg);
       MSD_LEVEL2_DBGPRINT("   NextWr=%d,ReadInd=%d, MaxMsg=%d \n", NextWrite,ReadIndex,MaxMessages);
       padapter->snd_data_queue = Msg;
       padapter->flags.SendDataPending |= MERC_ADAPTER_FLAG_SEND_DATA_PEND ; 
   }

out:
   // save write pointer 
   CirBuffer->Write = NextWrite;
   MSD_LEVEL2_DBGPRINT("snd_data2adapter: SendCnt = %d  ",SentCount); 
   padapter->flags.LaunchIntr |=  MERC_ADAPTER_FLAG_LAUNCH_INTR;
   return(MD_SUCCESS);
}


/***************************************************************************
 * Function Name		: snd_alloc_Hif_data_blk
 * Function Type		: Host FW Send function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver snd_alloc_Hif_data_blk Routine.
 *				  This routine is used to allocate a Data block
 *				  from SRAM
 * Additional comments		: Will need mutexes  intrdpc. timerdpc if included
 ****************************************************************************/
PMERC_DATA_BLK_HDR snd_alloc_Hif_data_blk(pmercd_adapter_block_sT padapter)
{
   pmercury_host_info_sT        HostIfPtr;
   pmercury_free_hdr_list_sT	FreePtr; 
   PMERC_DATA_BLK_HDR		Ptr; 


   HostIfPtr=&padapter->phost_info->merc_host_info;

   FreePtr = (pmercury_free_hdr_list_sT)HostIfPtr->pfree_list_header_start; 

   if (FreePtr->HeadNodeOffset == FreePtr->TailNodeOffset) {
       return(NULL);
   }

   // If the SRAM is corrupt. Then declare the adapter out of service
   if (!((FreePtr->HeadNodeOffset != 0) && (FreePtr->TailNodeOffset != 0))) {
       padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
       printk("snd_alloc_Hif_data_blk: Sram Corruption - Adapter Out Of Service\n");
       return(NULL);
   }

   Ptr = (PMERC_DATA_BLK_HDR)TO_KV_ADDRESS(padapter,FreePtr->HeadNodeOffset); 

   FreePtr->HeadNodeOffset = Ptr->NextNodeOffset;
   FreePtr->FreeBlockCount--;

   // CpFreeCounter not used for now 
   if (FreePtr->HeadNodeOffset == 0) {
       FreePtr->TailNodeOffset = 0;
   }

   Ptr->NextNodeOffset = 0;

   return(Ptr);
}

/***************************************************************************
 * Function Name		: snd_release_Hif_data_blk
 * Function Type		: Host FW Send function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver snd_release_Hif_data_blk Routine.
 *				  This routine is used to release a transfer 
 *				  completed Data block to the SRAM
 * Additional comments		:
 ****************************************************************************/
merc_void_t snd_release_Hif_data_blk(pmercd_adapter_block_sT padapter, PMERC_DATA_BLK_HDR DataBlockAddr)
{
   pmercury_host_info_sT HostIfPtr;
   PMERC_FREE_LIST_HDR	 FreePtr; 
   PMERC_DATA_BLK_HDR	 Ptr; 
   merc_uint_t		 DataBlockOffset;


   HostIfPtr=&padapter->phost_info->merc_host_info;

   // calc hif offset for data block  node
   DataBlockOffset = (merc_uint_t)TO_HIF_OFFSET(padapter,DataBlockAddr);

   FreePtr = (PMERC_FREE_LIST_HDR)HostIfPtr->pfree_list_header_start; 

   // is the free list empty
   if (FreePtr->HeadNodeOffset == 0) {
       FreePtr->HeadNodeOffset = DataBlockOffset;
       FreePtr->TailNodeOffset = DataBlockOffset;
       goto out;
   }

   Ptr = (PMERC_DATA_BLK_HDR)TO_KV_ADDRESS(padapter,FreePtr->TailNodeOffset);

   // link current tail to new node 
   Ptr->NextNodeOffset = DataBlockOffset;

   // update the tail
   FreePtr->TailNodeOffset = DataBlockOffset;

out:	

   Ptr = (PMERC_DATA_BLK_HDR)TO_KV_ADDRESS(padapter,FreePtr->TailNodeOffset);

   // set it's link to null 
   Ptr->NextNodeOffset = 0;

   // Increament the Free count in shared Ram 
   FreePtr->FreeBlockCount++;
   return;
}

