/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdsupp.c
 * Description			: Common Support functions
 *
 *
 **********************************************************************/

#include "msd.h"
#include "msdpciif.h"
#if defined LiS || defined LIS || defined LFS
#define _MSDSUPP_C
#include "msdextern.h"
#undef _MSDSUPP_C
#else
#define _MSDSUPPNATIVE_C
#include "msdextern.h"
#undef _MSDSUPPNATIVE_C
#endif 

#define EXENAME_SIZE    32 
#ifdef LFS
typedef merc_void_t (*FUNCTION)(long);
#else
typedef merc_void_t (*FUNCTION)(size_t);
#endif

/***************************************************************************
 * Function Name		: supp_alloc_buf
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver AllocateBuf Routine. This 
 *				  routine is used to allocate a buffer.
 * Additional comments		:
 ***************************************************************************/
PSTRM_MSG supp_alloc_buf(size_t Size,int Type)
{
	PSTRM_MSG	Msg;

	MSD_FUNCTION_TRACE("supp_alloc_buf", ONE_PARAMETER, (size_t)Size );

	// Must change message type to M_PROTO as default 
        Msg = allocb(Size,Type);
	if (Msg) {
		MD_SET_MSG_TYPE(Msg, M_PROTO);
		// set the write ptr here so caller doesn't have to 
		MD_SET_MSG_WRITE_PTR(Msg, MD_GET_MSG_READ_PTR(Msg) + Size);
		MSD_ZERO_MEMORY((pmerc_char_t)MD_GET_MSG_READ_PTR(Msg), Size);
	}
	else {
	}
	return(Msg);

}

/***************************************************************************
 * Function Name		: supp_wait4Buf
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver WaitForBuf Routine. This 
 *				  routine is used to intiate a wait till some
 *				  memory become available in host main memroy 
 *				  area. Solaris calls the specified function
 *				  when memory is available. Called when the
 *				  AllocateBuf fails
 * Additional comments		:
 ***************************************************************************/
