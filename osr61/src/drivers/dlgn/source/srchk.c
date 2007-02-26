/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srchk.c
 * Description                  : SRAM prptocol - config/validations
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
#include "d40defs.h"
#include "dxdevdef.h"
#include "dxlibdrv.h"
#include "srmap.h"
#include "vrxlow.h"
#include "dxdrv.h"

#include "srcmd.h"
#include "srdefs.h"
#include "srmisc.h"
#include "srpm.h"

#include "srchk.h"

#include "srextern.h"
#include "drvdebug.h"

#include "dpci.h"


/*
 * Defines for board memory overlap checking
 */
#define OL_NONE                  0
#define OL_PARTIAL               1
#define OL_EXACT                 2


/*
 * Prototypes for this module's PRIVATE functions
 */
#ifdef LFS
PRIVATE int sr_chkd40(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkvr40(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkvr121(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkvr160(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkspan(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkT1span(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkE1span(GN_BOARD *, GN_BOARD *, ULONG);
PRIVATE int sr_chkovr(DX_BOARD *, DX_BOARD *);
PRIVATE int sr_setvr160dbid(GN_BOARD *, ULONG);
#else
PRIVATE int sr_chkd40();
PRIVATE int sr_chkvr40();
PRIVATE int sr_chkvr121();
PRIVATE int sr_chkvr160();
PRIVATE int sr_chkspan();
PRIVATE int sr_chkT1span();
PRIVATE int sr_chkE1span();
PRIVATE int sr_chkovr();
PRIVATE int sr_setvr160dbid();
#endif


/*
 * Macros for various checks/comparisons
 */
#define IS_FBAM(n)         ((n)->db_sr.br_ioport != DX_NOPORT)
#define IS_D4XE(n)         ((n)->db_sr.br_ioport <= D4XE_MAXLOC)
#define IS_SAMEADDR(n,m)   ((n)->db_sr.br_physaddr == (m)->db_sr.br_physaddr)
#define IS_SAMEPORT(n,m)   ((n)->db_sr.br_ioport == (m)->db_sr.br_ioport)
#ifdef SPARC_PCI
#define IS_BASEBD(n)       (!((ULONG)((n)->db_sr.br_physaddr) & 0x00007FFF))
#else	/* fix for 16K CSP firmware */
#define IS_BASEBD(n)       (!((ULONG)((n)->db_sr.br_physaddr) & 0xFFF07FFF))
#endif

/*
 * Global variable for global rearm sequence
 */
SR_BDSRAM *Sr_ReArmp = NULL;
#ifdef PCI_SPAN
extern PGP_CONTROL_BLOCK GpControlBlock;
#endif /* PCI_SPAN */

/**********************************************************************
 *        NAME: sr_cfgchk
 * DESCRIPTION: Validates given board configuration.
 *      INPUTS: gnbdp - pointer to first GN_BOARD structure in an
 *                array of GN_BOARD structures.
 *              nbds - number of GN_BOARD structures.
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: Only called at PM startup time.
 **********************************************************************/
int sr_cfgchk(gnbdp, nbds)
GN_BOARD *gnbdp;
ULONG    nbds;
{
   GN_BOARD *bdp = gnbdp;
   GN_BOARD *ebdp = bdp + nbds;
   DX_BOARD *dxbdp;
   int 	     indx;
#if 0
#ifdef VME_LIVE_INSERTION
   for (; bdp; bdp = bdp->bd_nbdp) {
#else
   for (; bdp < ebdp; bdp++) {
#endif /* VME_LIVE_INSERTION */
      dxbdp = (DX_BOARD *)bdp->bd_devdepp;

      switch (dxbdp->db_phystype) {
#ifndef VME_SPAN
      case DI_D40:
      case DI_D41:
         if (sr_chkd40(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
      case DI_VR40:
         if (sr_chkvr40(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
      case DI_VR121:
         if (sr_chkvr121(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
      case DI_VR160:
         if (sr_chkvr160(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
#endif /* VME_SPAN */
      case DI_SPAN:
         if (sr_chkspan(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
      case DI_T1SPAN:
#ifndef BRI_SUPPORT /* Check is bypassed to support BRI boards */
         if (sr_chkT1span(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
#endif /* BRI_SUPPORT */
         break;
      case DI_E1SPAN:
         if (sr_chkE1span(bdp, gnbdp, nbds) != 0) {
            return (-1);
         }
         break;
#ifdef DCB
      case DI_DCB:
      case DI_DCB320:
      case DI_DCB640:
      case DI_DCB960:
#endif
         break;
      default:
         DLGC_MSG4(CE_WARN,"sr_cfgck: board: %s: unknown type: 0x%x",
            bdp->bd_name, dxbdp->db_phystype);
         return (-1);
         break;
      }
   }
#endif 
#ifndef VME_SPAN
   /*
    * Set up Sr_ReArmp for the global rearm if there are any locator
    * boards in the system.
    */
   Sr_ReArmp = NULL;

   for (bdp = gnbdp; bdp < ebdp; bdp++) {
      dxbdp = (DX_BOARD *)bdp->bd_devdepp;

      switch (dxbdp->db_phystype) {
      case DI_D40:
      case DI_D41:
#if 0
         if (!IS_D4XE(dxbdp)) {
            continue;
         }
#endif
         break;
      case DI_SPAN:
         if (!IS_BASEBD(dxbdp)) {
            continue;
         }
         break;
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
         continue;
      }

      if (Sr_ReArmp == NULL) {
         Sr_ReArmp = &(dxbdp->db_sr);

#ifndef SPARC_PCI
#ifdef PCI_SPAN
    	 for (indx = 0; indx <= PCI_MAX_BOARDS; indx++) {
#ifdef LFS
            if (((int)(long)Sr_ReArmp->br_physaddr == (GpControlBlock->GpVirtMapBlock[indx].PhysAddr & 0xFFFF0000) ) || 
            ((int)(long)Sr_ReArmp->br_physaddr == (GpControlBlock->GpVirtMapBlock[indx].PhysAddr & 0xFFFFE000) ) ) {
#else
            if (((int)Sr_ReArmp->br_physaddr == (GpControlBlock->GpVirtMapBlock[indx].PhysAddr & 0xFFFF0000) ) || 
            ((int)Sr_ReArmp->br_physaddr == (GpControlBlock->GpVirtMapBlock[indx].PhysAddr & 0xFFFFE000) ) ) {
#endif
/* D82U board has two D4XE boards, to enable Global Rearm skip one board */
             if ( GpControlBlock->BoardAttrs[indx].BoardType == PCI_D82U_ID ) 
		 bdp++;

             Sr_ReArmp = NULL ;
             break;
            }
         }
#else
      } else {
         if (dxbdp->db_physaddr != Sr_ReArmp->br_physaddr) {
            dlgn_msg(CE_WARN,"%s: invalid address:0x%x, expected 0x%x",
               bdp->bd_name, dxbdp->db_physaddr, Sr_ReArmp->br_physaddr);
            return (-1);
         }
#endif /* PCI_SPAN */
#endif /* SPARC_PCI */
      }
   }

   return (sr_setvr160dbid(gnbdp,nbds));

#else /* VME_SPAN */

   return (0);

#endif /* VME_SPAN */
}


/**********************************************************************
 *        NAME: sr_chkd40
 * DESCRIPTION: Validates configuration for D4X boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkd40(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;


   /* Verify shared RAM size */
   /* Allow any #channels to support TTS */
   if (dxbdp->db_totmem != 0x2000) {
      return (-1);
   }

   /* If it's a D4XE board, check it against the rules for the D4XE */
   if (IS_D4XE(dxbdp)) {
      for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
         if (bp == bdp) continue;   /* Dont check against self! */
         dp = (DX_BOARD *)bp->bd_devdepp;
         if (IS_D4XE(dp) && !IS_SAMEADDR(dp,dxbdp)) {
            continue;
         } else {
            cnt++;
         }
         if (cnt == 0) {
            DLGC_MSG4(CE_WARN,"%s and %s: not at same address",
               bdp->bd_name, bp->bd_name);
            return(-1);
         }
         cnt = 0;
      }
      return (0);
   }

   /* If it's using FBAM, check against the rules for D4x using FBAM */
   if (IS_FBAM(dxbdp)) {
      /* May overlap with any board using FBAM */
      for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
         if (bp == bdp) continue;   /* Dont check against self! */
         switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
         case OL_EXACT:
         case OL_PARTIAL:
            if (!IS_FBAM(dp)) {
               DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
                  bdp->bd_name, bp->bd_name);
               return (-1);
            }
            break;
         default:
            continue;
            break;
         }
      }
      return (0);
   }

   /*
    * It's a non-FBAM D40/D41 board. May exactly overlap with one VR40
    * board and nothing else
    */
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
      if (bp == bdp) continue;   /* Dont check against self! */
      switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
      case OL_EXACT:
         if ((dp->db_phystype == DI_VR40) && (++cnt == 1)) {
            continue;
         }
         /* FALL THRU */
      case OL_PARTIAL:
         DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
            bdp->bd_name, bp->bd_name);
         return (-1);
         break;
      default:
         continue;
         break;
      }
   }
   return (0);
}


/**********************************************************************
 *        NAME: sr_chkvr40
 * DESCRIPTION: Validates configuration for VR40 boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkvr40(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;

   /* Verify shared RAM size */
   if (dxbdp->db_totmem != 0x2000) {
      return (-1);
   }

   /* May exactly overlap with up to one non-FBAM D4X board */
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
      if (bp == bdp) continue;   /* Dont check against self! */
      switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
      case OL_EXACT:
         if ( ((dp->db_phystype == DI_D40) || (dp->db_phystype == DI_D41)) &&
             !IS_FBAM(dp) && (++cnt == 1) ) {
            continue;
         }
         /* FALL THRU */
      case OL_PARTIAL:
         DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
            bdp->bd_name, bp->bd_name);
         return (-1);
         break;
      default:
         continue;
         break;
      }
   }

   return (0);
}


/**********************************************************************
 *        NAME: sr_chkvr121
 * DESCRIPTION: Validates configuration for VR121 boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkvr121(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;

   /* Verify #channels, shared RAM size */
   if (bdp->bd_nlogdevs != 13 || dxbdp->db_totmem != 0x2000) {
      return (-1);
   }


   if (IS_FBAM(dxbdp)) {
      /* Allowed to overlap with any other FBAM board */
      for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
         if (bp == bdp) continue;   /* Dont check against self! */
         switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
         case OL_EXACT:
         case OL_PARTIAL:
            if (!IS_FBAM(dp)) {
               DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
                  bdp->bd_name, bp->bd_name);
               return (-1);
            }
            break;
         default:
            continue;
            break;
         }
      }
      return (0);
   }

   /* Board is NOT using FBAM. May NOT overlap with any other boards */
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
      if (bp == bdp) continue;   /* Dont check against self! */
      switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
      case OL_EXACT:
      case OL_PARTIAL:
         DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
            bdp->bd_name, bp->bd_name);
         return (-1);
         break;
      default:
         continue;
         break;
      }
   }
   return (0);
}


/**********************************************************************
 *        NAME: sr_chkvr160
 * DESCRIPTION: Validates configuration for VR160 boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkvr160(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;

   /* Verify shared RAM size */
   if (dxbdp->db_totmem != 0x2000) {
      return (-1);
   }


   if (IS_FBAM(dxbdp)) {
      /* May overlap with any other FBAM board */
      for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
         if (bp == bdp) continue;   /* Dont check against self! */
         switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
         case OL_EXACT:
         case OL_PARTIAL:
            if (!IS_FBAM(dp)) {
               DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
                  bdp->bd_name, bp->bd_name);
               return (-1);
            }
            break;
         default:
            continue;
            break;
         }
      }
      return (0);
   }

   /* 
    * Board is NOT using FBAM. May exactly overlap with up to three
    * other non-FBAM VR160 boards
    */
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
      if (bp == bdp) continue;   /* Dont check against self! */
      switch (sr_chkovr(dxbdp, (dp = (DX_BOARD *)bp->bd_devdepp))) {
      case OL_EXACT:
         if ((dp->db_phystype == DI_VR160) && !IS_FBAM(dp) && (++cnt <= 3)) {
            continue;
         }
         /* FALL THRU */
      case OL_PARTIAL:
         DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
            bdp->bd_name, bp->bd_name);
         return (-1);
         break;
      default:
         continue;
         break;
      }
   }
   return (0);
}


