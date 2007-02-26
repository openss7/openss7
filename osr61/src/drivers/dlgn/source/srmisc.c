/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srmisc.c 
 * Description                  : SRAM protocol - miscellaneous
 *
 *
 **********************************************************************/

#ifdef LINUX
#include "dlgclinux.h"
#else
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/stream.h>
#include <sys/strlog.h>
#include <sys/stropts.h>
#include <sys/param.h>
#include <sys/cmn_err.h>

#ifdef sun
#include <sys/ddi.h>
#include <sys/sunddi.h>
#endif /* sun */
#endif /* LINUX */

#include "typedefs.h"
#include "gndefs.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndlgn.h"

#include "d40defs.h"
#include "d40low.h"
#include "dxdevdef.h"
#include "dxlibdrv.h"
#include "dxmsg.h"
#include "fln.h"
#include "srmap.h"

#include "dxdrv.h"

#include "srdefs.h"
#include "srmisc.h"

#include "dlgcos.h"
#include "srextern.h"
#include "drvdebug.h"

#ifdef SVR4_MOT
#include <sys/ddi.h>
#endif /* SVR4_MOT */

/* PM's IRQ list */
#ifndef VME_SPAN
SR_IRQLIST *Sr_Irq[GN_MAXIRQ] = { 0 };
#else
#ifdef SVR4_MOT
extern int sr_byte_copy;
#else
int sr_byte_copy = 0;
#endif /* SVR4_MOT */
SR_IRQLIST *Sr_Irq[GN_MAXSLOTS + 1] = { 0 };
#endif /* VME_SPAN */

#ifdef sun
extern int sr_check_spurrious;
extern kmutex_t         inthw_lock;
extern kmutex_t		intdrv_lock;
extern kmutex_t		dlgn_stoplock;
extern kcondvar_t	dlgn_stopcv;
#endif /*sun*/

extern int in_intr_srvc;

/***********************************************************************
 *        NAME: sr_loopstop
 * DESCRIPTION: Build a stop message and loop it back to this PM via
 *              the Sync Module's loopback function.
 *      INPUTS: mp  - pointer to the message packet.
 *              ldp - logical struct pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void sr_loopstop(mp, ldp)
mblk_t      *mp;
GN_LOGDEV   *ldp;
{
   register mblk_t      *tmp;
   register GN_CMDMSG   *cmp;
   register GN_CMDMSG   *omp = &(((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr);


   /* Get a blank free message block */
   if ((tmp = dlgn_copymp(NULL,PM_DRVMSGSIZE)) == NULL) {
      DLGC_MSG3(CE_WARN,"sr_loopstop: %s: No message packet available!",
         ldp->ld_name);
      return;
   }

   tmp->b_datap->db_type = M_PROTO;

   cmp = &(((DL_DRVCMD *)tmp->b_rptr)->gn_drvhdr);
   cmp->cm_msg_type  = MT_DEVICE;
   cmp->fw_msg_ident = SR_STOP;
   cmp->cm_address.da_handle = omp->cm_address.da_handle;

   ((DL_DRVCMD *)tmp->b_rptr)->dc_cmnd =
      ((DX_LOGDEV *)ldp->ld_devdepp)->dl_stopcmd;

   /*
    * Send this message up to the SM where it will be looped back to
    * the PM at some future time.
    */
   dlgn_loopback(ldp, tmp);
}


/***********************************************************************
 *        NAME: sr_putstop
 * DESCRIPTION: This function processes the SR_STOP call sent either by
 *              the user via a putmsg() or by internal loopback.
 *      INPUTS: mp - ptr to message containing the command/reply packet.
 *              ldp - ptr to the associated logical device structure
 *     OUTPUTS: A PC_STOP command has been sent to the firmware if the
 *              current function awaiting reply is enabled for being
 *              stopped as indicated by the LD_STOP flag.
 *     RETURNS: none
 *       CALLS: sr_reply() dlgn_msg() splstr() splx() freemsg() linkb()
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
int sr_bdintr(GN_BOARD *, register DX_FWI *);
#endif
int sr_putstop(ldp, mp, flag)
GN_LOGDEV   *ldp;
mblk_t      *mp;
UCHAR       flag;
{
   register    DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   register    DL_DRVCMD   *dcp;
   register    DX_FWI *fwip;
#if defined(sun) && defined(ISA_SPAN)  || defined (LINUX)
   volatile D4XSRAM     *srp;
#else /* sun && ISA_SPAN */
   D4XSRAM     *srp;
#endif /* sun && ISA_SPAN  */
   int         rc = 0;
   int         nticks = 0;
   int	       devnum;
   int	       fw_check_flag = 0;
   GN_BOARD    *bdp = ldp->ld_gbp;
#ifdef VME_SPAN
   int	       do_syncro;
#endif /* VME_SPAN */

#ifdef VME_DEBUG
   int         sr_write_to_cmd2 = 0;
   unsigned char sr_cmnd_read2 = 0;
#endif /* VME_DEBUG */

   if (mp->b_datap->db_type != M_PROTO) {
      rc = DLE_BADCMD;
      goto error;
   }

   /* Ensure no variable data present */
   if (msgdsize(mp) != 0) {
      rc = DLE_BADSIZE;
      goto error;
   }

   /* Point to the DL_DRVCMD packet */
   dcp = (DL_DRVCMD *)mp->b_rptr;
