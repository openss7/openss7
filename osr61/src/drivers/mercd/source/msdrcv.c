/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdrcv.c
 * Description			: Host FW Receive functions 
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDRCV_C_
#include "msdextern.h"
#undef _MSDRCV_C_

#include "msdpciif.h"

/***************************************************************************
 * Function Name		: msd_sram_sanity_dump
 * Function Type		: Host FW Receive function
 * Inputs			: padapter, caller
 * Outputs			: none - print to system log
 * Calling functions		: send and receive mesages
 * Description			: Dumps few registers during Sram Corruption
 ****************************************************************************/
void msd_sram_sanity_dump(pmercd_adapter_block_sT padapter)
{
#ifndef LFS
   merc_int_t                   i;
   merc_uint_t                  ConfigId;
#endif
   merc_uint_t                  MaxMessages;
   merc_uchar_t                 RegVal;
#ifndef LFS
   merc_uchar_t                 InUchar;
#endif
   merc_uchar_t                 HostRamReq;
#ifndef LFS
   pmerc_uchar_t                PanicAddr;
#endif
   PMERC_CIR_BUFFER             CirBuffer;
   pmercury_host_info_sT        HostIfPtr;
#ifndef LFS
   PMERCURY_HOST_IF_CONFIG_ACK  ConfigAck;
   mercd_dhal_mem_copy_sT meminfo = { 0 };
#endif

   printk("mercd: Sram Sanity Debug\n");

   if (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED) {
       printk("Adapter not in Downloaded state\n");
       return;
   }

   if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
       return;
   }

   if (!(padapter->flags.SramOurs & MERC_ADAPTER_FLAG_SRAM_IS_OURS)) {
       MERC_GET_SRAM_NO_INTR_LOCK((&padapter->phost_info->reg_block), HostRamReq);
       if (!(HostRamReq & MERC_HOST_RAM_GRANT_R) || (HostRamReq == 0xFF)) {
           printk("mercd: unable to acquire Sram\n");
           return;
       }
   }

   padapter->phost_info->reg_block.HostRamStart =
                  padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;

   RegVal = MsdRegReadUchar(padapter->phost_info->reg_block.HostRamStart+0x0007FF80);
   if (RegVal == 0xFF) return;
   printk("mercd: Register (0x0007FF80) = %#x\n", RegVal);
   RegVal = MsdRegReadUchar(padapter->phost_info->reg_block.HostRamStart+0x0007FF84);
   if (RegVal == 0xFF) return;
   printk("mercd: Register (0x0007FF84) = %#x\n", RegVal);
   RegVal = MsdRegReadUchar(padapter->phost_info->reg_block.HostRamStart+0x0007FF88);
   if (RegVal == 0xFF) return;
   printk("mercd: Register (0x0007FF88) = %#x\n", RegVal);
   RegVal = MsdRegReadUchar(padapter->phost_info->reg_block.HostRamStart+0x0007FF90);
   if (RegVal == 0xFF) return;
   printk("mercd: Register (0x0007FF90) = %#x\n", RegVal);
   RegVal = MsdRegReadUchar(padapter->phost_info->reg_block.HostRamStart+0x0007FF94);
   if (RegVal == 0xFF) return;
   printk("mercd: Register (0x0007FF94) = %#x\n", RegVal);

#ifdef LFS
   RegVal = *(merc_uchar_t*)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA0_MODE_REG);
#else
   RegVal = (merc_uchar_t)padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA0_MODE_REG;
#endif
   if (RegVal == 0xFF) return;
   printk("mercd: MSD_PCI_PLX_DMA0_MODE_REG = %#x\n", RegVal);
#ifdef LFS
   RegVal = *(merc_uchar_t*)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA1_MODE_REG);
#else
   RegVal = (merc_uchar_t)padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA1_MODE_REG;
#endif
   if (RegVal == 0xFF) return;
   printk("mercd: MSD_PCI_PLX_DMA1_MODE_REG = %#x\n", RegVal);

   HostIfPtr=&padapter->phost_info->merc_host_info;
   CirBuffer=(PMERC_CIR_BUFFER)HostIfPtr->fw_to_host_msg_start;
   MaxMessages = padapter->phost_info->host_config.maxFwDataXfer;

   printk("mercd: Read %#x  Write %#x  MaxMsg %#x\n",
   CirBuffer->Read, CirBuffer->Write, MaxMessages);
   if (!(CirBuffer->Read<MaxMessages)||!(CirBuffer->Write<MaxMessages)) {
       printk("mercd: Unable to recover from corruption. Board Out of Service.\n");
       padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
   } else {
       printk("mercd: recovered from corruption.\n");
   }

#if 0
   // crashing - fix later
   if (padapter->panicDump == NULL)
       mercd_zalloc(padapter->panicDump, pmerc_uchar_t, MD_PANIC_DUMP_MAX_SIZE);
   if (padapter->panicDump) {
       ConfigAck = (PMERCURY_HOST_IF_CONFIG_ACK) CONFIG_ACK_ADDRESS(padapter->phost_info->reg_block.HostRamStart);
       PanicAddr = (merc_uchar_t *)(&(ConfigAck->general[0]));
       CHECK_SRAM_ADDRESS(PanicAddr, "supp_read_brd_panic_buf", padapter);
       meminfo.src = (pmerc_char_t)PanicAddr;
       meminfo.dest = (pmerc_char_t)padapter->panicDump;
       meminfo.size = MD_PANIC_DUMP_MAX_SIZE;
       meminfo.datatype = MSD_INT_DATA;
       meminfo.dir = MSD_TO_HOST;
       memcpy(meminfo.dest, meminfo.src, MD_PANIC_DUMP_MAX_SIZE);
       printk("mercd: dumping Panic Location on Adapter %d...\n", padapter->adapternumber);
       for (i=0; i<MD_PANIC_DUMP_MAX_SIZE; i++) {
            if (!(i%8))
                printk("\n");
            printk("%c ", padapter->panicDump[i]);
       }
       printk("\n");
   } else {
       printk("mercd: unable to allocate memory for Panic Buffer\n");
   }
#endif
}

/***************************************************************************
 * Function Name		: rcv_msgs_from_adapter
 * Function Type		: Host FW Receive function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver MsdRcvMsgFromAdapter Routine.
 *				  This routine receives message from adapter.
 * Additional comments		:
 ****************************************************************************/
