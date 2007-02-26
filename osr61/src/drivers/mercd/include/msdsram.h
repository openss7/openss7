/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdsram.h
 * Description                  : SRAM data structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDSRAM_
#define _MSDSRAM_

/*  Host ram regions */

#define MERC_HOST_RAM_START	0x00000000
#define MERC_HOST_RAM_END	0x0007FF7F

#define GENERAL_ENTRIES 32
/* 
 * Copying regions 
 */
#define MSD_BYTE_DATA		0
#define MSD_INT_DATA		1
#define MSD_SYS_BSTREAM_DATA	2
#define MSD_SYS_GSTREAM_DATA 	3


#define MSD_MAX_INTRS 255

#ifdef MERCD_DATATYPE_INTEL
#define MSD_SYS_DATA		MSD_INT_DATA
#else
#define MSD_SYS_DATA		MSD_BYTE_DATA
#endif

#define MSD_TO_HOST		0
#define MSD_TO_SRAM		1

// Panic buffer offset and size 
// #define MERC_FW_PANIC_SIZE	0x1000	// From offset TOP upto this size

// MERC Config RAM Size 
#define MERC_CONFIG_RAM_SIZE 0x100

// Shared Ram Start offset from base address 
#define MERC_SRAM_START_OFFSET		0x100	// Size could be 0x80000 or 0x20000  

// Clear message ready interrupt register 
#define MERC_CLR_MSG_READY_INTR_REG	0x0007FF83

// Defines for message ready intrrupt register operations
#define MERC_MSG_READY_INTR_R		0x01
#define MERC_HOST_RAM_PEND_INTR_R	0x02
#define MERC_ERROR_INTR_R			0x04
#define MERC_CLR_MSG_READY_INTR_W	0x01

// Clear host ram pending interrupt register 
#define MERC_CLR_HOST_RAM_PEND_INTR_REG	0x0007FF87
#define MERC_CLR_HOST_RAM_PEND_INTR_W	0x01

// Clear error interrupt register 
#define MERC_CLR_ERROR_INTR_REG	0x0007FF8B
#define MERC_CLR_ERROR_INTR_W		0x01

// set CP interrupt register 
#define MERC_SET_CP_INTR_REG			0x0007FF8F
#define MERC_CP_INTR_ALREADY_SET_R	0x01
#define MERC_SET_CP_INTR_W				0x01

// set CP NMI register 
#define MERC_SET_CP_NMI_REG			0x0007FF93
#define MERC_CP_NMI_ALREADY_SET_R	0x01
#define MERC_SET_CP_NMI_W			0x01

// clear CP NMI register 
#define MERC_CLR_CP_NMI_REG		0x0007FF97
#define MERC_CLR_CP_NMI_W		0x01

// set board reset register 
#define MERC_SET_BOARD_RESET_REG		0x0007FF9B
#define MERC_BOARD_ALREADY_RESET_R	0x01
#define MERC_SET_BOARD_RESET_W			0x01

// Clear board reset register 
#define MERC_CLR_BOARD_RESET_REG	0x0007FF9F
#define MERC_CLR_BOARD_RESET_W		0x01


// Set Host ram request register
#define MERC_SET_HOST_RAM_REQUEST_REG			0x0007FFA3
#define MERC_SET_HOST_RAM_REQUEST_W				0xFF
#define MERC_HOST_RAM_REQUEST_ALREADY_SET_R	0x01
#define MERC_HOST_RAM_GRANT_R						0x02
#define MERC_HOST_RAM_GRANT_PEND_R				0x04


// Clear Host ram request register 
#define MERC_CLR_HOST_RAM_REQUEST_REG	0x0007FFA7
#define MERC_CLR_HOST_RAM_REQUEST_W		0xFF
#define MERC_HOST_RAM_GRANT_FOR_CLR_R	0x01
#define MERC_HOST_RAM_PEND_FOR_CLR_R		0x00


// Check Host ram error access register (read/written w/o sema) 
#define MERC_CHK_HOST_RAM_ERROR_ACC_REG	0x0007FFAB
#define MERC_CHK_HOST_RAM_ERROR_ACC_W	0xFF
#define MERC_CHK_HOST_RAM_ERROR_ACC_R	0x01


// Set Boot Host ram bit register 
#define MERC_SET_BOOT_HOST_RAM_BIT_REG	0x0007FFAF
#define MERC_SET_BOOT_HOST_RAM_BIT_W		0xFF
#define MERC_BOOT_FROM_HOST_RAM_BIT_R	0x01
#define MERC_BOOT_FROM_FLASH_BIT_R		0x00


// Clear Boot Host ram bit register 
#define MERC_CLR_BOOT_HOST_RAM_BIT_REG	0x0007FFB3
#define MERC_CLR_BOOT_HOST_RAM_BIT_W		0xFF

// Debug Return Value locations
#define MERC_DEBUG_REGISTER_ONE	0xF8
#define MERC_DEBUG_REGISTER_TWO	0xFC

