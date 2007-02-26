/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antpci.c
 * Description                  : antares PCI module 
 *
 *
 **********************************************************************/

#ifdef PCI_ANT
#ifdef WNT_386
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stdarg.h>
#include <stream.h>
#include <stropts.h>
#include <cmn_err.h>
#else
#ifdef LINUX
#include "dlgclinux.h"
#else
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#ifndef AIX_ANT
#include <sys/cmn_err.h>
#else
#include "ant_aix.h" 
#endif
#endif /* LINUX */
#endif
#if (defined(SVR4) || defined(UNISYS_MP) || defined(UNISYS_UP))
    #ifndef SE_NOSLP
        #ifdef SVR4
            #ifndef AIX_ANT
            #include <sys/kmem.h>
#ifndef SVR5
            #include <sys/strsubr.h>
#endif /* SVR5 */
            #endif
        #else
        #define SE_NOSLP 1
        #endif
    #endif
#endif


/* Generic Driver */
#include "gndefs.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndlgn.h"
#include "gnlibdrv.h"
#if (defined(SOL_ANT) || defined(AIX_ANT))
#include "gndbg.h"
#include "gncfd.h"
#endif
#include "include.h"
#include "antares.ver"		 

/* from intrface sub directory */
#ifdef LFS
#include "extvar.h"
#else
#include "var.h"
#endif
#include "intrface.h"
#include "anprot.h"

/* from protocol sub_directory */
#include "protocol.h"
#ifdef LFS
#include "extern.h"
#else
#include "antglob.h"
#endif
#include "drvprot.h"

/* from hardware sub directory */
#include "hardexp.h"

/* Antares Device Access Module */
#include "anlibdrv.h"
#include "anstart.h"
#include "andrv.h"
#include "andam.h"

#ifdef WNT_ANT
#include "ant_wnt.h"
VOID cmn_err(ULONG,...);
#endif

#ifdef BIG_ANT
#include "byteswap.h"
#else
#define LBYTESWAP(w) w
#define WBYTESWAP(w) w
#endif /* BIG_ANT */

#define MAX_ID_NUM	16
#define	A4_HIGH_ADDRESS 6
#define	A4_SUB_ADDRESS 2

static ULONG AntVirtAddr[MAX_ID_NUM] = { 0 };
static ULONG AntPciCfgVirtAddr[MAX_ID_NUM] = { 0 };

SHORT numPciBoards = 0;
int validate_pci_port(ushort port);
void AntPciIntrReset(WORD port);
void AntAddrSet(volatile char *vaddr, char *ioaddr);
ULONG get_virt_port(ushort port);
void SetPciIo32Mode(ushort port, int Flag);

#ifdef LFS
VOID wctrl_isr(WORD port, BYTE data);
#endif

/* Routine to convert edge-to-level interrupt reset.
 * This routine is called with mutex/lock owned.
 * So, do not call any routines that does it again,
 * to avoid recurssive mutex/lock panic.
 */
void
AntPciIntrReset(WORD port)
{
#ifndef LFS
	volatile char   *addr;
#endif

#define	ANT_PCI_EDGE_ADDR	0x00C00100

#ifdef ANT_CNVRTR_BRD
#ifdef SPARC_PCI
    	dlgn_intr_reset(get_pci_cfg_virt_addr(port));
#else
    	PlxInterruptReset(get_pci_cfg_virt_addr(port));
#endif /* SPRAC_PCI */
#else
#ifdef DUAL_FLIP_FLOPS /* support both, single and dual flip-flops */
	addr = (char *)get_virt_port(port);
	AntAddrSet(addr, (char *)ANT_PCI_EDGE_ADDR);
	addr += (2 * A4_SUB_ADDRESS);	/* data reg */
#ifdef LINUX
        outw(1, addr); /* write anything */
#else
        outw(addr, 1); /* write anything */
#endif /* LINUX */
#endif /* DUAL_FLIP_FLOPS */
	/* single flip-flops */
	clear_irq_isr(port);   /* Clear the interrupt */
#endif /* ANT_CNVRTR_BRD */
}

void
AntResetAllPciBoards(void)
{
	int id;
	WORD port;

	for (id = 0; id < MAX_ID_NUM; id++) {
		if (AntVirtAddr[id]) {
			static unsigned int m = 0;

			port = (id << 4) & 0xff;
			if (is_irq(port)) {
				static unsigned int n = 0;

				if (n++ < 5)
				cmn_err(0, "irq set for %d ", id);
    				clear_irq(port);   /* Clear the interrupt */
			}
			if (m++ < 5)
			cmn_err(0, "Reset irq level for port %x\n", port);
			AntPciIntrReset(port);
		}
	}
}


