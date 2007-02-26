/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antpci.h
 * Description                  : antares pci definitions
 *
 *
 **********************************************************************/

#ifndef __ANTPCI_H__
#define __ANTPCI_H__

#define PCI_PLX_CFG_SIZE	0x80
#define PCI_PLX_INTR_REG        0x4C
#define PCI_PLX_FF_REG          0x50
#define PCI_PLX_INTR_ENABLE_BIT 0x41
#define PCI_PLX_INTR_ACTIVE_BIT 0x04

#define BYTESETFLIPFLOP		0x96
#define BYTERESETFLIPFLOP	0x92

#define PCI_MAX_BOARDS		16
 
#define PCI_ANT_ID		0x017812C7
#define ANT_VENDID	        0x10B5
#define ANT_DEVID	        0x9050
#define ANT_BCLASS	        0x06
#define ANT_SCLASS	        0x80

#ifdef SCO_PCI
#define PCI_CONF_BASE0 		0x10  /* Configuration address for DLGC PCI */
#define PCI_CONF_BASE2 		0x18  /* SRAM address for DLGC PCI */
#define PCI_CONF_SUBVENID 	0x2C  /* Subvendor  ID for DLGC PCI */
#define PCI_CONF_SUBSYSID 	0x2E  /* Subsystem ID for DLGC PCI */
#define PCI_CONF_ILINE 		0x3C  /* Interrupt Line for DLGC PCI */
#endif

#endif /* __ANTPCI_H__ */
