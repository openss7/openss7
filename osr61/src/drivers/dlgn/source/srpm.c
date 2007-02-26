/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srpm.c
 * Description                  : SRAM protocol - main module
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
#define _SVR4_SOURCE	1
#include <sys/os7/compat.h>
#else
#include <sys/LiS/config.h>
#if (CLONE__CMAJOR_0 > 220)
#define DLGC_LIS_FLAG
#include <sys/LiS/module.h>
#endif
#endif
#include "dlgclinux.h"

#include "typedefs.h"
#include "gndefs.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndbg.h"
#include "gncfd.h"
#include "gnerrno.h"

#include "d40defs.h"
#include "d40low.h"
#include "dxdevdef.h"
#include "dxlibdrv.h"
#include "dxmsg.h"
#include "dxstart.h"
#include "fln.h"
#include "srmap.h"
#include "srdbg.h"

#include "dxdrv.h"

#include "srcmd.h"
#include "srdefs.h"
#include "srmisc.h"
#include "srpm.h"

#include "sbamapin.h"

#include "dlgcos.h"
#include "srextern.h"
#include "drvdebug.h"

#ifdef PCI_SPAN
#include "dpci.h"
#endif /* PCI_SPAN */

#ifdef SVR4_MOT
#include <sys/ddi.h>
#endif /* SVR4_MOT */

/* PM state flags */
#define SR_PMINIT               0x0001

#ifdef BIG_ENDIAN
#include "endian.h"
#define	RWORD(x)	READ_WORD(&x)
#define	WWORD(x, d)	WRITE_WORD(&x, d)
#endif /* BIG_ENDIAN */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef _STATS_
extern int StatisticsMonitoring;
extern SRAM_DRV_STATISTICS DriverStats;
#endif

/*
 * PM internal state structure
 */
typedef struct pm_state {
   ULONG       srpm_flags;
   SR_MAPLST   *srpm_maplistp;
} PM_STATE;


/*
 * Module private data
 */
PRIVATE PM_STATE Sram_State = {0};      /* PM state structure */

/*  Added */
/*
 * Externals
 */
#ifdef ISA_SPAN
USHORT IsaBrdId = 16;
extern unsigned char *sbaLocatorIDaddr[];
extern int sbapmstarted;
extern int sbaCurrIOport;
#endif

#ifdef PCI_SPAN
extern GLOBAL_PCI_INFO GlobalPCIData[];
extern PGP_CONTROL_BLOCK GpControlBlock;
#endif /* PCI_SPAN */

#ifdef sun
int sr_spurrious_cnt;
int sr_check_spurrious;
#endif /* sun */

#ifdef DONGLE_SECURITY
int sr_dongle_init = 0;
#endif /* DONGLE_SECURITY */

#ifdef sun
#include <sys/mutex.h>
extern kmutex_t         inthw_lock;
extern kmutex_t		dlgn_stoplock;
extern kcondvar_t	dlgn_stopcv;
#endif /* sun */

#ifdef LINUX
#ifndef LFS
#include <sys/lislocks.h>
extern lis_spin_lock_t  inthw_lock;
#else
extern spinlock_t inthw_lock;
#endif
extern int Single_Board_SS;
#endif

#ifdef LFS
int sr_cfgchk(GN_BOARD *, ULONG);
#ifdef PCI_SPAN
extern int sba_mapin( unsigned short NewBoard, unsigned short PciBoardId );
#else
extern unsigned short sba_mapin( unsigned short NewBoard );
#endif /* PCI_SPAN */
#endif

/***********************************************************************
 *        NAME: sramstartpm
 * DESCRIPTION: PM initialization routine
 *      INPUTS: gnbdp - ptr to start of GN_BOARD array for this PM
 *              ngp - ptr to non-genric data area.
 *              nbds - total number of BOARDS on this PM
 *              nlds - total number of LOGDEVS on this PM
 *              ngsz - Total # bytes of non-generic data area.
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
int sramstartpm(GN_BOARD *gnbdp, char *ngp, ULONG nbds, ULONG nlds, ULONG ngsz)
#else
int sramstartpm(gnbdp, ngp, nbds, nlds, ngsz)
GN_BOARD    *gnbdp;
char        *ngp;
ULONG       nbds;
ULONG       nlds;
ULONG       ngsz;
#endif
{
   SR_MAPLST   *mlp = NULL;
   DX_INITBD   *dibp;
   DX_INITBD   *edibp;
   DX_BOARD    *ddp;
   GN_BOARD    *bp;
   GN_LOGDEV   *ldp;
   
#ifdef PCI_SPAN
   int          indx;
#endif /* PCI_SPAN */

   static char sr_signonmsg[] =
      "\nDialogic Shared RAM Protocol Module\n%s\n%s\n";
   static char sr_rightsmsg[] = 
      "Copyright (C) 2005 Intel Corp.\nALL RIGHTS RESERVED\n\n";

#ifdef ISA_SPAN
   /*
    * Initialize Current MappedIn board to be NOPORT if SBAPM is not there.
    */
   if (sbapmstarted == FALSE) {
      sbaCurrIOport = DX_NOPORT;
      bzero(sbaLocatorIDaddr, (D4XE_MAXLOC + 1));
#ifdef PCI_SPAN
      bzero(GlobalPCIData, (PCI_MAX_BOARDS * sizeof(GLOBAL_PCI_INFO)));
#endif /* PCI_SPAN */
   }
#endif /* ISA_SPAN */

   /* See if the PM has already been started */
   if (Sram_State.srpm_flags & SR_PMINIT) {
      DLGC_MSG2(CE_WARN,"sramstartpm: busy, cannot re-start\n");
      return (-1);
   }

#ifdef DONGLE_SECURITY
   /* Initialize the dongle flag */
   sr_dongle_init = 0;
#endif /* DONGLE_SECURITY */

   /* Display the signon message */
   DLGC_MSG4(CE_CONT, sr_signonmsg, PKGVER, OSTYPEVER );
   DLGC_MSG2(CE_CONT, sr_rightsmsg);

   /* Validate the device dependent data sizes */
   if (nbds * sizeof(DX_INITBD) != ngsz) {
      DLGC_MSG2(CE_WARN,"sramstartpm: bad size, cannot start PM\n");
      return (-1);
   }