// Define the Debug valid results
#define FIRST_DEBUG_VALID_RESULT	 0x03
#define SECOND_DEBUG_VALID_RESULT 0xFC


// in millisecs 
#define MERC_BOARD_RESET_WAIT_INTERVAL	100
#define MERC_BOARD_RESET_TIMEOUT			5000


#define MERC_INTR_ADAPTER(RegisterBlock) {\
	MsdRegWriteUchar(RegisterBlock->SetCpIntr, \
		(merc_uchar_t)MERC_SET_CP_INTR_W); \
	}

#define MERC_GET_SRAM_NO_INTR_LOCK(RegisterBlock,HostRamReq) { \
	MsdRegWriteUchar(RegisterBlock->SetHostRamRequest, \
				(merc_uchar_t)MERC_SET_HOST_RAM_REQUEST_W);  \
	HostRamReq=MsdRegReadUchar(RegisterBlock->SetHostRamRequest);\
	}

#define MERC_GET_SRAM_LOCK(RegisterBlock,HostRamReq) { \
	MsdRegWriteUchar(RegisterBlock->SetHostRamRequest, \
				(merc_uchar_t)MERC_SET_HOST_RAM_REQUEST_W);  \
	HostRamReq=MsdRegReadUchar(RegisterBlock->ClrHostRamRequest);\
	}

#define MERC_CHECK_SRAM_LOCK(RegisterBlock,HostRamReq) { \
	HostRamReq=MsdRegReadUchar(RegisterBlock->SetHostRamRequest);\
	}

#define MERC_FREE_SRAM_LOCK(RegisterBlock) { \
	MsdRegWriteUchar(RegisterBlock->ClrHostRamRequest, \
				(merc_uchar_t)MERC_CLR_HOST_RAM_REQUEST_W);  \
	}

#define MERC_SET_BOARD_RESET(RegisterBlock) { \
	MsdRegWriteUchar(RegisterBlock->SetBoardReset, \
				(merc_uchar_t)0xFF);  \
	}

#define MERC_CLEAR_BOARD_RESET(RegisterBlock) { \
	MsdRegWriteUchar(RegisterBlock->ClrBoardReset, \
				(merc_uchar_t)0xFF);  \
	}

#define MERC_SET_BOOT_HOST_RAM_BIT(RegisterBlock) { \
	MsdRegWriteUchar(RegisterBlock->SetBootHostRamBit, \
				(merc_uchar_t)0xFF);  \
	}

#define MERC_CLR_BOOT_HOST_RAM_BIT(RegisterBlock) { \
	MsdRegWriteUchar(RegisterBlock->ClrBootHostRamBit, \
				(merc_uchar_t)0xFF);  \
	}

#define MERC_CLEAR_INTERRUPT(RegBlock,IntrCode) { \
	if(IntrCode & MERC_MSG_READY_INTR_R) \
		MsdRegWriteUchar(RegBlock->ClrMsgReadyIntr, \
				MERC_CLR_MSG_READY_INTR_W); \
	if(IntrCode & MERC_HOST_RAM_PEND_INTR_R) \
		MsdRegWriteUchar(RegBlock->ClrHostRamPendIntr, \
				MERC_CLR_HOST_RAM_PEND_INTR_W); \
	if(IntrCode & MERC_ERROR_INTR_R) \
		MsdRegWriteUchar(RegBlock->ClrErrorIntr, \
				MERC_CLR_ERROR_INTR_W); \
	}

// Debug Read
#define MERC_CHECK_DEBUG_ONE(RegisterBlock,DebugVal) { \
	DebugVal=MsdRegReadUchar(RegisterBlock->DebugOneAddress);\
	}

#define MERC_CHECK_DEBUG_TWO(RegisterBlock,DebugVal) { \
	DebugVal=MsdRegReadUchar(RegisterBlock->DebugTwoAddress);\
	}


#define MERC_CONFIG_BLK_ALIGNMENT	16

#define CONFIG_ACK_ADDRESS(base) \
    ((PCHAR)base+supp_rnd2nxt(( \
    sizeof(MERCURY_HOST_IF_CONFIG) +\
    (sizeof(mercd_dma_sgt_chain_node) *\
    (MAX_MERC_TX_DMA_CHAINS_ON_BOARD +\
    MAX_MERC_RX_DMA_CHAINS_ON_BOARD))),\
    MERC_CONFIG_BLK_ALIGNMENT))

/////////SRAMDMA BEGIN/////////////////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
#define TX_CHAIN_BASEADDR(base) \
    (PCHAR)base+supp_rnd2nxt(( \
    sizeof(MERCURY_HOST_IF_CONFIG) +\
    MSD_CONFIG_ROM_MAX_SIZE) ,\
    MERC_CONFIG_BLK_ALIGNMENT)

