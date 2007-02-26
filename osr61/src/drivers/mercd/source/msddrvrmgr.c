/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msddrvrmgr.c
 * Description			: Driver Manager functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDDRVRMGR_C_
#include "msdextern.h"
#include "msdsramif.h"
#undef _MSDDRVRMGR_C_

/***************************************************************************
 * Function Name                : mid_drvrmgr_cfg_drvr_start
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/

void mid_drvrmgr_cfg_drvr_start(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PCFG_DRIVER_START               DrvStartPtr;
        REGISTER PMDRV_MSG              MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        mercd_dhal_drvr_identify_sT     drvr_identifyinfo = { 0 };
        mercd_osal_dpc_register_sT      dpc_reginfo = { 0 };

        merc_int_t  cnt;
        merc_int_t  MsgSize;
        merc_uint_t ErrorCode = MD_OK;
        merc_uint_t ReturnValue = 0;
        merc_uint_t j;
       MSD_FUNCTION_TRACE("mid_drvrmgr_cfg_drvr_start", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));

        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        // sync msg uses the sync return path
        MD_SET_MDMSG_BIND_HANDLE(MdMsg, 0);
        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        // Init the driver with the message parameters
        DrvStartPtr = (PCFG_DRIVER_START)(MD_GET_MDMSG_PAYLOAD(MdMsg));

        // validity checking
        if ( (DrvStartPtr->MaxBind > MSD_ABSOLUTE_MAX_BIND) ||
                  (DrvStartPtr->MaxStreamConnections > MSD_ABSOLUTE_MAX_STREAMS) ) {

                MSD_LEVEL2_DBGPRINT("mid_drvrmgr_cfg_drvr_start:",
                                     "MaxBind: %d Max: %d MaxStrCon: %d MaxStr: %d\n",
                               DrvStartPtr->MaxBind, MSD_ABSOLUTE_MAX_BIND, 
                               DrvStartPtr->MaxStreamConnections,
                               MSD_ABSOLUTE_MAX_STREAMS
                               );

                ErrorCode = CD_ERR_BAD_PARAMETER;
                goto out;
        }

        if(DrvStartPtr->MaxBind > MsdControlBlock->maxbind) {

                pmercd_bind_block_sT *pbind_block_list;
                pmercd_open_block_sT *popen_block_queue;

                mercd_zalloc( pbind_block_list,
                              pmercd_bind_block_sT *,
                              sizeof(pmercd_bind_block_sT)
                               *
                              DrvStartPtr->MaxBind
                             );



                bcopy( (void *) MsdControlBlock->pbind_block_list,
                       (void *) pbind_block_list,
                       sizeof( pmercd_bind_block_sT ) * MsdControlBlock->maxbind ) ;

                mercd_free(MsdControlBlock->pbind_block_list,
                           MsdControlBlock->maxbind * sizeof(pmercd_bind_block_sT),
                           MERCD_FORCE);

                MsdControlBlock->pbind_block_list = pbind_block_list;

                mercd_zalloc( popen_block_queue,
                               pmercd_open_block_sT *,
                               sizeof(pmercd_open_block_sT)
                                *
                               DrvStartPtr->MaxBind
                             );

                bcopy( (void *) MsdControlBlock->popen_block_queue,
                       (void *) popen_block_queue,
                       sizeof( popen_block_queue ) * MsdControlBlock->maxbind ) ;

                mercd_free(MsdControlBlock->popen_block_queue,
                           MsdControlBlock->maxopen * sizeof(pmercd_open_block_sT),
                           MERCD_FORCE);

                MsdControlBlock->popen_block_queue = popen_block_queue;
        }

        // setup the global control block with parameters
        MsdControlBlock->maxbind = DrvStartPtr->MaxBind;
        MsdControlBlock->maxstreams = DrvStartPtr->MaxStreamConnections;
        MsdControlBlock->maxadapters = MD_MAX_ADAPTER_COUNT;

        // check the current state
        MSD_DRIVER_STATE_CHECK(MERCD_CTRL_BLOCK_INIT);

        // perform validity checking
        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_DRIVER_START, "DriverStart");

        // Make Sure that the Driver is sanitized to start

        (*mercd_dhal_func[MERCD_DHAL_DRVR_IDENTIFY])((void *)&drvr_identifyinfo);

        if(drvr_identifyinfo.ret != MD_SUCCESS) {
           MSD_LEVEL2_DBGPRINT("mid_drvrmgr_cfg_drvr_start MERCD_DHAL_DRVR_IDENTIFY fails\n");
           ErrorCode = CD_ERR_KAW_START_FAILED;
           goto out;
        }

        // Init the driver with the message parameters
        DrvStartPtr = (PCFG_DRIVER_START)(MD_GET_MDMSG_PAYLOAD(MdMsg));

        // validity checking
        if ( (DrvStartPtr->MaxBind > MSD_ABSOLUTE_MAX_BIND) ||
             (DrvStartPtr->MaxStreamConnections > MSD_ABSOLUTE_MAX_STREAMS) ) {
               MSD_LEVEL2_DBGPRINT("mid_drvrmgr_cfg_drvr_start %d %d\n",
                        DrvStartPtr->MaxStreamConnections, MSD_ABSOLUTE_MAX_STREAMS);
                ErrorCode = CD_ERR_BAD_PARAMETER;
                goto out;
        }

        for( cnt = 0; cnt < MSD_MAX_BOARD_ID_COUNT ; ++cnt) {
#ifndef LFS
                pmercd_adapter_block_sT padapter;
                mercd_osal_timeout_start_sT timeoutinfo = { 0 };
#endif


                if(mercd_adapter_map[cnt] < MSD_MAX_BOARD_ID_COUNT) {

                        for(j=0;j<MSD_MAX_STREAM_ID;j++)
                                MsdControlBlock->padapter_block_list[mercd_adapter_map[cnt]]->pstream_connection_list[j] = NULL;

                        dpc_reginfo.Function = (PMD_FUNCTION)MERCD_GENERIC_INTR_PROCESSING_ADDR;

                        dpc_reginfo.Context = (pmerc_void_t) &mercd_adapter_map[cnt];
                        dpc_reginfo.phw_info = MsdControlBlock->padapter_block_list[mercd_adapter_map[cnt]]->phw_info;
                        dpc_reginfo.ConfigId = cnt;

                        (*mercd_osal_func[MERCD_OSAL_DPC_REGISTER])((void *) &dpc_reginfo);

                        if(dpc_reginfo.ret != MD_SUCCESS){
                                ErrorCode = PAM_ERR_REGISTER_DPC;
                                // MSD_ERR_DBGPRINT( "DriverStart: Cannot register DPC\n");
                                ReturnValue = 0;
                                break;
                        }


                }

        }

        // set driver state to READY
        MsdControlBlock->MsdState  = MERCD_CTRL_BLOCK_READY;

out:
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
         else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        return;

}
/***************************************************************************
 * Function Name                : mid_drvrmgr_cfg_pam_start
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_drvrmgr_cfg_pam_start(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        REGISTER PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_uint_t ErrorCode = MD_OK;
        merc_uint_t ReturnValue = 0;

        MSD_FUNCTION_TRACE("mid_drvrmgr_cfg_pam_start", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));

        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        // sync msg uses the sync return path
        MD_SET_MDMSG_BIND_HANDLE(MdMsg, 0);

        // send back an error/reply message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
         else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        return;

}

/***************************************************************************
 * Function Name                : mid_drvrmgr_cfg_drvr_shutdown
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_drvrmgr_cfg_drvr_shutdown(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{

#if 0
        mercd_osal_dpc_deregister_sT    dpc_dereginfo = { 0 };
        REGISTER PMDRV_MSG              MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        PMSD_OPEN_BLOCK                 OpenBlock;
        merc_int_t    MsgSize;
        merc_uint_t   ErrorCode = MD_OK;
        merc_uint_t   ReturnValue = 0;
        merc_ushort_t cnt;

        MSD_FUNCTION_TRACE("mid_drvrmgr_cfg_drvr_shutdown", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));

        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        // sync msg uses the sync return path
        MD_SET_MDMSG_BIND_HANDLE(MdMsg, 0);
        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        // perform validity checking
        MSD_DRIVER_STATE_CHECK(MERCD_CTRL_BLOCK_READY);

        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_PAM_START, "DriverStop");

        // Any active adapters?
        if(MsdAdapterList.QueueHead){
                ErrorCode = CD_ERR_SYSTEM_ERROR;
                goto out;
        }


        for ( cnt = 0; cnt < MSD_MAX_ADAPTER_COUNT; ++cnt) {
                dpc_dereginfo.ConfigId = cnt;
                dpc_dereginfo.DpcHandle = MercDeferredQBlock[cnt]->DpcHandle;

                (*mercd_osal_func[MERCD_OSAL_DPC_DEREGISTER])((void *) &dpc_dereginfo);

                if( dpc_dereginfo.ret != MD_SUCCESS){
                        ErrorCode = PAM_ERR_DEREGISTER_DPC;
                        ReturnValue = 0;
                        MSD_ERR_DBGPRINT("DriverStop: Cannot deregister DPC\n");
                        goto out;
                }

                // free the DeferredQ block memory
                MSD_FREE_KERNEL_MEMORY((pmerc_void_t)MercDeferredQBlock[cnt],
                                        sizeof(MERC_DEFERRED_Q_BLOCK));

                MercDeferredQBlock[cnt] = NULL;
        }
        // reset the driver state to INIT
        MsdControlBlock->MsdState = MERCD_CTRL_BLOCK_INIT;

out:
        // send back an error/reply message
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
         else
                supp_process_sync_receive(MsdOpenBlock, Msg);

#endif

        return;
}

/***************************************************************************
 * Function Name                : mid_drvrmgr_cfg_pam_shutdown
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_drvrmgr_cfg_pam_shutdown(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        REGISTER PMDRV_MSG      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_int_t  MsgSize;
        merc_uint_t ErrorCode = MD_OK;
        merc_uint_t ReturnValue = 0;

        MSD_FUNCTION_TRACE("mid_drvrmgr_cfg_pam_shutdown", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));

        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        // sync msg uses the sync return path
        MD_SET_MDMSG_BIND_HANDLE(MdMsg, 0);
        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        // Basically NOTHING IS DONE HERE
        // MSD_ENTER_HW_MUTEX();

        // check the current state
        MSD_DRIVER_STATE_CHECK(MERCD_CTRL_BLOCK_READY);

        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, CFG_PAM_SHUTDOWN, "DriverStop:PAM");

        // MSD_EXIT_HW_MUTEX();
out:
        // send back an error/reply message
        // MSD_ENTER_HW_MUTEX();
        Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

        if(MD_IS_MSG_ASYNC(Msg))
                supp_process_receive(MsdOpenBlock, Msg);
         else
                supp_process_sync_receive(MsdOpenBlock, Msg);

        // MSD_EXIT_HW_MUTEX();
        return;
}

/***************************************************************************
 * Function Name                : mid_drvrmgr_pam_get_panic_dump
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :   THIS FUNCTION IS NOT  PORTED TO NEW ARCH.
 ****************************************************************************/