#ifndef SVR4_MOT
#ifdef VME_LIVE_INSERTION
   /* Initialize the shared-RAM mapping functions.
    * For Live Insertion, take into account that all boards
    * may be in the system at some point during runtime.
    */
   if ((mlp = sr_mapinit(GN_MAXSLOTS*9)) == NULL) { /* rwt changed to 9 */
#else
   /* Initialize the shared-RAM mapping functions */
   if ((mlp = sr_mapinit(nbds)) == NULL) {
#endif /* VME_LIVE_INSERTION */
      DLGC_MSG2(CE_WARN,"sramstartpm: sr_mapinit failed\n");
      return (-1);
   }
#endif /* SVR4_MOT */

   /* Get ptrs to device-dependent memory area */
   bp = gnbdp;
   edibp = ((DX_INITBD *)ngp) + nbds;

   /*
    * Initialize all the device-dependent BOARD and LOGDEV areas on this
    * PM. Remember that the Config Driver has already zeroed out the
    * device-dependent BOARD and LOGDEV structures prior to calling this
    * routine.
    */
#ifdef VME_LIVE_INSERTION
   for (dibp = (DX_INITBD *)ngp; dibp < edibp; dibp++, bp = bp->bd_nbdp) {
#else
   for (dibp = (DX_INITBD *)ngp; dibp < edibp; dibp++, bp++) {
#endif /* VME_LIVE_INSERTION */

#ifndef VME_SPAN
#ifndef  NOSBA
      /*
       * If SBAPM has been started then we can perform this check.
       * If the IO port is not set to DX_NOPORT, it means that the board
       * wants to use FBAM support so call the SpringBoard PM to validate
       * the port value. If the port is invalid, set it to DX_NOPORT (so
       * that it will not use FBAM).
       */
      if ((sbapmstarted == TRUE) &&
          ((dibp->dib_ioport != DX_NOPORT) && 
           (sba_chkport(dibp->dib_ioport, dibp->dib_physaddr) < 0))) {
         DLGC_MSG4(CE_WARN,
            "sramstartpm: %s: Invalid port: 0x%x, ignored.\n",
            bp->bd_name, dibp->dib_ioport);
         dibp->dib_ioport = DX_NOPORT;
      }
#endif /* NOSBA */
#endif /* VME_SPAN */

      /* Point to the DX_BOARD structure for this BOARD */
      ddp = (DX_BOARD *)bp->bd_devdepp;

      /* Initialize the required DX_BOARD members */
      ddp->db_physaddr = dibp->dib_physaddr;
      ddp->db_totmem   = dibp->dib_totmem;
      ddp->db_ioport   = dibp->dib_ioport;
#ifdef BRI_SUPPORT
      ddp->db_dslnum   = dibp->dib_dslnum;
#endif /* BRI_SUPPORT */
      
#ifdef PCI_SPAN
#ifdef SPARC_PCI
      ddp->db_pcibrdid = bp->bd_irq - 1;
#else
      ddp->db_pcibrdid = IsaBrdId;
#endif /* SPARC_PCI */

 for (indx = 0; indx <= PCI_MAX_BOARDS; indx++) {
#ifdef LFS
     if ((((int)(long)dibp->dib_physaddr & 0xffff0000) == GpControlBlock->GpVirtMapBlock[indx].PhysAddr) ||
         (((int)(long)dibp->dib_physaddr & 0xffffe000) == GpControlBlock->GpVirtMapBlock[indx].PhysAddr) ) {
#else
     if ((((int)dibp->dib_physaddr & 0xffff0000) == GpControlBlock->GpVirtMapBlock[indx].PhysAddr) ||
         (((int)dibp->dib_physaddr & 0xffffe000) == GpControlBlock->GpVirtMapBlock[indx].PhysAddr) ) {
#endif
                ddp->db_pcibrdid = indx;
		break;
     }
 }
#endif /* PCI_SPAN */

#if 0
cmn_err(CE_WARN,"srpm: brdid = %d\n", ddp->db_pcibrdid);

cmn_err(CE_WARN, "sramstartpm: bd_nlogdevs=0x%x, bd_boardid=0x%x pmid=0x%x", gnbdp->bd_nlogdevs, gnbdp->bd_boardid, gnbdp->bd_pmid);
#endif 

      STRACE3("sramstartpm: bd_nlogdevs=0x%x, bd_boardid=0x%x pmid=0x%x", gnbdp->bd_nlogdevs, gnbdp->bd_boardid, gnbdp->bd_pmid);

      STRACE3("sramstartpm: physaddr=0x%lx, totmem=0x%lx, ioport=0x%lx", dibp->dib_physaddr, dibp->dib_totmem, dibp->dib_ioport);

      /* Save the board's type */
      switch (ddp->db_phystype = dibp->dib_type) {
      case DI_SPAN:
         ddp->db_logtype = DI_D40;
         break;
      default:
         ddp->db_logtype = dibp->dib_type;
         break;
      }

      /* Initialize all the required DX_LOGDEV members on this BOARD */
      for (ldp = bp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
         sr_setqdefaults((DX_LOGDEV *)ldp->ld_devdepp);
      }
   }

#ifdef VME_SPAN
   /*
   ** The following "for" loop must be done before sr_cfgchk(), which
   ** will set gbp_same_fwi_addr to the right GN_BOARD structure if
   ** more than one board share the same shared RAM.
   */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
#endif /* VME_LIVE_INSERTION */

      /* In here, set the variable to point back to itself.  By default,
      ** all virtual boards are assumed to have their own shared RAM
      ** address.  (However, in the case of the Span card, the DTI and 
      ** the first voice virtual board 1 share the same shared RAM area.
      ** If that is the case, the gbp_same_fwi_addr will be set the first
      ** voice virtual board.)
      */
      ((DX_BOARD *)(bp->bd_devdepp))->gbp_same_fwi_addr = bp;
   }
#endif /* VME_SPAN */

   /*
    * Check all boards for correct configuration including shared
    * memory segment overlaps; exit if incorrect.
    */

   if (sr_cfgchk(gnbdp, nbds) != 0) {
      DLGC_MSG2(CE_WARN,
         "sramstartpm: invalid configuration, cannot start PM\n");
      return (-1);
   }

   /*
    * Add each board to the list for shared RAM mapping. Since the 
    * configuration was validated above, disable overlap checking.
    */
#ifdef SVR4_MOT
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
      if (sr_mapmem((SR_BDSRAM *)&(((DX_BOARD *)bp->bd_devdepp)->db_sr)) != 0) {
         DLGC_MSG3(CE_WARN,"sramstartpm: cannot map board:%s\n",bp->bd_name);
      }
   }
#else /* SVR4_MOT */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
#endif /* VME_LIVE_INSERTION */
      if (sr_mapadd(mlp,bp,&(((DX_BOARD *)bp->bd_devdepp)->db_sr),
            SR_SHARESEG) != 0) {
         DLGC_MSG3(CE_WARN,"sramstartpm: cannot map board:%s\n",bp->bd_name);
      }
   }

   /* Map the boards into the kernel address space */
   if (sr_map2mem(mlp) == -1) {
      DLGC_MSG2(CE_WARN,"sramstartpm: mapping failure, cannot start\n");
      return (-1);
   }
#endif /* SVR4_MOT */

   /* Save the map ptr into the PM's state structure */
   Sram_State.srpm_maplistp = mlp;

   /* Mark the PM's state as initialized */
   Sram_State.srpm_flags |= SR_PMINIT;

#ifdef sun
   sr_spurrious_cnt = 0;
#endif

   /* Return SUCCESS */
   return (0);
}


/***********************************************************************
 *        NAME: sramstoppm
 * DESCRIPTION: PM shutdown routine
 *      INPUTS: bdp - pointer to generic BOARD structure
 *            : nbds - number of boards on this PM
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
int sramstoppm(GN_BOARD *gnbdp, ULONG nbds)
#else
int sramstoppm(gnbdp, nbds)
GN_BOARD *gnbdp;
ULONG    nbds;
#endif
{
   GN_BOARD    *bp;
   GN_BOARD    *ebdp;
   GN_LOGDEV   *ldp;


   /* Make sure PM is running */
   if (!(Sram_State.srpm_flags & SR_PMINIT)) {
      DLGC_MSG2(CE_WARN,"sramstoppm: inactive, cannot halt\n");
      return (-1);
   }

#ifdef DONGLE_SECURITY
   /* Initialize the dongle flag */
   sr_dongle_init = 0;
#endif /* DONGLE_SECURITY */

   /* Make sure all LOGDEVs on all BOARDs are closed and stopped */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   ebdp = gnbdp + nbds;
   for (bp = gnbdp; bp < ebdp; bp++) {
#endif /* VME_LIVE_INSERTION */
      for (ldp = bp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
         if (ldp->ld_flags & (LD_START | LD_OPEN)) {
            DLGC_MSG3(CE_WARN,"sramstoppm: %s active, cannot halt PM\n",
               ldp->ld_name);
            return (-2);
         }
      }
   }

   /*
    * NOTE: Don't need to clear any of the BOARD or LOGDEV data areas as
    * this will be done by the Config Driver when this routine exits.
    */
