/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File name                    : mercd_osshl_linux_pci.c
 * Description                  : OS specific Hardware Layer
 *
 *
 **********************************************************************/

#include <linux/version.h>
#ifdef LFS
#include <sys/kmem.h>
#endif

#include "msd.h"
#define MERCD_OSSHL_LINUX_PCI_C
#include "msdextern.h"
#undef MERCD_OSSHL_LINUX_PCI_C

#include "msdpciif.h"
#include "msdversion.ver"

#define    streammem  0
#define    dmakmem    1


int Mercd_Adapter_Count = 0;
extern int NonDefaultId;
extern int RH_Enabled;

// CPCI PCI Table Initialization
volatile MSD_PROBE_BOARD_CONFIG msd_probe_brd_cfg[] =
{
  { BRD_PCI,      0x10B5, 0x906D, 0x4001, 0x12C7, 0x0000A0E6, 0x00000001, 0x40010000, 4, 3, 2, 3 },
  { BRD_PCIe,     0x12C7, 0x5356, 0x5007, 0x12C7, 0x0000A0E6, 0x00000001, 0x40010000, 4, 3, 2, 3 },
  { BRD_PCIe,     0x12C7, 0x5356, 0x4002, 0x12C7, 0x0000A0E6, 0x00000001, 0x40010000, 4, 3, 2, 3 },
  { BRD_CPCI_1_9, 0x10B5, 0x9054, 0x4001, 0x12C7, 0x0000A0E6, 0x00000001, 0x40110000 , 5, 3, 2, 4 },
  { BRD_CPCI_1_9, 0x10B5, 0x9054, 0x4001, 0x12C7, 0x0000A0E6, 0x00000001, 0x40020000 , 5, 3, 2, 4 },
  { BRD_CPCI_DTI16_21554,   0x1011, 0x0046, 0x5001, 0x12C7, 0xa0e6, 0x1, 0x40110000, 0, 0, 0, 0}, /* 21554 */
  { BRD_CPCI_ROZETTA_21554, 0x1011, 0x0046, 0x5002, 0x12C7, 0xa0e6, 0x1, 0x40010000, 0, 0, 0, 0}, /* 21554*/
  { BRD_CPCI_DTI16_21555,   0x8086, 0xB555, 0x5001, 0x12C7, 0xa0e6, 0x1, 0x40110000, 0, 0, 0, 0}, /* 21555 */
};


static merc_uint_t
addresses[] =
{
        PCI_BASE_ADDRESS_0,
        PCI_BASE_ADDRESS_1,
        PCI_BASE_ADDRESS_2,
        PCI_BASE_ADDRESS_3
};
//THIRD_ROCK Support
struct deviceTypeArray DM3DeviceTypes [] =
  {
      { 0x4001, SRAM_FAMILY },       // all SRAM boards.
      { 0x4002, SRAM_FAMILY },       // all DISI boards.
      { 0x5005, SRAM_FAMILY },       // DMV-B boards
      { 0x5001, THIRD_ROCK_FAMILY }, // DTI8/16 hardware with a YAVA PAI / 21554
      { 0x5002, THIRD_ROCK_FAMILY }, // Rozetta Blade
      { 0x5007, SRAM_FAMILY },       // New Brunswick PCI Express board.
      { 0, INVALID_BOARD_FAMILY }    // Not our hardware.
  };

/////////////////SRAMDMA BEGIN////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
merc_uchar_t total_merc_tx_dma_chains = TOTAL_MERC_TX_DMA_CHAINS;
merc_uchar_t total_merc_rx_dma_chains = TOTAL_MERC_RX_DMA_CHAINS;

merc_uchar_t max_merc_tx_dma_chains_on_board = MAX_MERC_TX_DMA_CHAINS_ON_BOARD;
merc_uchar_t max_merc_rx_dma_chains_on_board = MAX_MERC_RX_DMA_CHAINS_ON_BOARD;
#endif
/////////////////SRAMDMA END////////////////


/***************************************************************************
 * Function Name                : linux_pci_drvr_identify
 * Function Type                : DHAL & OSSHL Function
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_DRVR_IDENTIFY Interfaces
 * Description                  : Streams Driver Start Routine.
 * Additional comments          :
 ****************************************************************************/
void linux_pci_drvr_identify(void *rcvPtr)
{
        pmercd_dhal_drvr_identify_sT pdrvr = (pmercd_dhal_drvr_identify_sT) rcvPtr;

        MSD_FUNCTION_TRACE("linux_pci__drvr_identify", ONE_PARAMETER, (size_t) rcvPtr);

        /* Get the Force board model number
         * Must make sure FORCE doesn't change the string
        */

        supp_ver2string(VersionString, VER_WITHBUILD, MsdDriverVersion);

        printk("\n*********************************\n");
	printk("Native DM3 Linux Device Driver\n");
	printk("Version: %s", MsdDriverVersion);

        MsdStringCopy(VersionString, MsdDriverVersion);

        printk("\nCopyright (C) 2006 Dialogic Corp.\n");
	printk("ALL RIGHTS RESERVED\n");
	printk("Mercury Driver running on x86\n");
	printk("*********************************\n");

        pdrvr->ret = MD_SUCCESS;
        return;
}

/***************************************************************************
 * Function Name                : linux_pci_device_copy
 * Function Type                : DHAL & OSSHL function
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_MEM_COPY Interfaces
 * Description                  : Streams Driver pci_device_copy Routine. This
 *                                routine copies data to adapter.
 * Additional comments          :
 ****************************************************************************/
void linux_pci_device_copy(void *rcvPtr)
{
        pmercd_dhal_mem_copy_sT pmem = (pmercd_dhal_mem_copy_sT) rcvPtr;

	memcpy(pmem->dest, pmem->src, pmem->size);

        return;
}

/***************************************************************************
 * Function Name                : linux_pci_get_prop
 * Function Type                : OSSHL & DHAL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_BRD_GETPROP
 * Description                  :  PCI Board Specific Routine. This routine
 *                                 reads the properties of a particular board
* Additional comments          :
 ****************************************************************************/
void linux_pci_get_prop(void *rcvPtr)
{

        pmercd_dhal_brd_getprop_sT pbrd = (pmercd_dhal_brd_getprop_sT) rcvPtr;

        merc_uint_t                       tmp_address=0;
        merc_uint_t                       tmp_address1=0;
        merc_uchar_t                      intr;
        merc_uint_t                       barIndex;

        if (!pbrd->dip) {
            printk("get_prop: Invalid device info (dip) ...(%p)\n", pbrd->dip);
            pbrd->ret = MD_FAILURE;
            return;
        }

        if (pci_read_config_byte(pbrd->dip, PCI_INTERRUPT_LINE, &intr) != PCIBIOS_SUCCESSFUL) {
            printk("get_prop: PCI config interrupt line ... failed\n");
            pbrd->ret = MD_FAILURE;
            return;
        }

        if (pbrd->dip->irq == 0) {
            printk("get_prop: PCI interrupt line %d\n", pbrd->dip->irq);
            pbrd->ret = MD_FAILURE;
            return;
        }

        mercd_zalloc( pbrd->phw_info->intr_info, pmercd_intr_info_sT, MERCD_INTR_INFO);

        pbrd->phw_info->intr_info->intr_number = 0;

        pbrd->phw_info->intr_info->intr_vec = pbrd->dip->irq;
        pbrd->phw_info->slot_number = pbrd->phw_info->pci_slot_number = PCI_SLOT( pbrd->dip->devfn ) ;
        pbrd->phw_info->bus_number = pbrd->dip->bus->number;
        pbrd->phw_info->bus_type =  BUS_TYPE_PCI;


        // Allocate Virtual Map structs ...
#ifdef LFS
        mercd_zalloc( pbrd->phw_info->virt_map_q, pmercd_virt_map_sT*, PCI_VIRT_MAP_O_SIZE);
#else
        mercd_zalloc( pbrd->phw_info->virt_map_q, pmercd_virt_map_sT, PCI_VIRT_MAP_O_SIZE);
#endif
        mercd_zalloc( pbrd->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP], pmercd_virt_map_sT, MERCD_VIRT_MAP);
        mercd_zalloc( pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP], pmercd_virt_map_sT, MERCD_VIRT_MAP);

        // Allocate Virtual WW Map structs
#ifdef LFS
        mercd_zalloc( pbrd->phw_info->virt_ww_map_q, pmercd_virt_map_sT*, PCI_VIRT_MAP_O_SIZE);
#else
        mercd_zalloc( pbrd->phw_info->virt_ww_map_q, pmercd_virt_map_sT, PCI_VIRT_MAP_O_SIZE);
#endif
        mercd_zalloc( pbrd->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP], pmercd_virt_map_sT, MERCD_VIRT_MAP);
        mercd_zalloc( pbrd->phw_info->virt_ww_map_q[MERCD_PCI_SRAM_MAP], pmercd_virt_map_sT, MERCD_VIRT_MAP);

        // Assign Virtual Map addrresses ...
        pbrd->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr = pci_resource_start(pbrd->dip,0);

        // THIRD_ROCK Support
        if (pbrd->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr & 0x0000000f) {
            pbrd->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr &= ~0x0000000f;
        }

        // To find out about the Size of the memory
        pci_read_config_dword(pbrd->dip, addresses[0], (u32 *)&tmp_address);
        pci_write_config_dword(pbrd->dip, addresses[0], ~0 );
        pci_read_config_dword(pbrd->dip, addresses[0], (u32 *)&tmp_address1);

        if (!tmp_address1) {
            pbrd->ret = MD_FAILURE;
            printk("get_prop: Unable to read address[0] (tmp_address1 = %x) ...", tmp_address1);
            return;
        }
        pbrd->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->byte_count = (size_t)((0xffffffff - tmp_address1)+1);

        // Rozetta Support
        if (pbrd->phw_info->pciSubSysId == SUBSYSID_ROZETTA_21554) {
            barIndex = 3;
        } else {
	    // DTI16
            barIndex = 2;
	}

        pci_write_config_dword(pbrd->dip, addresses[0], pci_resource_start(pbrd->dip,0));

        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr = pci_resource_start(pbrd->dip,barIndex);

        // THIRD_ROCK Support
        if (pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr & 0x0000000f) {
           pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr &= ~0x0000000f;
        }

        // To find out about the Size of the memory
        pci_read_config_dword(pbrd->dip, addresses[barIndex], (u32 *)&tmp_address);
        pci_write_config_dword(pbrd->dip, addresses[barIndex], 0xffffffff);
        pci_read_config_dword(pbrd->dip, addresses[barIndex], (u32 *)&tmp_address1);

        if (!tmp_address1) {
            pbrd->ret = MD_FAILURE;
            printk("get_prop: Unable to read address[barIndex] (tmp_address1 = %x)\n", tmp_address1);
            return;
        }

        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count = (size_t)((0xffffffff-tmp_address1)+1);

        pci_write_config_dword(pbrd->dip, addresses[barIndex], pci_resource_start(pbrd->dip, barIndex));

        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->end_offset = pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count - 1;

        // DMVB - setup for Bar3
        if ((pbrd->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
            (pbrd->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
            pbrd->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr = pci_resource_start(pbrd->dip,3);
            if (pbrd->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr & 0x0000000f) {
                pbrd->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr &= ~0x0000000f;
            }
            pbrd->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->byte_count = MSD_PCI_BAR3_SIZE;
        }

        return;
}


/***************************************************************************
 * Function Name                : linux_pci_intr_enable
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_INTR_ENABLE
 * Description                  :  PCI Board Specific Routine. This routine
 *                                 enables interrupts from a board specified
 *                                 by the ConfigId
 * Additional comments          :
 ****************************************************************************/
void linux_pci_intr_enable(void *rcvPtr)
{
        pmercd_hw_info_sT phw_info; /*DTI16*/
        pmercd_dhal_intr_enable_sT pintr = (pmercd_dhal_intr_enable_sT) rcvPtr;
        size_t IntrReg;
        merc_int_t  slotn = mercd_adapter_map[pintr->ConfigId] ;
        phw_info = pintr->phw_info;

        MSD_LEVEL2_DBGPRINT(CE_NOTE, "linux_pci_intr_enable(): slot (%d) cfgid (%d)\n",
                slotn,
                pintr->ConfigId);

        if (phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
            IntrReg = MsdPlxGetIntrReg(slotn);
	    if (phw_info->pacific_chip) {
		IntrReg |= MSD_PCI_PACIFIC_INTR_ENABLE;
	    } else {
		IntrReg |= MSD_PCI_PLX_INTR_ENABLE;
	    }
	    IntrReg |= MSD_PCI_PLX_INTR_ENABLE;
            MsdPlxPutIntrReg(slotn, IntrReg);
          } else {
            //THIRD_ROCK Family
            //IntrReg = Msd21554GetIntrReg(slotn);
          }

        return;
}

/***************************************************************************
 * Function Name                : linux_pci_intr_disable
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_INTR_DISABLE Interfaces
 * Description                  : PCI Board Specific Routine. This routine
 *                                disables interrupts from a board specified
 *                                by the ConfigId
 * Additional comments          :
 ****************************************************************************/
void linux_pci_intr_disable(void *rcvPtr)
{
        pmercd_dhal_intr_disable_sT pintr = (pmercd_dhal_intr_disable_sT) rcvPtr;
        size_t IntrReg;
        merc_int_t  slotn = mercd_adapter_map[pintr->ConfigId] ;

        MSD_FUNCTION_TRACE("linux_pci_intr_disable", ONE_PARAMETER, (size_t) rcvPtr);
        IntrReg = MsdPlxGetIntrReg(slotn);
	if (MsdControlBlock->padapter_block_list[slotn]->phw_info->pacific_chip) {
            IntrReg &= ~MSD_PCI_PACIFIC_INTR_ENABLE;
	} else {
	    IntrReg &= ~MSD_PCI_PLX_INTR_ENABLE;
	}
	IntrReg &= ~MSD_PCI_PLX_INTR_ENABLE;
        MsdPlxPutIntrReg(slotn, IntrReg);
        return;
}
/***************************************************************************
 * Function Name                : linux_pci_map_adapter_parm
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_BRD_MAP_ATTR
 * Description                  : Streams Driver MapAdapterParam Routine.
 *                                This routine maps kernel virtual address
 *                                and registers interrupt  function
 * Additional comments          :
 ****************************************************************************/
void linux_pci_map_adapter_parm(void *rcvPtr)
{

        pmercd_dhal_brd_map_attr_sT pmap = (pmercd_dhal_brd_map_attr_sT) rcvPtr;
        mercd_osal_mem_phys_virt_sT meminfo = { 0 } ;

        MSD_FUNCTION_TRACE("linux_pci_map_adapter_parm",ONE_PARAMETER,
                                                        (size_t)rcvPtr);

        meminfo.PhysAddr = pmap->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr;
        meminfo.ByteCount = pmap->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->byte_count;

        // Map the PLX address to kernel virtual
        (*mercd_osal_func[MERCD_OSAL_MEM_PHYS_VIRT])((void *) &meminfo);

        if(meminfo.ret == NULL) {
                printk("linux_pci_mem_map: MsdPhysToVirt failed");
                pmap->ret = MD_FAILURE;
                return;
        }

        pmap->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr = meminfo.ret;

        meminfo.PhysAddr = pmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr;
        meminfo.ByteCount = pmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count;

        // Map the shared ram address to kernel virtual
        (*mercd_osal_func[MERCD_OSAL_MEM_PHYS_VIRT])((void *) &meminfo);

        if(meminfo.ret == NULL) {
                MSD_ERR_DBGPRINT("linux_pci_mem_map: MsdPhysToVirt failed\n");
                pmap->ret = MD_FAILURE;
                return;
        }

        pmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr = meminfo.ret;

        MSD_LEVEL2_DBGPRINT("mercd: SRAM virt addr : %x\n", meminfo.ret);

	// DMVB - map Bar3
	if ((pmap->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
            (pmap->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
	   meminfo.PhysAddr = pmap->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->phys_addr;
	   meminfo.ByteCount = pmap->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->byte_count;
	   
	   // Map the PLX address to kernel virtual
           (*mercd_osal_func[MERCD_OSAL_MEM_PHYS_VIRT])((void *) &meminfo); 

	   if (meminfo.ret == NULL) {
	       printk("linux_pci_mem_map: MsdPhysToVirt failed for Bar3\n");
	       pmap->ret = MD_FAILURE;
	       return;
	   }

	   pmap->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr = meminfo.ret;

	   MSD_LEVEL2_DBGPRINT("mercd: WW virt addr : %x\n", meminfo.ret);
	}

        return;
}

/***************************************************************************
 * Function Name                : linux_pci_unmap_adapter_parm
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_BRD_UNMAP_ATTR Interfaces
 * Description                  : Streams Driver UnMapAdapterParam Routine.
 *                                This routine Unmaps kernel virtual address
 *                                 and de-registers interrupt function
 * Additional comments          :
 ****************************************************************************/

void linux_pci_unmap_adapter_parm(void *rcvPtr)
{
pmercd_dhal_brd_unmap_attr_sT punmap = (pmercd_dhal_brd_unmap_attr_sT) rcvPtr;
mercd_osal_intr_deregister_sT intr_deregisterinfo = { 0 };
mercd_dhal_intr_disable_sT    intr_disableinfo = { 0 };
mercd_osal_mem_virt_free_sT   meminfo = { 0 };
pmercd_adapter_block_sT       padapter;
merc_int_t     ConfigId = punmap->ConfigId;

////////////SRAMDMA BEGIN///////////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
#ifndef LFS
mercd_dhal_dma_free_sT   dma_free = { 0 };
#endif
#endif
////////////SRAMDMA END///////////////////////////////


        MSD_FUNCTION_TRACE("linux_pci_mem_unmap", ONE_PARAMETER, (size_t)rcvPtr);

        intr_disableinfo.ConfigId = ConfigId;

        padapter = MsdControlBlock->padapter_block_list[ mercd_adapter_map[ ConfigId ] ];

        if( !(padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR) ) {

            meminfo.VirtAddr = (size_t)&punmap->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;

            // dealloc the virtual address
            (*mercd_osal_func[MERCD_OSAL_MEM_VIRT_FREE])((void *) &meminfo);

            mercd_free(punmap->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP], 
                        MERCD_VIRT_MAP, MERCD_FORCE);

             // meminfo.DevAccHandlePtr = 
             //       &punmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;

             meminfo.VirtAddr = (size_t)&punmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;

             // dealloc the virtual address
             (*mercd_osal_func[MERCD_OSAL_MEM_VIRT_FREE])((void *) &meminfo);

             mercd_free(punmap->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP], 
                           MERCD_VIRT_MAP, MERCD_FORCE);

             mercd_free(punmap->phw_info->virt_map_q, PCI_VIRT_MAP_O_SIZE, MERCD_FORCE);

	     // deallocate Bar3 for DMVB
	     if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
                 (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
		 meminfo.VirtAddr = (size_t)&punmap->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;
		 (*mercd_osal_func[MERCD_OSAL_MEM_VIRT_FREE])((void *) &meminfo);
		 mercd_free(punmap->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP],MERCD_VIRT_MAP, MERCD_FORCE);
	         mercd_free(punmap->phw_info->virt_ww_map_q[MERCD_PCI_SRAM_MAP],MERCD_VIRT_MAP, MERCD_FORCE);
	         mercd_free(punmap->phw_info->virt_ww_map_q, PCI_VIRT_MAP_O_SIZE, MERCD_FORCE);
	     }		 

	     // Kill tasklet
             tasklet_kill(&padapter->phw_info->intr_info->dpc_task);	

             // De-register the interrupt service routine
#ifdef LFS
             intr_deregisterinfo.IsrHandle = (long)&punmap->phw_info->intr_info->isr_handle;
#else
             intr_deregisterinfo.IsrHandle = &punmap->phw_info->intr_info->isr_handle;
#endif
             intr_deregisterinfo.ConfigId = ConfigId;
             intr_deregisterinfo.phw_info = punmap->phw_info;
             intr_deregisterinfo.dip = punmap->dip;
             intr_deregisterinfo.IsrArg = (size_t)padapter;

             (*mercd_osal_func[MERCD_OSAL_INTR_DEREGISTER])((void *) &intr_deregisterinfo);

             if (intr_deregisterinfo.ret != MD_SUCCESS) {
                 printk("mercd: Intr Unreg: Failed");
                 punmap->ret = MD_FAILURE;
                 return;
             }

             padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;
        }
        return;
}