#define RX_CHAIN_BASEADDR(base) \
    (PCHAR)base+supp_rnd2nxt(( sizeof(MERCURY_HOST_IF_CONFIG) \
     + sizeof(mercd_dma_sgt_chain_node) *\
     MAX_MERC_TX_DMA_CHAINS_ON_BOARD ) +\
     MSD_CONFIG_ROM_MAX_SIZE,\
     MERC_CONFIG_BLK_ALIGNMENT)
#endif
/////////SRAMDMA END/////////////////////////


// Mercury Register block structure : SRAM
typedef struct _mercd_reg_block {
	pmerc_uchar_t   HostRamStart;
	pmerc_uchar_t   ClrMsgReadyIntr;
	pmerc_uchar_t   ClrHostRamPendIntr;
	pmerc_uchar_t   ClrErrorIntr;
	pmerc_uchar_t   SetCpIntr;
	pmerc_uchar_t   SetCpNmi;
	pmerc_uchar_t   ClrCpNmi;
	pmerc_uchar_t   SetBoardReset;
	pmerc_uchar_t   ClrBoardReset;
	pmerc_uchar_t   SetHostRamRequest;
	pmerc_uchar_t   ClrHostRamRequest;
	pmerc_uchar_t   SetBootHostRamBit;
	pmerc_uchar_t   ClrBootHostRamBit;
	pmerc_uchar_t   DebugOneAddress;
	pmerc_uchar_t   DebugTwoAddress;
}mercd_reg_block_sT, *pmercd_reg_block_sT;


// Mercury Host Config Block Structure : SRAM
typedef struct _mercury_host_config_info {
	merc_uint_t  protocolRev;
	merc_uint_t  checkSum;
	merc_uint_t  hostConfigSize; 
	merc_uint_t  maxHostMsgs;
	merc_uint_t  maxFwMsgs;
	merc_uint_t  dataAlignment;  /* power of 2 starts at 2**1 */
	merc_uint_t  maxHostDataXfer;
	merc_uint_t  maxFwDataXfer;
	merc_uint_t  hostConfigMask;
	merc_uint_t  hwIntInterval;
	merc_uint_t  dataBlockLimit; /* Must be < 100 */
	merc_uint_t  extBlockLimit;  
	merc_uint_t  boardNumber;
	merc_uint_t  NodeNumber;
	merc_uint_t  vmeIntLevel;
	merc_uint_t  vmeIntVector;
}mercury_host_config_info_sT, *pmercury_host_config_info_sT;



// Mercury Host Config Ack Structure
typedef struct _mercury_host_config_info_ack {
	merc_uint_t  protocolRev;
	merc_uint_t  checkSum;
	merc_uint_t  replyCode;
	merc_uint_t  pHostToFwMsgs;
	merc_uint_t  pFwToHostMsgs;
	merc_uint_t  pFreeNodes;
	merc_uint_t  blockCount;
	merc_uint_t  pHostDataXfer;
	merc_uint_t  pFwDataXfer;
	merc_uint_t  general[GENERAL_ENTRIES];
	merc_uchar_t Pad[12];
}mercury_host_config_info_ack_sT, *pmercury_host_config_info_ack_sT;

/* Free list header struct for bulk data block control */
/* 
 * task #7953: changed merc_uint_t to size_t for 64bit compatiblity 
 */
typedef struct _mercury_free_hdr_list_hdr{
	merc_uint_t   HeadNodeOffset; 
	merc_uint_t   TailNodeOffset; 
	merc_uint_t   FreeBlockCount;
	merc_uint_t   CpFreeCounter;  
} mercury_free_hdr_list_sT, *pmercury_free_hdr_list_sT;

/* 
 * task #7953: changed merc_uint_t to size_t for 64bit compatiblity 
 */
typedef struct _mercury_host_info {
	size_t   host_to_fw_msg_start;  /* Was -> merc_uint_t    host_to_fw_msg_start; */
	size_t   fw_to_host_msg_start;  /* Was -> merc_uint_t    fw_to_host_msg_start; */
	size_t   msg_ring_buffer_size;  /* Was -> merc_uint_t    msg_ring_buffer_size; */
	size_t   host_to_fw_data_start; /* Was -> merc_uint_t   host_to_fw_data_start; */
	size_t   fw_to_host_data_start; /* Was -> merc_uint_t   fw_to_host_data_start; */
	size_t   data_ring_buffer_size; /* Was -> merc_uint_t   data_ring_buffer_size; */
	pmercury_free_hdr_list_sT  pfree_list_header_start; 
}mercury_host_info_sT, *pmercury_host_info_sT;

// Host Information Structure : SRAM
typedef struct _mercd_host_info {
	mercd_reg_block_sT              reg_block;
	mercury_host_config_info_sT     host_config;
	mercury_host_config_info_ack_sT host_config_ack;
	mercury_host_info_sT            merc_host_info;
	merc_uchar_t                    merc_config_rom[MSD_CONFIG_ROM_MAX_SIZE];
}mercd_host_info_sT, *pmercd_host_info_sT;

#define MERCD_HOST_INFO sizeof(mercd_host_info_sT)
#endif /* _MSDSRAM_ */