#ifdef sun
   /* Since the PM is no longer active, can go ahead and reset the
   ** the board, i.e. SPAN.  This is particularly important in situation
   ** such as running on Solaris 2.4 that drivers can be dynamically
   ** loaded and unloaded. 
   */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < ebdp; bp++) {
#endif /* VME_LIVE_INSERTION */
         sr_bd_hdreset(bp);
   }
#endif /* sun */

#ifdef SVR4_MOT
#else
   /* Unmap the boards */
   if (sr_unmap(Sram_State.srpm_maplistp) == -1) {
      DLGC_MSG2(CE_WARN,"sramstoppm: unmap failed!\n");
   }
#endif /* SVR4_MOT */

   Sram_State.srpm_maplistp = (SR_MAPLST *)NULL;

   /* Mark the PM as halted */
   Sram_State.srpm_flags &= ~SR_PMINIT;

#ifdef ISA_SPAN
   /* Clear out the Locator ID Structure */
#ifdef LINUX
   bzero(sbaLocatorIDaddr, (D4XE_MAXLOC * sizeof(unsigned char *)));
#else
   bzero((caddr_t)sbaLocatorIDaddr, (D4XE_MAXLOC * sizeof(unsigned char *)));
#endif /* LINUX */
#ifdef PCI_SPAN
#ifdef LINUX
   bzero(GlobalPCIData, (PCI_MAX_BOARDS * sizeof(GLOBAL_PCI_INFO)));
#else
   bzero((caddr_t)GlobalPCIData, (PCI_MAX_BOARDS * sizeof(GLOBAL_PCI_INFO)));
#endif /* LINUX */
#endif /* PCI_SPAN */
#endif /* ISA_SPAN */
   /* Return SUCCESS */
   return (0);
}

/***********************************************************************
 *        NAME: sramstartbd
 * DESCRIPTION: PM board startup routine
 *      INPUTS: bdp - ptr to GN_BOARD for board to be started
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
void sr_addirq(GN_BOARD *bdp);
void sr_rmvirq(GN_BOARD *bdp);
int PciIntEnb(char *, int);
#endif
int sramstartbd(bdp)
GN_BOARD *bdp;
{
#if defined(sun) || defined(LINUX)
   volatile D4XSRAM     *bdaddr;
#else  
   D4XSRAM     *bdaddr;
#endif /* sun */
   DX_BOARD    *dxbdp = ((DX_BOARD *) bdp->bd_devdepp);
   DX_LOGDEV   *dxldp;
   GN_LOGDEV   *ldp;
   DX_FWI      *fwip;
   ULONG       bdtype;
   ULONG       fwnum = 0;
   USHORT      nchans;
   USHORT      offset;
   USHORT      ioport = GET_IOPORT(bdp);
#ifdef PCI_SPAN
   USHORT      PCIBoardId =  GET_PCIBRDID(bdp);
#endif /* PCI_SPAN */
   int 	       z_flag;

   /* Get the pointer to the board's shared RAM area & validate it */
   if ((bdaddr = (D4XSRAM *)(dxbdp->db_sramaddr)) == NULL) {
      DLGC_MSG3(CE_WARN,"sramstartbd: %s: no address, board ignored\n",
         bdp->bd_name);
      return (-1);
   }

   DLGCSPINLOCK(inthw_lock, z_flag);
   
#ifdef VME_SPAN
   if (sram_in_range(dxbdp->db_ioport, 
		dxbdp->db_physaddr, dxbdp->db_totmem) == 0) {
      DLGC_MSG4(CE_NOTE, "sramstartbd: shared RAM addr=0x%lx out of range for board #%d.\n", dxbdp->db_physaddr, dxbdp->db_ioport); 
      return (-1);
   }

   if (sram_present(dxbdp->db_physaddr, dxbdp->db_totmem) == 0) {
      DLGC_MSG4(CE_NOTE, "sramstartbd: shared RAM addr=0x%lx for board #%d is not responding to read/write.\n", dxbdp->db_physaddr, dxbdp->db_ioport); 
      return (-1);
   }

#ifdef DONGLE_SECURITY
   if (sr_dongle(bdp)) {
      DLGC_MSG3(CE_WARN, 
         "sramstartbd: board in slot id #%d is probably not of Dialogic product.  Abort starting the board.\n", dxbdp->db_ioport);
      return (-1);
   }
#endif /* DONGLE_SECURITY */

#else /* VME_SPAN */

   /*
    * If the SpringBoard PM has not been started, then
    * Activate the board's memory if it is using FBAM.
    * Otherwise, Determine if a locator technology board is present.
    */
#ifdef PCI_SPAN
   if (sbapmstarted == FALSE) {
      if (GpControlBlock->BoardAttrs[PCIBoardId].BusType != BUS_TYPE_PCI) {
         if ( (ioport < 0x100) && (sbaLocatorIDaddr[ioport] == (UCHAR *)0) ) {
              sbaLocatorIDaddr[ioport] = (UCHAR *) ((UCHAR *)bdaddr + 0xe0);
         } else {
   	      /* Determine if a locator technology board is present */
              if ((ioport != DX_NOPORT) && (ioport <= D4XE_MAXLOC)) {
                 if (sbaLocatorIDaddr[ioport] <= (unsigned char *)1) {
                     DLGC_MSG3(CE_WARN,"sramstartbd: %s Missing, board ignored\n",
				     				  bdp->bd_name);
   		     DLGCSPINUNLOCK(inthw_lock, z_flag);
                     return (-1);
                 }
              }
         }
      } else {
         if ((ioport < 0x100) && 
	     (GlobalPCIData[PCIBoardId].LocatorAddress == (UCHAR *) 0)) {
              GlobalPCIData[PCIBoardId].LocatorAddress = 
		     			 (UCHAR *)((UCHAR *)bdaddr + 0xe0);
         } else {
              if ((ioport != DX_NOPORT) && (ioport <= D4XE_MAXLOC)) {
                  if (GlobalPCIData[PCIBoardId].LocatorAddress <= (UCHAR *)1) {
                      DLGC_MSG3(CE_WARN,"sramstartbd: %s Missing, board ignored\n",
                                                                    bdp->bd_name);
   		      DLGCSPINUNLOCK(inthw_lock, z_flag);
                      return -1;
                  }
              }
         }
      }
   }     
#else
   if (sbapmstarted == FALSE) {
      if ( (ioport < 0x100) && (sbaLocatorIDaddr[ioport] == (UCHAR *)0) ) {
           sbaLocatorIDaddr[ioport] = (UCHAR *) ((UCHAR *)bdaddr + 0xe0);
      }
   } else {
      /* Determine if a locator technology board is present */
      if ((ioport != DX_NOPORT) && (ioport <= D4XE_MAXLOC)) {
         if (sbaLocatorIDaddr[ioport] <= (unsigned char *)1) {
             DLGC_MSG3(CE_WARN,"sramstartbd: %s Missing, board ignored\n",
			    				    bdp->bd_name);
   	     DLGCSPINUNLOCK(inthw_lock, z_flag);
             return (-1);
         }
      } 
   }
#endif /* PCI_SPAN */

#endif /* VME_SPAN */

#if 0
      DLGC_MSG3(CE_WARN,"sramstartbd: Port = 0x%x: PciId = 0x%x\n",
		ioport, PCIBoardId);
         return (-1);
#endif

#ifdef ISA_SPAN
   /* Activate the board's memory if it is using FBAM */
   if (ioport != DX_NOPORT) {
#ifdef PCI_SPAN
      sba_mapin(ioport, PCIBoardId);
#else
      sba_mapin(ioport);
#endif
   }

   /* Add the board to the interrupt chain */
    sr_addirq(bdp);