/***************************************************************************
 * Function Name                : linux_pci_verify_merc_adapter
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver IsMercBoard Routine. This
 *                                routine checks whether the board is a
 *                                Mercury board.
 * Additional comments          :
 ****************************************************************************/
void linux_pci_verify_merc_adapter(void *rcvPtr)
{
        pmercd_dhal_drvr_verify_sT pbrdinfo = (pmercd_dhal_drvr_verify_sT) rcvPtr;
        size_t   VendorId;
        size_t   PrimaryBoardId;
        size_t   SecondaryBoardId;
        ssize_t   i;

        MSD_FUNCTION_TRACE("verify_merc_adapter", ONE_PARAMETER, (size_t) rcvPtr);

        // make sure there's a board in this slot.

        VendorId = (size_t)MERC_READ_CONFIG_ROM_TRI_BYTES(pbrdinfo->ConfigStart
                                                     +MERC_CR_OFFSET_VENDOR_ID);


        PrimaryBoardId = MERC_READ_CONFIG_ROM_ULONG(pbrdinfo->ConfigStart
                                                + MERC_CR_OFFSET_BOARD_PRI_ID);

        // Mask out the lower bytes as we don't want to check DM3 model number,
        // just want to ensure that the card is DM3.
        PrimaryBoardId &= MD_VMSCP_ID_MASK;

        SecondaryBoardId = MERC_READ_CONFIG_ROM_ULONG(pbrdinfo->ConfigStart
                                                  +MERC_CR_OFFSET_BOARD_SEC_ID);

        // Also check the Protocol version specified in the SecondaryBoardId area
        // MD_SCP_ID changes with for VME and PCI platforms
        // Integrated PCI/cPCI driver changes

        //printk("verify merc: vendorid: 0x%x primaryboardid: 0x%x secondaryboardid: 0x%x\n", 
        //                                 VendorId,
        //                                 PrimaryBoardId,
        //                                 SecondaryBoardId);

        pbrdinfo->ret = MD_FAILURE;
        for (i = 0; i < MAX_PROBE_BRD_CFG; ++i){

                if( VendorId == VENDOR_ID(i) &&
                    SecondaryBoardId == SEC_VENDOR_ID(i) &&
                    PrimaryBoardId == PRIMARY_BOARD_ID(i) ){
                        pbrdinfo->ret = MD_SUCCESS;
                }
        }

        return;
}

/***************************************************************************
 * Function Name                : linux_pci_save_attr
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_save_attr(void *rcvPtr)
{

        pmercd_dhal_brd_save_attr_sT pbrd = (pmercd_dhal_brd_save_attr_sT) rcvPtr;
        size_t   SramStart;
        size_t   SramTop;
        size_t   SramSize;
#ifndef LFS
        static int i = 0;
#endif
        //size_t   SlotNumber;

        merc_uchar_t LocatorId;
        //SlotNumber = (merc_ulong_t)pbrd->slotNumber;

        MSD_FUNCTION_TRACE("linux_pci_save_attr", ONE_PARAMETER, (size_t)rcvPtr);

        // read in all the necessary parameters
        SramStart = MERC_READ_CONFIG_ROM_TRI_BYTES(pbrd->ConfigStart +MERC_CR_OFFSET_SRAM_START);
        SramTop = MERC_READ_CONFIG_ROM_TRI_BYTES(pbrd->ConfigStart +MERC_CR_OFFSET_SRAM_VIRT_TOP);
        SramSize = MERC_READ_CONFIG_ROM_TRI_BYTES(pbrd->ConfigStart +MERC_CR_OFFSET_SRAM_SIZE);

        MSD_LEVEL2_DBGPRINT("linux_pci_save_attr: Slot=%d, SramStart=%x, SramTop=%x, SramSize=%x",
						  pbrd->SlotNumber, SramStart, SramTop, SramSize);

	if (!RH_Enabled) {
            // FOR THIRD_ROCK boards: LocatorId read at PCI_CR_OFFSET_LOCATOR_ID is 0x80. 
	    // When the slot number is inerpreted out of this we always end up getting a zero.
            // This was not the case for DM3 hardware, where the first nibble always a non-zero. 
	    // So interpreting the slot number the following way worked
            // To get around this problem, we used 'device' member of the pci_dev_t which is a 
	    // device id or slot id. We store that slot number directly into phw_info->slot_number.
            // This would certainly take care of our problem of deriving a slot number.
            LocatorId = MsdRegReadUchar(pbrd->ConfigStart+PCI_CR_OFFSET_LOCATOR_ID);
            if (LocatorId != 0xFF) {
                LocatorId &= 0x1F;
                pbrd->phw_info->slot_number = (LocatorId & 0xFF);
            }
        }

        if (pbrd->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) 
            pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count = 0;

#ifndef PPS
        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->start_offset = SramStart;
        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->end_offset = SramTop;
        pbrd->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->byte_count = SramSize;
        MsdRegReadUcharBuffer((pmerc_char_t)pbrd->ConfigStart,
                              (pmerc_char_t)pbrd->phost_info->merc_config_rom,
                              MSD_CONFIG_ROM_MAX_SIZE);
#endif

        return;
}


/***************************************************************************
 * Function Name                : linux_pci_intr_check
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :

 ****************************************************************************/
void linux_pci_intr_check(void *rcvPtr)
{
        pmercd_adapter_block_sT padapter = (pmercd_adapter_block_sT) rcvPtr;

        size_t PlxIntrReg;

        MSD_FUNCTION_TRACE("linux_pci_intr_check", ONE_PARAMETER, (size_t) rcvPtr);
        PlxIntrReg = MsdPlxGetIntrReg(padapter->phw_info->slot_number);

        if ((padapter->state == MERCD_ADAPTER_STATE_MAPPED)    ||
            (padapter->state == MERCD_ADAPTER_STATE_INIT)      || 
	    (padapter->state == MERCD_ADAPTER_STATE_SUSPENDED)) {
             MSD_LEVEL2_DBGPRINT("Hotswap: Board non-existent. \n");
             padapter->phw_info->ret = MD_FAILURE; // shared int case
             return;
        }

///////////////SRAMDMA BEGIN//////////////////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
        if (!(PlxIntrReg & ( MSD_PCI_PLX_INTR_LOCAL | 
                             MSD_PCI_PLX_INT_CSR_DMA0_ACTIVE_R |
                             MSD_PCI_PLX_INT_CSR_DMA1_ACTIVE_R ))) 
#else
        if (!(PlxIntrReg & ( MSD_PCI_PLX_INTR_LOCAL)))
#endif
///////////////SRAMDMA END//////////////////////////////////////
        {
           MSD_LEVEL2_DBGPRINT(
             "linux_pci_intr_check(): No interrupt at INTR_LOCAL DMA0/1, may be from another board\n");
           padapter->phw_info->ret = MD_FAILURE;
           return;
        }

        return;
}

/////////////////SRAMDMA BEGIN////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
/***************************************************************************
 * Function Name                : linux_pci_dma_alloc_handle
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            : MERCD_DHAL_DMA_ALLOC_HANDLE
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_dma_alloc_handle(void *rcvPtr)
{
   pmercd_dhal_dma_init_sT pdmainfo = (pmercd_dhal_dma_init_sT) rcvPtr;
   pmercd_dma_info_sT      pdma;
   pmercd_adapter_block_sT padapter;
   merc_int_t              i;
#ifndef LFS
   merc_int_t              ConfigId;
   merc_int_t              ret;
#endif
   merc_ulong_t            tmp;
#ifndef LFS
   merc_uint_t             size;
#endif
   merc_uchar_t            rem;
   merc_int_t  slotn = mercd_adapter_map[pdmainfo->ConfigId] ;

   padapter = MsdControlBlock->padapter_block_list[ slotn ];

    if(!padapter)  {
         MSD_ERR_DBGPRINT(
             "ERROR: linux_pci_dma_alloc_handle NULL ptr: 0x%x %s #%d\n",
              padapter, __FILE__, __LINE__
              );
          pdmainfo->ret = MD_FAILURE;
          return;
    }

   pdma = (pmercd_dma_info_sT) mercd_allocator(sizeof(mercd_dma_info_sT));
   padapter->phw_info->pdma_info = pdma;
   pdma->rx_dma_count = 0;
   pdma->tx_dma_count = 0;

   pdma->rx_mbuf_count = 0;

   MSD_INIT_MUTEX(&pdma->snd_dma_mutex, "Snd DMA Mutex", NULL);

#ifdef POST_DMA_FREE
   pdma->dmaRdIndex = 0;
   pdma->dmaWrIndex = 0;
   pdma->dma_blk_count = 0;
#endif   

   pdma->total_merc_tx_dma_chains = total_merc_tx_dma_chains;
   pdma->total_merc_rx_dma_chains = total_merc_rx_dma_chains;

   pdma->max_merc_tx_dma_chains_on_board = max_merc_tx_dma_chains_on_board;
   pdma->max_merc_rx_dma_chains_on_board = max_merc_rx_dma_chains_on_board;

  if (pdma->total_merc_rx_dma_chains)
    {
       mercd_zalloc( pdma->prxdma_res, dma_desc_sT *, 
                                        sizeof(dma_desc_sT) *
                                        pdma->total_merc_rx_dma_chains
                                        );
    }

  if (pdma->total_merc_tx_dma_chains)
    {
      mercd_zalloc(pdma->ptxdma_res, dma_desc_sT *,
                                         sizeof(dma_desc_sT) * 
                                         pdma->total_merc_tx_dma_chains
                                         );
    }

   /////////////////////////////////////////////////////////////////////////
   //Alloc Rx DMA descriptors on Board
   /////////////////////////////////////////////////////////////////////////
   if(pdma->max_merc_rx_dma_chains_on_board) 
     {
#ifdef LFS
        pdma->phys_rx_chain_base =  (merc_uchar_t *) 
                                    (long)(MsdPlxGetPlxLocalBaseAddr(slotn));
#else
        pdma->phys_rx_chain_base =  (merc_uchar_t *) 
                                    (MsdPlxGetPlxLocalBaseAddr(slotn));
#endif
        //printk( "pdma->phys_rx_chain_base is %x \n",  pdma->phys_rx_chain_base);

        tmp  = (MSD_CONFIG_ROM_MAX_SIZE + 
                sizeof(MERCURY_HOST_IF_CONFIG) +
                sizeof(mercd_dma_sgt_chain_node) *
                pdma->max_merc_tx_dma_chains_on_board);

        tmp |= (MsdPlxGetPlxLocalBaseAddr(slotn));
        pdma->phys_rx_chain_base = (merc_uchar_t *) (tmp - 1);

        //printk( "pdma->phys_tx_chain_base is %x \n",  pdma->phys_rx_chain_base);
        
        pdma->virt_rx_chain_base = (merc_uchar_t  *)
                         RX_CHAIN_BASEADDR(padapter->phost_info->reg_block.HostRamStart);

        //printk( "pdma->virt_rx_chain_base is %x \n",  pdma->virt_rx_chain_base);
     } /* max_merc_tx_dma_chains_on_board */

    //Allocate any remaining Rx descriptors on Host
	//Please note that we never tried Descriptors on Host ... and pdma->total_merc_rx_dma_chains= max_merc_rx_dma_chains_on_board  and all defined in msdstruct.h.  So the following brunch is  disabled. 
	
    if((rem = (pdma->total_merc_rx_dma_chains - max_merc_rx_dma_chains_on_board)))
     {

		pdma->virt_rx_chain_base_on_host = abstract_alloc_dma( sizeof(mercd_dma_sgt_chain_node) * rem );
        MSD_ZERO_MEMORY(
                         pdma->virt_rx_chain_base_on_host,
                         sizeof(mercd_dma_sgt_chain_node) * rem 
                       );

        if(!pdma->virt_rx_chain_base) 
             pdma->virt_rx_chain_base = pdma->virt_rx_chain_base_on_host;

        pdma->phys_rx_chain_base_on_host =
             (merc_uchar_t *) virt_to_phys(pdma->virt_rx_chain_base_on_host);  

        if(!pdma->phys_rx_chain_base)
                  pdma->phys_rx_chain_base = pdma->phys_rx_chain_base_on_host;
     } /* rem */


     /////////////////////////////////////////////////////////////////////////
     //Alloc Tx DMA descriptors on Board
     /////////////////////////////////////////////////////////////////////////

	 ////////////////////////////////////////////////////////////////////////
	 // TRANSMIT DMA IS NOT TESTED YET 
	 /////////////////////////////////////////////////////////////////////// 
     if(pdma->max_merc_tx_dma_chains_on_board) 
      {
        tmp  = MSD_CONFIG_ROM_MAX_SIZE + sizeof(MERCURY_HOST_IF_CONFIG);
        tmp |= (MsdPlxGetPlxLocalBaseAddr(slotn));
        pdma->phys_tx_chain_base =  (merc_uchar_t *)(tmp - 1);
#ifdef LFS
        printk("pdma->phys_tx_chain_base is %p \n",  pdma->phys_tx_chain_base);
#else
        printk("pdma->phys_tx_chain_base is %x \n",  pdma->phys_tx_chain_base);
#endif
        pdma->virt_tx_chain_base =  (merc_uchar_t *)
                  TX_CHAIN_BASEADDR(padapter->phost_info->reg_block.HostRamStart);
#ifdef LFS
        printk("pdma->virt_tx_chain_base is %p \n",  pdma->virt_tx_chain_base);
#else
        printk("pdma->virt_tx_chain_base is %x \n",  pdma->virt_tx_chain_base);
#endif

      } /* max_merc_tx_dma_chains_on_board */

     //Allocate any remaining Tx descriptors on Host
     if((rem = pdma->total_merc_tx_dma_chains -  pdma->max_merc_tx_dma_chains_on_board))
      {

		pdma->virt_tx_chain_base_on_host = abstract_alloc_dma( sizeof(mercd_dma_sgt_chain_node) * rem );

        if(!pdma->virt_tx_chain_base)
             pdma->virt_tx_chain_base = pdma->virt_tx_chain_base_on_host;

        pdma->phys_tx_chain_base_on_host =
             (merc_uchar_t *)virt_to_phys(pdma->virt_tx_chain_base_on_host);

        if(!pdma->phys_tx_chain_base)
                  pdma->phys_tx_chain_base = pdma->phys_tx_chain_base_on_host;
      } /* rem */

      ////////////////////////////////////////////////////////////////////
      //Get a ref to each descriptor on Tx ring
      /////////////////////////////////////////////////////////////////
       for (i = 0; i < pdma->total_merc_tx_dma_chains ; i++) 
        {
          if(i< pdma->max_merc_tx_dma_chains_on_board ) 
           {
             pdma->ptxdma_res[i].pdma_chain =  (pmercd_dma_sgt_chain_node) 
                    (pdma->virt_tx_chain_base  + i*sizeof( mercd_dma_sgt_chain_node)); 
             pdma->ptxdma_res[i].location = MERC_DESCR_ON_BOARD;
                    
           }
          else
           {
             pdma->prxdma_res[i].pdma_chain = (pmercd_dma_sgt_chain_node)
                                              (pdma->virt_tx_chain_base_on_host + 
                                              (i-max_merc_tx_dma_chains_on_board) *
                                              sizeof( mercd_dma_sgt_chain_node));
             pdma->ptxdma_res[i].location = MERC_DESCR_ON_HOST;
           }
          
           //Place holder for our mblk for house-keeping
           pdma->ptxdma_res[i].dma_mb = 0;
           pdma->ptxdma_res[i].size = 0;
           pdma->ptxdma_res[i].flags = 0;
        } /* for total_merc_tx_dma_chains */
      
      /////////////////////////////////////////////////////////////////////////////
      //Get a ref to each descriptor on Rx ring
      /////////////////////////////////////////////////////////////////////////////
       for (i = 0; i < pdma->total_merc_rx_dma_chains ; i++) 
        {
           if(i< pdma->max_merc_rx_dma_chains_on_board ) 
            {
              pdma->prxdma_res[i].pdma_chain = (pmercd_dma_sgt_chain_node)
                    (pdma->virt_rx_chain_base  + i*sizeof( mercd_dma_sgt_chain_node));
              pdma->prxdma_res[i].location = MERC_DESCR_ON_BOARD;
            }
           else
            {
              pdma->prxdma_res[i].pdma_chain = (pmercd_dma_sgt_chain_node)
                      (pdma->virt_rx_chain_base_on_host + 
                      (i-max_merc_rx_dma_chains_on_board) *
                      sizeof( mercd_dma_sgt_chain_node));
              pdma->prxdma_res[i].location = MERC_DESCR_ON_HOST;
            }

            //Place holder for our mblk for house-keeping
            pdma->prxdma_res[i].dma_mb = 0;
            pdma->prxdma_res[i].size = 0;
            pdma->prxdma_res[i].flags = 0;
        } /* for total_merc_rx_dma_chains */


	// This place holder to keep pointers to message buffers during the receive dma process
    for (i = 0; i < MAX_MERC_RX_DMA_CHAINS_ON_BOARD  ; i++)
      pdma->rxmbufdma[i] = NULL;

  return;
}