/**********************************************************************
 *        NAME: sr_chkspan
 * DESCRIPTION: Validates configuration for SpanCard boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkspan(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;
   UCHAR    *addr;
   ULONG    loc;

   STRACE2("sr_chkspan: db_totmem=0x%x, db_ioport=0x%x", dxbdp->db_totmem, dxbdp->db_ioport);

   /* Verify shared RAM size */
#ifndef VME_SPAN
   if (dxbdp->db_totmem != 0x2000) {
      return (-1);
   }
#else
   if (dxbdp->db_totmem != SH_RAM_SIZE) {
      return (-1);
   }
#endif /* VME_SPAN */

   addr = dxbdp->db_physaddr;
   if ((loc = dxbdp->db_ioport) > 0xFF) {
      return (-1);
   }

#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
#endif /* VME_LIVE_INSERTION */
      if (bp == bdp) continue;   /* Dont check against self! */
      dp = (DX_BOARD *)bp->bd_devdepp;
      /* Addr/locator must be unique */
      if ((dp->db_physaddr == addr) && (dp->db_ioport == loc)) {
         switch(dp->db_phystype) {
         case DI_T1SPAN:
         case DI_E1SPAN:
            /* Allow ONE network board to have same addr/loc */
            if (++cnt == 1) {
               break;
            }
            /* FALL THRU */
         default:
            DLGC_MSG4(CE_WARN,"%s and %s: address/locator conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
            break;
         }
      }

      /* May overlap with any other FBAM board */
      switch (sr_chkovr(dxbdp, dp)) {
      case OL_EXACT:
      case OL_PARTIAL:
         if (!IS_FBAM(dp)) {
            DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
         }
         break;
      default:
         break;
      }
   }
   return (0);
}