#endif /* ISA_SPAN */

   /* Attempt to reset the board */
   if (sr_bdreset(bdp) != 0) {
      DLGC_MSG3(CE_WARN,"sramstartbd: %s: reset failed, board ignored\n",
         bdp->bd_name);
#ifdef ISA_SPAN
      sr_rmvirq(bdp);
#endif /* ISA_SPAN */
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      return (-1);
   }

   /* Get the firmware version from shared RAM */
   dxbdp->db_fwver = 
      (10 * (bdaddr->dl_vers[2] - '0')) + (bdaddr->dl_vers[3] - '0');

   ldp = bdp->bd_ldp;
   dxldp = (DX_LOGDEV *)(ldp->ld_devdepp);

   /*
    * If the board's type has been specified as DI_D4x, then read its
    * type from the shared RAM area and update if necessary.
    */
   switch (bdtype = dxbdp->db_logtype) {
   case DI_DCP:
   case DI_VR40:
   case DI_VR121:
   case DI_VR160:
   case DI_T1SPAN:
   case DI_E1SPAN:
#ifdef DCB
   case DI_DCB:
   case DI_DCB320:
   case DI_DCB640:
   case DI_DCB960:
#endif
      break;
   case DI_D40:
   case DI_D41:
      /* Get the exact type from shared RAM */
      switch (bdtype = bdaddr->dl_btype) {
      case 0:
         dxbdp->db_logtype = DI_D40;
         break;
      case 1:
         dxbdp->db_logtype = DI_D41; 
         break;
      default:
#ifdef LFS
         DLGC_MSG4(CE_WARN,"sramstartbd: %s: unknown type:%lx, board ignored\n",
            bdp->bd_name, bdtype);
#else
         DLGC_MSG4(CE_WARN,"sramstartbd: %s: unknown type:%x, board ignored\n",
            bdp->bd_name, bdtype);
#endif
#ifdef ISA_SPAN
         sr_rmvirq(bdp);
#endif /* ISA_SPAN */
         DLGCSPINUNLOCK(inthw_lock, z_flag);
         return (-1);
         break;
      }
      break;
   default:
#ifdef LFS
      DLGC_MSG4(CE_WARN,"sramstartbd: %s: invalid type:%lx, board ignored\n",
         bdp->bd_name, bdtype);
#else
      DLGC_MSG4(CE_WARN,"sramstartbd: %s: invalid type:%x, board ignored\n",
         bdp->bd_name, bdtype);
#endif
#ifdef ISA_SPAN
      sr_rmvirq(bdp);
#endif /* ISA_SPAN */
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      return (-1);
      break;
   }

   /* Set up the type on the board device's ldp */
   dxldp->dl_type = dxbdp->db_logtype;

   /* 
    * If it's NOT a VR121, VR160, T1SPAN, or E1SPAN then get the number of
    * channel devices on the board out of shared RAM.
    */
   switch (dxbdp->db_logtype) {
   case DI_VR121:
   case DI_VR160:
   case DI_T1SPAN:
   case DI_E1SPAN:
#ifdef DCB
   case DI_DCB:
   case DI_DCB320:
   case DI_DCB640:
   case DI_DCB960:
#endif
      break;
   default:
      if ((nchans = bdaddr->dl_nchans) < (bdp->bd_nlogdevs - 1)) {
         DLGC_MSG5(CE_WARN,
            "sramstartbd: %s: bad channel count: %d (expected %d), board ignored\n",
            bdp->bd_name, nchans, bdp->bd_nlogdevs - 1);
#ifdef ISA_SPAN
         sr_rmvirq(bdp);
#endif /* ISA_SPAN */
         DLGCSPINUNLOCK(inthw_lock, z_flag);
         return (-1);
      }
      break;
   }

   /* Set up the board's DEV area FWI */
   fwip = dxbdp->db_fwi;
#ifdef BIG_ENDIAN /*  */
   offset = RWORD(bdaddr->dl_devarea) << PGRPHSHIFT;
#else
   offset = bdaddr->dl_devarea << PGRPHSHIFT;
#endif /* BIG_ENDIAN */
   fwip->fwi_addr = (UCHAR *)bdaddr + offset;
   fwip->fwi_num  = fwnum;
   fwip->fwi_type = IO_DEVFWI;
   fwip->fwi_ldp  = NULL;

   STRACE1("sramstartbd: bdaddr=0x%lx", bdaddr);
#ifdef BIG_ENDIAN /*  */
   STRACE3("sramstartbd: DEV fwi_addr=0x%lx, offset=0x%lx, dl_devarea=0x%lx", fwip->fwi_addr, offset, RWORD(bdaddr->dl_devarea));
#else
   STRACE3("sramstartbd: DEV fwi_addr=0x%lx, offset=0x%lx, dl_devarea=0x%lx", fwip->fwi_addr, offset, bdaddr->dl_devarea);
#endif /* BIG_ENDIAN */

   /* Set up the board's channel FWI's (non-DTI type boards) */
   switch (dxbdp->db_logtype) {
   case DI_T1SPAN:
   case DI_E1SPAN:
#ifdef DCB
   case DI_DCB:
   case DI_DCB320:
   case DI_DCB640:
   case DI_DCB960:
#endif
      break;
   default:

      /* Set up the board's CHANNEL area FWI's */
      for (++fwip; fwnum < 4 ; fwip++) {

#ifdef BIG_ENDIAN /*  */
         offset = RWORD(bdaddr->dl_bfaddr[fwnum]) << PGRPHSHIFT;
#else
         offset = bdaddr->dl_bfaddr[fwnum] << PGRPHSHIFT;
#endif /* BIG_ENDIAN */

         fwip->fwi_addr = (UCHAR *)bdaddr + offset;
         fwip->fwi_num  = ++fwnum;
         fwip->fwi_type = IO_CHNFWI;
         fwip->fwi_ldp  = NULL;

         STRACE2("sramstartbd: CHN fwi_addr=0x%lx, offset=0x%lx, dl_bfaddr=0x%lx", fwip->fwi_addr, offset);

      }
      break;
   }

   for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
      dxldp = (DX_LOGDEV *)(ldp->ld_devdepp);
#ifdef VME_SPAN /*  */
      sr_bzero((char *)(&dxldp->dl_stop), sizeof(DX_DEVSTOP));
#else
      bzero((char *)(&dxldp->dl_stop), sizeof(DX_DEVSTOP));
#endif /* VME_SPAN */
      dxldp->dl_stopcmd.pc_cmd = PC_STOP;
      dxldp->dl_stopflag = DX_STOPCMD;
      if (ldp != bdp->bd_ldp) {
         dxldp->dl_stopfwi = IO_CHNFWI;
         switch (dxbdp->db_logtype) {
         case DI_T1SPAN:
         case DI_E1SPAN:
#ifdef DCB
   	 case DI_DCB:
   	 case DI_DCB320:
   	 case DI_DCB640:
   	 case DI_DCB960:
#endif
            dxldp->dl_type = dxbdp->db_logtype | DT_TS;
            break;
         default:
            dxldp->dl_type = dxbdp->db_logtype | DT_CH;
            break;
         }
      } else {
#ifdef DCB
         dxldp->dl_stopfwi = 0x0F;
#else
         dxldp->dl_stopfwi = IO_DEVFWI;
#endif
      }
      ldp->ld_flags |= LD_START;
   }
#ifdef VME_SPAN
   /* Add the board to the interrupt chain */
   sr_addirq(bdp);