/***************************************************************************
 * Function Name                : linux_pci_dma_free_handle
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            : MERCD_DHAL_DMA_FREE_HANDLE
 * Description                  : Clean all DMA handles .... 
 * Additional comments          :
 ****************************************************************************/
void linux_pci_dma_free_handle(void *rcvPtr)
{
  pmercd_dhal_dma_free_sT pdmainfo = (pmercd_dhal_dma_free_sT) rcvPtr;
  pmercd_dma_info_sT                 pdma;
#ifndef LFS
  merc_int_t                         i;
  merc_uchar_t                       rem;
#endif
  pmercd_adapter_block_sT            padapter;
#ifndef LFS
  merc_int_t                         ConfigId;
#endif
  merc_int_t  slotn = mercd_adapter_map[pdmainfo->ConfigId] ;

  padapter = MsdControlBlock->padapter_block_list[ slotn ];

  if(padapter) {
  pdma = padapter->phw_info->pdma_info;
  
  if(pdma) {
    MSD_DESTROY_MUTEX(&pdma->snd_dma_mutex);

  if(pdma->virt_tx_chain_base_on_host)
  if((pdma->total_merc_tx_dma_chains - pdma->max_merc_tx_dma_chains_on_board) && (pdma->virt_tx_chain_base_on_host))
     abstract_free_mem( pdma->virt_tx_chain_base_on_host );

  if(pdma->virt_rx_chain_base_on_host)
    if((pdma->total_merc_rx_dma_chains - pdma->max_merc_rx_dma_chains_on_board) && (pdma->virt_rx_chain_base_on_host))
     abstract_free_mem( pdma->virt_rx_chain_base_on_host );
 
  if (pdma->total_merc_rx_dma_chains) {
      if(pdma->prxdma_res)
     	mercd_free( pdma->prxdma_res, 
               (sizeof(dma_desc_sT) * pdma->total_merc_rx_dma_chains),
               MERCD_FORCE);
  }
  
  if (pdma->total_merc_tx_dma_chains) {
      if(pdma->ptxdma_res)
      	mercd_free(pdma->ptxdma_res, 
                (sizeof(dma_desc_sT) * pdma->total_merc_tx_dma_chains ),
                 MERCD_FORCE);
  }

  mercd_free(pdma, sizeof(mercd_dma_info_sT), MERCD_FORCE);
  }
 } 
         
 return;
}
#endif
///////////////SRAMDMA END//////////////////////////////////////


