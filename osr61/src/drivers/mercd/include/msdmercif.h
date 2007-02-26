/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdmercif.h
 * Description                  : Interface Definitions
 *
 *
 **********************************************************************/

#ifndef _MSDMERCIF_
#define _MSDMERCIF_

// Mercury board and MERC bus hardware interface definitions 

// MERC base addresses 
// each slot takes up 0x80000 

#define MERC_BASE_ADDRESS_SLOT1		0x00080000

#define MD_MERC_ID_DIALOGIC		0x0000A0E6
#define MD_VMSCP_ID			0x40000000
#define MD_PMSCP_ID_1			0x40010000
#define MD_PMSCP_ID_2			0x40110000
#define MD_VMSCP_ID_MASK		0xFFFF0000
#define MD_VMSCP_PROTOCOL_VER		0x00000001

// Maximum size of the config ROM
#define MERC_CR_MAX_SIZE			0x100

// MERC config rom memory map 

#define MERC_CR_OFFSET_CHECKSUM		0x03
#define MERC_CR_LENGTH_CHECKSUM		4
#define MERC_CR_OFFSET_LENGTH		0x07
#define MERC_CR_LENGTH_LENGTH		3
#define MERC_CR_OFFSET_VENDOR_ID	0x27
#define MERC_CR_LENGTH_VENDOR_ID	3
#define MERC_CR_OFFSET_BOARD_PRI_ID	0x33
#define MERC_CR_LENGTH_BOARD_PRI_ID	4
#define MERC_CR_OFFSET_BOARD_SEC_ID	0x43
#define MERC_CR_LENGTH_BOARD_SEC_ID	4
#define MERC_CR_OFFSET_SRAM_START	0x83
#define MERC_CR_LENGTH_SRAM_START	3
#define MERC_CR_OFFSET_SRAM_VIRT_TOP	0x8F
#define MERC_CR_LENGTH_SRAM_VIRT_TOP	3
#define MERC_CR_OFFSET_SRAM_SIZE	0x9B
#define MERC_CR_LENGTH_SRAM_SIZE	3

// MERC pre-configures each slot with 0x80000 (512K)
// bytes and slot 0 begins at 0x0
#define MERC_SLOT0_PHYS_ADDR		0x00000000
#define MERC_SLOT1_PHYS_ADDR		0x80000
#define MERC_SLOT_SIZE			0x80000
#define MERC_CONFIG_ROM_MAX_SIZE	0x100
#define MERC_MAX_SLOTS			24

#define MERC_READ_CONFIG_ROM_ULONG(A) \
 	(  (*((pmerc_uchar_t)(A)))<<24 | (*((pmerc_uchar_t)(A)+4))<<16 | \
	(*((pmerc_uchar_t)(A)+8))<<8 | (*((pmerc_uchar_t)(A)+12))  )

#define MERC_READ_CONFIG_ROM_TRI_BYTES(A) \
 	(  (*((pmerc_uchar_t)(A)))<<16 | (*((pmerc_uchar_t)(A)+4))<<8 | \
	(*((pmerc_uchar_t)(A)+8)) )


#endif // _MSDMERCIF_
