/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdpciif.h
 * Description                  : pci interface definitions
 *
 *
 **********************************************************************/

#ifndef _2_msdpciif_h_H
#define _2_msdpciif_h_H

#ifndef lint
#ifdef LFS
#ident "@(#) %filespec: msdpciif.h-12 %  (%full_filespec: msdpciif.h-12:incl:hsw#3 %) "
#else
static char    *_2_msdpciif_h = "@(#) %filespec: msdpciif.h-12 %  (%full_filespec: msdpciif.h-12:incl:hsw#3 %) ";
#endif
#endif

// DM3 Board PCI Bus and Hardware Interface definitions

/////////////////////////////////////////////////////////////////////
// Run time registers on PCI PLX 9060 Chip
/////////////////////////////////////////////////////////////////////

#define MSD_PCI_PLX_INTR_REG            0x68            // Intr Status & Control Reg
#define MSD_PCI_PLX_INTR_ENABLE         0x00010900      // Intr Enable
#define MSD_PCI_PLX_INTR_ABORT          0x00004000      // PCI Abort Intr
#define MSD_PCI_PLX_INTR_LOCAL          0x00008000      // Local Intr
#define MSD_PCI_PLX_INTR_DOORBELL       0x00100000      // Local Doorbell active Intr
#define MSD_PCI_PLX_INTR_DMA            0x00400000      // DMA Intr
#define MSD_PCI_PLX_INTR_BIST           0x00800000      // BIST Intr
#define MSD_PCI_PLX_INTR_RETRY_OVER     0x08000000      // Retries Exhausted for Master

// HOT SWAP Control Reg for Plx ( in PCI config space)
#define MSD_PCI_PLX_ENUM_ASSERT        0xC0

// Pacific Chip Support
#define MSD_PCI_PACIFIC_INTR_ENABLE     0x00000900      // Intr Enable

// Pacific Chip requires bit 16 of the INT_CSR to be disabled
#define MSD_PCI_PACIFIC_INT_CSR_DMA0_ENABLE_W   0x000C0900
#define MSD_PCI_PACIFIC_INT_CSR_DMA1_ENABLE_W   0x000C0900

// Pacific Chip needs to set the READY# to arm the ASIC differently than PLX
#define MSD_PCI_PACIFIC_DMA_MODE_CHAINING           0x7C3
#define MSD_PCI_PACIFIC_DMA_MODE_NON_CHAINING       0x104C3


//////////////////////////////////////////////////////////////////////
// WW Support
// Runtime Registers for 9054
//////////////////////////////////////////////////////////////////////
#define MERCD_WW_PLX_REMAP_REG         0x28
#define MERCD_WW_PLX_H2B_INTR_REG      0x60
#define MERCD_WW_PLX_B2H_INTR_REG      0x64
#define MERCD_WW_PLX_BIGEND_REG        0x0C
//Pci to Local Doorbell active
#define MSD_PCI_INTR_DOORBELL_FROM_PLX       0x00002000
//BigEnd Register Mask
#define MERCD_WW_BIGEND_REGISTER_BITS 0xC0
/////////////////////////////////////////////////////////////
//WW Support
//Runtime Registers for 21554. These are all 16Bit registers
//Registers are:
// 1. Host to Board Intr Register.
// 2. Board to Host Intr Register. (We can determine if the interrupt
//    is ours, if so, what kind of interrupt it is.
// 3. A separate register for clearing the interrupt from Board.
// 4. A register for generating a soft reset of the board. This
//    register resides with in the config space.
/////////////////////////////////////////////////////////////
#define MSD_PCI_21554_H2B_INTR_REG                0x9E
#define MSD_PCI_21554_B2H_INTR_REG                0x9C
#define MSD_PCI_21554_B2H_INTR_CLEAR_REG          0x98
#define MSD_PCI_21554_SOFT_RESET_REG              0xD8
// A constant time for leaving the reset pin high on 21554
#define RESET_ACTIVE_TIME_ON_21554                0x0A
//Pci to Local Doorbell active, Basically we check for a Non-NULL
#define MSD_PCI_INTR_DOORBELL_FROM_21554          0xffff

#define MSD_PCI_BAR3_SIZE			  0x100
#define MSD_WW_RESET_INTERRUPT_TO_BOARD           0x8
#define MSD_WW_PCI_I20_STATUS_REGISTER            0xE8