#endif

   DLGCSPINUNLOCK(inthw_lock, z_flag);

   /* Single Board Start Support Only If Enabled */
   if (Single_Board_SS && (PCIBoardId < 0x10)) {
       PciIntEnb(MAP_ADDR(GET_CFG_PHY_ADDR(PCIBoardId), 0x80), PCIBoardId);
   }
   
   /* Display the board start-up message */
   DLGC_MSG3(CE_CONT, "%s: Board started\n", bdp->bd_name);

   return (0);
}

/***********************************************************************
 *        NAME: sramstopbd
 * DESCRIPTION: PM board shutdown routine
 *      INPUTS: bdp - ptr to GN_BOARD for board to be stopped
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int PciIntDis(char *, int);
#endif
int sramstopbd(bdp)
GN_BOARD *bdp;
{
   GN_LOGDEV *ldp;
#ifdef ISA_SPAN
   USHORT     ioport = GET_IOPORT(bdp);
#endif /* ISA_SPAN */
#ifdef PCI_SPAN
   USHORT      PCIBoardId =  GET_PCIBRDID(bdp);
#endif /* PCI_SPAN */

   /* For Single Board Stop - Force the Stop during actvity */
   if (!Single_Board_SS) {
      /* Make sure that all LOGDEVs on this board are closed */
      for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
          if (ldp->ld_flags & LD_OPEN) {
             DLGC_MSG3(CE_WARN,"sramstopbd: %s busy, cannot halt board\n", 
			     ldp->ld_name);
#ifdef VME_SPAN
#ifndef VME_LIVE_INSERTION
             dlgn_hangup(ldp);
#endif /* VME_LIVE_INSERTION */
#endif /* VME_SPAN */
#ifdef ISA_SPAN
             sr_rmvirq(bdp);
#endif /* ISA_SPAN */
             return (-2);
          }
      }
   }

#ifdef VME_SPAN
   /* Stop the board from issuing interrupts. */
   sr_bd_hdreset(bdp);
#endif /* VME_SPAN */

   /* Remove the board from the IRQ list */
   sr_rmvirq(bdp);

#ifdef VME_LIVE_INSERTION /* rwt, change the lights */
   /* Attempt to reset the board */
   sr_bd_hdreset(bdp);
#endif

   /* Mark each LOGDEV as being stopped */
   for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
      ldp->ld_flags &= ~LD_START;
   }

#ifdef ISA_SPAN
   /*
    * If the SpringBoard PM has not been started, then
    * De-Activate the board's memory if it is using FBAM.
    */
   if (sbapmstarted == FALSE) {
      if (ioport < 0x100) {
         sbaLocatorIDaddr[ioport] = (UCHAR *) 0;
#ifdef PCI_SPAN
         GlobalPCIData[PCIBoardId].LocatorAddress = (UCHAR *) 0;
#endif /* PCI_SPAN */
      }
   }
#endif /* ISA_SPAN */
   
   /* Single Board Stop Support Only If Enabled */
   if (Single_Board_SS && (PCIBoardId < 0x10)) {
       PciIntDis(MAP_ADDR(GET_CFG_PHY_ADDR(PCIBoardId), 0x80), PCIBoardId);
   }

   return (0);
}


/***********************************************************************
 *        NAME: sramopen
 * DESCRIPTION: PM open routine
 *      INPUTS: ldp - ptr to GN_LOGDEV structure
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sramopen(ldp)
GN_LOGDEV *ldp;
{
   DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;

   /* Return error if device not started */
   if (!(ldp->ld_flags & LD_START)) {
#ifdef VME_LIVE_INSERTION_B
      if (ldp->ld_flags & LD_OOSVC)
	 return (-1);
      else
#endif /* VME_LIVE_INSERTION */
      return (-1);
   }

   ldp->ld_flags |= LD_OPEN;
   dxldp->dl_opencnt++;

   return (0);
}


/***********************************************************************
 *        NAME: sramclose
 * DESCRIPTION: PM close routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sramclose(ldp, flags)
GN_LOGDEV   *ldp;
ULONG       flags;
{
   DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   USHORT      ioport;
   DLGCDECLARESPL(oldspl)
#ifdef PCI_SPAN
   USHORT      PCIBoardId;
#endif /* PCI_SPAN */

   /* Return SUCCESS if already closed */
   if (!(ldp->ld_flags & LD_OPEN)) {
      return (0);
   }

#ifdef sun
   if (flags & GN_DEVQUEUED) 
      sr_reset_device(ldp);
#endif /* sun */

   /* Send a STOP to FW if an outstanding command is indicated */
   if (flags & GN_DEVBUSY) {
#ifndef VME_SPAN
      /* Activate the board's memory if it is using FBAM */
      if ((ioport = GET_IOPORT(ldp->ld_gbp)) != DX_NOPORT) {
#ifdef PCI_SPAN
         PCIBoardId =  GET_PCIBRDID(ldp->ld_gbp);
         sba_mapin(ioport, PCIBoardId);
#else
         sba_mapin(ioport);
#endif /* PCI_SPAN */
      }
#endif /* VME_SPAN */
      sr_cleanup(ldp, STOP_STOPONLY);
   }

   /* Do structure cleanup only upon last close for this instance */
   if (--dxldp->dl_opencnt > 0) {
      return (0);
   }

   DLGCSPLSTR(oldspl);

   /* Clean up the send queue on this LOGDEV */
   if (dxldp->dl_sndq) {
      freemsg(dxldp->dl_sndq);
      dxldp->dl_sndq = 0;
   }
   dxldp->dl_sndcnt = 0;

#ifdef VME_SPAN
   /*
   ** 1/6/95: Clean up temporary send buffer.
   */
   if (dxldp->dl_sndq_tmp) {
      freemsg(dxldp->dl_sndq_tmp);
      dxldp->dl_sndq_tmp = 0;
      if (ldp->ld_blockflags & LD_BULKQUEUED)
         ldp->ld_blockflags &= ~LD_BULKQUEUED;
   }
#endif /* VME_SPAN */

   /* Clean up the receive queue on this LOGDEV */
   if (dxldp->dl_rcvq) {
      freemsg(dxldp->dl_rcvq);
      dxldp->dl_rcvq = 0;
   }
   dxldp->dl_rcvcnt = 0;

   /* Force the state to idle */
   /*** EFN: call SM here instead of doing it directly? ***/
#ifndef VME_SPAN
   ldp->ld_genstate = ldp->ld_idlestate;
#else /* VME_SPAN */
   if (ldp->ld_genstate & GN_STOPFLAG) {
      DLGCMUTEX_ENTER(&dlgn_stoplock);
      if (ldp->ld_idlestate == 1) {
         ldp->ld_genstate = ldp->ld_idlestate;
         cv_broadcast(&dlgn_stopcv);
      } else {
         ldp->ld_genstate = ldp->ld_idlestate | GN_STOPFLAG;
      }
      DLGCMUTEX_EXIT(&dlgn_stoplock);
   } else {
      ldp->ld_genstate = ldp->ld_idlestate;
   }
#endif /* VME_SPAN */

   /* Mark the LOGDEV as closed */
   ldp->ld_flags &= ~LD_OPEN;

   DLGCSPLX(oldspl);

   return (0);
}


/***********************************************************************
 *        NAME: sramsendcmd
 * DESCRIPTION: PM command processing routine
 *      INPUTS: ldp - ptr to GN_LOGDEV device to receive command
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: *** MAY BE ENTERED AT INTERRUPT TIME ***
 **********************************************************************/