md_status_t supp_wait4Buf(merc_uint_t Size, merc_int_t (*Func)(pmerc_void_t), size_t Arg)
{

	MSD_FUNCTION_TRACE("supp_wait4Buf", THREE_PARAMETERS, (size_t)Size,
						(size_t)Func, (size_t)Arg);

	// Previous allocb hass failed, Use bufcall to schedule a call to
	// the Func after the buffer is available
#if   defined( __sparcv9 )
	bufcall(Size, BPRI_MED, (FUNCTION)Func, (pmerc_void_t) Arg);
#else
#ifdef LFS
	abstract_bufcall(Size, BPRI_MED, (void *)Func, Arg);
#else
	abstract_bufcall(Size, BPRI_MED, (FUNCTION)Func, Arg);
#endif
#endif
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: supp_rnd2nxt
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver RoundToNext Routine. This
 *				  routine is used to round-off the number to 
 *				  the next index.
 * Additional comments		:
 ***************************************************************************/
merc_uint_t supp_rnd2nxt(merc_uint_t Size, merc_uint_t N)
{
	merc_uint_t Remainder;

	MSD_FUNCTION_TRACE("MsdRoundToNExt", TWO_PARAMETERS, 
			  (size_t)Size, (size_t)N);

	if((Remainder=Size%N) == 0)
		return(Size);
	return(Size+(N-Remainder));
}

/***************************************************************************
 * Function Name		: supp_dispatch_up_Q
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver DispatchUp Routine. This 
 *				  routine handles all the F/W Messages going 
 *				  up to the user. The Stream data messages 
 *				  and Mercury messages come from streams and
 *				  message receive routines repsectively.
 * Additional comments		:
 ***************************************************************************/
md_status_t supp_dispatch_up_Q(pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg)
{
	PMDRV_MSG MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PMERC_HOSTIF_MSG	FwMessage;
	pmercd_open_block_sT	OpenBlockContext;

#ifdef _8_BIT_INSTANCE
	MSD_HANDLE	BindHandle;
#else
        MBD_HANDLE      BindHandle;
#endif

	merc_uchar_t		MsgFlag;

	MSD_FUNCTION_TRACE("supp_dispatch_up_Q", TWO_PARAMETERS, 
				(size_t)AdapterBlock, (size_t)Msg);

	MSD_ASSERT(Msg->b_cont);
	FwMessage = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;

	// read the message flag 
	MsgFlag = MERCURY_GET_MESSAGE_FLAG(FwMessage);

	MD_SET_MDMSG_ID(MdMsg,MID_PAM_RECEIVE_MERCURY);
			
#ifdef _8_BIT_INSTANCE
	BindHandle = (MD_HANDLE)MD_GET_MDMSG_BIND_HANDLE(MdMsg);
#else
        BindHandle = (MBD_HANDLE)MD_GET_MDMSG_BIND_HANDLE(MdMsg);
#endif

	// get bind handle and context 
	OpenBlockContext = (pmerc_void_t)MD_MAP_BIND_TO_OPEN_CONTEXT(BindHandle);

	MD_SET_MDMSG_CLASS(MdMsg,MD_CLASS_PAM);

	 if(BindHandle && MsdControlBlock->pbind_block_list[BindHandle]){
                MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
                MD_SET_MDMSG_USER_CONTEXT(MdMsg,MD_MAP_BIND_TO_USER_CONTEXT(BindHandle));
        }

	// send the message to user
	supp_process_receive(OpenBlockContext,Msg);

	return(MD_SUCCESS);

}

/***************************************************************************
 * Function Name		: supp_cfg_ack
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver ProcConfigAck Routine. 
 *				  This routine receives the Config Ack from
 *				   Kernel and proecsses it.
 * Additional comments		:
 ***************************************************************************/
merc_void_t supp_cfg_ack(pmercd_adapter_block_sT padapter)
{
	pmercury_host_config_info_ack_sT ConfigAck;
    	pmercury_host_info_sT		HostIfPtr;
	mercd_dhal_mem_copy_sT meminfo = { 0 };

	MSD_FUNCTION_TRACE("supp_cfg_ack", ONE_PARAMETER, (size_t)padapter);

	HostIfPtr = &padapter->phost_info->merc_host_info;

	// calculate config ack offset 
	ConfigAck =	(pmercury_host_config_info_ack_sT)
			CONFIG_ACK_ADDRESS(padapter->phost_info->reg_block.HostRamStart);

	// save a copy of config ack in local buffer 
	meminfo.src = (pmerc_char_t)ConfigAck;
	meminfo.dest = (pmerc_char_t)&padapter->phost_info->host_config_ack;
	meminfo.size = sizeof(mercury_host_config_info_ack_sT);
	meminfo.datatype = MSD_INT_DATA;
	meminfo.dir = MSD_TO_HOST;

	(*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((pmerc_void_t )&meminfo);

	ConfigAck = &padapter->phost_info->host_config_ack;


	if(ConfigAck->replyCode != MERC_HOSTIF_CFG_OK) {
		goto out;
	}

	// now setup our freq. used numbers
	HostIfPtr->host_to_fw_msg_start = 
			TO_KV_ADDRESS(padapter,ConfigAck->pHostToFwMsgs);

	HostIfPtr->fw_to_host_msg_start = 
			TO_KV_ADDRESS(padapter,ConfigAck->pFwToHostMsgs);

	HostIfPtr->host_to_fw_data_start = 
			TO_KV_ADDRESS(padapter,ConfigAck->pHostDataXfer);

	HostIfPtr->fw_to_host_data_start = 
			TO_KV_ADDRESS(padapter,ConfigAck->pFwDataXfer);

	// assume msg buffer entry is Mercury msg size 
	HostIfPtr->msg_ring_buffer_size = sizeof(MERC_HOSTIF_MSG);

	HostIfPtr->data_ring_buffer_size = sizeof(MERC_DATA_CIR_BUF_ENTRY);

	HostIfPtr->pfree_list_header_start = 
		(pmercury_free_hdr_list_sT)TO_KV_ADDRESS(padapter,ConfigAck->pFreeNodes);

out:
	padapter->flags.HostifConfigSent &= ~MERC_ADAPTER_FLAG_HOSTIF_CONFIG_SENT;

	// send back a reply to let user know 
	if (supp_snd_cfg_rply(padapter, ConfigAck->replyCode) != MD_SUCCESS) {
		MSD_ERR_DBGPRINT("supp_cfg_ack: SendConfigReply failed.\n");
		return;
	}

	// Kick off the timer for the SRAM board also
        time_chk_snd_timer(padapter);

	return;
}

/***************************************************************************
 * Function Name		: supp_snd_cfg_rply
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver SendConfigReply Routine.
 *				  This routine send the Config reply to the user.
 * Additional comments		:
 ***************************************************************************/
md_status_t supp_snd_cfg_rply(pmercd_adapter_block_sT AdapterBlock, merc_uint_t ReplyCode)
{
	PMDRV_MSG	MdMsg;
	PSTRM_MSG	Msg;
	PPAM_BOARD_START_ACK	Ptr;
#ifndef LFS
	PMERC_BOARD_BLOCK	MercBoardAdapter;
#endif
	pmercd_open_block_sT MsdOpenBlock;

	MSD_FUNCTION_TRACE("supp_snd_cfg_rply", TWO_PARAMETERS, 
				(size_t)AdapterBlock, (size_t)ReplyCode);

	// allocate a fresh message 
	Msg= supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(PAM_BOARD_START_ACK),GFP_ATOMIC); 
	if (Msg == NULL) {
		MSD_ERR_DBGPRINT("supp_snd_cfg_rply: Allocatebuf failed.\n");
		// put in bufcall later
		return(MD_FAILURE);
	}

   	MSD_ZERO_MEMORY((pmerc_char_t)Msg->b_rptr, sizeof(MDRV_MSG)+sizeof(PAM_BOARD_START_ACK));

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	Ptr = (PPAM_BOARD_START_ACK) MD_GET_MDMSG_PAYLOAD(MdMsg);

	Ptr->BoardNumber = AdapterBlock->phw_info->slot_number;
	if(ReplyCode == MERC_HOSTIF_CFG_OK) 
		Ptr->ErrorCode = MD_OK;
	else
		Ptr->ErrorCode = ReplyCode;

	// fill in the essentials 
	MdMsg->MessageClass = MD_CLASS_PAM;
	MdMsg->MessageId = MID_PAM_BOARD_START_ACK;


	// MsdOpen is saved by the original sendign side 

   	MsdOpenBlock=(pmercd_open_block_sT)AdapterBlock->pcfg_open_block;

	// set the adapter state to ready if no error
	if(Ptr->ErrorCode != MD_OK) {
		AdapterBlock->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
                cmn_err(CE_CONT,"supp_snd_cfg_rply : Adapter Out Of Service\n");
           }
	else
		AdapterBlock->state = MERCD_ADAPTER_STATE_STARTED;


	// pass the message to the user
	supp_process_receive(MsdOpenBlock, Msg);

	// AssociatedOpenBlock is used only for Config Reply
	AdapterBlock->pcfg_open_block = NULL;

	return(MD_SUCCESS);
}


/***************************************************************************
 * Function Name		: supp_cfg_map_number
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver ConfigMapNumber Routine. 
 *				  This routine maps boardnumber to a config 
 *				  id and returns the same
 * Additional comments		:
 ***************************************************************************/
merc_int_t supp_cfg_map_number(merc_uint_t BoardNumber)
{
	merc_int_t id;

	MSD_FUNCTION_TRACE("supp_cfg_map_number", 
			ONE_PARAMETER, (size_t)BoardNumber);

	// search the MsdConfigIdMapTable for the mathcing board number 
	for (id = 0; id < MSD_MAX_BOARD_ID_COUNT; id++) {
		if (mercd_adapter_map[id] == (merc_ushort_t)BoardNumber)
			break;
	}
	if (id == MSD_MAX_BOARD_ID_COUNT) 
		id = -1;

	// return the index if match is found, else return -1 
	return(id);
}

/***************************************************************************
 * Function Name		: supp_drvr_mk_ack
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver DriverMakeAck Routine. 
 *				  This routine makes an Ack for user function
 * Additional comments		:
 ***************************************************************************/
PSTRM_MSG
supp_drvr_mk_ack(PSTRM_MSG Msg,merc_uint_t MsgId,merc_uint_t ErrorCode,merc_uint_t ReturnValue)
{
	PSTRM_MSG	NewMsg;
	PMD_ACK		AckPtr;
	PMDRV_MSG	MdMsg;
	merc_int_t	BufSize;

#ifdef _8_BIT_INSTANCE
	MD_HANDLE	BindHandle;
#else
        MBD_HANDLE      BindHandle;
#endif

#if defined LiS || defined LIS || defined LFS
	MSD_FUNCTION_TRACE("MsdDriveMakeAck", FOUR_PARAMETERS, (size_t)Msg,
			(size_t)MsgId, (size_t)ErrorCode, (size_t)ReturnValue);

	// make sure of Msg space for MD_ACK 
	if(MD_IS_MSG_SYNC(Msg)) {
		BufSize = MD_GET_MSG_SIZE(Msg);
	} else {
		BufSize = (pmerc_char_t)MD_GET_MSG_DATABUF_LIMIT(Msg) - 
				(pmerc_char_t)MD_GET_MSG_READ_PTR(Msg);
	}

	// allocate new message if one passed is too small 
	if(BufSize < (sizeof(MDRV_MSG)+sizeof(MD_ACK))) {
		if(MD_IS_MSG_SYNC(Msg)) {
			MSD_ERR_DBGPRINT("supp_drvr_mk_ack: too small msg,ID=%x, size=%x.\n",
					MsgId, BufSize);
			MD_SET_MSG_NACK(Msg);
			return(Msg);
		}

		// allocate a new message 
		NewMsg = (PSTRM_MSG)supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(MD_ACK),GFP_ATOMIC);
		if(NewMsg == NULL) {
			MSD_ERR_DBGPRINT("supp_drvr_mk_ack: allocate msg failed\n");
			return(Msg);
		}

		// Needed ????
		MSD_ZERO_MEMORY((pmerc_char_t)NewMsg->b_rptr, sizeof(MDRV_MSG)+sizeof(MD_ACK));

		// copy the old msg to the new 
		MsdCopyMemory((pmerc_char_t)MD_GET_MSG_READ_PTR(Msg),
			 (pmerc_char_t)MD_GET_MSG_READ_PTR(NewMsg), sizeof(MDRV_MSG));

		// must set type to old msg M_PROTO or M_IOCTL 
		MD_SET_MSG_TYPE(NewMsg, MD_GET_MSG_TYPE(Msg));

		// free old message 
		MSD_FREE_MESSAGE(Msg);
		Msg = NewMsg;
	}
	else {
		// set the message wptr and rptr to reflect MD_ACK 
		Msg->b_wptr = Msg->b_rptr + sizeof(MDRV_MSG)+sizeof(MD_ACK);
	}
#endif /* LiS */

	// setup the MID_ACK message fields 
	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	MD_SET_MDMSG_ID(MdMsg, MID_ACK);
	BindHandle = MD_GET_MDMSG_BIND_HANDLE(MdMsg);

	if(BindHandle>MsdControlBlock->maxbind) {
		MSD_ERR_DBGPRINT("supp_drvr_mk_ack:BindHandle = 0x%x!\n", BindHandle);
		BindHandle = 0;
	}

	// set the user context and other fields in MID_ACK
        if(BindHandle && MsdControlBlock->pbind_block_list[BindHandle]){
                merc_uint_t UserContext;

                UserContext = MD_MAP_BIND_TO_USER_CONTEXT(BindHandle);
                MD_SET_MDMSG_USER_CONTEXT(MdMsg, UserContext);
        }

	AckPtr = (PMD_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
	AckPtr->MessageId = MsgId;;
	AckPtr->ErrorCode = ErrorCode;
	AckPtr->ReturnValue = ReturnValue;
	return(Msg);
}

/***************************************************************************
 * Function Name		: supp_do_unbind
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver DoUnBind Routine. This routine
 *				  does internal unbind for all active binds 
 *				  found when a user process does a close w/o 
 *				  doing unbind, it closes active stream 
 *				  connections associated with this bind and
 *				  deallocates all bind related space.
 * Additional comments		:
 ***************************************************************************/
md_status_t supp_do_unbind(PMSD_BIND_BLOCK BindBlock)
{
	PSTRM_MSG	Msg;
	PMDRV_MSG	MdMsg;

	// WW Changes:
#ifndef LFS
	merc_uint_t  			i = 0;
	merc_uint_t  			AdapterNumber;
	merc_uint_t             	AdapterWithWWW = 0;
	MD_STATUS 			Status = MD_SUCCESS;
	pmercd_adapter_block_sT 	padapter;
	mercd_ww_dmaDscr_Index_sT 	dmaDescSt= {0};
#endif

	MSD_FUNCTION_TRACE("supp_do_unbind", ONE_PARAMETER,(size_t)BindBlock);

	// alloc and create a UNBIND message 
	Msg = (PSTRM_MSG)supp_alloc_buf(sizeof(MDRV_MSG),GFP_ATOMIC);
	if(Msg == NULL) {
		MSD_ERR_DBGPRINT("supp_do_unbind: cannot allocate msg.\n");
		return(MD_FAILURE);
	}

	// Needed ????
	MSD_ZERO_MEMORY((pmerc_char_t)Msg->b_rptr, sizeof(MDRV_MSG));

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

	// create a UNBIND message and send to the router
	MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
	MD_SET_MDMSG_ID(MdMsg, MID_DISPATCHER_UNBIND);
	MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindBlock->bindhandle);
	MD_SET_MDMSG_USER_CONTEXT(MdMsg, BindBlock->UserContext);
	MdMsg->MessageFlags |= MD_MSG_FLAG_NO_ACK;

	// send the msg as if from user.  Lower functions should be
	// shielded from the source 

	BindBlock->flags |= (MSD_BIND_FLAG_EXIT_WOMUTEX | 
			     MERCD_WW_FLAG_FORCED_UNBIND);

	(*mercd_osal_mid_func[MID_DISPATCHER_UNBIND])(BindBlock->popen_block, Msg);

	// from this point on, BindBlock is invalid 

	MSD_LEVEL2_DBGPRINT("supp_do_unbind: Freeing the message.\n");
	// Free the message 
	MSD_FREE_MESSAGE(Msg);
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: supp_sram_start_offset
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver SramStartOffset Routine.
 *				  This routine returns the SRAM start offset.
 * Additional comments		:
 ***************************************************************************/
merc_uint_t supp_sram_start_offset(merc_int_t ConfigId)
{
	pmercd_adapter_block_sT	padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]];

	MSD_FUNCTION_TRACE("supp_sram_start_offset", ONE_PARAMETER, (size_t)ConfigId);

     	if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
	    (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
            return MSD_CONFIG_ROM_MAX_SIZE;
        } else {
	    return(MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->start_offset);	 
        }
}

