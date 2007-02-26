/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srintr.c
 * Description                  : SRAM protocol - ISR
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
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
#include "gndlgn.h"

#include "d40defs.h"
#include "d40low.h"
#include "dxdevdef.h"
#include "dxlibdrv.h"
#include "fln.h"
#include "fx12low.h"
#include "srmap.h"

#include "dxdrv.h"
#include "vrxlow.h"

#include "srbulk.h"
#include "srmisc.h"
#include "srdefs.h"

#include "sbamapin.h"
/* Enable Voice View code */
#include "vvfwdefs.h"

/* Enable Phymdimple code */
#include "fxphym.h"

#include "dlgcos.h"
#include "srextern.h"
#include "drvdebug.h"

#ifdef PCI_SPAN
#include "dpci.h"
#endif /* PCI_SPAN */

#ifdef SVR4_MOT
#include <sys/ddi.h>
#endif /* SVR4_MOT */

#ifdef BIG_ENDIAN
#include "endian.h"
#define RWORD(x)	READ_WORD(&x)
#define WWORD(x, d)	WRITE_WORD(&x, d)
#endif /* BIG_ENDIAN */

/* EFN: Temporary for FBAM until SB PM provides a .h file */
#define DL_MSG_RDY_Bit           ((unsigned char)(0x40))

/* EFN: Temporary for SpanCard until DTI provides a .h file */
#define DTI_DEVNUM               0x02
#define DCP_CMD                  0x52


#define SR_NOREPLY               0xFFFF


/*
 * Macros to take the edge off some ugliness
 */
#ifdef BIG_ENDIAN /*  */
#define IS_DEVBD(a,b)      (((b) = RWORD((a)->dv_reply.dl_data3)) == DEV_BOARD)
#else
#define IS_DEVBD(a,b)      (((b) = (a)->dv_reply.dl_data3) == DEV_BOARD)
#endif /* BIG_ENDIAN */
#define IS_FLN(a)          ((a)->dv_reply.dl_cmd == FLN_ID)
#define IS_ROUTE(a)        (((a)->dv_reply.dl_cmd == DXXX_CH_ID) && \
                            ((a)->dv_reply.dl_sub == DXXX_ASSIGNTS_RPY))
#ifdef _ISDN
#ifdef BIG_ENDIAN /*  */
#define IS_ISDNBD(a)       (((a)->dv_reply.dl_cmd == ISDN_CMD) && \
                           (((RWORD((a)->dv_reply.dl_data3)) & 0x1F) == 0))
#else
#define IS_ISDNBD(a)       (((a)->dv_reply.dl_cmd == ISDN_CMD) && \
                           ((((a)->dv_reply.dl_data3) & 0x1F) == 0))
#endif /* BIG_ENDIAN */
#endif

/*
 * Macros to compute the correct FW buffer to use to retreive data
 */
#ifdef BIG_ENDIAN /*  */
#define GET_CHBUFNUM(n)          (RWORD((n)->bf_reply.dl_data) & 0x01)
#define GET_DVBUFNUM(n)          (RWORD((n)->dv_reply.dl_data) & 0x01)
#else
#define GET_CHBUFNUM(n)          ((n)->bf_reply.dl_data & 0x01)
#define GET_DVBUFNUM(n)          ((n)->dv_reply.dl_data & 0x01)
#endif /* BIG_ENDIAN */

/*
 * Macro to correctly read the byte count from the shared RAM. The
 * word value in dl_bcnt must have the upper 7 bits masked out.
 */
#ifndef BIG_ENDIAN /*  */
#define GET_BYTECNT(n)  ( (n) & 0x01FF )	/* max 512 bytes */
#else
#define GET_BYTECNT(n)  (RWORD(n))		/* max 7.5K bytes */
#endif /* BIG_ENDIAN */

#ifdef sun
#include <sys/mutex.h>
extern kmutex_t         inthw_lock;
#endif /*sun*/

#ifdef LINUX
#ifndef LFS
#include <sys/lislocks.h>
extern lis_spin_lock_t         inthw_lock;
#else
extern spinlock_t         inthw_lock;
#endif
extern int ft_dl;
#endif /*LINUX*/

/* following should be in d40low.h */
#ifndef DL_FWASSERT
#define DL_FWASSERT	0x77
#endif
#ifndef DL_PRSTATUS
#define DL_PRSTATUS	0x1D
#endif
#ifndef DL_PRRECORD	
#define DL_PRRECORD	0x1C
#endif

extern int in_intr_srvc;

/* fw assert data */
unsigned char fw_dbg_info[FW_DBG_SIZE] = { 0 };

#ifdef _STATS_
extern int StatisticsMonitoring;
extern SRAM_DRV_STATISTICS DriverStats;
#endif

/*
 * Prototypes for this module's PRIVATE functions
 */
#ifdef LFS
PRIVATE int sr_bdintr(GN_BOARD *, register DX_FWI *);
PRIVATE USHORT sr_chnreply(GN_LOGDEV *, CHNBUFF *, int *);
PRIVATE USHORT sr_devreply(GN_LOGDEV *, CHNBUFF *, DEVBUFF *, int *);
PRIVATE int sr_buildreply(GN_LOGDEV *, mblk_t *, DL_DRVCMD *, DX_FWI *);
PRIVATE int sr_ck160(DX_BOARD *, DEVBUFF *);
#else
PRIVATE int sr_bdintr();
PRIVATE USHORT sr_chnreply();
PRIVATE USHORT sr_devreply();
PRIVATE int sr_buildreply();
PRIVATE int sr_ck160();
#endif



/*
 * Private Module Global Data
 */
PRIVATE USHORT sztbl[] = {
#ifndef VME_SPAN
   4096, 2048, 1024, 512, 256, 128, 64, 16, 4
#else
   8192, 4096, 2048, 1024, 512, 256, 128, 64, 16, 4
#endif /* VME_SPAN */
};

PRIVATE ULONG global_bdmask = 0;
PRIVATE ULONG global_devnum = 0;

extern SR_BDSRAM *Sr_ReArmp;

#if (!defined(sun) && defined(PCI_SPAN))
extern int sramvec;
extern int sbavec;
#endif /* SVR3 */

#ifdef PCI_SPAN
extern PGP_CONTROL_BLOCK GpControlBlock;
#endif /* PCI_SPAN */

#ifdef sun /*  */
int
sramrecvcmd(GN_LOGDEV *ldp_fw_event)
{
   unsigned char i;
   USHORT ioport;
   D4XSRAM *srp;
   DX_BOARD *dxbdp;
   SR_IRQLIST *irqp;
   DLGCDECLARESPL(oldspl)

#ifdef PCI_SPAN
   int oldspl;
   volatile UCHAR *vaddr1, *vaddr2, *vaddr3, *vaddr4;
   UCHAR Toggle, ToggleValue = 0;
   int index;
   ULONG lvalue;
#endif /* PCI_SPAN */

#ifdef sun
   STRACE1("sramrecvcmd: ldp=0x%x\n", ldp_fw_event);
#endif /* sun */

   dxbdp = (DX_BOARD *)ldp_fw_event->ld_gbp->bd_devdepp;
   irqp = (SR_IRQLIST *)&dxbdp->db_irq;

    DLGCMUTEX_ENTER(&inthw_lock);

#ifndef VME_SPAN
   /* Activate the board's memory if it is using FBAM */
   if ((ioport = GET_IOPORT(ldp_fw_event->ld_gbp)) != DX_NOPORT) {
#ifdef PCI_SPAN
      sba_mapin_intr(ioport, GET_PCIBRDID(ldp_fw_event->ld_gbp));
#else
      sba_mapin_intr(ioport);
#endif /* PCI_SPAN */
   }
#endif /* VME_SPAN */

   sr_bdintr(ldp_fw_event->ld_gbp, dxbdp->db_fwi + ldp_fw_event->ld_devnum);

#ifdef sun
   /* Clear the soft int scheduling flag */
   ldp_fw_event->ld_flags &= ~LD_SOFT_INT;
#endif /* sun */

   srp = irqp->si_bdaddr;

   DLGCSPLSTR(oldspl);

   srp->dl_req = 0;	/* Clear the interrupts */

#ifdef sun
   i = srp->dl_req;	/* flush the write buffer in the processor */
#endif /* sun */

#ifdef _STATS_
    if (StatisticsMonitoring){
        DriverStats.InterruptCounter++;
    }
#endif /* _STATS_ */

   DLGCSPLX(oldspl);

   DLGCMUTEX_EXIT(&inthw_lock);
   return;
}
#endif /* sun */

/***********************************************************************
 *        NAME: sramintr
 * DESCRIPTION: SRAM PM interrupt handler routine
 *
 *              This routine is set up to handle the following host-to-
 *              board command protocol:
 *
 *              A. Channel device command via channel buffer area:
 *                    dl_reqcopy = Channel device/buffer number (1-4)
 *                    dl_data3 = undefined
 *
 *              B. Channel device command via DEV area:
 *                    dl_reqcopy = 0xFF
 *                    dl_data3 = Channel device no. (1-4)
 *
 *              C. Board device command via channel buffer area:
 *                    dl_reqcopy = 1 (Only uses channel #1 buffer area)
 *                    dl_data3 = undefined
 *
 *                 NOTE: This case is distinguished from (A) above by
 *                 setting dl_fwip->fwi_ldp to the ldp for the board
 *                 device that issued the command instead of NULL.
 *
 *              D. Board device command via DEV area:
 *                    dl_reqcopy = 0xFF
 *                    dl_data3 = 0xFF
 *
 *              SPECIAL CASE OF MF40 COMMANDS: These are all board-level
 *              commands that go thru the channel buffer #1 area:
 *                    dl_reqcopy = 0xFF
 *                    dl_data3 = 1
 *
 *                 This case is distinguished from (B) above by setting
 *                 fwip->fwi_ldp to the ldp for the device that sent the
 *                 command instead of NULL.
 *
 *      INPUTS: vecnum - the IRQ vector number
 *     OUTPUTS: none
 *     RETURNS: The number of devices serviced; 0 if no devices serviced
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifndef sun /*  */

#ifdef LFS
void sr_copy(char *s, char *d, int size);
#endif

#ifdef LFS
#ifdef PCI_SPAN
extern int sba_mapin( unsigned short NewBoard, unsigned short PciBoardId );
#else
extern unsigned short sba_mapin( unsigned short NewBoard );
#endif /* PCI_SPAN */
#endif

#ifdef LFS
int sramintr(int vecnum)
#else
int sramintr(vecnum)
int vecnum;
#endif
{
   register SR_IRQLIST  *irqp;
   USHORT   initialport = DX_NOPORT;
   register D4XSRAM     *srp;
   int      loc_cnt = 0;
#ifdef LFS
   int      rc = 0;
#else
   int      i, rc = 0;
#endif
   USHORT   prevport;
   UCHAR    devnum;
#ifdef PCI_SPAN
   int      indx;
   USHORT   initialid = 0;
   int      newid;
    
   volatile UCHAR *vaddr1, *vaddr2, *vaddr3, *vaddr4;
   UCHAR Toggle, ToggleValue = 0;
   int instance = 0;
   ULONG lvalue;

   /* Check for Dialogic Magnum Boards Only */
   if (vecnum >= 0x10) {
       instance = vecnum - 0x10;
   } else if ((sramvec != 0) && (sramvec == vecnum)) {
       instance = 0x10;
   } else if ((sbavec != 0) && (sbavec == vecnum)) {
       instance = 0x11;
   } else {
       return(0);
   }
#endif /* PCI_SPAN */

   for (irqp = Sr_Irq[vecnum]; irqp; irqp = irqp->si_nextp) {

      if ((prevport = irqp->si_ioport) != DX_NOPORT) {
         if (prevport > D4XE_MAXLOC) {
            /* Handle normally if not a LOCATOR board */
            if (inb(prevport) & DL_MSG_RDY_Bit) {
               /* There may be an interrupt on this board; map it in */
#ifdef PCI_SPAN
 	       newid = sba_mapin(prevport, instance);
               if (initialport == DX_NOPORT) {
                  initialport = (USHORT)(newid & 0x0000FFFF);
                  initialid   = (USHORT)(newid >> 16);
               } 
#else
               prevport = sba_mapin(prevport);
	       if (initialport == DX_NOPORT) 
		  initialport = prevport;
#endif
            } else {
               /* No interrupt on this board; go on to the next one. */
               continue;
            }
         } else {
            /* It's a LOCATOR board */
#ifdef PCI_SPAN
            newid = sba_mapin(prevport, instance);
            if (initialport == DX_NOPORT) {
                  initialport = (USHORT)(newid & 0x0000FFFF);
                  initialid   = (USHORT)(newid >> 16);
            }
#else
            prevport = sba_mapin(prevport);
	    if (initialport == DX_NOPORT) 
		initialport = prevport;
#endif /* PCI_SPAN */

         }
      }
   
      /*
       * Read dl_reqcopy from shared RAM and decide what to do.
       */
#ifdef LINUX
      srp = irqp->si_bdaddr;
      switch (readb(&srp->dl_reqcopy)) {
#else
      switch ((srp = (irqp->si_bdaddr))->dl_reqcopy) {
#endif /* LINUX */
      default:
      case 0:
         /* Interrupt is not for this board so go on to next one. */
         continue;
	 break;
   
      case 1:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
         continue;
         
         }

         /* Message is in board's CHANNEL #1 buffer */
         sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 1);
#ifdef LINUX
         newid = sba_mapin(prevport, instance);
         if (initialport == DX_NOPORT) {
             initialport = (USHORT)(newid & 0x0000FFFF);
             initialid   = (USHORT)(newid >> 16);
         }
#endif /* LINUX */
         break;
   
      case 2:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
            continue;
         }
         /* Message is in board's CHANNEL #2 buffer */
         sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 2);