#ifdef VME_SPAN
   do_syncro = ((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr.fw_msg_ident;
#endif /* VME_SPAN */

   /* Ensure that number of replies is valid */
   if (dcp->gn_drvhdr.cm_nreplies != 0) {
      rc = DLE_BADPARAM;
      goto error;
   }

#ifdef VME_SPAN
   /* Before stopping the channel, need to clean up all queued pending 
   ** commands for the device.
   */
#endif /* VME_SPAN */

   srp  = (D4XSRAM *)(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_sramaddr);

   /* Wait 250ms. for board to become ready */
#ifdef LINUX
   while (readb(&srp->pc_req) != (UCHAR)0) {
#else
   while (srp->pc_req != (UCHAR)0) {
#endif /* LINUX */
#if 1
#ifdef LINUX
      if (readb(&srp->dl_req) != (UCHAR)0) {
#if 0 /* for Solaris - needs to be tested */
      if (srp->dl_req != (UCHAR)0) {
#endif
          if (nticks >= 200) {
	     /* Prevent NULL or bad fwip from entering sr_bdintr */
             switch(dxldp->dl_stopfwi) {
              case IO_CHNFWI:
               fw_check_flag = 1;
               fwip = &((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_fwi[ldp->ld_devnum];
              case IO_DEVFWI:
               fw_check_flag = 1;
               fwip = ((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_fwi;
              default:
               fw_check_flag = 0;
             }
	     if (fw_check_flag) {
                in_intr_srvc = 1;
                sr_bdintr(bdp, fwip);
                devnum = readb(&srp->dl_msgrdy);   /* Clear interrupt line */
                srp->dl_reqcopy = 0;
                srp->dl_req = 0;
                devnum = readb(&srp->dl_req);
	        in_intr_srvc = 0;
	     }
          }
      }
#endif /* LINUX */
#endif
      if (nticks++ >= 250) {
	 /* get here if ISA is sharing interrupt with PCI */ 
         DLGC_MSG3(CE_WARN,"sr_putstop: %s not ready for command!\n",
             ldp->ld_name);
	 cmn_err(CE_CONT, " SRAM:     dl_req=%#x  pc_req=%#x\n", 
		      readb(&srp->dl_req), readb(&srp->pc_req));
	 cmn_err(CE_CONT, " HOST->FW: %x %x %x %x %x %x %x %x\n",
	        (dcp)->dc_cmnd.pc_cmd, (dcp)->dc_cmnd.pc_data&0x00ff,
		((dcp)->dc_cmnd.pc_data>>8)&0x00ff, (dcp)->dc_cmnd.pc_data2,
		(dcp)->dc_cmnd.pc_data3&0x00ff, ((dcp)->dc_cmnd.pc_data3>>8)&0x00ff,
		(dcp)->dc_cmnd.pc_sub, (dcp)->dc_cmnd.pc_data4);
         rc = DLE_BADBOARD;
         goto error;
      }
      dlgn_busywait(1);
   }

   STRACE3_FWCMD("sr_putstop: SR_STOP ldp=0x%x, ldp->ld_gbp=0x%x, devnum=%d",
      ldp, ldp->ld_gbp, ldp->ld_devnum);

   switch(dxldp->dl_stopfwi) {
   case IO_CHNFWI:
      if (ldp->ld_devnum > 4) {
         freemsg(mp);
         return (0);
      }
      fwip = &((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_fwi[ldp->ld_devnum];
      if (fwip->fwi_addr == NULL) {
         freemsg(mp);
         return (0);
      }
#ifdef VME_DEBUG
      sr_write_to_cmd2 = 1;
#endif /* VME_DEBUG */
      ((CHNBUFF *)fwip->fwi_addr)->bf_cmnd = dcp->dc_cmnd;
      srp->pc_req = (UCHAR)fwip->fwi_num;
      break;
   case IO_DEVFWI:
      fwip = ((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_fwi;
      if (fwip->fwi_addr == NULL) {
         freemsg(mp);
         return (0);
      }
#ifdef VME_DEBUG
      sr_write_to_cmd2 = 1;
#endif /* VME_DEBUG */
      ((DEVBUFF *)fwip->fwi_addr)->dv_cmnd = dcp->dc_cmnd;
      srp->pc_req = (UCHAR)DEVICE;
      break;
   default:
      rc = DLE_BADSTATE;
      goto error;
      break;
   }

#ifdef VME_SPAN
   if (do_syncro == SR_STOPSYNC) {
      DLGCMUTEX_ENTER(&dlgn_stoplock);
   }
#endif /* VME_SPAN */

#ifdef VME_DEBUG
   if (sr_write_to_cmd2 == 0) {
      DLGC_MSG5(CE_PANIC, "sr_putstop: Wrote to DX_CMND ? srp=0x%x, pc_req=0x%x, stopfwi=0x%x", srp, srp->pc_req, dxldp->dl_stopfwi);
   }
   srp->pc_msgrdy = 0xC;
#else
   srp->pc_msgrdy = 0;
#endif /* VME_DEBUG */
   freemsg(mp);

#ifdef VME_SPAN
   if (do_syncro == SR_STOPSYNC) {
      ldp->ld_genstate = ldp->ld_idlestate;
      if (ldp->ld_genstate != 0x1) {
         ldp->ld_genstate |= GN_STOPFLAG;
         DLGCMUTEX_EXIT(&intdrv_lock);

         while (ldp->ld_genstate & GN_STOPFLAG) {
            cv_wait(&dlgn_stopcv, &dlgn_stoplock);
         }

         DLGCMUTEX_EXIT(&dlgn_stoplock);
         DLGCMUTEX_ENTER(&intdrv_lock);
      } else {
         DLGCMUTEX_EXIT(&dlgn_stoplock);
      }
   }
#endif /* VME_SPAN */

   return (0);

error:

   if (flag != STOP_NOSENDREPLY) {
      sr_reply(ldp, mp, rc);
   } else {
      freemsg(mp);
   }
   return rc;
}


/***********************************************************************
 *        NAME: sr_sendreply
 * DESCRIPTION: 
 *      INPUTS: ldp - pointer to target logical device (STREAM).
 *              mp - pointer to message being returned to user.
 *              rc - return code to be sent to the user.
 *     OUTPUTS: Message queued for sending upstream if possible.
 *     RETURNS:  0 if msg queued for sending; 1 if queue full;
 *              -1 if logical device not open.
 *       CALLS: freemsg() dlgn_reply()
 *    CAUTIONS: none
 **********************************************************************/
int sr_sendreply(ldp, mp, rc)
GN_LOGDEV   *ldp;
mblk_t      *mp;
int         rc;
{
   /* If not open, discard the message and return an error */
   if (!(ldp->ld_flags & LD_OPEN))  {
      freemsg(mp);
      return (-1);
   }

   ((GN_CMDMSG *)mp->b_rptr)->fw_errcode = rc;
   if (mp->b_datap->db_type == M_DATA) {
      mp->b_datap->db_type = M_PROTO;
   }

   dlgn_reply(ldp, mp);
   return (0);
}


/***********************************************************************
 *        NAME: sr_reply
 * DESCRIPTION: This function returns a reply to the Sync module via
 *              dlgn_reply().
 *      INPUTS: ldp - pointer to the ldp
 *              mp - pointer to the user request message
 *              rc - return code to return to the user
 *     OUTPUTS: reply has been sent to the user
 *     RETURNS: none
 *       CALLS: dlgn_reply()
 *    CAUTIONS: IOCTL message type are not supported.
 **********************************************************************/
void sr_reply(ldp, mp, rc)
GN_LOGDEV   *ldp;
register mblk_t *mp;
int         rc;
{
   register GN_CMDMSG *drvhdrp = (GN_CMDMSG *)mp->b_rptr;

   if (rc) {
      drvhdrp->cm_msg_type = MT_ERRORREPLY;
      mp->b_wptr = mp->b_rptr + sizeof(DL_DRVCMD);
      if (mp->b_cont != NULL) {
         freemsg(mp->b_cont);
         mp->b_cont = NULL;
      }
   }

   drvhdrp->fw_errcode = rc;
   if (mp->b_datap->db_type == M_DATA) {
      mp->b_datap->db_type = M_PROTO;
   }

   dlgn_reply(ldp, mp);
}


/***********************************************************************
 *        NAME: sr_getdata
 * DESCRIPTION: Copy 'datalen' bytes from 'datap' to message blocks (of
 *              'allocsize' in length) headed by 'dmpp'. Message blocks
 *              are allocated by this function as needed.
 *      INPUTS: dmpp - pointer to target message block header
 *              datap - source data pointer
 *              datalen - # of bytes to copy
 *              allocsize - #bytes to alloc when new msg block needed
 *     OUTPUTS: data copied to allocated message block(s).
 *     RETURNS: 0 = success; DLE_NOMEM if can't alloc needed msg block.
 *       CALLS: allocb() dlgn_msg() linkb() bcopy()
 *    CAUTIONS: 
 **********************************************************************/
int sr_getdata(dmpp, datap, datalen, allocsize)
mblk_t   **dmpp;
UCHAR    *datap;
ULONG    datalen;
ULONG    allocsize;
{
   register mblk_t   *dbp = *dmpp;
   register ULONG    ncopy;
   ULONG    lallocsize;
   ULONG    i;
   ULONG    *sztblp;
   static   ULONG sztbl[] = {
#ifndef VME_SPAN
      4096, 2048, 1024, 512, 256, 128, 64, 16, 4
#else
      8192, 4096, 2048, 1024, 512, 256, 128, 64, 16, 4
#endif /* VME_SPAN */
   };



   if (dbp != NULL) {

      for ( ; dbp->b_cont != NULL; dbp = dbp->b_cont)
         ;

      if (dbp->b_wptr >= dbp->b_datap->db_lim) {
         dbp = NULL;
      }
   }

   while (datalen > 0) {

      if (dbp == NULL) {
         /* Allocate a new block to contain the variable data */
         lallocsize = allocsize;
         if (allocsize == sztbl[0]) {
            i = 2;
         } else {
            i = 4;
         }
         sztblp = sztbl;

         while ((dbp = (mblk_t *)allocb(lallocsize, BPRI_HI)) == NULL) {

            do {
               if (*sztblp < lallocsize) {
                  break;
               }
            } while (sztblp++, --i);

            if (allocsize != sztbl[0]) {
#ifdef LFS
               DLGC_MSG3(CE_WARN,"sr_getdata: %lu size allocation failure!",
                                                                 lallocsize);
#else
               DLGC_MSG3(CE_WARN,"sr_getdata: %d size allocation failure!",
                                                                 lallocsize);
#endif
            }
            if (i == 0) {
               return DLE_NOMEM;
            }
            lallocsize = *sztblp;
         }

         if (*dmpp == NULL) {
            *dmpp = dbp;
         } else {
            linkb(*dmpp, dbp);
         }
      }

      lallocsize = dbp->b_datap->db_lim - dbp->b_wptr;
      ncopy = datalen;

      if (ncopy > lallocsize) {
         ncopy = lallocsize;
      }

#ifdef BIG_ENDIAN /*  */
      sr_copy(datap, dbp->b_wptr, ncopy);
#else
      bcopy((char *)datap, (char *)dbp->b_wptr, ncopy);
#endif /* BIG_ENDIAN */

      datap += ncopy;
      dbp->b_wptr += ncopy;
      datalen -= ncopy;

      if (dbp->b_wptr >= dbp->b_datap->db_lim) {
         dbp = NULL;
      }
   }

   return (0);
}


/**********************************************************************
 *        NAME: sr_putdata
 * DESCRIPTION: Copy 'datalen' bytes from message block(s) headed by
 *              'dmpp' to 'datap'.
 *      INPUTS: dmpp - pointer to source message block header
 *              datap - target data pointer
 *              datalen - # of bytes to copy
 *     OUTPUTS: data copied from allocated message block(s).
 *     RETURNS:  0 = success
 *              >0 = # bytes not copied due to insufficient data.
 *       CALLS: bcopy() freeb()
 *    CAUTIONS: 
 **********************************************************************/
int sr_putdata(dmpp, datap, datalen)
mblk_t   **dmpp;
UCHAR    *datap;
ULONG    datalen;
{
   mblk_t   *dbp = *dmpp;
   ULONG     blocksize;
   ULONG     ncopy;

   while (datalen > 0) {

      if (dbp == NULL) {
         return datalen;
      }

      blocksize = dbp->b_wptr - dbp->b_rptr;
      ncopy = datalen;

      if (ncopy > blocksize) {
         ncopy = blocksize;
      }

#ifdef BIG_ENDIAN /*  */
      sr_copy(dbp->b_rptr, datap, ncopy);
#else
      bcopy((char *)dbp->b_rptr, (char *)datap, ncopy);
#endif /* BIG_ENDIAN */

      dbp->b_rptr += ncopy;
      datap += ncopy;
      datalen -= ncopy;

      if (dbp->b_rptr >= dbp->b_wptr) {
         *dmpp = dbp->b_cont;
         freeb(dbp);
         dbp = *dmpp;
      }
   }

   return (0);
}


/***********************************************************************
 *        NAME: sr_cleanup
 * DESCRIPTION: send a STOP command to FW and reset PM internal queues
 *              and structs
 *      INPUTS: ldp - Logical device to clean up.
 *     OUTPUTS: Pending request has been removed if present.
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS: This function is only meant to be called from the close
 *              routine.
 **********************************************************************/
void sr_cleanup(ldp, flags)
GN_LOGDEV *ldp;
ULONG flags;
{
   DLGCDECLARESPL(oldspl)
   mblk_t      *mp;
#ifndef LFS
   DX_BOARD    *dxbdp = (DX_BOARD *)ldp->ld_gbp->bd_devdepp;
#endif
   DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   GN_CMDMSG   *hptr;

   /*
    * Send a STOP command to the firmware if the SR_STOPONLY
    * flag has been set. Exits after the stop is performed.
    */
   if (flags & STOP_STOPONLY) {

      /* Build a STOP command and send it to FW */
      if ((mp = dlgn_copymp(NULL,PM_DRVMSGSIZE)) == NULL) {
         DLGC_MSG3(CE_WARN,"sr_cleanup: %s cannot copy mp!",
            ldp->ld_name);
         return;
      }

      mp->b_datap->db_type = M_PROTO;

      hptr = &(((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr);
      hptr->cm_msg_type = MT_DEVICE;
      hptr->fw_msg_ident = SR_STOP;

      ((DL_DRVCMD *)mp->b_rptr)->dc_cmnd = dxldp->dl_stopcmd;

      if (mp->b_cont) {
         freemsg(mp->b_cont);
         mp->b_cont = 0;
      }

      /* tell sr_putstop not to send any error reply to the stream */
      sr_putstop(ldp, mp, STOP_NOSENDREPLY);

      return;
   }

   /* Clean up send and receive queues  */

   DLGCSPLSTR(oldspl);

   if (dxldp->dl_sndq) {
      freemsg(dxldp->dl_sndq);
   }

#ifdef VME_SPAN
   if (dxldp->dl_sndq_tmp) {
      freemsg(dxldp->dl_sndq_tmp);
      dxldp->dl_sndq_tmp = NULL;
      if (ldp->ld_blockflags & LD_BULKQUEUED)
         ldp->ld_blockflags &= ~LD_BULKQUEUED;
   }
#endif /* VME_SPAN */

   if (dxldp->dl_rcvq) {
      freemsg(dxldp->dl_rcvq);
   }

   dxldp->dl_sndq = 0;
   dxldp->dl_rcvq = 0;
   dxldp->dl_sndcnt = 0;
   dxldp->dl_rcvcnt = 0;

   /* reestablish default values for send and receive queues */
   sr_setqdefaults(dxldp);

   DLGCSPLX(oldspl);
   return;
}


/***********************************************************************
 *        NAME: sr_putsilence
 * DESCRIPTION: Copy 'datalen' bytes of silent voice data to 'datap'.
 *      INPUTS: datap - target data pointer
 *              datalen - # of bytes to copy
 *     OUTPUTS: silence data copied to target data location
 *     RETURNS: none.
 *       CALLS: none.
 *    CAUTIONS: none.
 **********************************************************************/
void sr_putsilence(datap, datalen)
register UCHAR *datap;
register ULONG datalen;
{
   STRACE1("sr_putsilence %d bytes", datalen);

   while (datalen--) {
      *datap++ = ADPCM_SIL;
   }
}


/***********************************************************************
 *        NAME: sr_bdreset
 * DESCRIPTION: Provides an unconditional reset on the given board.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sr_bdreset(bdp)
GN_BOARD *bdp;
{
   SR_BDSRAM   *sbp = &(((DX_BOARD *)(bdp->bd_devdepp))->db_sr);
#if defined(sun) && defined(ISA_SPAN) || defined(LINUX)
   volatile D4XSRAM     *bdaddr = (D4XSRAM *)sbp->br_sramaddr;
#else /* sun && ISA_SPAN  */
   D4XSRAM     *bdaddr = (D4XSRAM *)sbp->br_sramaddr;
#endif /* sun && ISA_SPAN  */
   int         nticks = 0;
#ifdef SVR4_MOT
   extern int  sr_clr_reset_wait;
#else
#ifndef LFS
   int  sr_clr_reset_wait = 1;
#endif
#endif /* SVR4_MOT */


   /* Reset the board if it exists */
   bdaddr->set_reset = 0;

   STRACE3("sr_bdreset: bdaddr=0x%lx, memlen=0x%lx, set_reset=0x%lx",
      bdaddr, sbp->br_memlen, &bdaddr->set_reset);

   /* Clear the board's 1st memory range */
   if (sr_clearbd((UCHAR *)bdaddr, sbp->br_memlen) != 0) {
      DLGC_MSG3(CE_CONT,"%s: Missing, board ignored\n", bdp->bd_name);
      return (-1);
   }

#ifndef VME_SPAN
   /* Clear the board's 2nd memory range if it has one */
   if (sbp->br_sramaddr2) {
      if (sr_clearbd(sbp->br_sramaddr2, sbp->br_memlen2) != 0) {
         DLGC_MSG3(CE_CONT,"%s: Missing, board ignored\n", bdp->bd_name);
         return (-1);
      }
   }
#endif /* VME_SPAN */

#ifdef VME_SPAN
   dlgn_busywait(sr_clr_reset_wait);
#endif /* VME_SPAN */

   /* Start it, and wait for it to acknowledge */
   bdaddr->pc_req = (UCHAR)REQ_RESET;
   bdaddr->clr_reset = 0;

   /* Wait 1.25 secs. for board to acknowledge */
#ifdef LINUX
   /*
    * gcc compiler optimizes out this read because we just wrote
    * to this address.  readb macro takes care of the problem
    */ 
   while (readb(&bdaddr->pc_req) != (UCHAR)0) {
#else
   while (bdaddr->pc_req != (UCHAR)0) {
#endif /* LINUX */
      if (nticks++ >= 3000) { /* rwt changed from 1250 */
         DLGC_MSG3(CE_CONT,"%s: Cannot start, board ignored",
            bdp->bd_name);
         return (-1);
      }
      dlgn_busywait(1);
   }

   return (0);
}


/***********************************************************************
 *        NAME: sr_setqdefaults
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
void sr_setqdefaults(dxldp)
register DX_LOGDEV *dxldp;
{
   /* PM default low and high water marks */
#ifndef VME_SPAN
   static USHORT Lowater_Default = 2048;
   static USHORT Hiwater_Default = 3076;
#else
   static USHORT Lowater_Default = 2 * BF_BUFSIZ;
   static USHORT Hiwater_Default = 4 * BF_BUFSIZ;
#endif /* VME_SPAN */

   dxldp->dl_lowater = Lowater_Default;
   dxldp->dl_hiwater = Hiwater_Default;
}


/***********************************************************************
 *        NAME: sr_addirq
 * DESCRIPTION: In ISA, the function adds a board entry to the list of
 *              board having the same interrupt.  This way, when an
 *              interrupt arrives, boards on the list will be checked
 *              for firmware events.
 *              In VME, interrupting boards are identified by an interrupt
 *              vector or STATUS/ID that comes from the Spancard.  This vector
 *              is different from that of the ISA platform.  Each base-and
 *              daughter Spancard has an unique vector.  When the interrupt
 *              handler is invoked, the kernel passes a controller number
 *              to the handler.  The controller number is the logical number
 *              of the Spancard in the system and is mapped to the vector.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
void sr_addirq(GN_BOARD *bdp)
#else
void sr_addirq(bdp)
GN_BOARD *bdp;
#endif
{
   register SR_IRQLIST **irqpp;
   register SR_IRQLIST *irqp;
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   int vec = bdp->bd_irq;
   DLGCDECLARESPL(oldspl)
#if 0
   cmn_err(CE_WARN, "sr_addirq: vecnum = %d\n", vec);
#endif
   /*
    * If this board is already on the list, or if this board has
    * no IRQ number, then exit without adding it to the list.
    */
   if ((dxbdp->db_irq.si_bdp != NULL) || (vec == 0)) {
      return;
   }

   DLGCSPLSTR(oldspl);

#ifndef VME_SPAN
   /* Find a place to insert the new entry based on the "vec" number */
   for (irqpp = Sr_Irq + vec; *irqpp; irqpp = &((*irqpp)->si_nextp));
#else
   /* In VME, the db_ioport is the logical Spancard number and its range
   ** is from 0 to 9 for a maximum of 10 boards.
   */
   for (irqpp = Sr_Irq + dxbdp->db_ioport; *irqpp; irqpp = &((*irqpp)->si_nextp));
#endif /* VME_SPAN */

   *irqpp = irqp = &dxbdp->db_irq;
   irqp->si_nextp = NULL;
   irqp->si_bdp = bdp;
   irqp->si_dxbdp = dxbdp;
   irqp->si_bdaddr = (D4XSRAM *)dxbdp->db_sramaddr;
   irqp->si_ioport = (int)dxbdp->db_ioport;

#ifdef sun
   sr_check_spurrious = 1;
#endif /* sun */

   DLGCSPLX(oldspl);

   STRACE3("sr_addirq: bdaddr=0x%lx, irqp=0x%lx, ioport=0x%lx", 
	irqp->si_bdaddr, irqp, irqp->si_ioport);

}


/***********************************************************************
 *        NAME: sr_rmvirq
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
void sr_rmvirq(GN_BOARD *bdp)
#else
void sr_rmvirq(bdp)
GN_BOARD *bdp;
#endif
{
   register SR_IRQLIST **irqpp;
   register SR_IRQLIST *irqp;
   register DX_BOARD *dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   int vec = bdp->bd_irq;
   DLGCDECLARESPL(oldspl)


   /*
    * If this board is NOT on the list, or if this board has
    * no IRQ number, then exit without removing it from the list.
    */
   if ((dxbdp->db_irq.si_bdp == NULL) || (vec == 0)) {
      return;
   }

   DLGCSPLSTR(oldspl);

#ifndef VME_SPAN
   for (irqpp = Sr_Irq + vec; *irqpp; irqpp = &((*irqpp)->si_nextp)) {
#else
   for (irqpp = Sr_Irq + dxbdp->db_ioport; *irqpp; irqpp = &((*irqpp)->si_nextp)) {
#endif /* VME_SPAN */
      if ((*irqpp)->si_bdp == bdp) {
         irqp = *irqpp;
         *irqpp = (*irqpp)->si_nextp;
#ifdef SVR4
         bzero((caddr_t)irqp, sizeof(SR_IRQLIST));
#else
         bzero(irqp, sizeof(SR_IRQLIST));
#endif /* SVR4 */
         break;
      }
   }

#ifdef sun
   sr_check_spurrious = 0;
#endif /* sun */

   DLGCSPLX(oldspl);
}

#ifdef sun /*  */
/***********************************************************************
 *        NAME: sr_reset_device
 * DESCRIPTION: The function clears the DL_REQ so that the firmware
 *              can continue sending events if any to the host.
 *      INPUTS: GN_LOGDEV *ldp
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
void
sr_reset_device(GN_LOGDEV *ldp)
{
   D4XSRAM *srp;
   DLGCDECLARESPL(oldspl)

   STRACE2("sr_reset_device: ldp=0x%lx, dev=%d", ldp, ldp->ld_devnum);

   srp = (D4XSRAM *)(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_sramaddr);

   DLGCSPLSTR(oldspl);
   if (srp->dl_req != 0)
      srp->dl_req = 0;	/* Clear the interrupt if one is pending.
                        ** Clearing it unconditionally can mess up
                        ** the FW.
                        */
   DLGCSPLX(oldspl);

   return;
}

/***********************************************************************
 *        NAME: sr_board_busy
 * DESCRIPTION: The function determines whether the board is busy.
 *      INPUTS: GN_LOGDEV *ldp
 *     OUTPUTS: 
 *     RETURNS: 1 if busy, 0 if not busy
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int
sr_boardbusy(GN_LOGDEV *ldp)
{
   D4XSRAM *srp;

   srp = (D4XSRAM *)(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_sramaddr);

   if (srp->pc_req != (UCHAR)0) 
      return 1;

   return 0;
}

#ifdef VME_SPAN /*  */
/***********************************************************************
 *        NAME: sr_set_PC_INTREQ
 * DESCRIPTION: The function writes one byte to the PC_INTREQ location.
 *              If the byte is set 1, the firmware will interrupt the host
 *              when the board is ready for the next host command.  If the
 *              byte is set to 0, the firmware will not interrupt the host
 *              when the board is ready to receive a host command.
 *      INPUTS: GN_BOARD *gbp, UCHAR flag
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
void
sr_set_PC_INTREQ(GN_BOARD *gbp, UCHAR flag)
{
   D4XSRAM *srp;

   STRACE2("sr_set_PC_INTREQ: gbp=0x%lx, flag=0x%lx", gbp, flag);

   srp = (D4XSRAM *)(((DX_BOARD *)(gbp->bd_devdepp))->db_sramaddr);
   srp->pc_intreq = (UCHAR)flag;

   return;
}
#endif /* VME_SPAN */

/***********************************************************************
 *        NAME: sr_put_pending_cmd
 * DESCRIPTION: The function removes a host command entry in a device 
 *              outstanding commands queue and queues the entry in the board 
 *              pending commands queue.
 *      INPUTS: GN_LOGDEV *ldp, mblk_t *mp
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: MUST BE CALLED WITH INTERRUPT DISABLED.
 **********************************************************************/
void
sr_put_pending_cmd(GN_LOGDEV *ldp, mblk_t *mp)
{
   GN_BOARD *gbp;
   LD_CMD *ld_cmdp, *tmp;

   /* If there are entries in the outstanding commands queue, find the one
   ** that has the command message, mp, and remove the it from the queue.
   */
   if (ld_cmdp = ldp->ld_cmds) {
      do {
         if (ld_cmdp->lc_mp == mp)
            break;
      } while (ld_cmdp = ld_cmdp->next);
	   
      if (ld_cmdp) {

         /* Remove the LD_CMD entry from the device outstanding cmd queue.
         */
         dlgn_dequeue_hostcmd(&ldp->ld_cmds, ld_cmdp);
         ldp->ld_cmdcnt--;

         /* Re-check the timeout queue for this device.  If there is no 
         ** outstanding commands in the queue and if the device was queued
         ** for timeout check earlier, dequeue the device from the 
         ** timeout queue.
         */
         if (ldp->ld_flags & LD_TIMEOUT && ldp->ld_cmds == NULL)
            dlgn_dequeue_timeout(ldp);
      }
   }

   /* Put the LD_CMD entry that was just removed from the device outstanding 
   ** commands queue into the board pending commands queue.  If there was no 
   ** entry removed from the outstanding commands queue, it means that:
   **   1) the command message does not expect any firmware response, and
   **   2) the board becomes busy when the command message is about
   **      to be forwarded to the firmware.
   */
   if (!ld_cmdp) {
      if ((ld_cmdp = (LD_CMD *)dlgn_allocate_pending_cmd(ldp, mp)) == NULL)
         return;
   }

   /*
   ** If the board is DTI and uses the same shared RAM address as another
   ** voice virtual board, queue the pending command in the latter.
   ** If it is a voice board, queue in its own board pending commands queue.
   ** This is simply done by referencing gbp_same_fwi_addr.
   */
   gbp = (GN_BOARD *)(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->gbp_same_fwi_addr);
   dlgn_queue_hostcmd(&gbp->bd_cmds, ld_cmdp);
   gbp->bd_cmdcnt++;

   STRACE3_FWCMD("sr_put_pending_cmd: ldp=0x%lx, gbp=0x%lx, pc_cmd=0x%x",
	ldp, gbp, (((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd) );
   STRACE3_FWCMD("sr_put_pending_cmd: mp=0x%lx, cmdcnt=0x%lx - %d",
      mp, gbp->bd_cmdcnt, 0);
}
#endif /* sun */

/***********************************************************************
 *        NAME: sr_copy
 * DESCRIPTION: Sr_copy supports OS and CPU independent byte copy as
 *              well as alignment guarantee on word copy
 *      INPUTS: s - source data pointer
 *              d - destination data pointer
 *              size - size of the data to copy
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
void
#endif
sr_copy(char *s, char *d, int size)
{
   int	i, offset;
   unsigned char b;
   unsigned short *ss, *sd;

#ifdef BYTE_COPY_ALL_THE_WAY
   while (size--)
      *d++ = *s++;

   b = *--d;	/* read back to flush the processor buffer */

   return;
#endif /* BYTE_COPY_ALL_THE_WAY */

   /*
    * This routine ensures that byte copy is actually carried out.
    * Depending on kernel's bcopy is risky because some of them
    * optimize the move by doing word or long copy when applicable.
    */

   /* Optimize move specifically for DX_CMND and DX_REPLY
    */
   if (size == sizeof(DX_CMND)) {	/* Employ the Duff move method */
         *d++ = *s++;
         *d++ = *s++;
         *d++ = *s++;
         *d++ = *s++;
         *d++ = *s++;
         *d++ = *s++;
         *d++ = *s++;
         *d = *s;

         b = *d;	/* read back to flush the processor buffer */
         return;
   }

   /* Check for alignment */
   /*
    * Use bcopy if told to do so or if the starting address
    * of either the source or destination is odd.  Copy a word
    * in odd address boundary will cause a bus error and/or
    * therefore panic the kernel.
    *
   if (sr_byte_copy || (((int)s & 0x1) || ((int)d & 0x1))) {
    *
    */
#ifdef LFS
   if (((int)(long)s & 0x1) || ((int)(long)d & 0x1)) {
#else
   if (((int)s & 0x1) || ((int)d & 0x1)) {
#endif

      while (size--)
         *d++ = *s++;

      b = *--d;	/* read back to flush the processor buffer */
      return;
   }

   /* Optimize Duff move using 8 shorts at a time
   */
   i = size / 2;
   ss = (unsigned short *)s;
   sd = (unsigned short *)d;

   if (i >= 8) {
      while (1) {
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         i -= 8;
         if (i < 8)
      	   break;
      }
   }

   /* Check for any left over of shorts.  Must be less than 8
   */
   switch(i) {
      case 0:
         break;
      case 1:
         *sd = *ss;
         break;
      case 2:
         *sd++ = *ss++;
         *sd = *ss;
         break;
      case 3:
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd = *ss;
         break;
      case 4:
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd = *ss;
         break;
      case 5:
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd = *ss;
         break;
      case 6:
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd = *ss;
         break;
      case 7:
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd++ = *ss++;
         *sd = *ss;
         break;
   }

   offset = size - 1;

   /* Check for left over single byte when the size is odd */
   if (size % 2) {
      *(d + offset) = *(s + offset);
   }

   b = *(d + offset);/* read back to flush the processor buffer */

   return;

}

#ifdef BIG_ENDIAN /*  */
/***********************************************************************
 *        NAME: sr_bzero
 * DESCRIPTION: Sr_bzero supports OS and CPU independent byte zeroing
 *      INPUTS: s - source data pointer
 *              size - size of the data to copy
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int
sr_bzero(char *s, int size)
{
   unsigned char b;

   while (size--)
      *s++ = 0;

   b = *--s; 	/* read back to flush to processor buffer */
}
#endif /* BIG_ENDIAN */

#ifdef VME_SPAN /*  */
/***********************************************************************
 *        NAME: sr_queue_cmd
 * DESCRIPTION: sr_queue_cmd determines whether a FW command should be
 *              queued or dispatched to the FW depending on the whether
 *              PC_REQ is cleared and whether there are already pending
 *              commands to be forwared first.  If the PC_REQ is cleared
 *              and there is no pending commands for the virtual board
 *              of this logical device, send the FW command at once.  However
 *              if there are pending commands in the queue, queue the
 *              current command.  And if the PC_REQ is cleared, dispatch
 *              the first FW command in the pending queue to the device
 *              of the board.
 *      INPUTS: ldp - logical device pointer
 *              mp - the message that contains the FW command
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int
sr_queue_cmd(ldp, mp)
GN_LOGDEV *ldp;
mblk_t  *mp;
{
   DLGCDECLARESPL(oldspl)
   int cnt;
   GN_BOARD *bdp;
   DX_BOARD *dxbdp;
   D4XSRAM *d4xsram;

   /*
   ** When it comes to checking board pending commands, one should not
   ** use the board pointer as referenced by ldp->ld_gbp.  One must use
   ** the gbp_same_fwi_addr pointer because it points to the board pending
   ** command queue.  As in the case of DTI and the 1st voice board, the
   ** gbp_same_fwi_addr of both boards points to the same shared RAM area
   ** and therefore the same queue.
   */

   bdp = (GN_BOARD *)(((DX_BOARD *)
            (ldp->ld_gbp->bd_devdepp))->gbp_same_fwi_addr);
   dxbdp = (DX_BOARD *)bdp->bd_devdepp;
   d4xsram = (D4XSRAM *)(dxbdp->db_sramaddr);

   /*
   ** Unconditionally set to request PC_REQZERO interrupt to close a
   ** race condition.  The worst case is that we get a spurrious interrupt.
   */
   d4xsram->pc_intreq = (UCHAR) 1;

   if (d4xsram->pc_req == (UCHAR) 0) {

      if (bdp->bd_cmds == NULL) {
         /*
         ** If there are no pending commands, turn off PC_REQZERO interrupt
         ** request.  Tell the calling function that the command can be sent.
         */
         d4xsram->pc_intreq = (UCHAR) 0;
         return 0;
      }
      else {
         /*
         ** Leave the PC_REQZERO interrupt request on since we have pending 
         ** commands.  Queue the current outgoing command and dispatch
         ** the first one on the board pending command queue.  Tell the
         ** calling function that the command was queued.
         */
         sr_put_pending_cmd(ldp, mp);

#ifdef VME_DEBUG
         STRACE2("sr_queue_cmd: bdp=0x%x, cnt=%d", bdp, bdp->bd_cmdcnt);
#endif /* VME_DEBUG */

         sr_dispatch_pending_cmd(bdp);
         return 1;
      }
   }

   sr_put_pending_cmd(ldp, mp);
   return 1;
}
#endif /* VME_SPAN */

#ifdef sun
/***********************************************************************
 *        NAME: sr_dispatch_pending_cmd
 * DESCRIPTION: dispatch_pending_cmd dequeues a pending command and
 *              send it to the device of the board.  The device is extracted
 *              from the FW command message.
 *      INPUTS: gbp - Board device pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int
sr_dispatch_pending_cmd(GN_BOARD *gbp)
{
   register GN_LOGDEV      *ldp;
   register GN_CMDMSG      *drvhdr;
   GN_CMDMSG               *ptr;
   DX_BOARD                *dxbdp;
   DX_LOGDEV               *dxldp;
   LD_CMD                  *ld_cmdp;
   mblk_t                  *mp;

   STRACE2("sr_dispatch_pending_cmd: gbp=0x%x, cmd cnt=0x%x\n",
      gbp, gbp->bd_cmdcnt);

   if (gbp->bd_cmds == NULL || gbp->bd_cmdcnt < 1) {
#ifdef VME_DEBUG
/*
      DLGC_MSG3(CE_NOTE,
         "sr_dispatch_pending_cmd: gbp=0x%x no pending cmds to dispatch", gbp);
*/
#endif /* VME_DEBUG */
      return(0);
   }

   /* Remove the first entry from the board pending commands queue.
   */
   ld_cmdp = gbp->bd_cmds;
   dlgn_dequeue_hostcmd(&gbp->bd_cmds, ld_cmdp);
   gbp->bd_cmdcnt--;

   mp = ld_cmdp->lc_mp;
   drvhdr = (GN_CMDMSG *)mp->b_rptr;

   /* Now, find the device to which the command message is to be sent.
   */
   if ((ldp = (GN_LOGDEV *)dlgn_get_ldp(mp)) == NULL) {
      _dlgn_mfree((char *)ld_cmdp);
      return (gbp->bd_cmdcnt);
   }

   STRACE3_FWCMD("sr_dispatch_pending_cmd: ldp=0x%x, gbp=0x%x, pc_cmd=0x%x",
      ldp, gbp, (((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd));
   STRACE3_FWCMD("sr_dispatch_pending_cmd: mp=0x%lx, cmdcnt=0x%lx left - %d",
      mp, gbp->bd_cmdcnt, 0);

   /* Put the command in the outstanding commands queue only if 
   ** the command expects replies from the FW.  Don't forget to queue 
   ** the device in the timeout queue if the device has not been done so.
   */
   if (drvhdr->cm_nreplies > 0) {

      dlgn_queue_hostcmd(&ldp->ld_cmds, ld_cmdp);
      ldp->ld_cmdcnt++;

      if (!(ldp->ld_flags & LD_TIMEOUT) && ldp->ld_cmds)
         dlgn_queue_timeout(ldp);
   }
   else {
      /* Since no response is expected, should free the command
      ** structure.
      */
      _dlgn_mfree((char *)ld_cmdp);
   }

   /*
   ** Set up structure fields as demanded by the D4X protocol.
   ** The following piece of code (til sr_device()) is from sramsendcmd().
   */

   ptr = &((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr;
   dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
   dxldp = (DX_LOGDEV *)ldp->ld_devdepp;

   /*
    * We must set up dl_fwip on this ldp to point to the correct
    * DX_FWI struct to use at interrupt time to get the reply to
    * this command. The dl_fwip->fwi_ldp pointer is used as a
    * "semaphore": it is normally NULL but it may be set to a valid
    * ldp by the following code so that the interrupt routine can be
    * made aware of exceptions to the normal host-board protocol.
    */

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
	    DLGC_VMEDEBUG_MSG3(CE_NOTE, "sr_dispatch_pending_cmd: MF40, fwi_ldp=0x%x", ldp);
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
	   DLGC_VMEDEBUG_MSG5(CE_NOTE, "sr_dispatch_pending_cmd: msgtype=0x%x, fwi_ldp=0x%x, pc_cmd=0x%x",
			      ptr->cm_msg_type, ldp, ((DL_DRVCMD *)mp->b_rptr)->dc_cmnd.pc_cmd);
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

   /* Send the command to the device
   */
   if (ptr->cm_msg_type == MT_DEVICE)
      sr_device(ldp, mp);
   else if (ptr->cm_msg_type == MT_BULK) {
#ifdef VME_SPAN
      /*
      ** 12/11/96: The following logic was taken from sr_bulkstart().
      ** 6/1/95: If there is a temporary send buffer, link it to the main
      ** send queue.  We shouldn't put the temporary buffer into the main
      ** one until the latter is processed as was originally intended, i.e.
      ** size.
      */
      if (ldp->ld_blockflags & LD_BULKQUEUED) {
   
         ldp->ld_blockflags &= ~LD_BULKQUEUED;
   
         if (dxldp->dl_sndq_tmp) {
   
            STRACE3_PLAY("sr_bulkstart: ldp=0x%x move bytes=0x%x in dl_sndq_tmp to dl_sndq - 0x%x",
               ldp, msgdsize(dxldp->dl_sndq_tmp), ldp->ld_blockflags);
   
            sr_data2q(dxldp->dl_sndq_tmp, ldp);
            dxldp->dl_sndq_tmp = NULL;
   
            /* Set state just like it's done in sr_bulkdone().
            */
            ldp->ld_blockflags |= LD_PUTDONE;
            ldp->ld_blockflags |= LD_BULKDONE;
         }
      }
#endif
      sr_bulk(ldp, mp);
   } else {
      DLGC_MSG4(CE_NOTE, "sr_dispatch_pending_cmd: %s: unknown message type:%x, ignored.", ldp->ld_name, ptr->cm_msg_type);
   }

#ifdef sun
#ifdef VME_SPAN /*  */
   /* Set the PC_REQZERO interrupt request bit if there are still pending
   ** commands.
   */
   if (gbp->bd_cmdcnt) {
      sr_set_PC_INTREQ(gbp, 1);
   }
   /*
   **
   else {
      sr_set_PC_INTREQ(gbp, 0);
   }
   **
   */
#endif /* VME_SPAN */
#endif /* sun */

   return(gbp->bd_cmdcnt);
}

/***********************************************************************
 *        NAME: sr_bd_hdreset
 * DESCRIPTION: sr_bd_hdreset resets the board and freezes the board
 *              CPU so that no more interrupt activities can be 
 *              generated to the host.  This is to prevent situations
 *              in which the host driver is dynamically unloaded and
 *              no host interrupt handler can be dispatched to service
 *              the interrupt.  The consequence of not servicing interrupts
 *              is that the host can be hung as a result of interrupts
 *              pounding the host processor.
 *      INPUTS: bdp - Board device pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
sr_bd_hdreset(GN_BOARD *bdp)
{
   SR_BDSRAM   *sbp = &(((DX_BOARD *)(bdp->bd_devdepp))->db_sr);
   DX_BOARD    *dxbdp = ((DX_BOARD *) bdp->bd_devdepp);
   D4XSRAM     *bdaddr = (D4XSRAM *)sbp->br_sramaddr;
   char *p;

#ifdef VME_SPAN /*  */
   if (sram_present(dxbdp->db_physaddr, dxbdp->db_totmem) == 0)
      return;
   
   p = (char *)bdaddr;

   *(p + 0xf8) = 0;
   *(p + 0xf4) = 0;
   *(p + 0xff) = 0;
#endif /* VME_SPAN */

#ifdef ISA_SPAN /*  */
   bdaddr->set_reset = 0;
#endif /* ISA_SPAN */

}

#ifdef VME_SPAN /*  */
/* At most 8 virtual boards in a SPAN
*/
#define MAXSIZE	(SH_RAM_SIZE * 8)
int
sr_make_sram_multiple(size)
int size;
{
   int remainder;

   if (size <= MAXSIZE)
      return MAXSIZE;

   if ((remainder = (size % MAXSIZE)) == 0)
      return size;

   return(size - remainder + MAXSIZE);
}
#endif /* VME_SPAN */


#ifdef VME_SPAN
#ifdef DONGLE_SECURITY

#define	DONGLE_CODE_LENGTH	8
#define DONGLE_CODE_OFFSET	0x7EE6

#define DONGLE_STATUS_UNREAD	0
#define DONGLE_STATUS_READ	1
#define DONGLE_STATUS_CLONED	2

/***********************************************************************
 *        NAME: sr_dongle
 * DESCRIPTION: The sr_dongle function is called when a virtual board
 *              is started.  The dongle code resides only in the base
 *              board; however, the FW will generate another unique
 *              "pseudo" dongle code for the daughter board.  FW basically
 *              guarantees that all dongle codes are unique.
 *
 *              This function will fill the dongle code array for each
 *              physical board in the slot.  Using the slot number and
 *              the offset into the status area, the dongle code can be
 *              read and saved in a static array.  The dongle code for
 *              each physical board will be read once.  When the number
 *              of dongle codes is at least two, comparison will be
 *              carried out.  If uniqueness is not found among the dongle
 *              code, the virtual board reset will fail.
 *      INPUTS: GN_BOARD *gbp
 *     OUTPUTS: 
 *     RETURNS: 0 if no conflict or not enough dongle entries to do the
 *              comparison.
 *              1 if there is at least one conflict in the dongle code
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int
sr_dongle(GN_BOARD *gbp)
{
   int          i, j, slot;
   DX_BOARD     *dxbdp = ((DX_BOARD *) gbp->bd_devdepp);
   char         *srcp, *kva, *p;
   int          base_sram;

   int          reference_dongle_code_index;
   int          reference_dongle_code[2];/* 8 bytes for dongle code */
   static int   number_of_dongle_code;
   static int   sr_dongle_code[GN_MAXSLOTS + 1][2];
   static int   sr_dongle_code_status[GN_MAXSLOTS + 1]; /* 1 = read, 0 = not */

   extern int   dlgn_maddr[];
   extern int   dlgn_ilev[];
   extern int   sr_dongle_init;

   /* Initialize the dongle code array if it has not been done so.
   */
   if (sr_dongle_init == 0) {

      sr_dongle_init = 1;

      /* sr_dongle_init is set to 0 when the this PM is started or stopped.
      */
      number_of_dongle_code = 0;

      for (i = 0; i < GN_MAXSLOTS + 1; i++) {
         sr_dongle_code_status[i] = DONGLE_STATUS_UNREAD;
         sr_dongle_code[i][0] = -1;
         sr_dongle_code[i][1] = -1;
      }
   }

   /* Now, read the dongle code for the board in slot if it hasn't been done.
   ** If all the bytes are 0, the code for the board is not read.  The dongle
   ** code can come from any of the virtual boards in the base or daughter
   ** board.
   */
   slot = dxbdp->db_ioport;

#ifdef SVR4_MOT
   /* Need to subtract 1 from the slot number because the id field
   ** in the /etc/edt_data file starts at 1.  The id starts at 1
   ** because at one time the program parses the edt_data file can
   ** only process 128 entries.
   */ 
   base_sram = dlgn_maddr[slot - 1];
   if (dlgn_ilev[slot - 1] == -1)
      return (0);

#else
#ifdef sun
   base_sram = dlgn_maddr[slot];
   if (dlgn_ilev[slot] == -1)
      return (0);

#endif /* sun */
#endif /* SVR4_MOT */

   /* Check if dongle code is read.  If it is, return.
   */
   switch (sr_dongle_code_status[slot]) {

   case DONGLE_STATUS_UNREAD:
       break;

   case DONGLE_STATUS_CLONED:	/* Cloned board */
       return(1);

   case DONGLE_STATUS_READ:
      return (0);
   } 

   /* Pursue to read the dongle code for the board in the slot.
   */

#ifdef SVR4_MOT
   /* Map the VMEbus address.
   */
   srcp = niomapin((addr_t)base_sram + DONGLE_CODE_OFFSET, DONGLE_CODE_LENGTH);
   if (srcp == NULL)
      return (0);
#else
#ifdef sun

   if ((kva = (char *)sr_map_regs(((int)base_sram & 0xffffff) + DONGLE_CODE_OFFSET, 
         4096)) == NULL) {
      return (0);
   }

   srcp = kva;

#endif /* sun */
#endif /* SVR4_MOT */

   /* Read the dongle code
   */
   p = (char *)&sr_dongle_code[slot][0];
   for (i = 0; i < DONGLE_CODE_LENGTH; i++)
      *p++ = *srcp++;

   number_of_dongle_code++;
   sr_dongle_code_status[slot] = DONGLE_STATUS_READ;

#ifdef VME_DEBUG_DONGLE
   cmn_err(CE_CONT, "sr_dongle: slot=%d, dongle at 0x%x=0x%x,0x%x\n",
      slot, kva, sr_dongle_code[slot][0], sr_dongle_code[slot][1]);
#endif /* VME_DEBUG_DONGLE */

#ifdef sun
   /* Unmap the VMEbus address.  Don't use the virtual address to the VMEbus
   ** after the addresses are unmapped; otherwise, the kernel could panic.
   */
   sr_unmap_regs(((int)base_sram & 0xffffff) + DONGLE_CODE_OFFSET, 4096); 
#endif /* sun */

   /* Since there are not enough dongle codes to do the comparison,
   ** return OK.
   */
   if (number_of_dongle_code < 2)
      return (0);

#ifdef VME_DEBUG_DONGLE
   cmn_err(CE_CONT, "sr_dongle: # of dongle=%d\n", number_of_dongle_code);
#endif /* VME_DEBUG_DONGLE */

   /* Identify possible cloned board by comparing the reference dongle
   ** against the rest.  The comparison is done like this.  Board 1
   ** is compared against the rest, i.e. board 2 to board n;
   ** board 2 is compared against rest, i.e. board 1, board 3 to board n;
   ** and so on.
   */
   for (i = 0; i < GN_MAXSLOTS + 1; i++) {

      if (sr_dongle_code_status[i] == DONGLE_STATUS_UNREAD)
         continue;

      reference_dongle_code[0] = sr_dongle_code[i][0];
      reference_dongle_code[1] = sr_dongle_code[i][1];

      for (j = 0; j < GN_MAXSLOTS + 1; j++) {

         if (sr_dongle_code_status[j] == DONGLE_STATUS_UNREAD)
            continue;

         if (i == j)	/* skip the reference dongle code or itself */
            continue;
         
         if (reference_dongle_code[0] == sr_dongle_code[j][0] && 
            reference_dongle_code[1] == sr_dongle_code[j][1]) {

            sr_dongle_code_status[j] = DONGLE_STATUS_CLONED;

            return(1);
         }
      }
   }

   return (0);
}
#endif /* DONGLE_SECURITY */
#endif /* VME_SPAN */

#ifdef VME_LIVE_INSERTION_B

#define DLGN_LDP(slot)	Sr_Irq[slot]->si_bdp->bd_ldp

/***********************************************************************
 *        NAME: sr_do_comtst_if_started
 * DESCRIPTION: Perform the online-diag comtst for a started board.
 *		If this board is not started, just return
 *		Check the comtst flag.
 *		If it's clear, set the flag, and send msg down to fw.
 *		If it's still set, the ISR didn't rcv an ack, 
 *		i.e. the fw is dead! Send msg to daemon, and event up.
 *      INPUTS: slot id ( == vec_num)
 *     OUTPUTS: 
 *     RETURNS: -1 if fw is detected to be dead, else 0
 *       CALLS: sr_send2bd DLGN_SETERR_RETURN dlgn_send_event
 *    CAUTIONS: allow enough time for fw to ack, and our dpc to process
 *		it in worst case scenario (20 boards doint data i/o).
 *		For now, run this loop every 5 seconds.
 **********************************************************************/
int
sr_do_comtst_if_started(int slot_id)
{
	GN_LOGDEV   *ldp;
	static int sec = 0;

	/* brd must be started */
	if (Sr_Irq[slot_id] == NULL) {
		return(0);
	}

	ldp = DLGN_LDP(slot_id);
	if (ldp->ld_flags & LD_START) { /* brd is started */
		/* Check the comtst flag.
		 * If it's clear, set the flag, and send msg down to fw.
		 * If it's still set, the ISR didn't rcv an ack, 
		 * i.e. the fw is dead! Send msg to daemon, and event up.
		 */
		if ((ldp->ld_flags & LD_COMTST_SENT) == 0) {
			DX_LOGDEV   *dxldp = ((DX_LOGDEV *)ldp->ld_devdepp);
			DX_BOARD    *dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
			DX_FWI      *fwip;
			DL_DRVCMD   drvcmd;

			/* set the flag prior to sending msg - window */
			ldp->ld_flags |= LD_COMTST_SENT;

			/* Now, format the msg for fw */
			/* dxlibdrv.h, d40defs.h, dtixxx.h */

			bzero((char *)&drvcmd, sizeof(drvcmd));
			drvcmd.dc_cmnd.pc_cmd = NTH_DEVNUM;
			drvcmd.dc_cmnd.pc_sub = NTH_COMTST;
			drvcmd.dc_cmnd.pc_data4 = NTH_TERM;

			dxldp->dl_fwip = &dxbdp->db_fwi[0];
			fwip = dxldp->dl_fwip;
			fwip->fwi_type = IO_DEVFWI;

			/* Now, send the msg to fw - srampm/srcmd.c */
			sr_send2bd(ldp, &drvcmd, NULL, 0);
		}
		/* allow 5 sec for fw to respond, and our isr to get to it */
		else if (sec++ > 5) {
			sec = 0;
			cmn_err(CE_CONT, "sram: fw (%d) is dead!\n", 
				slot_id);

			/* avoid this check again */
			ldp->ld_flags &= ~LD_COMTST_SENT;

			/* avoid comtst for this board again */
			/* also make sure new apps fail for this brd */
			ldp->ld_flags &= ~LD_START;

			/* make sure new apps fail with proper err */
			ldp->ld_flags |= LD_OOSVC;

			/* send MT_EVENT to stream head/srl for open apps */
			/* lib/dxxlib/dx_event.c */
			dlgn_send_event(Sr_Irq[slot_id]->si_bdp, MC_BOARDBAD);

			/* caller will send msg to dlgmond daemon */
			return(-1);
		}
	}
	return(0);
}
#endif /* VME_LIVE_INSERTION */
#endif /* sun */