/**********************************************************************
 *        NAME: sr_chkT1span
 * DESCRIPTION: Validates configuration for SpanCard T1 boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkT1span(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;
   UCHAR    *addr = dxbdp->db_physaddr;
   ULONG    loc;

#ifdef VME_SPAN
   STRACE3("sr_chkT1span: bd_nlogdevs=0x%x, db_totmem=0x%x, db_ioport=0x%x", bdp->bd_nlogdevs, dxbdp->db_totmem, dxbdp->db_ioport);

   /* Verify # channels, shared RAM size, addr on 0x10000 boundary */
   if (bdp->bd_nlogdevs != 25 || dxbdp->db_totmem != SH_RAM_SIZE) {
      return (-1);
   }
#endif /* VME_SPAN */

   if ((loc = dxbdp->db_ioport) > 0x1F) {
      return (-1);
   }

   /* May overlap with any other FBAM board */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
#endif /* VME_LIVE_INSERTION */
      if (bp == bdp) continue;   /* Dont check against self! */
      dp = (DX_BOARD *)bp->bd_devdepp;
      /* Addr/locator must be unique */
      if ((dp->db_physaddr == addr) && (dp->db_ioport == loc)) {
         switch(dp->db_phystype) {
         case DI_SPAN:
            /* Allow ONE SpanCard board to have same addr/loc */
            if (++cnt == 1) {
#ifdef VME_SPAN
               dxbdp->gbp_same_fwi_addr = bp;
               STRACE2("sr_chkT1span: DTI=0x%x, set gbp_same_fwi_addr=0x%x",
                  bdp, dp->gbp_same_fwi_addr);
#endif /* VME_SPAN */
               break;
            }
            /* FALL THRU */
         default:
            DLGC_MSG4(CE_WARN,"%s and %s: address/locator conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
            break;
         }
      }

      switch (sr_chkovr(dxbdp, dp)) {
      case OL_EXACT:
      case OL_PARTIAL:
         if (!IS_FBAM(dp)) {
            DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
         }
         break;
      default:
         break;
      }
   }
   return (0);
}

