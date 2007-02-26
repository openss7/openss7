/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_linux_pci_dhal.c
 * Description                  : DM3 Hardware Abstraction Layer
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MERCD_DHAL_C
#include "msdextern.h"
#undef _MERCD_DHAL_C

/*
 * DHAL Functional Definition for 
 * OS Specific Hardware Layer to Core DM3 Driver Architecture Layer 
 * OS Specific Hardware Layer to OS Specific Driver Layer
 * vice versa.
 */

int initialize_linux_pci_dhal()
{
        MSD_FUNCTION_TRACE("initialize_linux_pci_dhal", NO_PARAMETERS);

	mercd_dhal_func[MERCD_DHAL_DRVR_VERIFY] = &linux_pci_verify_merc_adapter;
	mercd_dhal_func[MERCD_DHAL_DRVR_IDENTIFY] = &linux_pci_drvr_identify;
	mercd_dhal_func[MERCD_DHAL_DRVR_SPECIFIC] = &mercd_return_success;

	mercd_dhal_func[MERCD_DHAL_DRVR_FREE] = &linux_pci_drvr_free;
	mercd_dhal_func[MERCD_DHAL_DRVR_PROBE] = &linux_pci_drvr_probe;

	mercd_dhal_func[MERCD_DHAL_MEM_COPY] = &linux_pci_device_copy;
	mercd_dhal_func[MERCD_DHAL_MEM_PEEK] = &mercd_return_success;

	mercd_dhal_func[MERCD_DHAL_BRD_GETPROP] = &linux_pci_get_prop;
	mercd_dhal_func[MERCD_DHAL_BRD_MAP_ATTR] = &linux_pci_map_adapter_parm;
	mercd_dhal_func[MERCD_DHAL_BRD_UNMAP_ATTR] = &linux_pci_unmap_adapter_parm;
	mercd_dhal_func[MERCD_DHAL_BRD_FILL_ATTR] = &mercd_return_success;
	mercd_dhal_func[MERCD_DHAL_BRD_SAVE_ATTR] = &linux_pci_save_attr;
	mercd_dhal_func[MERCD_DHAL_BRD_CLEAN_ATTR] = &mercd_return_success;
	mercd_dhal_func[MERCD_DHAL_BRD_GETINUMBER] = &mercd_return_success;
	mercd_dhal_func[MERCD_DHAL_BRD_SCAN] = &mercd_return_success;

	mercd_dhal_func[MERCD_DHAL_INTR_CHECK] = &linux_pci_intr_check;
	mercd_dhal_func[MERCD_DHAL_INTR_ENABLE] = &linux_pci_intr_enable;
	mercd_dhal_func[MERCD_DHAL_INTR_DISABLE] = &linux_pci_intr_disable;

        //////////SRAMDMA BEGIN///////////////////////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
        mercd_dhal_func[MERCD_DHAL_DMA_ALLOC_HANDLE] = &linux_pci_dma_alloc_handle;
        mercd_dhal_func[MERCD_DHAL_DMA_FREE_HANDLE] = &linux_pci_dma_free_handle;
        mercd_dhal_func[MERCD_DHAL_PREPARE_DMA] = &linux_pci_prepare_dma;
#endif
        //////////SRAMDMA END///////////////////////////////////////////

        //WW support
        mercd_dhal_func[MERCD_DHAL_WW_MF_ALLOC] = &linux_pci_ww_mf_mem_alloc;
        mercd_dhal_func[MERCD_DHAL_WW_MF_DEALLOC] = &linux_pci_ww_mf_mem_dealloc;
        //mercd_dhal_func[MERCD_DHAL_WW_RCV_COPY_FROM_DMAABLE_BUFFERS] = &linux_pci_ww_recv_descriptor_table_from_dmaable_buffers
        mercd_dhal_func[MERCD_DHAL_WW_BRD_MAP_PLXATTR] = &linux_pci_ww_map_adapter_plxparm;
        mercd_dhal_func[MERCD_DHAL_WW_INTR_CHECK] = &linux_pci_ww_intr_check;
        mercd_dhal_func[MERCD_DHAL_WW_GET_INTR_REQUEST] = &linux_pci_ww_get_intr_request;
        mercd_dhal_func[MERCD_DHAL_WW_CLEAR_INTR_REQUEST] = &linux_pci_ww_clear_intr_request;
        mercd_dhal_func[MERCD_DHAL_WW_RCV_BIGMSG_DIRCOPY_FROM_KERNEL] = &linux_pci_ww_recv_bigmsg_descriptor_table_from_strm_buffers;
        mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE] = &linux_pci_ww_dealloc_descriptor_table;
        mercd_dhal_func[MERCD_DHAL_WW_SWITCH_PLXMAP] = &linux_pci_ww_switch_adapter_mappings;
        //mercd_dhal_func[MERCD_DHAL_WW_BRD_UNMAP_ATTR] = &linux_pci_ww_unmap_adapter_parm;
        //mercd_dhal_func[MERCD_DHAL_WW_SND_COPY_FROM_DMAABLE_BUFFERS] = &linux_pci_ww_send_descriptor_table_from_dmaable_buffers;
        mercd_dhal_func[MERCD_DHAL_WW_SND_DIRCOPY_FROM_KERNEL] = &linux_pci_ww_send_descriptor_table_from_strm_buffers;
        mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC] = &linux_pci_ww_dealloc_snd_bigmsg_descriptors;
        mercd_dhal_func[MERCD_DHAL_WW_REPLENISH_RCV_DESC] = &linux_pci_ww_recv_replenish_descriptor_buffers;
        mercd_dhal_func[MERCD_DHAL_WW_RCV_STREAM_DIRCOPY_FROM_KERNEL] = &linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers;



	return(0);

}

void mercd_return_success(void *rcvPtr)
{

	return;
}	
	