/***************************************************************************
 * Function Name		: supp_fnd_free_bnd_entry
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver FindFreeBindEntry Routine.
 *				  This routine finds a free entry in the bind 
 *				  table.
 * Additional comments		:
 ***************************************************************************/
#ifdef _8_BIT_INSTANCE
MSD_HANDLE supp_fnd_free_bnd_entry()
#else
MBD_HANDLE supp_fnd_free_bnd_entry()
#endif
{
#ifndef LFS
    int max_bind_search=20;
#endif
#ifdef _8_BIT_INSTANCE
	MSD_HANDLE i;
#else
        MBD_HANDLE i;
#endif 

	MSD_FUNCTION_TRACE("supp_fnd_free_bnd_entry", NO_PARAMETERS);

	// valid bind handle starts at 1 
	for(i = 1; i < MsdControlBlock->maxbind; i++) {
		if(MsdControlBlock->pbind_block_list[i] == NULL  ) {
#if 0
		    int j=MsdControlBlock->BindFreeIndex - 1;
			int k=1;

			  if(j == -1) j = MSD_MAX_BINDTOBE_FREED_INDEX-1;
		
			  do {
				pmercd_bind_block_sT BindBlock;

				// The following allow us not to give last max_bind_search handles to user..
				if(BindBlock = MsdControlBlock->pbind_free_block_list[j]) {
					if(BindBlock->bindhandle == i) break;
				 } else {
					return(i);
				 }

				k++;

			    if( (--j) == -1) 
					j=MSD_MAX_BINDTOBE_FREED_INDEX - 1 ;

			   }while(k < max_bind_search);	
			
			   if(k == max_bind_search) 
			   	 return(i);
#endif
		return(i);
	}
	}
	return(0);
}

/***************************************************************************
 * Function Name		: supp_read_brd_panic_buf
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver supp_read_brd_panic_buf Routine.
 *				  This routine reads the Mercury Kernel Panic Dump
 * Additional comments		:  THIS FUNCTION IS NOT PORTED TO NEW ARCH YET 
 ***************************************************************************/