/***********************************************************************
 *        NAME: validate_pci_port
 * DESCRIPTION: This function validates virtual address existence for
 *              the given physical i/o port address.
 *      INPUTS: port - physical i/o port address
 *     OUTPUTS: 
 *     RETURNS: SUCCESS (0) or FAILURE (-1)
 *       CALLS: 
 *   CALLED BY: inw, inl, outw, outl
 *    CAUTIONS: 
 **********************************************************************/
int
validate_pci_port(ushort port)
{
	ushort id;

	id = (port & 0xff) >> 4;

	if ((id >= MAX_ID_NUM) || (!AntVirtAddr[id])) {
#ifdef DBGPRT
		cmn_err(CE_CONT, "PROB: addr for port 0x%x, id 0x%x = 0x%x\n",
			port, id, AntVirtAddr[id]);
#endif
		return(-1);
	}
	SetPciIo32Mode(port, 1);
	return(0);
}

/***********************************************************************
 *        NAME: get_virt_port
 * DESCRIPTION: This function returns virtual port for the given physical
 *              i/o port address.
 *      INPUTS: port - physical i/o port address
 *     OUTPUTS: 
 *     RETURNS: virtual port address
 *       CALLS: 
 *   CALLED BY: inw, inl, outw, outl
 *    CAUTIONS: 
 **********************************************************************/
ULONG
get_virt_port(ushort port)
{
	ULONG virtaddr;
	BYTE offset;

#ifdef ANT_CNVRTR_BRD
	offset = port & 0x7;
#else
	offset = (port & 0x7) << 1;	/* long-word aligned ports */
#endif /* ANT_CNVRTR_BRD */

	/* 
	 * The remap address of the coverter board is 0x200.
	 * For the pci boards, valid port values are from 0 to 0xf7.
	 * Valid offset is 0 to 7 for isa and cnvrtr brd,
	 * and from 0 to 15 for the real pci board.
	 */

	virtaddr = AntVirtAddr[(port & 0xff) >> 4] + offset;

	return(virtaddr);
}

/***********************************************************************
 *        NAME: get_pci_cfg_virt_addr
 * DESCRIPTION: This function returns virtual addr of 9050's local cfg
 *              registers for the given physical i/o port address.
 *      INPUTS: port - physical i/o port address
 *     OUTPUTS: 
 *     RETURNS: virtual port address of 9050's local cfg registers
 *       CALLS: 
 *   CALLED BY: anintr (for converter boards only)
 *    CAUTIONS: 
 **********************************************************************/
ULONG
get_pci_cfg_virt_addr(ushort port)
{
	ULONG cfgvirtaddr;

	cfgvirtaddr = AntPciCfgVirtAddr[(port & 0xff) >> 4];

	return(cfgvirtaddr);
}

void
all_pci_cfg_virt_addr(int *num, ULONG *list)
{
	int i, idx;

	idx = 0;
	for (i = 0; i < MAX_ID_NUM; i++) {
		if (AntPciCfgVirtAddr[i]) {
			list[idx++] = AntPciCfgVirtAddr[i];
		}
	}
	*num = idx;
}

void
AntAddrSet(volatile char *vaddr, char *ioaddr)
{
   ushort haddr, laddr;
   volatile char *addr;

   haddr = (ushort) WBYTESWAP(((ULONG)ioaddr >> 16) & 0xffff);
   laddr = (ushort) WBYTESWAP((ULONG)ioaddr & 0xffff);

#ifdef ANT_CNVRTR_BRD
   addr = vaddr + A4_HIGH_ADDRESS;	/* hi-addr */
#else
   addr = vaddr + (2 * A4_HIGH_ADDRESS);	/* hi-addr */
#endif /* ANT_CNVRTR_BRD */

#ifdef LINUX
#ifdef LFS
   outw(haddr, (int)(long)addr);
#else
   outw(haddr, (int)addr);
#endif
#else
   outw((int)addr, haddr);
#endif /* LINUX */

#ifdef ANT_CNVRTR_BRD
   addr -= A4_SUB_ADDRESS;	/* lo-addr */
#else
   addr -= (2 * A4_SUB_ADDRESS);	/* lo-addr */
#endif /* ANT_CNVRTR_BRD */

#ifdef LINUX
#ifdef LFS
   outw(laddr, (int)(long)addr);
#else
   outw(laddr, (int)addr);
#endif
#else
   outw((int)addr, laddr);
#endif /* LINUX */
}

