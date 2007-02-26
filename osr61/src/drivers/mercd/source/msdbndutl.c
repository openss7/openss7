/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdbndutl.c
 * Description			: Bind Manager Utility/Support functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDBNDUTL_C
#include "msdextern.h"
#undef _MSDBNDUTL_C


/***************************************************************************
 * Function Name		: bnd_snd_death_msg2brd
 * Function Type		: Support function for Bind manager function
 * Inputs			: Flags,
 *				  BlockPtr
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
md_status_t bnd_snd_death_msg2brd(merc_uint_t Flags, pmerc_void_t BlockPtr)
{
	PHIF_PROC_EXIT_NOTIFY	ExitNotifyPtr;
	PHIF_PROC_EXIT_ENTRY	ExitEntryPtr;
	PMERC_HOSTIF_MSG	MercMsg;
	PSTRM_MSG	NewMsg;
	PSTRM_MSG	DataMsg;
	PMSD_QUEUE	Queue;
	pmercd_adapter_block_sT	AdapterBlock;
	pmercd_bind_block_sT	BindBlock;
	pmercd_open_block_sT	MsdOpenBlock;
	merc_uint_t		i,j;
	merc_uint_t		BindCount = 0;
	merc_uchar_t		Flag;
	HIF_PROC_EXIT_ENTRY SourceAddressTable[100];
        merc_uint_t             NormalUnBind = 0;

        MSD_FUNCTION_TRACE("bnd_snd_death_msg2brd", TWO_PARAMETERS,
                            (size_t) Flags, (size_t) BlockPtr);


	if (Flags == MSD_ONLY_ONE_BIND_DEATH) {
		BindBlock = (PMSD_BIND_BLOCK)BlockPtr;
                NormalUnBind = BindBlock->NormalUnBind;
		BindCount = 1;
		// Always zero (board, node, processor)
		ExitEntryPtr = &SourceAddressTable[0];
		MERCURY_SET_SOURCE_NODE(ExitEntryPtr, 0);
		MERCURY_SET_SOURCE_BOARD(ExitEntryPtr, 0);
		MERCURY_SET_SOURCE_PROC(ExitEntryPtr, 0);
		MERCURY_SOURCE_BIND2COMP(ExitEntryPtr, BindBlock->bindhandle);
	} else {
		MsdOpenBlock = (PMSD_OPEN_BLOCK)BlockPtr;
		Queue = (PMSD_QUEUE)&MsdOpenBlock->bind_block_q;
		BindBlock = (PMSD_BIND_BLOCK)(PMSD_QUEUE)Queue->QueueHead;
                NormalUnBind = BindBlock->NormalUnBind;
		while (BindBlock) {
			ExitEntryPtr = &SourceAddressTable[BindCount];
      			// Always zero (board, node, processor)
			MERCURY_SET_SOURCE_NODE(ExitEntryPtr, 0);
			MERCURY_SET_SOURCE_BOARD(ExitEntryPtr, 0);
			MERCURY_SET_SOURCE_PROC(ExitEntryPtr, 0);
			MERCURY_SOURCE_BIND2COMP(ExitEntryPtr, BindBlock->bindhandle);
			BindCount += 1;
			BindBlock = BindBlock->Next;
		}
		// Set BindBlock to first Valid value which will be used
		// while setting the message
		Queue = (PMSD_QUEUE)&MsdOpenBlock->bind_block_q;
		BindBlock = (PMSD_BIND_BLOCK)(PMSD_QUEUE)Queue->QueueHead;
	}



	for (i=0; i<MSD_MAX_BOARD_ID_COUNT; i++) {

 	     if (MsdControlBlock->padapter_block_list[i] != NULL)  {
	
		 AdapterBlock = MsdControlBlock->padapter_block_list[i];	

		 ////////////////////////////////////////////////////////////
		 //if ((AdapterBlock->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
		 //{
          if (NormalUnBind) 
					{
		        MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
           }
          else
		     MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
		//}
		//else
		// MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
		 ////////////////////////////////////////////////////////////

		 // Find the matching logical board id number 
		 for(j=0;j<MSD_MAX_BOARD_ID_COUNT; j++) {
		     if(mercd_adapter_log_to_phy_map_table[j] == i )
			 break;
		 }

	         if (j < MSD_MAX_BOARD_ID_COUNT ) { 

		     // Check if the Bind handle was using this Adapter or not ?
		     if ((BindBlock->ExitNotifyBindBoard & 
			 (MSD_EXIT_NOTIFY_BIND_BOUND << j /* AdapterBlock->AdapterNumber */)) ) {
			
			  // validate the pam and board states 
			  if (AdapterBlock->state == MERCD_ADAPTER_STATE_DOWNLOADED) {
				
			      MSD_LEVEL2_DBGPRINT("bnd_snd_death_msg2brd: Found Adapter %d\n", j);

			      // get a new messgae block for MDRV_MSG
			      NewMsg = supp_alloc_buf(sizeof(MDRV_MSG), GFP_ATOMIC);
			      if (NewMsg == NULL) {
				  MSD_ERR_DBGPRINT("bnd_snd_death_msg2brd: mercd_allocate failed\n");
				  return(MD_FAILURE);
			      }

			      //MSD_LEVEL2_DBGPRINT("bnd_snd_death_msg2brd: Dispaching. ptr %lx\n", BindBlock->OpenBlockPtr);

			      // Allocate space for the exit notify message 
			      // Size = (BindCount * EntrySize) + 1 (+1 for Count in Exit Message)
			      DataMsg = supp_alloc_buf((sizeof(MERC_HOSTIF_MSG) +
					((BindCount * sizeof(HIF_PROC_EXIT_ENTRY)) + sizeof(merc_uint_t))), GFP_ATOMIC);

			      if (DataMsg == NULL) {
				  MSD_ERR_DBGPRINT("bnd_snd_death_msg2brd:allocbuf for data failed\n");
				  mercd_streams_free(NewMsg, sizeof(MDRV_MSG), MERCD_FORCE);
				  return(MD_FAILURE);
			      }

			      MD_SET_MSG_TYPE(DataMsg, M_DATA);
			      // link the exit notify message with the MDRV_MSG
			      MSD_LINK_MESSAGE(NewMsg,DataMsg);
 
		 	      MSD_ZERO_MEMORY((pmerc_char_t)DataMsg->b_rptr, (sizeof(MERC_HOSTIF_MSG) +
				     (BindCount * sizeof(HIF_PROC_EXIT_ENTRY)) + sizeof(merc_uint_t))); 

			      MercMsg = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;
			      MERCURY_SET_BODY_SIZE(MercMsg, sizeof(MERC_HOSTIF_MSG) +
				    ((BindCount * sizeof(HIF_PROC_EXIT_ENTRY)) + sizeof(merc_uint_t))); 

			      // create a proc death mercury message
			      // must make sure we have correct header for Proc Death 	
			      Flag = MERCURY_FLAG_32_ALIGNMENT|MERCURY_FLAG_NULL_BUFFER;
			      MERCURY_SET_MESSAGE_FLAG(MercMsg,Flag);
			      MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);
			      MERCURY_SET_MESSAGE_TYPE(MercMsg, QCNTRL_PROC_DEATH);
			      MERCURY_SET_TRANSACTION_ID(MercMsg,AdapterBlock->TransactionId);
			      AdapterBlock->TransactionId++;
			      MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
			      MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
			      MERCURY_SET_DEST_COMPINST(MercMsg,PROC_DEATH_DEST_COMPINST);
			      MERCURY_SET_DEST_BOARD(MercMsg,j /* AdapterBlock->AdapterNumber */);
			      MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);

			      ExitNotifyPtr = (PHIF_PROC_EXIT_NOTIFY)MERCURY_BODY_START(MercMsg);
			      ExitNotifyPtr->Count = BindCount;
			      ExitEntryPtr =  (PHIF_PROC_EXIT_ENTRY)(&(ExitNotifyPtr->Entry));

			      MsdCopyMemory((PCHAR)(&SourceAddressTable[0]), (pmerc_char_t)ExitEntryPtr, 
					      (BindCount * sizeof(HIF_PROC_EXIT_ENTRY)) );

			      MSD_LEVEL2_DBGPRINT("Proc_Death msg sent: Adapter %d\n", j); 
			      
                	      // WW support
    			      // send the msg
               		      if (!(AdapterBlock->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                  		   MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);

                  		   MSD_ENTER_MUTEX(&AdapterBlock->snd_msg_mutex);
                  		 if ((strm_Q_snd_msg(NewMsg, AdapterBlock)) != MD_SUCCESS) {
                         	     MSD_ASSERT(0);
                   		 }
                   		 MSD_EXIT_MUTEX(&AdapterBlock->snd_msg_mutex);

                  		 MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
                         
               		      } else {
                  		 if (AdapterBlock->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {

			          if(NormalUnBind) 
								{
                   MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);
                   MSD_ENTER_MUTEX_IRQ(&AdapterBlock->phw_info->intr_info->intr_mutex);
				         } 
				        else
                  {
               	     MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);
                  }

                  if ((strm_ww_Q_snd_msg(NewMsg, AdapterBlock)) != MD_SUCCESS) {
                         		 MSD_ASSERT(0);
                   }

							 if (NormalUnBind) {
                      MSD_EXIT_MUTEX_IRQ(&AdapterBlock->phw_info->intr_info->intr_mutex);
				              MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
								}
							 else
                {
				        MSD_ENTER_MUTEX(&AdapterBlock->adapter_block_mutex);
                }

                   		 } else {
                     		     cmn_err(CE_WARN, "Adapter out of srvc\n");
                     		     AdapterBlock->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
                   		 }
                 	      }

   			  } /* MERCD_ADAPTER_STATE_DOWNLOADED */
       		     } /* MSD_EXIT_NOTIFY_BIND_BOUND */

	         }

					 ///////////////////////////////////////////////////////////////
	  //if ((AdapterBlock->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY))
     //    						{
								if (NormalUnBind)
									{
										MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);
									}
								 else
										MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);
		//				}
	  //		 else
	  //						MSD_EXIT_MUTEX(&AdapterBlock->adapter_block_mutex);
				  ////////////////////////////////////////////////////////////////
				 
   
             }

	}

	return(MD_SUCCESS);
}
/***************************************************************************
 * Function Name                : bnd_dispatch_close_strm
 * Function Type                : Support function for Bind manager function
 * Inputs                       : BindBlock
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
md_status_t bnd_dispatch_close_strm(pmercd_bind_block_sT BindBlock)
{
        PSTRM_MSG       Msg;
        PMDRV_MSG       MdMsg;
        PSTREAM_CLOSE_INTERNAL  Ptr;
       pmercd_adapter_block_sT           padapter;
        pmercd_stream_connection_sT       StreamBlock;

        StreamBlock = BindBlock->stream_connection_ptr;
        padapter = StreamBlock->padapter_block;


        MSD_FUNCTION_TRACE("bnd_dispatch_close_strm", ONE_PARAMETER, (size_t)BindBlock);

        MSD_ASSERT(BindBlock);

         // allocate a message for our internal use
         Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_CLOSE_INTERNAL), GFP_ATOMIC);
         if(Msg== NULL) {
                MSD_ERR_DBGPRINT("DispatchCloseStream: alloc buffer failed.\n");
                return(MD_FAILURE);
         }

         // now mimic a STREM_CLOSE using the newly allocated message
         MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
         MD_SET_MDMSG_CLASS(MdMsg,MD_CLASS_CORE);
         MD_SET_MDMSG_ID(MdMsg,MID_STREAM_CLOSE_INTERNAL);
         if(BindBlock->stream_connection_ptr==NULL) {
                // System must be coming down
                MSD_ERR_DBGPRINT("DispatchCloseStream:Stream Ptr NULL.\n");
                MSD_FREE_MESSAGE(Msg);
                return(MD_SUCCESS);
         }

	 // LA: Fri Aug 31 14:57:06 EDT 2001
	 // The order for setting the stream handle before setting the bind handle
	 // is imperative for the stream group api change in the libqhost.so
         MD_SET_MDMSG_STREAM_HANDLE(MdMsg,
                ((pmercd_stream_connection_sT) (BindBlock->stream_connection_ptr))->handle);

         MD_SET_MDMSG_BIND_HANDLE(MdMsg,BindBlock->bindhandle);

         // set the message body
         Ptr = (PSTREAM_CLOSE_INTERNAL)MD_GET_MDMSG_PAYLOAD(MdMsg);
         Ptr->StreamBlock = BindBlock->stream_connection_ptr;

         // call the stream manager to process the close
         (*mercd_osal_mid_func[MID_STREAM_CLOSE_INTERNAL])(BindBlock->popen_block,
                                                                            Msg);
        return(MD_SUCCESS);
}
