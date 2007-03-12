/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_ossdl_linux.c
 * Description                  : OS specific driver layer
 *
 *
 **********************************************************************/

#include <linux/version.h>
#ifdef LFS
#include <sys/kmem.h>
#endif

#include "msd.h"
#define MERCD_OSSDL_LINUX_C
#include "msdextern.h"
#undef MERCD_OSSDL_LINUX_C

#include "msdpciif.h"

/* 
 * This file defines the OSSDL interfaces functions for Linux 2.x
 * The following functions are present in this file 
 * 
 * linux_register_dpc
 * linux_deregister_dpc
 * linux_intr_register_intr_srvc
 * linux_intr_deregister_intr_srvc
 * linux_phys2virt
 * linux_free_virt_addr
 * linux_timeout
 * linux_untimeout
 * linux_intr_srvc
 */

extern int DPC_Disabled;

/***************************************************************************
 * Function Name                : linux_phys2virt
 * Function Type                : OSSDL and OSAL funtion
 * Inputs                       : rcvPtr
 * Outputs                      : Virtual Address 
 * Calling functions            : MERCD_OSAL_MEM_PHYS_VIRT Interfaces
 * Description                  : Solaris Implementation of Streams Driver
 *				  PhysicaToVirtual Routine. This routine maps
 *				  a physical address to Kernel virtual address.
 * Additional comments          :
 ****************************************************************************/
void linux_phys2virt(void *rcvPtr)
{
	pmercd_osal_mem_phys_virt_sT pmem = (pmercd_osal_mem_phys_virt_sT)rcvPtr;
	mercd_dhal_drvr_specific_sT drvr_specific = { 0 };
	pmerc_void_t kva = 0;

	MSD_FUNCTION_TRACE( "linux_phys2virt", ONE_PARAMETER, (size_t) rcvPtr);

        	
#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
	kva = vremap((size_t)pmem->PhysAddr, (size_t)pmem->ByteCount);
#else
	kva = ioremap((size_t)pmem->PhysAddr, (size_t)pmem->ByteCount);
#endif

	if ( ! kva ) {
		printk("PhysToVirt: failed PhysAddr=%x, Count=%x\n", pmem->PhysAddr, pmem->ByteCount);
		pmem->ret = NULL;
		return;
	}

	MSD_LEVEL2_DBGPRINT("PhysToVirt:Virt=%x, Phys=%x, Count=%x\n", 
	       kva, pmem->PhysAddr, pmem->ByteCount);

	/* 
         * Platform Specific function 
	 */
	drvr_specific.ConfigId = pmem->ConfigId;

	(*mercd_dhal_func[MERCD_DHAL_DRVR_SPECIFIC])((void *)&drvr_specific);

	pmem->ret = kva;

	return;
}

/***************************************************************************
 * Function Name                : linux_free_virt_addr
 * Function Type                : OSSDL and OSAL function
 * Inputs                       : rcvPtr
 * Outputs                      : MD_STATUS
 * Calling functions            : MERCD_OSAL_MEM_VIRT_FREE Interfaces
 * Description                  : Streams Driver FreeVirtualAddress Routine.
 *                                This routine frees the virtual address
 *                                mapped to the physical address.
 * Additional comments          :
 ****************************************************************************/
void linux_free_virt_addr(void *rcvPtr)
{
	pmercd_osal_mem_virt_free_sT pmem = (pmercd_osal_mem_virt_free_sT)rcvPtr;
        
	MSD_FUNCTION_TRACE("linux_free_virt_addr", ONE_PARAMETER, (size_t)rcvPtr);

#if LINUX_VERSION_CODE < 0x020100               /* 2.0 kernel */
	vfree((void *)pmem->VirtAddr);
#else
	iounmap((void *)pmem->VirtAddr);

#endif
	pmem->ret = MD_SUCCESS;

	return;
}


/***************************************************************************
 * Function Name                : linux_timeout
 * Function Type                : OSSDL and OSAL function
 * Inputs                       : rcvPtr
 * Outputs                      : MD_STATUS
 * Calling functions            : MERCD_OSAL_TIMEOUT_START Interfaces
 * Description                  : Streams Driver Timeout Routine. This routine
 *                                sets a Timeout interval for a board
 * Additional comments          :
 ****************************************************************************/