/**********************************************************************
 *        NAME: sr_chkE1span
 * DESCRIPTION: Validates configuration for SpanCard E1 boards
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if configuration is OK, -1 if not
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_chkE1span(bdp, gnbdp, nbds)
GN_BOARD *bdp;
GN_BOARD *gnbdp;
ULONG    nbds;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   register DX_BOARD *dp;
   register GN_BOARD *bp;
   int      cnt = 0;
   UCHAR    *addr = dxbdp->db_physaddr;
   ULONG    loc;

#ifdef VME_SPAN
   STRACE3("sr_chkE1span: bd_nlogdevs=0x%x, db_totmem=0x%x, db_ioport=0x%x", bdp->bd_nlogdevs, dxbdp->db_totmem, dxbdp->db_ioport);

   /* Verify # channels, shared RAM size, addr on 0x10000 boundary */
   if (bdp->bd_nlogdevs != 31 || dxbdp->db_totmem != SH_RAM_SIZE) {
      return (-1);
   }
#endif /* VME_SPAN */

   if ((loc = dxbdp->db_ioport) > 0x1F) {
      return (-1);
   }

   /* May overlap with any other FBAM board */
#ifdef VME_LIVE_INSERTION
   for (bp = gnbdp; bp; bp = bp->bd_nbdp) {
#else
   for (bp = gnbdp; bp < gnbdp + nbds; bp++) {
#endif /* VME_LIVE_INSERTION */
      if (bp == bdp) continue;   /* Dont check against self! */
      dp = (DX_BOARD *)bp->bd_devdepp;
      /* Addr/locator must be unique */
      if ((dp->db_physaddr == addr) && (dp->db_ioport == loc)) {
         switch(dp->db_phystype) {
         case DI_SPAN:
            /* Allow ONE SpanCard board to have same addr/loc */
            if (++cnt == 1) {
#ifdef VME_SPAN
               dxbdp->gbp_same_fwi_addr = bp;
               STRACE2("sr_chkE1span: DTI=0x%lx, set gbp_same_fwi_addr=0x%x",
                  bdp, dp->gbp_same_fwi_addr);
#endif /* VME_SPAN */
               break;
            }
            /* FALL THRU */
         default:
            DLGC_MSG4(CE_WARN,"%s and %s: address/locator conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
            break;
         }
      }

      switch (sr_chkovr(dxbdp, dp)) {
      case OL_EXACT:
      case OL_PARTIAL:
         if (!IS_FBAM(dp)) {
            DLGC_MSG4(CE_WARN,"%s and %s: board address conflict",
               bdp->bd_name, bp->bd_name);
            return (-1);
         }
         break;
      default:
         break;
      }
   }
   return (0);
}


/**********************************************************************
 *        NAME: sr_setvr160dbid
 * DESCRIPTION: Assigns daughterboard ID numbers to all VR160 boards.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  0 - if all boards set OK; -1 if error detected.
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_setvr160dbid(gnbdp,nbds)
GN_BOARD *gnbdp;
ULONG nbds;
{
   GN_BOARD *ebdp = gnbdp + nbds;
   GN_BOARD *bdp;
   GN_BOARD *tbdp;
   DX_BOARD *dxbdp;
   DX_BOARD *tdxbdp;
   ULONG    curaddr;
   ULONG    curport;
   ULONG    dbdcnt;

   /*
    * Go thru the array of GN_BOARD structures for the PM. For each
    * VR160 board, look thru the rest of the array for other VR160
    * boards at the same address and port. Assign the daughterboard ID
    * number to each VR160 in the order that is is found in the array.
    */
   for (bdp = gnbdp; bdp < ebdp; bdp++) {
      dxbdp = (DX_BOARD *)bdp->bd_devdepp;
      /*
       * If the board is not a VR160, or if it has already had a
       * daughterboard number assigned, then skip it.
       */
      if ((dxbdp->db_phystype != DI_VR160) || (dxbdp->db_vr160bd != 0)) {
         continue;
      }

      /*
       * A port/address pair identifies a specific physical VR160 board.
       * This is the first occurence of this port/address pair. Save
       * the port/address pair for use below and mark this board as the 
       * first daughterboard on the physical VR160 board.
       */
      curaddr = (ULONG) dxbdp->db_physaddr;
      curport = dxbdp->db_ioport;
      dxbdp->db_vr160bd = dbdcnt = 1;

      /*
       * Now search through the rest of the boards in the array, looking
       * for any additional VR160 boards at this port/address pair.
       */
      for (tbdp = bdp + 1; tbdp < ebdp; tbdp++) {
         tdxbdp = (DX_BOARD *)tbdp->bd_devdepp;

         /*
          * Skip this board if not a VR160 or if it has already been
          * assigned a daughterboard ID.
          */
         if ((tdxbdp->db_phystype != DI_VR160) || (tdxbdp->db_vr160bd != 0)) {
            continue;
         }

         /*
          * If this VR160 board is on the same physical VR160 board
          * as the one from above (port/address pair match), then 
          * record and increment the daughterboard ID.
          */
         if ((curaddr == (ULONG) tdxbdp->db_physaddr) &&
               (curport == tdxbdp->db_ioport) ) {
            /*
             * If we have too many daughterboards on this physical
             * board, then the given configuration is invalid.
             */
            if (dbdcnt < VR_MAXDBD160) {
               /* Record and increment the daughterboard ID */
               tdxbdp->db_vr160bd = 1 << dbdcnt;
               dbdcnt++;
            } else {
               DLGC_MSG4(CE_WARN,
                  "%s and %s: invalid configuration: too many daughterboards",
                  bdp->bd_name, tbdp->bd_name);
               return (-1);
            }
         }
      }
   }

   return (0);
}


/**********************************************************************
 *        NAME: sr_chkovr
 * DESCRIPTION: Checks for memory overlap between two boards.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: Does not check for segment wrap
 **********************************************************************/
PRIVATE int sr_chkovr(dp1, dp2)
DX_BOARD *dp1;
DX_BOARD *dp2;
{
   register ULONG b1 = (ULONG) dp1->db_physaddr;
   register ULONG e1 = b1 + dp1->db_totmem - 1;
   register ULONG b2 = (ULONG) dp2->db_physaddr;
   register ULONG e2 = b2 + dp2->db_totmem - 1;

   /* Check for exact overlap */
   if ((b1 == b2) && (e1 == e2)) {
      return OL_EXACT;
   }

   /* Check for partial overlap */
   if (((b2 <= e1) && (b2 >= b1)) || ((e2 >= b1) && (e2 <= e1))) {
      return (OL_PARTIAL);
   }

   return OL_NONE;
}