merc_int_t rcv_msgs_from_adapter(pmercd_adapter_block_sT padapter)
{
   PMERC_CIR_BUFFER	CirBuffer= NULL;
   merc_int_t	NextRead;
   merc_int_t	WriteIndex;
   merc_uint_t NextMessageAddress;
   merc_uint_t MaxMessages;
   pmercury_host_info_sT           HostIfPtr;


   HostIfPtr=&padapter->phost_info->merc_host_info;
   CirBuffer=(PMERC_CIR_BUFFER)HostIfPtr->fw_to_host_msg_start;
   MaxMessages = padapter->phost_info->host_config.maxFwMsgs;

   // get the read and write pointers 
   NextRead = CirBuffer->Read;
   WriteIndex = CirBuffer->Write;

   // If sram is corrupt, let the sanity function take care of it
   if (!(NextRead < MaxMessages)||!(WriteIndex < MaxMessages)){
       msd_sram_sanity_dump(padapter);
       return(MD_FAILURE);
   }

   // do while there's more to process 
   while (NextRead != WriteIndex) {
       NextMessageAddress = (merc_uint_t)HostIfPtr->fw_to_host_msg_start+ sizeof(MERC_CIR_BUFFER) +
				             ((sizeof(MERC_HOSTIF_MSG)+MD_OLD_BODY_SIZE)*NextRead);
       MSD_LEVEL2_DBGPRINT("NextMessageAddress: %lx\n", NextMessageAddress);
       CHECK_SRAM_ADDRESS(NextMessageAddress, "rcv_msgs_from_adapter", padapter);

       // now read in and process 
       if (rcv_rceive_msg(padapter, NextMessageAddress) != MD_SUCCESS) {
	   MSD_ERR_DBGPRINT("MercReceiveMesg: Recv Msgs() failed.\n");
	   CirBuffer->Read = NextRead;
	   goto out;
       }

       NextRead++;
       if (NextRead == MaxMessages) {
  	   NextRead = 0;
	   continue;
       }
   }

   CirBuffer->Read = NextRead;

out:
   return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: rcv_data_from_adapter
 * Function Type		: Host FW Receive function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver MsdDataMsgFromAdapter Routine.
 *				  This routine receives data from adapter.
 * Additional comments		:
 ****************************************************************************/
merc_int_t rcv_data_from_adapter(pmercd_adapter_block_sT padapter,  int doDMA)
{
    PMERC_CIR_BUFFER	  CirBuffer= NULL;
    merc_int_t	          NextRead;
    merc_int_t	          WriteIndex;
    merc_int_t	          ReturnCode = MD_SUCCESS;
    merc_uint_t	          NextMessageAddress;
    merc_uint_t	          MaxMessages;
    pmercury_host_info_sT HostIfPtr;
    merc_int_t            cannotDMA=0;
    merc_ulong_t          totalIn = 0;
    merc_int_t i;


    HostIfPtr = &padapter->phost_info->merc_host_info; 
    CirBuffer = (PMERC_CIR_BUFFER)HostIfPtr->fw_to_host_data_start;
    MaxMessages = padapter->phost_info->host_config.maxFwDataXfer;

    NextRead = CirBuffer->Read;
    WriteIndex = CirBuffer->Write;

    // If sram is corrupt, let the sanity function take care of it
    if (!(NextRead< MaxMessages)||!(WriteIndex< MaxMessages)){
        msd_sram_sanity_dump(padapter);
        return(MD_FAILURE);
    }

    while (NextRead != WriteIndex) {
        NextMessageAddress = (merc_uint_t)HostIfPtr->fw_to_host_data_start
              +sizeof(MERC_CIR_BUFFER) +(sizeof(MERC_DATA_CIR_BUF_ENTRY)*NextRead);

        CHECK_SRAM_ADDRESS(NextMessageAddress, "rcv_data_from_adapter", padapter);
	 	  
        if (doDMA) {
            if ((ReturnCode = pci_rcv_process_dma(padapter, NextMessageAddress)) !=MD_SUCCESS) {
                cannotDMA = 1;
                totalIn++;
                if (padapter->phw_info->pdma_info->rx_dma_count) {
                    // we did manage to gather some dma blocks, so start 'em and 
	  	    // return with sram-lock intact. note: this means that if we've 
	 	    // got so many dma blocks and hit a block for which we cannot do
		    // dma, then we stop processing any further blocks and do the dma now.
                    if (ReturnCode == MD_MEM_FAILURE) {
		        cannotDMA = 0; 
		        CirBuffer->Read = NextRead;
		        break;
		    } else {
		        MSD_LEVEL2_DBGPRINT("rcv_data_from_adapter: CannotDMA last block ");
		    }
                } 
            }
 	} else {
	    cannotDMA = 1;
	}

	// If we hit block that we can not DMA follow through ... 
        if (cannotDMA) {
	    // Please notice changes in rcv_receive_data
            if ((ReturnCode = (rcv_receive_data(padapter, NextMessageAddress)) != MD_SUCCESS)) {
	        // If this falls here, it is a serious error.
                CirBuffer->Read = NextRead;
                goto out;
            }
        }

        totalIn++;
        NextRead++;
        if (NextRead == MaxMessages) {
            NextRead = 0;
            continue;
        }
    } /* while */

    // this means that we were able to do dma for ALL data blocks or
    // SOME data blocks until we found one that couldn't be dma'ed!
    // note that we have not advanced the circular read index.
    // We'll do that 'after' dma completes.
    /////////////////// Correct Source Block Begin////////////////
    CirBuffer->Read = NextRead;

    if (padapter->phw_info->pdma_info->rx_dma_count && totalIn) {
	// We may have jumped to from rcv_process_dma failure and there is a pending DMA..  
        padapter->flags.DPCDmaPending |= MERC_ADAPTER_FLAG_DPC_DMA_PENDING;
        padapter->phw_info->pdma_info->rx_dma_started++;
#ifndef POST_DMA_FREE
	// We can increment the index safely here, as we will not mess with SRAM anymore... 
        CirBuffer->Read = NextRead;
#else
        // note that we have not advanced the circular read index.
        // We'll do that 'after' dma completes.
        padapter->phw_info->pdma_info->dmaRdIndex = CirBuffer->Read;
        padapter->phw_info->pdma_info->dmaWrIndex = CirBuffer->Write;
        padapter->phw_info->pdma_info->dma_blk_count = totalIn;
#endif

        // Start DMA on channel 1 (Please note we are passing slot)
        MsdPlxDmaEnable_Chn1(mercd_adapter_map[padapter->adapternumber]);
	MsdPlxDmaStart_Chn1(mercd_adapter_map[padapter->adapternumber]);

    } else { 
	//We are not doing DMA this time..... 
        for (i=0; i < padapter->phw_info->pdma_info->rx_mbuf_count; i++) {
             if (padapter->phw_info->pdma_info->rxmbufdma[i] != 0) {
                 PSTRM_MSG       Msg;
                 Msg = (PSTRM_MSG) padapter->phw_info->pdma_info->rxmbufdma[i];
                 fwmgr_msg_reply_mgr(Msg, padapter);
             } 
     	}  

	// Reset the mbuf counter, all messages are delivered..... 
  	padapter->phw_info->pdma_info->rx_mbuf_count = 0;
	 
    } 

out:
    CirBuffer->Read = NextRead;
    return(ReturnCode);
}


/***************************************************************************
 * Function Name		: rcv_rceive_msg
 * Function Type		: Host FW Receive function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			:  Streams Driver rcv_rceive_msg Routine.
 *				   This routine reads a single message from 
 *				   the shared Ram
 * Additional comments		:
 ****************************************************************************/
md_status_t rcv_rceive_msg(pmercd_adapter_block_sT padapter, size_t MsgAddress)
{
    PMERC_HOSTIF_MSG	 FwMessage; 
    PSTRM_MSG		 Msg;
    PMDRV_MSG		 MdMsg;
    PSTRM_MSG		 DataMsg;
    PMERC_HOSTIF_MSG	 MercMsg;
    PMERC_DATA_BLK_HDR	 DataBlkPtr; 
    mercd_dhal_mem_copy_sT mem_copyinfo = { 0 } ;
    merc_uint_t		 MsgBodySize;
    merc_uchar_t	 Class;
    pmercd_bind_block_sT BindBlock;	
#ifdef _8_BIT_INSTANCE
    MSD_HANDLE		 BindHandle;
#else
    MBD_HANDLE      	 BindHandle;
#endif

    MercMsg = (PMERC_HOSTIF_MSG)MsgAddress;

    if ((MsgBodySize=MERCURY_GET_BODY_SIZE(MercMsg)) <= 24) {
	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
	if (Msg == NULL) {
	    printk("rcv_rceive_msg: Unable to allocate memory\n");
 	    return (MD_FAILURE);
 	}

	DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+MsgBodySize,GFP_ATOMIC);
	if (DataMsg == NULL) {
	    printk("rcv_rceive_msg: Unable to allocate data memory\n"); 
	    MSD_FREE_MESSAGE(Msg);
	    return (MD_FAILURE);
 	}

	MD_SET_MSG_TYPE(DataMsg, M_DATA);
	MSD_LINK_MESSAGE(Msg,DataMsg);

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	FwMessage = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

	mem_copyinfo.src = (pmerc_char_t)MsgAddress;
	mem_copyinfo.dest = (pmerc_char_t)FwMessage;
	mem_copyinfo.size = sizeof(MERC_HOSTIF_MSG) + MsgBodySize;
	mem_copyinfo.datatype = MSD_INT_DATA;
	mem_copyinfo.dir = MSD_TO_HOST;

	(*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

        if (MERCURY_GET_TRANSACTION_ID(FwMessage) == 0x800000) {
            padapter->rcvflowcount++;
        }
    } else {
	DataBlkPtr = (PMERC_DATA_BLK_HDR)TO_KV_ADDRESS(padapter,MERCURY_GET_BODY_SIZE(MercMsg));

	MSD_LEVEL2_DBGPRINT("rcv_rceive_msg: Buf Offset=%d\n", MERCURY_GET_BODY_SIZE(MercMsg));

	MercMsg = (PMERC_HOSTIF_MSG) TO_KV_ADDRESS(padapter, DataBlkPtr->DataBlockOffset);

	// check for alignement of address in SRAM 
#ifdef LFS
	if ((merc_uint_t) (long)MercMsg % 4) {
	    printk("rcv_rceive_msg: Misalligned SRAM Data Block offset. %d\n", (merc_uint_t) (long)MercMsg);
#else
	if ((merc_uint_t) MercMsg % 4) {
	    printk("rcv_rceive_msg: Misalligned SRAM Data Block offset. %d\n", (merc_uint_t) MercMsg);
#endif
	    return(MD_FAILURE);
	}

	// make sure the 'MercMsg' is in SRAM space
	CHECK_SRAM_ADDRESS(MercMsg , "rcv_rceive_msg()", padapter) ;

	MsgBodySize = MERCURY_GET_BODY_SIZE(MercMsg);

	if (!((MsgBodySize <= MD_MAX_BODY_SIZE) && (MsgBodySize >= MD_MIN_BODY_SIZE))) {
	    MSD_LEVEL2_DBGPRINT("rcv_rceive_msg: Invalid msg body size (%d)\n", MsgBodySize);
	    return(MD_FAILURE);
	}

	MSD_ASSERT(DataBlkPtr->DataCount == sizeof(MERC_HOSTIF_MSG)+ MsgBodySize);

	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
	if (Msg == NULL) {
	    printk("rcv_rceive_msg: Unable to allocate memory\n");
	    return(MD_FAILURE);
	}

	DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG)+MsgBodySize,GFP_ATOMIC);
	if (DataMsg == NULL) {
	    printk("rcv_rceive_msg: Unable to allocate data memory\n");
	    MSD_FREE_MESSAGE(Msg);
  	    return(MD_FAILURE);
	}
	
	MD_SET_MSG_TYPE(DataMsg, M_DATA);
	MSD_LINK_MESSAGE(Msg,DataMsg);

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	FwMessage = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

	CHECK_SRAM_ADDRESS(MercMsg, "rcv_rceive_msg", padapter);
	
	mem_copyinfo.src = (pmerc_char_t)MercMsg;
	mem_copyinfo.dest = (pmerc_char_t)FwMessage;
	mem_copyinfo.size = sizeof(MERC_HOSTIF_MSG) + MsgBodySize;
	mem_copyinfo.datatype = MSD_INT_DATA;
	mem_copyinfo.dir = MSD_TO_HOST;

	(*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

	snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)DataBlkPtr);
    }

    // Trace Code: Copy the Message to the buffer (if required)
    if (padapter->flags.TraceLevelInfo & MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED) {
        supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMERC_HOSTIF_MSG)FwMessage);
    }

    // fetch the message class 
    Class = MERCURY_GET_MESSAGE_CLASS(FwMessage);

    switch (Class) {
	case MERCURY_CLASS_DRIVER: // to be procss locally 
	     if (fwmgr_reply_mgr(padapter,Msg)==MD_FAILURE) {
		 MSD_ERR_DBGPRINT("rcv_rceive_msg: ProcDriverClass failed.\n");
		 return(MD_SUCCESS);
	     }
	     break;

	case MERCURY_CLASS_SYSTEM: // send to user 
	case MERCURY_CLASS_RESOURCE:	
	case MERCURY_CLASS_RESOURCE_INT:	/* PTR1489 */ {
	     // Branch for covering board to board messaging
             merc_uint_t X_MSG_AdapterNUmber;
             FwMessage = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;

             X_MSG_AdapterNUmber = MERCURY_GET_DEST_BOARD(FwMessage);
             if (X_MSG_AdapterNUmber) {
                 pmercd_adapter_block_sT pXadapter;
                 merc_uint_t XAdapterNumber;

                 if (X_MSG_AdapterNUmber >= MsdControlBlock->maxadapters) {
                     MSD_ERR_DBGPRINT("mercd: adapter number from the firmware\n");
                     MSD_FREE_MESSAGE(Msg);
		     return(MD_FAILURE);
                 }
 
                 XAdapterNumber = mercd_adapter_log_to_phy_map_table[X_MSG_AdapterNUmber];
 
                 if (XAdapterNumber != mercd_adapter_map[padapter->adapternumber]) {
                     // Check if this is a Xmessages.....
                     if (XAdapterNumber == 0xFF) {
                         MSD_FREE_MESSAGE(Msg);
		          break;
                     }
 
                     pXadapter = MsdControlBlock->padapter_block_list[XAdapterNumber];
                     if (pXadapter == NULL) {
                         MSD_FREE_MESSAGE(Msg);
			 break;
                     }
 
                     // validity checking
                     if ((pXadapter->state & MERCD_ADAPTER_STATE_STARTED) ||
                         (pXadapter->state == MERCD_ADAPTER_STATE_DOWNLOADED)) {
                         merc_uint_t     Result;

			 if ((pXadapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	  		     (pXadapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
                             MSD_ENTER_MUTEX(&pXadapter->snd_msg_mutex);
                             Result = strm_Q_snd_msg(Msg, pXadapter);
                             MSD_EXIT_MUTEX(&pXadapter->snd_msg_mutex);
                         } else {
                             Result = strm_ww_Q_snd_msg(Msg, pXadapter);
                         }

                         if (Result != MD_SUCCESS) {
                            MSD_ERR_DBGPRINT("MsdDownMessageStrategy:strm_Q_snd_msg failed.");
			    MSD_ERR_DBGPRINT(" Error=%d\n", Result);
                            MSD_FREE_MESSAGE(Msg);
                         }
			 break;

                     } else {
                         MSD_FREE_MESSAGE(Msg);
			 break;
                     }
                 }
             }

             // extract bind handle from destination address
             //16BIT Change
#ifdef _8_BIT_INSTANCE
             BindHandle = MERCURY_GET_DEST_COMPINST(FwMessage);
#else
             if (padapter->rtkMode == 0) {
                 //We need to access 8BIT Macros
                 //BindHandle = (FwMessage->SourceDestComponent & 0xFFFF0000) >> 16;
                 BindHandle = MERCURY_GET_DEST_COMPINST_8BITMODE(FwMessage);
             } else {
 		 BindHandle = MERCURY_GET_DEST_COMP(FwMessage);
                 BindHandle = BindHandle << 8;
                 BindHandle |= MERCURY_GET_DEST_INST(FwMessage);
             }
#endif

	     // must ensure handle is kopacetic 
	     if (BindHandle == 0 || BindHandle >= MsdControlBlock->maxbind) { 
		 MSD_FREE_MESSAGE(Msg);
		 return(MD_SUCCESS);
	     }
	 
	     // test to see if handle is valid.  It coiuld've been
	     //	closed just before this message is received

	     if ((BindBlock = MsdControlBlock->pbind_block_list[BindHandle]) == NULL) {
		 MSD_FREE_MESSAGE(Msg);
		 return(MD_SUCCESS);
	     }

	     if (BindBlock->stream_connection_ptr) {
		 printk("rcv_rceive_msg: This can not be a streaming kind of message BH is %d\n",BindHandle);
		 MSD_FREE_MESSAGE(Msg);
		 return(MD_SUCCESS);
	     }

	     MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
	     MD_SET_MDMSG_USER_CONTEXT(MdMsg, MD_MAP_BIND_TO_USER_CONTEXT(BindHandle));

	     MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)) +sizeof(MDRV_MSG));

	     if (MsdControlBlock->pbind_block_list[BindHandle]->flags == 0x8000) {
		 MSD_FREE_MESSAGE(Msg);
		 return(MD_SUCCESS);
	     }

	     if (supp_dispatch_up_Q(padapter,Msg)==MD_FAILURE) {
	  	 MSD_ERR_DBGPRINT("rcv_rceive_msg: ProcSystemClass failed.\n");
		 return(MD_FAILURE);
	     }
	     break;
	}
	default:
	     MSD_ERR_DBGPRINT("rcv_rceive_msg: Invalid Message Class (%d)", Class);
	     MSD_FREE_MESSAGE(Msg);
	     return(MD_FAILURE);
    }

    return(MD_SUCCESS);
}



