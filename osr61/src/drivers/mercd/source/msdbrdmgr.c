/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
 /**********************************************************************
 * File Name 			: msdbrdmgr.c
 * Description			: Board Manager functions
 *
 *
 ***********************************************************************/

#include "msd.h"
#include "msdpciif.h"
#define _MSDBRDMGR_C_
#include "msdextern.h"
#undef _MSDBRDMGR_C_

extern int HCS_Flag;
extern int No_Send_From_DPC;
/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_scan_brd
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			  Msg
 * Outputs		: 
 * Calling functions	: MDI 
 * Description		: Applicable for VME platform under Solaris
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_cfg_scan_brd(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_SCAN_BOARD			ScanBrdPtr;
	REGISTER PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	mercd_dhal_brd_scan_sT		brdinfo = { 0 };
	merc_uint_t			ErrorCode = MD_OK;
	merc_uint_t			ReturnValue = 0;
	merc_uint_t			MsgSize;

	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_scan_brd", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	// check the current state 
	// perform validity checking

	MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_SCAN_BOARD, "ScanBoard");

	ScanBrdPtr = (PCFG_SCAN_BOARD)MD_GET_MDMSG_PAYLOAD(MdMsg);

	brdinfo.Slot = ScanBrdPtr->Slot;
	
	(*mercd_dhal_func[MERCD_DHAL_BRD_SCAN])((void *)&brdinfo);

	if ( brdinfo.ret != MD_SUCCESS ) {
		ErrorCode = brdinfo.ret;
		ReturnValue = ScanBrdPtr->Slot;
		MSD_ERR_DBGPRINT("ERROR: Scan: Ret %x\n", brdinfo.ret);
	}

out:
	// send back an error/reply message

	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 
		supp_process_receive(MsdOpenBlock, Msg);
	else 
		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_get_phys_brd_attrib
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_cfg_get_phys_brd_attrib(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_GET_PHYS_BOARD_ATTR_ACK	PhysBrdAttrAckPtr;
	REGISTER PMDRV_MSG	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t			MsgSize;
	merc_uint_t			PhysBrdAttrAckSize;
	merc_uint_t			ErrorCode = MD_OK;
	merc_uint_t			ReturnValue = 0;

	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_get_phys_brd_attrib", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	// perform validity checking
	PhysBrdAttrAckSize = sizeof(CFG_GET_PHYS_BOARD_ATTR_ACK) +
			(MSD_MAX_BOARD_ID_COUNT*sizeof(MD_PHYS_BOARD_ATTR));

	if (MsgSize >= PhysBrdAttrAckSize) {

                MSD_LEVEL2_DBGPRINT("MsgSize: %d, PhysBrdAttrAckSize: %d\n", 
                                                  MsgSize, PhysBrdAttrAckSize);
		PhysBrdAttrAckPtr = (PCFG_GET_PHYS_BOARD_ATTR_ACK)

		MD_GET_MDMSG_PAYLOAD(MdMsg);

		fill_attr(PhysBrdAttrAckPtr);

		// just change the msg id and send the msg back
		MdMsg->MessageId = MID_CFG_GET_PHYS_BOARD_ATTR_ACK;

		// return the message to KAW via sync path
		supp_process_sync_receive(MsdOpenBlock, Msg);

		return;
	}
	/*
 	 * Error path 
	 */
	MSD_ERR_DBGPRINT("PhysAttr: too small message, %d < %d\n",
				MsgSize, PhysBrdAttrAckSize);
	ErrorCode = CD_ERR_BAD_MSG_SIZE;
	ReturnValue = MsgSize;
#ifndef LFS
out:
#endif
	// send back an error/reply message

	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 
		supp_process_receive(MsdOpenBlock, Msg);
	else 
		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
}

#ifdef LFS
void mid_print_adapter_info(void)
#else
void mid_print_adapter_info()
#endif
{

   int cnt, BoardNumber, brdcount;
   pmercd_adapter_block_sT		padapter;

   brdcount = MsdControlBlock->adapter_count;

   for (cnt=0; (cnt<brdcount) && (cnt<MSD_MAX_BOARD_ID_COUNT);cnt++) {
      BoardNumber = mercd_adapter_map[cnt];
      if( BoardNumber == 0xFF ) continue;
      padapter = MsdControlBlock->padapter_block_list[BoardNumber];
      printk("Board %d : Snd Flow Count %d/%d Rcv Flow Count %d \n",
                    cnt, 
                    padapter->sndflowcount, 
                    padapter->sndflowcount1, 
                    padapter->rcvflowcount);
   }
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_get_brd_state
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	:
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_cfg_get_brd_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_GET_BOARD_STATE_ACK    BrdStateAckPtr;
	PMD_BOARD_STATE             BrdStatePtr;
	pmercd_adapter_block_sT     padapter;
	REGISTER PMDRV_MSG	    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t	            ErrorCode = MD_OK;
	merc_uint_t	            ReturnValue = 0;
	merc_uint_t	            MsgSize;
	merc_uint_t	            BrdStateAckSize;
	merc_ushort_t		    cnt,search_count;

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	BrdStateAckSize = sizeof(CFG_GET_BOARD_STATE_ACK) +
			(MSD_MAX_BOARD_ID_COUNT*sizeof(MD_BOARD_STATE));

	if (MsgSize < BrdStateAckSize) {
		MSD_ERR_DBGPRINT("PhysAttr: too small message, %d < %d\n", MsgSize, BrdStateAckSize);
		ErrorCode = CD_ERR_BAD_MSG_SIZE;
		ReturnValue = MsgSize;
		goto out;
	}

	BrdStateAckPtr = (PCFG_GET_BOARD_STATE_ACK)
	MD_GET_MDMSG_PAYLOAD(MdMsg);

	BrdStateAckPtr->BoardCount = MsdControlBlock->adapter_count;
	BrdStatePtr = (PMD_BOARD_STATE)(BrdStateAckPtr + 1);

	for (cnt=0; (cnt<BrdStateAckPtr->BoardCount) && (cnt<MSD_MAX_BOARD_ID_COUNT);cnt++, BrdStatePtr++) {
	     BrdStatePtr->ConfigId = cnt;
	     BrdStatePtr->BoardNumber = mercd_adapter_map[cnt];
		
	     if (BrdStatePtr->BoardNumber == 0xFF) {
                 MSD_LEVEL2_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_state Brd Num: 0x%x\n", BrdStatePtr->BoardNumber);
                 BrdStatePtr->BoardNumber = -1;
	         continue;
	     }

	     padapter = MsdControlBlock->padapter_block_list[BrdStatePtr->BoardNumber];
 
             for (search_count = 0 ; search_count < MSD_MAX_BOARD_ID_COUNT; search_count++) {
                 if (mercd_adapter_log_to_phy_map_table[search_count] == mercd_adapter_map[cnt] ) {
                     BrdStatePtr->BoardNumber = search_count;
                     break;
                 }
             }

	     MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
	     switch (padapter->state) {
		case MERCD_ADAPTER_STATE_INIT:				
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_INIT\n");
                     BrdStatePtr->BoardState = BOARD_STATE_INITED;
                     padapter->sndflowcount = 0;
                     padapter->sndflowcount1 = 0;
                     padapter->rcvflowcount = 0;
	   	     break;
			
		case MERCD_ADAPTER_STATE_CONFIGED:		
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_CONFIGED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_CONFIGED;
	 	     break;
			
		case MERCD_ADAPTER_STATE_READY:		
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_READY\n");
                     BrdStatePtr->BoardState = BOARD_STATE_MAPPED;
		     break;
	
		case MERCD_ADAPTER_STATE_MAPPED:	 
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_MAPPED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_MAPPED;
		     break;

		case MERCD_ADAPTER_STATE_STARTED:		
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_STARTED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_STARTED;
		     break;
	
		case MERCD_ADAPTER_STATE_CP_STARTED:
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_STARTED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_STARTED;
                     break;

		case MERCD_ADAPTER_STATE_DOWNLOADED:	
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_DOWNLOADED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_DOWNLOADED;
		     break;

		case MERCD_ADAPTER_STATE_STOPPED:		
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_STOPPED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_STOPPED;
		     break;

		case MERCD_ADAPTER_STATE_OUT_OF_SERVICE:	
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_OUT_OF_SERVICE\n");
                     BrdStatePtr->BoardState = BOARD_STATE_OUT_OF_SERVICE;
		     break;

		case MERCD_ADAPTER_STATE_CRASHED:		
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_CRASHED\n");
                     BrdStatePtr->BoardState = BOARD_STATE_CRASHED;
		     break;

		case MERCD_ADAPTER_STATE_OUT_OF_SYNC:	
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_OUT_OF_SYNC\n");
                     BrdStatePtr->BoardState = BOARD_STATE_OUT_OF_SYNC;
		     break;
			
		case MERCD_ADAPTER_STATE_SUSPENDED:
                     MSD_LEVEL2_DBGPRINT("padapter->state MERCD_ADAPTER_STATE_SUSPENDED\n");
	 	     BrdStatePtr->BoardNumber = 0;
                     BrdStatePtr->BoardState = BOARD_STATE_SUSPENDED;
		     break;

		default:						
                     MSD_LEVEL2_DBGPRINT("padapter->state ADAPTER STATE INVALID\n");
                     BrdStatePtr->BoardState = BOARD_STATE_INVALID;
		     break;

	     }
	
	     if (BrdStatePtr->BoardNumber != search_count) {
                 BrdStatePtr->BoardNumber = 0xFF;
             }

             if ((padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED) &&
	         (padapter->state != MERCD_ADAPTER_STATE_STARTED) &&
                 (padapter->state != MERCD_ADAPTER_STATE_CP_STARTED)) {
                 mid_save_brd_params(padapter);
             }

	     MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	}
	
	
        // mid_print_adapter_info();

	
	// return the message to Driver via sync path
	MdMsg->MessageId = MID_CFG_GET_BOARD_STATE_ACK;
	supp_process_sync_receive(MsdOpenBlock, Msg);
	
	return;

out:
	// send back an error/reply message
	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if (MD_IS_MSG_ASYNC(Msg)) {
	    supp_process_receive(MsdOpenBlock, Msg);
	} else {
	    supp_process_sync_receive(MsdOpenBlock, Msg);
	}
	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_get_brd_diag_state
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	:
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_cfg_get_brd_diag_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PCFG_GET_BRD_DIAG_STATE         Ptr, AckPtr;
        pmercd_adapter_block_sT         padapter;
        REGISTER PMDRV_MSG              MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_int_t                      MsgSize;
        merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
        merc_uint_t                     DebugVal;
#ifndef LFS
        merc_uint_t                     BoardNumber;
#endif
        merc_ulong_t                    regOffset;
        merc_uint_t                     regValue;
	merc_uchar_t			HostRamReq;

        MSD_FUNCTION_TRACE("mid_brdmgr_cfg_get_brd_diag_state", TWO_PARAMETERS,
                        (size_t)MsdOpenBlock, (size_t)(Msg));

        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_GET_BRD_DIAG_STATE,"GetDiagState");

        Ptr = (PCFG_GET_BRD_DIAG_STATE)MD_GET_MDMSG_PAYLOAD(MdMsg);

        AckPtr = Ptr;

        AckPtr->BoardState = BOARD_STATE_CONFIGED;

        // Debug Info