md_status_t supp_read_brd_panic_buf( pmercd_adapter_block_sT padapter,
                                   pmerc_uchar_t PanicBuffer)
{
	PMERCURY_HOST_IF_CONFIG_ACK  ConfigAck;
	mercd_dhal_mem_copy_sT meminfo = { 0 };
	merc_uchar_t			InUchar;
#ifndef LFS
	merc_uint_t			ConfigId;
#endif
	pmerc_uchar_t 			PanicAddr;


	MSD_FUNCTION_TRACE("supp_read_brd_panic_buf", TWO_PARAMETERS, 
			(size_t)padapter , (size_t)PanicBuffer);

	if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
	    // Request the host ram without asking for interrupt
  	    MERC_GET_SRAM_NO_INTR_LOCK((&padapter->phost_info->reg_block),InUchar);

	    if (!(InUchar & MERC_HOST_RAM_GRANT_R)) {
		printk("PamReadBoardPanicBuffer: does not have host ram!\n");
		return(MD_FAILURE);
	    }

	    // transfer the panic block from the host ram
	    ConfigAck = (PMERCURY_HOST_IF_CONFIG_ACK)
	 		 CONFIG_ACK_ADDRESS(padapter->phost_info->reg_block.HostRamStart);
	    PanicAddr = (merc_uchar_t *)(&(ConfigAck->general[0]));

	    CHECK_SRAM_ADDRESS(PanicAddr, "supp_read_brd_panic_buf", padapter);
	} else {
	    PanicAddr = padapter->phost_info->reg_block.HostRamStart+MSD_PCI_THIRD_ROCK_CRASH_DUMP_LOC;
	}

	MSD_LEVEL2_DBGPRINT("Read from panic addr 0x%x\n", PanicAddr);

	meminfo.src = (pmerc_char_t)PanicAddr;
	meminfo.dest = (pmerc_char_t)PanicBuffer;
	meminfo.size = MD_PANIC_DUMP_MAX_SIZE;
	meminfo.datatype = MSD_INT_DATA;
	meminfo.dir = MSD_TO_HOST;

	(*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((pmerc_void_t )&meminfo);

	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: supp_flush_stream_Q
 * Function Type		: Common Support Functions 
 * Inputs			: q - STREAMS queue
 *				 mp - The current message
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver FlushStreamQ Routine. 
 *				  Canonical Flush handling
 * Additional comments		:
 ***************************************************************************/
MD_PRIVATE merc_void_t supp_flush_stream_Q(queue_t *q, mblk_t *mp)
{
	MSD_FUNCTION_TRACE("supp_flush_stream_Q", TWO_PARAMETERS, (size_t)(q), 
								(size_t)(mp));

	// Check to see if Write queue should be flushed 
	if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
		abstract_flushq(q, FLUSHALL);
	}

	// Check to see if Read queue should be flushed 
	if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {
		abstract_flushq(RD(q), FLUSHALL);
		// We must insure that this message is sent to all modules on the
		// stream. We need only worry about the upstream bound (Read queue) 
		// messages. First we clear the write bit (So it isn't sent 
		// back downstream by stream head) then send it upstream with qreply()
		*mp->b_rptr |= FLUSHR;  // Set read queue flush 
		*mp->b_rptr &= ~FLUSHW; // Clear write queue flush 
		abstract_putq(RD(q), mp);
	}
}

/***************************************************************************
 * Function Name		: supp_process_receive
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver ProcessReceive Routine. For
 *				  getmsg() call from user, send the msg upstream
 * Additional comments		:
 ***************************************************************************/
merc_void_t supp_process_receive(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
	MSD_FUNCTION_TRACE("supp_process_receive", TWO_PARAMETERS, 
				(size_t)(MsdOpenBlock), (size_t)(Msg));

	if(MsdOpenBlock == NULL)  {
                MSD_FREE_MESSAGE(Msg);
                return;
         }

#ifdef LFS
	 if((ulong)MsdOpenBlock < 0xFFFF) {
                MSD_FREE_MESSAGE(Msg);
                return;
         }
#else
	 if(MsdOpenBlock < 0xFFFF) {
                MSD_FREE_MESSAGE(Msg);
                return;
         }
#endif

	 if(MsdOpenBlock->state == MERCD_OPEN_STATE_CLOSE_PEND) {
	   MSD_FREE_MESSAGE(Msg);
	   return;
	 }

#if defined LiS || defined LIS || defined LFS
	if (MsdOpenBlock->up_stream_write_q)
	    putq(RD(MsdOpenBlock->up_stream_write_q), Msg);
	else cmn_err(CE_WARN, "Write queue is NULL\n");
#else
        //The following change is required for SMP
	MSD_ENTER_MUTEX(&MsdOpenBlock->readers_queue_mutex);
	queue_put_msg_Array(&MsdOpenBlock->readersArray, Msg, MSD_READER);
	MSD_EXIT_MUTEX(&MsdOpenBlock->readers_queue_mutex);

	WAKEUP_WAITQUEUE(MsdOpenBlock->readers);
#endif /* LiS */
}

/***************************************************************************
 * Function Name		: supp_process_sync_receive
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			:  Streams Driver ProcessSyncReceive Routine.
 *				   For SYNC upstream msgs (mainly replies of
 *				   IOCTL commands)
 * Additional comments		:
 ***************************************************************************/
merc_void_t supp_process_sync_receive(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg)
{
	struct iocblk *iocp; 
	PSTRM_MSG	IoctlMsg;
	PMDRV_MSG	MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

	MSD_FUNCTION_TRACE("supp_process_sync_receive", TWO_PARAMETERS, 
				(size_t)(MsdOpenBlock), (size_t)(Msg));

         Msg->b_datap->db_base = NULL;
	if(MsdOpenBlock == NULL)  {
                MSD_FREE_MESSAGE(Msg);
                return;
         }

#ifdef LFS
	 if((ulong)MsdOpenBlock < 0xFFFF) {
                MSD_FREE_MESSAGE(Msg);
                return;
         }
#else
	 if(MsdOpenBlock < 0xFFFF) {
                MSD_FREE_MESSAGE(Msg);
                return;
         }
#endif

	 if(MsdOpenBlock->state == MERCD_OPEN_STATE_CLOSE_PEND) {
	   MSD_FREE_MESSAGE(Msg);
	   return;
	 }

 	 MSD_FREE_MESSAGE(Msg);
#if defined LiS || defined LIS || defined LFS
	MSD_ASSERT(MsdOpenBlock);
	MSD_ASSERT(MsdOpenBlock->up_stream_write_q);
	MSD_ASSERT(!(MD_IS_MSG_ASYNC(Msg)));
	MSD_ASSERT(MdMsg->MessagePtr);

	// retrieve the original ioctl msg header from the Ulong1 member 
	IoctlMsg = (PSTRM_MSG)MdMsg->MessagePtr;

	// set to ioc ack
	IoctlMsg->b_datap->db_type = M_IOCACK;

	// relink the ioctl msg with driver msg
	IoctlMsg->b_cont = Msg;
	MdMsg->MessagePtr=0;

	// fill in the iocblk 
	iocp = (struct iocblk *)IoctlMsg->b_rptr;
	iocp->ioc_rval = 0;
	iocp->ioc_error = 0;

	if (MD_IS_MSG_NACK(Msg)) {
     		IoctlMsg->b_datap->db_type = M_IOCNAK;
	     	iocp->ioc_error = EINVAL;
     		iocp->ioc_count = 0;
	} else {
		// count assumes only one drivermsg block 
		// need to set all driver msg blocks to M_DATA 
		Msg->b_datap->db_type = M_DATA;
		iocp->ioc_count = Msg->b_wptr-Msg->b_rptr;
	}
	if (MsdOpenBlock->up_stream_write_q)
	    putq(RD(MsdOpenBlock->up_stream_write_q), IoctlMsg);
	else cmn_err(CE_WARN, "Write queue is NULL\n");
#endif /* LiS */
}

/***************************************************************************
 * Function Name		: supp_ver2string
 * Function Type		: Common Support Functions 
 * Inputs			: pInString - An Dialogic Version string
 *				: option    - VER_VERSION - standard version number
 *					      VER_WITHBUILD - build number included
 *					      VER_BUILDONLY - build number only
 * 					      VER_EXENAME - return only executable
 *							     name
 *					      VER_COMMENT - return comment string
 * Outputs			: pOutString - Buffer to place ASCII printable 
 *				               version string
 * Calling functions		:
 * Description			:  Converts a Dialogic version string to a
 *				   printable ASCII string with the correct
 *				   version number in it
 * Additional comments		:
 ***************************************************************************/
