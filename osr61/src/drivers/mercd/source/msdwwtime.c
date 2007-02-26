/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwtime.c
 * Description                  : WW Time functions
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDTIME_C_
#include "msdextern.h"
#undef _MSDTIME_C_

/***************************************************************************
 * Function Name                : time_ww_chk_snd_timer
 * Function Type                : Time function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver CheckSendTimer Routine.
 *                                This routine sets the PAM send timer which
 *                                when kicked off, will cause the driver to
 *                                send all messages and bulk data to the target
 *                                adapter.
 * Additional comments          :
 ****************************************************************************/
md_status_t time_ww_chk_snd_timer(pmercd_adapter_block_sT padapter)
{
        md_status_t     Status;
        mercd_osal_timeout_start_sT timeoutinfo = { 0 };

        MSD_FUNCTION_TRACE("time_ww_chk_snd_timer 0x%x\n", (size_t) padapter);

        MSD_ASSERT(padapter);

        if(( padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND ) ||
          ( padapter->state ==  MERCD_ADAPTER_STATE_MAPPED ) ||
	    (!(padapter->pww_info->mfaBaseAddressDescr.host_address))) {
           return(MD_SUCCESS);
        }

        padapter->flags.SendTimeoutPending |= MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND;

        timeoutinfo.Interval = padapter->phw_info->timer_info->snd_timeout_interval;
        timeoutinfo.Function = (PMSD_FUNCTION)mercd_ww_generic_timeout_processing;
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
