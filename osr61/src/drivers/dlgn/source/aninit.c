/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : aninit.c
 * Description                  : antares initialization
 *
 *
 **********************************************************************/

#ifndef sun
#ifdef LINUX
#include "dlgclinux.h"
#else
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/immu.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <sys/stream.h>
#include <sys/cmn_err.h>
#endif

#include "dlgcos.h"
#include "include.h"
#ifdef BIG_ANT
#include "byteswap.h"
#endif

#include "antpci.h"

#define IPL4 		      0x0004

#ifdef SVR3
#include <sys/devreg.h>
#include <sys/ci/ciintr.h>
#include <sys/pci.h>

extern int antvec;	/* from Space.c */

#ifdef ANT_CNVRTR_BRD
void PlxInterruptReset(BYTE *PlxVirtAddr);
#endif /* ANT_CNVRTR_BRD */

void
anpmintr(int vec) { anintr(0); }

#ifdef PCI_INTR_DEBUG
void anpmPintr0(int vec) { anpciintr(0, 0); }
void anpmPintr1(int vec) { anpciintr(0, 1); }
void anpmPintr2(int vec) { anpciintr(0, 2); }
void anpmPintr3(int vec) { anpciintr(0, 3); }
void (*apciintrs[])() = { anpmPintr0, anpmPintr1, anpmPintr2, anpmPintr3 };
#endif /* PCI_INTR_DEBUG */

/***************************************************************************
 *        NAME: static BYTE *an_getvirtmem(dip)
 * DESCRIPTION: This function maps the physical address to the virtual
 *            : address.
 *      INPUTS: paddr - Physical Address.
 *            : size  - Amount of physical memory.
 *     OUTPUTS: Nothing.
 *     RETURNS: Virtual address or NULL, if map fails.
 *       CALLS: sptalloc() or physmap()
 *    CAUTIONS: None.
 ***************************************************************************/
static BYTE *
an_getvirtmem(ULONG paddr, ULONG size)
{
   BYTE *vaddr;
   int an_offset; /* offset, if memory mapped addr not at 4k boundary */

   /*
    * Save the offset of the address before calculating the segment.
    * The virtual address will be obtained at the segment boundary
    * since sptalloc operates at page boundaries (4K).  We will add
    * the offset value to size to obtain memory upto the last possible
    * address to be used.
    */
   an_offset = paddr & 0x01FFF;
   paddr &= 0xFFFFE000; /* must be at page boundary */
   size += an_offset;
   if ((vaddr = (BYTE *)sptalloc(btoc(size), PG_P | PG_PCD, btoc(paddr),
   	NOSLEEP)) == NULL)
   {
      cmn_err(CE_WARN, "anpm: Cannot map 0x%x bytes at segment 0x%x",
	size, paddr);
   }
   vaddr += an_offset;

   return vaddr;
}


/***************************************************************************
 *        NAME: static VOID an_rlsvirtmem(vaddr, size)
 * DESCRIPTION: This function releases the virtual address mapped earlier.
 *      INPUTS: vaddr - Virtual address.
 *            : size  - Size of virtual memory.
 *     OUTPUTS: Nothing.
 *     RETURNS: Nothing.
 *       CALLS: sptfree() or physmap_free()
 *    CAUTIONS: None.
 ***************************************************************************/
static VOID
an_rlsvirtmem(BYTE *vaddr, ULONG size)
{
   int an_offset; /* offset, if memory mapped addr not at 4k boundary */

   /*
    * Before freeing the virtual address space we need to retrace back to
    * the address and size we had used to allocate.
    */
   an_offset = (ULONG)vaddr & 0x01FFF;
   vaddr -= an_offset;
   size  += an_offset;
   sptfree(vaddr, btoc(size), 0);
}
#endif /* SVR3 */

/***********************************************************************
 *        NAME: PlxInterruptEnable
 * DESCRIPTION: Write to PCI board's Plx interrupt control/status register
 *              to enable PCI interrupts.
 *              For VME board, this is done by the firmware.
 *      INPUTS: config id
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 * CALLED FROM: GpInterruptEnable, GpProbePciBoard
 *  LOGIC FLOW:
 *		- get plx virt addr using config id
 *		- calculate addr for the plx interrupt control/status reg
 *		- read plx interrupt control/status reg
 *		- write plx interrupt control/status reg or'ing enable bits
 *    CAUTIONS:
 **********************************************************************/