#ifdef LINUX
         newid = sba_mapin(prevport, instance);
         if (initialport == DX_NOPORT) {
             initialport = (USHORT)(newid & 0x0000FFFF);
             initialid   = (USHORT)(newid >> 16);
         }
#endif /* LINUX */
         break;
   
      case 3:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
            continue;
         }
         /* Message is in board's CHANNEL #3 buffer */
         sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 3);
#ifdef LINUX
         newid = sba_mapin(prevport, instance);
         if (initialport == DX_NOPORT) {
             initialport = (USHORT)(newid & 0x0000FFFF);
             initialid   = (USHORT)(newid >> 16);
         }
#endif /* LINUX */
         break;
   
      case 4:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
            continue;
         }
         /* Message is in board's CHANNEL #4 buffer */
         sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 4);
#ifdef LINUX
         newid = sba_mapin(prevport, instance);
         if (initialport == DX_NOPORT) {
             initialport = (USHORT)(newid & 0x0000FFFF);
             initialid   = (USHORT)(newid >> 16);
         }
#endif /* LINUX */
         break;

      case 0x77:
	 {
	 /* Handle FW Assert by sending message to Host */
	 int      dev, j;
	 ULONG    fw_dbg_prev;
	 ULONG    fw_dbg_virt;
	 register GN_LOGDEV  *ldp;
	 register DX_FWI     *fwip;
	 register DEVBUFF    *dvbp;
	 register DX_BOARD   *dxbdp;
	 register SR_IRQLIST *tmp = irqp; /* save for later */
	 
	 cmn_err(CE_WARN, "Firmware Assert detected.\n");

	 /* find an OPEN ldp... */
	 fw_dbg_prev = 0xffffffff;
	 for (irqp = Sr_Irq[vecnum]; irqp; irqp = irqp->si_nextp) {
	      for (dev=1; dev<5; dev++) {
		   fwip = irqp->si_dxbdp->db_fwi + dev;

		   /* check to avoid panic */
#ifdef LFS
		   if (fwip->fwi_num == 0) {
		       continue;
 		   }
#else
		   if (fwip->fwi_num == NULL) {
		       continue;
 		   }
#endif

		   ldp = irqp->si_bdp->bd_ldp + fwip->fwi_num;
		   dxbdp = irqp->si_dxbdp;
		   dvbp  = (DEVBUFF *) dxbdp->db_fwi[dev].fwi_addr;

		   /* check to avoid panic */
                   if ((ldp == NULL) || (ldp->ld_gbp == NULL)) {
                        continue;
                   }

		   /* this board must be D/4X - causes duplicate messages */
                   if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
                       continue;
                   }

		   /* dump debug area for all asserted devices */
		   if (readb(&irqp->si_bdaddr->dl_req) == DL_FWASSERT) {
		       /* get the virtual address of debug area */
		       if ((dxbdp->db_logtype == PCI_MSI_ID)   ||
			   (dxbdp->db_logtype == PCI_PYTHON_ID)||
			   (dxbdp->db_logtype == PCI_D41JCT_ID)||
			   (dxbdp->db_logtype == PCI_COYOTE_ID)){
			   /* 2K Board */
			   fw_dbg_virt = (ULONG) dxbdp->db_sr.br_sramaddr+FW_DBG_2K_OFFSET;
		       } else if (dxbdp->db_logtype == PCI_D82U_ID) {
			   /* 4K Board */
			   fw_dbg_virt = (ULONG) dxbdp->db_sr.br_sramaddr+FW_DBG_4K_OFFSET;
		       } else {
			   /* 8K Board */
			   fw_dbg_virt = (ULONG) dxbdp->db_sr.br_sramaddr+FW_DBG_8K_OFFSET;
		       }

		       /* print per virtual address to avoid redundant prints */
		       if (fw_dbg_virt != fw_dbg_prev) {
			   fw_dbg_prev = fw_dbg_virt;

			   /* get the assert data to buffer */
#ifdef LFS
			   sr_copy((char *)fw_dbg_virt, (char *)fw_dbg_info, FW_DBG_SIZE);
#else
			   sr_copy(fw_dbg_virt, fw_dbg_info, FW_DBG_SIZE);
#endif

			   cmn_err(CE_CONT, "Debug Area for %s\n", ldp->ld_name);

			   /* print in hex */
			   for (j=0; j<FW_DBG_SIZE; j++) {
				cmn_err(CE_CONT, " %02X ", fw_dbg_info[j]);
				if (!((j+1)%16))
				    cmn_err(CE_CONT, "\n");
			   }

			   /* print name of file and line in ACSII */
			   j = fw_dbg_info[29] <<8;
			   j = j + fw_dbg_info[28];
			   cmn_err(CE_CONT, " Assert occurred at line %d in file ", j);
			   for (j=34; j<FW_DBG_SIZE; j++) {
				cmn_err(CE_CONT, "%c", fw_dbg_info[j]);
			   }
			   cmn_err(CE_CONT, "\n");
		       }
		   }

		   /* on current irq open device, send a message to host */
                   if ( (ldp->ld_gbp->bd_irq == vecnum) && ((ldp->ld_flags & LD_OPEN) != 0) ) {
		       dvbp->dv_reply.dl_cmd = DL_FWASSERT;
		       dvbp->dv_reply.dl_bcnt = FW_DBG_SIZE;
		       sr_bdintr(irqp->si_bdp, fwip);
		   }
	      } /* for all devs */
	 } /* for all irqs */

	 /* restore the irqp */
	 irqp = tmp;
	 break;
	 }
		      
      case 0xFF:
         {
         /* Point to the shared RAM's DEV area */
         register DX_BOARD *dxbdp = irqp->si_dxbdp;
         register DEVBUFF  *dvbp  = (DEVBUFF *) dxbdp->db_fwi[0].fwi_addr;
#ifdef BRI_SUPPORT
	 register USHORT  dslnum;
#endif /* BRI_SUPPORT */

         if (dvbp == NULL) { /* Fix for Interrupt sharing on UW7 */
            continue;
         }

         /*
          * Determine the message type and decide whether or not this
          * particular board should service it.
          */
         switch (dvbp->dv_reply.dl_cmd) {
#ifdef _ISDN
	 case ISDN_CMD:
#endif /* _ISDN */
#ifndef DCB
         case DTI_DEVNUM:
#endif /* DCB */
            /*
             * It's a network command (DTI) message. Do not service this
             * message if this is not a SpanCard network device.
             */
            if ((dxbdp->db_logtype & (DT_SPAN|__DT_DTI__)) != (DT_SPAN|__DT_DTI__)) {
               continue;
            }

#ifdef BRI_SUPPORT
            /*
             * This message could be from a BRI station.  We must match the
             * DSL number with the one stored in our DX_BOARD structure.
             */
            dslnum = dvbp->dv_reply.dl_lstat >> 3;
            if (dxbdp->db_dslnum != dslnum) {
               continue;
            }
#endif /* BRI_SUPPORT */

	    global_bdmask = 0;
            break;

#ifdef DCB
         case DTI_DEVNUM:
            if ((dxbdp->db_logtype & DI_DCB) == DI_DCB) {
               switch (dxbdp->db_logtype) {
               case DI_DCB320:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP1) {
                     continue;
                  }
                  break;

               case DI_DCB640:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP2) {
                     continue;
                  }
                  break;

               case DI_DCB960:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP1) {
                     continue;
                  }
                  break;

               default :
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != DCBBRD) {
                     continue;
                  }
                  break;
               } 
            }
 
            /*
             * It's a network command (DTI/ISDN) message. Do not service
             * this message if this is not a SpanCard network device.
             */
            if ((dxbdp->db_logtype & (DT_SPAN|__DT_DTI__)) != (DT_SPAN|__DT_DTI__)) {
               continue;
            }
            break;
#endif /* DCB */

         case VR_DEVNUM:
            /*
             * It's a VR message. If this is NOT a VR board, or if it
             * is a VR160 board but not the correct one for this
             * message, then skip it and check the next board.
             */
            if ((!(dxbdp->db_logtype & DT_VRXXX)) ||
               ((dxbdp->db_logtype == DI_VR160) && !sr_ck160(dxbdp,dvbp))) {
               continue;
            }
            break;

	 case  DCP_CMD:
            if (!(dxbdp->db_logtype & DI_DCP)) {
		continue;
	    }
	    break;

         default:
            /*
             * For all other messages, the board must be a D/4X. Also allow
             * the case for the vr_route() command which does not look like a
             * VR_DEVNUM command. In this case we must service it for a VR121
             * and a VR160 but NOT for a VR40.
             */
            switch (dxbdp->db_logtype) {
	    case DI_DCP:
            case DI_VR40:
            case DI_T1SPAN:
            case DI_E1SPAN:
               continue; 
               break;
            case DI_VR160:
               if (!sr_ck160(dxbdp,dvbp)) {
                  continue;
               }
		global_bdmask = 0;
               break;      /* Service it */
            default:
               break;      /* Service it */
            }
         }
   
         /* Process the interrupt using the FWI for the DEV area */
         sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi);
#ifdef LINUX
         newid = sba_mapin(prevport, instance);
         if (initialport == DX_NOPORT) {
             initialport = (USHORT)(newid & 0x0000FFFF);
             initialid   = (USHORT)(newid >> 16);
         }
#endif /* LINUX */
         break;
         }
      }
      /*
       * Perform the EOI sequence on the board
       */
#ifdef LINUX
      /*
       * gcc optimizes out these reads.  Use readb macro to take
       * care of it.  Reading them into the devnum variable doesn't fool
       * this compiler, especially when you never use it!
       * Change this at your own risk!!!
       */
      devnum = readb(&srp->dl_msgrdy); /* Clear interrupt line */
      srp->dl_reqcopy = 0;
      srp->dl_req = 0;
      devnum = readb(&srp->dl_req);
#else
      devnum = srp->dl_msgrdy;  /* Clear interrupt line */
      srp->dl_reqcopy = 0;
      srp->dl_req = 0;
      devnum = srp->dl_req;
#endif /* LINUX */

#ifdef _STATS_
      if (StatisticsMonitoring){
          DriverStats.InterruptCounter++;
      }
#endif /* _STATS_ */
      /* Increment count of boards that were serviced */
      rc++;

      /*
       * If this is a LOCATOR board, increment count of LOCATOR boards
       * that have been serviced.
       */
      if (irqp->si_ioport <= D4XE_MAXLOC) {
         loc_cnt++;
      }
   } 
      
     /* Restore previous board mapping */
      if (initialport != DX_NOPORT) {
#ifdef PCI_SPAN
	sba_mapin(initialport, initialid);
#else
        sba_mapin(initialport);
#endif /* PCI_SPAN */
      }

#ifdef PCI_SPAN 
        indx = instance;
 
    if (loc_cnt) 
    {
        if (GpControlBlock->BoardAttrs[indx].BusType == BUS_TYPE_PCI) 
        {
           vaddr2 = (UCHAR *)GpControlBlock->GpVirtMapBlock[indx].CfgVirtAddr;
           vaddr3 = (UCHAR *)GpControlBlock->GpVirtMapBlock[indx].VirtAddr;
           vaddr4 = vaddr2 + PCI_PLX_INTR_REG;  
	   if ((GpControlBlock->BoardAttrs[indx].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(indx))) {
               vaddr1 = vaddr2 = vaddr2 + PCIe_PLX_FF_REG;
	   } else {
               vaddr1 = vaddr2 = vaddr2 + PCI_PLX_FF_REG;
	   }

	   /* Coyote Boards */
	   if (GpControlBlock->BoardAttrs[indx].BoardType == PCI_COYOTE_ID) {
	      vaddr3 = vaddr3 + 0xE2;
	      *vaddr3 = D4XE_DATA;
	      return (rc);
	   } 

	   if ((GpControlBlock->BoardAttrs[indx].BoardType != PCI_PYTHON_ID) && 
	       (GpControlBlock->BoardAttrs[indx].BoardType != PCI_D41JCT_ID)) 
           {
             if (IS_SPANPLUS_TYPE(GpControlBlock->BoardAttrs[indx].BoardType)) 
             {
		lvalue = *(volatile ULONG *) vaddr4;
#ifdef DBG_PRT
    		cmn_err(CE_NOTE, "val = 0x%x, boardtype = 0x%x\n", lvalue, GpControlBlock->BoardAttrs[indx].BoardType);
#endif
                lvalue = lvalue | PLX_CLEAR;
		*(volatile unsigned long *)vaddr4 = lvalue ;		  
             } else {
              ToggleValue = *vaddr2;

              if (!ToggleValue) {
                 Toggle = BYTERESETFLIPFLOP;
              } else {
                 Toggle = ToggleValue & 0xFB;
              }
              *vaddr2 = Toggle;
 
              ToggleValue = *vaddr1;
 
              if (!ToggleValue) {
                 Toggle = BYTESETFLIPFLOP;
              } else {
                Toggle = ToggleValue | 0x04;
              }
              *vaddr1 = Toggle;
             }
              vaddr3 = vaddr3 + 0xE2;
              *vaddr3 = D4XE_DATA;
	   } else {	
             if (Sr_ReArmp && Sr_ReArmp->br_sramaddr) 
	        ((D4XSRAM *)Sr_ReArmp->br_sramaddr)->dl_rearm = D4XE_DATA;
	   }
        } else {

        /* If we serviced any LOCATOR boards, issue the global re-arm. */
        /* Global Rearm for ISA/PCI boards */
        if (Sr_ReArmp && Sr_ReArmp->br_sramaddr) {
	   ((D4XSRAM *)Sr_ReArmp->br_sramaddr)->dl_rearm = D4XE_DATA;
         }
        }
   }