//HOT SWAP Control Reg for 21554 ( in PCI config space)
#define MSD_PCI_21554_ENUM_ASSERT 0xC0
/////////////////////////////////////////////////////////////////////
//WW Support
//Runtime Registers for 80321:
/////////////////////////////////////////////////////////////////////
#define MSD_PCI_80321_H2B_INTR_REG                 0x20
#define MSD_PCI_80321_B2H_INTR_REG                 0x2C
//80321 interrupt control and status: Local to Pci
#define MSD_PCI_80321_B2H_INTR_CS_REG              0x30
//Pci to Local doorbell active intr
#define MSD_PCI_INTR_DOORBELL_FROM_80321           0x00000004//Local to Pci doorbell active

////////////////////////////////////////////////////////////////////////
//THIRD_ROCK Post Location and the contents
//21554: We use the scratchpad register 0, for post location
//80321:
////////////////////////////////////////////////////////////////////////
#define MERCD_3RDROCK_21554_POST_LOC          0xA8
#define MERCD_3RDROCK_80321_POST_LOC          0x18

////////////////////////////////////////////////////////////////////////
//THIRD_ROCK Crash Dump Location
////////////////////////////////////////////////////////////////////////
#define MSD_PCI_THIRD_ROCK_CRASH_DUMP_LOC       0x800
#define MSD_PCI_THIRD_ROCK_CRASH_DUMP_SIZE      0x100
                // retries to a Target

///////////////////////////////////////////////////////////////////////
//InBoundQ Port/OutBound Q Port Register offsets are the same
//for Plx9054, 21554, 80321
///////////////////////////////////////////////////////////////////////
#define MERCD_WW_INBOUND_QUEUE_REGISTER_ADDRESS   0x40
#define MERCD_WW_OUTBOUND_QUEUE_REGISTER_ADDRESS  0x44

///////////////////SRAMDMA BEGIN//////////////////////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
#define MIN_SEND_DMA_THR          60000
#define MIN_RECV_DMA_THR          24 

#define MSD_PCI_PLX_LOCAL_BASE    0x04

#define MSD_PCI_PLX_DMA0_CSR      0xA8
#define MSD_PCI_PLX_DMA1_CSR      0xA9

#define MSD_PCI_PLX_DMA_CSR_START_XFR_W        0x3
#define MSD_PCI_PLX_DMA_CSR_XFR_DONE_R         0x1000
#define MSD_PCI_PLX_DMA_CSR_CLR_INT_W          0x8
#define MSD_PCI_PLX_DMA_CSR_ABORT_XFER_W       0x400

// The following is moved to msdsram code
#define MSD_PCI_PLX_INT_CSR_DMA0_ENABLE_W      0x000D0900
#define MSD_PCI_PLX_INT_CSR_DMA1_ENABLE_W      0x000D0900

#define MSD_PCI_PLX_INT_CSR_DMA0_ACTIVE_R      0x00200000
#define MSD_PCI_PLX_INT_CSR_DMA1_ACTIVE_R      0x00400000

#define MSD_PCI_PLX_ANY_INT_ACTIVE \
        ( MSD_PCI_PLX_INT_CSR_LOCAL_ACTIVE_R |\
          MD_PCI_PLX_INT_CSR_DMA1_ACTIVE_R |\
          MSD_PCI_PLX_INT_CSR_DMA0_ACTIVE_R |\
          MSD_PCI_PLX_INT_CSR_BIST_ACTIVE_R |\
          MSD_PCI_PLX_INT_CSR_DOORBELL_ACTIVE_R )

#define MSD_PCI_PLX_DMA0_MODE_REG               0x80
#define MSD_PCI_PLX_DMA1_MODE_REG               0x94

#define MSD_PCI_PLX_DMA_MODE_CHAINING           0x683
#define MSD_PCI_PLX_DMA_MODE_NON_CHAINING       0x10483

#define MSD_PCI_PLX_DMA0_DATA_ADDR_REG  0x84
#define MSD_PCI_PLX_DMA1_DATA_ADDR_REG  0x98

#define MSD_PCI_PLX_DMA_BIGEND                  0x0C

#define MSD_PCI_PLX_DMA1_MODE_BIGEND            0xD0
#define MSD_PCI_PLX_DMA0_MODE_BIGEND            0xD0

#define MSD_PCI_PLX_DMA0_LOC_DATA_ADDR_REG      0x88
#define MSD_PCI_PLX_DMA1_LOC_DATA_ADDR_REG      0x9C


#define MSD_PCI_PLX_DMA0_XFER_SIZE_REG          0x8C
#define MSD_PCI_PLX_DMA1_XFER_SIZE_REG          0xA0

