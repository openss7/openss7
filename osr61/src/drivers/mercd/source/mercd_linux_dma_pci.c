/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_linux_dma_pci.c
 * Description                  : PCI DMA utilities
 *
 ****************************************************************************/

#include "msd.h"
#define MERCD_LINUX_DMA_PCI_C
#include "msdextern.h"
#undef MERCD_LINUX_DMA_PCI_C

#include "msdpciif.h"


/***************************************************************************
 * Function Name                : linux_pci_prepare_dma
 * Function Type                : DHAL & OSSHL Function
 * Inputs                       : rcvPtr
 * Outputs                      : none
 * Calling functions            : MERCD_DHAL_PREPARE_DMA Interfaces
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void linux_pci_prepare_dma(void *rcvPtr)
{
  pmercd_dhal_dma_sT        pdma_prep = (pmercd_dhal_dma_sT) rcvPtr;
  pmercd_dma_info_sT        pdmainfo;
  pmercd_adapter_block_sT   padapter;
  merc_uchar_t              *start, *end ;
  merc_int_t                len = 0;
  merc_int_t                size;
  mblk_t                    *mb;
  merc_uint_t               base_offset;
  merc_int_t                slotn;

  pdmainfo = pdma_prep->pdma;
  padapter = pdma_prep->padapter;

  padapter->flags.DPCDmaPending = 0;
   
  slotn = mercd_adapter_map[ padapter->adapternumber] ;

  mb = (mblk_t *)(pdma_prep->mb);

  //Now calculate the actual datablock address on the freenodelist
  //from which we are DMA ing into/from mblk. We know the offset into the 
  //datablock, so rest is pointer arithmatic

  base_offset = pdmainfo->dma_base_offset;
  // used to be base_offset = (MsdPlxGetPlxLocalBaseAddr(slotn)) + MSD_CONFIG_ROM_MAX_SIZE;
  // Now it is in msdbrdutl.c ..... 

  //Physical Address of the datablock
  pdma_prep->sramAddr += base_offset;             //;0xfff80101 base_offset;

  //Align the address !!!!!!!
  pdma_prep->sramAddr &= 0xFFFFFFFE;

  if(pdma_prep->dir == MSD_TO_HOST) 
   {
     
      //Check for exceeding the scatter-gather dma chains
      if((pdmainfo->rx_dma_count + 1) > pdmainfo->total_merc_rx_dma_chains)
       {
#ifdef LFS
         cmn_err(CE_CONT, 
                    "Exceeding the dma chain: 0x%lx\n", pdmainfo->rx_dma_count);
#else
         cmn_err(CE_CONT, 
                    "Exceeding the dma chain: 0x%x\n", pdmainfo->rx_dma_count);
#endif
         pdma_prep->ret = MD_FAILURE;
         return;
       }

      for (start = mb->b_rptr ; start < mb->b_wptr ; ) 
       {
         if (((size_t)(start)| (PAGE_SIZE - 1)) > (size_t) (mb->b_wptr - 1))
            end = mb->b_wptr - 1;
         else
            end = (merc_uchar_t *)((size_t) start | (PAGE_SIZE - 1));

         size = end - start + 1 ;
   
         //Save the mblk to give to putnext after DMA completion
         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].dma_mb = pdma_prep->mb;
         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].pdma_chain->hostAddr = 
                                               virt_to_phys(start);
         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].pdma_chain->xferSize =
                                                                 size;
         len+=size;
         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].size = len;

         //Store the physical address of the datablock in sgnode
         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].pdma_chain->sramAddr = 
                                        pdma_prep->sramAddr;

         pdmainfo->prxdma_res[pdmainfo->rx_dma_count].flags = MERC_PAGE_VALID;

         if(len < pdma_prep->size)
          {
           pdmainfo->prxdma_res[pdmainfo->rx_dma_count].flags |= MERC_PAGE_PASS;
          }
         else
          {
           pdmainfo->prxdma_res[pdmainfo->rx_dma_count].flags |= MERC_PAGE_END;
          }

          pdmainfo->prxdma_res[pdmainfo->rx_dma_count].pdma_chain->nextnode = 
#ifdef LFS
                  (((merc_uint_t)(long)(pdmainfo->phys_rx_chain_base) + 
                  ((pdmainfo->rx_dma_count+1) * (sizeof(mercd_dma_sgt_chain_node))))|(MSD_PCI_PLX_DMA_DESCRIPTOR_RD));
#else
                  (((merc_uint_t)(pdmainfo->phys_rx_chain_base) + 
                  ((pdmainfo->rx_dma_count+1) * (sizeof(mercd_dma_sgt_chain_node))))|(MSD_PCI_PLX_DMA_DESCRIPTOR_RD));
#endif
     
          //Move on to the next loc in datablock on board
          pdma_prep->sramAddr += len;
          pdmainfo->rx_dma_count++;
          pdma_prep->size -= len;
          start = end + 1;
     
       } /* for */

   } 
  else /* MSD_TO_SRAM */
   {
	
	 // This code is not tested to be supported in future releases ....... 
#ifndef LFS
     merc_uint_t       base_offset;
#endif

     if((pdmainfo->tx_dma_count + 1) > pdmainfo->total_merc_tx_dma_chains) {
#ifdef LFS
         cmn_err(CE_CONT,
                    "Exceeding the dma chain: 0x%lx\n", pdmainfo->tx_dma_count);
#else
         cmn_err(CE_CONT,
                    "Exceeding the dma chain: 0x%x\n", pdmainfo->tx_dma_count);
#endif
         pdma_prep->ret = MD_FAILURE;
         return;
     }

     for (start = mb->b_rptr ; start < mb->b_wptr ; )
      {
        if (((size_t)(start)| (PAGE_SIZE - 1)) > (size_t) (mb->b_wptr - 1))
           end = mb->b_wptr - 1;
        else
           end = (merc_uchar_t *)((size_t) start | (PAGE_SIZE - 1));

        size = end - start + 1 ;

        //Save the mblk to free after DMA completion
        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].dma_mb = pdma_prep->mb;
        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].pdma_chain->hostAddr =
                                              virt_to_phys(start);
        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].pdma_chain->xferSize =
                                                                 size;
        len+=size;
        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].size = len;

        //Store the physical address of the datablock in sgnode
        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].pdma_chain->sramAddr =
                                        pdma_prep->sramAddr;

        pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].flags = MERC_PAGE_VALID;

        if(len < pdma_prep->size)
         {
           pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].flags |= MERC_PAGE_PASS;
         }
        else
         {
          pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].flags |= MERC_PAGE_END;
         }

#ifdef LFS
         pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].pdma_chain->nextnode =
                  (((merc_uint_t)(long)(pdmainfo->phys_tx_chain_base) + 
                  ((pdmainfo->tx_dma_count+1) * (sizeof(mercd_dma_sgt_chain_node))))|(MSD_PCI_PLX_DMA_DESCRIPTOR_RD));
#else
         pdmainfo->ptxdma_res[pdmainfo->tx_dma_count].pdma_chain->nextnode =
                  (((merc_uint_t)(pdmainfo->phys_tx_chain_base) + 
                  ((pdmainfo->tx_dma_count+1) * (sizeof(mercd_dma_sgt_chain_node))))|(MSD_PCI_PLX_DMA_DESCRIPTOR_RD));
#endif
    
         //Move on to the next loc in datablock on board
         pdma_prep->sramAddr += len;
         pdmainfo->tx_dma_count++;
         pdma_prep->size -= len;
         start = end + 1;
      } /* for */

   } /* if pdma_prep->dir */

  
   //if you need to sync cache do a sync here
   //linux 2.4 onwards we have sync premitives
   //Solaris Sparc ddi_dma_sync.....

   pdma_prep->ret = MD_SUCCESS;

   return;
}