#else
   if (loc_cnt && Sr_ReArmp && Sr_ReArmp->br_sramaddr) {
      ((D4XSRAM *)Sr_ReArmp->br_sramaddr)->dl_rearm = D4XE_DATA;
   }
#endif /* PCI_SPAN */

   return (rc);
}

#else /* sun  */

#ifdef VME_SPAN /*  */
int sramintr(vecnum)
int vecnum;
{
   register SR_IRQLIST  *irqp;
   register D4XSRAM     *srp;
   GN_BOARD *gbp;
   UCHAR    devnum;
   int      sched_err;
   int      i, rc = 0;

   extern int sr_spurrious_cnt;
   extern int sr_check_spurrious;
   extern int dlgn_vec[];
   int      rc_dl_msgrdy = 0;
   int      rc_pc_reqzero = 0;

   for (irqp = Sr_Irq[vecnum]; irqp; irqp = irqp->si_nextp) {

      /*
       * Read dl_reqcopy from shared RAM and decide what to do.
       */
      if ((srp = (irqp->si_bdaddr))->pc_dltype == 1) {

         rc_dl_msgrdy++;

         sched_err = 1;	/* Assume scheduling error or set just to deal
                        ** with the case of spurrious interrupt. */

         switch (srp->dl_reqcopy) {
         default:
         case 0:
            /* Interrupt is not for this board so go on to next one. */
#ifdef VME_DEBUG
            cmn_err(CE_NOTE, "spurrious DL_MSG int: dl_reqcopy=0x%x, 0x%x",
               srp->dl_reqcopy, irqp->si_bdp);
#endif /* VME_DEBUG */
            goto sr_clear_dl_msgrdy;
            break;
      
         case 1:
            /* For all other messages, the board must be a D/4X */
            if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
               goto sr_check_pc_req_int;
            }

            STRACE1("sramintr: DL_MSG_INTMASK CHN# 1 gbp=0x%lx", irqp->si_bdp);

            /* Message is in board's CHANNEL #1 buffer */
#ifdef sun
            /* If unable to schedule a software interrupt to process
            ** the firmware event, we cannot process the event in the
            ** interrupt handler because the mutexes are not set up
            ** to block HW interrupt. we must clear the interrupt.
            */
            if ((sched_err 
               = sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 1)) == 1) {
               cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
            }
#else
            if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 1)) == 1)
               sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 1);
#endif /* sun */
            break;
   
         case 2:
            /* For all other messages, the board must be a D/4X */
            if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
               goto sr_check_pc_req_int;
            }

            STRACE1("sramintr: DL_MSG_INTMASK CHN# 2 gbp=0x%lx", irqp->si_bdp);

            /* Message is in board's CHANNEL #2 buffer */
#ifdef sun
            if ((sched_err = 
               sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 2)) == 1) {
               cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
            }
#else
            if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 2)) == 1)
               sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 2);
#endif /* sun */
            break;
   
         case 3:
            /* For all other messages, the board must be a D/4X */
            if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
               goto sr_check_pc_req_int;
            }

            STRACE1("sramintr: DL_MSG_INTMASK CHN# 3 gbp=0x%lx", irqp->si_bdp);

            /* Message is in board's CHANNEL #3 buffer */
#ifdef sun
            if ((sched_err = 
               sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 3)) == 1) {
               cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
            }
#else
            if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 3)) == 1)
               sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 3);
#endif /* sun */
            break;
   
         case 4:
            /* For all other messages, the board must be a D/4X */
            if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
               goto sr_check_pc_req_int;
            }

            STRACE1("sramintr: DL_MSG_INTMASK CHN# 4 gbp=0x%lx", irqp->si_bdp);

            /* Message is in board's CHANNEL #4 buffer */
#ifdef sun
            if ((sched_err = 
               sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 4)) == 1) {
               cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
            }
#else
            if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 4)) == 1)
               sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 4);
#endif /* sun */
            break;
   
         case 0xFF:
            {
            /* Point to the shared RAM's DEV area */
            register DX_BOARD *dxbdp = irqp->si_dxbdp;
            register DEVBUFF  *dvbp  = (DEVBUFF *) dxbdp->db_fwi[0].fwi_addr;
   
            /*
             * Determine the message type and decide whether or not this
             * particular board should service it.
             */
            switch (dvbp->dv_reply.dl_cmd) {
#ifdef _ISDN
            case ISDN_CMD:
#endif
            case DTI_DEVNUM:
               /*
                * It's a network command (DTI) message. Do not service this
                * message if this is not a SpanCard network device.
                */
               if ((dxbdp->db_logtype & (DT_SPAN|__DT_DTI__)) != (DT_SPAN|__DT_DTI__)) {
#ifdef VME_DEBUG_2
               cmn_err(CE_NOTE, "dl_reqcopy is 0xFF, not SPAN or DTI, bd=0x%x",
                  irqp->si_bdp);
#endif /* VME_DEBUG_2 */
                 continue; 
               }
               break;

	    case  DCP_CMD:
            	  if (!(dxbdp->db_logtype & DI_DCP)) {
		      continue;
	    	  }
	    	  break;
   
            default:
               /*
                * For all other messages, the board must be a D/4X. Also allow
                * the case for the vr_route() command which does not look like a
                * VR_DEVNUM command. In this case we must service it for a VR121
                * and a VR160 but NOT for a VR40.
                */
               switch (dxbdp->db_logtype) {
               case DI_VR40:
               case DI_T1SPAN:
               case DI_E1SPAN:
                  goto sr_check_pc_req_int;
                  break;
               case DI_VR160:
                  if (!sr_ck160(dxbdp,dvbp)) {
#ifdef VME_DEBUG
                     cmn_err(CE_NOTE, "dl_reqcopy is 0xFF, is DI_VR160, bd=0x%x",
                        irqp->si_bdp);
#endif /* VME_DEBUG */
                     goto sr_clear_dl_msgrdy;
                  }
                  break;      /* Service it */
               default:
                  break;      /* Service it */
               }
            }
   
            STRACE1("sramintr: DL_MSG_INTMASK DEV gbp=0x%lx", irqp->si_bdp);

            /* Process the interrupt using the FWI for the DEV area */
#ifdef sun
            if ((sched_err = sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp)) == 1) {
               cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
            }
#else
            if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp)) == 1)
               sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi);
#endif /* sun */
            break;
            }
         }

sr_clear_dl_msgrdy:
         srp->pc_dltype = 0;
         if (srp->dl_reqcopy != 0)
            srp->dl_reqcopy = 0;

         devnum = srp->dl_msgrdy;  /* Clear interrupt line */

         if (sched_err == 1) {	/* Clear FW interrupt if scheduling failed */
            srp->dl_req = 0;
         }
      }
   
      /* Check and process PC_REQZERO interrupt if any.
      */
sr_check_pc_req_int:
      if (srp->pc_rztype == 1) {

         rc_pc_reqzero++;

         gbp = ((DX_BOARD *)(irqp->si_bdp->bd_devdepp))->gbp_same_fwi_addr;

         STRACE1("sramintr: PC_REQZERO_INT gbp=0x%lx", gbp);

         srp->pc_reqzero = gbp->bd_cmdcnt;
         srp->pc_rztype = 0;
         i = srp->pc_reqzero;

         /*
         ** After sending a command from the pending command queue,
         ** turn off PC_ZEROREQ interrupt if there are no more pending
         ** commands.
         */
         if (gbp->bd_cmds) {
#ifdef sun
            sr_schedule_send_DPC(gbp);
            srp->pc_intreq = 1;
#else
            if (sr_dispatch_pending_cmd(gbp) == 0) {
               srp->pc_intreq = 0;
            }
#endif /* sun */
         }
      }
   }

   /* Leave this code in for the production software just to check for 
   ** FW spurrious interrupt which is defined as that the interrupt mask
   ** is not set.  However, the code will not acknowledge hardware spurrious
   ** interrupt from a board that is not part of the configuration.
   */
   if (sr_check_spurrious == 1 && rc_dl_msgrdy == 0 && rc_pc_reqzero == 0) {

      if (dlgn_vec[vecnum] == -1) {
         cmn_err(CE_WARN, 
            "sramintr: SPAN spurrious interrupt, invalid bd id=%d\n", vecnum);
         return (rc);
      }

      sr_spurrious_cnt++;
      cmn_err(CE_WARN, "sramintr: SPAN spurrious interrupt, valid bd id=%d\n",
         vecnum);
   }
   
   return (rc);
}
#endif /* VME_SPAN */

#ifdef ISA_SPAN /*  */
int sramintr(vecnum)
int vecnum;
{
   register SR_IRQLIST  *irqp;
   register D4XSRAM     *srp;
   GN_BOARD *gbp;
   USHORT   initialport = DX_NOPORT;
   volatile UCHAR    devnum;
   int      sched_err;
   int      i, rc = 0;
   int      loc_cnt = 0;
   USHORT   prevport;

#ifdef PCI_SPAN
   volatile UCHAR *vaddr1, *vaddr2, *vaddr3, *vaddr4;
   UCHAR    Toggle, ToggleValue = 0;
   int      index = 0;
   USHORT   initialid = 0;
   int      newid;
   int      instance;
   ULONG    lvalue;
#endif /* PCI_SPAN */

   extern int sr_spurrious_cnt;
   extern int sr_check_spurrious;
   extern int dlgn_vec[];
   int      rc_dl_msgrdy = 0;
   int      rc_pc_reqzero = 0;
#if 0
   extern USHORT sba_mapin_intr();
#endif

#ifdef SPARC_PCI        /* for Antares support */
   if (!vecnum)
      return(0);

   instance = vecnum - 1;
#endif /* SPARC_PCI */
 
   for (irqp = Sr_Irq[vecnum]; irqp; irqp = irqp->si_nextp) {

      if ((prevport = irqp->si_ioport) != DX_NOPORT) {
         if (prevport > D4XE_MAXLOC) {
#ifndef SPARC_PCI
            /* Handle normally if not a LOCATOR board */
            if (inb(prevport) & DL_MSG_RDY_Bit) {
               /* There may be an interrupt on this board; map it in */

#ifdef PCI_SPAN
 	       newid = sba_mapin_intr(prevport, instance);
#else
               prevport = sba_mapin_intr(prevport);
#endif /* PCI_SPAN */
	       if (initialport == DX_NOPORT) 
               {
#ifdef PCI_SPAN
                  initialport = (USHORT)(newid & 0x0000FFFF);
                  initialid   = (USHORT)(newid >> 16 );
#else
		  initialport = prevport;
#endif /* PCI_SPAN */
	      }
            } else {
               /* No interrupt on this board; go on to the next one. */
               continue;
            }
#endif /* SPARC_PCI */
         } else {
            /* It's a LOCATOR board */
#ifdef PCI_SPAN
               newid = sba_mapin_intr(prevport, instance);
#else 
               prevport = sba_mapin_intr(prevport);
#endif /* PCI_SPAN */
               if (initialport == DX_NOPORT) 
               { 
#ifdef PCI_SPAN
                  initialport = (USHORT)(newid & 0x0000FFFF);
                  initialid   = (USHORT)(newid >> 16 );
#else 
                  initialport = prevport;
#endif /* PCI_SPAN */
              }  

         }
      }

#ifdef DEBUG
     cmn_err(CE_WARN, "value = %x, ioport = %x", irqp->si_dxbdp->db_logtype,
               irqp->si_ioport);
#endif

      /*
       * Read dl_reqcopy from shared RAM and decide what to do.
       */
      rc_dl_msgrdy++;

      sched_err = 1;	/* Assume scheduling error or set just to deal
                        ** with the case of spurrious interrupt. */

      switch ((srp = (irqp->si_bdaddr))->dl_reqcopy) {
      default:
      case 0:
         /* Interrupt is not for this board so go on to next one. */
#ifdef VME_DEBUG
         cmn_err(CE_NOTE, "spurrious DL_MSG int: dl_reqcopy=0x%x, 0x%x",
            srp->dl_reqcopy, irqp->si_bdp);
#endif /* VME_DEBUG */
         continue ;
         break;
   
      case 1:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
           continue; 
         }

         STRACE1("sramintr: DL_MSG_INTMASK CHN# 1 gbp=0x%lx", irqp->si_bdp);

         /* Message is in board's CHANNEL #1 buffer */
#ifdef sun
         /* If unable to schedule a software interrupt to process
         ** the firmware event, we cannot process the event in the
         ** interrupt handler because the mutexes are not set up
         ** to block HW interrupt. we must clear the interrupt.
         */
         if ((sched_err 
            = sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 1)) == 1) {
            cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
         }
#else
         if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 1)) == 1)
            sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 1);
#endif /* sun */
         break;

      case 2:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
         continue;
	}

         STRACE1("sramintr: DL_MSG_INTMASK CHN# 2 gbp=0x%lx", irqp->si_bdp);

         /* Message is in board's CHANNEL #2 buffer */
#ifdef sun
         if ((sched_err = 
            sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 2)) == 1) {
            cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
         }
#else
         if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 2)) == 1)
            sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 2);
#endif /* sun */
         break;

      case 3:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
           continue; 
         }

         STRACE1("sramintr: DL_MSG_INTMASK CHN# 3 gbp=0x%lx", irqp->si_bdp);

         /* Message is in board's CHANNEL #3 buffer */
#ifdef sun
         if ((sched_err = 
            sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 3)) == 1) {
            cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
         }
#else
         if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 3)) == 1)
            sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 3);