/***************************************************************************
 * Function Name                : linux_pci_drvr_probe
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            : MERCD_DHAL_DRVR_PROBE
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_drvr_probe(void *rcvPtr)
{
        pmercd_dhal_drvr_probe_sT   pdrvr = (pmercd_dhal_drvr_probe_sT) rcvPtr;
        mercd_dhal_brd_getprop_sT   brdprob = { 0 };
        mercd_dhal_brd_map_attr_sT  mapinfo = { 0 };
        mercd_dhal_brd_save_attr_sT saveinfo ={ 0 };
        pmercd_adapter_block_sT     padapter;
        merc_ushort_t               SubSysId;
        merc_ushort_t               DeviceId;
        merc_ulong_t                CurrOffset=0;
#ifdef LFS
        merc_uint_t                 CListOffset=0;
        merc_uint_t                 NextCListOffset=0;
#else
        merc_ulong_t                CListOffset=0;
        merc_ulong_t                NextCListOffset=0;
#endif
#ifdef LFS
        merc_int_t                  i, j;
#else
        merc_int_t                  i, j, flag;
#endif
        mercd_hs_t*                 hsd = NULL;
        merc_uint_t                 pacificaddr=0;
        merc_uint_t                 Vpacificaddr=0;
        merc_uchar_t                PacificReg;
        merc_uchar_t 		    LocatorId;
	merc_ulong_t		    PowerScale=0xFFFFFFFF;
	merc_ulong_t		    PowerLimit=0xFFFFFFFF;
#ifdef LFS
	merc_uint_t		    PowerProvided=0xFFFFFFFF;
#else
	merc_ulong_t		    PowerProvided=0xFFFFFFFF;
#endif

        pdrvr->ret = MD_FAILURE;

	if (!(MsdControlBlock->adapter_count) ) {
	    mercd_zalloc(MsdControlBlock->padapter_block_list, 
			 pmercd_adapter_block_sT *, MERCD_ADAPTER_BLOCK_LIST);
            mercd_zalloc(MsdControlBlock->pbind_block_list, 
			 pmercd_bind_block_sT *, MERCD_INITIAL_BIND_BLOCK_LIST);
            mercd_zalloc(MsdControlBlock->pbind_free_block_list, pmercd_bind_block_sT *,
			 sizeof(pmercd_bind_block_sT)*(MSD_MAX_BINDTOBE_FREED_INDEX+1));

            MsdControlBlock->maxbind = MSD_INITIAL_MAX_BIND;

            MsdControlBlock->OpenFreeIndex=0;
            MsdControlBlock->BindFreeIndex=0;

            for (i=0;i<MSD_MAX_BOARD_ID_COUNT;++i) {
                 mercd_adapter_map[i] = 0xFF;
                 mercd_adapter_log_to_phy_map_table[i] = 0xFF;
                 MsdControlBlock->padapter_block_list[i] = NULL;
            }

            for (i=0;i<MsdControlBlock->maxbind;++i) {
                 MsdControlBlock->pbind_block_list[i] = NULL;
            }

	    MsdControlBlock->MsdState  = MERCD_CTRL_BLOCK_INIT;
	}

        mercd_zalloc(padapter, pmercd_adapter_block_sT, MERCD_ADAPTER_BLOCK);
	if (!padapter) {	
	    printk("linux_pci_drvr_probe: Unable to allocate memory for padpater\n");
	    return;
	}
        mercd_zalloc(padapter->phw_info, pmercd_hw_info_sT, MERCD_HW_INFO);
	if (!padapter->phw_info) {
	    mercd_free(padapter, MERCD_ADAPTER_BLOCK, MERCD_FORCE);
	    printk("linux_pci_drvr_probe: Unable to allocate memory for phw_info\n");
            return;
        }
        mercd_zalloc(padapter->phost_info, pmercd_host_info_sT, MERCD_HOST_INFO);
	if (!padapter->phost_info) {
	    mercd_free(padapter->phw_info, MERCD_HW_INFO, MERCD_FORCE);
	    mercd_free(padapter, MERCD_ADAPTER_BLOCK, MERCD_FORCE);
	    printk("linux_pci_drvr_probe: Unable to allocate memory for phost_info\n");
            return;
        }
        mercd_zalloc(padapter->phw_info->timer_info, pmercd_timer_info_sT, MERCD_TIMER_INFO);
	if (!padapter->phw_info->timer_info) {
	    mercd_free(padapter->phost_info, MERCD_HOST_INFO, MERCD_FORCE);
	    mercd_free(padapter->phw_info, MERCD_HW_INFO, MERCD_FORCE);
	    mercd_free(padapter, MERCD_ADAPTER_BLOCK, MERCD_FORCE);
	    printk("linux_pci_drvr_probe: Unable to allocate memory for timer_info\n");
            return;
        }
        mercd_zalloc(padapter->pstream_connection_list, pmerc_void_t *, MERCD_STREAMS_BLOCK_LIST);
	if (!padapter->pstream_connection_list) {
	    mercd_free(padapter->phw_info->timer_info, MERCD_TIMER_INFO, MERCD_FORCE);
	    mercd_free(padapter->phost_info, MERCD_HOST_INFO, MERCD_FORCE);
            mercd_free(padapter->phw_info, MERCD_HW_INFO, MERCD_FORCE);
            mercd_free(padapter, MERCD_ADAPTER_BLOCK, MERCD_FORCE);
            printk("linux_pci_drvr_probe: Unable to allocate memory for pstream_connection_list\n");
            return;
        }


        padapter->state = MERCD_ADAPTER_STATE_MAPPED;

        padapter->pdevi = pdrvr->pdevi;
        hsd = (mercd_hs_t*)pci_get_drvdata(pdrvr->pdevi);
        if (!hsd) {
            printk("mercd_drvr_probe: hsd is Null\n");
            return;
        }
        hsd->padapter = padapter;

        MSD_INIT_MUTEX(&padapter->adapter_block_mutex, "Adapter Block Mutex", NULL);
        MSD_INIT_MUTEX(&padapter->snd_msg_mutex, "Send Msg Mutex", NULL);
        MSD_INIT_MUTEX(&padapter->snd_data_mutex, "Send Data Mutex", NULL);
        MSD_INIT_MUTEX(&padapter->stream_connection_mutex, "Stream Connection Mutex", NULL);
        MSD_INIT_MUTEX(&padapter->phw_info->timer_info->timer_flag_mutex, " Timer Flag Mutex ", NULL);

        for (i = 0; i <= MsdControlBlock->adapter_count; i++) {
            if (mercd_adapter_map[i] < MSD_MAX_BOARD_ID_COUNT &&
                MsdControlBlock->padapter_block_list[mercd_adapter_map[i]] != NULL ) {
	        if (MsdControlBlock->padapter_block_list[mercd_adapter_map[i]]->pdevi == pdrvr->pdevi) {
                    return;
                }
	    }
        }
	i--;

        pci_read_config_word(padapter->pdevi, PCI_SUBSYSTEM_ID, &SubSysId);
        padapter->phw_info->boardFamilyType = linux_pci_get_board_family(SubSysId);
        padapter->phw_info->pciSubSysId = SubSysId;
        if (padapter->phw_info->boardFamilyType == INVALID_BOARD_FAMILY) {
            printk("linux_pci_drvr_probe: SubDev Id = 0x%x (INVALID_BOARD_FAMILY Type)\n", SubSysId);
	    pdrvr->ret = MD_FAILURE;
            return;
        }

        // Pacific Chip support
	pci_read_config_word(pdrvr->pdevi, PCI_DEVICE_ID, &DeviceId);
        if (DeviceId == PCI_DEVICE_ID_DLGC_4143 ) {
            padapter->phw_info->pacific_chip = 1;

            // Pacific Chip boards have a problem with BIOS Post Configuration
            // where the eeprom burned to 1 on certain BIOS will habg the machine.
            // All Pacific Chip boards will be burned to 0 as default and the
            // driver will set the LAS0BA and LAS1BA registers to 1 to enable
            // decoding to local address spaces 0 and 1.
            pacificaddr = pci_resource_start(pdrvr->pdevi, 0);
#ifdef LFS
            Vpacificaddr = (merc_uint_t)(ulong)ioremap((size_t)pacificaddr, PCI_BASE_ADDRESS_0);
#else
            Vpacificaddr = (merc_uint_t)ioremap((size_t)pacificaddr, PCI_BASE_ADDRESS_0);
#endif

#ifdef LFS
            PacificReg = *((volatile merc_uchar_t *)((long)Vpacificaddr +  0x04 ));
#else
            PacificReg = *((volatile merc_uchar_t *)(Vpacificaddr +  0x04 ));
#endif
            PacificReg |= 0x1;
#ifdef LFS
            *((volatile merc_uchar_t *)((long)Vpacificaddr +  0x04 )) = PacificReg;
#else
            *((volatile merc_uchar_t *)(Vpacificaddr +  0x04 )) = PacificReg;
#endif

#ifdef LFS
            PacificReg = *((volatile merc_uchar_t *)((long)Vpacificaddr +  0xF4 ));
#else
            PacificReg = *((volatile merc_uchar_t *)(Vpacificaddr +  0xF4 ));
#endif
            PacificReg |= 0x1;
#ifdef LFS
            *((volatile merc_uchar_t *)((long)Vpacificaddr +  0xF4 )) = PacificReg;
#else
            *((volatile merc_uchar_t *)(Vpacificaddr +  0xF4 )) = PacificReg;
#endif
#ifdef LFS
            iounmap((void *)(long)Vpacificaddr);
#else
            iounmap((void *)Vpacificaddr);
#endif
        } else {
            padapter->phw_info->pacific_chip = 0;
        }	

        // ----> linux_pci_get_prop()
        brdprob.dip = pdrvr->pdevi;
        brdprob.phw_info = padapter->phw_info;
        (*mercd_dhal_func[MERCD_DHAL_BRD_GETPROP])((void *)&brdprob);
        if (brdprob.ret != MD_SUCCESS){
            printk("linux_pci_get_prop() ... failed\n" ) ;
           return;
        }

        // ----> linux_pci_map_adapter_parm()
        mapinfo.ret = 0;
        mapinfo.dip = pdrvr->pdevi;
        mapinfo.phw_info = padapter->phw_info;
        (*mercd_dhal_func[MERCD_DHAL_BRD_MAP_ATTR])((void *)&mapinfo);
        if (mapinfo.ret != MD_SUCCESS) {
            printk("linux_pci_map_adapter_parm() ... failed\n" ) ;
            return;
        }

	// PCI Express support
	padapter->phw_info->typePCIExpress = 0;
	pci_read_config_dword(padapter->pdevi, PCI_CAPABILITY_LIST_REG_OFFSET, &CListOffset);
	while (CListOffset) {
	    // Get to the end of the capability list to determine if the board is PCIe
	    pci_read_config_dword(padapter->pdevi, CListOffset, &NextCListOffset);
	    CurrOffset = CListOffset;
	    CListOffset = (NextCListOffset&PCI_CAPABILITY_LIST_OFFSET_MASK) >> PCI_CAPABILITY_LIST_OFFSET;
	    NextCListOffset &= PCI_CAPABILITY_TYPE_MASK;
	    
	    // Check for PCIe board
	    if (NextCListOffset == PCI_EXPRESS_CAPABILITY) {
	   	// Get all the Power Status Values located in the register after the capability list
		CurrOffset += 0x4;
		pci_read_config_dword(padapter->pdevi, CurrOffset, &PowerProvided);
		if (PowerProvided != 0xFFFFFFFF) {
		    // Get the power scale and limit values to determine actual power provided
		    PowerScale = (PowerProvided & PCI_EXPRESS_POWER_SCALE_MASK) >> PCI_EXPRESS_POWER_SCALE_OFFSET;
		    PowerLimit = (PowerProvided & PCI_EXPRESS_POWER_LIMIT_MASK) >> PCI_EXPRESS_POWER_LIMIT_OFFSET;

		    switch (PowerScale) {
		      case 0x01: padapter->phw_info->actualPowerProvided = PowerLimit/10; break;
		      case 0x10: padapter->phw_info->actualPowerProvided = PowerLimit/100; break;
		      case 0x11: padapter->phw_info->actualPowerProvided = PowerLimit/1000; break;
		      default:   padapter->phw_info->actualPowerProvided = PowerLimit; break;
		    }
		} else {
#ifdef LFS
		    printk("Error: Unable to read Power Status Registers on PCIe board at %#lx\n", CurrOffset);
#else
		    printk("Error: Unable to read Power Status Registers on PCIe board at %#x\n", CurrOffset);
#endif
		    padapter->phw_info->actualPowerProvided = 0xFFFFFFFF;
		}

		// Read the power good value and calculate the power required by the board
		CurrOffset=SEAVILLE_POWER_GOOD_REG_OFFSET;
		padapter->phw_info->powerRequiredByBoard = 0x0;
 		pacificaddr = pci_resource_start(pdrvr->pdevi, 0); 
#ifdef LFS
            	Vpacificaddr = (merc_uint_t)(long)ioremap((size_t)pacificaddr, PCI_BASE_ADDRESS_0);
#else
            	Vpacificaddr = (merc_uint_t)ioremap((size_t)pacificaddr, PCI_BASE_ADDRESS_0);
#endif
		for (j=0; j<4; j++) {
		     PowerProvided =  *(volatile ULONG * const)(Vpacificaddr+CurrOffset);
	
		     if (PowerProvided & SEAVILLE_POWER_BUDGETTING_REG_USED) {
        		 // Need to add all 4 up to calculate the required voltage
			 padapter->phw_info->powerRequiredByBoard+= (PowerProvided & SEAVILLE_POWER_WATTAGE_READ_MASK);
		     }
		
		     // Save the power good value from the first read
		     if (!j) {
			 padapter->phw_info->powerGoodReg = 
				(PowerProvided & SEAVILLE_POWER_GOOD_MASK) >> SEAVILLE_POWER_GOOD_OFFSET;
		     }
		     CurrOffset += 0x4;
		}

		// Read in the User Override Power Management value
		CurrOffset=SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET;
		PowerProvided =  *(volatile ULONG * const)(Vpacificaddr+CurrOffset);
		padapter->phw_info->powerOverrideReg = 
		   (PowerProvided & SEAVILLE_USER_OVERRIDE_POWER_MASK) >> SEAVILLE_USER_OVERRIDE_POWER_OFFSET;
		   
		MSD_LEVEL2_DBGPRINT("DrvProbe: Found PCIe - actualPower %#x, powerReq %#x, powerGood %#x, powerOverride %#x\n", 
			padapter->phw_info->actualPowerProvided, padapter->phw_info->powerRequiredByBoard,
			padapter->phw_info->powerGoodReg, padapter->phw_info->powerOverrideReg);

		CListOffset = 0;
#ifdef LFS
		iounmap((void *)(long)Vpacificaddr);
#else
		iounmap((void *)Vpacificaddr);
#endif
		padapter->phw_info->typePCIExpress = 1;
	    }
	}

	// for RH on standby - need to staticly assign the slot number
	if (RH_Enabled) {
	    // Static table entry for ZT-5085 RH
	    switch ((padapter->phw_info->bus_number << 8 | padapter->phw_info->slot_number) & 0x0000ffff) {
	      case 0x209:  padapter->phw_info->slot_number = 3;  break;
	      case 0x20a:  padapter->phw_info->slot_number = 4;  break;
	      case 0x20b:  padapter->phw_info->slot_number = 5;  break;
	      case 0x20c:  padapter->phw_info->slot_number = 6;  break;
	      case 0x20d:  padapter->phw_info->slot_number = 7;  break;
	      case 0x20e:  padapter->phw_info->slot_number = 8;  break;
	      case 0x40e:
	      case 0x150e: padapter->phw_info->slot_number = 13; break;
	      case 0x40d:
	      case 0x150d: padapter->phw_info->slot_number = 14; break;
	      case 0x40c:
	      case 0x150c: padapter->phw_info->slot_number = 15; break;
	      case 0x40b:
	      case 0x150b: padapter->phw_info->slot_number = 16; break;
	      case 0x40a:
	      case 0x150a: padapter->phw_info->slot_number = 17; break;
	      case 0x409:
	      case 0x1509: padapter->phw_info->slot_number = 18; break;
	      default: 
		   printk("Non ZT-5085 Chassis\n"); 
        	   LocatorId = 
		      MsdRegReadUchar(padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr+PCI_CR_OFFSET_LOCATOR_ID);
	           if (LocatorId != 0xFF) {
                       LocatorId &= 0x1F;
                       padapter->phw_info->slot_number = (LocatorId & 0xFF);
                   }
                   break;
            }
        } 

        // ----> linux_pci_save_attr()
        saveinfo.ConfigStart = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;
        saveinfo.ConfigId= i;
        saveinfo.phw_info = padapter->phw_info;
        saveinfo.phost_info = padapter->phost_info;
        (*mercd_dhal_func[MERCD_DHAL_BRD_SAVE_ATTR])((void *)&saveinfo);
	
	Mercd_Adapter_Count++;
        MsdControlBlock->adapter_count = Mercd_Adapter_Count;
        padapter->pww_info = NULL;

        for (i = 0; i<= MSD_MAX_BOARD_ID_COUNT; ++i) {
             if (mercd_adapter_map[i] == 0xFF)
                 break;
        }

        if (i >= MSD_MAX_BOARD_ID_COUNT) {
            pdrvr->ret = MD_FAILURE;
            return;
        }

        // We will use the config id as slot number from now on...
        if (NonDefaultId) {
            padapter->phw_info->slot_number = i;
        }

	padapter->phw_info->clash_slot_id = 0xFF;
        if (MsdControlBlock->padapter_block_list[padapter->phw_info->slot_number] != NULL) {
            printk("WARN: Possible Slot/Board id %d clash!!! or cPCI Chassis Issue!!!\n", padapter->phw_info->slot_number);
            for (j=1;j<MSD_MAX_BOARD_ID_COUNT;j++)
                if (MsdControlBlock->padapter_block_list[j]  == NULL) {
                    padapter->phw_info->slot_number = j;
		    padapter->phw_info->clash_slot_id = j;
                    break;
                }
        }

        mercd_adapter_map[i] = padapter->phw_info->slot_number;
        MsdControlBlock->padapter_block_list[padapter->phw_info->slot_number] = padapter;
        padapter->adapternumber = i;

        pdrvr->ret = MD_SUCCESS;
        return;
}

/***************************************************************************
 * Function Name                : linux_pci_brd_probe
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            : MERCD_DHAL_BRD_PROBE
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_brd_probe(void *rcvPtr)
{
        pmercd_dhal_drvr_probe_sT   pdrvr = (pmercd_dhal_drvr_probe_sT) rcvPtr;
        mercd_dhal_intr_enable_sT   intrinfo ={ 0 };
#ifndef LFS
        mercd_dhal_brd_map_attr_sT  mapinfo = { 0 };
#endif
        mercd_dhal_drvr_verify_sT   drvinfo = { 0 };
        mercd_dhal_brd_save_attr_sT saveinfo ={ 0 };
        mercd_osal_intr_register_sT intrinfo_reg = { 0 };
        pmercd_adapter_block_sT padapter;
        merc_ushort_t           CmdReg;
        merc_ushort_t           HsCsr;
        merc_uint_t             DelayVal = 1;
        mercd_hs_t*             hsd = NULL;
        mercd_dhal_dma_init_sT  dmainit = { 0 };

        pdrvr->ret = MD_FAILURE;

        hsd = (mercd_hs_t*)pci_get_drvdata(pdrvr->pdevi);
        if (!hsd) {
            printk("mercd_brd_probe: hsd is Null\n");
            return;
        }
        padapter = hsd->padapter;

        if (!padapter) {
            printk("mercd_brd_probe: Adapter is Null\n");
            return;
        }

        if (padapter->phw_info->pciSubSysId == SUBSYSID_ROZETTA_21554) {
            padapter->rtkMode = 1;
        } else {
            padapter->rtkMode = 0;
        }

        padapter->pww_info = NULL;
	padapter->flags.WWFlags = 0;
        padapter->flags.SendTimeoutPending = 0;

        pci_read_config_word(pdrvr->pdevi, PCI_CONF_COMM, &CmdReg);
        CmdReg = (PCI_COMM_MAE | PCI_COMM_ME);
        pci_write_config_word(pdrvr->pdevi, PCI_CONF_COMM, CmdReg);
        pci_read_config_word(pdrvr->pdevi, PCI_CONF_COMM, &CmdReg);

        //Clear HsCsr
        if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
            pci_read_config_word(pdrvr->pdevi, PCI_CONF_HSCSR, &HsCsr);
            HsCsr |= PCI_HSCSR_CLR_INTR;
            pci_write_config_word(pdrvr->pdevi, PCI_CONF_HSCSR,  HsCsr);
        } else {
            pci_read_config_word(pdrvr->pdevi, PCI_CONF_21554_HSCSR, &HsCsr);
            HsCsr |= CPCI_HSCSR_CLR_INTR;
            pci_write_config_word(pdrvr->pdevi, PCI_CONF_21554_HSCSR,  HsCsr);
        }

        padapter->flags.PostFlags = MD_MIN_POST_ENABLE;
        if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
            reset_adapter(padapter);
            mid_wwmgr_check_3rdrock_running(padapter, MERCD_WW_BOOT_KERNEL);
        } else {
            padapter->phost_info->reg_block.HostRamStart = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;
            padapter->phost_info->reg_block.SetBoardReset=
                padapter->phost_info->reg_block.HostRamStart+MERC_SET_BOARD_RESET_REG;
            padapter->phost_info->reg_block.ClrBoardReset=
                padapter->phost_info->reg_block.HostRamStart+MERC_CLR_BOARD_RESET_REG;
            reset_adapter(padapter);
            set_current_state(TASK_UNINTERRUPTIBLE);
            schedule_timeout(((DelayVal * 2 * 500)*HZ)/1000);
            set_current_state(TASK_RUNNING);
	}

        saveinfo.ConfigStart = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;
        saveinfo.ConfigId = padapter->adapternumber;
        saveinfo.phw_info = padapter->phw_info;
        saveinfo.phost_info = padapter->phost_info;
        (*mercd_dhal_func[MERCD_DHAL_BRD_SAVE_ATTR])((void *)&saveinfo);
 
        if (padapter->phw_info->clash_slot_id != 0xFF) {
            padapter->phw_info->slot_number = padapter->phw_info->clash_slot_id;
        }

        MSD_ENTER_CONTROL_BLOCK_MUTEX();
        if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
            (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
            MSD_ZERO_MEMORY( (pmerc_char_t)&padapter->snd_msg_queue, sizeof(MSD_QUEUE) );
            padapter->snd_msg_Array.array = NULL;
            MsdControlBlock->arraySz += MSD_ARRAY_SIZE_SRAM;
        } else {
            // Array Implementation
#ifdef LFS
            mercd_zalloc(padapter->snd_msg_Array.array, Uint32 **, (sizeof(Uint32 *)*MSD_ARRAY_SIZE));
#else
            mercd_zalloc(padapter->snd_msg_Array.array, Uint32 *, (sizeof(Uint32 *)*MSD_ARRAY_SIZE));
#endif
            if (padapter->snd_msg_Array.array == NULL) {
                printk("mercd_probe: memory allocation failed for array\n");
                pdrvr->ret = MD_FAILURE;
                return;
            }

            padapter->snd_msg_Array.head = 0;
            padapter->snd_msg_Array.tail = 0;
            MsdControlBlock->arraySz += MSD_ARRAY_SIZE_WW;
        }
        MSD_EXIT_CONTROL_BLOCK_MUTEX();

        // Check the SRAM for the valid Dm3 id's 
        drvinfo.ConfigStart = padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr;
        drvinfo.ConfigId = padapter->adapternumber;

        // ----> linux_pci_intr_register()
        if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
            (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
            // SRAM Family Board
            intrinfo_reg.Isr = (PMSD_FUNCTION) linux_intr_srvc;
            // Initialize the DPC
            tasklet_init(&(padapter->phw_info->intr_info->dpc_task), 
		   (void *)mercd_generic_intr_processing, (unsigned long)&padapter->phw_info->slot_number);
        } else {
            // WW Family Board
            intrinfo_reg.Isr = (PMSD_FUNCTION) linux_ww_intr_srvc;
            // Initialize the DPC
            tasklet_init(&(padapter->phw_info->intr_info->dpc_task), 
		   (void *)mercd_ww_generic_intr_processing, (unsigned long)&padapter->phw_info->slot_number);
        }
        intrinfo_reg.dip = pdrvr->pdevi;
        intrinfo_reg.IsrArg = (size_t)padapter;
	intrinfo_reg.phw_info= padapter->phw_info;
        intrinfo_reg.ConfigId= padapter->phw_info->slot_number;
        intrinfo_reg.Level = padapter->phw_info->intr_info->level;
        intrinfo_reg.IntrVector = padapter->phw_info->intr_info->intr_vec;
        intrinfo_reg.IsrHandle = &padapter->phw_info->intr_info->isr_handle;
        (*mercd_osal_func[MERCD_OSAL_INTR_REGISTER])((void *)&intrinfo_reg);
        if (intrinfo_reg.ret != MD_SUCCESS) {
            printk("mercd_brd_probe: intr_register() ... failed\n" ) ;
            pdrvr->ret = MD_FAILURE;
            return;
        }

        intrinfo.ConfigId = padapter->adapternumber;
        intrinfo.phw_info = padapter->phw_info;
        // For DMV-B, do not enable interrupts
        if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DMVB) &&
            (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_NEWB)) {
           (*mercd_dhal_func[MERCD_DHAL_INTR_ENABLE])((void *)&intrinfo);
        }

        if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
            dmainit.ConfigId = padapter->adapternumber;
            (*mercd_dhal_func[MERCD_DHAL_DMA_ALLOC_HANDLE]) ((void *)&dmainit);
            if (dmainit.ret != MD_SUCCESS) {
                printk("mercd_brd_probe: dma_alloc() ... failed\n" ) ;
                pdrvr->ret = MD_FAILURE;
                return;
            }
        }

        padapter->state = MERCD_ADAPTER_STATE_INIT;
        pdrvr->ret = MD_SUCCESS;
        return;
}




/***************************************************************************
 * Function Name                : linux_pci_drvr_free
 * Function Type                : Common Hardware function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_drvr_free(void *rcvPtr)
{
        pmercd_dhal_drvr_free_sT pdrvr = (pmercd_dhal_drvr_free_sT) rcvPtr;
#ifndef LFS
        mercd_osal_timeout_stop_sT timeoutinfo = { 0 };
#endif
        mercd_dhal_brd_unmap_attr_sT brdunmapinfo = { 0 };
        pmercd_adapter_block_sT padapter;
#ifndef LFS
        merc_int_t i, j;
#endif
        mercd_hs_t* hsd = NULL;

        if (!MsdControlBlock->adapter_count) {
            printk("linux_pci_drvr_free: This is impossible; no adapters \n");
        }

        pdrvr->ret = MD_SUCCESS;

        hsd = (mercd_hs_t*)pci_get_drvdata(pdrvr->pdevi);
        if (!hsd) {
           printk("linux_pci_drvr_free: Hsd is Null\n");
           return;
        }

        if ((padapter = hsd->padapter) == NULL) {
	    printk("linux_pci_drvr_free: padapter is Null\n");
	    return;
	}

        // For a board (both SRAM/3rdRock) running WW mode, lets check some
        // conditions that may be useful for debugging.
        if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
            if (!(padapter->pww_info->state & MERCD_ADAPTER_WW_IN_SHUTDOWN))
                MSD_LEVEL2_DBGPRINT("Attempting to UNMAP the board without SHUTDOWN..\n");
        }

        // UNMAPPING: The following should work for:
        //  SRAM boards: running SARM protocol.
        //  SRAM boards: running WW protocol.
        //  3rdRock boards: running WW protocol.
        // Because, we always used 2 mappings with virt_map_q 
        // at indexes PCI_BRIDGE_MAP and PCI_SRAM_MAP.
        brdunmapinfo.ret = 0;
        brdunmapinfo.dip = padapter->pdevi;
        brdunmapinfo.ConfigId = padapter->adapternumber;
        brdunmapinfo.phw_info = padapter->phw_info;
        (*mercd_dhal_func[MERCD_DHAL_BRD_UNMAP_ATTR])((void *)&brdunmapinfo);
        if (brdunmapinfo.ret != MD_SUCCESS) {
            pdrvr->ret = ! MD_SUCCESS;
            printk("linux_pci_drvr_free: unmap is cfgid(%d) failed\n", padapter->adapternumber);
        }

        mercd_free(padapter->phw_info->intr_info, MERCD_INTR_INFO, MERCD_FORCE);
        mercd_free(padapter->phw_info->timer_info, MERCD_TIMER_INFO, MERCD_FORCE);
        mercd_free(padapter->phw_info, MERCD_HW_INFO, MERCD_FORCE);
        mercd_free(padapter->phost_info, MERCD_HOST_INFO, MERCD_FORCE);
        mercd_free(padapter->pstream_connection_list, MERCD_STREAMS_BLOCK_LIST, MERCD_FORCE);
        MSD_FREE_KERNEL_MEMORY(padapter->snd_msg_Array.array, (sizeof(Uint32 *)*MSD_ARRAY_SIZE));

        // decrease the array size for this adapter only
        if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
            (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
            MsdControlBlock->arraySz -= MSD_ARRAY_SIZE_SRAM;
        } else {
            MsdControlBlock->arraySz -= MSD_ARRAY_SIZE_WW;
        }

        // make sure size does not go to negative
        if (MsdControlBlock->arraySz < 0) {
            MsdControlBlock->arraySz = 0;
        }

	// HCS Support - free the state array
        if (padapter->isHCSSupported) {
            MSD_FREE_KERNEL_MEMORY(padapter->HCS.pStreamStateArray, (sizeof(Uint32)*padapter->HCS.maxNumOfHCS));
        }

        mercd_free(padapter, MERCD_ADAPTER_BLOCK, MERCD_FORCE);

        MsdControlBlock->padapter_block_list[mercd_adapter_map[padapter->adapternumber]] = NULL;
        mercd_adapter_map[padapter->adapternumber] = 0xFF;

        Mercd_Adapter_Count = MsdControlBlock->adapter_count-1;
        
}

/***************************************************************************
 * Function Name                : linux_pci_get_board_family
 * Function Type                :
 * Inputs                       : subSystemIdToFind
 * Outputs                      : return Board FAMILY TYPE
 * Calling functions            :
 * Description                  :
 * Additional comments          : THIRD_ROCK Support
 *                              : Returns BOARD FAMILY TYPE
***************************************************************************/

