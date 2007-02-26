/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_linux_pci.c
//
// Defines functions that deal with pmac pci interface 
// under linux.
// 
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_linux_pci.h"
#include "pmacd_board.h"
#include "pmacd_driver.h"
#include "21554.h"

extern int RH_Enabled;

/*
 *  FunctionName:  pmacd_updatePCIConfigRom()
 *
 *        Inputs:  pBoard - pointer to a board that needs the rom updated.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Updates the PCI config rom with the proper values for a 
 *                 particular board.
 *                 
 *      Comments:  
 */
void pmacd_updatePCIConfigRom(pmacd_pcibusdevice_t *pci){
  UINT16_T flags;
  struct pci_dev *pciDev;  
  pmacd_pciconfigrom_t *configRom;
  
  configRom = &(pci->configRom);
  pciDev =   pci->pciDev;

  
  // Copy stored versions of the IRQ and Bars into the ROM.
  // -- these were stored at the first load of the driver.
  // -- This is cross platform, but if the driver is unloaded and reloaded
  // -- while a board is out of the system, junk will be written to the 
  // -- config rom.
  pci_write_config_byte (pciDev, PCI_INTERRUPT_LINE, configRom->interruptLine);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_0,  configRom->bar[0]);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_1,  configRom->bar[1]);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_2,  configRom->bar[2]);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_3,  configRom->bar[3]);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_4,  configRom->bar[4]);
  pci_write_config_dword(pciDev, PCI_BASE_ADDRESS_5,  configRom->bar[5]);
  pci_write_config_byte(pciDev,  PCI_CACHE_LINE_SIZE, configRom->cacheLineSize);
  pci_write_config_byte(pciDev,  PCI_LATENCY_TIMER,   configRom->latencyTimer);


  // Check if Fast Back2Back transactions are supported. If so turn it on.
  pci_read_config_word(pciDev, PCI_STATUS, &flags);
  if(flags & PCI_STATUS_FAST_BACK)flags = PCI_COMMAND_FAST_BACK;
  
  flags |= PCI_COMMAND_IO |  // Turn on IO bus translation
    PCI_COMMAND_MEMORY |     // Turn on Memory bus translation
    PCI_COMMAND_MASTER |     // Turn on Bus Master enabled.
    PCI_COMMAND_INVALIDATE | // Turn on Memory Write and Invalidate Commands
    PCI_COMMAND_PARITY |     // Turn on Parity detection 
    PCI_COMMAND_SERR;        // Turn on SERR notification
  pci_write_config_word(pciDev, PCI_COMMAND, flags);
}


/*
 *  FunctionName:  pmacd_setupPciBar2()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *      Comments:  
 */