#define MSD_PCI_PLX_DMA0_CHAIN_DESCRIPTOR       0x90
#define MSD_PCI_PLX_DMA1_CHAIN_DESCRIPTOR       0xA4

#define MSD_PCI_PLX_DMA_INTERRUPT_ON_TERM       0x4

#define MSD_PCI_PLX_DMA_DESCRIPTOR_RD           0x8  // read into pci from local
#define MSD_PCI_PLX_DMA_DESCRIPTOR_EOC          0x2  // end-of-chain
#define MSD_PCI_PLX_DMA_DESCRIPTOR_INT          0x4  // int after terminal count
                                                     // retries to a Target

#define MSD_PCI_PLX_DMA_END_OF_CHAIN            0x2  // end-of-chain
#define MSD_CHAINS_ON_HOST                      0x1

#ifdef MERCD_LINUX
#define MsdPlxGetPlxLocalBaseAddr(slotn)             \
        *((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[slotn]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_LOCAL_BASE))

#define MsdPlxDmaEnableSctr_Mode_Chn0( slotn )  \
   	 merc_ulong_t DMAENABLESCTR_MODE_CHN0;\
	 if ((MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip) ||\
	     (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) \
	     DMAENABLESCTR_MODE_CHN0 = MSD_PCI_PACIFIC_DMA_MODE_CHAINING;\
	 else \
	     DMAENABLESCTR_MODE_CHN0 = MSD_PCI_PLX_DMA_MODE_CHAINING;\
        (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_MODE_REG)) = DMAENABLESCTR_MODE_CHN0) 

#define MsdPlxDmaDisableSctr_Mode_Chn0( slotn ) \
   	 merc_ulong_t DMADISABLESCTR_MODE_CHN0;\
	 if ((MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip) ||\
	     (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) \
	     DMADISABLESCTR_MODE_CHN0 = MSD_PACIFIC_PLX_DMA_MODE_NON_CHAINING;\
	 else \
	     DMADISABLESCTR_MODE_CHN0 = MSD_PCI_PLX_DMA_MODE_NON_CHAINING;\
        (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_MODE_REG)) = DMADISABLESCTR_MODE_CHN0)

#define MsdPlxDmaEnableSctr_Mode_Chn1( slotn )  \
   	 merc_ulong_t DMAENABLESCTR_MODE_CHN1;\
	 if ((MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip) ||\
	     (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) \
	     DMAENABLESCTR_MODE_CHN1 = MSD_PCI_PACIFIC_DMA_MODE_CHAINING;\
	 else \
	     DMAENABLESCTR_MODE_CHN1 = MSD_PCI_PLX_DMA_MODE_CHAINING;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_MODE_REG)) = DMAENABLESCTR_MODE_CHN1)

#define MsdPlxDmaDisableSctr_Mode_Chn1( slotn )  \
   	 merc_ulong_t DMADISABLESCTR_MODE_CHN1;\
	 if ((MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip) ||\
	     (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) \
	     DMADISABLESCTR_MODE_CHN1 = MSD_PACIFIC_PLX_DMA_MODE_NON_CHAINING;\
	 else \
	     DMADISABLESCTR_MODE_CHN1 = MSD_PCI_PLX_DMA_MODE_NON_CHAINING;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_MODE_REG)) = DMADISABLESCTR_MODE_CHN1)


#define MsdPlxDmaEnableBigEndian_Chn0( slotn ) \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA_BIGEND)) = MSD_PCI_PLX_DMA0_MODE_BIGEND)


#define MsdPlxDmaEnableBigEndian_Chn1( slotn ) \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA_BIGEND)) = MSD_PCI_PLX_DMA1_MODE_BIGEND)

#ifdef LFS
#define MsdPlxDmaEnable_Chn0( slotn ) { \
         merc_uint_t txbaseAddr; \
   	 merc_ulong_t DMA0_ENABLE;\
	 if (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip)\
	     DMA0_ENABLE = MSD_PCI_PACIFIC_INT_CSR_DMA0_ENABLE_W;\
	 else \
	     DMA0_ENABLE = MSD_PCI_PLX_INT_CSR_DMA0_ENABLE_W;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_INTR_REG )) = DMA0_ENABLE); \
         \
         txbaseAddr = ((volatile merc_uint_t)(long)((pmerc_uchar_t)MsdControlBlock->\
                     padapter_block_list[slotn]->phw_info->\
                     pdma_info->phys_tx_chain_base)); \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)(long)MsdControlBlock->\
         padapter_block_list[slotn]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_CHAIN_DESCRIPTOR)) = txbaseAddr); \
         }