void linux_timeout(void *rcvPtr)
{
	pmercd_osal_timeout_start_sT ptimeout = (pmercd_osal_timeout_start_sT) rcvPtr;
	size_t hz_wait= (ptimeout->Interval * HZ)/1000;

	//MSD_FUNCTION_TRACE("linux_timeout", ONE_PARAMETER, (size_t)rcvPtr);

#if defined LiS || defined LIS || defined LFS
	*ptimeout->Handle = (size_t)timeout(ptimeout->Function, 
	                                    (caddr_t)ptimeout->Context, 
	                                    hz_wait);
#else
	do {
	    struct timer_list *adapter_timeout;

	    adapter_timeout = ptimeout->Handle;
	    init_timer(adapter_timeout);
	    adapter_timeout->function = ptimeout->Function;
	    adapter_timeout->expires = abstract_jiffies() + hz_wait;
	    adapter_timeout->data = (unsigned long)ptimeout->Context;
	    add_timer(adapter_timeout);
	} while(0);
#endif /* LiS */

	ptimeout->ret = MD_SUCCESS;

	return;
}


/***************************************************************************
 * Function Name                : linux_untimeout
 * Function Type                : OSSDL and OSAL function
 * Inputs                       : rcvPtr
 * Outputs                      : MD_STATUS
 * Calling functions            : MERCD_OSAL_TIMEOUT_STOP Interfaces
 * Description                  : Streams Driver Untimeout Routine. This
 *                                routine removes previously installed Timeout
 * Additional comments          :
 ****************************************************************************/
void linux_untimeout(void *rcvPtr )
{
	pmercd_osal_timeout_stop_sT ptimeout = (pmercd_osal_timeout_stop_sT) rcvPtr;

	MSD_FUNCTION_TRACE("linux_untimeout", ONE_PARAMETER, (size_t)rcvPtr);

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
	(void) untimeout((int)(long)ptimeout->Handle);
#else
	(void) untimeout((int)ptimeout->Handle);
#endif
#else
	del_timer(ptimeout->Handle);
#endif /* LiS */
	
	ptimeout->ret = MD_SUCCESS;

	return;
}

/***************************************************************************
 * Function Name                : linux_intr_register_intr_srvc
 * Function Type                : OSSDL and OSAL Interface function
 * Inputs                       : rcvPtr
 * Outputs                      : MD_STATUS
 * Calling functions            : MERCD_OSAL_INTR_REGISTER Interfaces
 * Description                  : Streams Driver intr_register_intr_srvc
 *                                Routine. This routine registers Mercury
 *                                Driver Interrupt service routine with the
 *                                Solaris Kernel
 * Additional comments          :
 ****************************************************************************/

void linux_intr_register_intr_srvc(void *rcvPtr)
{
	pmercd_osal_intr_register_sT pintr = (pmercd_osal_intr_register_sT)rcvPtr;
	merc_int_t rc;

	MSD_FUNCTION_TRACE("linux_intr_register_intr_srvc", 
	                   ONE_PARAMETER,
	                   (size_t)rcvPtr);

	if (pintr->ConfigId >= MSD_MAX_BOARD_ID_COUNT ) {
		printk("mercd: IntrReg: Failed invalid ConfigId: config id(%d), Max id(%d)\n", pintr->ConfigId, MSD_MAX_BOARD_ID_COUNT ) ;
		pintr->ret = MD_FAILURE;
		return;
	}


       //WW support: request_irq can register linux_intr_srvc or 
       //linux_ww_intr_srvc

	if ( (rc = request_irq(pintr->dip->irq, (void *)pintr->Isr,  SA_SHIRQ, "mercdintr", (pmerc_void_t)pintr->IsrArg ))) {
		printk("mercd: IntrReg: request_irq() failed with return code %d\n", rc);
		pintr->ret = MD_FAILURE;
		return;
	}

	MSD_INIT_MUTEX(&pintr->phw_info->intr_info->intr_mutex, "hwintr mutex", NULL);

	// return the ISR Handle to caller
	*pintr->IsrHandle = pintr->ConfigId;

	pintr->ret = MD_SUCCESS;

	return;
}


/***************************************************************************
 * Function Name                : linux_intr_deregister_intr_srvc
 * Function Type                : OSSDL and OSAL Interface function
 * Inputs                       : rcvPtr
 * Outputs                      : MD_STATUS
 * Calling functions            : MERCD_OSAL_INTR_DEREGISTER Interfaces
 * Description                  : Streams Driver intr_deregister_intr_srvc Routine.
 *                                This routine is used to deregister the
 *                                interrupt with Solaris.
 * Additional comments          :
 ****************************************************************************/
