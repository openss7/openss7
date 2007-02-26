/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdbrdutl.c
 * Description			: Support for Board Manager functions
 *		 		 
 *
 *
 ***********************************************************************/

#include "msd.h"
#include "msdpciif.h"
#define _MSDBRDUTL_C_
#include "msdextern.h"
#undef _MSDBRDUTL_C_

merc_uint_t vstartbd = 0xffff;

/***************************************************************************
 * Function Name		: start_brd
 * Function Type		: Common Hardware function 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver StartBoard Routine. This
 *				  routine starts a Mercury board
 * Additional comments		:
 ****************************************************************************/
md_status_t start_brd(PMSD_OPEN_BLOCK MsdOpenBlock, pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{
	pmercd_reg_block_sT 	RegBlock;
	merc_uint_t             WaitCount;
	merc_uchar_t            InUchar;
	merc_uchar_t           	DebugVal;
	mercd_dhal_mem_copy_sT 	meminfo = { 0 };
    merc_uint_t             slotn;

	MSD_FUNCTION_TRACE("start_brd", ONE_PARAMETER, (size_t)padapter);

	// Enable interrupt - done after ISR registration
	// For SRAM 1.9 board users ...... 
#ifdef LFS
    RegBlock = &padapter->phost_info->reg_block;
#endif
	
    if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {

#ifndef LFS
	RegBlock = &padapter->phost_info->reg_block;
#endif

	// Request the host ram without asking for interrupt
	MERC_GET_SRAM_NO_INTR_LOCK(RegBlock, InUchar);

	WaitCount = 0;

	while(!(InUchar & MERC_HOST_RAM_GRANT_R)) {


		MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);

		if (((++WaitCount)*MERC_BOARD_RESET_WAIT_INTERVAL) > MERC_BOARD_RESET_TIMEOUT){

			MSD_ERR_DBGPRINT( "start_brd: ERR: HostRam Time Out\n");

			return(CD_ERR_HOST_RAM_TIMEOUT);

		}   

		MERC_GET_SRAM_NO_INTR_LOCK(RegBlock, InUchar);
	}



	// Check the State of Debug port values, if they are not Result SUCC
	// from Kernel, return error saying board not ready.

	MERC_CHECK_DEBUG_ONE(RegBlock, DebugVal);

	// DMV-B
	if ((DebugVal != FIRST_DEBUG_VALID_RESULT) &&
            ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	     (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI))) {
		// Release the SRAM

		MERC_FREE_SRAM_LOCK(RegBlock);

		// Return this error since the downloader will try again if it
		// is this error. Originally was returning MD_FAILUER

		return(CD_ERR_INVALID_DEBUG_PORT);

	}

	MERC_CHECK_DEBUG_TWO((&padapter->phost_info->reg_block), DebugVal);

	// DMV-B
	if ((DebugVal != SECOND_DEBUG_VALID_RESULT) &&
            ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	     (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI))) {

		// Release the SRAM

		MERC_FREE_SRAM_LOCK(RegBlock);

		// Return this error since the downloader will try again if it
		// is this error. Originally was returning MD_FAILUER

		return(CD_ERR_INVALID_DEBUG_PORT);
	}

    }  /* THIRD_ROCK_FAMILY */
      else
      {
        //There is no SRAM grant for THIRD_ROCK_FAMILY
      }
 
        vstartbd = 0xfff6;
          /* transfer the config block to host ram
           Common macro for device copy the last 2 fields will be
           skipped for VME
           The MSD_DEVICE_COPY -> vme_device_copy for VME platforms
                                  pci_device_copy for PCI platforms*/

        meminfo.src = (pmerc_char_t)&padapter->phost_info->host_config;
	if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
            meminfo.dest = (pmerc_char_t)padapter->phost_info->reg_block.HostRamStart;
	} else {
	   // please define 512 in a header file and use here
	   meminfo.dest = (pmerc_char_t)padapter->phost_info->reg_block.HostRamStart + 512;
    	}
        meminfo.size = sizeof(MERCURY_HOST_IF_CONFIG);

	memcpy(meminfo.dest, meminfo.src, meminfo.size);


        MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
        padapter->flags.HostifConfigSent |=MERC_ADAPTER_FLAG_HOSTIF_CONFIG_SENT;
        MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	
        //For 3rd Rock we should just return. No need to send any intr.
        //For SRAM boards:  don't really want to set CD adapter state here
        // but it makes things easier for now
        // indicate we're waiting for a config ack from fw
        //SRAM DM3 Boards ARE NOT 3rd ROCK. Hence falls through the
        //following branch.
        if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY)
         {
            if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	        (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
                MERC_INTR_ADAPTER(RegBlock);
                //////////////////SRAMDMA BEGIN/////////////////////////////////
                slotn = mercd_adapter_map[padapter->adapternumber];
		
	        // SAVE the sram address on the local bus, 
	        // it will be used for DMA ...... 
	        padapter->phw_info->pdma_info->dma_base_offset = 
		   (MsdPlxGetPlxLocalBaseAddr(slotn)) + MSD_CONFIG_ROM_MAX_SIZE;

	        // Chan 0 
                if (padapter->phw_info->pdma_info->total_merc_tx_dma_chains)
                {
                  MsdPlxDmaEnableSctr_Mode_Chn0(slotn);
                  MsdPlxDmaEnable_Chn0(slotn);
                }
                //Chan 1
                if (padapter->phw_info->pdma_info->total_merc_rx_dma_chains)
                {
                  MsdPlxDmaEnableSctr_Mode_Chn1(slotn);
                  MsdPlxDmaEnable_Chn1(slotn);
                }
                //////////////////SRAMDMA END/////////////////////////////////
            } else { // DMV-B
		MERC_FREE_SRAM_LOCK(RegBlock);
		mid_wwmgr_configure_brd_to_ww(MsdOpenBlock, padapter, Msg);
                return(MD_SUCCESS);
            }
	   
         }
        return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: reset_adapter
 * Function Type		: Common Hardware function 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver ResetAdapter Routine. This
 *				  routine Resets the Mercury controller
 * Additional comments		:
 ****************************************************************************/
