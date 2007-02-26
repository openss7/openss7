/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdtime.c
 * Description			: Time functions 
 *
 *
 **********************************************************************/
#include "msd.h"
#define _MSDTIME_C_
#include "msdextern.h"
#undef _MSDTIME_C_


/***************************************************************************
 * Function Name		: time_set_host_ram_sem_timer
 * Function Type		: Time function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver SetHostRamSem Routine. 
 *				  This routine is used for board death 
 *				  identification purpose. If timer expires 
 *				  and host Ram semaphore was not received the 
 *				  board is assumed dead.(What if the host ram 
 *				  is with host already?)
 ****************************************************************************/
md_status_t time_set_host_ram_sem_timer(pmercd_adapter_block_sT padapter) 
{
	md_status_t   Status;
	mercd_osal_timeout_start_sT timeoutinfo = { 0 };

	//MSD_FUNCTION_TRACE("time_set_host_ram_sem_timer", ONE_PARAMETER, (size_t)padapter);

	MSD_ASSERT(padapter);

	// check if send timer has already started 
	if(padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND){
		return(MD_SUCCESS);
	}

	// if no, indicate we've set the timeout and start the timer */
	padapter->flags.SendTimeoutPending  |= MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND;

	timeoutinfo.Interval = MSD_HRAM_SEM_TIMEOUT;
	timeoutinfo.Function = (PMSD_FUNCTION)time_host_ram_timeout;
	timeoutinfo.Context = padapter;
	timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id_sem;

	(*mercd_osal_func[MERCD_OSAL_TIMEOUT_START])((void *)&timeoutinfo);

	Status = timeoutinfo.ret;

	if(Status != MD_SUCCESS) {
		MSD_ERR_DBGPRINT("time_set_host_ram_sem_timer: timeout failed.\n");
		MSD_ASSERT(Status==MD_SUCCESS);
		padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND;
		return(MD_FAILURE);
	}
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: time_host_ram_timeout
 * Function Type		: Time function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver PamHostRamTimeout Routine. 
 *				  This routine is used for board death 
 *				  identification purpose. It will be called 
 *				  when timer expires and host Ram semaphore 
 *				  was not received the board state is  set to
 *				  out of service.
 * Additional comments		:
 ****************************************************************************/
void time_host_ram_timeout(pmercd_adapter_block_sT padapter)
{

#ifndef LFS
    mercd_osal_timeout_start_sT timeoutinfo = { 0 };
#endif

    //MSD_FUNCTION_TRACE("time_host_ram_timeout", ONE_PARAMETER, (size_t)padapter);
    MSD_ASSERT(padapter);

    // check if send timer has already started 
    if (!(padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND)){
        MSD_ERR_DBGPRINT("time_host_ram_timeout: pending Flag not set.\n");
	return;
    }

    // must clear the flag
    padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND;


    // Place adapter out of service
    // padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
    cmn_err(CE_WARN,"mercd: Unable to communicate with board (> 2 sec)\n");
}


/***************************************************************************
 * Function Name		: time_chk_snd_timer
 * Function Type		: Time function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver CheckSendTimer Routine. 
 *				  This routine sets the PAM send timer which 
 *				  when kicked off, will cause the driver to 
 *				  send all messages and bulk data to the target 
 *				  adapter.
 * Additional comments		:
 ****************************************************************************/
md_status_t time_chk_snd_timer(pmercd_adapter_block_sT padapter)
{
	md_status_t	Status;
#ifndef LFS
	static int i;
#endif
	mercd_osal_timeout_start_sT timeoutinfo = { 0 };

        //MSD_FUNCTION_TRACE("time_chk_snd_timer", ONE_PARAMETER, (size_t)padapter);
	MSD_ASSERT(padapter);

	// if no, indicate we've set the timeout and start the timer 

	if(( padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND ) || 
	  ( padapter->state ==  MERCD_ADAPTER_STATE_MAPPED ) || 
	  ( padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS ) ) {
	 
        return(MD_SUCCESS);
	}

	
	padapter->flags.SendTimeoutPending |= MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;


	timeoutinfo.Interval = padapter->phw_info->timer_info->snd_timeout_interval;
	timeoutinfo.Function = (PMSD_FUNCTION)mercd_generic_timeout_processing;
	timeoutinfo.Context = padapter;
	timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id;

	(*mercd_osal_func[MERCD_OSAL_TIMEOUT_START])((void *)&timeoutinfo);

    Status = timeoutinfo.ret;

	if(Status != MD_SUCCESS) {
		MSD_ASSERT(Status==MD_SUCCESS);
		padapter->flags.SendTimeoutPending &= ~MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;
		return(MD_FAILURE);
	}

	return(MD_SUCCESS);
}