#ifdef  USE_LOGICALID


        BoardNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

        if( BoardNumber == 0xFF){
        MSD_LEVEL2_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_diag_state: Ptr->BoardNumber: 0x%x\n",
                                    Ptr->BoardNumber);
                ErrorCode = CD_ERR_ADAPTER_INVALID;

        ReturnValue = Ptr->BoardNumber;
        goto out;

    	}

        padapter = MsdControlBlock->padapter_block_list[BoardNumber];

        if( padapter == NULL) {

        MSD_LEVEL2_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_diag_state: BoardNumber: 0x%x\n",
                             BoardNumber);
                MSD_LEVEL2_DBGPRINT("GET_BRD_DIAG_STATE: Invalid Adapter Number!\n");

                ErrorCode = CD_ERR_ADAPTER_INVALID;

                ReturnValue = Ptr->BoardNumber;

                goto out;
        }

#else

		if ( mercd_adapter_map[Ptr->BoardNumber] == 0xFF) {

			MdMsg->MessageId = MID_CFG_GET_POST_STATE_ACK;
		
			supp_process_sync_receive(MsdOpenBlock, Msg);

			return;
		}

		padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->BoardNumber]];

		if( padapter == NULL ) {

			ErrorCode = CD_ERR_ADAPTER_INVALID;

			goto out; 

	    }
	
#endif

        switch (padapter->phw_info->pciSubSysId)
        {
        case SUBSYSID_21554:
        case SUBSYSID_ROZETTA_21554:
                //The post location is a 32 bit scratchpad register
                regOffset = (merc_ulong_t)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr + MERCD_3RDROCK_21554_POST_LOC);
                regValue = *((volatile merc_uint_t *)(regOffset));
                AckPtr->DiagRetVal1 = (merc_uchar_t)(regValue & 0x000000ff);
                AckPtr->DiagRetVal2 = (merc_uchar_t)((regValue & 0x0000ff00) >> 8);
				if((AckPtr->DiagRetVal1 == 2) &&  (AckPtr->DiagRetVal2 == 0x7d)) {
					padapter->rtkMode = 1;
					padapter->state = MERCD_ADAPTER_STATE_CP_STARTED;
				} else
					padapter->rtkMode = 0;

                break;
      case SUBSYSID_80321:
              regOffset = (merc_ulong_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr + MERCD_3RDROCK_80321_POST_LOC;
             regValue = *((volatile merc_uint_t *)(regOffset));
              AckPtr->DiagRetVal1 = (merc_uchar_t)(regValue & 0x000000ff);
              AckPtr->DiagRetVal2 = (merc_uchar_t)((regValue & 0x0000ff00) >> 8);
	      if((AckPtr->DiagRetVal1 == 2) &&  (AckPtr->DiagRetVal2 == 0x7d))
		 padapter->rtkMode = 1;
	      else 
	         padapter->rtkMode = 0;
              break;

        default: // Then it is assumed to be the SRAM BOARD

                MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

                if(padapter->flags.SramOurs & MERC_BOARD_FLAG_SRAM_IS_OURS){

                        MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

                        /* -- UNLOCK -- */

                        MSD_LEVEL2_DBGPRINT("GET_BRD_DIAG_STATE: Sram is not ours!\n");
                        ErrorCode = CD_ERR_ADAPTER_BUSY;

                        ReturnValue = Ptr->BoardNumber;

                        goto out;
                }

		// DMV-B
	        if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       		    (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
		    MERC_GET_SRAM_LOCK((&padapter->phost_info->reg_block), HostRamReq);
		    if (!(HostRamReq & MERC_HOST_RAM_GRANT_FOR_CLR_R))
                        printk("Failed to get the sram \n");
                }

                MERC_CHECK_DEBUG_ONE((&padapter->phost_info->reg_block), DebugVal);

                AckPtr->DiagRetVal1 = (merc_uchar_t)DebugVal;

                MERC_CHECK_DEBUG_TWO((&padapter->phost_info->reg_block), DebugVal);

                AckPtr->DiagRetVal2 =  (merc_uchar_t)DebugVal;

                MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);


		if((AckPtr->DiagRetVal1 == 3) &&  (AckPtr->DiagRetVal2 == 0x7c)) {
		    padapter->rtkMode = 1;
		    padapter->state = MERCD_ADAPTER_STATE_CP_STARTED;
		} else {
		    padapter->rtkMode = 0;

		    // DMV-B
                    if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       			(padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
                        if((AckPtr->DiagRetVal1 == 2) &&  (AckPtr->DiagRetVal2 == 0x7d))
			   padapter->rtkMode = 1;
		    }
		}

		// DMV-B
                if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       		    (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
	            MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
                }
        }

        

        // just change the msg id and send the msg back
        MdMsg->MessageId = MID_CFG_GET_BRD_DIAG_STATE_ACK;

        // return the message to Driver via sync path
        supp_process_sync_receive(MsdOpenBlock, Msg);

        return;