#ifdef ANT_BRD_DBG
#ifndef FORCE_16_BITS
void
check_data32(ushort port, volatile char *ioaddr)
{
   volatile char *addr;
   ulong data;
   int i, m, f = 0, t, n = 0, prt = 0;
   int start = 0;

t = 0x10; n = 0x40;

   SetPciIo32Mode(port, 1);

 for (m=0; m<t; m++) {
   A4_setAddress(port, (ULONG)ioaddr);
   addr = (char *)get_virt_port(port + A4_SUB_ADDRESS);	/* data reg */

   for (i = 0; i < n; i++) {
   	outl((int)addr, i);	/* write to data reg */
   }
   A4_setAddress(port, (ULONG)ioaddr);
   for (i = 0; i < n; i++) {
   	data = inl((int)addr);	/* read from data reg */
	if (data != i) {
		if (!start++)
			cmn_err(CE_CONT, "32bit:%x: ", ioaddr);
		f++;
		if (((data & i) == i) && ((data >> 8) == 0xff))
   		cmn_err(CE_CONT, "%02x ", i);
		else
   		cmn_err(CE_CONT, "0x%x: %x != %x  ", m, data, i);
	}
   }
#if 0
   A4_setAddress(port, (ULONG)ioaddr);
   for (i = 0; i < n; i++) {
   	data = inl((int)addr);	/* read from data reg */
	if (data != i) {
		if (!start++)
			cmn_err(CE_CONT, "%x: ", ioaddr);
		f++;
		if (prt++ > 200)
			return;
		if (((data & i) == i) && ((data >> 8) == 0xff))
   		cmn_err(CE_CONT, "%02x ", i);
		else
   		cmn_err(CE_CONT, "0x%x: %x != %x  ", m, data, i);
	}
   }
#endif
 }
   if (f)
	cmn_err(CE_CONT, "\n");
}

void
check_auto32(ushort port, volatile char *ioaddr)
{
   volatile char *addr;
   ushort laddr, val;
   ulong dataval, prev;
   int i, m, f = 0, t, b, n = 0;
   int start = 0;

t = 0x10; b = 0x40;

   SetPciIo32Mode(port, 1);

   prev = laddr = (ushort) WBYTESWAP((ULONG)ioaddr & 0xffff);

 for (m=0; m<t; m++) {
   A4_setAddress(port, (ULONG)ioaddr);
   addr = (char *)get_virt_port(port + A4_SUB_ADDRESS);	/* data reg */

   for (i = 1; i < b; i++) {
      dataval = inl((int)addr);	/* read from data reg */

      val = inw((int)(addr + (2*A4_SUB_ADDRESS))); /* read lo addr */
      val = WBYTESWAP(val);

      if (val == prev) {
	n++;
      }
      else if (val != (laddr+i)) {
	if (!m /* && (i==1) */) {
  cmn_err(0, "32:addr %x != %x\n", val, (laddr+i));
	f++;
	}
	else if (val != (prev+i)) {
  cmn_err(0, "32:addr %x != %x (prev)\n", val, (prev+i));
	f++;
	}
      }
      prev = val;
   }
 }
if (n || f)
  cmn_err(CE_CONT, "%32:x: failed %x (%x ffs) of %x times\n", ioaddr, n, f, (t * b));
}
#endif /* not FORCE_16_BITS */

void
check_data(ushort port, volatile char *ioaddr)
{
   volatile char *addr;
   ushort data;
   int i, m, f = 0, t, n = 0, prt = 0;
   int start = 0;

t = 0x10; n = 0x40;

 for (m=0; m<t; m++) {
   A4_setAddress(port, (ULONG)ioaddr);
   addr = (char *)get_virt_port(port + A4_SUB_ADDRESS);	/* data reg */

   for (i = 0; i < n; i++) {
#ifdef LINUX
        outw(i, (int)addr);     /* write to data reg */
#else
        outw((int)addr, i);     /* write to data reg */
#endif /* LINUX */
   }
   A4_setAddress(port, (ULONG)ioaddr);
   for (i = 0; i < n; i++) {
   	data = inw((int)addr);	/* read from data reg */
	if (data != i) {
		if (!start++)
			cmn_err(CE_CONT, "%x: ", ioaddr);
		f++;
		if (prt++ > 100)
			return;
   		cmn_err(CE_CONT, "%x: %x != %x  ", m, data, i);
	}
   }
#if 0
   A4_setAddress(port, (ULONG)ioaddr);
   for (i = 0; i < n; i++) {
   	data = inw((int)addr);	/* read from data reg */
	if (data != i) {
		if (!start++)
			cmn_err(CE_CONT, "%x: ", ioaddr);
		f++;
		if (prt++ > 100)
			return;
   		cmn_err(CE_CONT, "R2:%x: %x != %x  ", m, data, i);
	}
   }
#endif
 }
   if (f)
	cmn_err(CE_CONT, "\n");
}

