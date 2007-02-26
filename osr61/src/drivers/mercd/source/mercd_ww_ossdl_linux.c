/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_ww_ossdl_linux.c
 * Description                  : OS specific WW driver layer
 *
 *
 **********************************************************************/

#include "msd.h"
#include "../include/msdpciif.h"
#define MERCD_OSSDL_SOLARIS_C
#include "msdextern.h"
#undef MERCD_OSSDL_SOLARIS_C



//#define printdbg printk
extern int DPC_Disabled;

/***************************************************************************
 * Function Name                : linux_ww_intr_srvc
 * Function Type                : Interrupt functions
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver mercd_intr_srvc Routine.
 *                                This is the interrupt service routine
 *                                registered and called on interrupt by linux
 *                                WW driver for DM3 Plx.
 * Additional comments          : WWPENDING
 ****************************************************************************/

irqreturn_t linux_ww_intr_srvc(int InterruptVector, void *dev_id, struct pt_regs *regs)
{
#ifndef LFS
        merc_uint_t                    ConfigId = 0;
#endif
        merc_uint_t                    IntrReason;
	merc_uchar_t 		       need_ww_int = 0;
        pmercd_ww_dev_info_sT          pwwDev;
        mercd_dhal_ww_intr_check_sT    wwintrinfo = { 0 };
        pmercd_adapter_block_sT        padapter;


        padapter = (pmercd_adapter_block_sT) dev_id;
        if (!padapter) {
	    return IRQ_NONE;
        }

        //The following check is very important for a 3rd Rock board.
        //As soon we register a interrupt srvc() during attach time
        //there could be a shared interrupt from other devices. But
        //we alloc mem for pww_info only in set_param. Hence we try
        //get to a stale pointer.
        if (!(padapter->pww_info)) {
           return IRQ_NONE;
        }

	if (!MSD_SPIN_TRYLOCK(&padapter->phw_info->intr_info->intr_mutex)) {
            return IRQ_NONE;
        }  

        pwwDev = padapter->pww_info;

        // Check if it is Our Interrupt. Interrrupt C/S register 13th Bit
        // Should tell us.
        wwintrinfo.padapter = padapter;
        (*mercd_dhal_func[MERCD_DHAL_WW_INTR_CHECK])((void *)&wwintrinfo);

        if (wwintrinfo.ret != MD_SUCCESS) {
            MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex); 
            return IRQ_NONE;
        }

        //Check again to see if we switched over to WW mode
        if (pwwDev->state == MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING) {
	    padapter->flags.WWFlags &= ~MERCD_ADAPTER_WW_MODE_NOTENABLED;
            padapter->flags.WWFlags |= MERCD_ADAPTER_WW_I20_MESSAGING_READY |
                                       MERCD_ADAPTER_WW_MODE_ENABLED;
        }

        // get the interrupt reason. Only for Plx and 80321
        (*mercd_dhal_func[MERCD_DHAL_WW_GET_INTR_REQUEST])((void *)&wwintrinfo);

        IntrReason = wwintrinfo.intrReason;

        // clear the interrupt now since the interrupt is saved in "IntrReason"
        (*mercd_dhal_func[MERCD_DHAL_WW_CLEAR_INTR_REQUEST])((void *)&wwintrinfo);

	if (!padapter->pww_info->intr_reason )
	    padapter->pww_info->intr_reason  =  IntrReason;
	
        if(padapter->phw_info->un_flag)
                need_ww_int=0;
        else {
                need_ww_int=1;
        	padapter->phw_info->un_flag |= UNIT_BUSY;
        	padapter->flags.RecvPending |= MERC_ADAPTER_FLAG_RECEIVE_PEND;
	}

        MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);

        if (need_ww_int) {

            MSD_LEVEL2_DBGPRINT("dpc_ww_task: slotn = %d \n", padapter->phw_info->slot_number);

	    //Scheduling can be done in different ways as shown below
	    if (DPC_Disabled) {
#ifdef LFS
                mercd_ww_generic_intr_processing( padapter->phw_info->slot_number);
#else
                mercd_ww_generic_intr_processing( (void *) &padapter->phw_info->slot_number);
#endif
            } else {
	        tasklet_hi_schedule(&(padapter->phw_info->intr_info->dpc_task));
            }

            //atomic_inc(&padapter->pww_info->pww_counters->dpc_scheduled);
        } else  {
	    MSD_LEVEL2_DBGPRINT("No scheduling this time...\n");
	}

        return IRQ_HANDLED;
}