merc_ushort_t linux_pci_get_board_family(merc_ushort_t subSystemIdToFind)
{
  merc_uint_t index = 0;

  MSD_FUNCTION_TRACE("supp_snd_cfg_rply", ONE_PARAMETER, (size_t)subSystemIdToFind);

  while (DM3DeviceTypes[index].boardFamilyType != INVALID_BOARD_FAMILY) {
       if (DM3DeviceTypes[index].subsystemId == subSystemIdToFind)
               return (DM3DeviceTypes[index].boardFamilyType);
           index ++;
  }
  return INVALID_BOARD_FAMILY;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_mf_mem_alloc
 * Function Type                : This Allocs Mem for MFs and MSG Blks
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_ww_mf_mem_alloc(void *rcvPtr)
{
#ifndef LFS
 int ret;
#endif
 pmercd_adapter_block_sT padapter;
 pmercd_ww_dev_info_sT pwwDev;
 pmercd_dhal_ww_msgmf_alloc_sT palloc;
 volatile pmerc_void_t vaddress;
 volatile pmerc_void_t palign;
 merc_ulong_t size;
 merc_ww_mf_memory_sT *resource;
 int order;


  MSD_FUNCTION_TRACE("linux_pci_ww_mf_mem_alloc", ONE_PARAMETER, (size_t)rcvPtr);
  palloc = (pmercd_dhal_ww_msgmf_alloc_sT)rcvPtr;
  palloc->ret = MD_SUCCESS;
  pwwDev = palloc->pwwDevi;
  resource = (merc_ww_mf_memory_sT *)palloc->resource;
  padapter = (pmercd_adapter_block_sT)pwwDev->padapter;


  //1.Allocate a non-paged contiguous memory pool and get a base
  //  virtual address.

  size = resource->size;

  //NOTE: The following for sure returns a contiguous pool
  //of memory. But, as per the documentation the limitation
  //is that the __get_dma_pages will only take up
  //to a maximum order of 5, i.e we get only 32 contiguous
  //pages. The following abstract_get_free_pages is equivalent to
  //calling :
  // int order = __get_order(size);
  //return __get_dma_pages(GFP_KERNEL, order);
  //LiS will take care of the responsibility of calculating the order
  //depending on the size.
  //But, in 2.2 kernel, abstract_get_free_pages is calling
  //abstract_free_pages(GFP_KERNEL..) not __get_dma_pages

  //vaddress = abstract_get_free_pages(size);
  //NOTE: when tried with abstract_get_free_pages, some how board
  //could not do the Direct IO for the first MF where we
  //send the init params. I see data transfer abort on the
  //console port. The physical address of the MF is same as
  //I see in my driver. May be b'cos I am using 2.2 kernel.
  //Need to Test this in 2.4 kernel.

  //But, Board could direct IO, for the address I got from
  //abstract_alloc_dma(). From the FAQs, this also gives a
  //contiguous memory, but prone to fail.

  //As per the FAQs, pci_malloc_consistent is more reliable
  //for DMAble memory, and this call is available in 2.4
  //kernels.

  //For using the abstract_alloc_dma, which calls abstract_malloc
  //with GFP_DMA priority.

  // DMV-B increased MFAs require more DMA memory - using get_free_pages
  for (order = 0; (1 << order) * PAGE_SIZE < size; order++);
#ifdef LFS
  vaddress = (void *)__get_free_pages(GFP_DMA, order);
#else
  vaddress = __get_free_pages(GFP_DMA, order);
#endif

  if (vaddress == NULL) {
      printk("abstract_alloc_dma FAILED...\n");
      palloc->ret = MD_FAILURE;
      return;
  }

  //2.Do the 128 Byte alignment
  palign = (pmerc_void_t)(((size_t)vaddress + 0x80)&(~0x7f));
  resource->host_address = palign;

  //3. Store the vaddress separately, since we advanced by
  //0x80 before finding a 128byte boundary. We need this
  //while freeing up the memory.
  resource->start_address = vaddress;

  //4. Get the Physical Address
  resource->board_address = (pmerc_void_t)virt_to_phys(palign);

  //5. Save the order which will be needed in the free
  resource->order = order;

return;
}

/**************************************************************************************
 * Function Name                : linux_pci_ww_recv_descriptor_table_from_strm_buffers
 * Function Type                : This Allocs DMAble Mem for BIG MSG Blks from strems
 *                              : mblks
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          : Allocation SCHEME1
 ***************************************************************************************/
void linux_pci_ww_recv_bigmsg_descriptor_table_from_strm_buffers(void *rcvPtr)
{
#ifndef LFS
 int                                           ret;
#endif
 pmercd_dhal_ww_rcv_copybigmsg_str_sT    pdir_copy;
 pmercd_adapter_block_sT                  padapter;
 pmercd_ww_dev_info_sT                      pwwDev;
 pmercd_ww_dma_descr_sT                    pbigmsg;
 merc_ww_rcvbigmsg_memory_sT             *resource;
#ifndef LFS
 merc_ulong_t                                 size;
#endif
 merc_ulong_t                           numBigMsgs;
 merc_ulong_t                             szBigMsg;
 merc_uint_t                                 count;
 merc_uint_t                               purpose;
 merc_uint_t                           start_index;
 merc_uint_t                             end_index;
 PSTRM_MSG                                     Msg;
 //Extended Stream Buffers
 pmerc_uchar_t                           pesbuffer;
#if defined LiS || defined LIS || defined LFS
 frtn_t                                   *pesbfrtn;
#endif /* LiS */

 MSD_FUNCTION_TRACE("linux_pci_ww_recv_descriptor_table_from_strm_buffers", ONE_PARAMETER, 
                     (size_t)rcvPtr);

 pdir_copy = (pmercd_dhal_ww_rcv_copybigmsg_str_sT)rcvPtr;
 pdir_copy->ret = MD_SUCCESS;

 //Get WW struct and Adapter struct
 pwwDev = pdir_copy->pwwDevi;
 padapter = pwwDev->padapter;

 //Purpose of the call
 purpose = pdir_copy->purpose;

 //Receive BIG Msg Blk Mem structure
 resource = &(pwwDev->BigMsgRcvMemStr);

 //Get the Number and Size of Big Message Blocks
 numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
 szBigMsg = pwwDev->pww_param->sizeofBigMsgBlocks;

 if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC)
 {
   //Allocate the index table
   resource->pbigMsgDescr = (pmercd_ww_dma_descr_sT *)
           mercd_allocator(numBigMsgs * sizeof(pmercd_ww_dma_descr_sT));
   if (resource->pbigMsgDescr == NULL)
   {
     printk("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
             Alloc for pbigMsgDescr Failed\n");
     MSD_ERR_DBGPRINT("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
             Alloc for pbigMsgDescr Failed\n");
     pdir_copy->ret = MD_FAILURE;
     return;
   }

  start_index = 0;
  end_index = numBigMsgs;

 }
 else
 { /* Single Descriptor Alloc */
  start_index = pdir_copy->index;
  end_index = pdir_copy->index+1;
 } /* if DESC table ALLOC */

 //Get into each index of the table if this is the whole table
 //allocation. If it is just one descriptor allocation, just
 //get to the descriptor at the index
 for ( count = start_index; count < end_index; count++)
 {

   if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC) {
       //Alloc pmercd_dma_descr_sT
       pbigmsg = (pmercd_ww_dma_descr_sT) mercd_allocator(MERCD_WW_BIGMSG_DESCR);
       if (pbigmsg == NULL)
        {
     	  printk("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
             Alloc for pbigmsg Failed\n");
     	  MSD_ERR_DBGPRINT("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
             Alloc for pbigmsg Failed\n");
     	  pdir_copy->ret = MD_FAILURE;
     	  return;
        }
       pbigmsg->flag = 0;
    }
   else
    {
       pbigmsg = resource->pbigMsgDescr[count];
       pbigmsg->flag = 0;
    }

   //Bzero the pbigmsg memory..
   MSD_ZERO_MEMORY(pbigmsg, MERCD_WW_BIGMSG_DESCR);

   //Allocate an mblk
   Msg = supp_alloc_buf(szBigMsg, GFP_ATOMIC);

   //If any allocb fails, return MD_FAILURE, return for now
   //until we come up with a SCHEME2

     if (Msg == NULL)
     {
        printk("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
                mblk allocation Failed\n");
        MSD_ERR_DBGPRINT("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
                mblk allocation Failed\n");
        pdir_copy->ret = MD_FAILURE;
        return;
       //WWPENDING: Think about the SCHEME2 in future.
        //can we use abstract_alloc_dma(). Looks like
        //this is not as reliable as abstract_get_free_pages.
        //We may endup crossing a page boundary with
        //abstract_alloc_dma
     }/*Msg == NULL*/

     if (Msg->b_cont)
     {
        printk("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
                mblk has b_cont\n");
        MSD_ERR_DBGPRINT("linux_pci_ww_recv_descriptor_table_from_strm_buffers:\
                mblk has b_cont\n");
        pdir_copy->ret = MD_FAILURE;
        abstract_freeb(Msg);
        return;
     }

   //Now see if the mblk of size 'szBigMsg' is crossing a page
   //boundary. If this is within a page boundary, then accept it.
   //Else, free the streams msg, do esballoc on abstract_get_free_pages.
   //NOTE: Here we presume that the admin would not ask for a
   //BIG Msg Block, that is too big, so that we end up crossing
   //more than one page boundary. But, we dont really care, we
   //just discard the previous mblk and do esballoc.

   if (((size_t)(Msg->b_rptr)| (PAGE_SIZE - 1)) > (size_t) (Msg->b_wptr - 1))
    {
       //Did not exceed the page boundary. Just save it
       pbigmsg->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT |
                       MERCD_WW_DMA_DESCR_EBIT);
       pbigmsg->host_address = (pmerc_void_t)Msg;
       pbigmsg->board_address =  (pmerc_void_t)virt_to_phys((pmerc_void_t)(Msg->b_rptr));
       pbigmsg->size = szBigMsg;
       //If this is the whole table allocation store the new descriptor
       if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC)
                 resource->pbigMsgDescr[count]=pbigmsg;
    }
   else
    {
#if defined LiS || defined LIS || defined LFS
       pbigmsg->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT |
                       MERCD_WW_DMA_DESCR_PBIT |  MERCD_WW_DMA_DESCR_EBIT);

       //Free the Message
       abstract_freeb(Msg);

       //Do an esballoc
        pesbuffer = abstract_get_free_pages(szBigMsg);
#ifdef LFS
        pesbfrtn = (frtn_t *)mercd_allocator(sizeof(frtn_t));
#else
        pesbfrtn = (pmerc_uchar_t)mercd_allocator(sizeof(frtn_t));
#endif
        pesbfrtn->free_func = supp_esb_free;
        pesbfrtn->free_arg = pesbuffer;
        Msg = abstract_esballoc(pesbuffer, szBigMsg, BPRI_MED, pesbfrtn);
        pbigmsg->host_address = (pmerc_void_t)Msg;
        pbigmsg->board_address =  (pmerc_void_t)virt_to_phys((pmerc_void_t)pesbuffer);
        pbigmsg->size = szBigMsg;
        //If this is the whole table allocation store the new descriptor
        if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC)
                   resource->pbigMsgDescr[count]=pbigmsg;
#endif /* LiS */
      }
 } /* count */
  return;
}

/**************************************************************************************
 * Function Name        : linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers
 * Function Type        : This Allocs DMAble Mem for BIG MSG Blks from strems
 *                      : mblks
 * Inputs               :
 * Outputs              :
 * Calling functions    :
 * Description          :
 * Additional comments  : This caters to the Receive Streams
 ***************************************************************************************/
void linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers(void *rcvPtr)
{
#ifndef LFS
 int                                           ret;
#endif
 pmercd_dhal_ww_rcv_copybigmsg_str_sT    pdir_copy;
 pmercd_adapter_block_sT                  padapter;
#ifdef LFS
 pmercd_ww_dev_info_sT                      pwwDev;
 pmercd_ww_dma_descr_sT                    pdmadescr = NULL;
#else
 pmercd_ww_dma_descr_sT                    pdmadescr;
#endif
#ifndef LFS
 merc_ulong_t                                 size;
#endif
 merc_ulong_t                               szData;
 merc_uint_t                                 count;
 merc_uint_t                               purpose;
#ifdef LFS
 merc_uint_t                           start_index = 0;
 merc_uint_t                             end_index = 0;
#else
 merc_uint_t                           start_index;
 merc_uint_t                             end_index;
#endif
 PSTRM_MSG                    Msg, DataMsg, RcvMsg;
 //Extended Stream Buffers
#ifndef LFS
 pmerc_uchar_t                           pesbuffer;
#if defined LiS || defined LIS || defined LFS
 frtn_t                                   *pesbfrtn;
#endif /* LiS */
#endif
 pmercd_stream_connection_sT           StreamBlock;

 MSD_FUNCTION_TRACE("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers", 
                      ONE_PARAMETER, 
                     (size_t)rcvPtr);

 pdir_copy = (pmercd_dhal_ww_rcv_copybigmsg_str_sT)rcvPtr;
 pdir_copy->ret = MD_SUCCESS;

#ifdef LFS
 //Get WW struct and Adapter struct
 pwwDev = pdir_copy->pwwDevi;
 padapter = pwwDev->padapter;
#endif

 //Purpose of the call
 purpose = pdir_copy->purpose;
 StreamBlock = (pmercd_stream_connection_sT)(pdir_copy->pwwDevi);

////////////////////////////////////////////////////////////////
//For BStream: szData = 4056;
//For GStream: szData could be anything
////////////////////////////////////////////////////////////////
szData = StreamBlock->WWRcvBufSize; 


if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC)
  {
       StreamBlock->pRcvStrmDmaDesc = (pmercd_ww_dma_descr_sT *)
	       mercd_allocator(StreamBlock->WWRcvNumBufs *
                               sizeof(pmercd_ww_dma_descr_sT));

       if (StreamBlock->pRcvStrmDmaDesc == NULL)
        {
          printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pRcvStrmDmaDesc Alloc Failed\n");
          MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pRcvStrmDmaDesc Alloc Failed\n");
          pdir_copy->ret = MD_FAILURE;
          return;
        }

       MSD_ZERO_MEMORY(StreamBlock->pRcvStrmDmaDesc,
		       StreamBlock->WWRcvNumBufs * sizeof(pmercd_ww_dma_descr_sT));
       start_index = 0;
       end_index = StreamBlock->WWRcvNumBufs;
  }
else
 if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_ALLOC)
   {
        start_index = pdir_copy->index;
        end_index = pdir_copy->index+1;
   }


////////////////////////////////////////////////////////////////
 //Get into each index of the table if this is the whole table
 //allocation. If it is just one descriptor allocation, just
 //get to the descriptor at the index
 for ( count = start_index; count < end_index; count++)
 {
   if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC) 
    {
       //Alloc pmercd_dma_descr_sT
       pdmadescr = (pmercd_ww_dma_descr_sT) mercd_allocator(MERCD_WW_BIGMSG_DESCR);

       if (pdmadescr == NULL)
        {
          printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pdmadescr Alloc Failed\n");
          MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pdmadescr Alloc Failed\n");
          pdir_copy->ret = MD_FAILURE;
          return;
        }

       pdmadescr->flag = 0;
       //Bzero the pdmadescr memory..
       MSD_ZERO_MEMORY(pdmadescr, MERCD_WW_BIGMSG_DESCR);
       //We Need One More for BStream
       if (!(StreamBlock->type ==  STREAM_OPEN_F_GSTREAM))
         {
          pdmadescr->next = (pmercd_ww_dma_descr_sT) 
                            mercd_allocator(MERCD_WW_BIGMSG_DESCR);

          if (pdmadescr->next == NULL)
           {
             printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pdmadescr->next Alloc Failed\n");
             MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  pdmadescr->next Alloc Failed\n");
             pdir_copy->ret = MD_FAILURE;
             return;
           }

          pdmadescr->next->flag = 0;
          //Bzero the pdmadescr->next memory..
          MSD_ZERO_MEMORY(pdmadescr->next, MERCD_WW_BIGMSG_DESCR);
         }
     }
    else
     {
      if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_ALLOC)
       {
        pdmadescr = StreamBlock->pRcvStrmDmaDesc[count];

	// leak fix: check if block was not used so no need to allocate again
	if (pdmadescr->host_address)
            continue;

        pdmadescr->flag = 0;
        pdmadescr->host_address = 0;
        pdmadescr->board_address = 0;
        //There is a next descriptor for BStream
        if (!(StreamBlock->type ==  STREAM_OPEN_F_GSTREAM))
         {
         if (pdmadescr->next) 
          {
           pdmadescr->next->flag = 0;
           pdmadescr->next->host_address = 0;
           pdmadescr->next->board_address = 0;
           //Bzero the pdmadescr->next memory..
           MSD_ZERO_MEMORY(pdmadescr->next, MERCD_WW_BIGMSG_DESCR);
          }
        }
       }
     }

     ///////////////////////////////////////////////////////////
     //Allocate mblks Depending on the StreamBlock Type
     //MERCURY_HOST_IF_BLK_SIZE = 4032 
     //sizeof(USER_HEADER) = 24
     //////////////////////////////////////////////////////////
     Msg = supp_alloc_buf(sizeof(MDRV_MSG)+sizeof(STREAM_RECEIVE), GFP_ATOMIC);
     if (Msg == NULL)
      {
         printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk1 Alloc Failed\n");
         MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk1 Alloc Failed\n");
         pdir_copy->ret = MD_FAILURE;
         return;
      }

     //Alloc the Datablock depending on the type of the Streams
     if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
      {
	DataMsg = supp_alloc_buf(szData, GFP_ATOMIC);
	if (DataMsg == NULL)
	 {
           printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk2 Alloc Failed for GStream\n");
           MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk2 Alloc Failed for GStream\n");
           pdir_copy->ret = MD_FAILURE;
           return;
	 }
	MSD_LINK_MESSAGE(Msg,DataMsg);

       //Leave the message on pdmadescr
       pdmadescr->host_address = (pmerc_void_t)Msg;
       //There is no next descriptor for G Streams
      }
     else /* BSTREAM */
      {
        DataMsg = supp_alloc_buf(MERCURY_HOST_IF_BLK_SIZE, GFP_ATOMIC);
       
        if(DataMsg == NULL)
         {
           printk("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk2 Alloc Failed for BStream\n");
           MSD_ERR_DBGPRINT("linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers\
                  mblk2 Alloc Failed for BStream\n");
           pdir_copy->ret = MD_FAILURE;
           return;
         }
	MSD_LINK_MESSAGE(Msg,DataMsg);
       //Leave the message on pdmadescr
       pdmadescr->host_address = (pmerc_void_t)Msg;
       pdmadescr->next->host_address = (pmerc_void_t)Msg->b_cont;
     } /* BSTREAM */

    //If this is the whole table allocation store the new descriptor
    if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC)
         StreamBlock->pRcvStrmDmaDesc[count] = pdmadescr;
  }/* for */