/***************************************************************************
 * Function Name		: rcv_receive_data
 * Function Type		: Host FW Receive function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			:  Streams Driver rcv_receive_data Routine.
 *				   This routine is used to read a data message
 *				   from the Adapter SRAM and send it to user.
 * Additional comments		: Please notice DMA changes ...
 ****************************************************************************/
merc_int_t rcv_receive_data(pmercd_adapter_block_sT padapter,size_t MsgAddress)
{
    PMDRV_MSG			MdMsg;
    merc_uint_t      		returnStat = MD_SUCCESS;
    MERC_DATA_BLK_HDR		DataBlock;
    PMERC_DATA_BLK_HDR		DataBlockPtr=&DataBlock;
    pmerc_void_t		KvDataBlockPtr;
#ifdef LFS
    pmerc_char_t		KvSramData = '\0';
#else
    pmerc_char_t		KvSramData;
    pmercury_host_info_sT   	HostIfPtr;
#endif
    PMERC_DATA_CIR_BUF_ENTRY 	CirBufEntry= NULL;
    PSTRM_MSG			Msg = NULL;
    PSTRM_MSG			DataMsg = NULL;
    PSTREAM_RECEIVE		Ptr;
    pmercd_stream_connection_sT  StreamBlock;
    mercd_dhal_mem_copy_sT mem_copyinfo = { 0 } ;

    CirBufEntry = (PMERC_DATA_CIR_BUF_ENTRY)MsgAddress;

    // get pointer to actual data 
    KvDataBlockPtr = (pmerc_void_t)TO_KV_ADDRESS(padapter,CirBufEntry->DataHeaderOffset); 

    DataBlockPtr = (PMERC_DATA_BLK_HDR)KvDataBlockPtr;
    if ((DataBlockPtr->BufFlags & HIF_STREAM_F_EOS) && !(DataBlockPtr->DataCount)) {
	// This is a zero byte last block format CloseStream message 
 	PSTREAM_SESS_CLOSE_ACK  ClosePtr;
	// get a local buffer 
	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
	if (Msg == NULL) {
	    printk("rcv_receive_data: Unable to allocate memory\n");
	    returnStat = MD_MEM_FAILURE;
	    returnStat = MD_MEM_FAILURE;
	    goto out;
	}

	Msg->b_cont=NULL;

	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
	MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);
	MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)+sizeof(STREAM_SESS_CLOSE_ACK)));
		
	ClosePtr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
	ClosePtr->StreamId = CirBufEntry->StreamId; 
	
	// SRAM DMA ADDITIONS ......
	// The following is for orderly delivery
    	padapter->phw_info->pdma_info->rxmbufdma[padapter->phw_info->pdma_info->rx_mbuf_count] = Msg;
    	padapter->phw_info->pdma_info->rx_mbuf_count++;	
	//fwmgr_msg_reply_mgr(Msg, padapter);
		
 	snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)KvDataBlockPtr);
	return (MD_SUCCESS);
    }

    if (DataBlockPtr->NextNodeOffset != 0) {
 	// ignore this field. Make it NULL, F/W dows not garentee its value
	MSD_ERR_DBGPRINT("rcv_receive_data: nextnode not null.\n");
	DataBlockPtr->NextNodeOffset = 0;
    }

    // make sure data count is not overly large 
    if (DataBlockPtr->DataCount > (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)) ) {
 	printk("rcv_receive_data: Bad DataCount=%d\n", DataBlockPtr->DataCount);
	MSD_ASSERT(DataBlockPtr->DataCount <= (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER)));
	returnStat = MD_FAILURE;
	goto out;
    }

    // get a local buffer 
    Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_RECEIVE),GFP_ATOMIC);
    if (Msg == NULL) {
	printk("rcv_receive_data: Unable to allocate memory\n");
 	returnStat = MD_MEM_FAILURE;
	goto out;
    }

    // is stream id any good?
    if (CirBufEntry->StreamId >= MSD_MAX_STREAM_ID) {         
        printk("rcv_receive_data: Invalid StreamId %d\n", CirBufEntry->StreamId);
	snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)DataBlockPtr);
	return(MD_SUCCESS);  	
	/* PH: I know this looks funny, but this lets the caller */
	/* get passed the ring slot of the message instead */
	/* of leaving it for a retry later. */
    }

    StreamBlock = padapter->pstream_connection_list[CirBufEntry->StreamId]; 

    // is stream block pointer NULL? 
    if (StreamBlock == NULL) {
	printk("rcv_receive_data: No StreamBlock for is %d\n", CirBufEntry->StreamId);
	snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)DataBlockPtr);
	return(MD_SUCCESS); 
 	/* PH: I know this looks funny, but this lets the caller */
	/* get passed the ring slot of the message instead */
	/* of leaving it for a retry later. */
   }

   // Take care of null buf case 
   if (DataBlockPtr->DataCount) {
	MSD_ASSERT(DataBlockPtr->DataBlockOffset);
	// get virt address of data 
	KvSramData = (pmerc_char_t)TO_KV_ADDRESS(padapter, DataBlockPtr->DataBlockOffset);
 
	if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
  	    DataMsg = supp_alloc_buf(DataBlockPtr->DataCount,GFP_ATOMIC);
	} else {
	    DataMsg = supp_alloc_buf(DataBlockPtr->DataCount - sizeof(USER_HEADER),GFP_ATOMIC );
        }
	
	if (DataMsg == NULL) {
	    printk("rcv_receive_data: Unable to allocate data memory\n");
	    returnStat = MD_MEM_FAILURE;
	    MSD_FREE_MESSAGE(Msg);
	    goto out;
	}
	
 	MD_SET_MSG_TYPE(DataMsg,M_DATA);	

	MSD_LEVEL2_DBGPRINT("MercRecvDATA: MS %d, DC %d, SU %d, KV %d\n", 
	  MsdMessageDataSize(DataMsg), DataBlockPtr->DataCount, sizeof(USER_HEADER), KvSramData);

	CHECK_SRAM_ADDRESS(KvSramData, "rcv_receive_data", padapter);

	// is this a GStream?
	if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
  	    mem_copyinfo.src = (pmerc_char_t)KvSramData;
	    mem_copyinfo.dest = (pmerc_char_t)MD_GET_MSG_READ_PTR(DataMsg);
	    mem_copyinfo.size = DataBlockPtr->DataCount;
	    mem_copyinfo.datatype = MSD_SYS_DATA;
	    mem_copyinfo.dir = MSD_TO_HOST;

 	    (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

	    MD_SET_MSG_WRITE_PTR(DataMsg, (MD_GET_MSG_READ_PTR(DataMsg) + (DataBlockPtr->DataCount)));
	} else {		
	    mem_copyinfo.src = (pmerc_char_t)KvSramData + sizeof(USER_HEADER);
	    mem_copyinfo.dest = (pmerc_char_t)MD_GET_MSG_READ_PTR(DataMsg);
	    mem_copyinfo.size = DataBlockPtr->DataCount - sizeof(USER_HEADER);
	    mem_copyinfo.datatype = MSD_SYS_DATA;
	    mem_copyinfo.dir = MSD_TO_HOST;

	    (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

	    MD_SET_MSG_WRITE_PTR(DataMsg, (MD_GET_MSG_READ_PTR(DataMsg) + 
			(DataBlockPtr->DataCount - sizeof(USER_HEADER))));
	}
	MSD_LINK_MESSAGE(Msg,DataMsg);
    }


    //// SRAM DMA ADDITIONS ...... 
    padapter->phw_info->pdma_info->rxmbufdma[padapter->phw_info->pdma_info->rx_mbuf_count] = Msg;
    padapter->phw_info->pdma_info->rx_mbuf_count++;	

    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

    MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
    MD_SET_MDMSG_ID(MdMsg, MID_STREAM_RECEIVE);

    Ptr = (PSTREAM_RECEIVE)MD_GET_MDMSG_PAYLOAD(MdMsg);

    mem_copyinfo.src = (pmerc_char_t)KvSramData;
    mem_copyinfo.dest = (pmerc_char_t)(&Ptr->StreamUserHeader);
    mem_copyinfo.size = sizeof(USER_HEADER);
    mem_copyinfo.dir = MSD_TO_HOST;

    if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
 	mem_copyinfo.datatype = MSD_SYS_GSTREAM_DATA;
    } else {
	mem_copyinfo.datatype = MSD_SYS_BSTREAM_DATA;
    }

    (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);

    Ptr->StreamId = CirBufEntry->StreamId; 
    Ptr->Flags = 0;

    Ptr->Sequence = (merc_uint_t)DataBlockPtr->Sequence;