merc_int_t supp_ver2string (pmerc_char_t pInString, merc_int_t option, pmerc_char_t pOutString)
{
	char  type;
	char  *pTmp;
	char  major[5];
	char  beta_s[3];
	int   beta=0;
	char  alpha[3];
	char  build[6];  // Build is limited to 5 digits, more than enough
	int   cnt;
	char  exename[EXENAME_SIZE];
	char  *comment;
   
	MSD_FUNCTION_TRACE("supp_ver2string", THREE_PARAMETERS,
				(size_t)pInString,
				(size_t)option, (size_t)pOutString); 
	   
	// First check validity of string 
	if (!(strncmp(pInString,KEYWORD,sizeof(KEYWORD)))){
		return -1;
	}

	// get Version type 
	type = pInString[FIELD_TYPE];

	// Build major-minor number pair string 
	major[0] = pInString[FIELD_MAJOR];
	major[1] = '.';
	major[2] = pInString[FIELD_MINOR_DIG1];
	major[3] = pInString[FIELD_MINOR_DIG2];
	major[4] = '\0';
   
	// Build Beta number string and integer equivalent 
	beta_s[0] = pInString[FIELD_BETA_DIG1];
	beta_s[1] = pInString[FIELD_BETA_DIG2];
	beta_s[2] = 0;

	// Convert to integer so printf will remove leading 0 for me 
	beta = beta_s[0] - '0';
	beta *= 10;
	beta += (beta_s[1] - '0');
	
	// Build Alpha number string
	alpha[0] = pInString[FIELD_ALPHA_DIG1];
	alpha[1] = pInString[FIELD_ALPHA_DIG2];
	alpha[2] = 0;

	// Build the build number string - var length NULL terminated
	// or space terminated 
	pTmp = &(pInString[FIELD_BUILD_START]);
	cnt = 0;
	while ((*pTmp != 0) && (*pTmp != ' ')){
		build[cnt++] = *pTmp++;
	}

	// Terminate string 
	build[cnt] = 0;

	// Check if we reached end of string, if not grab rest of info 
	if (pTmp != 0) {
		// Executable name starts directly after build number 
		cnt = 0;
		// Skip space delimiter 
		pTmp++;
		while ((*pTmp != 0) && (*pTmp != ' ') && (cnt < EXENAME_SIZE)){
			exename[cnt++] = *pTmp++;
		}

		// Terminate string 
		exename[cnt] = 0;
      
		// Optional comment is directly after executable name 
		comment = pTmp;
	} else {
		exename[0]=0;
		comment = 0;
	}
   
	// These options only returns a piece 
	switch (option) {
	case VER_BUILDONLY:
		strcat(pOutString,"Build: ");
		strcat(pOutString,build);

		/* sprintf(pOutString,"Build: %s",build); */
		return 0;
	case VER_EXENAME:
		strcat(pOutString,exename);
		/* sprintf(pOutString,"%s",exename); */
		return 0;
	case VER_COMMENT:
		strcat(pOutString,comment);
		/* sprintf(pOutString,"%s",comment);          */
		return 0;
	}
   
	// All other options need at least the main part of the version string 
	switch(type){
	case TYPE_PRODUCTION:
		strcat(pOutString,major);
		/* sprintf(pOutString,"%s ",major);*/
		break;
	case TYPE_BETA:
		strcat(pOutString,major);
		strcat(pOutString," Beta ");
		strcat(pOutString,  beta_s);
		/* sprintf(pOutString,"%s Beta %d",major,beta);*/
		break;
	case TYPE_ALPHA:
		strcat(pOutString,major);
		strcat(pOutString," Alpha ");
		strcat(pOutString,  beta_s);
		strcat(pOutString,".");
		strcat(pOutString,alpha);

	/*	sprintf(pOutString,"%s Alpha %d.%s",major,beta,alpha);*/
		break;
	case TYPE_EXP:
		strcat(pOutString,major);
		strcat(pOutString," Exp ");
		strcat(pOutString,  beta_s);
		strcat(pOutString,".");
		strcat(pOutString,alpha);
		/*sprintf(pOutString,"%s Exp %d.%s",major,beta,alpha);*/
		break;
	case TYPE_SPEC:
		strcat(pOutString,major);
		strcat(pOutString," Spec ");
		strcat(pOutString,  beta_s);
		/* sprintf(pOutString,"%s Spec %d",major,beta);*/
		break;
	}

	// Tack on build number if required
	if (option == VER_WITHBUILD){
		cnt = strlen(pOutString);
		pTmp=pOutString+cnt;
		strcat(pTmp," Build: ");
		strcat(pTmp,build);
		/* sprintf(pTmp," Build: %s",build); */
	}

	return 0;
}



/***************************************************************************
 * Function Name		: supp_push_trace_msg
 * Function Type		: Common Support Functions 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver pushTraceMessage Routine.
 *				  This routine pushes a messages on the 
 *				  Driver trace buffer
 ***************************************************************************/
