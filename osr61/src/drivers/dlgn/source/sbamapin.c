/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbamapin.c
 * Description                  : board mapin functions
 *
 *
 **********************************************************************/
 
#ifdef LINUX
#include "dlgclinux.h"
#else
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>
#endif /* LINUX */

#include "typedefs.h"
#include "dxdevdef.h"
#include "sbamapin.h"

/*
 * NOTE: Following is all just for SB_MEMENABLE_Bit & SB_MIN_IOPORT defines
 * in sbapm.h. Suggest splitting up sbapm.h to reduce these interdependencies.
 * Also check for duplicates.
 */
#include "gndefs.h"
#include "gnmsg.h"
#include "gndrv.h"
#include "genboot.h"
#include "srmap.h"
#include "sbdlib.h"
#include "sbalibdrv.h"
#include "sbadefs.h"
#include "genbootdef.h"
#include "sbastart.h"
#include "sbapm.h"
#include "dlgcos.h"
#include "sbaextern.h"

#ifdef PCI_SPAN
#include "dpci.h"
#endif /* PCI_SPAN */

#ifdef SVR4_MOT
#include <sys/ddi.h>
#endif /* SVR4_MOT */

#define FBAM_RETRY                10
#ifdef NOSBA
#define SB_IOPORT_SLOTS		(((SB_MAX_IOPORT - SB_MIN_IOPORT) / 4) + 1 )
int	sbapmstarted = 0;
GLOBAL unsigned char	sbaSprgBdUseFbam[ SB_IOPORT_SLOTS ] = { 0 };
GLOBAL unsigned char	*sbaLocatorIDaddr[ D4XE_MAXLOC + 1 ] = { 0 };
int			sbaCurrIOport = DX_NOPORT;
#else
extern unsigned char        sbaSprgBdUseFbam[];
extern unsigned char        *sbaLocatorIDaddr[];
extern int sbaCurrIOport;
#endif /* NOSBA */
#ifdef PCI_SPAN
#ifdef LFS
GLOBAL GLOBAL_PCI_INFO GlobalPCIData[PCI_MAX_BOARDS] = { {0,}, };
#else
GLOBAL GLOBAL_PCI_INFO GlobalPCIData[PCI_MAX_BOARDS] = { 0 };
#endif
unsigned short PciCurrentBoardId = 0;
#endif /* PCI_SPAN */

#ifdef sun /*  */
USHORT	InDpcFlag = 0;
#endif

#ifndef VME_SPAN
#if (defined(__STDC__) || defined(__cplusplus))
/***********************************************************************
 *        NAME: sba_mapin
 * DESCRIPTION: Maps in an FBAM board. Does not attempt to map in the
 *              new board if: (1) the new board is not FBAM; or (2) the
 *              given IO port is the same as the current IO port.
 *      INPUTS: newport - IO port of the board to be mapped in or
 *              locator ID if board is a D4XE.
 *     OUTPUTS: 
 *     RETURNS: Returns the previous port if the new port was success-
 *              fully mapped in. Otherwise returns DX_NOPORT.
 *       CALLS: 
 *    CAUTIONS: Be aware that this routine is called VERY frequently
 *              and mostly at interrupt time. This routine can also
 *              called by other PM's. An interrupt routine that calls
 *              this function *MUST* save the old port value that this
 *              function will return. The interrupt routine *MUST* then
 *              call this function with the old port value prior to
 *              exiting in order to restore the previous mapping.
 *              THIS ROUTINE MUST *NOT* BE CALLED WITH AN INVALID PORT!
 *              
 *              THIS ROUTINE HAS BEEN COMPILED DOWN TO ASSEMBLY SOURCE
 *              AND THEN HAND-OPTIMIZED FOR MAXIMUM EFFICIENCY. ANY
 *              CHANGES MADE TO THIS ROUTINE MUST ALSO BE REFLECTED IN
 *              THE ASSEMBLY VERSION: sbamapin.s
 *              
 **********************************************************************/