void  pmacd_setupPciBar2(pmacd_pcibusdevice_t *bus, 
			 pmacd_mfpool_t *inboundMFs,
			 pmacd_mfpool_t *outboundMFs,
			 dma_addr_t streamBufferBlockBusAddr,
			 ULONG_T streamBufferBlockSize){
  
  int pageSize = 64*1024;  // Paging unit pages set to 64K
  volatile ULONG_T lRegister , lRegister2;
  UINT16_T control;
  int j,i;
  struct pci_dev *pciDev;
  ULONG_T baseAddr = (ULONG_T)bus->csrMappedBaseAddress;
  
  pmacd_updatePCIConfigRom(bus);
  pciDev = bus->pciDev;
  
  // SETUP INBOUND MFs
  // Compute how many pages in the look up table the MFs take up.
  i = (inboundMFs->count * inboundMFs->size) + pageSize - 1;
  i = i/ pageSize;
  j=i;
  baseAddr += UPSTREAM_MEM2_LOOKUPTBL;
  lRegister2 = inboundMFs->mfBlockStartBusAddr; 

  while(j){
    // read present value of the page entry
#ifdef LFS
    lRegister = readl((caddr_t)baseAddr);
#else
    lRegister = readl(baseAddr);
#endif
    
    // clear all but the reserved bits
    lRegister &= UPSTREAM_MEM2_RESERVED;  
    
    // Set Address Bits
    lRegister |= lRegister2 & UPSTREAM_MEM2_ADDRESS_MASK;
    
    // Set Control Bits
    lRegister |= UPSTREAM_MEM2_PREFETCH | UPSTREAM_MEM2_VALID;
    
    // Write register back
#ifdef LFS
    writel(lRegister, (caddr_t)baseAddr);
#else
    writel(lRegister, baseAddr);
#endif
    
    j--;
    baseAddr += 4; // Increment double word size
    lRegister2 += pageSize; 
  }
  
  // SETUP OUTBOUND MFs
  j = (outboundMFs->count * outboundMFs->size) + pageSize - 1;
  j = j / pageSize;
  i+=j;
  lRegister2 = outboundMFs->mfBlockStartBusAddr; 
  while(j){
    // read present value of the page entry
#ifdef LFS
    lRegister = readl((caddr_t)baseAddr);
#else
    lRegister = readl(baseAddr);
#endif
    
    // clear all but the reserved bits
    lRegister &= UPSTREAM_MEM2_RESERVED;  
    
    // Set Address Bits
    lRegister |=  lRegister2 & UPSTREAM_MEM2_ADDRESS_MASK;

    // Set Control Bits
    lRegister |= UPSTREAM_MEM2_PREFETCH | UPSTREAM_MEM2_VALID;
    
    // Write register back
#ifdef LFS
    writel(lRegister, (caddr_t)baseAddr);
#else
    writel(lRegister, baseAddr);
#endif

    j--;
    baseAddr += 4; // Increment double word size
    lRegister2 += pageSize; 


  }

  if(streamBufferBlockBusAddr && streamBufferBlockSize){
    // SETUP STREAM BUFFERS
    j = (streamBufferBlockSize) + pageSize - 1;
    j = j / pageSize;
    i+=j;
    lRegister2 = streamBufferBlockBusAddr; 
    while(j){
      // read present value of the page entry
#ifdef LFS
      lRegister = readl((caddr_t)baseAddr);
#else
      lRegister = readl(baseAddr);
#endif
      
      // clear all but the reserved bits
      lRegister &= UPSTREAM_MEM2_RESERVED;  
    
      // Set Address Bits
      lRegister |= lRegister2 & UPSTREAM_MEM2_ADDRESS_MASK;

      // Set Control Bits
      lRegister |=
	UPSTREAM_MEM2_PREFETCH | UPSTREAM_MEM2_VALID;
    
      // Write register back
#ifdef LFS
      writel(lRegister, (caddr_t)baseAddr);
#else
      writel(lRegister, baseAddr);
#endif

      j--;
      baseAddr += 4; // Increment double word size
      lRegister2 += pageSize; 
    }
  }
	
  // SETUP THE EXTRA ENTRIES TO INVALID
  while(i <= UPSTREAM_MEM2_LOOKUPTBL_SIZE){
#ifdef LFS
    lRegister = readl((caddr_t)baseAddr);
#else
    lRegister = readl(baseAddr);
#endif
    lRegister &= UPSTREAM_MEM2_RESERVED;  
    lRegister |= ~UPSTREAM_MEM2_VALID;
#ifdef LFS
    writel(lRegister, (caddr_t)baseAddr);
#else
    writel(lRegister, baseAddr);
#endif


    i++;
    baseAddr += 4;
  }
  
	
  // Setup Chip control for 64K page size for Bar2 and the I2O unit
  // for 256 entries.  (these should be made configurable.)
  pci_read_config_word(pciDev, CHIP_CONTROL_1_REG, &control);
  control &= ~(UINT16_T)(UPSTREAM_MEM2_PAGESIZE_MASK|I2O_ENABLE_MASK|I2O_SIZE_MASK);
  control |= (UPSTREAM_MEM2_PAGESIZE_64K|I2O_ENABLE|I2O_SIZE_256);
  pci_write_config_word(pciDev, CHIP_CONTROL_1_REG, control);
}


/*
 *  FunctionName:  pmacd_printPCIBusDev()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *      Comments:  
 */
void pmacd_printPCIBusDev(pmacd_pcibusdevice_t *pci){
  
  pmacd_printDebug("PCI Dev Bus Info:\n"); 
  pmacd_printDebug(" OS Assigned: IRQ:%i CSRBaseAddr:0x%lx ConfigBaseAddr:0x%lx\n",
		   pci->irq,
		   pci->csrBaseAddress,
		   pci->configBaseAddress);
  
  pmacd_printDebug(" ROM: IRQ:%i BaseAddr0:0x%x BaseAddr1:0x%x BaseAddr2:0x%x\n",
		   pci->configRom.interruptLine,
		   pci->configRom.bar[0],
		   pci->configRom.bar[1],
		   pci->configRom.bar[2]);

  pmacd_printDebug("            BaseAddr3:0x%x BaseAddr4:0x%x BaseAddr5:0x%x\n",
		   pci->configRom.bar[3],
		   pci->configRom.bar[4],
		   pci->configRom.bar[5]);
  
  pmacd_printDebug("            LatencyTimer:%i CacheLineSize:%i\n",
		   pci->configRom.latencyTimer,
		   pci->configRom.cacheLineSize);
}