#else
#define MsdPlxDmaEnable_Chn0( slotn ) { \
         merc_uint_t txbaseAddr; \
   	 merc_ulong_t DMA0_ENABLE;\
	 if (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip)\
	     DMA0_ENABLE = MSD_PCI_PACIFIC_INT_CSR_DMA0_ENABLE_W;\
	 else \
	     DMA0_ENABLE = MSD_PCI_PLX_INT_CSR_DMA0_ENABLE_W;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_INTR_REG )) = DMA0_ENABLE); \
         \
         txbaseAddr = ((volatile merc_uint_t)((pmerc_uchar_t)MsdControlBlock->\
                     padapter_block_list[slotn]->phw_info->\
                     pdma_info->phys_tx_chain_base)); \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[slotn]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_CHAIN_DESCRIPTOR)) = txbaseAddr); \
         }
#endif


#ifdef LFS
#define MsdPlxDmaEnable_Chn1( slotn) { \
         merc_uint_t rxbaseAddr; \
 	 merc_ulong_t DMA1_ENABLE;\
	 if (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip)\
	     DMA1_ENABLE = MSD_PCI_PACIFIC_INT_CSR_DMA1_ENABLE_W;\
	 else \
	     DMA1_ENABLE = MSD_PCI_PLX_INT_CSR_DMA1_ENABLE_W;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_INTR_REG )) = DMA1_ENABLE); \
         rxbaseAddr = ((volatile merc_uint_t)(long)((pmerc_uchar_t)MsdControlBlock->\
                     padapter_block_list[slotn]->phw_info->\
                     pdma_info->phys_rx_chain_base)); \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_CHAIN_DESCRIPTOR)) = (rxbaseAddr|MSD_PCI_PLX_DMA_DESCRIPTOR_RD)); \
         }
#else
#define MsdPlxDmaEnable_Chn1( slotn) { \
         merc_uint_t rxbaseAddr; \
 	 merc_ulong_t DMA1_ENABLE;\
	 if (MsdControlBlock->padapter_block_list[ slotn ]->phw_info->pacific_chip)\
	     DMA1_ENABLE = MSD_PCI_PACIFIC_INT_CSR_DMA1_ENABLE_W;\
	 else \
	     DMA1_ENABLE = MSD_PCI_PLX_INT_CSR_DMA1_ENABLE_W;\
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_INTR_REG )) = DMA1_ENABLE); \
         rxbaseAddr = ((volatile merc_uint_t)((pmerc_uchar_t)MsdControlBlock->\
                     padapter_block_list[slotn]->phw_info->\
                     pdma_info->phys_rx_chain_base)); \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_CHAIN_DESCRIPTOR)) = (rxbaseAddr|MSD_PCI_PLX_DMA_DESCRIPTOR_RD)); \
         }
#endif

#define MsdPlxDmaDisable_Chn0( slotn ) { \
         (*((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_CSR)) = 0); \
         \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_MODE_REG)) = 0); \
         }


#define MsdPlxDmaDisable_Chn1( slotn ) { \
         (*((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_CSR)) = 0); \
         \
         (*((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_MODE_REG)) = 0); \
         }

#define MsdPlxDmaGet_CSR_Chn0( slotn ) \
        *((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA0_CSR))

#define MsdPlxDmaGet_CSR_Chn1( slotn ) \
        *((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_DMA1_CSR))

#define MsdPlxDmaClrInt_Chn0( slotn ) \
         (*((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_CSR)) = MSD_PCI_PLX_DMA_CSR_CLR_INT_W)

#define MsdPlxDmaClrInt_Chn1( slotn ) \
         (*((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA1_CSR)) = MSD_PCI_PLX_DMA_CSR_CLR_INT_W)

#define MsdPlxDmaStart_Chn1(slotn) { \
        merc_ulong_t                dma_cnt; \
        merc_uint_t                 nextnode; \
\
		dma_cnt = MsdControlBlock->padapter_block_list[slotn]->\
                    phw_info->pdma_info->rx_dma_count - 1; \
\
		nextnode = padapter->phw_info->pdma_info->prxdma_res[dma_cnt].pdma_chain->nextnode; \
\
		nextnode &= 0x0000000F; \
        nextnode |= (MSD_PCI_PLX_DMA_END_OF_CHAIN | MSD_PCI_PLX_DMA_INTERRUPT_ON_TERM | MSD_PCI_PLX_DMA_DESCRIPTOR_RD ); \