void linux_intr_deregister_intr_srvc(void *rcvPtr)
{
  pmercd_osal_intr_deregister_sT pintr = (pmercd_osal_intr_deregister_sT)rcvPtr;

  MSD_FUNCTION_TRACE("linux_intr_deregister_intr_srvc", ONE_PARAMETER, (ULONG) rcvPtr);

  if (pintr->ConfigId >= MSD_MAX_BOARD_ID_COUNT) {
      printk("MsdDeregIntr: Illegal config id (%d) >= %d\n",
             			pintr->ConfigId, MSD_MAX_BOARD_ID_COUNT);
      pintr->ret = MD_FAILURE;
      return;
  }

  MSD_LEVEL2_DBGPRINT("MsdDeregIntr:free_irq @ IRQ %d,CfgId %d\n",
	                        pintr->dip->irq, pintr->ConfigId);

  // Must deregister interrutp from kernel
  free_irq(pintr->dip->irq, (void *)pintr->IsrArg);

  pintr->ret = MD_SUCCESS;
  return;
}


/***************************************************************************
 * Function Name                : linux_intr_srvc/mercdintr
 * Function Type                : Interrupt functions
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver mercd_intr_srvc Routine.
 *                                This is the interrupt service routine
 *                                registered and called on interrupt by Solaris
 * Additional comments          : DMA changes ..... 
 ****************************************************************************/