out:
        // send back an error/reply message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
        else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_set_brd_state_dnld
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_cfg_set_brd_state_dnld(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_SET_BRD_STATE_DNLD		Ptr;
	pmercd_adapter_block_sT		padapter;	
	REGISTER PMDRV_MSG	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t			ErrorCode = MD_OK;
	merc_uint_t			ReturnValue = 0;
	merc_int_t			MsgSize;
	merc_int_t			BoardNumber;
	PMERC_HOSTIF_MSG                MercMsg;
        PSTRM_MSG                       NewMsg;
        PSTRM_MSG                       DataMsg;

	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_set_brd_state_dnld", TWO_PARAMETERS, 
			    (size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_SET_BRD_STATE_DNLD, "SetBrdStateDnld");

	Ptr = (PCFG_SET_BRD_STATE_DNLD)MD_GET_MDMSG_PAYLOAD(MdMsg);

	BoardNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

	if( BoardNumber == 0xFF){
          MSD_LEVEL2_DBGPRINT("ERROR mid_brdmgr_cfg_set_brd_state_dnld: %d\n", Ptr->BoardNumber); 

        ErrorCode = CD_ERR_ADAPTER_INVALID;

        ReturnValue = Ptr->BoardNumber;

        goto out;
   
    }
	padapter = MsdControlBlock->padapter_block_list[BoardNumber];

	if( padapter == NULL) {

		MSD_LEVEL2_DBGPRINT("GET_BRD_DIAG_STATE: Invalid Adapter Number!\n");
	
		ErrorCode = CD_ERR_ADAPTER_INVALID;
		
		ReturnValue = Ptr->BoardNumber;
		
		goto out;
	}

	MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

	// Only Ready State can goto Downloaded state
	//if (padapter->state != MERCD_ADAPTER_STATE_STARTED) {
	if (!(padapter->state & MERCD_ADAPTER_STATE_STARTED)) {
		
		MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

		MSD_LEVEL2_DBGPRINT("SET_BRD_STATE_DNLD: Previous State not READY!\n");

		ErrorCode = CD_ERR_BAD_STATE;

		ReturnValue = Ptr->BoardNumber;

		goto out;
	}

	// Send a QCNTRL_INITIALIZE
        NewMsg = supp_alloc_buf(sizeof(MDRV_MSG), GFP_ATOMIC);
        if (NewMsg) {
            DataMsg = supp_alloc_buf(sizeof(MERC_HOSTIF_MSG),GFP_ATOMIC);
            if (DataMsg) {
                MD_SET_MSG_TYPE(DataMsg, M_DATA);
                MSD_LINK_MESSAGE(NewMsg,DataMsg);
                MercMsg = (PMERC_HOSTIF_MSG)DataMsg->b_rptr;

                MERCURY_SET_BODY_SIZE(MercMsg, 0);
                MERCURY_SET_MESSAGE_FLAG(MercMsg, 0x80);
                MERCURY_SET_MESSAGE_CLASS(MercMsg,MERCURY_CLASS_DRIVER);
                MERCURY_SET_MESSAGE_TYPE(MercMsg, QCNTRL_INITIALIZE);
                MERCURY_SET_TRANSACTION_ID(MercMsg, padapter->TransactionId);
                padapter->TransactionId++;
                MERCURY_SET_DEST_NODE(MercMsg,DEFAULT_DEST_NODE);
                MERCURY_SET_DEST_PROC(MercMsg,DEFAULT_DEST_PROC);
                MERCURY_SET_DEST_COMPINST(MercMsg,DEFAULT_DEST_COMPINST);
                MERCURY_SET_DEST_BOARD(MercMsg, padapter->adapternumber);
                MERCURY_SET_SOURCE_PROC(MercMsg, DEFAULT_SOURCE_PROC);

                if (!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
                   strm_Q_snd_msg(NewMsg, padapter);
                } else {
                   if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED) {
                       strm_ww_Q_snd_msg(NewMsg, padapter);
                   }
                }
            }
        }

	// Set the Adapter State to DOWNLOADED.
	padapter->state = MERCD_ADAPTER_STATE_DOWNLOADED;

	MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

	/* -- UNLOCK -- */
	
	// just change the msg id and send the msg back
	MdMsg->MessageId = MID_CFG_SET_BRD_STATE_DNLD_ACK;

	// return the message to Driver via sync path
	supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
out:
	// send back an error/reply message

	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 

		supp_process_receive(MsdOpenBlock, Msg);

	else 

		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_map_brd
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MID
 * Description		: 
 * Additional comments	: NEED TO INVESTIGATE MORE - HR 12/19/99 Redundant
 ****************************************************************************/

void mid_brdmgr_cfg_map_brd(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_MAP_BOARD			MapBrdPtr;
	REGISTER PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t				ErrorCode = MD_OK;
	merc_uint_t				ReturnValue = 0;
	merc_int_t				MsgSize;
	

	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_map_brd", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

    // perform validity checking
    MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_MAP_BOARD, "MapBoard");

    MapBrdPtr = (PCFG_MAP_BOARD)MD_GET_MDMSG_PAYLOAD(MdMsg);

    // perform board number validity checking
    if(MapBrdPtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) {
        printk("ERROR mid_brdmgr_cfg_map_brd: %d %d\n", MapBrdPtr->BoardNumber, 
                             MD_MAX_ADAPTER_COUNT ); 
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        ReturnValue = MapBrdPtr->BoardNumber;
        goto out;
    }

    if(MapBrdPtr->ConfigId >= MSD_MAX_BOARD_ID_COUNT) {
        printk("ERROR mid_brdmgr_cfg_map_brd: %d %d\n", 
                             MapBrdPtr->ConfigId , MSD_MAX_BOARD_ID_COUNT ); 
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        ReturnValue = MapBrdPtr->ConfigId;
        goto out;
    }

    if(mercd_adapter_log_to_phy_map_table[MapBrdPtr->BoardNumber] != 0xFF )  {
        printk("ERROR mid_brdmgr_cfg_map_brd: %d\n", 
               mercd_adapter_log_to_phy_map_table[MapBrdPtr->BoardNumber]); 
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        ReturnValue = MapBrdPtr->ConfigId;
        goto out;
    }

    if (MapBrdPtr->BoardNumber == 0) {
        printk("ERROR mid_brdmgr_cfg_map_brd: %d\n", MapBrdPtr->BoardNumber); 
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        ReturnValue = MapBrdPtr->BoardNumber;
        goto out;
    }

    mercd_adapter_log_to_phy_map_table[MapBrdPtr->BoardNumber] = mercd_adapter_map[MapBrdPtr->ConfigId];

out:
    // send back an error/reply message

    Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

    if(MD_IS_MSG_ASYNC(Msg))

        supp_process_receive(MsdOpenBlock, Msg);
    else
        supp_process_sync_receive(MsdOpenBlock, Msg);

    return;

}

/***************************************************************************
 * Function Name                : mid_save_brd_params
 * Function Type                : Common Hardware function
 * Inputs                       : padapter
 * Outputs                      : None
 * Calling functions            : None
 * Description                  : Saves parameters for hotswap usage
 * Additional comments          :
 ****************************************************************************/
void mid_save_brd_params(pmercd_adapter_block_sT padapter)
{
  MSD_FUNCTION_TRACE("mid_save_brd_params", ONE_PARAMETER, 
			(size_t)padapter);

  padapter->sndflowcount = 0;
  padapter->rcvflowcount = 0;
  if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY)
    {
 	padapter->phost_info->reg_block.HostRamStart = 
		padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;

        padapter->phost_info->reg_block.ClrMsgReadyIntr =
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_MSG_READY_INTR_REG;

        padapter->phost_info->reg_block.ClrHostRamPendIntr=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_HOST_RAM_PEND_INTR_REG;

        padapter->phost_info->reg_block.ClrErrorIntr=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_ERROR_INTR_REG;

        padapter->phost_info->reg_block.SetCpIntr=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_SET_CP_INTR_REG;

        padapter->phost_info->reg_block.SetCpNmi=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_SET_CP_NMI_REG;

        padapter->phost_info->reg_block.ClrCpNmi=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_CP_NMI_REG;

        padapter->phost_info->reg_block.SetBoardReset=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_SET_BOARD_RESET_REG;

        padapter->phost_info->reg_block.ClrBoardReset=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_BOARD_RESET_REG;

        padapter->phost_info->reg_block.SetHostRamRequest=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_SET_HOST_RAM_REQUEST_REG;


        padapter->phost_info->reg_block.ClrHostRamRequest=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_HOST_RAM_REQUEST_REG;

        padapter->phost_info->reg_block.SetBootHostRamBit=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_SET_BOOT_HOST_RAM_BIT_REG;

        padapter->phost_info->reg_block.ClrBootHostRamBit=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_CLR_BOOT_HOST_RAM_BIT_REG;

        // Debug address
        padapter->phost_info->reg_block.DebugOneAddress=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_DEBUG_REGISTER_ONE;

        padapter->phost_info->reg_block.DebugTwoAddress=
                padapter->phost_info->reg_block.HostRamStart
                +MERC_DEBUG_REGISTER_TWO;

        // Now adjust the start of shared ram for VME config ROM
        padapter->phost_info->reg_block.HostRamStart =
                padapter->phost_info->reg_block.HostRamStart
                + supp_sram_start_offset(padapter->adapternumber);
     }
     else
     {
              //WWPENDING: 3rd Rock Family
        padapter->phost_info->reg_block.HostRamStart =
                padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;

       // Debug address
        padapter->phost_info->reg_block.DebugOneAddress=
                                padapter->phost_info->reg_block.HostRamStart
                +MERC_DEBUG_REGISTER_ONE;

        padapter->phost_info->reg_block.DebugTwoAddress=
                padapter->phost_info->reg_block.HostRamStart
                +MERC_DEBUG_REGISTER_TWO;

        // Now adjust the start of shared ram for VME config ROM
        padapter->phost_info->reg_block.HostRamStart =
                padapter->phost_info->reg_block.HostRamStart
                + supp_sram_start_offset(padapter->adapternumber);

     }
	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_brd_config
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	: Redundant - Must be re-written for HotSwap Support
 ****************************************************************************/