#endif /* sun */
         break;

      case 4:
         /* For all other messages, the board must be a D/4X */
         if ((irqp->si_dxbdp->db_logtype & DT_DXXX) == 0) {
           continue; 
         }

         STRACE1("sramintr: DL_MSG_INTMASK CHN# 4 gbp=0x%lx", irqp->si_bdp);

         /* Message is in board's CHANNEL #4 buffer */
#ifdef sun
         if ((sched_err = 
            sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp + 4)) == 1) {
            cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
         }
#else
         if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp + 4)) == 1)
            sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi + 4);
#endif /* sun */
         break;

      case 0xFF:
         {
         /* Point to the shared RAM's DEV area */
         register DX_BOARD *dxbdp = irqp->si_dxbdp;
         register DEVBUFF  *dvbp  = (DEVBUFF *) dxbdp->db_fwi[0].fwi_addr;

         if (dvbp == NULL) { /* Fix for BW */
            continue;
         }
         /*
          * Determine the message type and decide whether or not this
          * particular board should service it.
          */
         switch (dvbp->dv_reply.dl_cmd) {
#ifdef _ISDN
         case ISDN_CMD:
#endif
#ifndef DCB
         case DTI_DEVNUM:
#endif
            /*
             * It's a network command (DTI) message. Do not service this
             * message if this is not a SpanCard network device.
             */
            if ((dxbdp->db_logtype & (DT_SPAN|__DT_DTI__)) != (DT_SPAN|__DT_DTI__)) {
#ifdef VME_DEBUG_2
            cmn_err(CE_NOTE, "dl_reqcopy is 0xFF, not SPAN or DTI, bd=0x%x",
               irqp->si_bdp);
#endif /* VME_DEBUG_2 */
              continue; 
            }
            break;
#ifdef DCB
         case DTI_DEVNUM:
            if ((dxbdp->db_logtype & DI_DCB) == DI_DCB) {
               switch (dxbdp->db_logtype) {
               case DI_DCB320:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP1) {
                     continue;
                  }
                  break;

               case DI_DCB640:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP2) {
                     continue;
                  }
                  break;

               case DI_DCB960:
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != HIDSP1) {
                     continue;
                  }
                  break;

               default :
                  if (((dvbp->dv_reply.dl_data) & 0xFF00) != DCBBRD) {
                     continue;
                  }
#ifdef DEBUG
		  cmn_err(CE_WARN, "BOARD");
#endif
                  break;
               }
            }

            /*
             * It's a network command (DTI/ISDN) message. Do not service
             * this message if this is not a SpanCard network device.
             */
            if ((dxbdp->db_logtype & (DT_SPAN|__DT_DTI__)) != (DT_SPAN|__DT_DTI__)) {
               continue;
            }
            break;
#endif
	case  DCP_CMD:
             if (!(dxbdp->db_logtype & DI_DCP)) {
	         continue;
	     }
	     break;

         default:
            /*
             * For all other messages, the board must be a D/4X. Also allow
             * the case for the vr_route() command which does not look like a
             * VR_DEVNUM command. In this case we must service it for a VR121
             * and a VR160 but NOT for a VR40.
             */
            switch (dxbdp->db_logtype) {
            case DI_VR40:
            case DI_T1SPAN:
            case DI_E1SPAN:
               continue; 
               break;
            case DI_VR160:
               if (!sr_ck160(dxbdp,dvbp)) {
#ifdef VME_DEBUG
                  cmn_err(CE_NOTE, "dl_reqcopy is 0xFF, is DI_VR160, bd=0x%x",
                     irqp->si_bdp);
#endif /* VME_DEBUG */
                 continue; 
               }
		global_bdmask = 0;
               break;      /* Service it */
            default:
               break;      /* Service it */
            }
         }

         STRACE1("sramintr: DL_MSG_INTMASK DEV gbp=0x%lx", irqp->si_bdp);

         /* Process the interrupt using the FWI for the DEV area */
#ifdef sun
         if ((sched_err = sr_schedule_recv_DPC(irqp->si_bdp->bd_ldp)) == 1) {
            cmn_err(CE_NOTE, "srintr: unable to schedule software interrupt\n");
         }
#else
         if ((sched_err = dlgn_DPC(irqp->si_bdp->bd_ldp)) == 1)
            sr_bdintr(irqp->si_bdp, irqp->si_dxbdp->db_fwi);
#endif /* sun */
         break;
         }
      }

      /*
       * Perform the EOI sequence on the board
       */
      devnum = (volatile unsigned char)(srp->dl_msgrdy);
                                /* Clear interrupt line */

      if ((volatile unsigned char)(srp->dl_reqcopy) != 0)
         srp->dl_reqcopy = (volatile unsigned char)0;

      if (sched_err == 1) {	/* Clear FW interrupt if scheduling failed */
         srp->dl_req = (volatile unsigned char)0;
      }

      /* Increment count of boards that were serviced */
      rc++;

      /*
       * If this is a LOCATOR board, increment count of LOCATOR boards
       * that have been serviced.
       */
      if (irqp->si_ioport <= D4XE_MAXLOC) {
         loc_cnt++;
      }

     }

      /* Restore previous board mapping */
      if (initialport != DX_NOPORT) {
#ifdef PCI_SPAN
         sba_mapin_intr(initialport, initialid);
#else
         sba_mapin_intr(initialport);
#endif /* PCI_SPAN */
      }
   

   /* Leave this code in for the production software just to check for 
   ** FW spurrious interrupt which is defined as that the interrupt mask
   ** is not set.  However, the code will not acknowledge hardware spurrious
   ** interrupt from a board that is not part of the configuration.
   */
   if (sr_check_spurrious == 1 && rc_dl_msgrdy == 0) {

      if (dlgn_vec[vecnum] == -1) {
         cmn_err(CE_WARN, 
            "sramintr: SPAN spurrious interrupt, invalid bd id=%d\n", vecnum);
         return (rc);
      }

      sr_spurrious_cnt++;
#if 0 /* DIL change - not print when interrupt comes from Antares */
      cmn_err(CE_WARN, "sramintr: SPAN spurrious interrupt, valid bd id=%d\n",
         vecnum);
#endif
   }
  
#ifdef PCI_SPAN
     for (index = 0; index<=PCI_MAX_BOARDS; index++) 
     {
         if (vecnum == GpControlBlock->IntrBlock[index].Level) 
         {
            vaddr2 = (UCHAR *)GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr;
            vaddr3 = (UCHAR *)GpControlBlock->GpVirtMapBlock[index].VirtAddr;
            vaddr4 = vaddr2 + PCI_PLX_INTR_REG;  
            vaddr1 = vaddr2 = vaddr2 + PCI_PLX_FF_REG;
             
	  if ((GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID) && 
	      (GpControlBlock->BoardAttrs[index].BoardType != PCI_COYOTE_ID) && 
	      (GpControlBlock->BoardAttrs[index].BoardType != PCI_D41JCT_ID)) 
          {
             if (IS_SPANPLUS_TYPE(GpControlBlock->BoardAttrs[index].BoardType)) 
             {
		lvalue = *(volatile ULONG *) vaddr4;
		lvalue = lvalue | PLX_CLEAR;		  
		*(volatile unsigned long *)vaddr4 = lvalue ;		  
             } else {
            ToggleValue = *vaddr2;
 
            if (!ToggleValue) 
	    {
                Toggle = BYTERESETFLIPFLOP;
            } else {
                Toggle = ToggleValue & 0xFB;
            }
          
            *vaddr2 = Toggle;
            ToggleValue = *vaddr1;
 
            if (!ToggleValue) 
	    {
                Toggle = BYTESETFLIPFLOP;
            } else {
                Toggle = ToggleValue | 0x04;
            }
        
            *vaddr1 = Toggle;
           }
          }
	  break;
         }
     }

      vaddr3 = vaddr3 + 0xE2;
      *vaddr3 = D4XE_DATA;
#else 
 
   /* If we serviced any LOCATOR boards, issue the global re-arm. */
   if (loc_cnt && Sr_ReArmp && Sr_ReArmp->br_sramaddr) {
      ((D4XSRAM *)Sr_ReArmp->br_sramaddr)->dl_rearm = D4XE_DATA;
   }
#endif /* PCI_SPAN */

   return (rc);
}
#endif /* ISA_SPAN */
#endif /* sun */


/**********************************************************************
 *        NAME: sr_bdintr
 * DESCRIPTION: Device-specific interrupt handling function.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int sr_bdintr(bdp, fwip)
GN_BOARD *bdp;
register DX_FWI   *fwip;
{
   register    DEVBUFF     *dvbp;
   register    GN_LOGDEV   *ldp;
   GN_LOGDEV   *nldp;
   mblk_t      *mp;
   int         rc;
   int         stopreq = 0;
   USHORT      fwreply;
   UCHAR       devnum;
#ifndef LFS
   int	       z_flag;
#endif
#ifdef VME_LIVE_INSERTION
   short       num;
#endif /* VME_LIVE_INSERTION */

   STRACE2("sr_bdintr: bdp->bd_boardid=0x%lx, fwip->fwi_addr=0x%lx",
      bdp->bd_boardid, fwip->fwi_addr);

   if (fwip->fwi_type == IO_CHNFWI) {
      /*
       * This section handles a FW message in one of the FW's channel
       * buffer areas. The actual channel buffer is given by
       * fwip->fwi_num. At this point we also need to determine which
       * device sent the command so that the reply can be properly
       * routed to that device. In most cases the device will be the
       * same channel device indicated by fwip->fwinum; however the only
       * exception to this is that if fwip->fwi_ldp has been set (is
       * non-NULL) on this FWI when the command was issued, then
       * fwip->fwi_ldp contains the ldp for the device that issued the
       * command.
       */

      /*
       * Assume the command came from the channel device. nldp is used 
       * to retrieve the message from the shared RAM area.
       */
#ifdef VME_LIVE_INSERTION
      for (nldp = bdp->bd_ldp, num = fwip->fwi_num;
           num-- && nldp; nldp = nldp->ld_nldp)
         ;
#else
      nldp = bdp->bd_ldp + fwip->fwi_num;
#endif /* VME_LIVE_INSERTION */

      /*
       * See if the command came from the board device. ldp is the 
       * device that sent the command.
       */
      if ((ldp = fwip->fwi_ldp) == NULL) {
         /* The command came from the channel device */
         ldp = nldp;                /* Point to the channel device */
      } else {
         /* The command came from the board device */
         fwip->fwi_ldp = NULL;      /* Clear the ptr for next time */
      }

      /* Make sure that the device is still open; exit if not */
      if ((ldp->ld_flags & LD_OPEN) == 0) {

         STRACE3_QUEUE("sr_bdintr: CHN ldp=0x%lx, ! LD_OPEN - %d, %d",
            ldp, 0, 0);

         return (0);
      }

      STRACE3_QUEUE("sr_bdintr: CHN nldp=0x%x, ldp=0x%x, ldp->ld_devnum=%d",
         nldp, ldp, ldp->ld_devnum);

      /* Get the reply from out of the shared RAM channel buffer area */
#ifdef LFS
      if ((fwreply = sr_chnreply(nldp,(CHNBUFF *)fwip->fwi_addr,&stopreq))
            == SR_NOREPLY) {
#ifdef _STATS_
       if (StatisticsMonitoring){
           DriverStats.NumberOfUnSolEvents++; /* Channel Area UnSol */
       }
#endif /* _STATS_ */
         return (0);
      }
#else
      if ((fwreply = sr_chnreply(nldp,fwip->fwi_addr,&stopreq))
            == SR_NOREPLY) {
#ifdef _STATS_
       if (StatisticsMonitoring){
           DriverStats.NumberOfUnSolEvents++; /* Channel Area UnSol */
       }
#endif /* _STATS_ */
         return (0);
      }
#endif
   } else {

      dvbp = (DEVBUFF *)fwip->fwi_addr;

      /*
       * This section handles a FW message in the DEV buffer area of the
       * device's shared RAM area. At this point we need to determine
       * which device sent the command so that the reply can be properly
       * routed to that device. We must read dl_data3 in the DEV area;
       * it will be equal to DEV_BOARD if the command came from the
       * board device, otherwise it will contain the number of the
       * channel device that sent the command. The IS_DEVBD(a,b) macro
       * will return true if dl_data3 is set to DEV_BOARD; it will also
       * save the value of dl_data3 into its second parameter:(b). There
       * are also some special cases that must be checked: the IS_FLN
       * macro will return true if a FLN command is detected (this is
       * required for some early FW versions); the IS_ROUTE macro will
       * return true if a dx_route() command is detected. If any of
       * these macros return true, the command came from a board device;
       * otherwise the command is treated as having come from a channel
       * device. For a channel device command, the dl_data3 value is
       * equal to the channel device number. The only exception to this
       * is that if fwip->fwi_ldp has been set (is non-NULL) on this FWI
       * when the command was issued, then fwip->fwi_ldp contains the
       * ldp for the device that issued the command.
       */

#ifdef VME_LIVE_INSERTION_B
	/* Point to the board device */
	ldp = bdp->bd_ldp;

	/* if flag indicates of msg sent, check the reply and clr flag */
	if ((ldp->ld_flags & LD_COMTST_SENT) &&
			/* if the reply is NTT_COMRSP, clear the flag */
			(dvbp->dv_reply.dl_u.du_s.ds_data3 == NTH_TERM) &&
         		(dvbp->dv_reply.dl_cmd == NTH_DEVNUM) &&
         		(dvbp->dv_reply.dl_u.du_s.ds_sub == NTT_COMRSP)) {

		/* clr flag if good resp */
		if (dvbp->dv_reply.dl_lstat == NET_OK) {
			ldp->ld_flags &= ~LD_COMTST_SENT;
		}

		return (0);
	}
#endif /* VME_LIVE_INSERTION */

#ifdef _ISDN
      /*
       * If this interrupt is from an ISDN board, then determine if the
       * board belongs to an NFAS group by checking for IN_NFAS flag.  If
       * it is, then determine by traversing the linked list of B-Channel
       * boards, the GN_BOARD structure for the ISDN board whose interface id
       * matches the interface id in the reply.  This is the board whose
       * command was processed by the D-Channel board and whose ldp should
       * be used to send the reply back up.
       */
      if ((dvbp->dv_reply.dl_cmd == ISDN_CMD) &&
          ((((DX_BOARD *)bdp->bd_devdepp)->db_nfasflag & IN_NFAS) == IN_NFAS)) {
         while (((DX_BOARD *)bdp->bd_devdepp)->db_isdnintfcid !=
                                   (dvbp->dv_reply.dl_lstat & 0x1F)) {
            if ((bdp = ((DX_BOARD *)bdp->bd_devdepp)->db_bchanbdp) == NULL) {
               return 0;
            }
         }
      }

      if (IS_DEVBD(dvbp,devnum) || IS_FLN(dvbp) || IS_ROUTE(dvbp) ||
          IS_ISDNBD(dvbp))
#else
      if (IS_DEVBD(dvbp,devnum) || IS_FLN(dvbp) || IS_ROUTE(dvbp))
#endif
      {

         /* Point to the board device */
         ldp = nldp = bdp->bd_ldp;

         /* Make sure that the device is still open; exit if not */
         if ((ldp->ld_flags & LD_OPEN) == 0) {

            STRACE3_QUEUE("sr_bdintr: DEVICE ldp=0x%lx, ! LD_OPEN - %d,%d",
               ldp, 0, 0);

            return (0);
         }

         /* Read the reply out of the DEV area */
         fwreply = (USHORT) DV_REPDECODE(dvbp->dv_reply);
         STRACE1("sr_bdintr IS_DEVBD: fwreply (0x%lx)", fwreply);

      } else {

         DX_BOARD *dxbdp = (DX_BOARD *) bdp->bd_devdepp;

         /*
          * On a VR160, devnum can be 1 - 16. On a VR160C, devnum can be
          * 1 - 8. We must reduce devnum to a range of 1 - (#chn on bd)
          */
         if (dxbdp->db_logtype == DI_VR160) {
            devnum = global_devnum;
         }

#ifdef _ISDN
         /*
          * On an ISDN board, devnum byte has the 3 high bits reserved for
          * determining if the Call Id is present or not.  We need to mask
          * those out.
          */
         if (dvbp->dv_reply.dl_cmd == ISDN_CMD) {
            devnum &= 0x1F;
         }
#endif

         /*
          * Assume the command came from the channel device. nldp is used 
          * to retrieve the message from the shared RAM area.
          */
#ifdef VME_LIVE_INSERTION
         for (nldp = bdp->bd_ldp, num = devnum;
              num-- && nldp; nldp = nldp->ld_nldp)
            ;
#else
         nldp = bdp->bd_ldp + devnum;
#endif /* VME_LIVE_INSERTION */

         /*
          * See if the command came from the board device. ldp is the 
          * device that sent the command.
          */
         if ((ldp = fwip->fwi_ldp) == NULL) {
            /* The command came from the channel device */
            ldp = nldp;                /* Point to the channel device */
         } else {
            /* The command came from the board device */
            fwip->fwi_ldp = NULL;      /* Clear the ptr for next time */
         }

         /* Make sure that the device is still open; exit if not */
         if ((ldp->ld_flags & LD_OPEN) == 0) {

            STRACE1("sr_bdintr: DEVICE ldp=0x%lx, ! LD_OPEN", ldp);

            return (0);
         }

         STRACE3_QUEUE("sr_bdintr: DEV nldp=0x%x, ldp=0x%x, ldp->ld_devnum=%d",
            nldp, ldp, ldp->ld_devnum);

         /* Get the reply from out of the shared RAM area */
#ifdef LFS
         if ((fwreply = sr_devreply(nldp,
               (CHNBUFF *)(((DX_BOARD *)(bdp->bd_devdepp))->db_fwi + devnum)->fwi_addr,
               dvbp, &stopreq)) == SR_NOREPLY) {

#ifdef _STATS_
        if (StatisticsMonitoring){
            DriverStats.NumberOfUnSolEvents++; /* Dev Area UnSol */
        }
#endif /* _STATS_ */
            return (0);
         }
#else
         if ((fwreply = sr_devreply(nldp,
               (((DX_BOARD *)(bdp->bd_devdepp))->db_fwi + devnum)->fwi_addr,
               dvbp, &stopreq)) == SR_NOREPLY) {

#ifdef _STATS_
        if (StatisticsMonitoring){
            DriverStats.NumberOfUnSolEvents++; /* Dev Area UnSol */
        }
#endif /* _STATS_ */
            return (0);
         }
#endif
      }
   }

   /*
    * A solicited event is defined as one that occurs when we are
    * expecting a reply for the logical device in question, and the 
    * message that arrives is the message we're expecting. Anything
    * else is considered an unsolicited event.
    */