void
PlxInterruptEnable(
BYTE *PlxVirtAddr)
{
	BYTE *IntrRegAddr;
	BYTE IntrReg;

	IntrRegAddr = PlxVirtAddr + PCI_PLX_INTR_REG;
	IntrReg = *IntrRegAddr;

        /* Setting bit7 to 1 panics the system = 0x81 */
        if (!IntrReg) {
            IntrReg = 0x41;
        } else {
            IntrReg = 0x41;
	    /* IntrReg |= PCI_PLX_INTR_ENABLE_BIT ; */
        }
	*IntrRegAddr = IntrReg;
}

/***********************************************************************
 *        NAME: PlxInterruptDisable
 * DESCRIPTION: Write to PCI board's Plx interrupt control/status register
 *              to disable PCI interrupts.
 *              For VME board, this is done by the firmware.
 *      INPUTS: config id
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 * CALLED FROM: GpInterruptDisable, GpPciAntCleanup
 *  LOGIC FLOW:
 *		- get plx virt addr using config id
 *		- calculate addr for the plx interrupt control/status reg
 *		- read plx interrupt control/status reg
 *		- write plx interrupt control/status reg and'ing disable bits
 *    CAUTIONS:
 **********************************************************************/
void
PlxInterruptDisable(
BYTE *PlxVirtAddr)
{
	BYTE *IntrRegAddr;
	BYTE IntrReg;

	IntrRegAddr = PlxVirtAddr + PCI_PLX_INTR_REG;
	IntrReg = *IntrRegAddr;

	if (!IntrReg) {
		*IntrRegAddr = 0x40;
	} else {
		*IntrRegAddr = IntrReg & 0x40;
        }
}

#ifdef SVR3

#ifdef ANT_CNVRTR_BRD
/* The following code is for Antares support.
 * It's not ifdef'ed out for easier maintainance.
 */
void
PlxInterruptReset(
BYTE *PlxVirtAddr)
{
  volatile BYTE *vaddr1, *vaddr2;
  BYTE    Toggle, ToggleValue = 0;

  vaddr2 = (BYTE *)PlxVirtAddr;
  vaddr1 = vaddr2 = vaddr2 + PCI_PLX_FF_REG;

  ToggleValue = *vaddr2;

  if (!ToggleValue) { /* 9050 7th bit problem - could read all 0's */
	Toggle = BYTERESETFLIPFLOP;
  } else {
	Toggle = ToggleValue & 0xFB;
  }

  *vaddr2 = Toggle;

  ToggleValue = *vaddr1;

  if (!ToggleValue) { /* 9050 7th bit problem - could read all 0's */
	Toggle = BYTESETFLIPFLOP;
  } else {
	Toggle = ToggleValue | 0x04;
  }

  *vaddr1 = Toggle;
}
#endif /* ANT_CNVRTR_BRD */