void mid_brdmgr_cfg_brd_config(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PCFG_BOARD_CONFIG		CfgBrdPtr;
	pmercd_adapter_block_sT	padapter;
	REGISTER PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t				ErrorCode = MD_OK;
	merc_uint_t				ReturnValue = 0;
	merc_int_t				MsgSize;
	merc_int_t				BoardNumber;
	
	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_brd_config", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	// perform validity checking
	MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_BOARD_CONFIG, "BoardConfig");

	// perform validity checking
	CfgBrdPtr = (PCFG_BOARD_CONFIG)MD_GET_MDMSG_PAYLOAD(MdMsg);

	if(CfgBrdPtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) {
		MSD_ERR_DBGPRINT("BoardConfig: Board number (%d) is > MAX (%d).\n",
					CfgBrdPtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
		ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
		ReturnValue = CfgBrdPtr->BoardNumber;
		goto out;
	}

	BoardNumber = mercd_adapter_log_to_phy_map_table[CfgBrdPtr->BoardNumber];

	if( BoardNumber == 0xFF){

        MSD_LEVEL2_DBGPRINT("ERROR mid_brdmgr_cfg_brd_config: BoardNumber %d\n", 
                             BoardNumber); 

        ErrorCode = CD_ERR_ADAPTER_INVALID;

        ReturnValue = CfgBrdPtr->BoardNumber;

        goto out;

    }

	padapter = MsdControlBlock->padapter_block_list[BoardNumber];

	if( padapter == NULL) {

		MSD_LEVEL2_DBGPRINT("mid_brdmgr_cfg_brd_config: Invalid Adapter Number!\n");
	
		ErrorCode = CD_ERR_ADAPTER_INVALID;
		
		ReturnValue = CfgBrdPtr->BoardNumber;
		
		goto out;
	}

	padapter->state = MERCD_ADAPTER_STATE_CONFIGED;

#ifndef PPS
        mid_save_brd_params(padapter);

	if(check_brd_exist(padapter) != MD_SUCCESS) {
                ErrorCode = PAM_ERR_NO_BOARD;
                ReturnValue = CfgBrdPtr->BoardNumber;  /* padapter->AdapterNumber; */
                MSD_ERR_DBGPRINT( "BoardConfig: Board check failed\n");
                goto out;
	}

        mid_save_brd_params(padapter);
#endif /* PPS */
 
	// Reset the board
    	reset_adapter(padapter);

out:
	// send back an error/reply message
	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 
		supp_process_receive(MsdOpenBlock, Msg);
	else 
		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_pam_brd_start
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	:
 * Description		:
 * Additional comments	:
 ****************************************************************************/

void mid_brdmgr_pam_brd_start(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PPAM_BOARD_START			BrdStartPtr;
	PMERCURY_HOST_IF_CONFIG			ConfigPtr;
	pmercd_adapter_block_sT			padapter;
	REGISTER PMDRV_MSG			MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	merc_uint_t				ErrorCode = MD_OK;
	merc_uint_t				ReturnValue = 0;
	merc_uint_t				AdapterNumber;
	merc_uint_t				Result;
	merc_int_t				MsgSize;
	


	MSD_FUNCTION_TRACE("mid_brdmgr_brd_start", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	BrdStartPtr = (PPAM_BOARD_START)MD_GET_MDMSG_PAYLOAD(MdMsg);
	
	AdapterNumber = BrdStartPtr->BoardNumber;

	AdapterNumber = mercd_adapter_log_to_phy_map_table[BrdStartPtr->BoardNumber];

	if(AdapterNumber == 0xFF) {

        MSD_ERR_DBGPRINT("mid_brdmgr_pam_brd_start: Invalid Adapter Number %d\n", AdapterNumber);

        ErrorCode = CD_ERR_ADAPTER_INVALID;

        ReturnValue = BrdStartPtr->BoardNumber;

        goto out;

	}
	
	padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

	if( padapter == NULL) {

		MSD_ERR_DBGPRINT("mid_brdmgr_pam_brd_start: Invalid Adapter Number!\n");
	
		ErrorCode = CD_ERR_ADAPTER_INVALID;
		
		ReturnValue = BrdStartPtr->BoardNumber;
		
		goto out;
	}

	/* -- LOCK -- */

	MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

	// use the AdapterBlock to find the AdapterBlock
	
	if(!(padapter->state & MERCD_ADAPTER_STATE_READY) &&
	   !(padapter->state & MERCD_ADAPTER_STATE_INIT) ) {
		ErrorCode = PAM_ERR_BAD_ADAPTER_STATE;
		ReturnValue = AdapterNumber;
		MSD_ERR_DBGPRINT( "BoardStart: AdapterBlock bad state=%x ",
		padapter->state);
		MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
		goto out;
	}

	MSD_RETURN_MSG_SIZE_CHECK(MsgSize, PAM_BOARD_START, "BoardStart");

	// Config block is right after the PAM_BOARD_START
	ConfigPtr = (PMERCURY_HOST_IF_CONFIG)((PPAM_BOARD_START)(BrdStartPtr+1));

       //////////////////SRAMDMA BEGIN//////////////////////////////////////////
	   //*****    Please notice that we are stealing memory from board by artificially  
	   //  increasing the size of MERCURY_HOST_IF_CONFIG structure.   
		
	 if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
	    ConfigPtr->hostConfigSize = sizeof(MERCURY_HOST_IF_CONFIG) +
                                   (((MAX_MERC_RX_DMA_CHAINS_ON_BOARD) +
                                    (MAX_MERC_TX_DMA_CHAINS_ON_BOARD)) * 
                                    (sizeof(mercd_dma_sgt_chain_node)));
	}

       //////////////////SRAMDMA END//////////////////////////////////////////

	// save the config block in Pam Adapter
	MsdCopyMemory((pmerc_char_t)ConfigPtr, (pmerc_char_t)&padapter->phost_info->host_config,
				sizeof(MERCURY_HOST_IF_CONFIG));
	
	// HCS Support
        if (HCS_Flag) {
	    if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) || 
	        (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
	    	padapter->phost_info->host_config.protocolRev = MERCURY_HOST_IF_HCS;
	    }
     	}

	// disable the flag until we know FW supports HCS
	padapter->isHCSSupported = 0;

	// Unlock the mutex for context switch be Fair ;->
	MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

#ifdef DRVR_STATISTICS
	padapter->streams_count = 0;
	padapter->fw_overrun_msg = 0;
	padapter->fw_overrun_data = 0;
	padapter->fw_no_sram_lock = 0;
#endif

	// call board start function to download config block to shared ram
	if((Result= start_brd(MsdOpenBlock, padapter, Msg)) != MD_SUCCESS){
		ErrorCode = Result;
		MSD_ERR_DBGPRINT("BoardStart: hw_start_brd failed. Error=%d\n" , ErrorCode);
		goto out;
	}

	/* -- LOCK -- */

	MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

	// must save the MsdOpenBlock so when the ack is rcvd
	// from FW, we know whom to send it to

	padapter->pcfg_open_block = (pmercd_open_block_sT)MsdOpenBlock;

	// Set a default value if the scd file is corrupted
	if((BrdStartPtr->PamTimeoutInterval < 100) || (BrdStartPtr->PamTimeoutInterval > 20))
		padapter->phw_info->timer_info->snd_timeout_interval = BrdStartPtr->PamTimeoutInterval;
	else  // We need to set a default value is the start_brd
	     padapter->phw_info->timer_info->snd_timeout_interval = 50;


	// for higher density memory limitation can be a problem
	// to counter we do NOT send messages from DPC but to 
	// compensate latency we may use 30 mSec timer and overlook
	// value send by pyramid.scd
	if (No_Send_From_DPC) {
	   if (BrdStartPtr->PamTimeoutInterval > 30)
	       padapter->phw_info->timer_info->snd_timeout_interval = 30;
	   else padapter->phw_info->timer_info->snd_timeout_interval = BrdStartPtr->PamTimeoutInterval;
	}
	     	
	MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

        // DMV-B
        if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DMVB) &&
	    (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_NEWB)) {
	    mercd_streams_free(Msg,MsgSize, MERCD_FORCE);
	}

	 //WW : WereWolf  Changes...... 

        if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY)
         {
            if (supp_snd_cfg_rply(padapter, MERC_HOSTIF_CFG_OK) != MD_SUCCESS)
            {
               printk("mid_brdmgr_pam_brd_start: SendConfigReply failed.\n");
              return;
            }
         }
	return;