void
check_auto(ushort port, volatile char *ioaddr)
{
   volatile char *addr;
   ushort val, prev, laddr;
   int i, m, f = 0, t, b, n = 0;
   int start = 0;

t = 0x10; b = 0x40;

   prev = laddr = (ushort) WBYTESWAP((ULONG)ioaddr & 0xffff);

 for (m=0; m<t; m++) {
   A4_setAddress(port, (ULONG)ioaddr);
   addr = (char *)get_virt_port(port + A4_SUB_ADDRESS);	/* data reg */

   for (i = 1; i < b; i++) {
      val = inw((int)addr);	/* read from data reg */
      val = inw((int)addr);	/* read from data reg */

      val = inw((int)(addr + A4_SUB_ADDRESS)); /* read lo addr */
      val = WBYTESWAP(val);

      if (val == prev) {
	n++;
      }
      else if (val != (laddr+i)) {
	if (!m /* && (i==1) */) {
  cmn_err(0, "addr %x != %x\n", val, (laddr+i));
	f++;
	}
	else if (val != (prev+i)) {
  cmn_err(0, "addr %x != %x (prev)\n", val, (prev+i));
	f++;
	}
      }
      prev = val;
   }
 }
if (n || f)
  cmn_err(CE_CONT, "%x: failed %x (%x ffs) of %x times\n", ioaddr, n, f, (t * b));
}
#endif /* ANT_BRD_DBG */

/* Issues related to local config reg params - wait states, etc. */
static void
CnvrtrBrd(unsigned long *a)
{

	/* cfg address is memory mapped */

#ifdef LFS
	   (void)CnvrtrBrd;
	   cmn_err(CE_CONT, "las0rr=%lx, las0ba=%lx, las0brd=%lx\n"
		 "cs0base=%lx, intcsr=%lx, cntrl=%lx\n",
		 LBYTESWAP(a[0]), LBYTESWAP(a[5]), LBYTESWAP(a[0xa]),
		 LBYTESWAP(a[0xf]), LBYTESWAP(a[0x13]), LBYTESWAP(a[0x14]));
#else
	   cmn_err(CE_CONT, "las0rr=%x, las0ba=%x, las0brd=%x\n"
		 "cs0base=%x, intcsr=%x, cntrl=%x\n",
		 LBYTESWAP(a[0]), LBYTESWAP(a[5]), LBYTESWAP(a[0xa]),
		 LBYTESWAP(a[0xf]), LBYTESWAP(a[0x13]), LBYTESWAP(a[0x14]));
#endif

	   if (a[0] != LBYTESWAP(0x0FFFFFF1)) {
	         a[0] = LBYTESWAP(0x0FFFFFF1);
	   }

#if 0
	   /* 94462422 or 94452422 works with reworked ant33 board */
	   /*
	   if (a[0xa] != LBYTESWAP(0x94452422)) {
	         a[0xa] = LBYTESWAP(0x94452422);
	   }
	   */
#endif

#ifdef ANT_CNVRTR_BRD
	   if (a[0x5] != LBYTESWAP(0x201)) {
	         a[0x5] = LBYTESWAP(0x201);
	   }
	   if (a[0xa] != LBYTESWAP(0x94462422)) {
	         a[0xa] = LBYTESWAP(0x94462422);
	   }
	   if (a[0x14] != LBYTESWAP(0xc5496)) {
	         a[0x14] = LBYTESWAP(0xc5496);
	   }
#else
	   if (a[0x5] != LBYTESWAP(0x1)) {
	         a[0x5] = LBYTESWAP(0x1);
	   }
	   if (a[0xa] != LBYTESWAP(0x90762422)) { /* works! = 0x90762422 */
	         a[0xa] = LBYTESWAP(0x90762422);
	   }
	   if (a[0xf] != LBYTESWAP(0x08000001)) {
	         a[0xf] = LBYTESWAP(0x08000001);
	   }
	   if (a[0x14] != LBYTESWAP(0xc5492)) {
	         a[0x14] = LBYTESWAP(0xc5492);
	   }
#endif /* ANT_CNVRTR_BRD */

#ifdef LFS
	   cmn_err(CE_CONT, "las0rr=%lx, las0ba=%lx, las0brd=%lx\n"
		 "cs0base=%lx, intcsr=%lx, cntrl=%lx\n",
		 LBYTESWAP(a[0]), LBYTESWAP(a[5]), LBYTESWAP(a[0xa]),
		 LBYTESWAP(a[0xf]), LBYTESWAP(a[0x13]), LBYTESWAP(a[0x14]));
#else
	   cmn_err(CE_CONT, "las0rr=%x, las0ba=%x, las0brd=%x\n"
		 "cs0base=%x, intcsr=%x, cntrl=%x\n",
		 LBYTESWAP(a[0]), LBYTESWAP(a[5]), LBYTESWAP(a[0xa]),
		 LBYTESWAP(a[0xf]), LBYTESWAP(a[0x13]), LBYTESWAP(a[0x14]));
#endif

}