\
		padapter->phw_info->pdma_info->prxdma_res[dma_cnt].pdma_chain->nextnode = nextnode; \
		*(pmerc_uchar_t)(((pmerc_uchar_t)(padapter->phw_info->\
           virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr)) + MSD_PCI_PLX_DMA1_CSR) = \
           MSD_PCI_PLX_DMA_CSR_START_XFR_W; \
\
}
        
#define MsdPlxDmaStart_Chn0( slotn ) { \
        mercd_dma_sgt_chain_node *ptxdma_chain; \
        merc_ulong_t  dma_cnt; \
        merc_uint_t   nextnode; \
        pmercd_adapter_block_sT padapter; \
\
	    padapter = MsdControlBlock->padapter_block_list[ slotn ]; \
        dma_cnt = padapter->phw_info->pdma_info->tx_dma_count - 1; \
        ptxdma_chain = (mercd_dma_sgt_chain_node *) &(padapter->phw_info->\
                       pdma_info->ptxdma_res[dma_cnt]); \
        nextnode = (merc_uint_t)(ptxdma_chain->nextnode); \
        nextnode &= 0x0000000F; \
        nextnode |= (MSD_PCI_PLX_DMA_END_OF_CHAIN |\
                     MSD_PCI_PLX_DMA_INTERRUPT_ON_TERM |\
                     padapter->phw_info->pdma_info->ptxdma_res[dma_cnt].location); \
        ptxdma_chain->nextnode = nextnode; \
        \
        (*((volatile merc_uchar_t *)((pmerc_uchar_t)MsdControlBlock->\
         padapter_block_list[ slotn ]->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
         MSD_PCI_PLX_DMA0_CSR)) = MSD_PCI_PLX_DMA_CSR_START_XFR_W); \
        }

#endif 

#endif
///////////////////SRAMDMA END//////////////////////////////////////////

#ifdef MERCD_SOLARIS
#define MsdPlxGetIntrReg(ConfigId)                              \
                        ddi_get32(MsdControlBlock->\
                                padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->\
                                virt_map_q[MERCD_PCI_BRIDGE_MAP]->acc_handle,\
                                (merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
                                padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->\
                                virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_INTR_REG))

#define MsdPlxPutIntrReg(ConfigId, Value)               \
                        ddi_put32(MsdControlBlock->\
                                padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->\
                                virt_map_q[MERCD_PCI_BRIDGE_MAP]->acc_handle,\
                                (merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
                                padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->\
                                virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MSD_PCI_PLX_INTR_REG),\
                                Value)
#endif

#ifdef MERCD_UNIXWARE

#define MsdPlxGetIntrReg(ConfigId)                              \
                *((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
                MCB_MsdPlxVirtMapBlock[ConfigId].VirtAddr+MSD_PCI_PLX_INTR_REG))

#define MsdPlxPutIntrReg(ConfigId, Value)               \
                *((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
                MCB_MsdPlxVirtMapBlock[ConfigId].VirtAddr+MSD_PCI_PLX_INTR_REG)) = Value;

#endif

#ifdef MERCD_LINUX

////////////////////////////////////////////////////////////////////////
//Plx Registers
////////////////////////////////////////////////////////////////////////
#define MsdPlxGetIntrReg( slotn ) \
        *((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
        padapter_block_list[ slotn ]->phw_info->\
        virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr + MSD_PCI_PLX_INTR_REG))


#define MsdPlxPutIntrReg(slotn, Value) \
        *((volatile merc_uint_t *)((pmerc_uchar_t)MsdControlBlock->\
        padapter_block_list[ slotn ]->phw_info->\
        virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr + MSD_PCI_PLX_INTR_REG)) = Value;

#endif

#define MSD_BYTE_DATA   0
#define MSD_INT_DATA            1

#define MSD_TO_HOST             0
#define MSD_TO_SRAM             1

// PCI Specific macros

#define MSD_PCI_REG_PROP_LEN  5
#define MSD_PCI_REG_PROP_PLX  1

#define MSD_PCI_ASSADDR_PROP_LEN    5
#define MSD_PCI_ASSADDR_PROP_PLX    0

//WW Support
#define MSD_WW_PCI_ASSADDR_PROP_PLX    3
#define MSD_WW_PCI_REG_PROP_PLX  4


#define MD_PCI_SUBDEVID                 0x4001
#define MD_PCI_SUBVENID                 0x12C7