#ifdef _STATS_
	if (StatisticsMonitoring){
            DriverStats.NumberOfReplies++;
        }
#endif /* _STATS_ */

   if (ldp->ld_cmdcnt == 0) {
      if (ldp != nldp) {
         ldp = nldp;
      }
   }

   if ((mp = dlgn_findreply(ldp,fwreply,PM_DRVMSGSIZE)) == NULL) {
#ifdef _STATS_
        if (StatisticsMonitoring){
            DriverStats.LostMessagesFromFW++;
        }
#endif /* _STATS_ */
      return (1);
   }

   /*
    * If a stop request was set up by any of the functions called
    * earlier in this routine then issue the stop command now.
    */
   if (stopreq == SR_STOPREQUEST) {
      DLGC_VMEDEBUG_MSG3(CE_NOTE, "sr_bdintr: SR_STOPREQUEST ldp=0x%lx", ldp);
      sr_loopstop(mp,ldp);
   }

   rc = sr_buildreply(ldp, mp, (DL_DRVCMD *)mp->b_rptr, fwip);

   if (!in_intr_srvc) { 
      DLGCMUTEX_EXIT(&inthw_lock);
#if LINUX_VERSION_CODE < 0x020400
      DLGCSPINUNLOCK(inthw_lock, z_flag);
#endif
   }
   rc = sr_sendreply(ldp, mp, rc);
   if (!in_intr_srvc) {
       DLGCMUTEX_ENTER(&inthw_lock);
#if LINUX_VERSION_CODE < 0x020400
       DLGCSPINLOCK(inthw_lock, z_flag);
#endif
   }
   return rc;
}


/***********************************************************************
 *        NAME: sr_chnreply
 * DESCRIPTION: This function is called by sr_bdintr in order to
 *              determine what channel buffer event, if any, has
 *              occured. This is done by converting some CST events into
 *              unique reply types, as well as by doing special
 *              processing for play and record.
 *      INPUTS: ldp - pointer to logical device structure
 *              chbp - address of channel buffer FW interface
 *     OUTPUTS: Some event types have been converted to others in situ.
 *     RETURNS: SR_NOREPLY = no reportable event has occured; else,
 *              returns the event type which is deemed to have occured.
 *       CALLS: sr_getdata() min() sr_putsilence() sr_putdata()
 *              dlgn_msg() freemsg()
 *    CAUTIONS: Has special handling for play/record events. Also, it is
 *              expected that the firmware will support the translated
 *              event types someday.
 **********************************************************************/