#ifdef SKIP  /* VISIT LATER NOTRACING FOR DATA YET */
	// Trace Code: Copy the Message to the buffer (if required)
	if (padapter->TraceLevel) {
		supp_push_trace_msg(padapter, MSD_BRD2DRV_TRACE,(PMDRV_MSG)MdMsg);
	}
#endif

    //// SRAM DMA ADDITIONS ......
    // We are saving this for inorder delivery
    // fwmgr_msg_reply_mgr(Msg, padapter);

    snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)KvDataBlockPtr);

    return(MD_SUCCESS);
out:

    // Must not release data node here since we'll be trying this again later 
    return(returnStat);	
}	

/***************************************************************************
 * Function Name                : pci_rcv_process_dma
 * Function Type                : DHAL & OSSHL Function
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_RCV_PROCESS_DMA Interfaces
 * Description                  :
 * Additional comments          : - DMA chain block in SRAM is updated to read  *
                                            one data block.
 ****************************************************************************/
int pci_rcv_process_dma(pmercd_adapter_block_sT padapter, size_t MsgAddress)
{

    PMDRV_MSG       	     MdMsg;
    MERC_DATA_BLK_HDR        DataBlock;
    PMERC_DATA_BLK_HDR       DataBlockPtr=&DataBlock;
    ULONG           	     KvDataBlockPtr;
    PCHAR                    KvSramData;
    PSTREAM_RECEIVE          Ptr;
    PSTRM_MSG                Msg = NULL;
    PSTRM_MSG                DataMsg = NULL;
#ifndef LFS
    pmercury_host_info_sT    HostIfPtr;
#endif
    PMERC_DATA_CIR_BUF_ENTRY CirBufEntry= NULL;
    pmercd_stream_connection_sT  StreamBlock;

    mercd_dhal_mem_copy_sT mem_copyinfo = { 0 } ;
    mercd_dhal_dma_sT   mem_prepdma = { 0 } ;

    merc_uint_t      returnStat = MD_SUCCESS;

    CirBufEntry = (PMERC_DATA_CIR_BUF_ENTRY)MsgAddress;

    // get pointer to actual data
    KvDataBlockPtr = (ULONG)TO_KV_ADDRESS(padapter,CirBufEntry->DataHeaderOffset);

    DataBlockPtr = (PMERC_DATA_BLK_HDR)KvDataBlockPtr;

    if ((DataBlockPtr->BufFlags & HIF_STREAM_F_EOS) && !(DataBlockPtr->DataCount)) {
	// This is a zero byte last block format CloseStream message
        PSTREAM_SESS_CLOSE_ACK  ClosePtr;
        
	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+MD_MAX_DRIVER_BODY_SZ,GFP_ATOMIC);
        if (Msg == NULL) {
	    // These brunches are not tested. If it falls,  
	    // we did not test if all streams memory is depleted 
	    // and bufalloc is necessary, especially for DMA ... 
            printk("pci_rcv_process_dma: unable to allocate memory\n");
            return(MD_FAILURE);
        }

 	Msg->b_cont=NULL;
        MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
        MD_SET_MDMSG_ID(MdMsg, MID_STREAM_CLOSE_SESS_ACK);
        MD_SET_MSG_WRITE_PTR(Msg, (MD_GET_MSG_READ_PTR(Msg)
	 +sizeof(MDRV_MSG)+sizeof(STREAM_SESS_CLOSE_ACK)));

        ClosePtr = (PSTREAM_SESS_CLOSE_ACK)MD_GET_MDMSG_PAYLOAD(MdMsg);
        ClosePtr->StreamId = CirBufEntry->StreamId;

	// We will save this message in mbuf chain tobe delivered to upstream later.  (inordely delivery)
	padapter->phw_info->pdma_info->rxmbufdma[padapter->phw_info->pdma_info->rx_mbuf_count] = Msg;
	padapter->phw_info->pdma_info->rx_mbuf_count++;

	returnStat = MD_SUCCESS;
	goto earlyExit;
    }

    if (DataBlockPtr->NextNodeOffset != 0) {
        // ignore this field. Make it NULL, F/W dows not garentee its value
        MSD_ERR_DBGPRINT("pci_rcv_process_dma: nextnode not null.\n");
        DataBlockPtr->NextNodeOffset = 0;
    }


    // make sure data count is not overly large
    if (DataBlockPtr->DataCount > (MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER))) {
	//padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
	printk("pci_rcv_process_dma:  Bad DataCount=%d\n", DataBlockPtr->DataCount);
	returnStat = MD_FAILURE;
        goto earlyExit;
    }


    // make sure data count is not  a small one
    if (DataBlockPtr->DataCount <  MIN_RECV_DMA_THR) {
	// For this case, we will fall under rcv_data..... PIO ...
	returnStat = MD_FAILURE;
	goto earlyExit;
    }

    // is stream id any good?
    if (CirBufEntry->StreamId >= MSD_MAX_STREAM_ID) {
	//padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
        printk("pci_rcv_process_dma: Invalid StreamId %d\n", CirBufEntry->StreamId);
	returnStat = MD_FAILURE;
	goto earlyExit;
    }

    StreamBlock = padapter->pstream_connection_list[CirBufEntry->StreamId];
    if (StreamBlock == NULL) {
	padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
	printk("pci_rcv_process_dma: No StreamBlock for is %d (Out Of Service) \n", CirBufEntry->StreamId);
	returnStat = MD_FAILURE;
	goto earlyExit;
    } 

    // Take care of null buf case
    if (DataBlockPtr->DataCount) {
	// get a local buffer
    	Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_RECEIVE),GFP_ATOMIC);
    	if (Msg == NULL) {
            printk("pci_rcv_process_dma: unable to allocate memory\n");
            returnStat = MD_MEM_FAILURE;
            goto earlyExit;
    	}
        
	if (!DataBlockPtr->DataBlockOffset) {
	    printk("pci_rcv_process_dma: null DataBlockPtr->DataBlockOffset\n");
	    returnStat = MD_MEM_FAILURE;
	    goto earlyExit;
        }
    
	// get virt address of data
        KvSramData = (PCHAR)TO_KV_ADDRESS(padapter, DataBlockPtr->DataBlockOffset);

        if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
            DataMsg = supp_alloc_buf(DataBlockPtr->DataCount, GFP_DMA);
        } else {
            DataMsg = supp_alloc_buf(DataBlockPtr->DataCount-sizeof(USER_HEADER), GFP_DMA);
	}

        if (DataMsg == NULL) {
	    printk("pci_rcv_process_dma: unable to allocate data memory\n");
            MSD_FREE_MESSAGE(Msg);
 	    returnStat = MD_MEM_FAILURE;
            goto out;
        }
   
	MD_SET_MSG_TYPE(DataMsg,M_DATA);

        MSD_LEVEL2_DBGPRINT("pci_rcv_process_dma: MS %d, DC %d, SU %d, KV %d\n",
            MsdMessageDataSize(DataMsg), DataBlockPtr->DataCount, sizeof(USER_HEADER), KvSramData);

        CHECK_SRAM_ADDRESS(KvSramData, "rcv_receive_data", padapter); 

	// is this a GStream?
        if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
	    MD_SET_MSG_WRITE_PTR(DataMsg, (MD_GET_MSG_READ_PTR(DataMsg) + (DataBlockPtr->DataCount)));	
  	    mem_prepdma.mb = DataMsg;
   	    mem_prepdma.size = DataBlockPtr->DataCount;

	    // Please notice, we are sending physical address offset  here... 
	    mem_prepdma.sramAddr = DataBlockPtr->DataBlockOffset;
   	    mem_prepdma.dir = MSD_TO_HOST;
	    mem_prepdma.pdma = padapter->phw_info->pdma_info;
	    mem_prepdma.acc_handle =	
		padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->acc_handle;
	    mem_prepdma.padapter = padapter;

	    (*mercd_dhal_func[MERCD_DHAL_PREPARE_DMA])((void *)&mem_prepdma);
	} else {
	    MD_SET_MSG_WRITE_PTR(DataMsg, (MD_GET_MSG_READ_PTR(DataMsg) +
                       (DataBlockPtr->DataCount - sizeof(USER_HEADER))));

	    // Please notice, we are sending physical address offset  here... 
	    mem_prepdma.sramAddr = DataBlockPtr->DataBlockOffset + sizeof(USER_HEADER); 
	    mem_prepdma.mb = DataMsg;
	    mem_prepdma.size = DataBlockPtr->DataCount - sizeof(USER_HEADER);
  	    mem_prepdma.dir = MSD_TO_HOST;
	    mem_prepdma.acc_handle = 
		padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->acc_handle;
	    mem_prepdma.pdma = padapter->phw_info->pdma_info;
	    mem_prepdma.padapter = padapter;

	    (*mercd_dhal_func[MERCD_DHAL_PREPARE_DMA])((void *)&mem_prepdma);

	    if (mem_prepdma.ret != MD_SUCCESS) {
	        // DMA was not successfull.. Exiting ... 
	        returnStat = MD_FAILURE;
        	goto earlyExit;
	    }
	}

	MSD_LINK_MESSAGE(Msg,DataMsg);

	// We are saving the head of the message header here ...... 
	// The following is not efficient.  We keep two place holders. 
	// One for DMA scatter gather chains, in order to release the DMA 
	// resources only. The other one  is for holding all DMA and PIO 
	// completed message buffers to be delivered once the DMA completes 
	// or there is nothing to DMA such as the one  rcv_data_from_adapter. 
	padapter->phw_info->pdma_info->prxdma_res[padapter->phw_info->pdma_info->rx_dma_count-1].dma_mb = Msg;
	padapter->phw_info->pdma_info->rxmbufdma[padapter->phw_info->pdma_info->rx_mbuf_count] = Msg;
        padapter->phw_info->pdma_info->rx_mbuf_count++;

    } else {
	returnStat = MD_FAILURE;
	goto earlyExit;
    }
  
    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

    MD_SET_MDMSG_CLASS(MdMsg, MD_CLASS_CORE);
    MD_SET_MDMSG_ID(MdMsg, MID_STREAM_RECEIVE);

    Ptr = (PSTREAM_RECEIVE)MD_GET_MDMSG_PAYLOAD(MdMsg);

    mem_copyinfo.src = (PCHAR)KvSramData;
    mem_copyinfo.dest = (PCHAR)(&Ptr->StreamUserHeader);
    mem_copyinfo.size = sizeof(USER_HEADER);
    mem_copyinfo.dir = MSD_TO_HOST;

    if (StreamBlock->type == STREAM_OPEN_F_GSTREAM) {
       	mem_copyinfo.datatype = MSD_SYS_GSTREAM_DATA;
    } else {
        mem_copyinfo.datatype = MSD_SYS_BSTREAM_DATA;
    }

    (*mercd_dhal_func[MERCD_DHAL_MEM_COPY])((void *)&mem_copyinfo);
		
    Ptr->StreamId = CirBufEntry->StreamId;
    Ptr->Flags = 0;
    Ptr->Sequence = (ULONG)DataBlockPtr->Sequence;

    // NO UPSTREAM DELIVERY HERE ..... 	
    // fwmgr_msg_reply_mgr(Msg, padapter);