//Now see if the mblk of size 'szBigMsg' is crossing a page
//boundary. If this is within a page boundary, then accept it.
//Else, free the streams msg, do esballoc on abstract_get_free_pages.

 for ( count = start_index; count < end_index; count++)
 {
   pdmadescr = StreamBlock->pRcvStrmDmaDesc[count];
   RcvMsg = pdmadescr->host_address;
     if (StreamBlock->type == STREAM_OPEN_F_GSTREAM)
      {
         /////////////////////////////////////////////////////////////////////
         //GSTREAM
         /////////////////////////////////////////////////////////////////////
         if (((size_t)(RcvMsg->b_rptr) + (PAGE_SIZE - 1)) > 
           (size_t) (RcvMsg->b_wptr - 1))
         {
            //Did not exceed the page boundary. Just save it
            pdmadescr->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT |
                      MERCD_WW_DMA_DESCR_EBIT);
            //pdmadescr->host_address is alredy Filled
            pdmadescr->board_address =  
                     (pmerc_void_t) pci_map_single(
                                                    padapter->pdevi,
                                                    (pmerc_void_t)(RcvMsg->b_cont->b_rptr),
                                                    szData,
                                                    PCI_DMA_FROMDEVICE
                                                  );
            pdmadescr->size = szData;
         }
       else
         {

           printk("mercd: No Support for esballoc (G)\n");
           pdir_copy->ret = MD_FAILURE;
           return;
#if 0
            //////////////////////////////////////////////////////////////////////
            //No esballoc: support at this point
            //////////////////////////////////////////////////////////////////////
            pdmadescr->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT |
                       MERCD_WW_DMA_DESCR_PBIT |  MERCD_WW_DMA_DESCR_EBIT);
            //Free the Message
            abstract_freeb(Msg);

            //Do an esballoc
            pesbuffer = abstract_get_free_pages(szData);
            pesbfrtn = (pmerc_uchar_t)mercd_allocator(sizeof(frtn_t));
            pesbfrtn->free_func = supp_esb_free;
            pesbfrtn->free_arg = pesbuffer;
            Msg = abstract_esballoc(pesbuffer, szBigMsg, BPRI_MED, pesbfrtn);
            pdmadescr->host_address = (pmerc_void_t)Msg;
            pdmadescr->board_address =  (pmerc_void_t)
                     virt_to_phys((pmerc_void_t)pesbuffer);
            pdmadescr->size = szData;
            //If this is the whole table allocation store the new descriptor
            if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC)
                 StreamBlock->pRcvStrmDmaDesc[count] = pdmadescr;
            /////////////////////////////////////////////////////////////////////
#endif

         }
      } /* GSTREAM */
    else
    { 
      /////////////////////////////////////////////////////////////////
      //BSTREAM
      /////////////////////////////////////////////////////////////////
      merc_uint_t hdr_message = 1;
      while ((RcvMsg) && (pdmadescr))
       {
        if (((size_t)(RcvMsg->b_rptr) + (PAGE_SIZE - 1)) >
            (size_t) (RcvMsg->b_wptr - 1))
         {
          if (!hdr_message)
           {
             //Did not exceed the page boundary. Just save it
             pdmadescr->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT |
                      MERCD_WW_DMA_DESCR_EBIT);
             //pdmadescr->host_address is alredy Filled
             pdmadescr->board_address = (pmerc_void_t)
                                        pci_map_single (
                                                         padapter->pdevi,
                                                         (pmerc_void_t)(RcvMsg->b_rptr),
                                                         MERCURY_HOST_IF_BLK_SIZE,
                                                         PCI_DMA_FROMDEVICE
                                                        );

             pdmadescr->size = MERCURY_HOST_IF_BLK_SIZE;
            }
           else
            {
             pmerc_uchar_t      brdaddress;
             PMDRV_MSG          MdMsg;
             PSTREAM_RECEIVE      Ptr;
             pdmadescr->flag = (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT );
             //pdmadescr->host_address is already FILLED
             MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(RcvMsg);
             Ptr = (PSTREAM_RECEIVE)MD_GET_MDMSG_PAYLOAD(MdMsg);
             brdaddress = &Ptr->StreamUserHeader.UserHeaderBytes[0];
             pdmadescr->board_address =  (pmerc_void_t)
                                          pci_map_single (
                                                          padapter->pdevi,
                                                          brdaddress,
                                                          sizeof(USER_HEADER),
                                                          PCI_DMA_FROMDEVICE
                                                        );

             pdmadescr->size = sizeof(USER_HEADER);
             //For a B Stream we have a next descriptor for data
           }
         }
        else
         {
           ////////////////////////////////////////////////////////
           //No esballoc support at this point
           ////////////////////////////////////////////////////////
           printk("mercd: No Support for esballoc (B)\n");
           pdir_copy->ret = MD_FAILURE;
           return;
         }

         RcvMsg= RcvMsg->b_cont;
         pdmadescr = pdmadescr->next;
         if ((hdr_message) && ((!RcvMsg) || (!pdmadescr)))
            {
              printk("This Should Never Happen. INVESTIGATE..\n");
              return;
            }
         hdr_message = 0;
      } /* while */

    } /* BSTREAM */
    
 } /* count */

  return;
}

/********************************************************************************
 * Function Name: linux_pci_ww_send_descriptor_table_from_strm_buffers
 * Function Type: This Allocs DMAble stream buffers for a write stream operation
 * Inputs:
 * Outputs:
 * Calling functions:
 * Description:
 * Additional comments: This caters for Send Side Streaming too.
 *********************************************************************************/
void linux_pci_ww_send_descriptor_table_from_strm_buffers(void *rcvPtr)
{
#ifndef LFS
  int                                         ret;
#endif
  pmercd_adapter_block_sT                     padapter;
  pmercd_ww_dev_info_sT                       pwwDev;
  pmercd_ww_dma_descr_sT                      pdmaDescr = 0;
  pmercd_ww_dma_descr_sT                      npdmaDescr = 0;
  pmercd_ww_dma_descr_sT                      nnpdmaDescr;
  merc_ulong_t                                size;
#ifndef LFS
  merc_uint_t                                 count;
#endif
  PSTRM_MSG                                   FirstMblk;
  PSTRM_MSG                                   Msg;
  PSTRM_MSG                                   nMsg;
  merc_ulong_t                                datalen;
  merc_uchar_t                                *start, *end ;
  pmercd_dhal_ww_snd_stream_mblk_sT           pdir_copy;
#ifndef LFS
  merc_ulong_t                                syncOffset;
  merc_ulong_t                                syncSize;
  pmercd_stream_connection_sT                 StreamBlock;
  pmerc_uchar_t                               vaddress;
#endif

  PMDRV_MSG                                   MdMsg;
  PSTREAM_SEND                                pStrmSend;
  PUSER_HEADER                                pUsrHdr;

  pdir_copy = (pmercd_dhal_ww_snd_stream_mblk_sT)rcvPtr;

  pdir_copy->ret = MD_SUCCESS;

  //Get WW struct and Adapter struct
  pwwDev = pdir_copy->pwwDevi;
  padapter = pwwDev->padapter;
 
  //Get the Msg pointer
  FirstMblk = (PSTRM_MSG)pdir_copy->mb;

  //If this is a BStream, obtain a descriptor for 24byte StreamUserHeader
  if (pdir_copy->purpose == MERCD_WW_BSTRM_WRITE_REQUEST) {
    pdmaDescr = pdir_copy->pdmaDescr;


    //printk("MERCD_WW_BSTRM_WRITE_REQUEST:..Alloc DMA Descriptor..\n");
    //printk("pdmaDescr: first one: 0x%x\n", pdmaDescr);

    //For BSTREAM, there going to be 2 DMA descriptors
    //@this index

    MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(FirstMblk);
    pStrmSend = (PSTREAM_SEND)MD_GET_MDMSG_PAYLOAD(MdMsg);
    pUsrHdr = &(pStrmSend->StreamUserHeader);

#if 0
    //////////////////////////////////////////////////////////////////////
    //The following is useful for Debug: This dumps the 24 byte UserHeader
    //which is part of STREAM_SEND.
    //////////////////////////////////////////////////////////////////////
    {
      pmerc_uchar_t ptr;
      merc_uchar_t flag;

      ptr = (pmerc_uchar_t)(pUsrHdr);
      flag = *((pmerc_uchar_t)(ptr+4));
      if (flag)
        {
          //printk("StreamSend: flag non zero: 0x%x\n", flag);
          //i_bprintf(ptr, 24);
          //printk("StreamSend: End..\n");
        }
    }
    //////////////////////////////////////////////////////////////////////
#endif

    pdmaDescr->flag |= (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT);
    //Not the last descriptor at this index. There  is going to be
    //one more descriptor at this index for data.
    pdmaDescr->flag &= ~(MERCD_WW_DMA_DESCR_EBIT);
    pdmaDescr->host_address  = (pmerc_void_t)FirstMblk;
    pdmaDescr->board_address = (pmerc_void_t)
	    pci_map_single(padapter->pdevi, pUsrHdr, sizeof(USER_HEADER), PCI_DMA_TODEVICE);
    pdmaDescr->size = (merc_ushort_t)sizeof(USER_HEADER);
    pdmaDescr->next = (pmercd_ww_dma_descr_sT)
		      MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(sizeof(mercd_ww_dma_descr_sT));
	    	      //mercd_allocator(sizeof(mercd_ww_dma_descr_sT));
    if (pdmaDescr->next == NULL) {
      printk("send_descriptor_table_from_strm_buffers pdmaDescr->next Alloc Failed\n");
      pdir_copy->ret = MD_FAILURE;
      return;
    }

    MSD_ZERO_MEMORY(pdmaDescr->next, sizeof(mercd_ww_dma_descr_sT));

    //printk("pdmaDescr->next: 0x%x\n", pdmaDescr->next);

    pdmaDescr->next->flag = 0;
    pdmaDescr->next->next = NULL;

    //We need to fill the descriptor for datablock now
    npdmaDescr = pdmaDescr->next;
    Msg = nMsg = (PSTRM_MSG)FirstMblk->b_cont;
    if (Msg == NULL) {
        printk("send_descriptor_table_from_strm_buffers\
               FirstMblk->b_cont is NULL\n");
        MSD_ERR_DBGPRINT("send_descriptor_table_from_strm_buffers\
               FirstMblk->b_cont is NULL\n");
        pdir_copy->ret = MD_FAILURE;
	return;
    }
  } else {
    //For GStream and Big Messages We come Here
    //Get the bigmsg descriptor at the index 'dmaDescIndex'
    npdmaDescr = pdmaDescr = pdir_copy->pdmaDescr;
    //Get the Data mblks
    Msg = nMsg = (PSTRM_MSG)FirstMblk->b_cont;
  }

  //This Msg mblk train has (the number of mblks == numBlks) linked through
  //b_cont. So atleast we need numBlks of descriptors at this index. But, If
  //an mblk's wp crosses a page boundary then we allocate an extra
  //descriptor and link it with the rest of the descriptors through
  //the descriptors next pointer
  while (Msg) {
   //Find the size of the mblk
   datalen = ((Msg)->b_wptr - (Msg)->b_rptr);

   if (!datalen) {
       //Possible for 0 byte messages - allow for this
       npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_SBIT | 
                            MERCD_WW_DMA_DESCR_VBIT | 
                            MERCD_WW_DMA_DESCR_EBIT);
   }

   //Determine how many DMA descriptors this mblk needs. Allocate additional
   //descriptors if we cross a page boundary
   for (start = Msg->b_rptr ; start < Msg->b_wptr ; ) {

        //Check if allocation is going over PAGE_SIZE
        if (((size_t)(start)| (PAGE_SIZE - 1)) > (size_t) (Msg->b_wptr - 1)) {
           end = Msg->b_wptr - 1;
        } else {
           npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_PBIT);
           end = (merc_uchar_t *)((size_t) start | (PAGE_SIZE - 1));
        }

        size = end - start + 1 ;

        // Set the rest of the flags
        // flag = 0x xxxp ecsv
        // p = will tell us if we are crossing a page boundary
        // e = will tell us if this is the last descriptor at this index
        // c = will tell us if (we cross a page boundary) or (we are on a b_cont link)
        // s = will tell if the origin of the BIG msg blk
        //   descriptor from streams or DMAble memory generated my kmalloc
        // v = will tell us who is the owner. Board or the Host. If the owner is the
        //   host that can be replaced with a new mblk at the time of replenishing

        // Lets just mark this as origined from Streams, and Host owns it. 0x xxxx xx11

        npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_SBIT | MERCD_WW_DMA_DESCR_VBIT);

        //If there is an mblk at b_cont or ( if we cross a page boundary too ???)
        //if ((Msg->b_cont) || (npbigmsg->flag & (MERCD_WW_DMA_DESCR_PBIT)))
        if (Msg->b_cont)
            npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_CBIT);

        //If this is the last descriptor at the specified index
        if (!(npdmaDescr->flag & (MERCD_WW_DMA_DESCR_PBIT)) && (!(Msg->b_cont)))
            npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_EBIT);

        if (size != 0) {
            npdmaDescr->host_address  = (pmerc_void_t)FirstMblk; /* just Msg, really dont care */
            npdmaDescr->board_address = (pmerc_void_t) 
		   pci_map_single(padapter->pdevi, start, size, PCI_DMA_TODEVICE);
            npdmaDescr->size = (merc_ushort_t)size;
            start = end + 1 ;

            if (npdmaDescr->flag & (MERCD_WW_DMA_DESCR_PBIT)) {
	        //check whether we need to add a new DMA descriptor
 	        if (start < Msg->b_wptr) {
                    npdmaDescr->next = (pmercd_ww_dma_descr_sT)
		       MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(sizeof(mercd_ww_dma_descr_sT));
                 
                    if (npdmaDescr->next == NULL) {
                        printk("snd_desc_table_frm_strm_buffers npdmaDescr->next Alloc Failed\n");
                        pdir_copy->ret = MD_FAILURE;
                        return;
                    }
                
                    npdmaDescr = npdmaDescr->next;
                    MSD_ZERO_MEMORY(npdmaDescr, sizeof(mercd_ww_dma_descr_sT));
                } else {
            	    //update the DMA flags within the DMA descriptor
                    npdmaDescr->flag &= ~(MERCD_WW_DMA_DESCR_PBIT);
                    npdmaDescr->flag |= (MERCD_WW_DMA_DESCR_EBIT);
                }
            }
        } /* size */
   } /* for start */


   //Go back to serve the next mblk on the b_cont chain, if exists
   //But, alloc one more descriptor if mblk on b_cont exists
   Msg = Msg->b_cont;
   if (Msg) {
       npdmaDescr->next = (pmercd_ww_dma_descr_sT)
	  MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(sizeof(mercd_ww_dma_descr_sT));
       if (npdmaDescr->next == NULL) {
           printk("send_descriptor_table_from_strm_buffers npdmaDescr->next Alloc Failed\n");
           pdir_copy->ret = MD_FAILURE;
           return;
       }

       npdmaDescr = npdmaDescr->next;
       MSD_ZERO_MEMORY(npdmaDescr, sizeof(mercd_ww_dma_descr_sT));
    }
  } /* while */

  //In case of MD_FAILURE do the clean up work before going for SCHEME2
  //Except the original descriptor, free every other descriptor on the
  //next chain.
  if (pdir_copy->ret == MD_FAILURE) {
      printk("send_descriptor_table from_strm_bufs..FAILS\n");
      npdmaDescr = pdmaDescr;
      while (npdmaDescr->next) {
#ifdef LFS
	 nnpdmaDescr = npdmaDescr;
#endif
         while (npdmaDescr->next) {
                nnpdmaDescr = npdmaDescr;
                npdmaDescr = npdmaDescr->next;
         }
         nnpdmaDescr->next = NULL;
         mercd_free(npdmaDescr, MERCD_WW_BIGMSG_DESCR, MERCD_FORCE);
         npdmaDescr = pdmaDescr;
      }
  }

  return;
}