merc_void_t 
supp_push_trace_msg(pmercd_adapter_block_sT padapter, merc_uint_t Flag, PMERC_HOSTIF_MSG fromPtr)
{
	pmercd_trace_copy_sT toPtr;
	merc_int_t   index;
#ifndef LFS
	clock_t  tmp_time ;
	struct timespec timediff;
	merc_ulong_t  ElapsedTime;
	static int first = 0;
#endif
        struct timeval tv;
#ifndef LFS
	char *data;
#endif
	merc_uint_t strId=MERC_INVALID_STREAMID;
	merc_uint_t bodySize = MERCURY_GET_BODY_SIZE(fromPtr);
	pmercd_trace_info_sT ptrace = padapter->ptrace;

#ifdef LFS
	merc_uint_t szCopy = sizeof(mercd_trace_copy_sT);
#else
	pmerc_uint_t usptb2d, usptd2b;
	merc_uint_t st,szCopy;
#endif

	MSD_FUNCTION_TRACE("supp_push_trace_msg", THREE_PARAMETERS, (size_t)padapter,
						(size_t)Flag, (size_t)fromPtr);

	if (!isMessageToBeTracedOnStreamId(padapter, fromPtr, Flag)) 
	  return;

	if (!isMessageToBeTracedOnMsgType(padapter, fromPtr, Flag)) 
	  return;

	if (!isMessageToBeTracedOnCompDesc(padapter, fromPtr, Flag)) 
	  return;

#if 0
	if (MERCURY_GET_MESSAGE_TYPE(fromPtr) >= 0x100) {
	  pmerc_uint_t pp = (pmerc_uint_t)(fromPtr + 1);
	  int kk;
	  for (kk=0; kk<(bodySize/4); kk++)
	    cmn_err(CE_CONT,"(%d) : body[%d] = %d\n",bodySize,kk,*(pp+kk));
	}
#endif

	// szCopy is not used if ptrace->MsgDataTrace is zero
	if (ptrace->MsgDataTrace)
	  szCopy = sizeof(mercd_trace_copy_sT) + ptrace->CurrentDataSizeLimit;

	// Copy message to the buffer. Use the Index as the pointer within
	// the buffer, but first check the wrap condition on the Index.
	if (Flag == MSD_DRV2BRD_TRACE) {
	  if (ptrace->Drv2BrdTraceIndex == -1) {
	    return;
	  }
	  index = ptrace->Drv2BrdTraceIndex;

	  if (ptrace->MsgDataTrace) 
	    toPtr = (pmercd_trace_copy_sT )((pmerc_char_t)(ptrace->Drv2BrdTraceBuf) + index * szCopy);
	  else
	    toPtr = (pmercd_trace_copy_sT )(&(ptrace->Drv2BrdTraceBuf[index]));
	} else {
	  if (ptrace->Brd2DrvTraceIndex == -1){
	    return;
	  }
	  index = ptrace->Brd2DrvTraceIndex;

	  if (ptrace->MsgDataTrace) 
	    toPtr = (pmercd_trace_copy_sT )((pmerc_char_t)(ptrace->Brd2DrvTraceBuf) + index * szCopy);
	  else
	    toPtr = (pmercd_trace_copy_sT )(&(ptrace->Brd2DrvTraceBuf[index]));

	}

	MsdCopyMemory((pmerc_char_t)fromPtr, (pmerc_char_t)toPtr, sizeof(MERC_HOSTIF_MSG));
#if 0
	toPtr->FlagTransactionId = fromPtr->FlagTransactionId;
	toPtr->MessageType = fromPtr->MessageType;
	toPtr->Source = fromPtr->Source;
	toPtr->Destination = fromPtr->Destination;
	toPtr->BodySize = fromPtr->BodySize;
	toPtr->SourceDestComponent = fromPtr->SourceDestComponent;
#endif
	if (bodySize > 0)
	  strId = GetStreamId(fromPtr);

	toPtr->Reserved = strId;

	getNewCurTime(&tv);
	toPtr->TimeStamp = tv.tv_sec;
        toPtr->TimeUSec = tv.tv_usec;

#if 0
	if((Flag == MSD_BRD2DRV_TRACE)) 
	   cmn_err(CE_CONT,"%s:  Type = %12x  Size= %6x  StrId=%6x \n",
           	"BRD2DRV",  toPtr->MessageType,  toPtr->BodySize, toPtr->Reserved );
        else
	   cmn_err(CE_CONT,"%s:  Type = %12x  Size= %6x  StrId=%6x \n",
           	"DRV2BRD",  toPtr->MessageType,  toPtr->BodySize, toPtr->Reserved );
#endif


	if (ptrace->MsgDataTrace) {
	  pmerc_char_t dataToPtr = (pmerc_char_t) ((pmerc_char_t)(toPtr) + sizeof(mercd_trace_copy_sT));
	  pmerc_char_t dataFromPtr = (pmerc_char_t) MERCURY_BODY_START(fromPtr);

	  if (bodySize <= ptrace->CurrentDataSizeLimit) {
	    //	    cmn_err(CE_CONT, "msdsupp ::: copy msgdata dataToptr = %x,dataFromPtr = %x, toPtr = %x, fromPtr = %x  bodysize = %d, limit =%d\n", dataToPtr,dataFromPtr, toPtr, fromPtr,  bodySize,ptrace->CurrentDataSizeLimit );
	    MsdCopyMemory(dataFromPtr, dataToPtr, bodySize); 
	  }
	  
	}
	
	// Increament the pointer
	if (Flag == MSD_DRV2BRD_TRACE) {
	  ptrace->Drv2BrdTotal++;
	  ptrace->Drv2BrdTraceIndex++;
	  ptrace->Drv2BrdTraceIndex = (ptrace->Drv2BrdTraceIndex == ptrace->Drv2BrdMessageCnt) ?  0 : ptrace->Drv2BrdTraceIndex;
	} else {
	  ptrace->Brd2DrvTotal++;
	  ptrace->Brd2DrvTraceIndex++;
	  ptrace->Brd2DrvTraceIndex = (ptrace->Brd2DrvTraceIndex == ptrace->Brd2DrvMessageCnt) ? 0 : ptrace->Brd2DrvTraceIndex;
	}
	
	if (ptrace->bSelect == MSGS_TRACE_SELECT)
	  SendTraceToApp(padapter, ptrace->dumpThreshold);

#if 0
	usptb2d =  ptrace->Brd2DrvTraceBuf;
	usptd2b =  ptrace->Drv2BrdTraceBuf;
	szCopy = ptrace->CurrentDataSizeLimit + sizeof(mercd_trace_copy_sT);
	for (st =0; st < (ptrace->Brd2DrvTraceIndex *szCopy)/4; st++) {
	  cmn_err(CE_CONT, "Brd2Drv : Value[ %d ]=%d \n", st, *(usptb2d + st));
	}

	for (st =0; st < ptrace->Drv2BrdTraceIndex *szCopy; st++) {
	  cmn_err(CE_CONT, "Drv2Brd : Value[ %d ]=%d \n", st, *(usptd2b + st));
	}
#endif
}

/***************************************************************************
 * Function Name		: SendTraceToApp
 * Function Type		: Sends the trace information upstream
 * Inputs			: 
 *                              : padapter -> the adapter structure pointer
 *                              : thresh   -> this is the threshold which 
 *                              : decides whether to send
 *                              : the trace to the app or not
 *                              : When called from supp_push_trace_msg 
 *                              : its value will be 
 *                              : padapter->ptrace->dumpThreshold
 *                              : When caleld from disable trace, its value is 0
 *                              : so that all the remaining messages are printed.
 * Outputs			: 
 * Calling functions		:
 * Description			: This function just calls MakeTraceMsg
 *                              : and gets a message which is returned
 *                              : upstream
 ***************************************************************************/
void  SendTraceToApp(pmercd_adapter_block_sT padapter, merc_uint_t thresh) {

  PSTRM_MSG trMsg =  (PSTRM_MSG)MakeTraceMsg(padapter, thresh);
  
  if (trMsg == NULL) { /* Threshold not reached */
    //	  cmn_err(CE_CONT,"SendTraceToApp : MakeTraceMsg returns null \n");
    return;
  }
  
#ifdef DEBUG_SELECT
  cmn_err(CE_CONT, "Just before sending from SendMsgsToApp\n");
#endif
  
  if (padapter->ptrace->openblk != NULL)
    supp_process_receive(padapter->ptrace->openblk, trMsg);

#ifdef DEBUG_SELECT
  cmn_err(CE_CONT, "Just After sending from SendMsgsToApp\n");
#endif

  return;

}

/***************************************************************************
 * Function Name		: MakeTraceMsg
 * Function Type		: Creates a trace msg to send up
 * Inputs			: 
 *                              : padapter -> the adapter structure pointer
 *                              : thresh   -> this is the threshold which 
 *                              : decides whether to send
 *                              : the trace to the app or not
 *                              : When called from supp_push_trace_msg 
 *                              : its value will be 
 *                              : padapter->ptrace->dumpThreshold
 *                              : When caleld from disable trace, its value is 0
 *                              : so that all the remaining messages are printed.
 * Outputs			: 
 * Calling functions		:
 * Description			: 
 ***************************************************************************/
