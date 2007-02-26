/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_linux_pci.h
//
// Defines functions that deal with pmac pci interface 
// under linux.
// 
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_LINUX_PCI_H
#define _PMACD_LINUX_PCI_H

#include "pmacd.h"
#include "pmacd_mf.h"
#include "pmacd_stream.h"

typedef struct{
  UINT8_T  interruptLine;
  UINT8_T  cacheLineSize;
  UINT8_T  latencyTimer;
  UINT32_T bar[6];
}pmacd_pciconfigrom_t;

typedef struct{
  MUTEX_T mutex;
  struct pci_dev *pciDev;
  unsigned int irq;
  pmacd_memaddress_t csrBaseAddress;
  pmacd_memaddress_t configBaseAddress;
  pmacd_pciconfigrom_t configRom;
  void *csrMappedBaseAddress;
  void *configMappedBaseAddress;
}pmacd_pcibusdevice_t;


void pmacd_updatePCIConfigRom(pmacd_pcibusdevice_t *pci);
BOOLEAN_T pmacd_configurePCIDevices(void);
BOOLEAN_T pmacd_configurePCIDevice(struct pci_dev *ppcidev);
void pmacd_printPCIBusDev(pmacd_pcibusdevice_t *pci);
void pmacd_setupPciBar2(pmacd_pcibusdevice_t *bus, 
			pmacd_mfpool_t *inboundMFs,
			pmacd_mfpool_t *outboundMFs,
			dma_addr_t streamBufferBlockBusAddr,
			ULONG_T streamBufferBlockSize);

#endif /* _PMACD_LINUX_PCI_H */


















