/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: 21554.h
//
// Defines name mappings for the 21554 registers and values.
/////////////////////////////////////////////////////////////////////////////
#ifndef _21554_H
#define _21554_H

// Register Map

// General Constants
//
#define uSEC10				     0x0A
#define	CONFIG_HEADER_SIZE		     0x1000
#define	CSR_LENGTH			     0x1000

// The following constants are used for the Configuration Header
//
#define	PRIMARY_COMMAND_REG	     0x04
#define IO_SPACE_ENABLE              0x0001
#define MEMORY_SPACE_ENABLE	     0x0002
#define MASTER_ENABLE		     0x0004
#define MEM_WRITE_AND_INVALIDATE     0x0010
#define	PERR_ENABLE		     0x0040
#define SERR_ENABLE		     0x0100
#define FAST_B2B_Enable              0x0200

#define PRIMARY_STATUS_REG           0x06
#define DATA_PARITY_DETECTED	     0x0100
#define SIGNALED_TARGET_ABORT	     0x0800
#define RECEIVED_TARGET_ABORT	     0x1000
#define RECEIVED_MASTER_ABORT	     0x2000
#define SIGNALED_SYSTEM_ERROR	     0x4000
#define DETECTED_PARITY_ERROR	     0x8000


/********
 *
 * Configuration Space Mappings
 *
 ********/

// Device-Specific Control and Status Registers

#define CHIP_CONTROL_0_REG	       0xCC
#define	PRIMARY_ACCESS_LOCKOUT         0x400

#define CHIP_CONTROL_1_REG	       0xCE
#define UPSTREAM_MEM2_PAGESIZE_MASK    0x0F00
#define	UPSTREAM_MEM2_DISABLE          0x0000
#define	UPSTREAM_MEM2_PAGESIZE_256     0x0100
#define	UPSTREAM_MEM2_PAGESIZE_512     0x0200
#define	UPSTREAM_MEM2_PAGESIZE_1K      0x0300
#define	UPSTREAM_MEM2_PAGESIZE_2K      0x0400
#define	UPSTREAM_MEM2_PAGESIZE_4K      0x0500
#define	UPSTREAM_MEM2_PAGESIZE_8K      0x0600
#define	UPSTREAM_MEM2_PAGESIZE_16K     0x0700
#define	UPSTREAM_MEM2_PAGESIZE_32K     0x0800
#define	UPSTREAM_MEM2_PAGESIZE_64K     0x0900
#define	UPSTREAM_MEM2_PAGESIZE_128K    0x0A00
#define	UPSTREAM_MEM2_PAGESIZE_256K    0x0B00
#define	UPSTREAM_MEM2_PAGESIZE_512K    0x0C00
#define	UPSTREAM_MEM2_PAGESIZE_1M      0x0D00
#define	UPSTREAM_MEM2_PAGESIZE_2M      0x0E00
#define	UPSTREAM_MEM2_PAGESIZE_4M      0x0F00

#define	I2O_ENABLE_MASK		       0x1000
#define I2O_ENABLE                     0x1000

#define	I2O_SIZE_MASK	               0xE000
#define	I2O_SIZE_256		       0x0000
#define	I2O_SIZE_512		       0x2000
#define	I2O_SIZE_1K		       0x4000
#define	I2O_SIZE_2K		       0x6000
#define	I2O_SIZE_4K		       0x8000
#define I2O_SIZE_8K                    0xA000
#define I2O_SIZE_16K                   0xC000
#define I2O_SIZE_32K                   0xE000

#define UPSTREAM_MEM2_LOOKUPTBL        0x100
#define UPSTREAM_MEM2_PREFETCH         0x00000008
#define UPSTREAM_MEM2_VALID            0x00000001
#define UPSTREAM_MEM2_RESERVED         0x000000F6
#define UPSTREAM_MEM2_ADDRESS_MASK     0xFFFFFF00
#define UPSTREAM_MEM2_LOOKUPTBL_SIZE   64

#define RESET_CONTROL_REG              0xD8
#define HOTSWAP_CONTROL_REG            0xEE
#define SECONDARY_RESET		       0x01
#define CHIP_RESET		       0x02
#define HSCSR_LED_LO                   0x08

// The following constants are used for the CSR Registers
//

// Doorbell IRQ Registers and values
#define PRIMARY_CLEAR_IRQ_REG		0x98
#define SECONDARY_SET_IRQ_REG		0x9E	
#define PRIMARY_CLEAR_IRQ_MASK_REG	0xA0
#define PRIMARY_SET_IRQ_MASK_REG	0xA4