PRIVATE USHORT sr_chnreply(ldp, chbp, stopreq)
GN_LOGDEV   *ldp;
CHNBUFF     *chbp;
int         *stopreq;
{
#ifndef LFS
   mblk_t      *dbp;
#endif
   DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   UCHAR       *dp;
   int         cnt2;
   ULONG       bufnum;
   ULONG       cnt;
   ULONG       ncnt;
   ULONG       threshold;
#ifndef LFS
   ULONG       mxrsz;
#endif
   USHORT      fwreply;
   USHORT      flags;

   /*
    * Check for certain special FW reply types in order to apply special
    * processing or conversions.
    */

   switch (fwreply = chbp->bf_reply.dl_cmd) {
   case DL_CST:
      /*
       * Convert certain CST flavors to be replies in their own right.
       * When FW catches up, this code will cease to be executed.
       */
#ifdef BIG_ENDIAN /*  */
      switch (RWORD(chbp->bf_reply.dl_data)) {
#else
      switch (chbp->bf_reply.dl_data) {
#endif /* BIG_ENDIAN */
      case CST_OFFHOOK:    /* Offhook complete */
         STRACE1("sr_chnreply: fwreply (0x%lx) is DL_OFFHOOKC", DL_OFFHOOKC);
         fwreply = chbp->bf_reply.dl_cmd = DL_OFFHOOKC;
#ifdef BIG_ENDIAN /*  */
         WWORD(chbp->bf_reply.dl_data, 0);
#else
         chbp->bf_reply.dl_data = 0;
#endif /* BIG_ENDIAN */
         break;

      case CST_ONHOOK:     /* Onhook complete */
         STRACE1("sr_chnreply: fwreply (0x%lx) is DL_ONHOOKC", DL_ONHOOKC);
         fwreply = chbp->bf_reply.dl_cmd = DL_ONHOOKC;
#ifdef BIG_ENDIAN /*  */
         WWORD(chbp->bf_reply.dl_data, 0);
#else
         chbp->bf_reply.dl_data = 0;
#endif /* BIG_ENDIAN */
         break;

      case CST_BUSY:    /* Call Analysis complete */
      case CST_NOANS:
      case CST_NORB:
      case CST_CNCT:
      case CST_CEPT:
      case CST_STOPD:
      case CST_NO_DIALTONE:
      case CST_DISCONNECT:
      case CST_HKFLSHRCVD:
      case CST_FAXTONE:
         STRACE1("sr_chnreply: fwreply (0x%lx) is DL_CALL", DL_CALL);
         fwreply = chbp->bf_reply.dl_cmd = DL_CALL;
         break;

      }
      break;

   case DL_B0EMPTY:
   case DL_B1EMPTY:
      /*
       * Handle play interrupts here. Move data from the buffer queue
       * until we run out. If we run out and user is not yet
       * done giving us voice data, play silence; else, move the last
       * partial buffer which will terminate the play. If we crossed
       * the 'threshold' boundary while copying the data, send up a
       * virtual buffer empty message; else, send up nothing.
       */

      bufnum = fwreply - DL_B0EMPTY;

      if ((ldp->ld_blockflags & (LD_BULKSEND | LD_BULKSTART)) !=
          (LD_BULKSEND | LD_BULKSTART)) {

#ifndef BIG_ENDIAN /*  */
         chbp->bf_cnt[bufnum] = 0;
#else
         WWORD(chbp->bf_cnt[bufnum], 0);
#endif /* BIG_ENDIAN */
         STRACE3_PLAY("sr_chnreply: ldp=0x%x, fwreply (0x%lx) is BUFEMPTY/SR_NOREPLY - 0x%x", ldp, SR_NOREPLY, 0); 
         fwreply = SR_NOREPLY;
         break; 
      }

      dp = chbp->bf_buff[bufnum];
      cnt = min((ULONG)dxldp->dl_sndcnt, (ULONG)BF_BUFSIZ);

      STRACE3_PLAY("sr_chnreply: ldp=0x%x, min of dl_sndcnt=0x%x, BF_BUFSIZ=0x%x",
         ldp, dxldp->dl_sndcnt, BF_BUFSIZ);

      if (((cnt < BF_BUFSIZ) && (cnt == 0)) && 
	  !(ldp->ld_blockflags & LD_BULKDONE)) {
#ifndef BIG_ENDIAN /*  */
         chbp->bf_cnt[bufnum] = BF_BUFSIZ;
#else
         WWORD(chbp->bf_cnt[bufnum], BF_BUFSIZ);
#endif /* BIG_ENDIAN */
         sr_putsilence(dp, BF_BUFSIZ);
         ldp->ld_lostevents++;
         ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_lostevents++;
         STRACE3_PLAY("sr_chnreply: ldp=0x%x, fwreply (0x%lx) is BUFEMPTY/SR_NOREPLY - 0x%x", ldp, SR_NOREPLY, 0);
      if (ldp->ld_flags & LD_WAITMORE) {
         fwreply = SR_NOREPLY;
         break;
      } else {
	ldp->ld_blockflags &= ~LD_PUTDONE;
	ldp->ld_flags |= LD_WAITMORE;
	fwreply = chbp->bf_reply.dl_cmd = DL_BUFEMPTY;
	break;
      }
    }
      /* Increment the count of #bytes played */
      ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_threshold += BF_BUFSIZ;

#ifndef BIG_ENDIAN /*  */
      chbp->bf_cnt[bufnum] = cnt;
#else
      WWORD(chbp->bf_cnt[bufnum], cnt);
#endif /* BIG_ENDIAN */
      sr_putdata(&dxldp->dl_sndq, dp, cnt);
      ncnt = dxldp->dl_sndcnt -= cnt;
      threshold = dxldp->dl_lowater;

      if (ncnt <= threshold && !(ldp->ld_flags & LD_WAITMORE)
           && !(ldp->ld_blockflags & LD_BULKDONE)) {
         /*
          * Indicate we're no longer up to date about library's needs
          */
         ldp->ld_blockflags &= ~LD_PUTDONE;
         ldp->ld_flags |= LD_WAITMORE;
         STRACE3_PLAY("sr_chnreply: ldp=0x%x, fwreply (0x%lx) is DL_BUFEMPTY - 0x%x", ldp, DL_BUFEMPTY, 0);
         fwreply = chbp->bf_reply.dl_cmd = DL_BUFEMPTY;

      } else {

         STRACE3_PLAY("sr_chnreply: ldp=0x%x, fwreply (0x%lx) is BUFEMPTY/SR_NOREPLY - 0x%x", ldp, SR_NOREPLY, 0);
         fwreply = SR_NOREPLY;
      }

      break;

   case DL_PRSTATUS:
      /* prompted record status - comptetion of play */
      chbp->bf_reply.dl_cmd = DL_PLAYBACK;

      /* switch this channel to receive mode */
      ldp->ld_blockflags |= LD_PRSTATUS;
      dxldp->dl_totrcvcnt = 0;
      chbp->bf_cnt[0] = 0;
      chbp->bf_cnt[1] = 0;
      
      /* handle as DL_PLAYBACK - continue */
      
   case DL_PLAYBACK:
      if ((ldp->ld_blockflags & (LD_BULKSEND | LD_BULKSTART)) !=
         (LD_BULKSEND | LD_BULKSTART)) {
         STRACE3_PLAY("sr_chnreply: ldp=0x%x fwreply (0x%lx) is PLAYBACK/SR_NOREPLY - 0x%x 0x%x", ldp, SR_NOREPLY, 0);
         fwreply = SR_NOREPLY;
         break;
      }
      STRACE3_PLAY("DL_PLAYBACK: ldp=0x%x, byte count = %d - 0x%x\n", 
                ldp, GET_BYTECNT(chbp->bf_reply.dl_bcnt), 0);

      /* Compute the number of bytes played. */
      ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_threshold +=
         GET_BYTECNT(chbp->bf_reply.dl_bcnt);

      /*
       * If we are up to date about library's needs, indicate that this
       * is no longer the case & that final library request is satisfied
       */
      if (ldp->ld_blockflags & LD_PUTDONE) {
	 if (ldp->ld_blockflags & LD_PRSTATUS)
             ldp->ld_blockflags = (ldp->ld_blockflags & ~LD_PUTDONE) | LD_BULKDONE | LD_PRSTATUS;
         else 
             ldp->ld_blockflags = (ldp->ld_blockflags & ~LD_PUTDONE) | LD_BULKDONE;
      }

      break;

   case DL_B0FULL:
   case DL_B1FULL:
      /* switch for prompted record */
      if (ldp->ld_blockflags & LD_PRSTATUS) {
	  ldp->ld_blockflags = LD_BULKRCV;
      }

      if ( (!((flags = ldp->ld_blockflags) & LD_BULKRCV)) ||
           (flags & LD_BULKDONE) ) {

         STRACE3_RECORD("sr_chnreply: channel=%d, fwreply (0x%lx) is BUFFULL/SR_NOREPLY - 0x%x", GET_CHBUFNUM(chbp), SR_NOREPLY, 0);
         fwreply = SR_NOREPLY;
         break;
      }

      /* Collect this buffer full of voice data and adjust count */
      if (sr_getdata(&dxldp->dl_rcvq,chbp->bf_buff[fwreply - DL_B0FULL],
          BF_BUFSIZ, sztbl[0]) != 0) {

         if (dxldp->dl_rcvq != NULL) {
            dxldp->dl_rcvcnt = msgdsize(dxldp->dl_rcvq);
         }

         ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_lostevents++;
         ldp->ld_lostevents++;
         STRACE3_RECORD("sr_chnreply: channel=%d, fwreply (0x%lx) is BUFFULL/SR_NOREPLY - 0x%x", GET_CHBUFNUM(chbp), SR_NOREPLY, 0);
         fwreply = SR_NOREPLY;

         break;
      }

      cnt = dxldp->dl_rcvcnt += BF_BUFSIZ;
      dxldp->dl_totrcvcnt += BF_BUFSIZ;

/* IBI */
#ifdef BIG_ENDIAN 
      WWORD(chbp->bf_cnt[fwreply - DL_B0FULL], 0);
#else
      chbp->bf_cnt[fwreply - DL_B0FULL] = 0;
#endif /* BIG_ENDIAN */
/* IBI */

#ifdef _STATS_
       if (StatisticsMonitoring){
           DriverStats.TotalBytesReadCounter += BF_BUFSIZ ; 
       }
#endif /* _STATS_ */


      /*
       * If library wants to wait for a record complete event, free up
       * any excess voice data and ignore this event.
       */

      /* Check to see if we have received all the data needed */
      if (dxldp->dl_totrcvcnt >= dxldp->dl_maxrcvsz) {
         /* 
          * At this point we need to issue a stop to the device but we
          * do not yet know which outstanding command has issued the
          * record. So set up the request for a stop here and let the
          * actual stop be issued later on in the interrupt handler
          * once we've discovered which command we are dealing with.
          */
         *stopreq = SR_STOPREQUEST;


         cnt = dxldp->dl_totrcvcnt - dxldp->dl_maxrcvsz;

         /* trim rcv que to right size */
         adjmsg(dxldp->dl_rcvq, -cnt);

         /* we're done with record */
         ldp->ld_blockflags |= LD_BULKDONE;

         /* tell buildreply() this is the last BUFFULL */
         STRACE3_RECORD("sr_chnreply: channel=%d, fwreply (0x%lx) is DL_LASTBUFFULL - 0x%x", GET_CHBUFNUM(chbp), DL_LASTBUFFULL, 0);
         fwreply = chbp->bf_reply.dl_cmd = DL_LASTBUFFULL;

      } else if (dxldp->dl_rcvcnt >= dxldp->dl_hiwater) {
         /* rcv queue is full */
         /* declare a virtual buffer full event */
         STRACE3_RECORD("sr_chnreply: channel=%d, fwreply (0x%lx) is DL_BUFFULL - 0x%x", GET_CHBUFNUM(chbp), DL_BUFFULL, 0);
         fwreply = chbp->bf_reply.dl_cmd = DL_BUFFULL;

      } else {
         /* still waiting to fill the rcv queue; don't do anything. */
         STRACE3_RECORD("sr_chnreply: channel=%d, fwreply (0x%lx) is BUFFULL/SR_NOREPLY - 0x%x", GET_CHBUFNUM(chbp), SR_NOREPLY, 0);
         fwreply = SR_NOREPLY;
      }
      break;

   case DL_PRRECORD:
      /* prompted record complete */
      chbp->bf_reply.dl_cmd = DL_RECORD;
      ldp->ld_blockflags = LD_BULKRCV;
      
      /* handle as DL_RECORD - continue */
   case DL_RECORD:      /* Record Complete event occured */
      /*
       * If record timed out, just ignore the event
       */
      if (!((flags = ldp->ld_blockflags) & LD_BULKRCV)) {
         STRACE3_RECORD("sr_chnreply: fwreply (0x%lx) is RECORD/SR_NOREPLY - 0x%x 0x%x",
            SR_NOREPLY, 0, 0);
         fwreply = SR_NOREPLY;
         break;
      }

      /*
       * Just send back DL_RECORD if RECORD was stopped intentionally 
       * either by the previous BUFFUL processing or by the application
       */
      if (flags & LD_BULKDONE) {
         break;
      }

      STRACE3_RECORD("DL_RECORD: channel number = %d, byte count = %d- 0x%x\n", 
                GET_CHBUFNUM(chbp), GET_BYTECNT(chbp->bf_reply.dl_bcnt), 0);

      /* Collect this partially full buffer of voice data & adj count */
      cnt = GET_BYTECNT(chbp->bf_reply.dl_bcnt);

      if (sr_getdata(&dxldp->dl_rcvq,chbp->bf_buff[GET_CHBUFNUM(chbp)],
          cnt, sztbl[0]) != 0) {
         if (dxldp->dl_rcvq != NULL) {
            dxldp->dl_rcvcnt = msgdsize(dxldp->dl_rcvq);
         }

      } else {

         dxldp->dl_totrcvcnt += cnt;
         dxldp->dl_rcvcnt += cnt;

         if ((cnt2 = dxldp->dl_totrcvcnt - dxldp->dl_maxrcvsz) > 0) {
            if (dxldp->dl_rcvq == NULL) {
               DLGC_MSG2(CE_WARN, "NULL POINTER: dxldp->dl_rcvq");
               DLGC_MSG4(CE_WARN, "Count: %d   cnt2: %d\n",
                   GET_BYTECNT(chbp->bf_reply.dl_bcnt), cnt2);
            } else {
               /* trim rcv que to right size */
               adjmsg(dxldp->dl_rcvq, -cnt2);
            }
         }
      }

#ifdef _STATS_
      if (StatisticsMonitoring){
          DriverStats.TotalBytesReadCounter += (unsigned int)cnt;
      }
#endif /* _STATS_ */

      /*
       * It is imperative that the LASTBUFFUL message is sent to
       * the SM before the DL_RECORD so that the SM clears the ldp
       * flags after it processes the LASTBUFFUL msg. Set the 
       * SENDLASTBUFF flag here to make sure this is done later.
       */
      ldp->ld_blockflags |=  (LD_BULKDONE | LD_SENDLASTBUFF);

      break;

   default:

      /* All other events, just return them as is */
      break;
   }

   return fwreply;
}



/***********************************************************************
 *        NAME: sr_devreply
 * DESCRIPTION: This function is called by sr_bdintr in order to
 *              determine what device area event, if any, has occured. 
 *              Special processing for send and receive fax.
 *      INPUTS: ldp - pointer to logical device structure
 *              chbp - address of channel buffer FW interface
 *     OUTPUTS: 
 *     RETURNS: SR_NOREPLY = no reportable event has occured; else,
 *              returns the event type which is deemed to have occured.
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE USHORT sr_devreply(ldp, chbp, devbp, stopreq)
GN_LOGDEV   *ldp;
CHNBUFF     *chbp;
DEVBUFF     *devbp;
int         *stopreq;
{
   DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
#ifndef LFS
   mblk_t      *dbp;
#endif
   UCHAR       *dp;
   ULONG       bufnum;
   ULONG       cnt;
   ULONG       ncnt;
   ULONG       threshold;
   USHORT      fwreply;
   USHORT      flags;
   USHORT      buffrpy;
 
   STRACE1("sr_devreply: DV_REPDECODE(devbp->dv_reply) = 0x%x\n",
         DV_REPDECODE(devbp->dv_reply));

   switch(fwreply = DV_REPDECODE(devbp->dv_reply))  {
      
   case FX_B0FULL_RPY:
   case FX_B1FULL_RPY:
   case VV_B0FULL_RPY:
   case VV_B1FULL_RPY:
      /*
       * If record timed out, just ignore the event
       */
      if (!((flags = ldp->ld_blockflags) & LD_BULKRCV)) {
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

      if (flags & LD_BULKDONE) {
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

      /*
       * Collect this buffer full of voice data and adjust count
       */
      if ((fwreply == FX_B0FULL_RPY) || (fwreply == FX_B1FULL_RPY)) {
         buffrpy = FX_B0FULL_RPY;
      } else {
         buffrpy = VV_B0FULL_RPY;
      }  

      if (sr_getdata(&dxldp->dl_rcvq,
         chbp->bf_buff[fwreply - buffrpy], BF_BUFSIZ,sztbl[0])
         != 0) {

         if (dxldp->dl_rcvq != NULL) {
            dxldp->dl_rcvcnt = msgdsize(dxldp->dl_rcvq);
         }

         ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_lostevents++;
         ldp->ld_lostevents++;
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;

         break;
      }

      cnt = dxldp->dl_rcvcnt += BF_BUFSIZ;
      dxldp->dl_totrcvcnt += BF_BUFSIZ;

      /*
       * If library wants to wait for a record complete event, free up
       * any excess voice data and ignore this event.
       */

      /* Check to see if we have received all the data needed */
      if (dxldp->dl_totrcvcnt >= dxldp->dl_maxrcvsz) {
         /* 
          * At this point we need to issue a stop to the device but we
          * do not yet know which outstanding command has issued the
          * record. So set up the request for a stop here and let the
          * actual stop be issued later on in the interrupt handler
          * once we've discovered which command we are dealing with.
          */
         *stopreq = SR_STOPREQUEST;

         cnt = dxldp->dl_totrcvcnt - dxldp->dl_maxrcvsz;

         /* trim rcv que to right size */
         adjmsg(dxldp->dl_rcvq, -cnt);

         /*
          * It is imperative that the LASTBUFFUL message is sent to
          * the SM before the DL_RECORD so that the SM clears the ldp
          * flags after it processes the LASTBUFFUL msg. Set the 
          * SENDLASTBUFF flag here to make sure this is done later.
          */
         ldp->ld_blockflags |=  (LD_BULKDONE | LD_SENDLASTBUFF);

      }
      else if (dxldp->dl_rcvcnt < dxldp->dl_hiwater) {
         /*
          * Receive queue is not full. Still waiting to fill the 
          * receive queue; don't do anything.
          */
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;

      }

      break;

   case FX_RCV_C_CMPLT_RPY:
   case VV_RCVPREP_CMPLT_RPY:
   case PHYM_RECV_CMPLT_RPY:

      /* If record timed out, just ignore the event */
      if (!((flags = ldp->ld_blockflags) & LD_BULKRCV)) {
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

      /*
       * Just send back DL_RECORD if RECORD was stopped intentionally 
       * either by the previous BUFFUL processing or by the application
       */
      if (flags & LD_BULKDONE) {
         break;
      }

      /*
       * Collect this partially full buffer of voice data and adjust
       * count
       */
      cnt = GET_BYTECNT(devbp->dv_reply.dl_bcnt);

      if (sr_getdata(&dxldp->dl_rcvq,
         chbp->bf_buff[GET_DVBUFNUM(devbp)],cnt,sztbl[0]) != 0) {
         if (dxldp->dl_rcvq!= NULL) {
            dxldp->dl_rcvcnt = msgdsize(dxldp->dl_rcvq);
         }
      } else {
         dxldp->dl_totrcvcnt += cnt;
         dxldp->dl_rcvcnt += cnt;
#if 0
         if ((cnt=dxldp->dl_totrcvcnt-dxldp->dl_maxrcvsz) > 0) {
            /* trim rcv que to right size */
            adjmsg(dxldp->dl_rcvq, -cnt);
         }
#endif
      }

      /*
       * It is imperative that the LASTBUFFUL message is sent to
       * the SM before the DL_RECORD so that the SM clears the ldp
       * flags after it processes the LASTBUFFUL msg. Set the 
       * SENDLASTBUFF flag here to make sure this is done later.
       */
      ldp->ld_blockflags |=  (LD_BULKDONE | LD_SENDLASTBUFF);

      break;

   case FX_B0EMPTY_RPY:
   case FX_B1EMPTY_RPY:
   case VV_B0EMPTY_RPY:
   case VV_B1EMPTY_RPY:
      if ((fwreply == FX_B0EMPTY_RPY) || (fwreply == FX_B1EMPTY_RPY)) {         buffrpy = FX_B0EMPTY_RPY;
      } else {
         buffrpy = VV_B0EMPTY_RPY;
      }  

      bufnum = fwreply - buffrpy;

      /* If play timed out, just ignore the event */
      if ((ldp->ld_blockflags & (LD_BULKSEND | LD_BULKSTART)) != 
            (LD_BULKSEND | LD_BULKSTART)) {
#ifndef BIG_ENDIAN /*  */
         chbp->bf_cnt[bufnum] = 0;
#else
         WWORD(chbp->bf_cnt[bufnum], 0);
#endif /* BIG_ENDIAN */
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

      dp = chbp->bf_buff[bufnum];
      cnt = min((ULONG)dxldp->dl_sndcnt, (ULONG)BF_BUFSIZ);

      STRACE2("sr_devreply: min of dl_sndcnt=0x%x, BF_BUFSIZ=0x%x\n",
         dxldp->dl_sndcnt, BF_BUFSIZ);

      if (cnt < BF_BUFSIZ && !(ldp->ld_blockflags & LD_BULKDONE)) {
#ifndef BIG_ENDIAN /*  */
         chbp->bf_cnt[bufnum] = BF_BUFSIZ;
#else
         WWORD(chbp->bf_cnt[bufnum], BF_BUFSIZ);
#endif /* BIG_ENDIAN */
         sr_putsilence(dp,BF_BUFSIZ);
         ldp->ld_lostevents++;
         ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_lostevents++;
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

#ifndef BIG_ENDIAN /*  */
      chbp->bf_cnt[bufnum] = cnt;
#else
      WWORD(chbp->bf_cnt[bufnum], cnt);
#endif /* BIG_ENDIAN */
      sr_putdata(&dxldp->dl_sndq,dp,cnt);
      ncnt = dxldp->dl_sndcnt -= cnt;
      threshold = dxldp->dl_lowater;
      
      if (ncnt <= threshold && !(ldp->ld_flags & LD_WAITMORE) 
         && !(ldp->ld_blockflags & LD_BULKDONE)) {
         /* 
          * Indicate we are no longer up to date about library's needs
          */
         ldp->ld_blockflags &= ~LD_PUTDONE;
         ldp->ld_flags |= LD_WAITMORE;
      } else {
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
      }

      break;
      
   case FX_SEND_C_CMPLT_RPY:
   case FX_SEND_C_PP_CMPLT_RPY:
   case FX_LOAD_FONT_CMPLT_RPY:
   case VV_XMITPREP_CMPLT_RPY:
   case PHYM_XMIT_CMPLT_RPY:

      if ((ldp->ld_blockflags & (LD_BULKSEND | LD_BULKSTART)) !=
         (LD_BULKSEND | LD_BULKSTART)) {
         STRACE1("sr_devreply: fwreply (0x%lx) is SR_NOREPLY", SR_NOREPLY);
         fwreply = SR_NOREPLY;
         break;
      }

      /*
       * Compute the amount of voice data sent to us by the library
       * which we were unable to play, and return this value in
       * dc_threshold.
       */
      ((DL_DRVCMD *)ldp->ld_blockmp->b_rptr)->dc_threshold =
#ifndef BIG_ENDIAN /*  */
         dxldp->dl_sndcnt + chbp->bf_cnt[0] + chbp->bf_cnt[1];
#else
         dxldp->dl_sndcnt + RWORD(chbp->bf_cnt[0]) + RWORD(chbp->bf_cnt[1]);
#endif /* BIG_ENDIAN */

      /*
       * If we are up to date about library's needs, indicate that
       * this is no longer the case, and that final library request
       * is satisfied.
       */
      if (ldp->ld_blockflags & LD_PUTDONE) {
         ldp->ld_blockflags = (ldp->ld_blockflags & ~LD_PUTDONE) | LD_BULKDONE;
      }

      break;

   default:

      break;

   }

   return(fwreply);
}


/***********************************************************************
 *        NAME: sr_buildreply
 * DESCRIPTION: Copy the reply message from the FW interface area to the
 *              reply packet. If there are variable data associated with
 *              this last reply, new packets of the required size are
 *              also allocated for it, the data are copied from the FW
 *              interface area to them, and the new packets are linked
 *              to the end of the current message with linkb(). Support
 *              special processing for play- and record- related events.
 *      INPUTS: ldp - pointer to logical device structure
 *              mp - pointer to reply message packet
 *              dcp - pointer to DL_DRVCMD structure
 *              fwip - pointer to FWI struct for relevant FW interface
 *     OUTPUTS: All reply data copied to reply packet.
 *     RETURNS: 0 = success;
 *              -1 = defer sending play or rec complete to user until
 *              next putmsg call; else error (DLE_NOMEM/DLE_TOOBIG).
 *       CALLS: sr_getdata() linkb()
 *    CAUTIONS: Has special handling for play/record events.
 **********************************************************************/
PRIVATE int sr_buildreply(ldp, mp, dcp, fwip)
GN_LOGDEV   *ldp;
mblk_t      *mp;
DL_DRVCMD   *dcp;
DX_FWI      *fwip;
{
   CHNBUFF        *chbp;
   DEVBUFF        *dvbp;
   GN_CMDMSG      *hdrp = &dcp->gn_drvhdr;
#ifndef LFS
   DL_DRVCMD      *dcp2;
#endif
   DX_LOGDEV      *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   mblk_t         *dmp = NULL;
   int            excess;
   int            rc = 0;
   USHORT         nfwbytes1;
   USHORT         nfwbytes2;
#ifdef DCB
   DEVBUFF1       *dvbp1;
#endif

   if (fwip->fwi_type == IO_CHNFWI) {

      chbp = (CHNBUFF *)fwip->fwi_addr;
      dcp->dc_reply = chbp->bf_reply;	/* assumed byte copy.
					** endian ok; lib will swap
					*/

      /* Command trace and filter */
      switch (ft_dl) {
         case -1:
            break;
         default:
            if (chbp->bf_reply.dl_cmd != ft_dl)
	        break;
         case 0:
            cmn_err(CE_CONT, "FW->HOST(CHN): %x %x %x %x %x %x %x %x (%s)\n",
		    chbp->bf_reply.dl_cmd, chbp->bf_reply.dl_lstat,
	            chbp->bf_reply.dl_data&0x00ff, (chbp->bf_reply.dl_data>>8),
	            chbp->bf_reply.dl_bcnt&0x00ff, (chbp->bf_reply.dl_bcnt>>8),
	            chbp->bf_reply.dl_sub, chbp->bf_reply.dl_data3,  ldp->ld_name);
            break;
      }	    

      switch (dcp->dc_reply.dl_cmd) {
      case DL_BUFEMPTY:
         hdrp->cm_msg_type = MT_VIRTEVENT; /* virtual event */
         hdrp->fw_msg_ident = MC_BUFFMT;
         break;

      case DL_PLAYBACK:
	 /* inform BDM to switch to receive for prompted record */
	 if (ldp->ld_blockflags & LD_PRSTATUS) {
	    hdrp->fw_msg_type |= DL_PRSTATUS;
 	 }

         /*
          * If play complete event, and final library request has been
          * satisfied, finish processing event now, with no data
          * appended.
          */
         if (ldp->ld_blockflags & LD_BULKDONE) {
            break;
         }

         /*
          * If final library request has not been satisfied because we
          * are not up to date regarding library's needs, indicate that
          * play complete event has occured, and delay further
          * processing of this event until we hear from the library or
          * until we timeout.
          */
         ldp->ld_blockflags |= LD_BULKDONE;
         rc = -1;
         break;

      case DL_BUFFULL:
         /*
          *  Send newly received data to the Bulk Data Module
          */
         hdrp->cm_msg_type = MT_BULK;
         hdrp->fw_msg_ident = MC_MOREDATA;
         dmp = (mblk_t *)dxldp->dl_rcvq;
         dxldp->dl_rcvq = NULL;
         dxldp->dl_rcvcnt = 0;
         break;

      case DL_LASTBUFFULL:
         /*
          *  Send last buffull msg to BDM
          */
         hdrp->cm_msg_type = MT_BULK;
         hdrp->fw_msg_ident = MC_LASTDATA;
         dmp = (mblk_t *)dxldp->dl_rcvq;
         dxldp->dl_rcvq = NULL;
         dxldp->dl_rcvcnt = 0;
         dxldp->dl_totrcvcnt = 0;
         dxldp->dl_maxrcvsz = 0;
         break;

      case DL_RECORD:

         /*
          * If the LD_SENDLASTBUF flag is set then we need to send an
          * LD_LASTDATA message to the BDM in order for it to be in the
          * correct state for the DL_RECORD message to follow.
          */
         if (ldp->ld_blockflags & LD_SENDLASTBUFF) {
            sr_lastbulk(mp,ldp,NULL);
            ldp->ld_blockflags &=  ~LD_SENDLASTBUFF;
         }

         if (!(ldp->ld_blockflags & LD_BULKDONE)) {
            ldp->ld_blockflags |= LD_BULKDONE;
            rc = -1;
         }
	 hdrp->fw_msg_ident = MC_RECEIVEDATA;
         break;


      case DL_CALL:
         /*
          * Retrieve data from shared RAM for Call Analysis
          */
         nfwbytes2 = dcp->dx_maxrepsize;

         if ((rc = sr_getdata(&dmp, chbp->bf_buff[1], nfwbytes2,
               nfwbytes2)) == 0) {
            dcp->dc_part2len = nfwbytes2;
            break;
         }

         if (dmp != NULL) {
            freemsg(dmp);
            dmp = NULL;
         }

         break;

      case DL_ERROR:

         DLGC_VMEDEBUG_MSG5(CE_NOTE, "sr_buildreply: DL_ERROR: ldp=0x%x, fwi_addr=0x%x, dl_data=0x%x",
            ldp, fwip->fwi_addr, dcp->dc_reply.dl_data);

         /* Set the message type to indicate an error. */
         hdrp->cm_msg_type = MT_ERRORREPLY;

         /* Mask out high byte of dl_data */
#ifndef BIG_ENDIAN
         dcp->dc_reply.dl_data &= 0x00FF;
#else
         dcp->dc_reply.dl_data &= 0xFF00;
#endif /* BIG_ENDIAN */

         /*
          * If the error occurred on a PC_SETTERMS command, we need to
          * zero out the fw_commandid and restore the device handle,
          * because PC_SETTERMS does not normally expect a reply.
          */
#ifdef BIG_ENDIAN /*  */
         if (RWORD(dcp->dc_reply.dl_data) == PC_SETTERMS) { 
            hdrp->fw_commandid = 0;
            hdrp->cm_address.da_handle = dxldp->dl_handle;
         }
#else
         if (dcp->dc_reply.dl_data == PC_SETTERMS) { 
            hdrp->fw_commandid = 0;
            hdrp->cm_address.da_handle = dxldp->dl_handle;
         }
#endif /* BIG_ENDIAN */

#ifdef _STATS_
        if (StatisticsMonitoring){
            DriverStats.NumberOfFWErrors++;
        }
#endif /* _STATS_ */
         break;

      case DL_FWASSERT:
	 /* setup the message */
	 hdrp->cm_event.rs_ident = DL_FWASSERT;
	 hdrp->cm_msg_type = MT_EVENT;

	 /* place the debug information to message block */
	 rc = sr_getdata(&dmp, fw_dbg_info, FW_DBG_SIZE, FW_DBG_SIZE);
	 if (dmp == NULL) {
#ifdef LFS
             cmn_err(CE_NOTE, "sr_buildreply: Unable to allocate message for FW Assert\n");
#else
             lis_printk("sr_buildreply: Unable to allocate message for FW Assert\n");
#endif
         } else {
             dmp->b_rptr = dmp->b_wptr - FW_DBG_SIZE;
         }

	 break;

	 
      default:
         /* DL_CST is the only recognized event */
         if (dcp->dc_reply.dl_cmd == DL_CST) {
            /* this part must be in close coordination with the
             * Library.  Since library sets the event mask to DL_CST,
             * we also set the event id to DL_CST 
             */
            hdrp->cm_event.rs_ident = DL_CST;
            hdrp->cm_msg_type = MT_EVENT;
         }

         /*
          * If any other message, collect data under the normal rules.
          */
#ifndef BIG_ENDIAN /*  */
         nfwbytes1 = chbp->bf_info[0];
         nfwbytes2 = chbp->bf_info[1];
#else
         nfwbytes1 = RWORD(chbp->bf_info[0]);
         nfwbytes2 = RWORD(chbp->bf_info[1]);
#endif /* BIG_ENDIAN */

         if (mp->b_datap->db_type != M_DATA) {
            excess =
               (nfwbytes1 + nfwbytes2) - dcp->dx_maxrepsize;
            if (excess > 0) {
               dcp->dx_maxrepsize = nfwbytes1 + nfwbytes2;
               if ((ULONG)excess <= (ULONG)nfwbytes2) {
                  nfwbytes2 -= excess;
               } else {
                  nfwbytes1 -= (excess - nfwbytes2);
                  nfwbytes2 = 0;
               }
            }

         } else {

            excess = 0;
         }

         if (nfwbytes1 > 0) {
            rc = sr_getdata(&dmp, chbp->bf_buff[0], nfwbytes1,
                nfwbytes1 + nfwbytes2);
         }

         if (rc == 0 && nfwbytes2 > 0) {
            rc = sr_getdata(&dmp, chbp->bf_buff[1], nfwbytes2,
               nfwbytes2);
         }

         if (rc == 0) {
            dcp->dc_part2len = nfwbytes2;

            if (excess > 0) {
               rc = DLE_TOOBIG;
            }

         } else if (dmp != NULL) {
            freemsg(dmp);
            dmp = NULL;
         }

         break;
      }

   } else {

      dvbp = (DEVBUFF *)fwip->fwi_addr;
      dcp->dc_reply = dvbp->dv_reply;	/* assumed byte copy.
					** endian ok; lib will swap
					*/
#ifdef DCB
      dvbp1 = (DEVBUFF1 *)fwip->fwi_addr;
#endif

      switch(DV_REPDECODE(dcp->dc_reply)) {

      case FX_B0EMPTY_RPY:
      case FX_B1EMPTY_RPY:
      case VV_B0EMPTY_RPY:   /* Voice View */
      case VV_B1EMPTY_RPY:
         hdrp->cm_msg_type = MT_VIRTEVENT;
         hdrp->fw_msg_ident = MC_BUFFMT;
         break;

      case FX_SEND_C_CMPLT_RPY:
      case FX_SEND_C_PP_CMPLT_RPY:
      case FX_LOAD_FONT_CMPLT_RPY:
      case VV_XMITPREP_CMPLT_RPY:   /* Voice View */
      case PHYM_XMIT_CMPLT_RPY:

         /*
          * If play complete event, and final library request has been
          * satisfied, finish processing event now, with no data
          * appended.
          */

         /* FAXCHG: examine the code here later */

         if (mp->b_datap->db_type != M_DATA ) {
            nfwbytes1 = min((ULONG)dcp->dx_maxrepsize, (ULONG)DV_BUFSIZ);
         } else {
            nfwbytes1 = DV_BUFSIZ;
         }

         if (nfwbytes1 > 0) {
            rc = sr_getdata(&dmp,dvbp->dv_repbf,nfwbytes1,nfwbytes1);
         }

         if (rc == 0) {
            dcp->dc_part2len = 0;
         } else if (dmp != NULL) {
            freemsg(dmp);
            dmp = NULL;
         }

         if (ldp->ld_blockflags & LD_BULKDONE) {
            break;
         }

         /*
          * If final library request has not been satisfied because we
          * are not up to date regarding library's needs, indicate that
          * play complete event has occured, and delay further
          * processing of this event until we hear from the library or
          * until timeout.
          */
         ldp->ld_blockflags |= LD_BULKDONE;
         rc = -1;

         break;
  
      case FX_B0FULL_RPY:
      case FX_B1FULL_RPY:
      case VV_B0FULL_RPY:
      case VV_B1FULL_RPY:

         /*
          * If the LD_SENDLASTBUF flag is set then we need to send an
          * LD_LASTDATA message to the BDM in order for it to be in the
          * correct state for the completion message to follow.
          */
         if (ldp->ld_blockflags & LD_SENDLASTBUFF) {
            sr_lastbulk(mp,ldp,dvbp);
            ldp->ld_blockflags &=  ~LD_SENDLASTBUFF;
         }

         /*
          *  Send newly received data to the Bulk Data Module
          */
         hdrp->cm_msg_type = MT_BULK;
         hdrp->fw_msg_ident= MC_MOREDATA;
         dmp = (mblk_t *)dxldp->dl_rcvq;
         dxldp->dl_rcvq = NULL;
         dxldp->dl_rcvcnt = 0;

         break;

      case FX_RCV_C_CMPLT_RPY:
      case VV_RCVPREP_CMPLT_RPY:
      case PHYM_RECV_CMPLT_RPY:
 
         /*
          * If the LD_SENDLASTBUF flag is set then we need to send an
          * LD_LASTDATA message to the BDM in order for it to be in the
          * correct state for the completion message to follow.
          */
         if (ldp->ld_blockflags & LD_SENDLASTBUFF) {
            sr_lastbulk(mp,ldp,dvbp);
            ldp->ld_blockflags &=  ~LD_SENDLASTBUFF;
         }

         /* FAXCHG: examine the code here later */

         if (mp->b_datap->db_type != M_DATA ) {
            nfwbytes1 = min((ULONG)dcp->dx_maxrepsize, (ULONG)DV_BUFSIZ);
         } else {
            nfwbytes1 = DV_BUFSIZ;
         }

         if (nfwbytes1 > 0) {
            rc = sr_getdata(&dmp,dvbp->dv_repbf,nfwbytes1,nfwbytes1);
         }

         if (rc == 0) {
            dcp->dc_part2len = 0;
         } else if (dmp != NULL) {
            freemsg(dmp);
            dmp = NULL;
         }

         if (ldp->ld_blockflags & LD_BULKDONE) {
            break;
         }

         ldp->ld_blockflags |= LD_BULKDONE;
         rc = -1;

         break;

      default:

	 if (dvbp->dv_reply.dl_cmd == DL_FWASSERT) {
	     /* setup the message */
	     hdrp->cm_event.rs_ident = DL_FWASSERT;
	     hdrp->cm_msg_type = MT_EVENT;

	     /* place the debug information to message block */
	     rc = sr_getdata(&dmp, fw_dbg_info, FW_DBG_SIZE, FW_DBG_SIZE);
             if (dmp == NULL) {
#ifdef LFS
                 cmn_err(CE_NOTE, "srintr: Unable to allocate message for FW Assert\n");
#else
                 lis_printk("srintr: Unable to allocate message for FW Assert\n");
#endif
             } else {
                 dmp->b_rptr = dmp->b_wptr - FW_DBG_SIZE;
             }

	     /* no need to process further */
	     break;
	 }

         if (mp->b_datap->db_type != M_DATA) {
#ifdef _ISDN
            /*
             * For ISDN boards, number of bytes returned by firmware can be
             * upto 272 bytes which is DV_ISDNBUFSIZ.
             */
            if (dvbp->dv_reply.dl_cmd == ISDN_CMD)
               nfwbytes1 = min((ULONG)dcp->dx_maxrepsize, (ULONG)DV_ISDNBUFSIZ);
            else
#endif

#ifdef DCB
               /* Check for DCB board and Active Talker event
                * Also determine the exact data based on DSP number
                * The number of bytes for DCB is 192
                */
            if (((dxldp->dl_type & DI_DCB) == DI_DCB) &&
                (dvbp->dv_reply.dl_sub == DCB_AT))
               nfwbytes1 = min((ULONG)dcp->dx_maxrepsize, (ULONG)DV_DCBBUFSIZ);
            else
#endif
               nfwbytes1 = min((ULONG)dcp->dx_maxrepsize, (ULONG)DV_BUFSIZ);
         } else {
#ifdef _ISDN
            /*
             * For ISDN boards, number of bytes returned by firmware can be
             * upto 272 bytes which is DV_ISDNBUFSIZ.
             */
            if (dvbp->dv_reply.dl_cmd == ISDN_CMD)
               nfwbytes1 = DV_ISDNBUFSIZ;
            else
#endif

#ifdef DCB
            if (((dxldp->dl_type & DI_DCB) == DI_DCB) &&
                (dvbp->dv_reply.dl_sub == DCB_AT))
               nfwbytes1 = DV_DCBBUFSIZ;
            else
#endif
            nfwbytes1 = DV_BUFSIZ;
         }

         if (nfwbytes1 > 0) {
#ifdef _ISDN
            /*
             * For ISDN boards, data is read from the 272 byte buffers beyond
             * the traditional 48 byte buffers in the device area.
             */
            if (dvbp->dv_reply.dl_cmd == ISDN_CMD)
               rc = sr_getdata(&dmp, dvbp->dv_isdnrepbf, nfwbytes1, nfwbytes1);
            else
#endif
#ifdef DCB
            /* Check whether the event is Active Talker Update if so
             * Check which DSP table updated. The assumption is the dl_data
             * byte can be 1 or 2. (DSP1 , DSP3  or DSP2)
             */
            if (((dxldp->dl_type & DI_DCB) == DI_DCB) &&
                (dvbp->dv_reply.dl_sub == DCB_AT))
               if ((dvbp->dv_reply.dl_data & 0xFF00) == HIDSP2) {
                  rc = sr_getdata(&dmp, dvbp1->dv_dcbrepbf2, nfwbytes1, nfwbytes1);
               } else {
                  rc = sr_getdata(&dmp, dvbp1->dv_dcbrepbf1, nfwbytes1, nfwbytes1);
               }
	    else
#endif
            rc = sr_getdata(&dmp, dvbp->dv_repbf, nfwbytes1, nfwbytes1);
         }

         if (rc == 0) {
            dcp->dc_part2len = 0;
         } else if (dmp != NULL) {
            freemsg(dmp);
            dmp = NULL;
         }

         break;
      }

   }

   /* Link the control and data portions of the reply if appropriate */
   if (dmp != NULL) {
      linkb(mp, dmp);
   }

   return rc;
}


/**********************************************************************
 *        NAME: sr_ck160
 * DESCRIPTION: Checks whether the given VR message in the shared RAM
 *              is meant for the specified VR160 board. 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:  nonzero if message is meant for this board; 0 if not.
 *       CALLS: 
 *    CAUTIONS: Should only be called for VR160 boards.
 **********************************************************************/
PRIVATE int sr_ck160(dxbdp, dvbp)
DX_BOARD *dxbdp;
DEVBUFF  *dvbp;
{

#ifndef VME_SPAN

   register DX_REPLY *drpp = &(dvbp->dv_reply);
   ULONG chnum = drpp->dl_data3;
   ULONG bdmask;
   DX_BOARD *tmpdxbdp = dxbdp;
   int       tmpchan;

   /*
    * Get the bitmask to determine which VR160 daughterboard should get
    * this message.
    */
   if (chnum >= 1 && chnum <= 32) {
      /*
       * For channel commands, compute daughterboard ID based upon
       * the given channel number.
       */
      /*
       * Channel computation should be based upon the number of channels which
       * is 4 for the VR160 and 2 for the VR160C. The driver currently uses
       * the DI_VR160 type to represent both of these boards.
       */
      if (tmpdxbdp->db_vr160bd == 1) {
         global_bdmask = 1;
         global_devnum = 0;
         tmpchan = 0;
         while ((tmpchan + tmpdxbdp->db_irq.si_bdp->bd_nlogdevs - 1) < chnum) {
            tmpchan += tmpdxbdp->db_irq.si_bdp->bd_nlogdevs - 1;
            global_bdmask <<= 1;
            tmpdxbdp++;
         }
         global_devnum = chnum - tmpchan;
      }
      bdmask = global_bdmask;
   } else {
      /*
       * For board commands, the daughterboard ID is in the message.
       * If it's an unrecognized message, set up to service the
       * message on all daughterboards.
       */
      switch (drpp->dl_sub) {
      case VR_DNLDRESVOCCMP:  /* Download resident vocab complete */
         bdmask = drpp->dl_bcnt;
         break;
      case VR_UBDPRMCMP:      /* Update board parameter complete */
      case VR_RDBDPRMCMP:     /* Read board parameter complete */
         bdmask = drpp->dl_data >> 8;
         break;
      default:                /* Unrecognized board command */
         bdmask = 0x0F;       /* service it on all daughterboards */
         break;
      }
   }

   /* 
    * If the bit for this board is set in the message, then tell the
    * caller that the message is for this VR160 board.
    */
   return (dxbdp->db_vr160bd & bdmask);

#else

   DLGC_MSG2(CE_WARN, "sr_chk160() not supported on VME\n");

#endif /* VME_SPAN */

}

#ifdef sun
int
sr_schedule_recv_DPC(ldp_fw_event)
GN_LOGDEV *ldp_fw_event;
{
   int rcode = 0; 

   /* Don't schedule another soft interrupt for this ldp if one is
   ** outstanding.
   */
   if (ldp_fw_event->ld_flags & LD_SOFT_INT) {
      STRACE2("sr_schedule_recv_DPC: LD_SOFT_INT ldp=0%x, bdp=0x%x\n", 
         ldp_fw_event, ldp_fw_event->ld_gbp);
      return(0);
   }

   STRACE2("sr_schedule_recv_DPC: ldp=0%x, bdp=0x%x\n", 
      ldp_fw_event, ldp_fw_event->ld_gbp);

   rcode = dlgn_queue_DPC(sramrecvcmd, ldp_fw_event);

   if (rcode == 0)
      ldp_fw_event->ld_flags |= LD_SOFT_INT;

   return(rcode);
}

int
sr_schedule_send_DPC(gbp)
GN_BOARD *gbp;
{
   extern sr_dispatch_pending_cmd();

   return(dlgn_queue_DPC(sr_dispatch_pending_cmd, gbp));
}
#endif /* sun */