void mid_drvrmgr_pam_get_panic_dump(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PMDRV_MSG               MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        pmercd_adapter_block_sT padapter;
        PPAM_GET_PANIC_DUMP     Ptr;
#ifndef LFS
        merc_int_t  MsgSize;
#endif
        merc_uint_t ErrorCode = MD_OK;
        merc_uint_t ReturnValue = 0;
        merc_int_t BoardNumber;

        MSD_FUNCTION_TRACE("mid_drvrmgr_pam_get_panic_dump", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));
        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        Ptr = (PPAM_GET_PANIC_DUMP)MD_GET_MDMSG_PAYLOAD(MdMsg);

        // validity checking
        if(Ptr->BoardNumber >  MSD_MAX_ADAPTER_COUNT) {
          MSD_ERR_DBGPRINT("GET_PANIC_DUMP: adapter number too large\n");
          ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
          ReturnValue = Ptr->BoardNumber;
          goto out;
        }

       BoardNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];

       if( BoardNumber == 0xFF){
        MSD_LEVEL2_DBGPRINT("BoardNumber invalid: 0x%x\n", BoardNumber);
        ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        ReturnValue = Ptr->BoardNumber;
        goto out;
       }

       padapter = MsdControlBlock->padapter_block_list[BoardNumber];

       if( padapter == NULL) {
          MSD_LEVEL2_DBGPRINT("GET_BRD_DIAG_STATE: Invalid Adapter Number! \n");
          ErrorCode = CD_ERR_ADAPTER_INVALID;
          ReturnValue = Ptr->BoardNumber;
          goto out;
        }

       // Copy the panic dump from MercBoardAdapter
        MsdCopyMemory((pmerc_char_t)padapter->panicDump,(pmerc_char_t)Ptr->PanicDump,
                                        MD_PANIC_DUMP_MAX_SIZE);

       // Send ack from here.
       // just change the msg id and send the msg back
       MdMsg->MessageId = MID_PAM_GET_PANIC_DUMP_ACK;
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


/* DRVR_TRACE */

/***************************************************************************
 * Function Name		: mid_drvrmgr_enable_drv_trace
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		: This function enables the tracing 
 *                              : NOTE : Config Id is used rather than logical
 *                              :        Board Number. Logical board number is 
 *                              :        valid only after downloader. SO, if we
 *                              :        want to trace downloader it won't be 
 *                              :        possible, that is why config id used
 ****************************************************************************/