PSTRM_MSG  MakeTraceMsg(pmercd_adapter_block_sT padapter, merc_uint_t thresh) {
  merc_uint_t ErrorCode = MD_OK;
  merc_uint_t ReturnValue = 0,copied;
  PSTRM_MSG   NewMsg;
  PMDRV_MSG   NewMdMsg;
  PSTRM_MSG   NewDataMsg;
  PCFG_SELECT_TRACE_REPLY ptr;
  pmercd_trace_info_sT trInfo = padapter->ptrace;
  merc_uint_t szCopy = sizeof(mercd_trace_copy_sT);

#if 0
  pmerc_uint_t usptb2d, usptd2b;
  merc_uint_t st;
#endif

  merc_uint_t b2dPrIndex = trInfo->Brd2DrvPrintIndex;
  merc_uint_t b2dTrIndex = trInfo->Brd2DrvTraceIndex;
#ifndef LFS
  merc_uint_t b2dTotal = trInfo->Brd2DrvTotal;
#endif


  merc_uint_t d2bPrIndex = trInfo->Drv2BrdPrintIndex;
  merc_uint_t d2bTrIndex = trInfo->Drv2BrdTraceIndex;
#ifndef LFS
  merc_uint_t d2bTotal = trInfo->Drv2BrdTotal;
#endif

  merc_uint_t b2dMsgCnt = trInfo->Brd2DrvMessageCnt;
  merc_uint_t d2bMsgCnt = trInfo->Drv2BrdMessageCnt;

  merc_uint_t d2bTail=0, b2dTail=0, d2bHead=0, b2dHead=0;
  merc_uint_t d2bWrap = 0, b2dWrap = 0;
  merc_int_t d2bCnt=0, b2dCnt=0;
#ifndef LFS
  merc_uint_t i=0;
#endif

  merc_uint_t DataFieldSize;
  pmerc_char_t	BrdToDrvDataFieldPtr,DrvToBrdDataFieldPtr;

  if (trInfo->MsgDataTrace)
    szCopy += trInfo->CurrentDataSizeLimit;

  d2bCnt = d2bTrIndex - d2bPrIndex;
  if (d2bCnt < 0) { // wrap around
    d2bHead =  d2bTrIndex;
    d2bCnt = d2bMsgCnt + d2bCnt;
    d2bWrap = 1;
    d2bTail =  d2bMsgCnt  - d2bPrIndex;
  }

  #ifdef DEBUG_SELECT
  cmn_err(CE_CONT, "tr->Drv2BrdTrIndex %d tr->Drv2BrdPrIndex %d                    d2bCnt = %d, szCopy = %d\n",
	  trInfo->Drv2BrdTraceIndex , trInfo->Drv2BrdPrintIndex, d2bCnt, szCopy);
  #endif

  b2dCnt = b2dTrIndex - b2dPrIndex; 
  if (b2dCnt < 0) { // wrap around
    b2dHead =  b2dTrIndex;
    b2dCnt = b2dMsgCnt  + b2dCnt;
    b2dWrap = 1;
    b2dTail =  b2dMsgCnt - b2dPrIndex; 
  }
  
  #ifdef DEBUG_SELECT
  cmn_err(CE_CONT,"tr->Brd2DrvTrIndex %d tr->Brd2DrvPrIndex %d b2dTot %d d2bTot %d b2dCnt = %d\n",
	  b2dTrIndex, b2dPrIndex, b2dTotal, d2bTotal, b2dCnt);
  #endif

  //  if ((d2bCnt < thresh) && (b2dCnt < thresh)) {
  if ((d2bCnt + b2dCnt) < thresh) { 
    #ifdef DEBUG_SELECT
    cmn_err(CE_CONT,"Returnning since threshold is higher %d\n", thresh);
    #endif
    goto out;
  } else {
    #ifdef DEBUG_SELECT
    cmn_err(CE_CONT, "Going Ahead \n");
    #endif
  }

#ifdef DEBUG_SELECT
  cmn_err(CE_CONT, "I AM IN SENDTRACE\n");
#endif

  NewMsg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
  if(NewMsg == NULL) {
    MSD_ERR_DBGPRINT("SendTraceToApp : Cannot alloc buffer\n");
    ErrorCode = CD_ERR_NO_MEMORY;
    ReturnValue = 0;
    goto out;
  }
  MSD_ZERO_MEMORY((pmerc_char_t)NewMsg->b_rptr,
		  sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ);

  DataFieldSize = (d2bCnt + b2dCnt) * szCopy;

  // Add CFG_SELECT_TRACE_REPLY size to the header portion
  NewDataMsg = supp_alloc_buf(sizeof(CFG_SELECT_TRACE_REPLY) + DataFieldSize,GFP_ATOMIC);

  if(NewDataMsg == NULL) {
    MSD_ERR_DBGPRINT("SendTraceToApp: NewDataMsg : Cannot alloc Data buffer\n");
    MSD_FREE_MESSAGE(NewMsg);
    ErrorCode = CD_ERR_NO_MEMORY;
    ReturnValue = 0;
    goto out;
  }
  
  MD_SET_MSG_TYPE(NewDataMsg, M_DATA);
  MSD_LINK_MESSAGE(NewMsg,NewDataMsg);
  
  NewMdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(NewMsg);
  ptr = (PCFG_SELECT_TRACE_REPLY)(NewDataMsg->b_rptr);
  
  ptr->Drv2BrdTraceIndex = 0;
  ptr->Brd2DrvTraceIndex = 0;
  ptr->Drv2BrdAllocated = d2bCnt;
  ptr->Brd2DrvAllocated = b2dCnt;

  BrdToDrvDataFieldPtr = (pmerc_char_t)(NewDataMsg->b_rptr) + sizeof(CFG_SELECT_TRACE_REPLY);

  DrvToBrdDataFieldPtr = BrdToDrvDataFieldPtr + (b2dCnt * szCopy); 
  
  if ( (b2dCnt > 0) && (b2dWrap == 0)) {

    MsdCopyMemory((pmerc_char_t)(trInfo->Brd2DrvTraceBuf) + b2dPrIndex * szCopy,
		  BrdToDrvDataFieldPtr,
		  //		(pmerc_char_t)(&(ptr->Brd2DrvTraceBuff[0])), 
		  b2dCnt * szCopy);
  }

   if ((d2bCnt > 0) && (d2bWrap == 0)) {
     MsdCopyMemory((pmerc_char_t)(trInfo->Drv2BrdTraceBuf) +  d2bPrIndex * szCopy,
		   DrvToBrdDataFieldPtr,
		   //		(pmerc_char_t)(&(ptr->Drv2BrdTraceBuff[0])), 
		   d2bCnt * szCopy);
   }

 if ( (b2dCnt > 0) && (b2dWrap == 1)) {

    MsdCopyMemory((pmerc_char_t)(trInfo->Brd2DrvTraceBuf) + b2dPrIndex * szCopy, 
		  BrdToDrvDataFieldPtr, 
		  b2dTail * szCopy);	
    
    MsdCopyMemory((pmerc_char_t)(trInfo->Brd2DrvTraceBuf), 
		  BrdToDrvDataFieldPtr + (b2dTail * szCopy), 
		  b2dTrIndex * szCopy);	
  }

 if ((d2bCnt > 0) && (d2bWrap == 1)) {
   MsdCopyMemory((pmerc_char_t)(trInfo->Drv2BrdTraceBuf) + d2bPrIndex * szCopy, 
		  DrvToBrdDataFieldPtr, 
		  d2bTail * szCopy);
    MsdCopyMemory((pmerc_char_t)(trInfo->Drv2BrdTraceBuf), 
		  DrvToBrdDataFieldPtr + (d2bTail * szCopy), 
		  d2bTrIndex * szCopy);	
  }

  ptr->Drv2BrdTraceIndex = 0;
  ptr->Brd2DrvTraceIndex = 0;
  ptr->Drv2BrdAllocated = d2bCnt;
  ptr->Brd2DrvAllocated = b2dCnt;

  if ((b2dPrIndex + b2dCnt) >= b2dMsgCnt)
    trInfo->Brd2DrvPrintIndex = (b2dPrIndex + b2dCnt)  - b2dMsgCnt;
  else
    trInfo->Brd2DrvPrintIndex = b2dPrIndex + b2dCnt;

  if ((d2bPrIndex + d2bCnt) >= d2bMsgCnt)
    trInfo->Drv2BrdPrintIndex = (d2bPrIndex + d2bCnt)  - d2bMsgCnt;
  else
    trInfo->Drv2BrdPrintIndex = d2bPrIndex + d2bCnt;
  
  copied = NewDataMsg->b_wptr - NewDataMsg->b_rptr;

#ifdef DEBUG_SELECT
  cmn_err(CE_CONT,"msdsupp.c: MTD ::::: copied=%d, (%d, %d, %d, %d, %d, %d, %d)\n",
	  copied, d2bCnt, b2dCnt, (d2bCnt + b2dCnt) * sizeof(mercd_trace_copy_sT), 
	  trInfo->Drv2BrdTraceIndex, trInfo->Drv2BrdPrintIndex, trInfo->Brd2DrvTraceIndex, 
	  trInfo->Brd2DrvPrintIndex);
#endif
  
  // Send ack from here.
  // just change the msg id and send the msg back 
  NewMdMsg->MessageClass = MD_CLASS_PAM;
  NewMdMsg->MessageId = MID_SELECT_TRACE_ACK;

#if 0
	usptb2d =  trInfo->Brd2DrvTraceBuf;
	usptd2b =  trInfo->Drv2BrdTraceBuf;
	szCopy = trInfo->CurrentDataSizeLimit + sizeof(mercd_trace_copy_sT);
	//	for (st =0; st < (trInfo->Brd2DrvTraceIndex *szCopy)/4; st++) {
	for (st =0; st < (trInfo->Brd2DrvTraceIndex *szCopy)/4; st++) {
	  cmn_err(CE_CONT, "Brd2Drv : Value[ %d ]=%d \n", st, *(usptb2d + st));
	}

	for (st =0; st < (trInfo->Drv2BrdTraceIndex *szCopy)/4; st++) {
	  cmn_err(CE_CONT, "Drv2Brd : Value[ %d ]=%d \n", st, *(usptd2b + st));
	}
#endif  

  return NewMsg;

  out : return NULL;
}