void reset_adapter(pmercd_adapter_block_sT  padapter)
{
  merc_uint_t  data;
#ifndef LFS
  pmerc_ulong_t loc;
#endif
  mercd_dev_info_T *Dip;

        MSD_FUNCTION_TRACE("reset_adapter", ONE_PARAMETER, (size_t)padapter);

  if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
        if (padapter->phost_info->reg_block.SetBoardReset == NULL) {

                MSD_ERR_DBGPRINT("reset_adapter: RegisterBlock NULL\n");

                return;
        }

        MERC_SET_BOARD_RESET((&padapter->phost_info->reg_block));

        MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);

        MERC_CLEAR_BOARD_RESET((&padapter->phost_info->reg_block));
  } /* !THIRD_ROCK_FAMILY */
 else
  {
  //3rd Rock Family
    //21554: The reset register is in the PCI configuration space it self.
    //Not on any of those two spaces we mapped earlier @ virt_map_q[0]
    //virt_map_q[1]
    switch(padapter->phw_info->pciSubSysId)
        {
          case SUBSYSID_21554:
          case SUBSYSID_ROZETTA_21554:

               //Set the reset Interrupt
               data = 0x01;
               Dip = padapter->pdevi;
               MsdWW21554WriteToResetReg(data);

               //Sleep
               MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);

               //Add a New Functionality of clearing the post location
               //during the reset time
               mid_wwmgr_clear_3rdrock_postlocation(padapter); 

               //Sleep
               MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
               //Clear the Reset Interrupt
               data = 0x00;
               MsdWW21554WriteToResetReg(data);
               MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
               break;

          case SUBSYSID_80321:
               break;

          default:
               break;
       } /* switch */
  } /*THIRD_ROCK_FAMILY*/
}

/***************************************************************************
 * Function Name		: shutdown_adapter
 * Function Type		: Common Hardware function 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver ShutDown Routine. This
 *				  routine shutdowns a Mercury board
 * Additional comments		:
 ****************************************************************************/