int sramsendcmd(ldp, mp)
GN_LOGDEV   *ldp;
mblk_t      *mp;
{
#ifndef LFS
   int oldspl;
#endif
   GN_CMDMSG   *ptr;
   DX_BOARD    *dxbdp;
   DX_LOGDEV   *dxldp;
   USHORT      ioport;
#ifdef PCI_SPAN
   USHORT      PCIBoardId;
#endif /* PCI_SPAN */

#if 0
      cmn_err(CE_WARN,"sramsendcmd: device: %s open", ldp->ld_name);
#endif

   /* Exit if no message */
   if (mp == NULL) {
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.NumberOfDriverFailedMessages++;
                }
#endif /* _STATS_ */
      return (0);
   }

   /* Exit if device not opened */
   if (!(ldp->ld_flags & LD_OPEN)) {
      DLGC_MSG3(CE_WARN,"sramsendcmd: device: %s not open\n", ldp->ld_name);
      sr_reply(ldp, mp, DLE_BADCMD);
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.NumberOfDriverFailedMessages++;
                }
#endif /* _STATS_ */
      return (0);
   }

   DLGCMUTEX_ENTER(&inthw_lock);

#ifdef sun
   /* Activate the board's memory if it is using FBAM */
   if ((ioport = GET_IOPORT(ldp->ld_gbp)) != DX_NOPORT) {
#ifdef PCI_SPAN
      PCIBoardId =  GET_PCIBRDID(ldp->ld_gbp);
#ifdef sun
      sba_mapin_intr(ioport, PCIBoardId);
#else
      sba_mapin(ioport, PCIBoardId);
#endif /* sun */
#else
#ifdef sun
      sba_mapin_intr(ioport);
#else
      sba_mapin(ioport);
#endif /* sun */
#endif /* PCI_SPAN */
   }
#endif /* sun */


   /* Determine the type of message received */
   switch (mp->b_datap->db_type) {
   default:
      DLGC_MSG3(CE_WARN,"sramsendcmd: %s: Unknown message type, ignored\n",
          ldp->ld_name);
      freemsg(mp);
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.NumberOfDriverFailedMessages++;
                }
#endif /* _STATS_ */
      DLGCMUTEX_EXIT(&inthw_lock);
      return 0;

   case M_FLUSH:
      /* we should never receive a FLUSH */
      freemsg(mp);
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.NumberOfDriverFailedMessages++;
                }
#endif /* _STATS_ */
      DLGCMUTEX_EXIT(&inthw_lock);
      return 0;

   case M_IOCTL:
      switch (((struct iocblk *)mp->b_rptr)->ioc_cmd) {
      default:
         sr_reply(ldp, mp, DLE_BADCMD);
         break;
      }
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.NumberOfDriverFailedMessages++;
                }
#endif /* _STATS_ */
      DLGCMUTEX_EXIT(&inthw_lock);
      return 0;

   case M_PROTO:
       /* Fall Through to process the message */
       break;
   }