#define	PMACD_CMD_DOORBELL		0x0001
#define	PMACD_PANIC_DOORBELL		0x0002
#define ALL_DOORBELLS_OFF	        0xFFFF

#define I2O_NO_MFA			0xFFFFFFFF

#define	I2O_OUTBOUND_QUEUE_REG	        0x44
#define I2O_OUTBOUND_POST_STATUS_REG	0x30
#define I2O_OUTBOUND_POST_IRQ_MASK_REG	0x34	
#define I2O_OUTBOUND_IRQ_MASK_BIT	0x08
#define I2O_OUTBOUND_IRQ_STATUS_BIT	0x08						
#define	I2O_INBOUND_QUEUE_REG	        0x40


// Upstream Memory 2 Lookup table
#define	M2LUT				0x100
#define	INBOUND_I2O_FIFO_LUT		M2LUT
#define OUTBOUND_I2O_FIFO_LUT		M2LUT+4
#define	MAX_STREAM_BFS			42
#define STREAM_BUFFER0_LUT		M2LUT+8
#define HOST_TRACE_BUFFER_LUT		STREAM_BUFFER0_LUT+4*MAX_STREAM_BFS

#define SCRATCHPAD_0_REG 0x0A8
#define SCRATCHPAD_1_REG 0x0AC
#define SCRATCHPAD_2_REG 0x0B0
#define SCRATCHPAD_3_REG 0x0B4
#define SCRATCHPAD_4_REG 0x0B8
#define SCRATCHPAD_5_REG 0x0BC
#define SCRATCHPAD_6_REG 0x0B0
#define SCRATCHPAD_7_REG 0x0C4


typedef enum {
  PMACD_CMD_NULL               = 0x00000000,  /* no command */
  /*  0x00000001 is reserved according to the firmware include file */
  PMACD_CMD_INIT               = 0x00000002,  /* run out of local SRAM */ 
  PMACD_CMD_INIT_RSP           = 0x00000003,  /* running out of SRAM response */ 
  PMACD_CMD_DOWNLOAD_START     = 0x00000004,  /* start download firmware */ 
  PMACD_CMD_DOWNLOAD_START_RSP = 0x00000005,  /* start download fw response */ 
  PMACD_CMD_PING               = 0x00000006,  /* ping */ 
  PMACD_CMD_PING_RSP           = 0x00000007,  /* ping response */ 
  PMACD_CMD_CONFIG             = 0x00000008,  /* configure PCI interface */ 
  PMACD_CMD_CONFIG_RSP         = 0x00000009,  /* configure PCI intf response */ 
  PMACD_CMD_RESET              = 0x0000000A,  /* reset the system */ 
  PMACD_CMD_RESET_RSP          = 0x0000000B,  /* reset response */ 
  PMACD_CMD_START              = 0x0000000C,  /* start all tasks */ 
  PMACD_CMD_START_RSP          = 0x0000000D,  /* start all tasks response */ 
  PMACD_CMD_QUIESCE            = 0x0000000E,  /* stop all tasks */ 
  PMACD_CMD_QUIESCE_RSP        = 0x0000000F,  /* stop all tasks response */ 
  PMACD_CMD_STOP               = 0x00000010,  /* stop all tasks */ 
  PMACD_CMD_STOP_RSP           = 0x00000011,  /* stop all tasks response */ 
  PMACD_CMD_RUN_FULL_DIAG      = 0x00000012,  /* run full diagnostics */ 
  PMACD_CMD_RUN_FULL_DIAG_RSP  = 0x00000013,  /* full diagnostics response */ 
  PMACD_CMD_GET_CODE_INFO      = 0x00000014,  /* get current code info */ 
  PMACD_CMD_GET_CODE_INFO_RSP  = 0x00000015,  /* current code info response */ 
  PMACD_CMD_DOWNLOAD_COMPLETE  = 0x00000016,  /* Host completed fw download */ 
  PMACD_CMD_DOWNLOAD_COMPLETE_RSP  = 0x00000017,  /* complete fw download resp*/ 
  PMACD_CMD_ENTER_BOOT         = 0x00000018,  /* enter boot */ 
  PMACD_CMD_ENTER_BOOT_RSP     = 0x00000019,  /* enter boot response */ 
  PMACD_CMD_TIMEOUT_RSP        = 0x10000003,  /* Timed out waiting for response */
  PMACD_CMD_WRONG_RSP          = 0x10000005   /* Got wrong response */

}pmacd_controlcmds_t;



#endif /* _21554_H */