md_status_t shutdown_adapter(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter)
{
   mercd_osal_timeout_stop_sT   timeoutinfo = { 0 };
   pmercd_ww_dev_info_sT        pwwDev;
   md_status_t                  status = MD_SUCCESS;

   mid_save_brd_params(padapter);

   //WW support
   if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
       //WW specific shutdown
       pwwDev = (pmercd_ww_dev_info_sT)padapter->pww_info;

       if (pwwDev == NULL) {
           printk("MercShutdownBoard: Unusual State...\n");
           printk("MercShutdownBoard: I20 Ready But No WW struct.\n");
           MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
           padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
           MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
           return(MD_FAILURE);
       }

       padapter->state = MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS;

       if (pwwDev->state == MERCD_ADAPTER_WW_SUCCEDED ) {
	   //We are Running in WW mode
           //Do the garbage in WW mode. Dealloc all the resources
           //for WW mode. We leave the pwwDev as it is.
	   if (padapter->state == MERCD_ADAPTER_STATE_OUT_OF_SERVICE) {
	       reset_adapter(padapter);
	   } else {
	       status = mid_wwmgr_shutdown_adapter(pwwDev, 0);
	   }

           //WWPENDING: we are leaving the state of padapter->flags.WWFlags
           //as MERCD_ADAPTER_WW_MODE_NOTENABLED and pwwDev->state to
           //MERCD_ADAPTER_WW_NOTREADY. But it should still have the
           //MERCD_ADAPTER_WW_I20_MESSAGING_READY bit set. We check this
           //at the time of detach and clear it after unmapping and freeing
           //the mappings
       }  else {
	   mid_save_brd_params(padapter);

	   //DMVB - shutdown the board in order to handle Cntrl-C
	   if (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) {
	       status = mid_wwmgr_shutdown_adapter(pwwDev, 0);
	   } else {
  	       reset_adapter(padapter);
	   }

	   if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
	      //release BH lock since we are using schedule_timeout in 3rdrock_running
	      MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
	      mid_wwmgr_check_3rdrock_running(padapter, MERCD_WW_BOOT_KERNEL);
	      MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
	   }
       }
   } else {
       //Perform the reset here for non-WW boards only
       padapter->state = MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS;
       mid_save_brd_params(padapter);
       reset_adapter(padapter);
     
       if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
 	   //release BH lock since we are using schedule_timeout in 3rdrock_running
           MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
       	   mid_wwmgr_check_3rdrock_running(padapter, MERCD_WW_BOOT_KERNEL);
	   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
       } else {
	   mid_sram_check_kernel_running(padapter);
       }
   }

   if (status == MD_SUCCESS) {
       padapter->rtkMode = 0;

       MSD_ENTER_MUTEX(&padapter->phw_info->timer_info->timer_flag_mutex );

      if (padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND) {
          MSD_EXIT_MUTEX(&padapter->phw_info->timer_info->timer_flag_mutex);
#if defined LiS || defined LIS || defined LFS
  	  timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id;
#else
	  timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id;
#endif /* LiS */
          (*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);

	  padapter->flags.SendTimeoutPending = 0;

          if (timeoutinfo.ret != MD_SUCCESS) {
            MSD_ERR_DBGPRINT("MercShutdownBoard: Untimeout failed.\n");
          }
  
      } else {
          MSD_EXIT_MUTEX(&padapter->phw_info->timer_info->timer_flag_mutex);
      }
   } else if (status == MD_PENDING) {
      // Kick off the timer to wait for the response. Currently status is
      // PENDING only for the WW based boards
      if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       	  (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
          time_ww_chk_snd_timer(padapter);
      }
   } 


   return(status);
}

/***************************************************************************
 * Function Name		: fill_attr
 * Function Type		: Common Hardware function 
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver FillBoardAttr Routine. This
 *				  routine fills the physical attributes like
 *				  bus, slot, sram addr, length, irq, vector,
 *				  etc. for all the boards found in the system.
 * Additional comments		:
 ****************************************************************************/
