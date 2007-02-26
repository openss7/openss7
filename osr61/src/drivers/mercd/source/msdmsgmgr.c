/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdmsgmgr.c
 * Description			: Message Manager functions 
 *
 *
 ***********************************************************************/

#include "msd.h"
#define _MSDMSGMGR_C_
#include "msdextern.h"
#undef _MSDMSGMGR_C_


/***************************************************************************
 * Function Name		: mid_msgmgr_pam_send_mercury
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:
 ****************************************************************************/
void mid_msgmgr_pam_send_mercury(PMSD_OPEN_BLOCK MsdOpenBlock, PSTRM_MSG Msg)
{
        PMDRV_MSG                 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        PMERC_HOSTIF_MSG          pHdr;
#ifndef LFS
        merc_uint_t               slotn = 0;
        merc_uint_t               i  = 0;
#endif
#ifdef _8_BIT_INSTANCE
        MSD_HANDLE                BindHandle = 0;
#else
        MBD_HANDLE                BindHandle = 0;
#endif
        merc_uint_t               ErrorCode = MD_OK;
        merc_uint_t               ReturnValue = 0;
        merc_uint_t               AdapterNumber;
        pmercd_adapter_block_sT   padapter;
        merc_uint_t               Result;
        pmercd_bind_block_sT      BindBlock;
        mercd_ww_dmaDscr_Index_sT dmaDescSt;


        // Decode the message class and send it to the destination
	if ((!MsdOpenBlock) || (!Msg)) {
	    printk("MsdDownMessage: null message\n");
            ErrorCode = CD_ERR_BAD_PARAMETER;
            goto out2;
        }

        // use the message's destination address to decode the 
	// board number validate both the control and data message sizes
        if (((Msg->b_wptr-Msg->b_rptr) != sizeof(MDRV_MSG)) ||
				      (Msg->b_cont == NULL)) {
            printk("MsdDownMessage: Bad MDRV_MSG Msg Size or no data\n");
            ErrorCode = CD_ERR_BAD_PARAMETER;
            goto out2;
        }

        if (MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr))>2048) {
            printk("MsdDownMessage: Bad Mercury Msg Body Size. %d \n",
               MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr)));
            ErrorCode = CD_ERR_BAD_PARAMETER;
            goto out2;
        }

        // validate data portion size
        if ((Msg->b_cont->b_wptr-Msg->b_cont->b_rptr) !=sizeof(MERC_HOSTIF_MSG)+
            MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr))) {
#ifdef LFS
            printk("MsdDownMessage: Bad Mercury Msg Body Size %zd %zu\n", 
 	       Msg->b_cont->b_wptr-Msg->b_cont->b_rptr, sizeof(MERC_HOSTIF_MSG)+
               MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG) Msg->b_cont->b_rptr)));
#else
            printk("MsdDownMessage: Bad Mercury Msg Body Size %d %d\n", 
 	       Msg->b_cont->b_wptr-Msg->b_cont->b_rptr, sizeof(MERC_HOSTIF_MSG)+
               MERCURY_GET_BODY_SIZE(((PMERC_HOSTIF_MSG) Msg->b_cont->b_rptr)));