earlyExit:

    // POST_DMA_FREE is not enabled by default. It is safe to release 
    // data blocks at this time as we will keep the SRAM lock intact 
	        // until DMA complete interrupt arrives. 
		
#ifndef POST_DMA_FREE
    if (returnStat != MD_FAILURE)
	snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)KvDataBlockPtr);	
#endif

out:
    return returnStat;

}

/***************************************************************************
 * Function Name                : postprocessDMA
 * Function Type                : DHAL & OSSHL Function
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : Interfaces
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int postprocessDMA( pmercd_adapter_block_sT padapter)
{
  merc_uint_t  returnStat = MD_FAILURE;
  merc_uint_t   i;
  pmercd_dma_info_sT pdma_info;

  pdma_info = padapter->phw_info->pdma_info;

  // If There is nothing to process, quit immediately.
  // We should also wonder why we get here
  if ( (pdma_info->rx_dma_count <= 0 ) &&
       (pdma_info->tx_dma_count <= 0 ))  
     {
        MSD_ERR_DBGPRINT( 
           "postprocess DMA failure,without any pending DMA data.",
           "  We had a DMA comple te interrupt.\n");
            return  returnStat;
    }


  if ( padapter->flags.DMAIntrReceived & MERC_ADAPTER_FLAG_RCV_DMA_INTR ) 
   {

	// The following was designed originally and found to be unncessary 
        // as we are holding the sram lock until DMA completes.  
 
#ifdef POST_DMA_FREE
      if(pdma_info->rx_dma_count) 
       {

          PMERC_CIR_BUFFER                CirBuffer= NULL;
          pmercury_host_info_sT           HostIfPtr;
          merc_int_t                      NextRead;
          merc_int_t                      WriteIndex;
          merc_ulong_t                    NextMessageAddress;
          merc_uint_t                     dmaBlkCount;
          merc_ulong_t                    MaxMessages;

          HostIfPtr=&padapter->phost_info->merc_host_info;
          CirBuffer=(PMERC_CIR_BUFFER)HostIfPtr->fw_to_host_msg_start;

          MaxMessages = padapter->phost_info->host_config.maxFwMsgs;

          // get the read and write pointers
          NextRead = CirBuffer->Read;
          WriteIndex = CirBuffer->Write;

          ASSERT (padapter->phw_info->pdma_info->dmaRdIndex == NextRead);
          ASSERT (padapter->phw_info->pdma_info->dmaWrIndex == NextWrite);

          dmaBlkCount = padapter->phw_info->pdma_info->DmaBlkCount;

          // do while there's more to process
          while(dmaBlkCount > 0 &&
                NextRead != WriteIndex ) 
           {
              NextMessageAddress = (ULONG)HostIfPtr->fw_to_host_msg_start+
                      sizeof(MERC_CIR_BUFFER) +
                      ((sizeof(MERC_HOSTIF_MSG)+MD_OLD_BODY_SIZE)*NextRead);

              CHECK_SRAM_ADDRESS(NextMessageAddress, "rcv_msgs_from_adapter",
                    padapter);

              // get pointer to actual data
              KvDataBlockPtr =
                (ULONG)TO_KV_ADDRESS(padapter,CirBufEntry->DataHeaderOffset);

              DataBlockPtr = (PMERC_DATA_BLK_HDR)KvDataBlockPtr;

              snd_release_Hif_data_blk(padapter,(PMERC_DATA_BLK_HDR)KvDataBlockPtr);
              NextRead++;

              if(NextRead == MaxMessages)
                {
                  NextRead = 0;
                  continue;
                }
             dmaBlkCount--;
         } /* while */

         CirBuffer->Read = NextRead;
         ASSERT (NextWrite == CirBuffer->Write);
     } /* rx_dma_count */