void fill_attr(PCFG_GET_PHYS_BOARD_ATTR_ACK AckPtr)
{
	merc_int_t i;
	PMD_PHYS_BOARD_ATTR BrdAttr;
	pmercd_adapter_block_sT padapter;

	MSD_FUNCTION_TRACE("fill_brd_attr", ONE_PARAMETER, (size_t)AckPtr); 

	AckPtr->BoardCount = MsdControlBlock->adapter_count;

	BrdAttr = (PMD_PHYS_BOARD_ATTR)(AckPtr + 1);

	for (i = 0; i < AckPtr->BoardCount && i < MSD_MAX_BOARD_ID_COUNT;
				 i++, BrdAttr++) {
		if ( mercd_adapter_map[i] == 0xFF ) /* listboards panic */
		    continue;

		// zero out unused variables to begin with 
		MSD_ZERO_MEMORY((pmerc_char_t)BrdAttr, sizeof(MD_PHYS_BOARD_ATTR));

		padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[i]];

		if( padapter == NULL )
			continue;

		BrdAttr->ConfigId = i;
		BrdAttr->BusType = padapter->phw_info->bus_type;
		BrdAttr->BusId = padapter->phw_info->bus_number;
		BrdAttr->SlotNumber = padapter->phw_info->slot_number;
		BrdAttr->PhysSramAddr = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr;
		BrdAttr->PhysSramSize = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count;
		BrdAttr->InterruptLevel = padapter->phw_info->intr_info->level;
		BrdAttr->InterruptVector = padapter->phw_info->intr_info->intr_vec;
		BrdAttr->InterruptSharable = 1;	// yes, sharable 
		BrdAttr->DmaChannel = MD_CFG_DMA_DISABLE; // not used 
                //PCI Slot Number for listboards
                BrdAttr->pciSlotNumber = padapter->phw_info->pci_slot_number;
	}

}
// ************************************************************ 
// Post Changes .......
// ************************************************************ 
md_status_t mid_sram_read_post_location( pmercd_adapter_block_sT padapter )
{
	merc_uchar_t            DebugVal, DebugVal2;
#ifndef LFS
	merc_uint_t             regValue;
	merc_uint_t             WaitCount;
	merc_uchar_t            InUchar;
#endif
	pmercd_reg_block_sT     RegBlock;	

	 RegBlock = &padapter->phost_info->reg_block; 

	 MERC_CHECK_DEBUG_ONE((&padapter->phost_info->reg_block), DebugVal); 

	 MERC_CHECK_DEBUG_TWO((&padapter->phost_info->reg_block), DebugVal2);

	 if (DebugVal !=  FIRST_DEBUG_VALID_RESULT)
        return(MERCD_WW_POST_IN_PROGRESS);
     else {}

	 MERC_CHECK_DEBUG_TWO((&padapter->phost_info->reg_block), DebugVal);

	 if (DebugVal != SECOND_DEBUG_VALID_RESULT)
		return(MERCD_WW_POST_IN_PROGRESS);
	 else
		return(MD_SUCCESS);

}
// ***************************************************************************
md_status_t mid_sram_check_kernel_running( pmercd_adapter_block_sT padapter )
{
	md_status_t             Status;
	merc_ulong_t            wcount = 0;
	merc_uint_t				wait_time;
#ifndef LFS
	merc_uchar_t            DebugVal;	
	merc_uint_t             regValue;
	merc_uint_t             WaitCount;
    merc_uchar_t            InUchar;
    pmercd_reg_block_sT     RegBlock;
#endif

    if (padapter->flags.PostFlags == MD_MIN_POST_ENABLE)  {
		 wait_time = 10000;
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
	 } else {
		 printk("Waiting now .... \n");
		 wait_time = 80000;
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
		 MsdBusyDelayMilliSec(MERC_BOARD_RESET_WAIT_INTERVAL);
	}
	Status = MD_FAILURE;

	do {
		Status = mid_sram_read_post_location(padapter);
		
		if (Status != MD_SUCCESS) {
			if(Status == MERCD_WW_POST_IN_PROGRESS) {
				if (++wcount > wait_time ){
					return(Status);
				 }
			 // cannot use schedule_timeout in bh level
			 MsdBusyDelayMilliSec(20);
		} else {
			  return (Status);
		}
	   } /* !MD_SUCCESS */ 
	 } while (Status != MD_SUCCESS);

	 return(MD_SUCCESS);	
}
// ***************************************************************************