#if defined LiS || defined LIS || defined LFS
//WW support
/***************************************************************************
 * Function Name                : supp_esb_free
 * Function Type                : Common Support Functions
 * Inputs                       : pesbuffer given to a call to esballoc
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Extended Streams Buf Free Routine.
 *                              : This routine called by freeb() while freeing an
 *                              : extended user buffer
 * Additional comments          :
 ***************************************************************************/
#ifdef LFS
void streamscall supp_esb_free(caddr_t pesbuffer)
#else
void supp_esb_free(pmerc_uchar_t pesbuffer)
#endif
{
 
  MSD_FUNCTION_TRACE("supp_esb_free", ONE_PARAMETER,
                       (size_t) pesbuffer);
  if (pesbuffer)
      abstract_free_pages(pesbuffer);
}

#else

/***************************************************************************
 * Function Name                : mercd_readNativeQueue
 * Function Type                : Common Support Functions
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Additional comments          :
 ***************************************************************************/
int mercd_readQueue(pmercd_open_block_sT MsdOpenBlock, size_t size,  merc_char_t *buffer, int dontBlock){
    int retValue = 0;
    ALLOCATE_LOCAL_WAITQUEUE;

    if (MsdOpenBlock->readersArray.head == MsdOpenBlock->readersArray.tail) {
        while(1){
           ADD_TO_WAITQUEUE(MsdOpenBlock->readers);
           if (MsdOpenBlock->readersArray.head != MsdOpenBlock->readersArray.tail){
               REMOVE_FROM_WAITQUEUE(MsdOpenBlock->readers);
               if (MsdOpenBlock->readersArray.head != MsdOpenBlock->readersArray.tail){
                   break;
               }
               ADD_TO_WAITQUEUE(MsdOpenBlock->readers);
           }

           // Queue is empty go to sleep or return.
           if (dontBlock){
               REMOVE_FROM_WAITQUEUE(MsdOpenBlock->readers);
               return -EAGAIN;
           }

           SLEEP_ON_WAITQUEUE(MsdOpenBlock->readers);
           REMOVE_FROM_WAITQUEUE(MsdOpenBlock->readers);

           if (signal_pending(current)){
               return -EINTR; /* a signal arrived */
           }
        }
    } 


    MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
    MSD_ENTER_MUTEX(&MsdOpenBlock->readers_queue_mutex);
    retValue = mercd_copyfromBuffers((PMSD_QUEUE)&MsdOpenBlock->readersArray, 
                                     (char *) MsdOpenBlock->kernelBuf, size);
    MSD_EXIT_MUTEX(&MsdOpenBlock->readers_queue_mutex);
    MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();

    if (!retValue)
        retValue = -EFAULT;

    // since we were in bh level we could not acces user buffer, so ... 
    copy_to_user(buffer,MsdOpenBlock->kernelBuf, size);

    return(retValue);
}


/***********************************************************************
 * Function Name                : mercd_copyfromBuffers
 * Function Type                : Common Support Functions
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Additional comments          :
 ***********************************************************************/
int mercd_copyfromBuffers(PMSD_QUEUE Queue, char *buffer, size_t size)
{
   PSTRM_MSG            Msg;
   merc_ulong_t         MsgBodySize;
   MSD_ARRAY            *Array = (MSD_ARRAY *)Queue;

   // get a message from the send queue
   // If nothing is available, simply get out
   if ((Msg = queue_get_msg_Array(Array, MSD_READER)) == NULL || 
                   !(buffer) || !(Msg->b_rptr) || (size <= 0)) {
       return -EAGAIN;
   }

   MsgBodySize = MD_GET_MSG_SIZE(Msg);


   if (size == MsgBodySize) {
       MsdCopyMemory(Msg->b_rptr, buffer, size);
   
       if (Msg->b_cont) {
           PSTRM_MSG            Msgbk;
           Msgbk = Msg->b_cont;
           Msg->b_cont = NULL;
           // There is a contination, let's put the buffer back
           queue_put_bk_msg_Array(Array, Msgbk, MSD_READER);
       } else {
           queue_commit_msg_Array(Array, MSD_READER);
       }

       mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
       return size; // For now readv complains the different size is returned...
   }

   if (size > MsgBodySize) {
       MsdCopyMemory(Msg->b_rptr, buffer, MsgBodySize);
        
       if (Msg->b_cont) {
           PSTRM_MSG            Msgbk;
           Msgbk = Msg->b_cont;
           Msg->b_cont = NULL;
           queue_put_bk_msg_Array(Array, Msgbk, MSD_READER);
           mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
           return size; // For now readv complains the different size is returned...
       } else {
           queue_commit_msg_Array(Array, MSD_READER);
       }
       mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
       return MsgBodySize;
   }

   if (size < MsgBodySize) {
       PSTRM_MSG            Msgbk;
       // In this case we will copy what the size is and 
       // put back the message to the front of the queue 
       // after incrementing the r_ptr for the same size.
       Msgbk = allocb(MsgBodySize-size, GFP_ATOMIC);
       if (Msgbk) {
           // copy buffer from message
           MsdCopyMemory(Msg->b_rptr, buffer, size);

           // setup new message, free old message, and queue new message
           MsdCopyMemory(Msg->b_rptr+size, Msgbk->b_rptr, MsgBodySize-size);
           Msgbk->b_cont = NULL;
           if (Msg->b_cont) {
               Msgbk->b_cont = Msg->b_cont;
               Msg->b_cont = NULL;
           }
           mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
           queue_put_bk_msg_Array(Array, Msgbk, MSD_READER);
           return MsgBodySize;
       } else {
           cmn_err(CE_WARN, "mercd_copyfromBuffer alloc failure\n");
           queue_put_bk_msg_Array(Array, Msg, MSD_READER);
           return -EAGAIN;
       }
   }
}
#endif /* LiS */
