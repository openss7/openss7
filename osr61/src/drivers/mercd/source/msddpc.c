/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msddpc.c
 * Description			: Generic Deferred processing functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDDPC_C_
#include "msdextern.h"
#undef _MSDDPC_C_

typedef merc_void_t (*FUNCTION)(merc_uint_t);
#define UUFUNCTION merc_int_t (*)(pmerc_void_t)

extern int NewCanTakeProtocol;
extern int No_Send_From_DPC;
extern int ReceiveDMA;
extern int SendDMA;

/***************************************************************************
 * Function Name		: mercd_generic_intr_processing
 * Function Type		: DPC function
 * Inputs			: Context1
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver InterruptDeferredProcessing
 *				  Routine. This routine does the work for Intr.
 *				  at a later stage.
 * Additional comments		:
 ****************************************************************************/
int mercd_generic_intr_processing(merc_ulong_t Context1)
{
	pmercd_adapter_block_sT padapter;
	merc_uchar_t 		HostRamReq;
	size_t	 		cnt; 


	// First Get the adapter code in the structure
	cnt = *(merc_uchar_t * )Context1;

	padapter = MsdControlBlock->padapter_block_list[cnt];

	if (!padapter) {
	    return(MERCD_INTR_UNCLAIMED);
	}

	if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
	    return(MERCD_INTR_UNCLAIMED);
	}

	MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);

	if (!(padapter->flags.RecvPending & MERC_ADAPTER_FLAG_RECEIVE_PEND)) {
	    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	    return(MERCD_INTR_UNCLAIMED);
	}

	padapter->phw_info->un_flag |= UNIT_BUSY;

	// Validate adapter state
	if ((padapter->state == MERCD_ADAPTER_STATE_OUT_OF_SERVICE) ||
	    (padapter->state == MERCD_ADAPTER_STATE_CRASHED) ||
	    (padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) ||
	    (padapter->state == MERCD_ADAPTER_STATE_OUT_OF_SYNC )) {
		MSD_ERR_DBGPRINT("MSDDpc: Adapter not in READY state "
		    "padapter=%x, AdapterNumber=%d state is %x ",
		    padapter, cnt, padapter->state);
		padapter->phw_info->un_flag = 0;
		padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;
    		MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
		return(MERCD_INTR_CLAIMED);
	}


	// DMA completion interrupt arrived ..... 
	// Handles both receive and Send DMA completions. Again only Receive DMA is tested.
	if (padapter->flags.DMAIntrReceived  ==
	   (MERC_ADAPTER_FLAG_RCV_DMA_INTR | MERC_ADAPTER_FLAG_DMA_INTR)) {
	    // Removed to cover Linux specific issue
	    // MERC_ADAPTER_FLAG_DPC_DMA_PENDING

	    // Post the buffers upstreams for Receive DMA, 
	    // release DMA resources and free message buffers for Send DMA
	    postprocessDMA(padapter);

	    padapter->phw_info->un_flag = 0;
	    
	    MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	    return(MERCD_INTR_CLAIMED);
	} else {  
	    /*************************************************************/

	    // No DMA interrupt, usual processing
	    MERC_GET_SRAM_LOCK((&padapter->phost_info->reg_block), HostRamReq);

	    if (!(HostRamReq & MERC_HOST_RAM_GRANT_FOR_CLR_R)) {
		MSD_LEVEL2_DBGPRINT("Failed to get the sram \n");
#ifdef DRVR_STATISTICS
		padapter->fw_no_sram_lock++;
#endif
#if 0
		if (!(padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND))
		    time_set_host_ram_sem_timer(padapter);
#endif /* Not Working */
			
		padapter->phw_info->un_flag = 0;
		padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;
		goto out2;
	    }

	    padapter->flags.SramOurs |= MERC_ADAPTER_FLAG_SRAM_IS_OURS;

	    if (padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND ) {
		mercd_osal_timeout_stop_sT timeoutinfo;
		padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND;
#if defined LiS || defined LIS || defined LFS
		timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id_sem;
#else
		timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id_sem;
#endif
		(*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);
	    } else {
	    }

	    // check whether we have the sram lock
	    if (!(padapter->flags.SramOurs  & MERC_ADAPTER_FLAG_SRAM_IS_OURS)) {
		MSD_ERR_DBGPRINT("MSDDpc:Don't have SRAM\n"); 
		padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
		padapter->phw_info->un_flag = 0;
		padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;
		goto out;
	    }

	    padapter->phw_info->un_flag = 0;

	    // if this is a config ack intr. Process it 
	    if (padapter->flags.HostifConfigSent & MERC_ADAPTER_FLAG_HOSTIF_CONFIG_SENT) {
		padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;
		supp_cfg_ack(padapter);
		goto out;
	    }

	    if (padapter->flags.RecvPending & MERC_ADAPTER_FLAG_RECEIVE_PEND) {
		padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;
		// process host ram receive message queue first 
		rcv_msgs_from_adapter(padapter);

		// Code is added to handle buffer unavailability problem
		// ReceiveDMA shows whether Recieve DMA is enabled or disabled.
		if (rcv_data_from_adapter(padapter,ReceiveDMA) != MD_SUCCESS) {

		    // We are doing a DMA, get out asap.
	            if (padapter->phw_info->pdma_info->rx_dma_count) {
		    	goto out2;
		    }
		    goto out;
	        }

	        if (padapter->phw_info->pdma_info->rx_dma_count) {
		    goto out2;
		}

	    }

	    if (!No_Send_From_DPC) {
	        // now see if we have anything to send 
	        if (!MSD_QUEUE_EMPTY(&padapter->snd_msg_queue)) {
		    snd_msgs2adapter(padapter);	
	        }

	        // Please notice that send DMA is disabled. 
	        if (padapter->stream_adapter_list.QueueHead) {
	  	    snd_data2adapter(padapter);	
	        }

	    }


	    // now clear the proper interrupts before letting go fo this adapter
out:
	    if (No_Send_From_DPC) {
		// must release sram lock so fw can access it 
		MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
		padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
	    } else {
	        // we're done with the board 

	        if (padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR) {
		    // sending intr automatically releases sram lock 
		    padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;

		    // shouldn't need FREE sram here 
		    MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
		    MERC_INTR_ADAPTER((&padapter->phost_info->reg_block));
		    // indicate we no longer have semaphore in adapter block 
		    padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
	        } else {
		    // must release sram lock so fw can access it 
		    MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
		    padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
	        }
	    }

	    if (NewCanTakeProtocol) {
	        strm_snd_can_takes(padapter);
	    }
    	}