out:

	// send back an error/reply message
	
	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 

		supp_process_receive(MsdOpenBlock, Msg);

	else 

		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;

}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_brd_shutdown
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	:
 * Algorithm:
 * 
 *  1. Look in the Adapter Block for any pending streams or data 
 *  2. If the shutdown is forced, free all the pending blocks and set the adapter to READY state
 *  3. If not forced, send back a busy if the user and schedule a timeout if one is not pending already.
 *	4. Set the Adapter State to SHUTDOWN_PROGRESS or READY
 ****************************************************************************/
void mid_brdmgr_cfg_brd_shutdown(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{ 
   PCFG_BOARD_SHUTDOWN		ShutBrdPtr;
   pmercd_adapter_block_sT	padapter;
   PSTRM_MSG				StrmMsg;
   REGISTER PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   merc_int_t				cnt;
   merc_int_t				MsgSize;
   merc_int_t				Result;
   merc_uint_t				ErrorCode = MD_OK;
   merc_uint_t				ReturnValue = 0;
   merc_uint_t				Flag = 0;
   merc_uint_t				AdapterNumber;
#ifndef LFS
   merc_uchar_t				Data;
   merc_char_t				hscsr;
   mercd_dhal_intr_disable_sT intr_disableinfo = { 0 };
#endif


   MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

   MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_BOARD_SHUTDOWN, "BoardShudown");

   ShutBrdPtr = (PCFG_BOARD_SHUTDOWN)MD_GET_MDMSG_PAYLOAD(MdMsg);

   // perform validity checking
   if (ShutBrdPtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) {
       MSD_ERR_DBGPRINT("BoardShutdown: Bad board number, %d >= %d\n",
				ShutBrdPtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
		
       ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
       ReturnValue = ShutBrdPtr->BoardNumber;
       goto out;
   }

   Flag = ShutBrdPtr->Flag;

   AdapterNumber =  mercd_adapter_log_to_phy_map_table[ShutBrdPtr->BoardNumber];

   if (AdapterNumber == 0xFF) {
       MSD_ERR_DBGPRINT("BoardShutdown: Invalid Adapter Number %d\n", AdapterNumber);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = ShutBrdPtr->BoardNumber;
       goto out;
   }	

   padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

   if (padapter == NULL) {
       MSD_LEVEL2_DBGPRINT("BoardShutdown: Invalid Adapter Number!\n");
       ErrorCode = CD_ERR_ADAPTER_INVALID; 
       ReturnValue = ShutBrdPtr->BoardNumber;
       goto out;
   }

   /* -- LOCK -- */
   MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

   // Traverse the event table to see if anyone's on this board
   switch (Flag) {
      case MID_NORMAL_SHUTDOWN:
           MSD_LEVEL2_DBGPRINT("Normal Shutdown for bd: %d\n", ShutBrdPtr->BoardNumber);

	   for (cnt = 0; cnt< MSD_MAX_STREAM_ID; ++cnt)	{
 	        if (padapter->pstream_connection_list[cnt] != NULL) {
                    MSD_LEVEL2_DBGPRINT("Active Open Streams\n");
	            ErrorCode = CD_ERR_ADAPTER_BUSY;
  		    MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
		    /* -- UNLOCK -- */
 		    goto out;
		}
           }
	
           // for SRAM boards - use queue and for WW use array	
	   if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	       (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
	       if (!MSD_QUEUE_EMPTY(&padapter->snd_msg_queue)|| (padapter->snd_data_queue)) {
                   MSD_LEVEL2_DBGPRINT("Msg or Data Q is NULL\n");
		   ErrorCode = CD_ERR_ADAPTER_BUSY;
		   MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	           /* -- UNLOCK -- */
	           goto out;
	       }
           } else {
	       if (!MSD_ARRAY_EMPTY(padapter->snd_msg_Array)) {
                   MSD_LEVEL2_DBGPRINT("Msg or Data Array is NULL\n");
	           ErrorCode = CD_ERR_ADAPTER_BUSY;
		   MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
		   goto out;
	       }
           }
	
           // board is clean, do the shutdown
	   MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	   /* -- UNLOCK -- */
			
	   if ((Result = shutdown_adapter(Msg, padapter)) == MD_FAILURE) {
	        ErrorCode = Result;
		ReturnValue = ShutBrdPtr->BoardNumber;
		MSD_ERR_DBGPRINT("BoardShutdown: shutdown_adapter failed! Error=%d\n", ErrorCode);
		goto out;
	   }

           if (Result == MD_SUCCESS) {
	       MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
	       padapter->state = MERCD_ADAPTER_STATE_INIT;
	       MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
           }

	   break;

      case MID_FORCED_SHUTDOWN:
	   MSD_LEVEL2_DBGPRINT("Doing Forced Shutdown of Board %d.\n", ShutBrdPtr->BoardNumber);

           if (!(padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY)) {
	       for (cnt = 0; cnt< MSD_MAX_STREAM_ID; ++cnt) {
 		    pmercd_stream_connection_sT StreamBlock;
					
		   if ((StreamBlock = padapter->pstream_connection_list[cnt]) != NULL) {
		       strm_free_srm_msgs(padapter, StreamBlock);
		       
                       MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
        	       queue_remove_from_Q(&padapter->stream_adapter_list, (PMSD_GEN_MSG)StreamBlock);
        	       padapter->pstream_connection_list[cnt] = NULL;
		       MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
					
		      if (StreamBlock->pbind_block)
			  StreamBlock->pbind_block->stream_connection_ptr = NULL;

        	      // free the stream block memory space
        	      mercd_free(StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE); 
		   }
	       }

	       if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	           (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
		   if ((StrmMsg = queue_get_msg_Q(&padapter->snd_msg_queue)) != NULL) 
		       MSD_FREE_MESSAGE(StrmMsg);
		   } else {
		       if ((StrmMsg = queue_get_msg_Array(&padapter->snd_msg_Array, MSD_SENDER)) != NULL) {
		   	   MSD_FREE_MESSAGE(StrmMsg);
			   queue_commit_msg_Array(&padapter->snd_msg_Array, MSD_SENDER);
                       }
                   }


           } /* MERCD_ADAPTER_WW_I20_MESSAGING_READY */
	   
           MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	   /* -- UNLOCK -- */
			
	   if ((Result = shutdown_adapter(Msg, padapter)) == MD_FAILURE){
	       ErrorCode = Result;
	       ReturnValue = ShutBrdPtr->BoardNumber;
	       MSD_ERR_DBGPRINT("BoardShutdown:hw_shutdown_adapter failed. Error=%d\n", ErrorCode);
	       goto out;
	   }

           if (Result == MD_SUCCESS) {
   	       MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
 	       padapter->state = MERCD_ADAPTER_STATE_INIT;
  	       MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
           }
	
 	   break;
   }

   mercd_adapter_log_to_phy_map_table[ShutBrdPtr->BoardNumber] = 0xFF;

out:
   // send back an error/reply message
   Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

   if (MD_IS_MSG_ASYNC(Msg)) 
       supp_process_receive(MsdOpenBlock, Msg);
   else 
       supp_process_sync_receive(MsdOpenBlock, Msg);

   return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_cfg_get_config_rom
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	:
 ****************************************************************************/
void mid_brdmgr_cfg_get_config_rom(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	REGISTER PMDRV_MSG			MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PCFG_GET_CONFIG_ROM			Ptr, AckPtr;
	pmercd_adapter_block_sT		padapter;
	merc_uint_t					ErrorCode = MD_OK;
	merc_uint_t					ReturnValue = 0;
	merc_int_t					MsgSize;

	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_get_config_rom", TWO_PARAMETERS, 
			(size_t)MsdOpenBlock, (size_t)(Msg));

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_GET_CONFIG_ROM, "GetConfigRom");

	Ptr = (PCFG_GET_CONFIG_ROM)MD_GET_MDMSG_PAYLOAD(MdMsg);

	AckPtr = Ptr;

	// Copy the Config ROM to the Ack message

        if ( mercd_adapter_map[Ptr->ConfigId] == 0xFF) {
           // ErrorCode = CD_ERR_ADAPTER_INVALID; // Error code for board taken out

           // just change the msg id and send the msg back
           MdMsg->MessageId = MID_CFG_GET_CONFIG_ROM_ACK;

           // return the message to user via sync path
           supp_process_sync_receive(MsdOpenBlock, Msg);

           return;
        }

	padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->ConfigId]];

	if( padapter == NULL ) {
		
                MSD_LEVEL2_DBGPRINT("mid_brdmgr_cfg_get_config_rom: ConfigId: %d\n",
                                    Ptr->ConfigId);
 
		ErrorCode = CD_ERR_ADAPTER_INVALID;

		goto out;
	}

        // Config Rom for Third Rock read after the post was completed
        // accessing here of the config rom will cause NMI
        if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
            padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->start_offset = MERC_READ_CONFIG_ROM_TRI_BYTES(
		   padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr + MERC_CR_OFFSET_SRAM_START);

            MsdRegReadUcharBuffer((pmerc_char_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr,
                                (pmerc_char_t)padapter->phost_info->merc_config_rom, MSD_CONFIG_ROM_MAX_SIZE);
        }

	bcopy((pmerc_char_t)(padapter->phost_info->merc_config_rom),
	      (pmerc_char_t)(AckPtr->ConfigRom), 
	      MSD_CONFIG_ROM_MAX_SIZE);
	
	// just change the msg id and send the msg back
	MdMsg->MessageId = MID_CFG_GET_CONFIG_ROM_ACK;

	// return the message to user via sync path
	supp_process_sync_receive(MsdOpenBlock, Msg);

	return;