/***************************************************************************
 *        NAME: SetPciIo32Mode(port, flag)
 * DESCRIPTION: This function sets the 9050 to perform 32bit or 16bit i/o.
 *		Bit fields have to be used due to 9050's 7th bit bug, due
 *		do which, read and then and/or can't be done, and we do not
 *		want to change other bits in the registers.
 *      INPUTS: port, Flag (1=32bit, 0=16bit)
 *     OUTPUTS: None
 *     RETURNS: Nothing.
 *       CALLS: Nothing.
 *   CALLED BY: 
 *    CAUTIONS: None.
 ***************************************************************************/
typedef struct {
#ifdef BIG_ANT
	ULONG B0_7:     8;
	ULONG B12_15:   4;
	ULONG u310_11:	2;	/* User i/o 3 of cntrl */
	ULONG B8_9:	2;
	ULONG bw22_23:  2;      /* Bus Width of las0brd */
	ULONG B16_21:   6;
	ULONG B24_31:   8;
#else
	ULONG B0_7:     8;
	ULONG B8_9:	2;
	ULONG u310_11:	2;	/* User i/o 3 of cntrl */
	ULONG B12_15:   4;
	ULONG B16_21:   6;
	ULONG bw22_23:  2;      /* Bus Width of las0brd */
	ULONG B24_31:   8;
#endif
} LAS_CNTRL;


void
SetPciIo32Mode(ushort port, int Flag)
{
	unsigned long *a;
	LAS_CNTRL *las0brd, *cntrl;

	a = (ULONG *)get_pci_cfg_virt_addr(port);
	las0brd = (LAS_CNTRL *)&a[0xa];
	cntrl = (LAS_CNTRL *)&a[0x14];

#ifndef FORCE_16_BITS	/* only 16-bit mode allowed */
	if (Flag && !(port & _A4_16BIT_PORT))
	{
	  if ((ULONG)a & 0x80) { /* 7th bit set - problem!! */
		a[0xa] = LBYTESWAP(0x90b62422); /* 32-bit bus width mode */
		a[0x14] = LBYTESWAP(0xc5c92);	/* antares 32 bit mode */
	  }
	  else {
		las0brd->bw22_23 = 2; /* 32-bit bus width mode */
		cntrl->u310_11 = 3;	/* antares 32 bit mode */
	  }
	}
	else
#endif
	{
	  if ((ULONG)a & 0x80) { /* 7th bit set - problem!! */
		a[0xa] = LBYTESWAP(0x90762422); /* 16-bit bus width mode */
		a[0x14] = LBYTESWAP(0xc5492);	/* antares 16 bit mode */
	  }
	  else {
		las0brd->bw22_23 = 1; /* 16-bit bus width mode */
		cntrl->u310_11 = 2;	/* antares 16 bit mode */
	  }
	}
}

/***************************************************************************
 *        NAME: int GetIdForAntPci(vaddr, CfgVirtAddr)
 * DESCRIPTION: This function reads the board Id from the
 *              appropriate memory locations of Antares PCI boards, and
 *              stores virtual addresses of the i/o space and local cfg
 *              space at the board id location as the index.
 *      INPUTS: vaddr - Virtual address of the i/o space and local cfg space
 *     OUTPUTS: None
 *     RETURNS: Board Id of the Antares PCI Board.
 *       CALLS: Nothing.
 *   CALLED BY: probe/load entry point of the dlgn driver
 *    CAUTIONS: None.
 ***************************************************************************/