#endif
            ErrorCode = CD_ERR_BAD_PARAMETER;
            goto out2;
        }

	// gat a pointer to the data message (useful for error debug info)
	pHdr = ((PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));

        // Get the board number from the mercury message header
        AdapterNumber = MERCURY_GET_DEST_BOARD((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr);

        // Check the AdapterNumber if it is zero that meens its a loopback
        // message. It is no longer supported, so send an error for this msg
        if (AdapterNumber == 0) { 
            printk("MsdDownMessage: Loopback message is not supported\n ");
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = AdapterNumber;
            goto out2;
        }

        // Do to Msg Hdr corruption we see weird board numbers, which are being 
	// validated to be good. That is because we only validate if the 
	// AdapterNumber<MsdControlBlock->maxadapters which is 255. What if we 
	// get a board id, lets say: 110. We still say it is OK? But, the table 
	// mercd_adapter_log_to_phy_map_table has only [MSD_MAX_ADAPTER_COUNT+1]
        // where MSD_MAX_ADAPTER_COUNT == MSD_MAX_BOARD_ID_COUNT
        if ((AdapterNumber > MSD_MAX_ADAPTER_COUNT) || 
	    (AdapterNumber > MsdControlBlock->maxadapters)) {
            printk("MsdDownMessage: adapter number too large %d\n", AdapterNumber);
	    printk("TransId %#x, msg %#x, Src %#x, Dest %#x, SD %#x, Sz %#x\n",
               pHdr->FlagTransactionId,pHdr->MessageType,pHdr->Source, 
	       pHdr->Destination, pHdr->SourceDestComponent, pHdr->BodySize);
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = AdapterNumber;
            goto out2;
        }

        dmaDescSt.index = AdapterNumber;
        AdapterNumber = mercd_adapter_log_to_phy_map_table[AdapterNumber];

        if (AdapterNumber == 0xFF) {
            printk("MsdDownMessage: Null adapter - may not be configured %d\n", AdapterNumber);
	    printk("TransId %#x, msg %#x, Src %#x, Dest %#x, SD %#x, Sz %#x\n",
               pHdr->FlagTransactionId,pHdr->MessageType,pHdr->Source, 
	       pHdr->Destination, pHdr->SourceDestComponent, pHdr->BodySize);
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = AdapterNumber;
            goto out2;
        }

        // use the padapter to find the PAM id
        padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

        if (padapter == NULL) {
            printk("MsdDownMessage: Null padapter - may not be configured %d\n", AdapterNumber);
	    printk("TransId %#x, msg %#x, Src %#x, Dest %#x, SD %#x, Sz %#x\n",
               pHdr->FlagTransactionId,pHdr->MessageType,pHdr->Source, 
	       pHdr->Destination, pHdr->SourceDestComponent, pHdr->BodySize);
            ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
            ReturnValue = AdapterNumber;
            goto out2;
        }

        MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

        if (((padapter->state != MERCD_ADAPTER_STATE_READY)     &&
            (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED) &&
            (padapter->state != MERCD_ADAPTER_STATE_STARTED)    &&
            (padapter->state != MERCD_ADAPTER_STATE_CP_STARTED))||
            (padapter->state == MERCD_ADAPTER_STATE_SUSPENDED))  {

            ErrorCode = PAM_ERR_BAD_ADAPTER_STATE;
            ReturnValue = padapter->phw_info->slot_number;
            printk("MsdDownMessage: padapter bad state=%d\n ", padapter->state);
	    printk("TransId %#x, msg %#x, Src %#x, Dest %#x, SD %#x, Sz %#x\n",
               pHdr->FlagTransactionId,pHdr->MessageType,pHdr->Source, 
	       pHdr->Destination, pHdr->SourceDestComponent, pHdr->BodySize);
            goto out;
        }

#ifdef _8_BIT_INSTANCE
         BindHandle = MERCURY_GET_SOURCE_COMPINST((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr);
         MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
#else
        ////////////////////////////////////////////////////////
        /* 16BIT Change: By default, this must be a 8 bit macro.
         * Here we will have padapter->rtkMode  which by default
         * is set to 0 for 8BIT RTK and to 1 for 16BIT RTK.
         */
        ////////////////////////////////////////////////////////

        if (padapter->rtkMode == 0) {
            /* SWAP the Header to make it 8BIT Hdr*/
#if 0
            pHdr = ((PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
            printk("MessageType: 0x%x\n", pHdr->MessageType);
            printk("AdapterNumber: %d rtkMode: %d\n", AdapterNumber, padapter->rtkMode);
            i_bprintf(pHdr, 24);
            BindHandle =(((PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr))->SourceDestComponent)
                          & (0x0000FFFF);
            if (pHdr->MessageType == 0x80011b)
               printk("This Message sends GetKernelVersion down\n");
#endif
            BindHandle = MERCURY_GET_SOURCE_COMPINST_8BITMODE((PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr);
            MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
        } else {
	    PMERC_HOSTIF_MSG FwMessage = (PMERC_HOSTIF_MSG)Msg->b_cont->b_rptr;
#if 0
             pHdr = ((PMERC_HOSTIF_MSG)(Msg->b_cont->b_rptr));
             printk("MessageType: 0x%x\n", pHdr->MessageType);
             printk("AdapterNumber: %d rtkMode: %d\n", AdapterNumber, padapter->rtkMode);
             i_bprintf(pHdr, 48);
             if (pHdr->MessageType == 0x80011b)
               printk("This Message sends GetKernelVersion down\n");
#endif
	     BindHandle = MERCURY_GET_SOURCE_COMP(FwMessage);
             BindHandle = BindHandle << 8;
             BindHandle |= MERCURY_GET_SOURCE_INST(FwMessage);
             MD_SET_MDMSG_BIND_HANDLE(MdMsg, BindHandle);
        }
#endif  // _8_BIT_INSTANCE

        // validate bind handle
        if (BindHandle > MsdControlBlock->maxbind || BindHandle == 0) {
            printk("MsdDownMessage: Source COMPINST is bad bind handle %d (max %d)\n", BindHandle, MsdControlBlock->maxbind);
	    printk("TransId %#x, msg %#x, Src %#x, Dest %#x, SD %#x, Sz %#x\n",
               pHdr->FlagTransactionId,pHdr->MessageType,pHdr->Source, 
	       pHdr->Destination, pHdr->SourceDestComponent, pHdr->BodySize);
            MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
            return;
        }

        // WW Support, THIRD_ROCK
        if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
            if ((Result=strm_Q_snd_msg(Msg, padapter)) != MD_SUCCESS) {
        	ErrorCode = PAM_ERR_BAD_ADAPTER_STATE;
                goto out;
            }
        } else {
	    // WW Mode Operation
            if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
                if (pHdr->MessageType == 0xc00001) {
		    // Dropping Start Protocol Message to Boot Kernel.
                    MSD_LEVEL1_DBGPRINT("MsdDownMessage: Drop message with type 0xc00001\n");
                    goto out;
                }
                if ((Result=strm_ww_Q_snd_msg(Msg, padapter)) != MD_SUCCESS) {
                    ErrorCode = Result;
                    printk("MsdDownMessage: strm_ww_Q_snd_msg failed. Err=%d\n", ErrorCode);
                    goto out;
                }
	     
                BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
                dmaDescSt.pBitMap = BindBlock->pWWMsgsToAdapters;
                dmaDescSt.szBitMap = BindBlock->szWWMsgsToAdapters;
                msgutl_ww_mark_dmadescr_used(&dmaDescSt);
                //i_bprintf(BindBlock->pWWMsgsToAdapters, 4);
            } else if (padapter->state == MERCD_ADAPTER_STATE_DOWNLOADED) {
                printk("MsdDownMessage: Downloaded board %d not configured properly\n", AdapterNumber);
                goto out;
            }
        }

	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	
        return;
out:
	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
out2:
        // Only Errors are retruned from here
        // if no ack is required, just return
        if (MD_IS_MSG_NO_ACK(Msg))
            return;

        // send back an ACK message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        // for async messages, send back a async reply
        if (MD_IS_MSG_ASYNC(Msg)) {
            // msg is freed in the send function
            supp_process_receive(MsdOpenBlock, Msg);
        } else {
            // sync msg uses the sync return path
            supp_process_sync_receive(MsdOpenBlock, Msg);
	}
}