/*
 *  FunctionName:  pmacd_configurePCIDevices()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *      Comments:  
 */
#if 0
BOOLEAN_T pmacd_configurePCIDevices(void){
  struct pci_dev *pciDev = NULL;
  UINT16_T subVendorId;
  UINT16_T subDeviceId;
  
  // Find and configure PCI Devices
  if(pci_present()){
    while((pciDev=pci_find_device(PMACD_VENDOR_ID_21554,PMACD_DEVICE_ID_21554,pciDev))){
      pci_read_config_word(pciDev, PCI_SUBSYSTEM_VENDOR_ID, &subVendorId);
      pci_read_config_word(pciDev, PCI_SUBSYSTEM_ID, &subDeviceId);
      
      if(subVendorId != PMACD_SUBSYS_VENDOR_ID ||
	 subDeviceId != PMACD_SUBSYS_DEVICE_ID){
	continue;
      }
      
      if(!pmacd_configurePCIDevice(pciDev)){
	// If the board was not allocated or could not be added to the driver
	// return that the configuration failed. Boards configured up to this
	// point will be cleaned up on driver unload.
	return(FALSE);
      }
    }
  }else{
    // PCI bus is not present on this system. No boards can be found.
    // No cleanup necessary just return failure.
    pmacd_printDebug("This is not a PCI system\n");
    return(FALSE);
  }

  return(TRUE);
}
#endif


/*
 *  FunctionName:  pmacd_configurePCIDevice()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *      Comments:  
 */