#ifdef sun /* Fix - System panic in Solaris */
      ptr = &((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr;
      dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
#else
   /*
    * If the command is to be sent to an ISDN board and the board is an NFAS
    * B Channel board, then mapin the associated D channel board.
    * No other action needs to be taken in order to send the command to the
    * D Channel board instead of the B Channel board, since structures for
    * all boards contain the same addresses because of FBAM and we will use
    * it to our advantage.
    *
    * Added support for routing from B Channel PCI boards to the D Channel ISA.
    * Note that all ldp structures have a pointer to the D Channel board in it
    * NFAS group.
    * 
    */
   dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
   if (((((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd == ISDN_CMD) &&
       (dxbdp->db_nfasflag == IN_NFAS_B)) && (dxbdp->db_nfasflag != 0)) {
      register GN_BOARD *dchanbdp  = dxbdp->db_dchanbdp;

      /* 
       * At this point route the command to the D channel board. 
       * This will now fix commands from PCI boards.  For the future D channel
       * PCI boards, it is important that dchanbdp is correctly assigned in
       * the nfas configuration routine.
       */       
      ldp = dchanbdp->bd_ldp;
      dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
      dchanbdp = dxbdp->db_dchanbdp;
       
      ioport = GET_IOPORT(dchanbdp);
#ifdef PCI_SPAN
      PCIBoardId =  GET_PCIBRDID(dchanbdp);
#endif /* PCI_SPAN */

   } else {
      ioport = GET_IOPORT(ldp->ld_gbp);
#ifdef PCI_SPAN
      PCIBoardId =  GET_PCIBRDID(ldp->ld_gbp);
#endif /* PCI_SPAN */
   }

   /* Activate the board's memory if it is using FBAM */
   if (ioport != DX_NOPORT) {
#ifdef PCI_SPAN
      sba_mapin(ioport, PCIBoardId);
#else
      sba_mapin(ioport);
#endif /* PCI_SPAN */
   }

    /* dxbdp has been calculated above when determining NFAS.  */

      ptr = &((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr;
#endif /* sun */
      dxldp = (DX_LOGDEV *)ldp->ld_devdepp;

      /*
       * Set up nestfwip to allow for one nested command call. This
       * may occur at interrupt time when we send data up to the BDM
       * and get a command message back from it right away.
       */
      dxldp->dl_nestfwip = dxldp->dl_fwip;


      /*
       * We must set up dl_fwip on this ldp to point to the correct
       * DX_FWI struct to use at interrupt time to get the reply to
       * this command. The dl_fwip->fwi_ldp pointer is used as a
       * "semaphore": it is normally NULL but it may be set to a valid
       * ldp by the following code so that the interrupt routine can be
       * made aware of exceptions to the normal host-board protocol.
       */

#ifdef VME_SPAN
      /* In VME, the host no longer spins CPU cycles to wait for PC_REQ to 
      ** become 0.  The host has the ability to put the command in the
      ** board pending commands queue.  This means that all FW commands 
      ** including those for play and record at the beginning must be checked 
      ** for potential queueing if the board is busy.  For MT_BULK messages that
      ** deal with replenishing buffers, there is no need to check for
      ** potential queueing.  However, the first MT_BULK for play and record
      ** which start the process must be checked also for potential queueing.
      */
      if (ptr->cm_msg_type == MT_DEVICE ||
            (ptr->cm_msg_type == MT_BULK && 
               (ptr->fw_msg_ident == MC_SENDDATA || 
                 ptr->fw_msg_ident == MC_LASTDATA || 
                  ptr->fw_msg_ident == MC_RECEIVEDATA))) {

         /*
         ** 12/11/96: Moved locking originally done in sr_queue_cmd() to
         ** here in order to protect ldp->ld_blockflags. 
         */
	DLGCMUTEX_ENTER(&inthw_lock);
	DLGCSPLSTR(oldspl);
	if (sr_queue_cmd(ldp, mp) == 1) {

            /*
            ** 6/1/95: If MT_BULK is queued, flag it.
            */
            if (ptr->cm_msg_type == MT_BULK) {

               if (ldp->ld_blockflags & LD_BULKQUEUED) {
                  STRACE3_PLAY(
                     "sramsendcmd: ldp=0x%x, already LD_BULKQUEUED! %d %d", 
                     ldp, 0, 0);
		  DLGCMUTEX_EXIT(&inthw_lock);
		  DLGCSPLSTR(oldspl);
                  return;
               } 

               ldp->ld_blockflags |= LD_BULKQUEUED;
               STRACE3_PLAY("sramsendcmd: ldp=0x%x, make LD_BULKQUEUED! datasize=0x%x %d",
                  ldp, msgdsize(mp->b_cont), 0);
            }
	    DLGCMUTEX_EXIT(&inthw_lock);
	    DLGCSPLSTR(oldspl);
            return;
	}
	DLGCMUTEX_EXIT(&inthw_lock);
	DLGCSPLSTR(oldspl);
      }
#endif

      if (ptr->fw_msg_ident == SR_DEVSEND) {
         /*
          * The command is going thru the DEV area so save the DEV
          * area FWI on this ldp. Note that this command may be from
          * either a board device or a channel device.
          */
         dxldp->dl_fwip = &dxbdp->db_fwi[0];

         /*
          * If this is an MF40 command, then it is a board-level command
          * going thru the DEV area but it will at reply (interrupt)
          * time look like a channel command going thru the DEV area
          * since the FW returns dl_reqcopy = 0xFF and dl_data3 = 1 for
          * MF40-type commands. We must set dl_fwip->fwi_ldp on the 
          * board-level FWI to this board's ldp so that the interrupt 
          * routine can locate the correct device that sent the command.
          * Note that the normal state of fwip->fwi_ldp is NULL; here
          * and one other place below are the only places where it is
          * set to a non-NULL value. Also note that the interrupt 
          * routine will re-set the value to NULL at interrupt time.
          */
         if (((((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd) == MF40_ID) &&
             (ldp->ld_devnum == 0)) {
            DLGC_VMEDEBUG_MSG3(CE_NOTE, "MF40_ID/devnum=0 called, fwi_ldp=0x%x", ldp);
            dxldp->dl_fwip->fwi_ldp = ldp;
         }

      } else {
         /*
          * The command is going thru the channel area. The FWI to
          * be saved depends upon whether this command is coming
          * in on a channel device or a board device.
          */
         if (ldp->ld_devnum == 0) {
            /*
             * The command is coming in on the board device so we
             * must save the FWI for channel buffer #1.
             */
            dxldp->dl_fwip = &dxbdp->db_fwi[1];

            /*
             * There is the special case of an MT_CONTROL message which
             * does not expect a reply so leave fwi_ldp in the FWI 
             * structure as NULL. Otherwise, fwi_ldp is set to this
             * ldp so that the interrupt routine can know that the 
             * interrupt is for the board device and not the channel
             * device. This is the only other case where fwi_ldp will
             * NOT be NULL. The interrupt routine resets fwi_ldp back
             * to NULL when it receives the interrupt for this reply.
             */
            if (ptr->cm_msg_type != MT_CONTROL) {
               DLGC_VMEDEBUG_MSG5(CE_NOTE, "sramsendcmd: msgtype=0x%x, fwi_ldp=0x%x, pc_cmd=0x%x", ptr->cm_msg_type, ldp, ((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd);
               dxldp->dl_fwip->fwi_ldp = ldp;
            }

         } else {
            /*
             * The command is coming in on one of the channel devices
             * so save the FWI for the channel buffer for this device.
             */
            dxldp->dl_fwip = &dxbdp->db_fwi[ldp->ld_devnum];
         }
      }

      switch (ptr->cm_msg_type) {
      case MT_CONTROL:
         STRACE0("sramsendcmd: M_PROTO/MT_CONTROL");
         sr_control(ldp, mp);
         break;

      case MT_DEVICE:
         STRACE0("sramsendcmd: M_PROTO/MT_DEVICE");
         sr_device(ldp, mp);
         break;

      case MT_BULK:
         STRACE0("sramsendcmd: M_PROTO/MT_BULK");
         if (ptr->fw_protocol & IO_DEVFWI) {
            dxldp->dl_fwip = &dxbdp->db_fwi[0];
         }
         sr_bulk(ldp, mp);
         break;

      case MT_VIRTEVENT:
         STRACE0("sramsendcmd: M_PROTO/MT_VIRTEVENT");
         sr_virt(ldp, mp);
         break;

      default:
         STRACE0("sramsendcmd: default");
         sr_reply(ldp, mp, DLE_BADCMD);
         break;
      }

      /* Restore the previously-saved value of dl_fwip */
      dxldp->dl_fwip = dxldp->dl_nestfwip;
#ifdef _STATS_
                if (StatisticsMonitoring){
                    DriverStats.DriverCommandsCounter++;
                }
#endif /* _STATS_ */

      DLGCMUTEX_EXIT(&inthw_lock);
      return (0);
}


/***********************************************************************
 *        NAME: srammemsz
 * DESCRIPTION: PM memory size routine
 *      INPUTS: bdszp - ptr to variable for dev-dep BOARD struct size.
 *              ldszp - ptr to variable for dev-dep LOGDEV struct size.
 *     OUTPUTS: Stores sizes in locations given by the ptr args.
 *     RETURNS: always zero.
 *       CALLS: none
 *    CAUTIONS: none
 **********************************************************************/
int srammemsz(bdszp,ldszp,pmnamep)
ULONG *bdszp;
ULONG *ldszp;
char  **pmnamep;
{
   static char sramname[] = "sram";

   *bdszp = (ULONG) sizeof(DX_BOARD);
   *ldszp = (ULONG) sizeof(DX_LOGDEV);
   *pmnamep = sramname;

   return (0);
}

#ifdef _STATS_
/***********************************************************************
 *        NAME: sramstats
 * DESCRIPTION: PM statistics routine
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: none
 *    CAUTIONS: none
 **********************************************************************/
#ifdef LFS
int sramstats(PM_STATS *pmstatp);
#endif
int sramstats(pmstatp)
PM_STATS *pmstatp;
{
   switch(pmstatp->cmd){

   case SR_STARTSTATS:
        bzero( (caddr_t)&DriverStats, sizeof(SRAM_DRV_STATISTICS));
	StatisticsMonitoring = TRUE;
	break;

   case SR_GETSTATS:
	DriverStats.TotalBytesTransferredCounter = DriverStats.TotalBytesReadCounter + DriverStats.TotalBytesWrittenCounter;
 	pmstatp->stat = DriverStats;	
	break;

   case SR_CLEARSTATS:
        bzero( (caddr_t)&DriverStats, sizeof(SRAM_DRV_STATISTICS));
	break;

   case SR_STOPSTATS:
	StatisticsMonitoring = FALSE;
	break;

   default:
	break;
   }

   return (0);
}
#endif /* _STATS_ */

/***********************************************************************
 *        NAME: sramdebug
 * DESCRIPTION: PM debug routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
int sramdebug(GN_DEBUG *gdbp, int *msgszp);
#endif
int sramdebug(gdbp, msgszp)
GN_DEBUG *gdbp;
int *msgszp;
{
   SRDBG_CMD *sdbgp = (SRDBG_CMD *)(gdbp + 1);
   GN_LOGDEV *ldp;

   if ((ldp = cf_name2ldp(sdbgp->dbg_bdname)) == NULL) {
      return (EGN_BADNAME);
   }

   switch (sdbgp->dbg_cmd) {
   case SRDBG_DUMPCMD: {
      SRDBG_DUMP *sdp = (SRDBG_DUMP *)(sdbgp + 1);
      SR_BDSRAM  *brp = &(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_sr);

      if (!(ldp->ld_flags & LD_START)) {
         return (EGN_BADSTATE);
      }

#ifdef ISA_SPAN
      if (brp->br_ioport != DX_NOPORT) {
#ifdef PCI_SPAN
         sba_mapin(brp->br_ioport, brp->br_pcibrdid);
#else
         sba_mapin(brp->br_ioport);
#endif /* PCI_SPAN */
      }
#endif /* ISA_SPAN */

      if (sdp->ds_offset >= sizeof(sdp->ds_srbuf)) {
         sdp->ds_offset = sizeof(sdp->ds_srbuf) - 1;
      }
      
      if (sdp->ds_length > (sizeof(sdp->ds_srbuf) - sdp->ds_offset)) {
         sdp->ds_length = sizeof(sdp->ds_srbuf) - sdp->ds_offset;
      }

#ifdef DLGC_LIS_FLAG
      __wrap_memcpy( (char *)sdp->ds_srbuf, (char *)brp->br_sramaddr + sdp->ds_offset, sdp->ds_length);
#else
      bcopy((char *)brp->br_sramaddr + sdp->ds_offset, (char *)sdp->ds_srbuf, sdp->ds_length);
#endif
      break;
      }
   default:
      return (EGN_BADCMD);
      break;
   }

   return (0);
}

#ifdef sun /*  */
int
sramioctl(char *device, int cmd, char *arg, char **arg2)
{
	PM_MEMSZ	*pm_memszp;
	PM_DEBUG	*pm_debugp;
	PM_STATS	*pm_statsp;
	GN_LOGDEV	*ldp;

	switch(cmd) {
	case PM_GET_PENDING_QUEUE_BOARD:
		ldp = (GN_LOGDEV *)device;
                *arg2 = (char *)
		   ((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->gbp_same_fwi_addr;
		break;
	case PM_DEQUEUE_DPC:
		sr_reset_device((GN_LOGDEV *)device);
		break;
	case PM_MEMSZ_CMD:
		pm_memszp = (PM_MEMSZ *)arg;
		srammemsz(&pm_memszp->bdsz, &pm_memszp->ldsz, 
						(char **)&pm_memszp->pmname);
		break;
	case PM_DEBUG_CMD:
		pm_debugp = (PM_DEBUG *)arg;
		sramdebug(pm_debugp->gn_debug, (int *)&pm_debugp->size); 
		break;
#ifdef _STATS_
	case PM_STATS_CMD:
		pm_statsp = (PM_STATS *)arg;
		sramstats(pm_statsp);
		break;
#endif /* _STATS_ */
	default:
		return;
	}
}


#ifdef VME_SPAN
#ifdef VME_LIVE_INSERTION
/***********************************************************************
 *        NAME: sramaddbd
 * DESCRIPTION: PM runtime environment add board routine
 *      INPUTS: gnbdp - ptr to GN_BOARD structure to add to this PM
 *              ngp - ptr to non-genric data area.
 *              nbds - total number of BOARDS to add to this PM
 *              nlds - total number of LOGDEVS to add to this PM
 *              ngsz - Total # bytes of non-generic data area.
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sramaddbd(gnbdp, ngp, nbds, nlds, ngsz)
GN_BOARD    *gnbdp;
char        *ngp;
ULONG       nbds;
ULONG       nlds;
ULONG       ngsz;
{
   DX_INITBD   *dibp;
   DX_INITBD   *edibp;
   DX_BOARD    *ddp;
   GN_BOARD    *bp;
   GN_LOGDEV   *ldp;

   /* Check if the PM is started. */
   if ((Sram_State.srpm_flags & SR_PMINIT) == 0) {
      DLGC_MSG2(CE_WARN,"sramaddbd: PM not started\n");
      return (-1);
   }

   /* Validate the device dependent data sizes */
   if (nbds * sizeof(DX_INITBD) != ngsz) {
      DLGC_MSG2(CE_WARN,"sramaddbd: bad size, cannot add board\n");
      return (-1);
   }

   /* Get ptrs to device-dependent memory area */
   bp = gnbdp;
   edibp = ((DX_INITBD *)ngp) + nbds;

   /*
    * Initialize all the device-dependent BOARD and LOGDEV areas on this
    * PM. Remember that the Config Driver has already zeroed out the
    * device-dependent BOARD and LOGDEV structures prior to calling this
    * routine.
    */
   for (dibp = (DX_INITBD *)ngp; dibp < edibp; dibp++, bp = bp->bd_nbdp) {

      /* Point to the DX_BOARD structure for this BOARD */
      ddp = (DX_BOARD *)bp->bd_devdepp;

      /* Initialize the required DX_BOARD members */
      ddp->db_physaddr = dibp->dib_physaddr;
      ddp->db_totmem   = dibp->dib_totmem;
      ddp->db_ioport   = dibp->dib_ioport;

      /* Save the board's type */
      switch (ddp->db_phystype = dibp->dib_type) {
      case DI_SPAN:
         ddp->db_logtype = DI_D40;
         break;
      default:
         ddp->db_logtype = dibp->dib_type;
         break;
      }

      /* Initialize all the required DX_LOGDEV members on this BOARD */
      for (ldp = bp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
         sr_setqdefaults((DX_LOGDEV *)ldp->ld_devdepp);
      }
   }

   /*
   ** The following "for" loop must be done before sr_cfgchk(), which
   ** will set gbp_same_fwi_addr to the right GN_BOARD structure if
   ** more than one board share the same shared RAM.
   */
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {

      /* In here, set the variable to point back to itself.  By default,
      ** all virtual boards are assumed to have their own shared RAM
      ** address.  (However, in the case of the Span card, the DTI and 
      ** the first voice virtual board 1 share the same shared RAM area.
      ** If that is the case, the gbp_same_fwi_addr will be set the first
      ** voice virtual board.)
      */
      ((DX_BOARD *)(bp->bd_devdepp))->gbp_same_fwi_addr = bp;
   }

   /*
    * Check all boards for correct configuration including shared
    * memory segment overlaps; exit if incorrect.
    */
   if (sr_cfgchk(gnbdp, nbds) != 0) {
      DLGC_MSG2(CE_WARN,
         "sramaddbd: invalid configuration, cannot start board\n");
      return (-1);
   }

   /*
    * Add each board to the list for shared RAM mapping. Since the 
    * configuration was validated above, disable overlap checking.
    */
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
      ddp = (DX_BOARD *)bp->bd_devdepp;
      /* Add each board to the list for shared RAM mapping. */
      if (sr_mapadd(Sram_State.srpm_maplistp, bp, &(ddp->db_sr), SR_SHARESEG)
      != 0) {
         DLGC_MSG3(CE_WARN,"sramaddbd: cannot map board:%s\n",bp->bd_name);
      }

      /* Map additional boards into the kernel address space */
      if (sr_map2mem_add(Sram_State.srpm_maplistp, bp, &(ddp->db_sr)) != 0) {
         DLGC_MSG3(CE_WARN,"sramaddbd: mapping failed on board:%s\n",bp->bd_name);
      }
   }

   /* Return SUCCESS */
   return (0);
}


/***********************************************************************
 *        NAME: sramremovebd
 * DESCRIPTION: PM runtime environment remove board routine
 *      INPUTS: bdp - pointer to generic BOARD structure
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sramremovebd(gnbdp)
GN_BOARD *gnbdp;
{
   GN_BOARD    *bp;
   GN_BOARD    *ebdp;
   GN_LOGDEV   *ldp;
   DX_BOARD    *ddp;

   /* Make sure PM is running */
   if ((Sram_State.srpm_flags & SR_PMINIT) == 0) {
      DLGC_MSG2(CE_WARN,"sramremovebd: PM not started.\n");
      return (-1);
   }

   bp = gnbdp;

      ddp = (DX_BOARD *)bp->bd_devdepp;

      /* Make sure all LOGDEVs on the BOARD are closed and stopped */
      for (ldp = bp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
         if (ldp->ld_flags & (LD_START | LD_OPEN)) {
            DLGC_MSG3(CE_WARN,"sramremovebd: %s active, cannot stop board\n",
               ldp->ld_name);
            return (-2);
         }
      }

      /* Unmap shared-RAM of additional boards. */
      if (sr_map2mem_remove(Sram_State.srpm_maplistp, bp, &(ddp->db_sr)) != 0) {
         DLGC_MSG3(CE_WARN,"sramaddbd: cannot map board:%s\n",bp->bd_name);
      }

   /* NOTE: Don't need to clear any of the BOARD or LOGDEV data areas
    * since they will be freed on return from this routine.
    */

   /* Return SUCCESS */
   return (0);
}
#endif /* VME_LIVE_INSERTION */
#endif /* VME_SPAN */
#endif /* sun */