void mid_drvrmgr_enable_drv_trace(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PMDRV_MSG		MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PCFG_ENABLE_DRV_TRACE	Ptr;
	merc_uint_t ErrorCode = MD_OK;
	merc_uint_t ReturnValue = 0;
	merc_uint_t AdapterNumber;
	pmercd_adapter_block_sT padapter;
	merc_uint_t sizeToAllocate;
	merc_uint_t msgsAllocated;
#ifndef LFS
	merc_uint_t strmId;
#endif
	merc_uint_t dataTraceSizeLimit;
#ifndef LFS
	merc_uint_t i;
#endif

	MSD_FUNCTION_TRACE("mid_drvrmgr_enable_drv_trace", TWO_PARAMETERS, 
				(size_t)MsdOpenBlock, (size_t)(Msg));
	MSD_ASSERT(MsdOpenBlock);
	MSD_ASSERT(Msg);

	Ptr = (PCFG_ENABLE_DRV_TRACE)MD_GET_MDMSG_PAYLOAD(MdMsg);

	if (Ptr == NULL) {
	  ErrorCode = CD_ERR_BAD_PARAMETER;
	  MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: Ptr is null \n");
	  goto out;
	}

	/* All of these are unsigned int ...still keep it!!! */
	if ((Ptr->BoardNumber < 0) || 
	    (Ptr->BoardNumber > MSD_MAX_ADAPTER_COUNT) ||
	    (Ptr->dataTraceSizeLimit < 0)) {
	  ErrorCode = CD_ERR_BAD_PARAMETER;
	  ReturnValue = Ptr->BoardNumber;
	  MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: BoardNumber or dataTraceSizeLimit or TraceStream  is in error \n");
	  goto out;
	}

	msgsAllocated = Ptr->TraceMsgs  ;
	if ((msgsAllocated <=0) || (msgsAllocated > MSD_MAX_TRACE_MSGS)) {
	  msgsAllocated =  MSD_MAX_TRACE_MSGS;
	}
	
	sizeToAllocate =  msgsAllocated * sizeof(mercd_trace_copy_sT);

	dataTraceSizeLimit = Ptr->dataTraceSizeLimit;
       	
	if (dataTraceSizeLimit > MSD_MAX_DATA_TRACE_SIZELIMIT) 
	  dataTraceSizeLimit = MSD_MAX_DATA_TRACE_SIZELIMIT;

	sizeToAllocate += (msgsAllocated * dataTraceSizeLimit);

#ifdef DEBUG_TRACE
	cmn_err(CE_CONT, "Board = %d, Tracelevel = %d, TraceMsgs = %d, SelectTrace = %d, Threshold = %d, dtSzLimit = %d\n",
	  Ptr->BoardNumber, Ptr->TraceLevel, Ptr->TraceMsgs, Ptr->SelectTrace, Ptr->ThreshDump, 
	  Ptr->dataTraceSizeLimit);

	for (    i=0; i<=Ptr->b2dFilter[0]; i++)
	  cmn_err(CE_CONT,"b2dFilter [%d] = %d\n",i,Ptr->b2dFilter[i]);
	
	for (    i=0; i<=Ptr->d2bFilter[0]; i++)
	  cmn_err(CE_CONT,"d2bFilter [%d] = %d\n",i,Ptr->d2bFilter[i]);
#endif

	/* Ptr->BoardNumber is Config Id. See function header comments */
	//	AdapterNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];
	AdapterNumber = mercd_adapter_map[Ptr->BoardNumber];

#ifdef DEBUG_TRACE
	cmn_err(CE_CONT, "msddrvrmgr ::::sizeToAllocate %d  AdapterNumber %d ptr->BoardNumber = %d\n",
		sizeToAllocate, AdapterNumber, Ptr->BoardNumber);
#endif

	MSD_LEVEL2_DBGPRINT("ENABLE_DRV_TRACE : sizeToAllocate %d  AdapterNumber %d ptr->BoardNumber = %d\n",
			    sizeToAllocate, AdapterNumber, Ptr->BoardNumber);

	if( AdapterNumber == 0xFF){

	  ErrorCode = CD_ERR_ADAPTER_INVALID;
	  
	  ReturnValue = Ptr->BoardNumber;

	  MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: AdapterNumber is invalid \n");
	  
	  goto out;
	}
	
	padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

	if( padapter == NULL) {

	  MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: Invalid padapter\n");
	  
	  ErrorCode = CD_ERR_ADAPTER_INVALID;
	  
	  ReturnValue = Ptr->BoardNumber;
	  
	  goto out;
	}

	if (padapter->flags.TraceLevelInfo) {

	   MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE : DrvTrace is already enabled \n");

	   ErrorCode = CD_ERR_BAD_BOARD_STATE;
	   
	   ReturnValue = AdapterNumber;
	   
	   goto out;
	}


	 if(Ptr->TraceLevel == 0) {

	   MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: TraceLevel Zero\n");
	   
	   ErrorCode = CD_ERR_BAD_PARAMETER;
	   
	   ReturnValue = AdapterNumber;
	   
	   goto out;
	 }

	 
	if (padapter->ptrace == NULL)  {

	  mercd_zalloc(padapter->ptrace, pmercd_trace_info_sT, MERCD_TRACE_INFO_LIST);

	  if(padapter->ptrace)
	    padapter->flags.TraceLevelInfo = MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED ;
	  else {
	    ErrorCode = CD_ERR_NO_MEMORY;
	    ReturnValue = AdapterNumber;
	    MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE : No Memory (1) \n");
	    goto out;
	  }
	   
	} 

	padapter->ptrace->openblk = MsdOpenBlock;
	padapter->ptrace->Drv2BrdPrintIndex = 0;
	padapter->ptrace->Brd2DrvPrintIndex = 0;

	if (dataTraceSizeLimit <= 0 ) {
	  padapter->ptrace->MsgDataTrace = MSG_DATA_NO_TRACE;
	  padapter->ptrace->CurrentDataSizeLimit = 0;
	}
	else {
	  padapter->ptrace->MsgDataTrace = MSG_DATA_TRACE;
	  padapter->ptrace->CurrentDataSizeLimit = dataTraceSizeLimit;
	}
	
	padapter->ptrace->Drv2BrdTotal = 0;
	padapter->ptrace->Brd2DrvTotal = 0;

	mercd_zalloc(padapter->ptrace->Drv2BrdTraceBuf, pmercd_trace_copy_sT, sizeToAllocate);
	mercd_zalloc(padapter->ptrace->Brd2DrvTraceBuf, pmercd_trace_copy_sT, sizeToAllocate);
	
	if ( (padapter->ptrace->Drv2BrdTraceBuf == NULL) ||
	     (padapter->ptrace->Brd2DrvTraceBuf == NULL) ) {
	  
	  if (padapter->ptrace->Drv2BrdTraceBuf)
	    mercd_free((pmerc_void_t)padapter->ptrace->Drv2BrdTraceBuf, sizeToAllocate, MERCD_FORCE );
	  if (padapter->ptrace->Brd2DrvTraceBuf)
	    mercd_free((pmerc_void_t)padapter->ptrace->Brd2DrvTraceBuf, sizeToAllocate, MERCD_FORCE );
	  
	  padapter->ptrace->Drv2BrdTraceBuf = padapter->ptrace->Brd2DrvTraceBuf = 0;
	  
	  if(padapter->flags.TraceLevelInfo &  MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED ) {
	    mercd_free(padapter->ptrace, MERCD_TRACE_INFO_LIST, MERCD_FORCE );
	    padapter->flags.TraceLevelInfo = 0 ;
	  }
	  
	  ErrorCode = CD_ERR_NO_MEMORY;
	  
	  ReturnValue = AdapterNumber;

	  MSD_ERR_DBGPRINT("ENABLE_DRV_TRACE: No Memory (2)\n");
	  
	  goto out;
	}
	
	fillTraceFilters(padapter->ptrace,Ptr);
	fillTraceStreamIds(padapter->ptrace,Ptr);
	fillTraceCompDescs(padapter->ptrace,Ptr);

	padapter->ptrace->Drv2BrdMessageCnt = msgsAllocated;
	padapter->ptrace->Brd2DrvMessageCnt = msgsAllocated;

	padapter->ptrace->Brd2DrvTraceIndex = 0;
	padapter->ptrace->Drv2BrdTraceIndex = 0;

	// Not Used
	padapter->ptrace->level = Ptr->TraceLevel;
	padapter->ptrace->match_criteria = Ptr->Criteria; 


	// Only in Select mode dumpThreshold has a meaning
	if (Ptr->SelectTrace > 0) {
	  padapter->ptrace->bSelect = MSGS_TRACE_SELECT;
	  if ((Ptr->ThreshDump > 0) && (Ptr->ThreshDump < (msgsAllocated/MSD_DUMPTHRESHOLD_FACTOR)))
	    padapter->ptrace->dumpThreshold = Ptr->ThreshDump;
	  else
	    padapter->ptrace->dumpThreshold = msgsAllocated/MSD_DUMPTHRESHOLD_FACTOR;
	}
	else {
	  padapter->ptrace->bSelect = MSGS_TRACE_RAW;
	  padapter->ptrace->dumpThreshold = 0;
	}

	// Send ack from here.
	// just change the msg id and send the msg back  \n");

	MSD_LEVEL2_DBGPRINT("Sending back the ack .... \n");

	/* For Solaris
	drv_getparm(LBOLT, (void *)&padapter->ptrace->StartTime);
	*/
	MdMsg->MessageId = MID_ENABLE_DRV_TRACE_ACK;

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
 * Function Name		: mid_drvrmgr_disable_drv_trace
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		: This function enables the tracing 
 *                              : NOTE : Config Id is used rather than logical
 *                              :        Board Number.
 ****************************************************************************/
void mid_drvrmgr_disable_drv_trace(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PMDRV_MSG	MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PCFG_DISABLE_DRV_TRACE	Ptr;
#ifndef LFS
	merc_int_t  MsgSize;
#endif
	merc_uint_t ErrorCode = MD_OK;
	merc_uint_t ReturnValue = 0;
	merc_uint_t AdapterNumber;
	pmercd_adapter_block_sT padapter;

	MSD_FUNCTION_TRACE("mid_drvrmgr_disable_drv_trace", TWO_PARAMETERS, 
				(size_t)MsdOpenBlock, (size_t)(Msg));
	MSD_ASSERT(MsdOpenBlock);
	MSD_ASSERT(Msg);

	Ptr = (PCFG_DISABLE_DRV_TRACE)MD_GET_MDMSG_PAYLOAD(MdMsg);

	if (Ptr == NULL) {
	  ErrorCode = CD_ERR_BAD_PARAMETER;
	  MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: Ptr is null \n");
	  goto out;
	}

	/* Ptr->BoardNumber is Config Id */
	//    AdapterNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];
	AdapterNumber = mercd_adapter_map[Ptr->BoardNumber];

	if( AdapterNumber == 0xFF){
	  ErrorCode = CD_ERR_ADAPTER_INVALID;
	  ReturnValue = Ptr->BoardNumber;
	  MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: AdapterNumber is invalid \n");
	  goto out;
	}

	padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

	if( padapter == NULL) {
	  
	  MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: Invalid padapter\n");
	  
	  ErrorCode = CD_ERR_ADAPTER_INVALID;
	  
	  ReturnValue = Ptr->BoardNumber;

	  goto out;
	}

	if (padapter->flags.TraceLevelInfo == 0 ) {

	  MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: DrvTrace is not enabled \n");
	  
	  ErrorCode = CD_ERR_BAD_BOARD_STATE;
	  
	  ReturnValue = AdapterNumber;
	  
	  goto out;
	}

	if(padapter->ptrace == NULL) {
	   
	   MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: Unexpected NULL trace pointertrace for Adapter # %d ,  \n",AdapterNumber);
	   
	   ErrorCode = CD_ERR_BAD_BOARD_STATE;
	   ReturnValue = AdapterNumber;
	   goto out;
	   
	}
	 
	if(padapter->ptrace->level == 0) {
	   MSD_ERR_DBGPRINT("DISABLE_DRV_TRACE: TraceLevel already Zero\n");
	   //	   cmn_err(CE_CONT,"MsdDrvrMgr : trace Level in disable is zero \n");
	   ErrorCode = CD_ERR_BAD_PARAMETER;
	   ReturnValue = AdapterNumber;
	   goto out;
	}
	 
	if(padapter->flags.TraceLevelInfo &  MERC_ADAPTER_FLAG_TRACE_STRUCT_ALLOCATED ) {
	  padapter->flags.TraceLevelInfo =  MERC_ADAPTER_FLAG_TRACE_STRUCT_NOT_ALLOCATED;

	  if (padapter->ptrace->Drv2BrdTraceBuf)
	    mercd_free((pmerc_void_t)padapter->ptrace->Drv2BrdTraceBuf,
		       padapter->ptrace->Drv2BrdMessageCnt * sizeof(mercd_trace_copy_sT),
		       MERCD_FORCE );	
	  
	  if (padapter->ptrace->Brd2DrvTraceBuf) 
	    mercd_free((pmerc_void_t)padapter->ptrace->Brd2DrvTraceBuf,
		       padapter->ptrace->Brd2DrvMessageCnt * sizeof(mercd_trace_copy_sT),
		       MERCD_FORCE );
	  
	  if (padapter->ptrace->FilterToDrv) 
	    mercd_free((pmerc_void_t)padapter->ptrace->FilterToDrv,
		       (padapter->ptrace->FilterToDrv[0] + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  
	  
	  if (padapter->ptrace->FilterToBrd) 
	    mercd_free((pmerc_void_t)padapter->ptrace->FilterToBrd,
		       (padapter->ptrace->FilterToBrd[0] + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  

	  if (padapter->ptrace->StrIdFilterToDrv) 
	    mercd_free((pmerc_void_t)padapter->ptrace->StrIdFilterToDrv,
		       (padapter->ptrace->StrIdFilterToDrv[0] + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  
	  
	  if (padapter->ptrace->StrIdFilterToBrd) 
	    mercd_free((pmerc_void_t)padapter->ptrace->StrIdFilterToBrd,
		       (padapter->ptrace->StrIdFilterToBrd[0] + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  

	  if (padapter->ptrace->SrcDesc) 
	    mercd_free((pmerc_void_t)padapter->ptrace->SrcDesc,
		       (padapter->ptrace->SrcDesc[0] * 2 + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  

	  if (padapter->ptrace->DstDesc) 
	    mercd_free((pmerc_void_t)padapter->ptrace->DstDesc,
		       (padapter->ptrace->DstDesc[0] * 2 + 1) * sizeof(merc_uint_t),
		       MERCD_FORCE );	  

	  padapter->ptrace->Brd2DrvTraceIndex = padapter->ptrace->Drv2BrdTraceIndex = 0;
	  padapter->ptrace->Brd2DrvPrintIndex = padapter->ptrace->Drv2BrdPrintIndex = 0;
	  padapter->ptrace->level = 0;
    	  padapter->ptrace->match_criteria = 0;
	  padapter->ptrace->FilterToBrd       = padapter->ptrace->FilterToDrv = NULL;
	  padapter->ptrace->StrIdFilterToBrd  = padapter->ptrace->StrIdFilterToDrv = NULL;
	  padapter->ptrace->SrcDesc           = padapter->ptrace->DstDesc = NULL;
	  padapter->ptrace->Brd2DrvTraceBuf   = padapter->ptrace->Drv2BrdTraceBuf = NULL;
	
	  if (padapter->ptrace)
	    mercd_free(padapter->ptrace, MERCD_TRACE_INFO_LIST, MERCD_FORCE );

	  padapter->ptrace = NULL;
	}
	 
	// Send ack from here.
	// just change the msg id and send the msg back 
	MdMsg->MessageId = MID_DISABLE_DRV_TRACE_ACK;

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
 * Function Name		: mid_drvr_get_drv_trace
 * Function Type		: manager function
 * Inputs			: MsdOpenBlock,
 *				  Msg
 * Outputs			: none
 * Calling functions		:
 * Description			:
 * Additional comments		:  THIS FUNCTION IS NOT PORTED TO NEW ARCH
 ****************************************************************************/
void  mid_drvr_get_drv_trace(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
	PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
	PCFG_GET_DRV_TRACE  Ptr;
#ifndef LFS
	merc_uint_t	tmpTraceLevel;
#endif
	merc_uint_t ErrorCode = MD_OK;
	merc_uint_t ReturnValue = 0;
	merc_uint_t AdapterNumber;
	merc_int_t MsgSize;
	pmercd_adapter_block_sT padapter;
#ifndef LFS
	merc_uint_t k=0,copied;
	pmercd_trace_copy_sT toPtr;
#endif
	merc_uint_t trCount; /* Number of messages to retrieve */

#ifndef LFS
	merc_uint_t sizeToCopy;
#endif

	MSD_FUNCTION_TRACE("mid_drvr_get_drv_trace", TWO_PARAMETERS, 
				(size_t)MsdOpenBlock, (size_t)(Msg));
	MSD_ASSERT(MsdOpenBlock);
	MSD_ASSERT(Msg);

	Ptr = (PCFG_GET_DRV_TRACE)MD_GET_MDMSG_PAYLOAD(MdMsg);

	if (Ptr == NULL) {
	  ErrorCode = CD_ERR_BAD_PARAMETER;
	  MSD_ERR_DBGPRINT("GET_DRV_TRACE: Ptr is null \n");
	  goto out;
	}

	MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

	if(MsgSize < sizeof(MD_ACK)) {
		MSD_ERR_DBGPRINT("GET_DRV_TRACE: too small message, "
				"%d < %d or < %d\n", MsgSize,
				sizeof(CFG_GET_DRV_TRACE), sizeof(MD_ACK));
		ErrorCode = CD_ERR_BAD_MSG_SIZE;
		goto out;
	}

	trCount = Ptr->TraceCount;

	//	AdapterNumber = mercd_adapter_log_to_phy_map_table[Ptr->BoardNumber];
	AdapterNumber = mercd_adapter_map[Ptr->BoardNumber];

    if( AdapterNumber == 0xFF){
        ErrorCode = CD_ERR_ADAPTER_INVALID;
        ReturnValue = Ptr->BoardNumber;
	MSD_ERR_DBGPRINT("GET_DRV_TRACE: AdapterNumber is invalid \n");

        goto out;
    }

    padapter = MsdControlBlock->padapter_block_list[AdapterNumber];

    if( padapter == NULL) {

	MSD_ERR_DBGPRINT("GET_DRV_TRACE: padapter is invalid \n");

        ErrorCode = CD_ERR_ADAPTER_INVALID;

        ReturnValue = Ptr->BoardNumber;

        goto out;
    }

    if (padapter->flags.TraceLevelInfo == 0) {

      MSD_ERR_DBGPRINT("mid_get_drv_trace : DrvTrace is not enabled \n");
      
      ErrorCode = CD_ERR_BAD_BOARD_STATE;
      
      ReturnValue = AdapterNumber;
      
      goto out;
    }

    if(padapter->ptrace == NULL ) {
      
      MSD_ERR_DBGPRINT("Trace structure NULL, major error\n");
      ErrorCode = CD_ERR_BAD_BOARD_STATE;
      ReturnValue = AdapterNumber;
      goto out;	
	  
    }    

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    if (Ptr->SelectFlag) {
      PSTRM_MSG trmsg = MakeTraceMsg(padapter, 0);
      supp_process_receive(MsdOpenBlock, trmsg);
      MSD_FREE_MESSAGE(Msg);
      //padapter->ptrace->level = tmpTraceLevel;
      return;
    } else {
      MSD_ERR_DBGPRINT("mid_get_drv_trace : Get trace not in Select mode. \n");
      ErrorCode = CD_ERR_BAD_BOARD_STATE;
      ReturnValue = AdapterNumber;
      goto out;
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
 * Function Name                : mid_getset_boot_host_ram_bit
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          :  THIS FUNCTION IS NOT PORTED TO NEW ARCH
 ****************************************************************************/
void mid_getset_boot_host_ram_bit(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
#ifdef SKIP
        PMDRV_MSG   MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        MSD_HANDLE   BindHandle;
        PMSD_ADAPTER_BLOCK AdapterBlock;
        PMERC_BOARD_BLOCK  MercBoardAdapter;
        PPAM_BOOT_HOST_RAM_BIT  Ptr;
        merc_int_t MsgSize;
        merc_uint_t ErrorCode = MD_OK;
        merc_uint_t ReturnValue = 0;
        merc_uint_t AdapterNumber;

        MSD_FUNCTION_TRACE("mid_getset_boot_host_ram_bit", TWO_PARAMETERS,
                                (size_t)MsdOpenBlock, (size_t)(Msg));
        MSD_ASSERT(MsdOpenBlock);
        MSD_ASSERT(Msg);

        Ptr = (PPAM_BOOT_HOST_RAM_BIT)MD_GET_MDMSG_PAYLOAD(MdMsg);
        AdapterNumber = Ptr->BoardNumber;

        // validity checking
        if(AdapterNumber > MsdControlBlock->MaxAdapters) {
                MSD_ERR_DBGPRINT("GET_PANIC_DUMP: adapter number too large\n");
                ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
                ReturnValue = AdapterNumber;
                goto out;
        }

        // find the AdapterBlock
        AdapterBlock = MsdAdapterMapTable[AdapterNumber].AdapterBlock;

        if(AdapterBlock == NULL) {
                MSD_ERR_DBGPRINT("GET_PANIC_DUMP: adapter block null, "
                                "may not be configured. Adpaternumber=%d\n",
                                AdapterNumber);
                ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
                ReturnValue = AdapterNumber;
                goto out;
        }
        MercBoardAdapter = (PMERC_BOARD_BLOCK)AdapterBlock->MercBoardLocal;
        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);
        if(MsgSize<sizeof(PAM_BOOT_HOST_RAM_BIT)||MsgSize < sizeof(MD_ACK)) {
                MSD_ERR_DBGPRINT("PAM_BOOT_HOST_RAMBIT: too small message, "
                             "%d < %d or < %d\n", MsgSize,
                                 sizeof(PAM_BOOT_HOST_RAM_BIT), sizeof(MD_ACK));
                ErrorCode = CD_ERR_BAD_MSG_SIZE;
                goto out;
        }

        if (MD_GET_MDMSG_ID(MdMsg) == MID_PAM_SET_BOOT_HOST_RAM_BIT) {
                if (Ptr->BootHostRamBit == MD_SET_BOOT_FROM_FLASH) {
                        MERC_CLR_BOOT_HOST_RAM_BIT((&MercBoardAdapter->RegisterBlock));
                } else if (Ptr->BootHostRamBit == MD_SET_BOOT_FROM_HOST_RAM) {
                        MERC_SET_BOOT_HOST_RAM_BIT((&MercBoardAdapter->RegisterBlock));
                } else {
                        ErrorCode = PAM_ERR_BAD_PARAMETER;
                        MSD_ERR_DBGPRINT("PAM_BOOT_HOST_RAMBIT: PAM_BOOT_HOST_RAM_BIT: "
                                        "invalid argument 0x%x\n",Ptr->BootHostRamBit);
                        goto out;
                }
        }

        Ptr->BootHostRamBit =
                       MsdRegReadUchar(MercBoardAdapter->RegisterBlock.SetBootHostRamBit);

        // Send ack from here.
        // just change the msg id and send the msg back */
        MdMsg->MessageId = MID_PAM_GET_BOOT_HOST_RAM_BIT_ACK;

        // return the message to user via sync path
        supp_process_sync_receive(MsdOpenBlock, Msg);

        return;
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
#endif

        return;
}

/***************************************************************************
 * Function Name        : mid_drvrmgr_get_statistics
 * Function Type        : manager function
 * Inputs               : MsdOpenBlock,
 *                        Msg
 * Outputs              : none
 * Calling functions    : MDI
 * Description          : 
 * Additional comments  :
 ****************************************************************************/

void mid_drvrmgr_get_statistics(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
        PGET_STATISTICS                 Ptr;
        PGET_STATISTICS_ACK             StatsAckPtr;
        MD_ACK                          *AckPtr;
        pmercd_adapter_block_sT         padapter=NULL;
        REGISTER PMDRV_MSG      MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
        merc_uint_t                     ErrorCode = MD_OK;
        merc_uint_t                     ReturnValue = 0;
        merc_int_t                      MsgSize;
#ifndef LFS
        merc_uint_t                     bhNumber;
#endif
        PGET_STATISTICS_ENTRY           ValuePtr;
        PWW_STATS                       wwValuePtr;
#ifndef LFS
        PMSD_MUTEX_STATS                pMutexStatsPtr;
        merc_uint_t                     AdapterNumber;
#endif


        MSD_FUNCTION_TRACE("mid_drvrmgr_get_statistics", TWO_PARAMETERS,
                            (size_t)MsdOpenBlock, (size_t)(Msg));

        MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

        MSD_RETURN_MSG_SIZE_CHECK(MsgSize, GET_STATISTICS, "GetStatistics");

        AckPtr = (MD_ACK *)MD_GET_MDMSG_PAYLOAD(MdMsg);
        Ptr = (PGET_STATISTICS)(AckPtr+1);
        StatsAckPtr = (PGET_STATISTICS_ACK)Ptr;
        ValuePtr = (PGET_STATISTICS_ENTRY)(Ptr + 1);
        wwValuePtr = (PWW_STATS)(ValuePtr+1);

        //For Future Use
        //pMutexStatsPtr = (PMSD_MUTEX_STATS)(wwValuePtr+1);

        //if (pMutexStatsPtr == NULL)
        // {
        //   ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
        //   ReturnValue = Ptr->boardNum;
        //   goto out;
        // }

        AckPtr->ErrorCode = MD_OK;
        // perform validity checking
        if(Ptr->boardNum >= MD_MAX_ADAPTER_COUNT) {

                MSD_ERR_DBGPRINT("mid_drvrmgr_get_statistics : Bad board number, %d >= %d\ n", Ptr->boardNum, MD_MAX_ADAPTER_COUNT);

                ErrorCode = CD_ERR_BAD_BOARD_NUMBER;

                ReturnValue = Ptr->boardNum;

                goto out;
        }

        //We send the Slot Number through the qGetStatistics API
        //Not the Board Number
        padapter = MsdControlBlock->padapter_block_list[Ptr->boardNum];

        if( padapter == NULL ) {

                MSD_ERR_DBGPRINT("mid_drvrmgr_get_statistics: bad padapter.\n");

                ErrorCode = CD_ERR_BAD_BOARD_NUMBER;

                ReturnValue = Ptr->boardNum;

                goto out;
        }

        MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);

        ValuePtr->ErrorCode = MD_OK;
        ValuePtr->bnHandles = MsdControlBlock->bind_count;
        ValuePtr->opHandles = MsdControlBlock->open_block_count;
        ValuePtr->totalStHandles = MsdControlBlock->open_streams;

        //Copy the WW Statistics
        if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY)
          {
            if (padapter->pww_info->state == MERCD_ADAPTER_WW_SUCCEDED)
             {
               MsdCopyMemory(padapter->pww_info->pww_counters,
                      wwValuePtr, sizeof(WW_STATS));
             }
           }
        else
         {
            MSD_ZERO_MEMORY(wwValuePtr, sizeof(WW_STATS));
         }

        //Future Use
        //Copye the Mutex Stats
        //MsdCopyMemory(pmutex_stats, 
        //              pMutexStatsPtr, 
        //              sizeof(MSD_MUTEX_STATS));


        // just change the msg id and send the msg back
        MdMsg->MessageId = MID_GET_STATISTICS_ACK;

        MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);

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
 * Function Name		: getNewCurTime
 * Function Type		: Utility Function
 * Inputs			: struct timeval *tv
 * Outputs			: 
 * Calling functions		: supp_push_trace_msg
 * Description			: Returns the time in timeval structure
 * Additional comments		: do_gettimeofday works only in LINUX
 ****************************************************************************/
/* Returns the Current Time - Used in Linux */
void getNewCurTime(struct timeval *tv) {
  do_gettimeofday(tv);
}

/***************************************************************************
 * Function Name		: fillTraceFilters
 * Function Type		: Utility Function
 * Inputs			: pmercd_trace_info_sT trace
 *                                PCFG_ENABLE_DRV_TRACE ptr
 *
 *                   Convention : The first element of the filter array 
 *                                indicates how many filter messages are present
 * Outputs			: 
 * Calling functions		: mid_enable_drv_trace
 * Description			: Gets the messages to be traced from ptr
 *                                and fills into the arrays in trace
 * Additional comments		:
 ****************************************************************************/
void fillTraceFilters(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr) {

  pmerc_uint_t b2dMsgArr = ptr->b2dFilter;
  pmerc_uint_t d2bMsgArr = ptr->d2bFilter;
  merc_uint_t count = *b2dMsgArr;
#ifndef LFS
  merc_uint_t i=0;
#endif

  trace->b2dFilterPresent = 0;
  trace->d2bFilterPresent = 0;

  MSD_LEVEL2_DBGPRINT("fillTraceFilter : FilterCount (b2d) = %d\n", count);

  if (count != 0) {
    mercd_zalloc(trace->FilterToDrv, pmerc_uint_t, (count + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->b2dFilter), (pmerc_char_t)(trace->FilterToDrv),
		  (count+1) * sizeof(merc_uint_t));
    trace->b2dFilterPresent = 1;

#ifdef DEBUG_TRACE
    for (i=0; i<= trace->FilterToDrv[0]; i++)
      cmn_err(CE_CONT, "FilterToDrv[i] = %x, %x\n", trace->FilterToDrv[i],*(b2dMsgArr + i));
#endif

  }

  count = *d2bMsgArr;
  MSD_LEVEL2_DBGPRINT("fillTraceFilter : FilterCount (d2b) = %d\n", count);
  if (count != 0) {
    mercd_zalloc(trace->FilterToBrd, pmerc_uint_t,  (count + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->d2bFilter), (pmerc_char_t)(trace->FilterToBrd),
		  (count+1) * sizeof(merc_uint_t));
    trace->d2bFilterPresent = 1;

#ifdef DEBUG_TRACE
   for (i=0; i<= trace->FilterToBrd[0]; i++)
      cmn_err(CE_CONT, "FilterToBrd[i] = %x, %x\n", trace->FilterToBrd[i], *(d2bMsgArr + i));
#endif


  }

}


/***************************************************************************
 * Function Name		: fillTraceStreamIds
 * Function Type		: Utility Function
 * Inputs			: pmercd_trace_info_sT trace
 *                                PCFG_ENABLE_DRV_TRACE ptr
 *
 *                   Convention : The first element of the filter array in Ptr
 *                                indicates how many stream ids are present
 * Outputs			: 
 * Calling functions		: mid_enable_drv_trace
 * Description			: Gets the messages to be traced (on streamid)  from ptr
 *                                and fills into the arrays in trace
 * Additional comments		:
 ****************************************************************************/
void fillTraceStreamIds(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr) {

  pmerc_uint_t b2dArr = ptr->b2dStrIdFilter;
  pmerc_uint_t d2bArr = ptr->d2bStrIdFilter;
  merc_uint_t count = *b2dArr;
#ifndef LFS
  merc_uint_t i=0;
#endif

  trace->b2dStrIdPresent = 0;
  trace->d2bStrIdPresent = 0;

  MSD_LEVEL2_DBGPRINT("fillTraceStreamIds : FilterCount (b2d) = %d\n", count);

  if (count != 0) {
    mercd_zalloc(trace->StrIdFilterToDrv, pmerc_uint_t, (count + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->b2dStrIdFilter), (pmerc_char_t)(trace->StrIdFilterToDrv),
		  (count+1) * sizeof(merc_uint_t));
    trace->b2dStrIdPresent = 1;

#ifdef DEBUG_TRACE
    for (i=0; i<= trace->StrIdFilterToDrv[0]; i++)
      cmn_err(CE_CONT, "StrIdFilterToDrv[i] = %x, %x\n", trace->StrIdFilterToDrv[i],*(b2dArr + i));
#endif

  }

  count = *d2bArr;
  MSD_LEVEL2_DBGPRINT("fillTraceStreamIds : FilterCount (d2b) = %d\n", count);
  if (count != 0) {
    mercd_zalloc(trace->StrIdFilterToBrd, pmerc_uint_t,  (count + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->d2bStrIdFilter), (pmerc_char_t)(trace->StrIdFilterToBrd),
		  (count+1) * sizeof(merc_uint_t));
    trace->d2bStrIdPresent = 1;

#ifdef DEBUG_TRACE
   for (i=0; i<= trace->StrIdFilterToBrd[0]; i++)
      cmn_err(CE_CONT, "StrIdFilterToBrd[i] = %x, %x\n", trace->StrIdFilterToBrd[i], *(d2bArr + i));
#endif


  }

}

/***************************************************************************
 * Function Name		: fillTraceCompDescs
 * Function Type		: Utility Function
 * Inputs			: pmercd_trace_info_sT trace
 *                                PCFG_ENABLE_DRV_TRACE ptr
 *
 *                   Convention : The first element of the filter array in Ptr
 *                                indicates how many comp descs are present
 * Outputs			: 
 * Calling functions		: mid_enable_drv_trace
 * Description			: ptr->SrcDesc[0] = 10, means that there are
 *                              : 10 src descs. For them there 10*2=20 merc_uint_t
 *                              : are needed. One CompDesc takes 6 bytes, so to
 *                              : store them 8 bytes are allocated.
 *                              : First merc_uint_t <proc, board, node>
 *                              : Second merc_uint_t<comp, instance>
 *                                
 * Additional comments		:
 ****************************************************************************/
void fillTraceCompDescs(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr) {

  pmerc_uint_t sd = ptr->SrcDesc;
  pmerc_uint_t dd = ptr->DstDesc;
  merc_uint_t count = *sd;
#ifndef LFS
  merc_uint_t i=0;
#endif

  trace->SrcDescPresent = 0;
  trace->SrcDescPresent = 0;

  MSD_LEVEL2_DBGPRINT("fillTraceCompDescs : FilterCount (SrcDesc) = %d\n", count);

  if (count != 0) {
    mercd_zalloc(trace->SrcDesc, pmerc_uint_t, (count * 2  + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->SrcDesc), (pmerc_char_t)(trace->SrcDesc),
		  (count * 2 + 1) * sizeof(merc_uint_t));
    trace->SrcDescPresent = 1;

#ifdef DEBUG_TRACE
    for (i=0; i<= 2 * trace->SrcDesc[0]; i++)
      cmn_err(CE_CONT, "SrcDesc[i] = %x, %x\n", trace->SrcDesc[i],*(sd + i));
#endif

  }

  count = *dd;
  MSD_LEVEL2_DBGPRINT("fillTraceCompDescs : FilterCount (DstDesc) = %d\n", count);
  if (count != 0) {
    mercd_zalloc(trace->DstDesc, pmerc_uint_t,  (count * 2 + 1) * sizeof(merc_uint_t));
    MsdCopyMemory((pmerc_char_t)(ptr->DstDesc), (pmerc_char_t)(trace->DstDesc),
		  (count * 2 + 1) * sizeof(merc_uint_t));
    trace->DstDescPresent = 1;

#ifdef DEBUG_TRACE
   for (i=0; i<= 2 * trace->DstDesc[0]; i++)
      cmn_err(CE_CONT, "DstDesc[i] = %x, %x\n", trace->DstDesc[i], *(dd + i));
#endif


  }

}


/***************************************************************************
 * Function Name		: isMessageToBeTracedOnMsgType
 * Function Type		: Utility Function
 * Inputs			: pmercd_adapter_block_sT
 *                                PMERC_HOSTIF_MSG
 *                                Trace Direction (MSD_DRV2BRD_TRACE or 
 *                                MSD_BRD2DRV_TRACE)
 * Outputs			: 1 if message needs to be traced
 *                                0 if message need not be traced
 * Calling functions		: supp_push_trace_msg 
 * Description			: This function checks the following criteria
 *                                   Checks whether this message needs to 
 *                                   traced based on the filters in adapter's 
 *                                   trace structure. If no messages are given
 *                                   as a parameter to drvtrace application
 *                                   it returns 1, which means all messages 
 *                                   have to be traced.
 *                                   If a message was given as a parameter 
 *                                   to drvtrace application, then this 
 *                                   funcion checks whether hMsg's type
 *                                   matches the parameter type and returns 1
 *                                   (if it matches, it needs to be traced)
 *                                   or 0 (need not be traced)
 * Additional comments		:
 ****************************************************************************/
merc_int_t isMessageToBeTracedOnMsgType(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction) {

  pmerc_uint_t ptr=NULL;
  merc_uint_t count,i;
  merc_int_t b2dFlag = padapter->ptrace->b2dFilterPresent;
  merc_int_t d2bFlag = padapter->ptrace->d2bFilterPresent;
  merc_uint_t messagetype;


#if 0
  /* Driver messages can be distinguished as 0 to 10. */
    
  if (MERCURY_GET_MESSAGE_CLASS(hMsg) == MERCURY_CLASS_DRIVER)
    messagetype = MERCURY_GET_MESSAGE_TYPE(hMsg);
  else
    messagetype = hMsg->MessageType;
#endif

  /* But for consistency, we are making all the message types to be 32 bit */
    messagetype = hMsg->MessageType;

#ifdef DEBUG_TRACE
  if (messagetype == 0x19) {
    cmn_err(CE_CONT, "TransId = %x, msgtype = %x, Source = %x, Dest = %x, SourceDest = %x, BodySize = %x\n",
	    hMsg->FlagTransactionId,messagetype,hMsg->Source, hMsg->Destination, 
	    hMsg->SourceDestComponent, hMsg->BodySize);
  }
#endif

  if (!b2dFlag && !d2bFlag)
    return 1;

  ptr = NULL;

  if ((direction == MSD_DRV2BRD_TRACE) && d2bFlag) {
    ptr = padapter->ptrace->FilterToBrd;
  } 

  if ((direction == MSD_BRD2DRV_TRACE) && b2dFlag) {
    ptr = padapter->ptrace->FilterToDrv;
  } 

  if (ptr == NULL)
    return 1;

  count = *ptr;

  if (count == 0)
    return 1;

  for (i=1; i<=count; i++) {
    if (*(ptr + i) == messagetype) 
      return 1;
  }

  return 0;
    
}

/***************************************************************************
 * Function Name		: isMessageToBeTracedOnStreamId
 * Function Type		: Utility Function
 * Inputs			: pmercd_adapter_block_sT
 *                                PMERC_HOSTIF_MSG
 *                                Trace Direction (MSD_DRV2BRD_TRACE or 
 *                                MSD_BRD2DRV_TRACE)
 * Outputs			: 1 if message needs to be traced
 *                                0 if message need not be traced
 * Calling functions		: supp_push_trace_msg 
 * Description			: This function checks the following criteria
 *                                Checks whether this message needs to 
 *                                traced based on the filters in adapter's 
 *                                trace structure. If no messages are given
 *                                as a parameter to drvtrace application
 *                                it returns 1, which means all messages 
 *                                have to be traced.
 *                                If a message was given as a parameter 
 *                                to drvtrace application, then this 
 *                                funcion checks whether hMsg's stream id
 *                                matches the parameter msgs' streamid and returns 1
 *                                (if it matches, it needs to be traced)
 *                                or 0 (need not be traced)
 * Additional comments		:
 ****************************************************************************/
merc_int_t isMessageToBeTracedOnStreamId(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction) {

  pmerc_uint_t ptr=NULL;
  merc_uint_t count,i;
  merc_int_t b2dStrIdFlag =  padapter->ptrace->b2dStrIdPresent;
  merc_int_t d2bStrIdFlag =  padapter->ptrace->d2bStrIdPresent;
 
  merc_uint_t strId = GetStreamId(hMsg);

  if (!b2dStrIdFlag && !d2bStrIdFlag)
    return 1;

  if ((direction == MSD_DRV2BRD_TRACE) && d2bStrIdFlag) {
    ptr = padapter->ptrace->StrIdFilterToBrd;
  } 

  if ((direction == MSD_BRD2DRV_TRACE) && b2dStrIdFlag) {
    ptr = padapter->ptrace->StrIdFilterToDrv;
  } 

  if (ptr == NULL)
    return 1;

  count = *ptr;
  
  for (i=1; i<=count; i++) {
    if (*(ptr + i) == strId) 
      return 1;
  }

  return 0;
}


/***************************************************************************
 * Function Name		: isMessageToBeTracedOnCompDesc
 * Function Type		: Utility Function
 * Inputs			: pmercd_adapter_block_sT
 *                                PMERC_HOSTIF_MSG
 *                                Trace Direction (MSD_DRV2BRD_TRACE or 
 *                                MSD_BRD2DRV_TRACE)
 * Outputs			: 1 if message needs to be traced
 *                                0 if message need not be traced
 * Calling functions		: supp_push_trace_msg 
 * Description			: This function checks the following criteria
 *                                Checks whether this message needs to 
 *                                traced based on the CompDesc filters in adapter's 
 *                                trace structure. If no messages are given
 *                                as a parameter to drvtrace application
 *                                it returns 1, which means all messages 
 *                                have to be traced.
 *                                If a message was given as a parameter 
 *                                to drvtrace application, then this 
 *                                funcion checks whether hMsg's CompDesc
 *                                matches the parameter msgs' CompDesc and returns 1
 *                                (if it matches, it needs to be traced)
 *                                or 0 (need not be traced)
 * Additional comments		:
 ****************************************************************************/
merc_int_t isMessageToBeTracedOnCompDesc(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction) {

  pmerc_uint_t ptr=NULL;
  merc_uint_t count,i;
  merc_int_t SrcFlag =  padapter->ptrace->SrcDescPresent;
  merc_int_t DstFlag =  padapter->ptrace->DstDescPresent;
  merc_uint_t valSrc1=0, valSrc2=0, valDst1=0, valDst2=0;

  valSrc1 = hMsg->Source;
  valDst1 = hMsg->Destination;
  valSrc2 = MERCURY_GET_SOURCE_COMPINST(hMsg);
  valDst2 = MERCURY_GET_DEST_COMPINST(hMsg);

#ifdef DEBUG_TRACE
  cmn_err(CE_CONT, "isMessageToBeTracedOnCompDesc : valSrc1 = %x valSrc2 = %x valDst1 = %x valDst2 = %x\n", 
	  valSrc1, valSrc2, valDst1, valDst2);
#endif

  if (!SrcFlag && !DstFlag) 
    return 1;

  if (DstFlag) {
    ptr = padapter->ptrace->DstDesc;

    if (ptr != NULL) {

      count = *ptr;
      
      ptr++;
      for (i=0; i<=count; i=i+2) {
#ifdef DEBUG_TRACE
	cmn_err(CE_CONT, "isMessageToBeTracedOnCompDesc (Dst): ptr + 1 = %x ptr + 2 = %x\n", *(ptr + i), *(ptr + i + 1));
#endif
	if (( *(ptr + i)  == valDst1) && ( *(ptr + i + 1) == valDst2))
	  return 1;
      }
    }
  }

  if (SrcFlag) {
    ptr = padapter->ptrace->SrcDesc;

    if (ptr != NULL) {

      count = *ptr;
      
      ptr++;
      for (i=0; i<=count; i=i+2) {
#ifdef DEBUG_TRACE
	cmn_err(CE_CONT, "isMessageToBeTracedOnCompDesc (Src): ptr + 1 = %x ptr + 2 = %x\n", *(ptr + i), *(ptr + i + 1));
#endif
	if (( *(ptr + i)  == valSrc1) && ( *(ptr + i + 1) == valSrc2))
	  return 1;
      }
    }
  } 

  return 0;
}


/***************************************************************************
 * Function Name		: GetStreamId
 * Function Type		: Utility Function
 * Inputs			: PMERC_HOSTIF_MSG msg
 * Outputs			: stream id of the message 
 *                                MERC_INVALID_STREAMID if there is no streamid
 * Calling functions		: supp_push_trace_msg 
 * Description			: Gets the streamid 
 * Additional comments		:
 ****************************************************************************/
merc_uint_t GetStreamId(PMERC_HOSTIF_MSG hMsg) {

  merc_ulong_t MessageId;

  MessageId = MERCURY_GET_MESSAGE_TYPE(hMsg);
  
  if (MessageId == QCNTRL_CAN_TAKE) {
    PHIF_CAN_TAKE BodyPtr;
    PHIF_CAN_TAKE_ENTRY EntryPtr;
    
    BodyPtr = (PHIF_CAN_TAKE) MERCURY_BODY_START(hMsg);
    EntryPtr = (PHIF_CAN_TAKE_ENTRY)(&(BodyPtr->Entry));
    return (EntryPtr->StreamId);     
  }

  if (MessageId ==  QCNTRL_OPEN_STREAM) {
    PHIF_OPEN_STREAM BodyPtr;
    BodyPtr = (PHIF_OPEN_STREAM) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  if (MessageId == QCNTRL_OPEN_STR_ACK) {
    PHIF_OPEN_STREAM_ACK BodyPtr;
    BodyPtr = (PHIF_OPEN_STREAM_ACK) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  if (MessageId == QCNTRL_CLOSE_STREAM) {
    PHIF_CLOSE_STREAM BodyPtr;
    BodyPtr = (PHIF_CLOSE_STREAM) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  if (MessageId == QCNTRL_FAILED) {
    PHIF_FAILED BodyPtr;
    BodyPtr = (PHIF_FAILED) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  if (MessageId == QCNTRL_SUCCEEDED) {
    PHIF_SUCCESS BodyPtr;
    BodyPtr = (PHIF_SUCCESS) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  if (MessageId == QCNTRL_SESS_CLOSED) {
    PHIF_SESS_CLOSED BodyPtr;
    BodyPtr = (PHIF_SESS_CLOSED) MERCURY_BODY_START(hMsg);
    return (BodyPtr->StreamId);         
  }

  return MERC_INVALID_STREAMID;
}