/******************************************************************************
 * Function Name                : linux_pci_ww_dealloc_snd_bigmsg_descriptors
 * Function Type                : This Deallocs Mem for
 *                              : BIG Msg Blocks at a particular index of the
 *                              : table
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Need to take care of both Streams buffers
 *                              : and DMAble area replacing the stream buffers
 * Additional comments          :
 *    We leave the first descritpor at dmaDescIndex intact,
 *    But we free the DMA descriptors following the next link
 *    of the first descritptor at dmaDescIndex.
 *
 ****************************************************************************/
void linux_pci_ww_dealloc_snd_bigmsg_descriptors(void *rcvPtr)
{
 pmercd_ww_dev_info_sT               pwwDev;
 pmercd_ww_dma_descr_sT              pdmaDescr;
 pmercd_ww_dma_descr_sT              npdmaDescr;
 pmercd_ww_dma_descr_sT              nnpdmaDescr;
 pmercd_dhal_ww_free_desc_sT         pfreeDesc;
 pmercd_adapter_block_sT             padapter;
#ifndef LFS
 merc_ulong_t                        size;
 merc_uint_t                         count;
 merc_uint_t                         ret;
 PSTRM_MSG                           Msg;
 mblk_t                              *mb;
#endif
 merc_uint_t                         memType;
 merc_ushort_t                       index;
 pmercd_stream_connection_sT         StreamBlock;
 

 pfreeDesc = (pmercd_dhal_ww_free_desc_sT)(rcvPtr);
 pfreeDesc->ret = MD_SUCCESS;
 pwwDev = pfreeDesc->pwwDevi;
 index = pfreeDesc->index;

 MSD_FUNCTION_TRACE("linux_pci_ww_dealloc_snd_bigmsg_descriptors", ONE_PARAMETER, 
                     (size_t)rcvPtr);

 padapter = pwwDev->padapter;

 if (pfreeDesc->purpose == MERCD_WW_STRM_WRITE_COMPLETE) {
    StreamBlock = (pmercd_stream_connection_sT)pfreeDesc->StreamBlock;
    npdmaDescr = pdmaDescr = StreamBlock->pSendStrmDmaDesc[index];
   }
 else { /* Big Msg Read Ack */
    npdmaDescr = pdmaDescr = pwwDev->BigMsgSndMemStr.pbigMsgDescr[index];
 }

 if (pdmaDescr == NULL)
  {
  printk("pdmaDescr is NULL...\n");
  MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
  padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
  MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
  pfreeDesc->ret = MD_FAILURE;
  return;
  }

 //if at index --->|0|->|1|->|2|->|3|
 //there are 4 descriptors as shown above, then we deallocate
 //1, 2, 3 and leave the descriptor 0, with its valid bit off

 if (pdmaDescr->flag & MERCD_WW_DMA_DESCR_VBIT)
 {
   if (pdmaDescr->flag & MERCD_WW_DMA_DESCR_SBIT)
       memType = streammem;
   else
       memType = dmakmem;
 }
 else
 { /* VBIT is off, Should never happen */
  pdmaDescr->flag = 0;
  pdmaDescr->board_address = 0;
  pdmaDescr->host_address = 0;
  pdmaDescr->size = 0;
  //printk("Adapter out: vbit off\n");
  //padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
  pfreeDesc->ret = MD_FAILURE;
  return;
 }

  switch (memType)
   {
     case streammem:

      //Validate the v-bit for all the descriptors
      while(npdmaDescr)
      {
        //Check the valid bit before deallocating
       if (!((npdmaDescr->flag) & (MERCD_WW_DMA_DESCR_VBIT)))
        {
#if 0
          printk("Adapter out: vbit not set\n");
          MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
          padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
          MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
          pfreeDesc->ret = MD_FAILURE;
          return;
#endif
        }
        npdmaDescr->flag = 0;
        npdmaDescr = npdmaDescr->next;
      } /* while */

      //Now free all the descritptors except the
      //first one

      npdmaDescr = pdmaDescr;

      while (npdmaDescr->next) {
#ifdef LFS
	 nnpdmaDescr = npdmaDescr;
#endif
         while (npdmaDescr->next) {
                nnpdmaDescr = npdmaDescr;
                npdmaDescr = npdmaDescr->next;
         }
	 
         //Do the pci unmap before freeing
	 if (pfreeDesc->purpose == MERCD_WW_STRM_WRITE_COMPLETE) {
	     pci_unmap_single(padapter->pdevi, (dma_addr_t)npdmaDescr->board_address,
			                         npdmaDescr->size, PCI_DMA_TODEVICE);
             npdmaDescr->host_address = 0;
          }

          nnpdmaDescr->next = NULL;
          mercd_free(npdmaDescr, MERCD_WW_BIGMSG_DESCR, MERCD_FORCE);
          npdmaDescr = pdmaDescr;
      }

      if (npdmaDescr != pdmaDescr)
          printk("Problem in dealloc the nexts\n");

      //Now we are left with one descriptor at index, which is the
      //first descriptor at that index.
      //Here in the first descriptor's host_address field we stored
      //the Streams FirstMblk we got from OS.( This is irrespecive
      //of whether we DMAd the Streams mblk directly to board or
      //copied it to DMAble memory allocd by ddi_dma_mem_alloc. So,
      //Free the Streams Msg we got from upper layers.

      //Do the pci unmap before freeing
      if (pfreeDesc->purpose == MERCD_WW_STRM_WRITE_COMPLETE) {
	  pci_unmap_single(padapter->pdevi,
			   (dma_addr_t)pdmaDescr->board_address,
			   pdmaDescr->size,
			   PCI_DMA_TODEVICE);
      }
      if (pdmaDescr->host_address)
         MSD_FREE_MESSAGE(pdmaDescr->host_address);
      break;

     case dmakmem:

           //WWPENDING: SCHEME2 in Future
           break;

      default:
           break;

   }
  //Clear the flag and other fields of the descriptor
   pdmaDescr->flag = 0;
   pdmaDescr->board_address = 0;
   pdmaDescr->host_address = 0;
   pdmaDescr->size = 0;

 //We are done
 return;
}

/********************************************************************************
 * Function Name                : linux_pci_ww_recv_replenish_descriptor_buffers
 *                              :
 * Function Type                : For the Rcv Big Msg Blocks, this will replenish
 *                              : the memory at a particular index of the Rcv
 *                              : Big Msg descritptor table.
 *                              : For Receive Streaming, this will relenish the
 *                              : the memory at a particular index in the per
 *                              : StreamBlock descriptor table.
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          : 
 *                                This is called after the big message/rceive 
 *                                streams buffers have been passed up to the 
 *                                libraries. This will invalidate a descriptor 
 *                                first, try to alloc a new stream mblk at this 
 *                                DMA descriptor index.
 *********************************************************************************/
void linux_pci_ww_recv_replenish_descriptor_buffers(void *rcvPtr)
{
 MD_STATUS                                    Status;
#ifdef LFS
 pmercd_ww_dev_info_sT                        pwwDev = NULL;
#else
 pmercd_ww_dev_info_sT                        pwwDev;
#endif
#ifdef LFS
 pmercd_ww_dma_descr_sT                      pdmadescr = NULL;
#else
 pmercd_ww_dma_descr_sT                      pdmadescr;
#endif
 merc_uint_t                            dmaDescIndex;
#ifndef LFS
 PSTRM_MSG                                       Msg;
#endif
 pmercd_dhal_ww_rcv_rep_bigmsgdes_sT       pRepDescr;
 mercd_dhal_ww_rcv_copybigmsg_str_sT  dir_copy = {0};
#ifndef LFS
 mercd_dhal_ww_rcv_copybigmsg_kmem_sT mb_alloc = {0};
#endif
 merc_uint_t                                 purpose;
#ifdef LFS
 pmercd_stream_connection_sT             StreamBlock = NULL;
#else
 pmercd_stream_connection_sT             StreamBlock;
#endif


 MSD_FUNCTION_TRACE("linux_pci_ww_recv_replenish_descriptor_buffers", ONE_PARAMETER, 
                     (size_t)rcvPtr);

 pRepDescr = (pmercd_dhal_ww_rcv_rep_bigmsgdes_sT)rcvPtr;
 pRepDescr->ret = MD_SUCCESS;

 dmaDescIndex = pRepDescr->index;
 purpose = pRepDescr->purpose;
 //Get the BIG Msg Descriptor at dmaDescIndex
 if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_ALLOC) 
   {
    pwwDev = (pmercd_ww_dev_info_sT)pRepDescr->pwwDevi;
    pdmadescr = pwwDev->BigMsgRcvMemStr.pbigMsgDescr[dmaDescIndex];
   }
 else
   if (purpose == MERCD_WW_STRMDATA_RCV_DESCR_ALLOC)
    {
     StreamBlock = (pmercd_stream_connection_sT)pRepDescr->pwwDevi;
     pdmadescr = StreamBlock->pRcvStrmDmaDesc[dmaDescIndex];
    }
   else
    printk("linux_pci_ww_recv_replenish_descriptor_buffers: UNKNOWN PURPOSE\n");

 //We need to turn off v bit, because, we
 //would have just send a streams buffer up to
 //the library at this index, and at the same time
 //we  might try freeing the streams mblk at this index
 //as a part of shutdown_adapter process.

 pdmadescr->flag &= ~(MERCD_WW_DMA_DESCR_VBIT);

  if ((pdmadescr->flag) & (MERCD_WW_DMA_DESCR_SBIT))
  { /* Streams Memory */
   //NOTE: we do not need to free the Streams memory
   //at this index, as these original mblks are sent
   //up to the application. OS will free them at its
   //will.
  }
 else
 {
   //Now, if it is a DMAble memory at this
   //index, then we need to explicitly
   //deallocate it. While sending the data
   //we copied the data from this memory to
   //a new mblk. So this memory was never
   //released.
   //SCHEME2 in Future
 }

 //Finally, overwrite the flag and others
 pdmadescr->flag          = 0;
 pdmadescr->board_address = 0;
 pdmadescr->host_address  = 0;
 pdmadescr->size          = 0;

 //Now allocate new memory at this index
 if (purpose == MERCD_WW_BIGMSG_RCV_DESCR_ALLOC)
  {
	 dir_copy.pwwDevi = pwwDev;
	 dir_copy.purpose = purpose;
	 dir_copy.index = dmaDescIndex;

	 (*mercd_dhal_func[MERCD_DHAL_WW_RCV_BIGMSG_DIRCOPY_FROM_KERNEL])\
                                                    ((void *)&(dir_copy));
	 Status = dir_copy.ret;

	 //SCHEME2 : Looks like SCHEME1 is not successful, so apply SCHEME2 for thos
	 //BIG blk descriptor for which SCHEME1 is failed

	 if(Status == MD_FAILURE)
	 {
	   //mb_alloc.pwwDevi = pwwDev;
	   //mb_alloc.resource = (pmerc_void_t)&(pwwDev->BigMsgRcvMemStr);
	   //mb_alloc.index = dmaDescIndex;
	   //mb_alloc.purpose = MERCD_WW_BIGMSG_RCV_DESCR_ALLOC;
	   //(*mercd_dhal_func[MERCD_DHAL_WW_RCV_COPY_FROM_DMAABLE_BUFFERS])
           //((pmerc_void_t)&mb_alloc);
	   //Status = mb_alloc.ret;
	   if (Status != MD_SUCCESS) {
	   printk("mid_wwmgr_allocate_mem_for_bigmsgblks: RCV MSGBLK Mem Alloc Failed\n");
	    }
	 }
         return;
 }
 else
 {
   if (StreamBlock->WWRcvStrDataBufPostMethod == MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED)
    {
	 return;
    }
   else
    { //MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE
  	dir_copy.pwwDevi = (pmercd_ww_dev_info_sT)StreamBlock;
        dir_copy.purpose = purpose; /* MERCD_WW_STRMDATA_RCV_DESCR_ALLOC */
        dir_copy.index = dmaDescIndex;
        (*mercd_dhal_func[MERCD_DHAL_WW_RCV_STREAM_DIRCOPY_FROM_KERNEL])\
                                                    ((void *)&(dir_copy));
        Status = dir_copy.ret;

        if (Status != MD_SUCCESS) {
         //////////////////////////////////////////////////////
         //SCHEME2: not implemented yet
         //////////////////////////////////////////////////////
        } 
        return;
    }
  } /* purpose */