out:
	// send back an error/reply message
	
	Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

	if(MD_IS_MSG_ASYNC(Msg)) 
		supp_process_receive(MsdOpenBlock, Msg);

	else 
		supp_process_sync_receive(MsdOpenBlock, Msg);

	return;
}

/***************************************************************************
 * Function Name	: mid_brdmgr_get_param
 * Function Type	: manager function
 * Inputs		: MsdOpenBlock,
 *			Msg
 * Outputs		: none
 * Calling functions	: MDI
 * Description		:
 * Additional comments	:
 ****************************************************************************/
void mid_brdmgr_get_param(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	pmercd_adapter_block_sT	padapter;
	PMDRV_MSG				MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PCFG_GET_PARAMETER		Ptr;
	PCFG_GET_PARAMETER_ACK	AckPtr;
	PCFG_PARAMETER_ENTRY	EntryPtr;
	merc_uint_t				ErrorCode = MD_OK;
	merc_int_t				i;

	MSD_FUNCTION_TRACE("mid_brdmgr_get_param", TWO_PARAMETERS, (size_t)MsdOpenBlock, (size_t)Msg);

	Ptr = (PCFG_GET_PARAMETER)MD_GET_MDMSG_PAYLOAD(MdMsg); 
	AckPtr = (PCFG_GET_PARAMETER_ACK)Ptr;
	AckPtr->ErrorCode = MD_OK;

	// user's count must be within correct limit
	if(Ptr->Count > MD_MAX_GET_PARAM_COUNT){
                MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_param: Count: %d Max: %d\n", 
                                     Ptr->Count, MD_MAX_GET_PARAM_COUNT );
		AckPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
		goto out;
	}

	// point to the first entry
	EntryPtr = (PCFG_PARAMETER_ENTRY)(Ptr+1);

        MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_param: Key: %d\n",
                             EntryPtr->Key);
 
	for(i=0; i<Ptr->Count; i++) {
		switch(EntryPtr->Key) { 
		case CFG_KEY_DRIVER_VERSION:
		{
			EntryPtr->ErrorCode = MD_OK;

			if(MsdStringLength(MsdDriverVersion) > MD_MAX_PARAMETER_CHAR_VALUE) {

                                MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_param: len: %d\n", 
                                                     MsdStringLength(MsdDriverVersion));
				EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;

				break;
			}

			MsdStringCopy(MsdDriverVersion,EntryPtr->Value.CharValue);
			break;
		}
		case CFG_KEY_BLOCK_SIZE:
		{
			EntryPtr->ErrorCode = MD_OK;
			
			EntryPtr->Value.UlongValue[1] = MERCURY_HOST_IF_BLK_SIZE;
			
			break;
		}
		case CFG_KEY_PROTO_VERSION:
		case CFG_KEY_HWINT_INTERVAL:
		case CFG_KEY_DBLOCK_LIMIT:
		case CFG_KEY_EBLOCK_LIMIT:
		case CFG_KEY_VMEINT_LEVEL:
		case CFG_KEY_VMEINT_VECTOR:
		{
			merc_uint_t	AdapterNumber=EntryPtr->Value.UlongValue[0];
			
			EntryPtr->ErrorCode = MD_OK;
			
			if(AdapterNumber >= MSD_MAX_ADAPTER_COUNT) {
                                MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_param: AdapterNumber: %d\n",
                                                     AdapterNumber );
				EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
				break;
			}

			padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

			if( padapter == NULL ) {

                          MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_param:  NULL AdapterNumber: %d\n", 
                                               AdapterNumber);
				
	           	  ErrorCode = CD_ERR_ADAPTER_INVALID;

		   	   goto out;
			}
			
			/* -- LOCK -- */

			MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

			if((padapter->state != MERCD_ADAPTER_STATE_STARTED) &&
				(padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED)) {

				MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

				/* -- UNLOCK -- */

				MSD_ERR_DBGPRINT("ProtoVersion: State Problem. state: %d\n",
                                                  padapter->state);

				EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;

				break;
			}

			if( EntryPtr->Key == CFG_KEY_PROTO_VERSION)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_PROTO_VERSION(padapter);
			else if( EntryPtr->Key == CFG_KEY_HWINT_INTERVAL)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_HWINT_INTERVAL(padapter);
			else if( EntryPtr->Key == CFG_KEY_DBLOCK_LIMIT)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_DBLOCK_LIMT(padapter);
			else if( EntryPtr->Key == CFG_KEY_EBLOCK_LIMIT)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_EBLOCK_LIMT(padapter);
			else if( EntryPtr->Key == CFG_KEY_VMEINT_LEVEL)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_MERCINT_LEVEL(padapter);
			else if( EntryPtr->Key == CFG_KEY_VMEINT_VECTOR)
				EntryPtr->Value.UlongValue[1] = MD_GET_ADAPTER_MERCINT_VECTOR(padapter);

			MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

			/* -- UNLOCK -- */

			break;
		
		}
		case CFG_KEY_MAX_BIND:
		case CFG_KEY_MAX_STRMS:
		{
			EntryPtr->ErrorCode = MD_OK;
			
			if( EntryPtr->Key == CFG_KEY_MAX_BIND)
				EntryPtr->Value.UlongValue[1] = MsdControlBlock->maxbind;
			else if( EntryPtr->Key == CFG_KEY_MAX_STRMS)
				EntryPtr->Value.UlongValue[1] = MsdControlBlock->maxstreams;

			break;
		}
		default:
	              MSD_ERR_DBGPRINT("ERROR: mid_brdmgr_get_param: default CASE\n");
			EntryPtr->ErrorCode = CD_ERR_BAD_PARAMETER;
			break;

		} // Switch 
		EntryPtr++;
	} //for