#define MD_PCI_VENID_LOCATION		0x70
#define MD_PCI_DLGCID			0x905410B5
#define MD_PCI_NEWBID                   0x535612c7

// For Rotary Switch

#define PCI_CR_OFFSET_LOCATOR_ID                0xA7
#define PCI_CR_OFFSET_SERIAL_NUM                0xAB
#define PCI_CR_LENGTH_SERIAL_NUM                8

#ifdef SKIP
typedef struct _MSD_PROBE_BOARD_CONFIG {
        merc_ushort_t  BoardType;
        merc_ushort_t  PLXVenId;
        merc_ushort_t  PLXDevId;
        merc_ushort_t  SubId;
        merc_ushort_t  SubVenId;
        merc_uint_t    VendorId;
        merc_uint_t    SecondaryVendorId;
        merc_uint_t    PrimaryBoardId;
        merc_ushort_t  pci_reg_prop_num;
        merc_ushort_t  pci_reg_prop_sram;
        merc_ushort_t  pci_assignedaddr_prop_sram;
        merc_ushort_t  pci_assignedaddr_prop_num;
} MSD_PROBE_BOARD_CONFIG, *PMSD_PROBE_BOARD_CONFIG;
#endif

// PCI Express
#define PCI_CAPABILITY_LIST_REG_OFFSET                  0x34            // Start of the PCI capability list
#define PCI_CAPABILITY_TYPE_MASK                        0x00FF          // Bits 7:0
#define PCI_CAPABILITY_LIST_OFFSET_MASK                 0xFF00          // Bits 15:8
#define PCI_CAPABILITY_LIST_OFFSET                      8

#define PCI_EXPRESS_CAPABILITY                          0x10            // Indicates PCI Express board.
#define PCI_EXPRESS_POWER_SCALE_MASK                    0x0C000000      // Bits 27:26 of the device capability
#define PCI_EXPRESS_POWER_SCALE_OFFSET                  26
#define PCI_EXPRESS_POWER_LIMIT_MASK                    0x03FC0000      // Bits 25:18 of the device capability
#define PCI_EXPRESS_POWER_LIMIT_OFFSET                  18

// Seaville specific defines for power management
#define SEAVILLE_POWER_GOOD_REG_OFFSET                  0x150           // Offset into BAR 0
#define SEAVILLE_POWER_GOOD_MASK                        0x20000000      // Bit 29
#define SEAVILLE_POWER_GOOD_OFFSET                      29

#define SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET         0x6C            // Offset into BAR 0
#define SEAVILLE_USER_OVERRIDE_POWER_MASK               0x20000         // Bit 17
#define SEAVILLE_USER_OVERRIDE_POWER_OFFSET             17

#define SEAVILLE_POWER_BUDGETTING_REG_USED              0x80000000              // MS bit
#define SEAVILLE_POWER_WATTAGE_READ_MASK                0x000000FF              // LS Byte

// Some Macros

#define BOARD_TYPE(x)                   msd_probe_brd_cfg[x].BoardType
#define PLX_VEN_ID(x)                   msd_probe_brd_cfg[x].PLXVenId
#define PLX_DEV_ID(x)                   msd_probe_brd_cfg[x].PLXDevId
#define SUBSYSTEM_ID(x)                 msd_probe_brd_cfg[x].SubId
#define SUB_VEN_ID(x)                   msd_probe_brd_cfg[x].SubVenId
#define VENDOR_ID(x)                    msd_probe_brd_cfg[x].VendorId
#define SEC_VENDOR_ID(x)                msd_probe_brd_cfg[x].SecondaryVendorId
#define PRIMARY_BOARD_ID(x)             msd_probe_brd_cfg[x].PrimaryBoardId
#define PCI_REG_PROP_NUM(x)             msd_probe_brd_cfg[x].pci_reg_prop_num
#define PCI_REG_PROP_SRAM(x)            msd_probe_brd_cfg[x].pci_reg_prop_sram
#define PCI_ASSIGNADDR_PROP_NUM(x)      msd_probe_brd_cfg[x].pci_assignedaddr_prop_num
#define PCI_ASSIGNADDR_PROP_SRAM(x)     msd_probe_brd_cfg[x].pci_assignedaddr_prop_sram

#define PCI_VIRT_MAP_O_SIZE             2*(sizeof(pmercd_adapter_block_sT))
#ifdef SKIP
/* ASA additions */
#define  MERCD_PCI_BRIDGE_MAP   0
#define  MERCD_PCI_SRAM_MAP     1
#endif

#endif