return;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_mf_mem_dealloc
 * Function Type                : This Deallocs Mem for MFs and MSG Blks
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_ww_mf_mem_dealloc(void *rcvPtr)
{

 pmercd_ww_dev_info_sT          pwwDev;
 pmercd_adapter_block_sT      padapter;

#ifndef LFS
 merc_uint_t ret;
#endif

 MSD_FUNCTION_TRACE("linux_pci_ww_mf_mem_dealloc", ONE_PARAMETER, 
                     (size_t)rcvPtr);

 pwwDev = (pmercd_ww_dev_info_sT)rcvPtr;
 padapter = pwwDev->padapter;

  if (pwwDev->mfaBaseAddressDescr.start_address)
#ifdef LFS
      free_pages((ulong)pwwDev->mfaBaseAddressDescr.start_address, pwwDev->mfaBaseAddressDescr.order);
#else
      free_pages(pwwDev->mfaBaseAddressDescr.start_address, pwwDev->mfaBaseAddressDescr.order);
#endif

  //Nothing to check to see if linux_free_mem is successful.

  //Zero out some critical fields..just in case..
  //You are going to dealloc pwwDev itself anyway
  pwwDev->mfaBaseAddressDescr.board_address = 0;
  pwwDev->mfaBaseAddressDescr.host_address = 0;
  pwwDev->mfaBaseAddressDescr.start_address = 0;
  pwwDev->mfaBaseAddressDescr.size= 0;
  pwwDev->inBoundQMfAddress = 0;
  pwwDev->outBoundQMfAddress = 0;

return;
}
/***************************************************************************
 * Function Name              : linux_pci_ww_dealloc_descriptor_table
 * Function Type              : This Deallocs Mem for
 *                            : BIG Msg Blocks
 * Inputs                     :
 * Outputs                    :
 * Calling functions          :
 * Description                : Need to take care of both Streams buffers
 *                            : and DMAble area replacing the stream buffers
 *                            :
 * Additional comments        : Caters to Both Send and Rcv sides
 ****************************************************************************/
void linux_pci_ww_dealloc_descriptor_table(void *rcvPtr)
{
 pmercd_ww_dev_info_sT                pwwDev;
 pmercd_ww_dma_descr_sT               pdmaDescr, npdmaDescr, nnpdmaDescr;
 pmercd_ww_dma_descr_sT               *resource;
 pmercd_adapter_block_sT              padapter;
#ifndef LFS
 merc_ulong_t                         size;
#endif
 merc_ulong_t                         numBigMsgs, numDescr;
 merc_ulong_t                         szMsg;
 merc_uint_t                          count;
#ifndef LFS
 merc_uint_t                          ret;
 PSTRM_MSG                            Msg;
 mblk_t                               *mb;
#endif
 mercd_dhal_ww_dealloc_dmadescr_sT   *descrfree;
 merc_uint_t                           purpose;
 pmercd_stream_connection_sT           StreamBlock;

 MSD_FUNCTION_TRACE("linux_pci_ww_dealloc_bigmsg_descriptor_table 0x%x", (size_t)rcvPtr);
 
    descrfree = ((mercd_dhal_ww_dealloc_dmadescr_sT *)(rcvPtr));
    pwwDev = descrfree->pwwDevi;
    resource = descrfree->resource;
    purpose = descrfree->purpose;

#ifdef LFS
    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;
#endif
 
    if (resource == NULL) {
        return; /* This should never happen */
    }
 
    //Get the Number and Size of Big Message Blocks
    if (purpose == MERCD_WW_STREAM_DESCR_DEALLOC) {
	StreamBlock = (pmercd_stream_connection_sT)descrfree->extradata;
        if (!StreamBlock)
            return;
	if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
            numDescr = StreamBlock->szRcvBitMap;
	else
            numDescr = StreamBlock->szSendBitMap;
	             
        szMsg = 0;
    } else {
	if ((pwwDev == NULL) || (pwwDev->pww_param == NULL)) {
	    printk("linux_pci_ww_dealloc: Null WW Adapter\n");
	    return;
	}
        numDescr = numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
        szMsg = pwwDev->pww_param->sizeofBigMsgBlocks;
    }
 
    for (count=0; count < numDescr; count++) {
         pdmaDescr = resource[count];

         if (!pdmaDescr)
             continue;
       
          if (purpose == MERCD_WW_STREAM_DESCR_DEALLOC) {
#ifdef LFS
	      StreamBlock = (pmercd_stream_connection_sT)descrfree->extradata;
#endif
	      if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY) {
		  npdmaDescr = pdmaDescr;

		  while (npdmaDescr->next) {
#ifdef LFS
	            nnpdmaDescr = npdmaDescr;
#endif
		    while (npdmaDescr->next) {
		      nnpdmaDescr = npdmaDescr;
		      npdmaDescr = npdmaDescr->next;
		    }

	   	    // New Phys Address
   		    // Do the pci unmap before freeing
		    if (npdmaDescr->board_address)
		        pci_unmap_single(padapter->pdevi, 
					(dma_addr_t)npdmaDescr->board_address,
				         npdmaDescr->size, PCI_DMA_FROMDEVICE);

	    	    nnpdmaDescr->next = NULL;
		    if (npdmaDescr)
		        mercd_free(npdmaDescr, MERCD_WW_BIGMSG_DESCR, MERCD_FORCE);
    		    npdmaDescr = pdmaDescr;
		  }

		  if (npdmaDescr != pdmaDescr)
		      printk("QCNTRL_SUCCEEDED:Problem in dealloc the nexts\n");

	          if (pdmaDescr->board_address) {
	              pci_unmap_single(padapter->pdevi, 
				      (dma_addr_t)pdmaDescr->board_address,
				       pdmaDescr->size, PCI_DMA_FROMDEVICE);
		  }

	      } /* STREAM_OPEN_F_RECEIVE_ONLY */
	      else /* Write StreamBlock */
		  if (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) {
		      npdmaDescr = pdmaDescr;
#ifdef LFS
	              nnpdmaDescr = npdmaDescr;
#endif
		      while (npdmaDescr->next) {
			while (npdmaDescr->next) {
		          nnpdmaDescr = npdmaDescr;
			  npdmaDescr = npdmaDescr->next;
		      	}
				
			// New Phys Address
			// Do the pci unmap before freeing
			if (npdmaDescr->board_address)
			    pci_unmap_single(padapter->pdevi, 
					    (dma_addr_t)npdmaDescr->board_address,
					     npdmaDescr->size, PCI_DMA_TODEVICE);

			nnpdmaDescr->next = NULL;
		        if (npdmaDescr)
			    mercd_free(npdmaDescr, MERCD_WW_BIGMSG_DESCR, MERCD_FORCE);
			npdmaDescr = pdmaDescr;
		      }	

		      if (npdmaDescr != pdmaDescr)
			  printk("QCNTRL_SUCCEEDED:Problem in dealloc the nexts\n");

		      if (pdmaDescr->board_address)
			  pci_unmap_single(padapter->pdevi, 
					  (dma_addr_t)pdmaDescr->board_address,
					   pdmaDescr->size, PCI_DMA_TODEVICE);
		  } /* StreamBlock read vs write */
	  } /* MERCD_WW_STREAM_DESCR_DEALLOC */
	  
	  if (pdmaDescr->host_address)
              MSD_FREE_MESSAGE(pdmaDescr->host_address);
	  
	  // Free the descriptor @ 'count' in the table
	  if (pdmaDescr)
              mercd_free(pdmaDescr, sizeof(mercd_ww_dma_descr_sT), MERCD_FORCE);
    }/* for */

    // We are done deallocating  all the descriptors
    // Lets deallocate the index table itself
    if (resource)
        mercd_free(resource, (numDescr * sizeof(pmercd_ww_dma_descr_sT)), MERCD_FORCE);
    resource = NULL;
    return;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_switch_adapter_mappings
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            :
 * Description                  : 
 *                              : plx mappings in SRAM mode are at
 *                              : padapter->phw_info->\
 *                              : virt_map_q[MERCD_PCI_BRIDGE_MAP].
 *                              : It replaces this with the new plx map 
 *                              : at pwwDev->plx_space_tree. It also preserves
 *                              : the SRAM specific Plx mappings.
 *
 *
 *
 *
 * Additional comments          :1.9 Boards Only
 ****************************************************************************/
void linux_pci_ww_switch_adapter_mappings(void *rcvPtr)
{
 pmercd_ww_dev_info_sT            pwwDev;
 pmercd_adapter_block_sT          padapter;
 pmercd_dhal_ww_switch_plxmap_sT  pswitchplx = (pmercd_dhal_ww_switch_plxmap_sT)rcvPtr;
 pmerc_void_t                     tmp;

 pwwDev = pswitchplx->pwwDevi;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

 MSD_FUNCTION_TRACE("linux_pci_ww_switch_adapter_mappings", ONE_PARAMETER,(size_t)rcvPtr);

 tmp = (pmerc_void_t)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]);
 
 //Store the new plx map in WW mode at MERCD_PCI_BRIDGE_MAP
 padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP] =
          padapter->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP];

 //Preserve the Original Plx Mappings in SRAM mode
 padapter->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP] = tmp;

 return;
}

/***************************************************************************
 * Function Name                : linux_pci_ww_map_adapter_plxparm
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            :
 * Description                  : Maps the PLX space in WW mode.
 *                              : This space is referenced by
 *                              : pwwDev->pww_plx_space_tree
 *                              :
 * Additional comments          : Caters to SRAM Family of Boards - Obsolete
 ****************************************************************************/

void linux_pci_ww_map_adapter_plxparm(void *rcvPtr)
{
#ifndef LFS
 int                                   ret;
#endif
 pmercd_dhal_ww_plxmap_sT              plxmap;
 pmercd_ww_dev_info_sT                 pwwDev;
 pmercd_virt_map_sT                    pvirt_map;
 pmercd_adapter_block_sT               padapter;
 mercd_osal_mem_phys_virt_sT           meminfo = { 0 } ;
 mercd_dev_info_T                      *Dip;
 merc_uint_t                           tmp_address=0;
 merc_uint_t                           tmp_address1=0;

 MSD_FUNCTION_TRACE("linux_pci_ww_map_adapter_plxparm", ONE_PARAMETER, 
                     (size_t)rcvPtr);

    plxmap= (pmercd_dhal_ww_plxmap_sT)rcvPtr;
    plxmap->ret = MD_SUCCESS;
    pwwDev = plxmap->pwwDevi;
    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;
    Dip = padapter->pdevi;

    pvirt_map = (pmercd_virt_map_sT)mercd_allocator(MERCD_VIRT_MAP);

    if (pvirt_map == NULL)
     {
      printk("linux_pci_ww_map_adapter_plxparm\
              pvirt_map alloc Failed\n");
      MSD_ERR_DBGPRINT("linux_pci_ww_map_adapter_plxparm\
              pvirt_map alloc Failed\n");
      plxmap->ret = MD_FAILURE;
      return;
     }

#ifdef LINUX
    pvirt_map->phys_addr = pci_resource_start(Dip, 3);
#else
    pvirt_map->phys_addr = Dip->base_address[3];
#endif
    //Some times LiS Pci calls are not masking out the first 4 bits
    if (pvirt_map->phys_addr  & 0x0000000f)
      {
         pvirt_map->phys_addr &= ~0x0000000f;
      }

     // To find out about the Size of the memory
     pci_read_config_dword(Dip, addresses[3], (u32 *)&tmp_address);
     pci_write_config_dword(Dip, addresses[3], ~0 );
     pci_read_config_dword(Dip, addresses[3], (u32 *)&tmp_address1);
     if( !tmp_address1 ){
        plxmap->ret = MD_FAILURE;
        printk("get_prop: Unable to read address[3] (tmp_address1 = %x) ", tmp_address1);
        return;
     }
     pvirt_map->byte_count = (size_t)((0xffffffff - tmp_address1)+1);

#if (LINUX_VERSION_CODE < 0x020348)
     pci_write_config_dword(Dip, addresses[3], Dip->base_address[3]);
#else
     pci_write_config_dword(Dip, addresses[3], pci_resource_start(Dip, 3));
#endif
     padapter->pww_info->pww_plx_space_tree = (pmerc_void_t)pvirt_map;

   //2. Map WW PLX SPACE to Virtual
  meminfo.PhysAddr = (merc_uint_t)((pmercd_virt_map_sT)(padapter->pww_info->pww_plx_space_tree))->phys_addr;
  meminfo.ByteCount = (merc_uint_t)((pmercd_virt_map_sT)(padapter->pww_info->pww_plx_space_tree))->byte_count;

  (*mercd_osal_func[MERCD_OSAL_MEM_PHYS_VIRT])((pmerc_void_t) &meminfo);


    if ( meminfo.ret == NULL)
      {
        MSD_ERR_DBGPRINT("linux_pci_mem_map: MsdPhysToVirt failed\n");
        printk("linux_pci_mem_map Phys to Virt failed..\n");
        plxmap->ret = MD_FAILURE;
        return;
      }

   ((pmercd_virt_map_sT)padapter->pww_info->pww_plx_space_tree)->virt_addr = meminfo.ret;

return;
}

/***************************************************************************
 * Function Name                : linux_ww_generate_soft_reset
 * Function Type                : DHAL & OSSHL functions
 * Inputs                       : pwwinfo
 * Outputs                      : none
 * Calling functions            :
 * Description                  : Do a soft reset on DMVB board
 *                              :
 * Additional comments          : Caters to DMVB Boards
 ****************************************************************************/
void linux_ww_generate_soft_reset(pmercd_adapter_block_sT padapter) {
#ifndef LFS
   mercd_ww_gen_doorbell_sT       soft_reset;
#endif
   merc_uint_t			  break_cnt = 0;
   merc_ulong_t                   vend_id_reg;
   merc_ulong_t                   bar0_address;
   merc_ulong_t                   i2o_status_reg;
   mercd_dhal_ww_switch_plxmap_sT switchplx = { 0 };
#ifndef LFS
   volatile pmerc_uchar_t         loc;
#endif


   if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
       (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {

#ifdef LFS
       bar0_address = (ulong)padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;
       vend_id_reg = readl((caddr_t)bar0_address+MD_PCI_VENID_LOCATION);
#else
       bar0_address = padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;
       vend_id_reg = readl(bar0_address+MD_PCI_VENID_LOCATION);
#endif

       // check for bar 3
       if ((vend_id_reg != MD_PCI_DLGCID) && (vend_id_reg != MD_PCI_NEWBID)) {
#ifdef LFS
           bar0_address = (ulong)padapter->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;
           vend_id_reg = readl((caddr_t)bar0_address+MD_PCI_VENID_LOCATION);
#else
           bar0_address = padapter->phw_info->virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr;
           vend_id_reg = readl(bar0_address+MD_PCI_VENID_LOCATION);
#endif
           if ((vend_id_reg != MD_PCI_DLGCID)&&(vend_id_reg != MD_PCI_NEWBID)) {
               printk("Unable to read board memory - Board out of service\n");
               padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
               return;
           }
       }

#ifdef LFS
       i2o_status_reg = readl((caddr_t)bar0_address+MSD_WW_PCI_I20_STATUS_REGISTER);
#else
       i2o_status_reg = readl(bar0_address+MSD_WW_PCI_I20_STATUS_REGISTER);
#endif

       if (i2o_status_reg & 0x01) {
           MsdWWPlxGenerateDoorBell(padapter, MSD_WW_RESET_INTERRUPT_TO_BOARD);

           switchplx.pwwDevi = padapter->pww_info;
           switchplx.ConfigId = padapter->adapternumber;
           (*mercd_dhal_func[MERCD_DHAL_WW_SWITCH_PLXMAP])((pmerc_void_t)&switchplx);

	   // allow for reset_ack interrupt to occur
	   while (break_cnt < 10) {
              break_cnt++;
              set_current_state(TASK_INTERRUPTIBLE);
              schedule_timeout((500*HZ)/1000);
              set_current_state(TASK_RUNNING);
	      
	      // break when reset_ack processed else wait for 5 seconds max
              if (padapter->pww_info->state == MERCD_ADAPTER_WW_IN_SHUTDOWN)
	          break;
           }
       }

       // todo: verify if board actually switched
   }

}