#ifdef PCI_SPAN
int sba_mapin( register unsigned short newport, register unsigned short PciBoardId )
#else
unsigned short sba_mapin( register unsigned short newport )
#endif /* PCI_SPAN */
#else
#ifdef PCI_SPAN
int sba_mapin(newport, PciBoardId)
register unsigned short newport;
register unsigned short PciBoardId;
#else
unsigned short sba_mapin(newport)
register unsigned short newport;
#endif /* PCI_SPAN */
#endif
{
   int      retry;
   int      oldport;
   UCHAR    chkcmd;
   DLGCDECLARESPL(oldspl)
#ifdef sun
   extern kmutex_t inthw_lock;
#endif

#if defined(sun) && defined(ISA_SPAN)
   DLGCMUTEX_ENTER(&inthw_lock);
#else
   DLGCSPLSTR(oldspl);
#endif

#ifdef PCI_SPAN
   if ((newport == DX_NOPORT) ||  ((PciCurrentBoardId == PciBoardId) && (sbaCurrIOport == newport)) ){
#else
   if ((newport == DX_NOPORT) || (sbaCurrIOport == newport)) {
#endif

#if defined(sun) && defined(ISA_SPAN)
      DLGCMUTEX_EXIT(&inthw_lock);
#else
      DLGCSPLX(oldspl);
#endif
      return (DX_NOPORT);
   }

   if (newport <= D4XE_MAXLOC) {

      if ((sbaCurrIOport != DX_NOPORT) && (sbaCurrIOport > D4XE_MAXLOC)) {
         /*
          * Disable currently active SpringBoard
          */
         outb(sbaCurrIOport,inb(sbaCurrIOport) & ~(SB_MEMENABLE_Bit));

         /* Verify that the board has been disabled */
         retry = 0;
         while ((chkcmd = inb(sbaCurrIOport)) & SB_MEMENABLE_Bit) {
            outb(sbaCurrIOport, chkcmd & ~(SB_MEMENABLE_Bit));
            if (++retry > FBAM_RETRY) {
#if defined(sun) && defined(ISA_SPAN)
               DLGCMUTEX_EXIT(&inthw_lock);
#else
               DLGCSPLX(oldspl);
#endif
               DLGC_MSG3(CE_WARN,"sba_mapin(1): map failed at port: 0x%x",
                  sbaCurrIOport);
               return (DX_NOPORT);
            }
         }
      }

      /*
       * A single write will enable the selected D4xE board and
       * disable all others D4XE boards. There is no way to verify 
       * whether the board was actually mapped in or not.
       */
      if (sbaLocatorIDaddr[newport] > (unsigned char *)1 ) {
#ifdef LINUX
	writeb((UCHAR)newport, sbaLocatorIDaddr[newport]);
#else
         *((UCHAR *)(sbaLocatorIDaddr[newport])) = newport;
#endif /* LINUX */

#ifdef PCI_SPAN
      } else
           if (GlobalPCIData[PciBoardId].LocatorAddress > (unsigned char *)1){
#ifdef LINUX
	       writeb((UCHAR)newport,GlobalPCIData[PciBoardId].LocatorAddress);
#else
              *((UCHAR *)(GlobalPCIData[PciBoardId].LocatorAddress)) =(UCHAR)newport;
#endif /* LINUX */

#endif /* PCI_SPAN */
      } else {
#if defined(sun) && defined(ISA_SPAN)
         DLGCMUTEX_EXIT(&inthw_lock);
#else
	 DLGCSPLX(oldspl);
#endif
         return (DX_NOPORT);
      }

   } else {

      /* Don't try to map the SpringBoard if it's not using FBAM */
      if (sbaSprgBdUseFbam[((unsigned int)(newport - SB_MIN_IOPORT)) >> 2] == 0) {
#if defined(sun) && defined(ISA_SPAN)
         DLGCMUTEX_EXIT(&inthw_lock);
#else
	 DLGCSPLX(oldspl);
#endif
         return (DX_NOPORT);
      }

      if (sbaCurrIOport != DX_NOPORT) {

         if (sbaCurrIOport <= D4XE_MAXLOC) {
            /*
             * A single write of 0xFF will disable all D4XE boards.
             * There is no way to verify that the boards have actually
             * been disabled.
             */
            if (sbaLocatorIDaddr[sbaCurrIOport] > (unsigned char *)1 ) {
               * (UCHAR *)(sbaLocatorIDaddr[sbaCurrIOport]) = 0xFF;
#ifdef PCI_SPAN
            } else
              if (GlobalPCIData[PciBoardId].LocatorAddress > (unsigned char *)1) {
                  *((UCHAR *)(GlobalPCIData[PciBoardId].LocatorAddress)) = 0xFF;
#endif /* PCI_SPAN */
          } else {
#ifdef LFS
               DLGC_MSG4(CE_WARN,"sbaLocatorIDaddr[ sbaCurrIOport=%d ] = 0x%p ", 
                  sbaCurrIOport, sbaLocatorIDaddr[sbaCurrIOport] );
#else
               DLGC_MSG4(CE_WARN,"sbaLocatorIDaddr[ sbaCurrIOport=%d ] = 0x%x ", 
                  sbaCurrIOport, sbaLocatorIDaddr[sbaCurrIOport] );
#endif
            }

         } else {
            /*
             * Disable the currently active SpringBoard
             */
            outb(sbaCurrIOport,inb(sbaCurrIOport) & ~(SB_MEMENABLE_Bit));

            /* Verify that the board has been disabled */
            retry = 0;
            while ((chkcmd = inb(sbaCurrIOport)) & SB_MEMENABLE_Bit) {
               outb(sbaCurrIOport, chkcmd & ~(SB_MEMENABLE_Bit));
               if (++retry > FBAM_RETRY) {
#if defined(sun) && defined(ISA_SPAN)
 		  DLGCMUTEX_EXIT(&inthw_lock);
#else
 		  DLGCSPLX(oldspl);
#endif
                  DLGC_MSG3(CE_WARN,"sba_mapin(2): map failed at port: 0x%x",
                     sbaCurrIOport);
                  return (DX_NOPORT);
               }
            }
         }
      }

      /*
       * Enable the new SpringBoard
       */
      outb(newport,inb(newport) | SB_MEMENABLE_Bit);

      /* Verify that the board has been enabled */
      retry = 0;
      while (!((chkcmd = inb(newport)) & SB_MEMENABLE_Bit)) {
         outb(newport, chkcmd | SB_MEMENABLE_Bit);
         if (++retry > FBAM_RETRY) {
            sbaCurrIOport = DX_NOPORT;
#if defined(sun) && defined(ISA_SPAN)
	    DLGCMUTEX_EXIT(&inthw_lock);
#else
	    DLGCSPLX(oldspl);
#endif
            DLGC_MSG3(CE_WARN,"sba_mapin(3): map failed at port: 0x%x",
               newport);
            return (DX_NOPORT);
         }
      }
   }

#ifdef PCI_SPAN
   oldport = (int)PciCurrentBoardId;
   PciCurrentBoardId = PciBoardId;
   oldport = (oldport << 16) | sbaCurrIOport;
#else 
   oldport = sbaCurrIOport;
#endif

   sbaCurrIOport = newport;
#if defined(sun) && defined(ISA_SPAN)
   DLGCMUTEX_EXIT(&inthw_lock);
#else
   DLGCSPLX(oldspl);
#endif

   return (oldport);
}
#ifdef sun
#if (defined(__STDC__) || defined(__cplusplus))
/***********************************************************************
 *        NAME: sba_mapin
 * DESCRIPTION: Maps in an FBAM board. Does not attempt to map in the
 *              new board if: (1) the new board is not FBAM; or (2) the
 *              given IO port is the same as the current IO port.
 *      INPUTS: newport - IO port of the board to be mapped in or
 *              locator ID if board is a D4XE.
 *     OUTPUTS: 
 *     RETURNS: Returns the previous port if the new port was success-
 *              fully mapped in. Otherwise returns DX_NOPORT.
 *       CALLS: 
 *    CAUTIONS: Be aware that this routine is called VERY frequently
 *              and mostly at interrupt time. This routine can also
 *              called by other PM's. An interrupt routine that calls
 *              this function *MUST* save the old port value that this
 *              function will return. The interrupt routine *MUST* then
 *              call this function with the old port value prior to
 *              exiting in order to restore the previous mapping.
 *              THIS ROUTINE MUST *NOT* BE CALLED WITH AN INVALID PORT!
 *              
 *              THIS ROUTINE HAS BEEN COMPILED DOWN TO ASSEMBLY SOURCE
 *              AND THEN HAND-OPTIMIZED FOR MAXIMUM EFFICIENCY. ANY
 *              CHANGES MADE TO THIS ROUTINE MUST ALSO BE REFLECTED IN
 *              THE ASSEMBLY VERSION: sbamapin.s
 *              
 **********************************************************************/
int sba_mapin_intr( register unsigned short newport, register unsigned short PciBoardId )
#else
int sba_mapin_intr(newport, PciBoardId)
register unsigned short newport;
register unsigned short PciBoardId;
#endif
{

#ifndef sun /*  */
   int      oldspl;
#endif
   int      retry;
   int      oldport;
   UCHAR    chkcmd;

#ifndef sun /*  */
   oldspl = splstr();
#endif

#ifdef PCI_SPAN
   if ((newport == DX_NOPORT) ||  ((PciCurrentBoardId == PciBoardId) && 
					    (sbaCurrIOport == newport)) ){
#else
   if ((newport == DX_NOPORT) || (sbaCurrIOport == newport)) {
#endif

#ifndef sun /*  */
      splx(oldspl);
#endif
      return (DX_NOPORT);
   }

   if (newport <= D4XE_MAXLOC) {

      if ((sbaCurrIOport != DX_NOPORT) && (sbaCurrIOport > D4XE_MAXLOC)) {
         /*
          * Disable currently active SpringBoard
          */
         outb(sbaCurrIOport,inb(sbaCurrIOport) & ~(SB_MEMENABLE_Bit));

         /* Verify that the board has been disabled */
         retry = 0;
         while ((chkcmd = inb(sbaCurrIOport)) & SB_MEMENABLE_Bit) {
            outb(sbaCurrIOport, chkcmd & ~(SB_MEMENABLE_Bit));
            if (++retry > FBAM_RETRY) {
#ifndef sun /*  */
               splx(oldspl);
#endif
               dlgn_msg(CE_WARN,"sba_mapin(1): map failed at port: 0x%x",
                  sbaCurrIOport);
               return (DX_NOPORT);
            }
         }
      }

      /*
       * A single write will enable the selected D4xE board and
       * disable all others D4XE boards. There is no way to verify 
       * whether the board was actually mapped in or not.
       */
      if (sbaLocatorIDaddr[newport] > (unsigned char *)1 ) {
         *((UCHAR *)(sbaLocatorIDaddr[newport])) = newport;
#ifdef PCI_SPAN
      } else
           if (GlobalPCIData[PciBoardId].LocatorAddress > (unsigned char *)1) {         *((UCHAR *)(GlobalPCIData[PciBoardId].LocatorAddress)) =(UCHAR)newport;
#endif /* PCI_SPAN */
      } else {
#ifndef sun /*  */
         splx(oldspl);
#endif
#if 0
         dlgn_msg(CE_WARN,"sbaLocatorIDaddr[ newport=%d ] = 0x%x ", 
            newport, sbaLocatorIDaddr[newport] );
#endif
         return (DX_NOPORT);
      }

   } else {

      /* Don't try to map the SpringBoard if it's not using FBAM */
      if (sbaSprgBdUseFbam[((unsigned int)(newport - SB_MIN_IOPORT)) >> 2] == 0) {
#ifndef sun /*  */
       	  splx(oldspl);
#endif
         return (DX_NOPORT);
      }

      if (sbaCurrIOport != DX_NOPORT) {

         if (sbaCurrIOport <= D4XE_MAXLOC) {
            /*
             * A single write of 0xFF will disable all D4XE boards.
             * There is no way to verify that the boards have actually
             * been disabled.
             */

            if (sbaLocatorIDaddr[sbaCurrIOport] > (unsigned char *)1 ) {
               * (UCHAR *)(sbaLocatorIDaddr[sbaCurrIOport]) = 0xFF;
#ifdef PCI_SPAN
            } else
              if (GlobalPCIData[PciBoardId].LocatorAddress > (unsigned char *)1) {
                  *((UCHAR *)(GlobalPCIData[PciBoardId].LocatorAddress)) = 0xFF;
#endif /* PCI_SPA */
            } else {
               dlgn_msg(CE_WARN,"sbaLocatorIDaddr[ sbaCurrIOport=%d ] = 0x%x ", 
                  sbaCurrIOport, sbaLocatorIDaddr[sbaCurrIOport] );
            }

         } else {
            /*
             * Disable the currently active SpringBoard
             */
            outb(sbaCurrIOport,inb(sbaCurrIOport) & ~(SB_MEMENABLE_Bit));

            /* Verify that the board has been disabled */
            retry = 0;
            while ((chkcmd = inb(sbaCurrIOport)) & SB_MEMENABLE_Bit) {
               outb(sbaCurrIOport, chkcmd & ~(SB_MEMENABLE_Bit));
               if (++retry > FBAM_RETRY) {
#ifndef sun /*  */
                  splx(oldspl);
#endif
                  dlgn_msg(CE_WARN,"sba_mapin(2): map failed at port: 0x%x",
                     sbaCurrIOport);
                  return (DX_NOPORT);
               }
            }
         }
      }

      /*
       * Enable the new SpringBoard
       */
      outb(newport,inb(newport) | SB_MEMENABLE_Bit);

      /* Verify that the board has been enabled */
      retry = 0;
      while (!((chkcmd = inb(newport)) & SB_MEMENABLE_Bit)) {
         outb(newport, chkcmd | SB_MEMENABLE_Bit);
         if (++retry > FBAM_RETRY) {
            sbaCurrIOport = DX_NOPORT;
#ifndef sun /*  */
            splx(oldspl);
#endif
            dlgn_msg(CE_WARN,"sba_mapin(3): map failed at port: 0x%x",
               newport);
            return (DX_NOPORT);
         }
      }
   }

#ifdef PCI_SPAN
   oldport = (int)PciCurrentBoardId;
   PciCurrentBoardId = PciBoardId;
   oldport = (oldport << 16) | sbaCurrIOport;
#else
   oldport = sbaCurrIOport;
#endif

   sbaCurrIOport = newport;
#ifndef sun /*  */
   splx(oldspl);
#endif

   return (oldport);
}
#endif /* sun */
#endif /* VME_SPAN */
