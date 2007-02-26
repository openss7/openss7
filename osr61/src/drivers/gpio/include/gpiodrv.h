/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gpiodrv.h
 * Description                  : GPIO header
 *
 *
 **********************************************************************/

#ifndef __GPIODRV_H__
#define __GPIODRV_H__

#ifdef __STANDALONE__            /* Required header files */
#include <sys/stream.h>
#include "gentypedefs.h"
#endif


/*
 * Defines
 */
#ifndef FALSE
#define FALSE                 0
#endif
#ifndef TRUE
#define TRUE                  ~FALSE
#endif

/*
 * Generic Port I/O Device Driver IOCTLs
 */
#define GPIO_LOGMSG                0x01
#define GPIO_READ_PORT             0x02
#define GPIO_WRITE_PORT            0x03
#define GPIO_READ_MEMORY           0x04
#define GPIO_WRITE_MEMORY          0x05
#define GPIO_LOCATE_BOARDS         0x06
#define GPIO_WRITE_CCM_DATA        0x07
#define GPIO_GET_VERSION_NUM       0x08
#define GPIO_MAP_BOARD             0x09
#define GPIO_ENABLE_INTERRUPTS     0x0A
#define GPIO_DISABLE_INTERRUPTS    0x0B
#define GPIO_GET_INTERRUPT_COUNT   0x0C
#define GPIO_RESET_INTERRUPT_COUNT 0x0D
#define GPIO_GET_PCI_BRD_INFO	   0x0F 
#define GPIO_GET_NUM_PCI_BRDS	   0x10
#define GPIO_GET_PCI_BRD_BUS_SLOT  0x11
#define GPIO_SET_PCI_BUS_NUM	   0x12
#define GPIO_GET_PCIe_POWER_STATUS 0x13

/*
 * Values used to extract segment and offset from an address
 */
#define SEGMENT               0xFE000
#define OFFSET                0x01FFF

/*
 * CCM Input Buffer Full Status
 */
#define CCM_IBF               0x02
#define CCM_WAIT_TICKS        5000000

/*
 * Board types
 */

#define PCI_TYPE_PYTHON		1
#define PCI_TYPE_MAGNUM		4
#define PCI_MAGNUM_T1_ID	0x052512C7
#define PCI_MAGNUM_E120_ID	0x052612C7
#define PCI_MAGNUM_E1_ID	0x052712C7

/*
 * Dialogic PCI board record definition 
 */
typedef struct {
	unsigned int	pbr_type;
	unsigned int 	pbr_brd_id;
	unsigned int 	pbr_slotnum;
	unsigned long 	pbr_address;
	unsigned long	pbr_cfgaddress;
	unsigned int 	pbr_irq;
	unsigned int 	pbr_instance;
}  DLGC_PCIBRD_REC, *PDLGC_PCIBRD_REC;

/*
 * Dialogic PCIe power status strucutre
 */
typedef struct {
	unsigned char	boardIndex;			// Physcial Board Id
	unsigned int	powerGoodRegValue;  		//  Power Good Status
	unsigned int	powerOverrideRegValue;		//  Power Management Override
	unsigned int	actualPowerProvidedValue;	// Actual Power Provided 
	unsigned int	powerRequiredByBoard;		// Power required by board.
	unsigned int 	busNumber;			// Bus number the board is placed in
	unsigned int	slotNumber;			// Slot Number the board is placed in
} POWER_STATUS_STRUCT,*pPOWER_STATUS_STRUCT;

	
/*
 * Shared RAM control registers area size and register offsets
 * from base to locate D4XEs and SPANs.
 */
#define SRAM_CTL_AREA_SIZE    0x100
#define D4XE_LOCATE1          0x0E4
#define D4XE_LOCATE2          0x0E0


/*
 * Typedefs
 */
typedef struct iocblk IOCBLK;


/*
 * Data Structures
 */
/*
 * An M_DATA block of a streams message can hold a maximum of 1024 bytes.
 * Any additional bytes are in the next M_DATA block.  We will use a size
 * that will allow us to send atleast 8K bytes (genboot.bin) at a time.
 * Since gpio_data structure has two ULONGs which take up 8 bytes, the
 * maximum size of buffer used to transfer data between kernel/user space
 * can be 1016 + 8 * 1024 = 9208 bytes.
 * Max size of the gpio_data structure would then be 9208 + 8 = 9216 bytes.
 */
#define GPIOBUFSIZE  9208
#define GPIOMAXSIZE  9216

typedef struct gpio_data {
   unsigned long address;
   unsigned int bufsize;
   unsigned char buffer[GPIOBUFSIZE];
} GPIO_DATA;

typedef struct gpio_mapdata {
   unsigned long address;
   unsigned char id;
} GPIO_MAPDATA;

typedef struct gpio_intdata {
   unsigned short int_cnt;
} GPIO_INTDATA;

#endif