out:
	// since all structs are identical, just change the msg
	//	id and send the msg back 
	MdMsg->MessageId = MID_CFG_GET_PARAMETER_ACK;

	// return the message to user via sync path 
	supp_process_sync_receive(MsdOpenBlock, Msg);

}

/***************************************************************************
 * Function Name		: mid_brdmgr_cfg_get_brd_config
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		: To be re-written as the ConfigBlk is obsolete 
 *				  Investigated for the legitimacy for the call
 ****************************************************************************/

void mid_brdmgr_cfg_get_brd_config(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PMDRV_MSG				MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PGET_BOARD_CONFIG		Ptr;
	PGET_BOARD_CONFIG_ACK	AckPtr;
	PCFG_BOARD_CONFIG		ConfigInfo;
	pmercd_adapter_block_sT	padapter;
	merc_uint_t             ErrorCode = MD_OK;
	merc_uint_t				AdapterNumber;
 
	MSD_FUNCTION_TRACE("mid_brdmgr_cfg_get_brd_config", 
			TWO_PARAMETERS, (size_t)MsdOpenBlock, (size_t)Msg); 

	Ptr = (PGET_BOARD_CONFIG)MD_GET_MDMSG_PAYLOAD(MdMsg);

	AckPtr = (PGET_BOARD_CONFIG_ACK)Ptr;

	AckPtr->ErrorCode = MD_OK;
 
	// perform validity checking 
	if(Ptr->BoardNumber >= MSD_MAX_ADAPTER_COUNT) {
	     MSD_ERR_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_config: BoardNumber %d, Max: %d\n",
                               Ptr->BoardNumber, MSD_MAX_ADAPTER_COUNT);
	     AckPtr->ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
	     AckPtr->ErrorCode = Ptr->BoardNumber;
	     goto out;
	}  

	AdapterNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

	if((AdapterNumber = 0xFF)) {
	
	   MSD_ERR_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_config: BoardNumber %d\n",
                             Ptr->BoardNumber);
	   ErrorCode = CD_ERR_ADAPTER_INVALID;

           goto out;

	}


	padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

	if( padapter == NULL ) {
	        MSD_ERR_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_config: AdapterNumber%d\n",
                                         AdapterNumber);
		
		ErrorCode = CD_ERR_ADAPTER_INVALID;

		goto out;
	}
	
	MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

	if (padapter->state != MERCD_ADAPTER_STATE_STARTED) {
	        MSD_ERR_DBGPRINT("ERROR: mid_brdmgr_cfg_get_brd_config: state %0x%x\n",
                                         padapter->state);
		
		MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

		/* -- UNLOCK -- */

		AckPtr->ErrorCode = CD_ERR_BAD_STATE;

		goto out;
	}  

         MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

	ConfigInfo = (PCFG_BOARD_CONFIG)(Ptr+1);

	// padapter->phost_info->reg_block = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;
	// copy the config data from adapter block to return message 
	MsdCopyMemory((pmerc_char_t)&padapter->phost_info->reg_block,
	              (pmerc_char_t)ConfigInfo,
	              sizeof(CFG_BOARD_CONFIG));

out:
	// since all structs are identical, just change the msg
	//	id and send the msg back 
	MdMsg->MessageId = MID_GET_BOARD_CONFIG_ACK;

	// return the message to user via sync path 
	supp_process_sync_receive(MsdOpenBlock, Msg);
}

/***************************************************************************
 * Function Name        : mid_brdmgr_set_post_state
 * Function Type        : manager function
 * Inputs               : MsdOpenBlock,
 *                      Msg
 * Outputs              : none
 * Calling functions    :
 * Description          :
 * Additional comments  :
 ****************************************************************************/
void mid_brdmgr_set_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PCFG_SET_BRD_POST_STATE	        Ptr, AckPtr;
        pmercd_adapter_block_sT         padapter;
        REGISTER PMDRV_MSG              MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_int_t                      MsgSize;
        merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
#ifndef LFS
        merc_uint_t                     BoardNumber;
#endif

        MSD_FUNCTION_TRACE("mid_brdmgr_set_post_state", TWO_PARAMETERS,
                        (size_t)MsdOpenBlock, (size_t)(Msg));

        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        Ptr = (PCFG_SET_BRD_POST_STATE)MD_GET_MDMSG_PAYLOAD(MdMsg);

        AckPtr = Ptr;

        // Debug Info

		if ( mercd_adapter_map[Ptr->ConfigId] == 0xFF) {
			MdMsg->MessageId = MID_CFG_SET_POST_STATE_ACK;
			supp_process_sync_receive(MsdOpenBlock, Msg);
			return;
        }

		 padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->ConfigId]];
        

		 if( padapter == NULL) {
        		MSD_LEVEL2_DBGPRINT("ERROR: mid_brdmgr_set_post_state: BoardNumber: 0x%x\n", BoardNumber);

                ErrorCode = CD_ERR_ADAPTER_INVALID;

                ReturnValue = Ptr->ConfigId;

                goto out;
        	}


		padapter->flags.PostFlags = Ptr->PostState;

		MdMsg->MessageId = MID_CFG_SET_POST_STATE_ACK;

		supp_process_sync_receive(MsdOpenBlock, Msg);

        return;

out:
        // send back an error/reply message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
        else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        return;
}