/***********************************************************************
 *        NAME: anpci_init
 * DESCRIPTION: init routine for driver's pci devices.
 *		Get all pci params, and reg intr.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
anpci_init(void)
{
	SHORT indx;
	struct pci_businfo businfo;
	struct pci_devinfo devinfo;
	driver_info dip = { 0 }; 
	ULONG address, regaddr, cfgaddr;
	BYTE *virtcfg;
        BYTE int_line;
	int id, i = 0;	
	int oldspl;

	/* Must look for PCI devices first because it is possible
	 * that a device on a PCI bus may have an IO base address
	 * in the EISA/ISA range.
	 */
	/* Now setup PCI interrupts, if any present */
	if (pci_buspresent(&businfo)) {
	  for (indx = 0; indx < PCI_MAX_BOARDS; indx++) {
		if (pci_search(ANT_VENDID, ANT_DEVID, 0xffff, 0xffff, indx, &devinfo) != 1) {
#ifdef PCI_DEBUG
		if (!indx)
		cmn_err(CE_CONT, "No Antares PCI boards present in the system.\n");
#endif
			break;
		}

#ifdef PCI_DEBUG2
		cmn_err(CE_CONT, "PCI : busnum = %x, funcnum = %x, slot = %d\n",
			devinfo.busnum, devinfo.funcnum, devinfo.slotnum);
#endif
		if (pci_readdword(&devinfo, PCI_CONF_SUBVENID, &address) == 0) {
	           cmn_err(CE_WARN, "Config Address failure!");
	           break;
	        }
		if (address != PCI_ANT_ID) {
			continue;
		}

		if (pci_readdword(&devinfo, PCI_CONF_BASE0, &address) == 0) {
	           cmn_err(CE_WARN, "Config Address failure!");
	           break;
	        }
                cfgaddr = address;
		virtcfg = an_getvirtmem(cfgaddr, PCI_PLX_CFG_SIZE);
		if (!virtcfg) {
		   break;
		}

	   	if (pci_readdword(&devinfo, PCI_CONF_BASE2, &address) == 0) {
	           cmn_err(CE_WARN, "Failed I/O Access");
	           break;
	        }
                regaddr = address & ~1;	/* remove the i/o bit */

	   	if (pci_readbyte(&devinfo, PCI_CONF_ILINE, &int_line) == 0) {
	           cmn_err(CE_WARN, "No Interrupt defined!");
	           break;
	        }

		/* regaddr is an i/o addr, and cfgaddr is memory mapped */
		id = GetIdForAntPci(regaddr, virtcfg);

		dip.name = "anpm";
		dip.weight = IPL4;
		dip.ipl = IPL4;
		dip.route = IROUTE_GLOBAL;
		dip.processor_mask = 1; /* non-multi-threaded */

		dip.bus = devinfo.busnum;
		dip.mode = IMODE_SHARED_CDRIVERIPL;

		dip.irq = IDIST_PCI_IRQ(devinfo.slotnum, devinfo.funcnum);
		dip.version = DRV_INFO_VERS_2; /* PCI device */
#ifdef PCI_INTR_DEBUG
		/* dip.intr = (int (*)())anpmPintr; */
		dip.intr = (int (*)())apciintrs[id];
#else
		dip.intr = (int (*)())anpmintr;
#endif

#ifdef PCI_DEBUG2
	        cmn_err(CE_CONT, "Ant%d: Cfg=%x(%x), io=%x, IRQ=%d(0x%x)\n",
			i, cfgaddr, virtcfg, regaddr, int_line, dip.irq); 
#ifdef ANT_CNVRTR_BRD
	        cmn_err(CE_CONT, "Antares converter board\n");
#endif /* ANT_CNVRTR_BRD */
#endif

		DLGCSPLSTR(oldspl);
		idistributed(&dip);
		DLGCSPLX(oldspl);

		PlxInterruptEnable(virtcfg);

    		printcfg("anpm", regaddr, 16, int_line, -1, 
		     "Antares PCI" \
		     ", %d/%d/%d, %x",
		     devinfo.busnum, devinfo.slotnum, id, virtcfg);

	        i++;
	  } /* pci_search for PCI MAX devices */
	} /* pci bus present */
	return;
}

/***********************************************************************
 *        NAME: anisa_init
 * DESCRIPTION: init routine for driver's isa devices.
 *		Register ISA interrupt.
 *      INPUTS: ISA irq
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
anisa_init(int antvec)
{
	driver_info dip = { 0 }; 
	int oldspl;

	/* Now, setup ISA interrupt, if one is present */
	if (antvec) {
		dip.name = "anpm";
		dip.weight = IPL4;
		dip.ipl = IPL4;
		dip.route = IROUTE_GLOBAL;
		dip.processor_mask = 1; /* non-multi-threaded */

		dip.irq = antvec; 
		dip.version = DRV_INFO_VERS_1; /* non-PCI device */
		dip.mode = IMODE_SHARED_DRIVER; /* ISA sharing */
		dip.mode = IMODE_SHARED_CDRIVERIPL;
		dip.intr = (int (*)())anpmintr;

		DLGCSPLSTR(oldspl);
		idistributed(&dip);
		DLGCSPLX(oldspl);

    		printcfg("anpm", 0, 0, antvec, -1, 
		     "Antares ISA");
	}

	return;
}

/***********************************************************************
 *        NAME: aninit
 * DESCRIPTION: init routine for the driver.
 *      INPUTS: none (antvec from Space.c)
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
aninit(void)
{
	anpci_init();
	anisa_init(antvec);
}

/***********************************************************************
 *        NAME: anhalt
 * DESCRIPTION: halt routine for driver.
 *		Disable interrupts and Freeup virtual memory.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
anhalt(void)
{
	int i, num;
	ULONG list[PCI_MAX_BOARDS];

	all_pci_cfg_virt_addr(&num, &list[0]);
	for (i = 0; i < num; i++) {
		PlxInterruptDisable((BYTE *)list[i]);
		an_rlsvirtmem((BYTE *)list[i], PCI_PLX_CFG_SIZE);
	}
}

#endif /* SVR3 */
#endif /* sun */