#endif
     
     for(i=0; i < pdma_info->rx_dma_count;i++) 
     {
#ifndef LFS
       PSTRM_MSG        Msg = NULL;
       PSTRM_MSG        LinkedMsg = NULL;
#endif
       merc_ulong_t     size;

       if (pdma_info->prxdma_res[i].flags & MERC_PAGE_VALID ) 
          {
             size = padapter->phw_info->pdma_info->prxdma_res[i].size;
             //Sync it if necessary..Linux2.4 provides sync primitives
	     // same cash issues ...... 
	     // ddi_dma_sync for Solaris sparc 

	     //For an ideal case, we incorporated the code
             //that checks the page boundary validation during
             //descriptor allocation and deallocation. eg: Look
             //at linux_pci_prepare_dma routine where we prepare
             //descriptors and validate the page boundary. 

             //The following code does the same, but for now
             //is not tested. Will be tested thoroughly in future.
             
#if 0 
             if(!(pdma_info->prxdma_res[i].flags & MERC_PAGE_VALID ) )
                goto out;

             Msg = (PSTRM_MSG)pdma_info->prxdma_res[i].dma_mb;

             if(((!pdma_info->prxdma_res[i].flags & MERC_PAGE_END))
                && (pdma_info->prxdma_res[i].flags & MERC_PAGE_PASS)) 
               {
                 pdma_info->prxdma_res[i].flags = 0;
                 i++;
                 if(!(pdma_info->prxdma_res[i].flags & MERC_PAGE_END ) )
                    goto out;
                 size = pdma_info->prxdma_res[i].size;
                 
                 //Sync it if necessary
                 LinkedMsg = (PSTRM_MSG)pdma_info->prxdma_res[i].dma_mb;
                 Msg->b_next =  LinkedMsg;

                 if(!pullupmsg(Msg,-1))
                 // Let's continue, but we drop the message.... :-(
                 cmn_err(CE_CONT,"FATAL ERROR:  DMA pullup msg problem  ");
              }
#endif
           pdma_info->prxdma_res[i].flags = 0;

        } /* MERC_PAGE_VALID */
     } /* for */

     pdma_info->rx_dma_count = 0;
     padapter->flags.DPCDmaPending = 0;

     // Per-Compaq Request: the following is moved here to
     // cover spurrious interrupts (back - to - back)
     for(i=0; i < padapter->phw_info->pdma_info->rx_mbuf_count; i++) {
         if (padapter->phw_info->pdma_info->rxmbufdma[i] != 0) {
             PSTRM_MSG       Msg = NULL;
             Msg = (PSTRM_MSG) padapter->phw_info->pdma_info->rxmbufdma[i];
             
              //The following is an utility function, that can
              //dump the bytes and nibbles in each byte, given
              //the address. Good for debug purpose
	      //i_bprintf(Msg->b_cont->b_rptr, 24);
             fwmgr_msg_reply_mgr(Msg, padapter);
         }

     }

    padapter->phw_info->pdma_info->rx_mbuf_count = 0;
    padapter->flags.DMAIntrReceived = 0;
   

   } /* MERC_ADAPTER_FLAG_RCV_DMA_INTR */
  else 
   { /* This is a Send DMA interrupt */
     for(i=0; i < pdma_info->tx_dma_count; i++) 
      {
        //If there are some descriptors done DMAing on the
        //send side, time to free them..
        if (pdma_info->ptxdma_res[i].flags & MERC_PAGE_VALID) 
         {
             if(pdma_info->ptxdma_res[i].flags & MERC_PAGE_END ) {
                MSD_FREE_MESSAGE(pdma_info->ptxdma_res[i].dma_mb);
             } else
                pdma_info->ptxdma_res[i].dma_mb = 0;
             pdma_info->ptxdma_res[i].flags = 0;
         }  
        pdma_info->tx_dma_count = 0;
       } /* for */
     padapter->flags.SndDmaPending &= ~MERC_ADAPTER_FLAG_SND_DMA_PENDING;
   } /* if */

    returnStat = MD_SUCCESS;
    return returnStat;

}