int
GetIdForAntPci(volatile char *vaddr, char *CfgVirtAddr)
{
#ifndef LFS
   int 	  i, rval;
#endif
   volatile char   *addr;
   ushort id, port, val;

#define	ANT_PCI_ID_ADDR		0x00C001B0

   /* Read id/peb register */
   addr = (char *)ANT_PCI_ID_ADDR;
   AntAddrSet(vaddr, (char *)addr);
#ifdef ANT_CNVRTR_BRD
   addr = vaddr + A4_SUB_ADDRESS;	/* data reg */
#else
   addr = vaddr + (2 * A4_SUB_ADDRESS);	/* data reg */
#endif /* ANT_CNVRTR_BRD */
#ifdef LFS
   val = inw((int)(long)addr);	/* read from data reg */
#else
   val = inw((int)addr);	/* read from data reg */
#endif
#ifdef BIG_ANT
   val = WBYTESWAP(val);
#endif /* BIG_ANT */
   id = (val & 0xff) >> 3;	/* bits d3-d7 */

#ifdef ANT_CNVRTR_BRD
   id = numPciBoards;
#endif /* ANT_CNVRTR_BRD */

   if (id >= MAX_ID_NUM) {
	cmn_err(CE_WARN, "Antares ID=0x%x, should be < 0x%x\n",
		id, MAX_ID_NUM);
   	id = numPciBoards;
   }

#ifdef LFS
   AntVirtAddr[id] = (int)(long)vaddr;
   AntPciCfgVirtAddr[id] = (int)(long)CfgVirtAddr;
#else
   AntVirtAddr[id] = (int)vaddr;
   AntPciCfgVirtAddr[id] = (int)CfgVirtAddr;
#endif
   port = (id << 4) & 0xff;

   /* Now we can use existing A4_read/write routines */

   numPciBoards++;

#ifdef ANT_BRD_DBG
/*
   CnvrtrBrd((unsigned long *)CfgVirtAddr);
*/

#define	ANT_PCI_LED_ADDR	0x00C001C0
#define	ANT_PCI_DSP_ADDR	0x00C000C0

   addr = vaddr;

   val = inw((int)addr);	/* read from ctrl reg */
#ifdef BIG_ANT
   val = WBYTESWAP(val);
#endif /* BIG_ANT */

   if (val & _A4_AIDIS) {	/* auto increament disabled */
      val &= (~_A4_AIDIS);	/* enable by clearing it */
#ifdef BIG_ANT
      val = WBYTESWAP(val);
#endif /* BIG_ANT */
#ifdef LINUX
      outw(val, (int)addr);     /* write to ctrl reg */
#else
      outw((int)addr, val);     /* write to ctrl reg */
#endif /* LINUX */
   }

   /* First set leds */
   A4_setAddress(port, ANT_PCI_LED_ADDR);
#ifdef ANT_CNVRTR_BRD
   addr = vaddr + A4_SUB_ADDRESS;	/* data reg */
#else
   addr = vaddr + (2 * A4_SUB_ADDRESS);	/* data reg */
#endif /* ANT_CNVRTR_BRD */

   val = 1;
#ifdef BIG_ANT
   val = WBYTESWAP(val);
#endif /* BIG_ANT */
#ifdef LINUX
   outw(val, (int)addr);        /* write to data reg */
#else
   outw((int)addr, val);        /* write to data reg */
#endif /* LINUX */

   /* Now read dsp-count register */
   A4_setAddress(port, ANT_PCI_DSP_ADDR);
#ifdef ANT_CNVRTR_BRD
   addr = vaddr + A4_SUB_ADDRESS;	/* data reg */
#else
   addr = vaddr + (2 * A4_SUB_ADDRESS);	/* data reg */
#endif /* ANT_CNVRTR_BRD */
   val = inw((int)addr);	/* read from data reg */
#ifdef BIG_ANT
   val = WBYTESWAP(val);
#endif /* BIG_ANT */
   val = val & 0xf;

#ifdef FORCE_16_BITS
   check_data(port, (volatile char *)0x00fc0000); /* start addr */
   check_auto(port, (volatile char *)0x00fc0000); /* start addr */
#else
   check_data32(port, (volatile char *)0x00fc0000); /* start addr */
   check_auto32(port, (volatile char *)0x00fc0000); /* start addr */
#endif

#endif /* ANT_BRD_DBG */

   return ((int)id);
}
#endif /* PCI_ANT */