BOOLEAN_T pmacd_configurePCIDevice(struct pci_dev *pciDev){
  pmacd_board_t *pBoard;
  UINT16_T subVendorId;
  UINT16_T subDeviceId;
  ULONG_T baseAddr;
  pmacd_hs_t *hsd = NULL;
  
  hsd = (pmacd_hs_t*)pci_get_drvdata(pciDev);
  if (hsd == NULL) {
      pmacd_printDebug("pmacd_configurePCIDevice -> error retreiving hsd object\n");
      return(FALSE);
  }
 
  pci_read_config_word(pciDev, PCI_SUBSYSTEM_VENDOR_ID, &subVendorId);
  pci_read_config_word(pciDev, PCI_SUBSYSTEM_ID, &subDeviceId);
#ifdef DEBUG
  pmacd_printDebug("pmacd_configurePCIDevice: streamsPerBoard=0x%x, buffersPerStream=0x%x,  bufferSize=0x%x\n",  
	pmacd_driver.parms.streamConfig.streamsPerBoard,   
        pmacd_driver.parms.streamConfig.buffersPerStream,  
        pmacd_driver.parms.streamConfig.bufferSize);
  pmacd_printDebug("PCI Device: Vendor:0x%x Device:0x%x\n",
		   pciDev->vendor,
		   pciDev->device);
  
  pmacd_printDebug(" SubVendor:0x%x SubDevice:0x%x\n",
		   subVendorId,
		   subDeviceId);
      
  pmacd_printDebug(" Bus:%i Slot:%i Class:0x%x\n",
		   pciDev->bus->number,
		   PCI_SLOT(pciDev->devfn),
		   pciDev->class);
      
  pmacd_printDebug(" PCI Base Addrs: 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",
		   pci_resource_start(pciDev,0),
		   pci_resource_start(pciDev,1),
		   pci_resource_start(pciDev,2),
		   pci_resource_start(pciDev,3),
		   pci_resource_start(pciDev,4),
		   pci_resource_start(pciDev,5));
  
  pmacd_printDebug(" PCI Base Addrs Flags: 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",
		   pci_resource_region_flags(pciDev,0),
		   pci_resource_region_flags(pciDev,1),
		   pci_resource_region_flags(pciDev,2),
		   pci_resource_region_flags(pciDev,3),
		   pci_resource_region_flags(pciDev,4),
		   pci_resource_region_flags(pciDev,5));
 #endif 
  MUTEX_ENTER(pmacd_driver.mutex);
  pBoard = pmacd_allocateBoard(pmacd_getFreeBoardId(),
			       pciDev->bus->number,
			       PCI_SLOT(pciDev->devfn),
			       0,
			       pmacd_BusType_Pci,
			       &(pmacd_driver.parms.msgUnitConfig),
			       &(pmacd_driver.parms.streamConfig));
  
  // If the board was not allocated or could not be added to the driver
  // return that the configuration failed. Boards configured up to this
  // point will be cleaned up on driver unload.
  if(!(pBoard != NULL &&
       pmacd_addBoardToDriver(pBoard))){
    MUTEX_EXIT(pmacd_driver.mutex);
    return(FALSE);
  }

  hsd->pBoard = pBoard;

  if ( pBoard->inboundMFs->mfBlockStart ) {
     pBoard->inboundMFs->mfBlockStartBusAddr = pci_map_single(pciDev, pBoard->inboundMFs->mfBlockStart, (pmacd_driver.parms.msgUnitConfig.inboundQueueSize * pmacd_driver.parms.msgUnitConfig.msgSize), PCI_DMA_BIDIRECTIONAL);
  }

  if ( pBoard->outboundMFs->mfBlockStart ) {
     pBoard->outboundMFs->mfBlockStartBusAddr = pci_map_single(pciDev, pBoard->outboundMFs->mfBlockStart, (pmacd_driver.parms.msgUnitConfig.outboundQueueSize * pmacd_driver.parms.msgUnitConfig.msgSize), PCI_DMA_BIDIRECTIONAL);
  }

  if ( pBoard->streams.streamBuffers ) {
     pBoard->streams.streamBuffersBusAddr = pci_map_single(pciDev, pBoard->streams.streamBuffers, pmacd_driver.parms.streamConfig.bufferSize, PCI_DMA_BIDIRECTIONAL);
  }

  MUTEX_EXIT(pmacd_driver.mutex);
  
  // Configure Bus Data
  pBoard->bus.pciDev = pciDev;
  
  pBoard->bus.configBaseAddress = 
    pci_resource_start(pciDev,PMACD_CONFIGBLOCK_BAR); // Downstream Memory 2 bar
 
  pBoard->bus.csrBaseAddress = 
    pci_resource_start(pciDev,PMACD_CSR_BAR);      
  
  // Map the shared memory into the virtual address
  // space in order to be able to properly access it.
  pBoard->bus.configMappedBaseAddress =
    ioremap(pBoard->bus.configBaseAddress, 
	    PMACD_CONFIGBLOCK_SIZE);
  
  pBoard->bus.csrMappedBaseAddress = 
    ioremap(pBoard->bus.csrBaseAddress, PMACD_CSR_SIZE);
  
  pBoard->bus.irq = pciDev->irq;
  
  
  // SETUP MFA Queue addresses for the inbound and outbound MFs.
  baseAddr = (ULONG_T) pBoard->bus.csrMappedBaseAddress;
  pBoard->inboundMFs->mfaQueueAddress = baseAddr + I2O_INBOUND_QUEUE_REG;
  pBoard->outboundMFs->mfaQueueAddress = baseAddr + I2O_OUTBOUND_QUEUE_REG;
  
  
  // Store raw config rom data.
  pci_read_config_byte(pciDev, PCI_INTERRUPT_LINE, 
		       &(pBoard->bus.configRom.interruptLine));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_0, 
			&(pBoard->bus.configRom.bar[0]));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_1, 
			&(pBoard->bus.configRom.bar[1]));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_2, 
			&(pBoard->bus.configRom.bar[2]));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_3, 
			&(pBoard->bus.configRom.bar[3]));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_4, 
			&(pBoard->bus.configRom.bar[4]));
  pci_read_config_dword(pciDev, PCI_BASE_ADDRESS_5, 
			&(pBoard->bus.configRom.bar[5]));
  pci_read_config_byte(pciDev,  PCI_CACHE_LINE_SIZE, 
		       &(pBoard->bus.configRom.cacheLineSize));
  pci_read_config_byte(pciDev,  PCI_LATENCY_TIMER,   
		       &(pBoard->bus.configRom.latencyTimer));
  if (!RH_Enabled)
     pmacd_hardResetBoard(pBoard,FALSE);
  
  return(TRUE);
}