irqreturn_t linux_intr_srvc(int InterruptVector, void *dev_id, struct pt_regs *regs)
{
  merc_uchar_t    IntrReason;
  merc_uchar_t    HostRamReq;
  merc_uchar_t    need_int = 0;
  merc_ulong_t    PlxIntrReg; 
  pmercd_adapter_block_sT   padapter;
  merc_int_t i;


  padapter = (pmercd_adapter_block_sT) dev_id;

  MSD_ASSERT( padapter );

  padapter->phw_info->ret = MD_SUCCESS;

  if((padapter->state == MERCD_ADAPTER_STATE_MAPPED) ||
     (padapter->state == MERCD_ADAPTER_STATE_READY) ||
     (padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) ||
     (padapter->state == MERCD_ADAPTER_STATE_INIT)) {
      return IRQ_NONE;
  }

  (*mercd_dhal_func[MERCD_DHAL_INTR_CHECK])( (void *)padapter) ;

  if ( padapter->phw_info->ret != MD_SUCCESS ) {
      return IRQ_NONE;
  }

  MSD_LEVEL2_DBGPRINT("mercd: interrupt check ... ok\n");

  if (!MSD_SPIN_TRYLOCK(&padapter->phw_info->intr_info->intr_mutex)) {
	return IRQ_NONE;
  }

  // get the interrupt reason
  IntrReason = MsdRegReadUchar(padapter->phost_info->reg_block.ClrMsgReadyIntr);

  MERC_CLEAR_INTERRUPT((&padapter->phost_info->reg_block), IntrReason);

  if (IntrReason & MERC_ERROR_INTR_R) {
      if (!(padapter->flags.SramOurs & MERC_BOARD_FLAG_SRAM_IS_OURS)) {
          // Get the Semaphore
          MERC_GET_SRAM_LOCK((&padapter->phost_info->reg_block), HostRamReq);

    	  if (!(HostRamReq & MERC_HOST_RAM_GRANT_FOR_CLR_R)){
              padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
              printk("linux_intr_srvc: Adapter Out Of service\n");
              MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
              return IRQ_NONE;
          }
      }
      
      if (padapter->panicDump == NULL)
          mercd_zalloc( padapter->panicDump, pmerc_uchar_t, MD_PANIC_DUMP_MAX_SIZE);
 
      if (!padapter->panicDump) {
          printk("linux_intr_srvc: unable to allocate memory\n");
          MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
          return IRQ_NONE;
      }

 
      (void)supp_read_brd_panic_buf(padapter, (pmerc_uchar_t)padapter->panicDump);

      printk("mercd: Got Error Interrupt from board %d. Sram Crash dump...\n", padapter->adapternumber);
      for (i=0; i<MD_PANIC_DUMP_MAX_SIZE; i++) {
	   printk("%c ", padapter->panicDump[i]);
	   if (!(i%8))
		printk("\n");
      }
      printk("\n");

      // This is a Board DEATH intr.
      // Just set the adapter state to OOS for now
      // expect user to send shutboard to clean up
      padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
      printk("linux_intr_srvc: Adapter Out Of service(1)\n");
      MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);

      return IRQ_NONE;
  }

  PlxIntrReg = MsdPlxGetIntrReg(mercd_adapter_map[padapter->adapternumber]);
	 
  ////////////////////////SRAMDMA BEGIN///////////////////////////
  // check for correct intr types
  if (!(IntrReason&(MERC_HOST_RAM_PEND_INTR_R|MERC_MSG_READY_INTR_R))) {
      if (PlxIntrReg & MSD_PCI_PLX_INT_CSR_DMA1_ACTIVE_R) {
          padapter->flags.DMAIntrReceived = (MERC_ADAPTER_FLAG_RCV_DMA_INTR | 
                                                 MERC_ADAPTER_FLAG_DMA_INTR);

          MsdPlxDmaClrInt_Chn1(mercd_adapter_map[padapter->adapternumber]);

	  // *** Free the SRAM LOCK  ***********************************
	  // We do not need this anymore, board can continue processing
	  //************************************************************

	  if (padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR) {
              //sending intr automatically releases sram lock
              padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;

              MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
              MERC_INTR_ADAPTER((&padapter->phost_info->reg_block));
              // indicate we no longer have semaphore in adapter block
              padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
          } else {
              // must release sram lock so fw can access it
              MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));
              padapter->flags.SramOurs  &= ~MERC_ADAPTER_FLAG_SRAM_IS_OURS;
          }
      } else  // Channel 0 is for Transmit interrupts. To be supported in the future 
          if (PlxIntrReg & MSD_PCI_PLX_INT_CSR_DMA0_ACTIVE_R) {
              padapter->flags.DMAIntrReceived =  (MERC_ADAPTER_FLAG_SND_DMA_INTR |
                                                         MERC_ADAPTER_FLAG_DMA_INTR);
              MsdPlxDmaClrInt_Chn0(mercd_adapter_map[padapter->adapternumber]);
      } else {
	      //This is not possible.. But just exit ..... 
              MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
              return IRQ_NONE;
      }
  } else {
      if (PlxIntrReg & MSD_PCI_PLX_INT_CSR_DMA1_ACTIVE_R) {
          MsdPlxDmaClrInt_Chn1(mercd_adapter_map[padapter->adapternumber]);

	  padapter->flags.DMAIntrReceived = (MERC_ADAPTER_FLAG_RCV_DMA_INTR |
                                                 MERC_ADAPTER_FLAG_DMA_INTR);

	  // Release the SRAM lock here .... , No more SRAM access needed.
	  if (padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR) {
              //sending intr automatically releases sram lock
              padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;

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
  }
  
  // Check for non-DMA Complete Interrupt during DMA - do not service it
  if (padapter->phw_info->pdma_info->rx_dma_count > 0) {
      if (padapter->flags.DPCDmaPending == 0) {
          MSD_ERR_DBGPRINT("mercd: Bad Case (rx_dma_count=%d and DPCDmaPending=%d)\n", padapter->phw_info->pdma_info->rx_dma_count, padapter->flags.DPCDmaPending);
          MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
          return IRQ_NONE;
      } else {
          if (padapter->flags.DMAIntrReceived == 0) {
              MSD_ERR_DBGPRINT("mercd: Received a non-DMA Complete Interrupt after DMA Started\n");
              MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
              return IRQ_NONE;
          }
      }
  } else {
      if (padapter->flags.DPCDmaPending > 0) {
          MSD_ERR_DBGPRINT("mercd: Bad Case (rx_dma_count=%d and DPCDmaPending=%d)\n", padapter->phw_info->pdma_info->rx_dma_count, padapter->flags.DPCDmaPending);
          MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);
          return IRQ_NONE;
      }
  }

  need_int = 0;

  // DPC is protected by Mutex ...
  if (padapter->phw_info->un_flag != UNIT_BUSY) {

      need_int = 1;

      // LA: Task# 14456 ...
      // before this was set regardless with or without an interrupt need_int ( 1 or 0 )
      padapter->flags.RecvPending |= MERC_ADAPTER_FLAG_RECEIVE_PEND;

      MSD_LEVEL2_DBGPRINT("dpc data addr = %x, slotn = %d, content = %d\n",
          padapter->phw_info->intr_info->dpc_task.data, padapter->phw_info->slot_number,
          *(merc_uchar_t *) padapter->phw_info->intr_info->dpc_task.data );


  }

  MSD_EXIT_MUTEX(&padapter->phw_info->intr_info->intr_mutex);

  if (need_int) {

      //Scheduling can be done in different ways as shown below
      if (DPC_Disabled) {
#ifdef LFS
          mercd_generic_intr_processing( (ulong)&padapter->phw_info->slot_number);
#else
          mercd_generic_intr_processing( (void *) &padapter->phw_info->slot_number);
#endif
      } else {
          tasklet_hi_schedule(&(padapter->phw_info->intr_info->dpc_task));
      }
  }

  return IRQ_HANDLED;
}