/***************************************************************************
 * Function Name                : mid_brdmgr_get_post_state
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                              Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_brdmgr_get_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PCFG_GET_BRD_POST_STATE	        Ptr, AckPtr;
        pmercd_adapter_block_sT         padapter;
        REGISTER PMDRV_MSG              MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_int_t                      MsgSize;
        merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
#ifndef LFS
        merc_uint_t                     BoardNumber;
#endif

        MSD_FUNCTION_TRACE("mid_brdmgr_get_post_state", TWO_PARAMETERS,
                        (size_t)MsdOpenBlock, (size_t)(Msg));

        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_GET_BRD_POST_STATE,"GetPostState");
        Ptr = (PCFG_GET_BRD_POST_STATE)MD_GET_MDMSG_PAYLOAD(MdMsg);

        AckPtr = Ptr;


        // Debug Info

		if ( mercd_adapter_map[Ptr->ConfigId] == 0xFF) {
			 MdMsg->MessageId = MID_CFG_GET_POST_STATE_ACK;
			 supp_process_sync_receive(MsdOpenBlock, Msg);
			 return;
		}

		
		padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->ConfigId]];

		if( padapter == NULL ) {
			MSD_LEVEL2_DBGPRINT("mid_brdmgr_cfg_get_config_rom: ConfigId: %d \n", Ptr->ConfigId);

			ErrorCode = CD_ERR_ADAPTER_INVALID;
		
			goto out;
		}

		AckPtr->PostState = padapter->flags.PostFlags;

		MdMsg->MessageId = MID_CFG_GET_POST_STATE_ACK;

		supp_process_sync_receive(MsdOpenBlock, Msg);

		return;

out:
        // send back an error/reply message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
        else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        return;
}

/***************************************************************************
 * Function Name                : mid_brdmgr_get_brd_to_cfgid
 * Function Type                : Used for unified WW start board
 * Inputs                       : MsdOpenBlock, Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_brdmgr_pam_brd_to_cfg(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{

   PBRDID_TO_CFGID          Ptr;
   pmercd_adapter_block_sT  padapter;
   REGISTER PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   merc_uint_t              ErrorCode = MD_OK;
   merc_uint_t              ReturnValue = 0;
   merc_int_t               MsgSize;
   merc_int_t               BoardNumber;
	
   MSD_FUNCTION_TRACE("mid_brdmgr_get_brd_to_cfgid", TWO_PARAMETERS,
                            (size_t)MsdOpenBlock, (size_t)(Msg));

   MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

   if (MsgSize < sizeof(BRDID_TO_CFGID)) {
       MSD_ERR_DBGPRINT("mid_brdmgr_get_brd_to_cfgid: Return MsgSize too small (%d < %d)\n", MsgSize,  sizeof(BRDID_TO_CFGID));
       ErrorCode = CD_ERR_BAD_MSG_SIZE;
       ReturnValue = MsgSize;
       goto out;
   }

   Ptr = (PBRDID_TO_CFGID)MD_GET_MDMSG_PAYLOAD(MdMsg);

   BoardNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

   if (BoardNumber == 0xFF) {
       MSD_LEVEL2_DBGPRINT("ERROR mid_brdmgr_get_brd_to_cfgid: %d\n", Ptr->BoardNumber);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->BoardNumber;
       goto out;
   }

   padapter = MsdControlBlock->padapter_block_list[BoardNumber];

   if (padapter == NULL) {
       MSD_LEVEL2_DBGPRINT("GET_BRD_TO_CFGID: Invalid Adapter Number!\n");
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->BoardNumber;
       goto out;
   }

   Ptr->CfgId = padapter->adapternumber;

   // just change the msg id and send the msg back
   MdMsg->MessageId =  MID_BRDID_TO_CFGID_ACK;

   // return the message to Driver via sync path
   supp_process_sync_receive(MsdOpenBlock, Msg);

   return;

out:
   // send back an error/reply message
   Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

   if (MD_IS_MSG_ASYNC(Msg))
       supp_process_receive(MsdOpenBlock, Msg);
   else
       supp_process_sync_receive(MsdOpenBlock, Msg);

   return;
}


/***************************************************************************
 * Function Name                : mid_brdmgr_cfg_get_sram
 * Function Type                : Used setup configid and size for getting sram
 * Inputs                       : MsdOpenBlock, Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_brdmgr_cfg_get_sram(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{

   PCFG_GET_SRAM            Ptr;
   pmercd_adapter_block_sT  padapter;
   REGISTER PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   merc_uint_t              ErrorCode = MD_OK;
   merc_uint_t              ReturnValue = 0;
   merc_int_t               MsgSize;
	
   MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

   if (MsgSize < sizeof(CFG_GET_SRAM)) {
       MSD_ERR_DBGPRINT("mid_brdmgr_cfg_get_sram: MsgSize too small (%d < %d)\n", MsgSize,  sizeof(CFG_GET_SRAM));
       ErrorCode = CD_ERR_BAD_MSG_SIZE;
       ReturnValue = MsgSize;
       goto out;
   }

   Ptr = (PCFG_GET_SRAM)MD_GET_MDMSG_PAYLOAD(MdMsg);

   // check if there is an adapter for this config id
   if (mercd_adapter_map[Ptr->CfgId] == 0xFF) {
       printk("mid_brdmgr_cfg_get_sram: Error - invaild config id %d\n", Ptr->CfgId);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->CfgId;
       goto out;
   }

   padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->CfgId]];

   if (!padapter) {
       printk("mid_brdmgr_cfg_get_sram: Error - invaild config id %d\n", Ptr->CfgId);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->CfgId;
       goto out;
   } 

   // check this board is not a WW board 
   if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
       MSD_LEVEL2_DBGPRINT("mid_brdmgr_cfg_get_sram: Error - nonSram board %d\n", Ptr->CfgId);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->CfgId;
       goto out;
   }

   // save the config id and size for mmap call
   MsdControlBlock->sramDumpSz = Ptr->Size;
   MsdControlBlock->sramDumpCfgId = Ptr->CfgId;

   // just change the msg id and send the msg back
   MdMsg->MessageId =  MID_GET_SRAM_ACK;

   // return the message to Driver via sync path
   supp_process_sync_receive(MsdOpenBlock, Msg);

   return;

out:
   // send back an error/reply message
   Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

   if (MD_IS_MSG_ASYNC(Msg))
       supp_process_receive(MsdOpenBlock, Msg);
   else
       supp_process_sync_receive(MsdOpenBlock, Msg);

   return;
}

/***************************************************************************
 * Function Name        : mid_brdmgr_get_power_status
 * Function Type        : manager function
 * Inputs               : MsdOpenBlock,
 *                        Msg
 * Outputs              :
 * Calling functions    : MDI
 * Description          : 
 * Additional comments  :
 ****************************************************************************/
void mid_brdmgr_get_power_status(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
   PMD_GET_POWER_STATUS     Ptr;
   pmercd_adapter_block_sT  padapter;
   REGISTER PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
   merc_uint_t              ErrorCode = MD_OK;
   merc_uint_t              ReturnValue = 0;
   merc_int_t               MsgSize;

   MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

   if (MsgSize < sizeof(MD_GET_POWER_STATUS)) {
       MSD_ERR_DBGPRINT("mid_brdmgr_get_power_status: MsgSize too small (%d < %d)\n", MsgSize,  sizeof(MD_GET_POWER_STATUS));
       ErrorCode = CD_ERR_BAD_MSG_SIZE;
       ReturnValue = MsgSize;
       goto out;
   }

   Ptr = (PMD_GET_POWER_STATUS)MD_GET_MDMSG_PAYLOAD(MdMsg);

#if 0
   switch (Ptr->boardIndex) {
        case 1:
        Ptr->powerGoodRegValue = 1;
        Ptr->powerOverrideRegValue = 0;
        Ptr->actualPowerProvidedValue = 25;
        Ptr->powerRequiredByBoardValue = 10;
        Ptr->busNumber = 1;
        Ptr->slotNumber =1;
        break;
        case 2:
        Ptr->powerGoodRegValue = 0;
        Ptr->powerOverrideRegValue = 0;
        Ptr->actualPowerProvidedValue = 10;
        Ptr->powerRequiredByBoardValue = 25;
        Ptr->busNumber = 1;
        Ptr->slotNumber =1;
        break;
        case 3:
        Ptr->powerGoodRegValue = 0;
        Ptr->powerOverrideRegValue = 1;
        Ptr->actualPowerProvidedValue = 25;
        Ptr->powerRequiredByBoardValue = 10;
        Ptr->busNumber = 1;
        Ptr->slotNumber =1;
        break;
        case 4:
        Ptr->powerGoodRegValue = 1;
        Ptr->powerOverrideRegValue = 1;
        Ptr->actualPowerProvidedValue = 10;
        Ptr->powerRequiredByBoardValue = 25;
        Ptr->busNumber = 1;
        Ptr->slotNumber =1;
        break;
        default:
        Ptr->powerGoodRegValue = 1;
        Ptr->powerOverrideRegValue = 0;
        Ptr->actualPowerProvidedValue = 25;
        Ptr->powerRequiredByBoardValue = 10;
        Ptr->busNumber = 1;
        Ptr->slotNumber =1;
        break;
   }
   // just change the msg id and send the msg back
   MdMsg->MessageId =  MID_GET_POWER_STATUS_ACK;

   // return the message to Driver via sync path
   supp_process_sync_receive(MsdOpenBlock, Msg);
   return;
#endif

   padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[Ptr->boardIndex]];

   if (!padapter) {
       printk("mid_brdmgr_get_power_status: Error - invaild board id %d\n", Ptr->boardIndex);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->boardIndex;
       goto out;
   }

   // PCIe: Power Managment Status
   if (padapter->phw_info->typePCIExpress) {
       Ptr->busNumber = padapter->phw_info->bus_number;
       Ptr->slotNumber = padapter->phw_info->slot_number;
       Ptr->powerGoodRegValue = padapter->phw_info->powerGoodReg;
       Ptr->powerOverrideRegValue = padapter->phw_info->powerOverrideReg;
       Ptr->actualPowerProvidedValue = padapter->phw_info->actualPowerProvided;
       Ptr->powerRequiredByBoardValue = padapter->phw_info->powerRequiredByBoard;
   } else {
       // not a PCIe - return error
       MSD_LEVEL2_DBGPRINT("mid_brdmgr_get_power_status: Error - nonPCIe board %d\n", Ptr->boardIndex);
       ErrorCode = CD_ERR_ADAPTER_INVALID;
       ReturnValue = Ptr->boardIndex;
       goto out;
   }

   // just change the msg id and send the msg back
   MdMsg->MessageId =  MID_GET_POWER_STATUS_ACK;

   // return the message to Driver via sync path
   supp_process_sync_receive(MsdOpenBlock, Msg);

   return;

out:
   // send back an error/reply message
   Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

   if (MD_IS_MSG_ASYNC(Msg))
       supp_process_receive(MsdOpenBlock, Msg);
   else
       supp_process_sync_receive(MsdOpenBlock, Msg);

   return;
}