out2:
	MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
	return(MERCD_INTR_CLAIMED);
}

/***************************************************************************
 * Function Name		: mercd_generic_timeout_processing
 * Function Type		: DPC function
 * Inputs			: padapter
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver mercd_timeout_dpc
 *				  Routine. This routine is called on system
 *				  timeout to send data and messages
 * Additional comments		:
 ****************************************************************************/
void mercd_generic_timeout_processing(pmercd_adapter_block_sT padapter)
{
	merc_uchar_t	HostRamReq;
#ifndef LFS
        int retVal = 0;
#endif

	MSD_ASSERT(padapter);

	MSD_FUNCTION_TRACE("mercd_timeout_dpc", ONE_PARAMETER, (size_t)padapter);

	if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
	    return;
	}

	// Please note that OUT_OF_SERVICE condition is enabled ..... 
	if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) || 
	    (padapter->state == MERCD_ADAPTER_STATE_SUSPENDED)         ||
	    (padapter->state == MERCD_ADAPTER_STATE_MAPPED)            ||
	    (padapter->state == MERCD_ADAPTER_STATE_OUT_OF_SERVICE)) {
		padapter->flags.SendTimeoutPending = 0;
		return;
	}

	MSD_ENTER_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);  

	// see if the send queue is empty 
	// Please notice the paranthesiz,  the code get out if dma is pending.... 
	if ((MSD_QUEUE_EMPTY(&padapter->snd_msg_queue) && 
	    !(padapter->stream_adapter_list.QueueHead)) ||
	    (padapter->phw_info->pdma_info->rx_dma_count)) {
		goto out;
	}
	
	MERC_GET_SRAM_LOCK((&padapter->phost_info->reg_block), HostRamReq);

        if (!(HostRamReq & MERC_HOST_RAM_GRANT_FOR_CLR_R)) {
#ifdef DRVR_STATISTICS
	    padapter->fw_no_sram_lock++;
#endif
#if 0
	    if (!(padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND))
		time_set_host_ram_sem_timer(padapter);
#endif /* Not Working */
	    goto out;
	} else {
	    padapter->flags.SramOurs |= MERC_ADAPTER_FLAG_SRAM_IS_OURS;

	    if (padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND) {
            	mercd_osal_timeout_stop_sT timeoutinfo;
            	padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND;
#if defined LiS || defined LIS || defined LFS
		timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id_sem;
#else
            	timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id_sem;
#endif
		(*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);
	    }

	    // need to clear recv pend flag if timer beat dpc 
	    if (padapter->flags.RecvPending & MERC_ADAPTER_FLAG_RECEIVE_PEND) {
                padapter->flags.RecvPending &= ~MERC_ADAPTER_FLAG_RECEIVE_PEND;

                // process host ram receive message queue first
                rcv_msgs_from_adapter(padapter);

		if (rcv_data_from_adapter(padapter,ReceiveDMA) != MD_SUCCESS) {
		    // We are doing a DMA, get out asap.
                    if (padapter->phw_info->pdma_info->rx_dma_count) {
                        goto out;
                    }
		    goto out2;
                }

                if (padapter->phw_info->pdma_info->rx_dma_count) {
                    goto out;
                }
	    }

	    if (!MSD_QUEUE_EMPTY(&padapter->snd_msg_queue)) {
		snd_msgs2adapter(padapter);
	    }

	    if (!(padapter->flags.DPCDmaPending & MERC_ADAPTER_FLAG_DPC_DMA_PENDING)) {
		snd_data2adapter(padapter);
		if (padapter->phw_info->pdma_info->tx_dma_count) {
		    //Enable Tx channel
		    //Initiate the DMA xfer from host to board
		    //Revisit this later while enabling xmits
		    padapter->flags.DPCDmaPending |=  MERC_ADAPTER_FLAG_DPC_DMA_PENDING;
		}
		padapter->flags.SendDataPending &= ~(MERC_ADAPTER_FLAG_SEND_DATA_PEND);
	    }
	}

out2 :
	if (padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR){
	    // sending intr automatically releases sram lock
	    padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;

	    // shouldn't need FREE sram here
	    MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
	    MERC_INTR_ADAPTER((&padapter->phost_info->reg_block));
	    // indicate we no longer have semaphore in adapter block
	    padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
	} else {

            // must release sram lock so fw can access it
            MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
            padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
        }

	if (NewCanTakeProtocol) {
	    strm_snd_can_takes(padapter);
	}

out :
      padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;
      time_chk_snd_timer(padapter); 
      MSD_EXIT_MUTEX_IRQ(&padapter->phw_info->intr_info->intr_mutex);
}