/***************************************************************************
 * Function Name                : linux_ww_pci_intr_check
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_ww_intr_check(void *rcvPtr)
{
   merc_uint_t                     IntrReg;
   pmercd_adapter_block_sT         padapter;
   pmercd_dhal_ww_intr_check_sT    pwwintr;
   pwwintr = (pmercd_dhal_ww_intr_check_sT)rcvPtr;
   padapter = pwwintr->padapter;

   //MSD_FUNCTION_TRACE("linux_pci_ww_intr_check", ONE_PARAMETER,
   //                         (size_t) rcvPtr);

   pwwintr->ret = MD_SUCCESS;

   if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
           MsdWWPlxGetIntrReg(IntrReg)
           if (!(IntrReg & MSD_PCI_INTR_DOORBELL_FROM_PLX)) {
               pwwintr->ret = MD_FAILURE;
               return;
           }

   } else { /* boardFamilyType */
        switch(padapter->phw_info->pciSubSysId)
           {
               case SUBSYSID_21554:
               case SUBSYSID_ROZETTA_21554:
                    MsdWW21554GetIntrReg(IntrReg)
                    //IntrReg == 0, implies interrupt is not generated
                    //by our board.
                    if (!(IntrReg & MSD_PCI_INTR_DOORBELL_FROM_21554)) {
                       pwwintr->ret = MD_FAILURE;
                       return;
                    }
                    //What ever we just read is the type of
                    //interrupt on 21554
                    pwwintr->intrReason = IntrReg;
                    break;

               case SUBSYSID_80321:
                    MsdWW80321GetIntrReg(IntrReg)
                    if (!(IntrReg & MSD_PCI_INTR_DOORBELL_FROM_80321)) {
                       pwwintr->ret = MD_FAILURE;
                       return;
                    }
                    break;

               default:
                    cmn_err(CE_WARN, "Dont know Who sent the Intr!!!!\n");
                    pwwintr->ret = MD_FAILURE;
                    break;
           } /* switch */

   } /* if */

 return;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_get_intr_request
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_ww_get_intr_request(void *rcvPtr)
{
   pmercd_adapter_block_sT         padapter;
   pmercd_dhal_ww_intr_check_sT    pwwintr;

   //MSD_FUNCTION_TRACE("linux_pci_ww_get_intr_request", ONE_PARAMETER,
   //                         (size_t) rcvPtr);

   pwwintr = (pmercd_dhal_ww_intr_check_sT)rcvPtr;
   padapter = pwwintr->padapter;

   pwwintr->ret = MD_SUCCESS;

   if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
           MsdWWPlxGetIntrRequest(pwwintr->intrReason)

   } else { /* boardFamilyType */
        switch(padapter->phw_info->pciSubSysId)
           {
               case SUBSYSID_21554:
               case SUBSYSID_ROZETTA_21554:
                    //Nothing needs to be done for 21554
                    //We alredy read the contents of the
                    //B2H_INTR_REG during the intr_check.
                    //That gave us the type of interrupt.
                    break;

               case SUBSYSID_80321:
                    MsdWW80321GetIntrRequest(pwwintr->intrReason)
                    break;

               default:
                    cmn_err(CE_WARN, "Dont know Who sent the Intr!!!!\n");
                    pwwintr->ret = MD_FAILURE;
                    break;
           } /* switch */

   } /* if */

 return;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_clear_intr_request
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_ww_clear_intr_request(void *rcvPtr)
{
   pmercd_adapter_block_sT         padapter;
   pmercd_dhal_ww_intr_check_sT    pwwintr;

   //MSD_FUNCTION_TRACE("linux_pci_ww_clear_intr_request", ONE_PARAMETER,
   //                         (size_t) rcvPtr);

   pwwintr = (pmercd_dhal_ww_intr_check_sT)rcvPtr;
   padapter = pwwintr->padapter;

   pwwintr->ret = MD_SUCCESS;

   if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
           MsdWWPlxClearIntrRequest(pwwintr->intrReason)
   } else { /* boardFamilyType */
        switch(padapter->phw_info->pciSubSysId)
           {
               case SUBSYSID_21554:
               case SUBSYSID_ROZETTA_21554:
                    MsdWW21554ClearIntrRequest(pwwintr->intrReason)
                    break;

               case SUBSYSID_80321:
                    MsdWW80321ClearIntrRequest(pwwintr->intrReason)
                    break;

               default:
                    cmn_err(CE_WARN, "Dont know Who sent the Intr!!!!\n");
                    pwwintr->ret = MD_FAILURE;
                    break;
           } /* switch */

   } /* if */

 return;
}
