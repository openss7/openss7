/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srcmd.c
 * Description                  : SRAM protocol - command functions
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
#include "gnbulk.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndlgn.h"

#include "d40low.h"
#include "d40defs.h"
#include "dxdevdef.h"
#include "dxlibdrv.h"
#include "dxmsg.h"
#include "fln.h"
#include "fx12low.h"
#include "srmap.h"

#include "dxdrv.h"
#include "vrxlow.h"

#include "srdefs.h"
#include "srbulk.h"
#include "srmisc.h"
#include "srcmd.h"

/* Enable Voice View code */
#include "vvfwdefs.h"

/* Enable Phymdimple code */
#include "fxphym.h"

#include "dlgcos.h"
#include "srextern.h"
#include "drvdebug.h"

#ifdef BIG_ENDIAN
#include "endian.h"
#define RWORD(x)	READ_WORD(&x)
#define WWORD(x, d)	WRITE_WORD(&x, d)
#endif /* BIG_ENDIAN */

#ifdef SVR4_MOT
#include <sys/ddi.h>
#endif /* SVR4_MOT */

#ifdef LFS
PRIVATE int sr_ctl_ocnt(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_brst(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_sprm(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_gprm(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_gdvi(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_gtls(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_sstp(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_flsh(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_dtip(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
#ifdef _ISDN
PRIVATE int sr_ctl_setintfcid(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_getintfcid(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
PRIVATE int sr_ctl_setnfasbd(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
#endif /* _ISDN */
#else
PRIVATE int sr_ctl_ocnt();
PRIVATE int sr_ctl_brst();
PRIVATE int sr_ctl_sprm();
PRIVATE int sr_ctl_gprm();
PRIVATE int sr_ctl_gdvi();
PRIVATE int sr_ctl_gtls();
PRIVATE int sr_ctl_sstp();
PRIVATE int sr_ctl_flsh();
PRIVATE int sr_ctl_dtip();
#ifdef _ISDN
PRIVATE int sr_ctl_setintfcid();
PRIVATE int sr_ctl_getintfcid();
PRIVATE int sr_ctl_setnfasbd();
#endif /* _ISDN */
#endif

#ifdef DCB 
#ifdef LFS
PRIVATE int sr_ctl_getatibits(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);
#else
PRIVATE int sr_ctl_getatibits();
#endif
#endif

#ifdef _ISDN
/*
 * External function from DLGN used to retrieve the LDP, given a handle.
 */
#ifdef LFS
extern GN_LOGDEV *dlgn_getldp(register int);
#else
extern GN_LOGDEV *dlgn_getldp();
#endif
#endif /* _ISDN */

#ifdef sun /*  */
extern kmutex_t inthw_lock;
#endif

extern int in_intr_srvc;
extern int ft_pc;

#ifdef _STATS_
extern int StatisticsMonitoring;
extern SRAM_DRV_STATISTICS DriverStats;
#endif

/* following should be in d40low.h */
#ifndef PC_PRRECORD
#define PC_PRRECORD     0x1C
#endif
/*
 * Note sc_msgsz is assumed to be fixed length.  In future if messages come
 * in with variable length data it will be a good idea to add a flag here
 * based on which comparison of sc_msgsz will be made with msgdatalen in
 * sr_control().
 */

typedef struct {
#ifdef LFS
   int (*sc_ctlact)(GN_LOGDEV *, mblk_t *, GN_CMDMSG *);        /* Ptr to control action fn. */
#else
   int (*sc_ctlact)();        /* Ptr to control action fn. */
#endif
   int sc_msgsz;              /* Expected msg. size in bytes */
   unsigned short sc_msgid;   /* Expected fw_msg_ident */
   unsigned short sc_nrep;    /* Number of expected replies */
   unsigned short sc_repid;   /* Reply code */
   unsigned short sc_repsz;   /* Reply size */
} SR_CTLINFO;


SR_CTLINFO Sc_Ctl[] = {                                                       
{ sr_ctl_ocnt, sizeof(ULONG),       SR_OPENCNT,        1, SR_OPENCNTCMPLT,       0 },
{ sr_ctl_brst, 0,                   SR_BDRESET,        1, SR_BDRESETCMPLT,       0 },
{ sr_ctl_sprm, sizeof(GN_BULKPARM), MC_SETPARM,        1, MC_SETPARMCMPLT,       0 },
{ sr_ctl_gprm, sizeof(GN_BULKPARM), MC_GETPARM,        1, MC_CURPARM,            sizeof(GN_BULKPARM) },
{ sr_ctl_gdvi, sizeof(DX_DEVINFO),  SR_GETDEVINFO,     1, SR_CURDEVINFO,         sizeof(DX_DEVINFO) },
{ sr_ctl_gtls, sizeof(DX_REPLY),    SR_GETLSTAT,       1, SR_CURLSTAT,           sizeof(DX_REPLY) },
{ sr_ctl_sstp, sizeof(DX_CMND),     SR_SETSTOP,        1, SR_SETSTOP,            sizeof(DX_CMND) },
{ sr_ctl_flsh, sizeof(GN_FLUSH),    MC_FLUSH,          0, 0,                     0 },
{ sr_ctl_dtip, sizeof(DTI_PARMINFO),SR_DTIPARMINFO,    1, SR_DTIPARMINFOCMPLT,   sizeof(DTI_PARMINFO) },

#ifdef _ISDN
{ sr_ctl_setintfcid, sizeof(USHORT),SR_SETINTFCID,     1, SR_SETINTFCIDCMPLT,    0 },
{ sr_ctl_getintfcid, sizeof(USHORT),SR_GETINTFCID,     1, SR_GETINTFCIDCMPLT,   sizeof(USHORT) },
{ sr_ctl_setnfasbd,  sizeof(ULONG), SR_SETNFASBD,      1, SR_SETNFASBDCMPLT,     0 },
#endif /* _ISDN */

#ifdef DCB
{ sr_ctl_getatibits, sizeof(DSP_INFO), SR_GETATIBITS,   1, SR_GETATIBITSCMPLT,   sizeof(DSP_INFO) },
#endif

};

/***********************************************************************
 *        NAME: sr_send2bd
 * DESCRIPTION: Send a command to the FW via the pre-specified fw area
 *              found in the fwip.
 *      INPUTS: ldp - target logical device pointer
 *              dcp - ptr to DL_DRVCMD structure for this command
 *              oldsplp - previous spl level recorded when disabling
 *                interrupts prior to sending command.
 *     OUTPUTS: Command has been sent to the board.
 *     RETURNS: 0 if success; DLE_BADBOARD if could not send command.
 *       CALLS: 
 *    CAUTIONS: If we sent the command to the board, we exit this 
 *              routine with interrupts disabled. We return the old
 *              spl level to the caller so that it may re-enable
 *              interrupts when done processing. Also, the ldp MUST
 *              have a valid dl_fwip!
 **********************************************************************/
#ifdef LFS
PRIVATE int sr_bdintr(GN_BOARD *, register DX_FWI *);
#endif
int sr_send2bd(ldp, dcp, oldsplp, datal)
GN_LOGDEV   *ldp;
DL_DRVCMD   *dcp;
int         *oldsplp;
int         datal;
{
   register    DX_LOGDEV   *dxldp = ((DX_LOGDEV *)ldp->ld_devdepp);
   DX_FWI      *fwip = dxldp->dl_fwip;
   GN_BOARD    *bdp  = ldp->ld_gbp;
   D4XSRAM     *srp  = (D4XSRAM *) (((DX_BOARD *)(bdp->bd_devdepp))->db_sramaddr);
   int         nticks = 0;
   GN_BOARD    *tbdp = NULL;          /* A temparary pointer. */
   int	       devnum;

#ifdef VME_DEBUG
   int         sr_write_to_cmd = 0;
   unsigned char sr_cmnd_read;
#endif /* VME_DEBUG */

   /* Wait 250ms. for board to become ready */
#ifdef LINUX
   while (readb(&srp->pc_req) != (UCHAR)0) {
#else
   while ((volatile unsigned char)srp->pc_req != (UCHAR)0) {
#endif /* LINUX */
#if 1
#ifdef LINUX
      if (readb(&srp->dl_req) != (UCHAR)0) {
#if 0 /* for Solaris - needs to be tested */
      if (srp->dl_req != (UCHAR)0) {
#endif
         if ( nticks >= 250 ){
              in_intr_srvc = 1;
              sr_bdintr(bdp, fwip);
              devnum = readb(&srp->dl_msgrdy); /* Clear interrupt line */
              srp->dl_reqcopy = 0;
              srp->dl_req = 0;
              devnum = readb(&srp->dl_req);
              in_intr_srvc = 0;
         }
      }
#endif /* LINUX */
#endif
#ifdef sun /*  */
      if (++nticks >= 250) {
#else
      if (++nticks >= 750) {
#endif
#ifdef _STATS_
	if (StatisticsMonitoring){
       	    DriverStats.LostMessagesToFW++;
        }
#endif
        {
         DLGC_MSG3(CE_WARN,"sr_send2bd: %s not ready for command!\n",
             ldp->ld_name);
	 cmn_err(CE_CONT, "SRAM:     dl_req=%#x  pc_req=%#x\n",
			   readb(&srp->dl_req), readb(&srp->pc_req));
	 cmn_err(CE_CONT, " HOST->FW: %x %x %x %x %x %x %x %x\n",
	         (dcp)->dc_cmnd.pc_cmd, (dcp)->dc_cmnd.pc_data&0x00ff,
		 ((dcp)->dc_cmnd.pc_data>>8)&0x00ff, (dcp)->dc_cmnd.pc_data2,
		 (dcp)->dc_cmnd.pc_data3&0x00ff, ((dcp)->dc_cmnd.pc_data3>>8)&0x00ff,
		 (dcp)->dc_cmnd.pc_sub, (dcp)->dc_cmnd.pc_data4);
         return DLE_BADBOARD;
	}
      }
#ifdef _STATS_
        if (StatisticsMonitoring){
            DriverStats.BusyWaitCounter++;
        }
#endif /* _STATS_ */
      dlgn_busywait(1);
   }

#ifdef _STATS_
   if (StatisticsMonitoring){
       DriverStats.FirmwareCommandsCounter++;
       if (nticks > DriverStats.PeakTicks) {
           DriverStats.PeakTicks = nticks;
       }
   }
#endif /* _STATS_ */

   /* Command trace and filter */
   switch (ft_pc) {
      case -1:
         break;
      default:
         if ((dcp)->dc_cmnd.pc_cmd != ft_pc)
	     break;
      case 0:
         cmn_err(CE_CONT, "HOST->FW:      %x %x %x %x %x %x %x %x (%s)\n",
		(dcp)->dc_cmnd.pc_cmd, (dcp)->dc_cmnd.pc_data&0x00ff,
                ((dcp)->dc_cmnd.pc_data>>8)&0x00ff, (dcp)->dc_cmnd.pc_data2,
                (dcp)->dc_cmnd.pc_data3&0x00ff, ((dcp)->dc_cmnd.pc_data3>>8)&0x00ff,
                (dcp)->dc_cmnd.pc_sub, (dcp)->dc_cmnd.pc_data4, ldp->ld_name);
         break;
   }	 

   /* Send command based on FWI type */
   if (fwip->fwi_type == IO_CHNFWI) {

      register CHNBUFF *chbp = (CHNBUFF *)fwip->fwi_addr;

      STRACE3_FWCMD("sr_send2bd: IO_CHNFWI #=%d, gbp=0x%x, pc_cmd=0x%lx",
	(UCHAR)fwip->fwi_num, ldp->ld_gbp, (dcp)->dc_cmnd.pc_cmd);

      /* Copy the DX_CMND command packet to the CHNBUFF area */
#ifdef VME_DEBUG
      sr_write_to_cmd = 1;
#endif /* VME_DEBUG */

/* #ifndef VME_SPAN  */
#ifndef BIG_ENDIAN /*  */
      chbp->bf_cmnd = dcp->dc_cmnd;
#else
      sr_copy((char *)&(dcp->dc_cmnd), (char *)&(chbp->bf_cmnd),
         sizeof(DX_CMND));
#endif /* BIG_ENDIAN */
/* #endif VME_SPAN  */

      /* Copy the variable data (if any) to the CHNBUFF area */
      if ((dxldp->dl_sndq != NULL) && (datal != 0)) {
         register ULONG nbytes1;
         register ULONG nbytes2;

         nbytes2 = dcp->dc_part2len;
         nbytes1 = min((ULONG)(dxldp->dl_sndcnt - nbytes2), (ULONG)BF_BUFSIZ);

#ifndef VME_SPAN
         chbp->bf_cnt[0] = nbytes1;
#else
         WWORD(chbp->bf_cnt[0], nbytes1);
#endif /* VME_SPAN */

         sr_putdata(&dxldp->dl_sndq, chbp->bf_buff[0], nbytes1);

         /* Clear out buffer[1] if this is a PC_DIAL command */
         if (dcp->dc_cmnd.pc_cmd == PC_DIAL) {
#ifndef VME_SPAN
            bzero((char *)chbp->bf_buff[1], BF_BUFSIZ);
#else
            sr_bzero((char *)(chbp->bf_buff[1]), 256); 
#endif /* VME_SPAN */
         }

         /*
          * Save the device handle to be written back if this is a
          * PC_SETTERMS command.  This is needed because there is
          * no reply to this command and in case of error the handle
          * is not available to send the reply back up.
          */
         if (dcp->dc_cmnd.pc_cmd == PC_SETTERMS) {
            dxldp->dl_handle = dcp->gn_drvhdr.cm_address.da_handle;
         }

#ifndef VME_SPAN
         chbp->bf_cnt[1] = nbytes2;
#else
         WWORD(chbp->bf_cnt[1], nbytes2);
#endif /* VME_SPAN */

         sr_putdata(&dxldp->dl_sndq, chbp->bf_buff[1], nbytes2);
         dxldp->dl_sndcnt -= (nbytes1 + nbytes2);

         STRACE3_FWCMD("sr_send2bd: nbytes1=0x%x, nbytes2=0x%x, dl_sndcnt=0x%x\n",
            nbytes1, nbytes2, dxldp->dl_sndcnt);

      }

      /* Tell FW which FWI is being used */
      srp->pc_req = (UCHAR)fwip->fwi_num;

     /* IBI */
      if (dcp->dc_cmnd.pc_cmd == PC_RECORD) {
         chbp->bf_cnt[0] = 0;
         chbp->bf_cnt[1] = 0;
      }
     /* IBI */

     /* FSK */
      if (dcp->dc_cmnd.pc_cmd == PC_PRRECORD) {
         /* save the maximum receive byte count */
         dxldp->dl_maxrcvsz = dcp->dx_maxrepsize;
      }
     /* FSK */

   } else {

      register DEVBUFF *dvbp = (DEVBUFF *)fwip->fwi_addr;

      STRACE3_FWCMD("sr_send2bd: IO_DEVFWI #0xFF, ldp=0x%x, gbp=0x%x, pc_cmd=0x%lx",
	 ldp, ldp->ld_gbp, dcp->dc_cmnd.pc_cmd);

#ifdef BRI_SUPPORT
      if (dcp->dc_cmnd.pc_cmd == ISDN_CMD) {
         DX_REPLY *isdncmdp = (DX_REPLY *)&(dcp->dc_cmnd);
         isdncmdp->dl_lstat |= (((DX_BOARD *)(bdp->bd_devdepp))->db_dslnum << 3) ;      }
#endif /* BRI_SUPPORT */

      /* Copy the DX_CMND command packet to the DEVBUFF area */
#ifdef VME_DEBUG
      sr_write_to_cmd = 1;
#endif /* VME_DEBUG */

/* #ifndef VME_SPAN  */
#ifndef BIG_ENDIAN /*  */
      dvbp->dv_cmnd = dcp->dc_cmnd;
#else
      sr_copy((char *)&(dcp->dc_cmnd), (char *)&(dvbp->dv_cmnd),
         sizeof(DX_CMND));
#endif /* BIG_ENDIAN */
/* #endif VME_SPAN  */

      /* If any extra dev-area data, copy it to the dev area buffer */
      if (dcp->gn_drvhdr.fw_protocol & IO_DEVDATA) {
/* #ifndef VME_SPAN  */
#ifndef BIG_ENDIAN /*  */

#ifdef DLGC_LIS_FLAG
 	 __wrap_memcpy( (char *)dvbp->dv_cmdbf, ((char *)dcp) + dcp->dx_offset, dcp->dx_bufsz);
#else
         bcopy(((char *)dcp) + dcp->dx_offset,(char *)dvbp->dv_cmdbf,dcp->dx_bufsz);
#endif

#else
         sr_copy(((char *)dcp) + dcp->dx_offset,dvbp->dv_cmdbf,dcp->dx_bufsz);
#endif /* BIG_ENDIAN */
/* #endif VME_SPAN  */
      }

      /* Copy the variable data (if any) to the DEVBUFF area */
      if ((dxldp->dl_sndq != NULL) && (datal != 0)) {
         /*
          * FAX or VOICEVIEW:  If this is a transmit data
          * command, the bulk data goes through the channel buffers
          */
         if ( ( ((dcp->dc_cmnd).pc_cmd == FAX_ID) &&
               (((dcp->dc_cmnd).pc_sub == SEND_FAX) ||
                ((dcp->dc_cmnd).pc_sub == PHYM_XMIT) ||
                ((dcp->dc_cmnd).pc_sub == LOAD_FONT)) ) ||
              ( ((dcp->dc_cmnd).pc_cmd == VVIEW_ID) &&
                ((dcp->dc_cmnd).pc_sub == VV_XMITPREP) ) ) {
 
            register ULONG    nbytes1;
            register ULONG    nbytes2;
            register CHNBUFF  *chbp = (CHNBUFF*)
               (((DX_BOARD *)bdp->bd_devdepp)->db_fwi[ldp->ld_devnum].fwi_addr);

            nbytes2 = dcp->dc_part2len;
            nbytes1 = min((ULONG)(dxldp->dl_sndcnt - nbytes2), (ULONG)(BF_BUFSIZ));

#ifndef VME_SPAN
            chbp->bf_cnt[0] = nbytes1;
#else
            WWORD(chbp->bf_cnt[0], nbytes1);
#endif /* VME_SPAN */

            sr_putdata(&dxldp->dl_sndq,chbp->bf_buff[0],nbytes1);

#ifndef VME_SPAN
            chbp->bf_cnt[1] = nbytes2;
#else
            WWORD(chbp->bf_cnt[1], nbytes2);
#endif /* VME_SPAN */

            sr_putdata(&dxldp->dl_sndq,chbp->bf_buff[1],nbytes2);
            dxldp->dl_sndcnt -= (nbytes1 + nbytes2);

#ifdef _ISDN
         /*
          * ISDN data goes to the 272 byte buffer beyond the traditional
          * 48 byte device area buffers.
          */
         } else if ((dcp->dc_cmnd).pc_cmd == ISDN_CMD) {
            sr_putdata(&dxldp->dl_sndq,dvbp->dv_isdncmdbf,dxldp->dl_sndcnt);
            dxldp->dl_sndcnt = 0;
#endif /* _ISDN */
         } else {  

            sr_putdata(&dxldp->dl_sndq,dvbp->dv_cmdbf,dxldp->dl_sndcnt);
            dxldp->dl_sndcnt = 0;
         }
      }

#ifdef DCB
      if ((dxldp->dl_type & DI_DCB) == DI_DCB) { 
         switch(dxldp->dl_type) {
         case DI_DCB320:
            dvbp->dv_cmnd.pc_data2 = BBDSP1; 
            break; 

            case DI_DCB640:
            dvbp->dv_cmnd.pc_data2 = BBDSP2; 
            break; 

            case DI_DCB960:
            dvbp->dv_cmnd.pc_data2 = DBDSP1; 
            break; 

            default:
            dvbp->dv_cmnd.pc_data2 = DCBBRD; 
            break;
         } 
      } 
#endif

#ifdef _ISDN
      /*
       * Fill in pc_data4 if library hasn't yet done so and this is not
       * an ISDN device.  In case of an ISDN device the library must
       * fill-in this field since it contains the Call Id present bits
       * as well.
       */
      if (((dcp->dc_cmnd).pc_cmd != ISDN_CMD) && (dvbp->dv_cmnd.pc_data4 == 0))
#else
      /* Fill in pc_data4 if library hasn't yet done so */
      if (dvbp->dv_cmnd.pc_data4 == 0)
#endif /* _ISDN */
      {
         /* Tell FW which logical device is being used */
         if (ldp->ld_devnum > 0) {
            DX_BOARD *dxbdp = (DX_BOARD *) ldp->ld_gbp->bd_devdepp;

            /*
             * If this a VR160, then pc_data4 must contain the physical
             * VR160 channel number not the logical device number.
             */
            if ((dxbdp->db_logtype == DI_VR160) &&
                (dvbp->dv_cmnd.pc_cmd == VR_DEVNUM)) {
               ULONG chnum = (ULONG) ldp->ld_devnum;
               ULONG bdmsk = (ULONG) dxbdp->db_vr160bd;
               tbdp = ldp->ld_gbp;
               while (bdmsk >>= 1) {
                  tbdp--;
                  chnum += (tbdp->bd_nlogdevs - 1);
               }
               dvbp->dv_cmnd.pc_data4 = (UCHAR)chnum;

            } else {
               dvbp->dv_cmnd.pc_data4 = (UCHAR)ldp->ld_devnum;
            }
         } else {
            dvbp->dv_cmnd.pc_data4 = DEV_BOARD;
            /* Special check for MF40 support */
            if (dvbp->dv_cmnd.pc_cmd == MF40_ID) {
               dvbp->dv_cmnd.pc_data4 = MFBDCH;
            }
         }
	} else {
              if ((dvbp->dv_cmnd.pc_data4 & 0x1f) == 0 )
              {
                 dvbp->dv_cmnd.pc_data4 |= (UCHAR)ldp->ld_devnum;
              }
      }
      /* Tell FW that device area FWI is being used */
      srp->pc_req = (UCHAR)DEVICE;
   }

   /* Disable interrupts & tell FW the message is there */
   DLGCSPLSTR(*oldsplp);

#ifdef VME_DEBUG
   if (sr_write_to_cmd == 0) {
      DLGC_MSG5(CE_PANIC, "sr_send2bd: Wrote to DX_CMND ? srp=0x%x, pc_req=0%x, fwi_type=0x%x\n",
         srp, srp->pc_req, fwip->fwi_type);
   }
   srp->pc_msgrdy = 0xB;
#else
   srp->pc_msgrdy = 0;
#endif /* VME_DEBUG */

   return (0);
}


/***********************************************************************
 *        NAME: sr_control
 * DESCRIPTION: processing function for the MT_CONTROL commands
 *      INPUTS: ldp - ldp pointer
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int sr_control(ldp, mp)
GN_LOGDEV   *ldp;
mblk_t      *mp;
{
   register SR_CTLINFO *scip = Sc_Ctl;
   register SR_CTLINFO *escip = Sc_Ctl + (sizeof(Sc_Ctl)/sizeof(SR_CTLINFO));
   register GN_CMDMSG  *cmdp = &(((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr);
   int rc;

   STRACE2("sr_control: ldp=0x%lx, fw_msg_ident=0x%lx", ldp, cmdp->fw_msg_ident);

   /* Find the matching fw_msg_ident command */
   for (; scip < escip; scip++) {
      if (scip->sc_msgid == cmdp->fw_msg_ident) {
         goto foundit;
      }
   }

   DLGC_MSG4(CE_WARN,"sr_control: unknown message type:%x on %s\n",
      cmdp->fw_msg_ident, ldp->ld_name);
   sr_reply(ldp, mp, DLE_BADCMD);
   return (-1);

foundit:

   if (msgdsize(mp) != scip->sc_msgsz) {
      sr_reply(ldp, mp, DLE_BADSIZE);
      return (-1);
   }

   if (cmdp->cm_nreplies != scip->sc_nrep) {
      sr_reply(ldp, mp, DLE_BADCMD);
      return (-1);
   }

   if (cmdp->cm_nreplies != 0) {
      int idx;
      for (idx = 0; idx < (int)cmdp->cm_nreplies; idx++) {
         if (cmdp->cm_reply[idx].rs_ident == scip->sc_repid) {
            goto reply_ok;
         }
      }
      sr_reply(ldp, mp, DLE_BADCMD);
      return (-1);
   }

reply_ok:

   /* Call the action fn for this command */
   if ((rc = (*(scip->sc_ctlact))(ldp, mp, cmdp)) != 0) {
      sr_reply(ldp, mp, rc);
      return (-1);
   }

   /* Send a reply if required */
   if (scip->sc_nrep != 0) {
      mblk_t *nmp = dlgn_findreply(ldp, scip->sc_repid, scip->sc_repsz);
      if (nmp == NULL) {
         sr_reply(ldp, mp, DLE_NOMEM);
         return (-1);
      }
      ((DL_DRVCMD *)nmp->b_rptr)->gn_drvhdr.cm_msg_type = MT_REPLY;
      sr_reply(ldp, nmp, 0);
   }

   return (0);
}


/***********************************************************************
 *        NAME: sr_device
 * DESCRIPTION: processing function for the MT_DEVICE commands
 *      INPUTS: ldp - ldp pointer
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int sr_device(ldp, mp)
GN_LOGDEV   *ldp;
mblk_t      *mp;
{
   register    DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   register    DL_DRVCMD   *dcp;
   register    int         datal;
   mblk_t      *dbp;
   int         rc;
   int         oldspl = -1;
   ULONG       maxdata;
   ULONG       datal2;
   USHORT      fwitype;
#ifndef LFS
   GN_BOARD    *bdp;
   DX_BOARD    *dxbdp;
   D4XSRAM     *d4xsram;
#endif

   switch (((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr.fw_msg_ident) {
   case DV_SEND:
   case SR_DEVSEND:
      
      /* Compute variable data length & verify that it is in bounds */
      datal = msgdsize(mp);
#ifdef _ISDN
      /* Point to original DL_DRVCMD */
      dcp = (DL_DRVCMD *)mp->b_rptr;

      if ((fwitype = dxldp->dl_fwip->fwi_type) == IO_CHNFWI) {
         maxdata = BF_BUFSIZ * 2;
      } else if (dcp->dc_cmnd.pc_cmd == ISDN_CMD) {
         maxdata = DV_ISDNBUFSIZ;
      } else {
         maxdata = DV_BUFSIZ;
      }

      /* Report error if out of range */
      if ((datal < 0) || (datal > maxdata)) {
         sr_reply(ldp, mp, DLE_BADSIZE);
         return (0);
      }
#else
      if ((fwitype = dxldp->dl_fwip->fwi_type) == IO_CHNFWI) {
         maxdata = BF_BUFSIZ * 2;
      } else {
         maxdata = DV_BUFSIZ;
      }
      
      /* Report error if out of range */
      if ((datal < 0) || (datal > maxdata)) {
         sr_reply(ldp, mp, DLE_BADSIZE);
         return (0);
      }
      
      /* Point to original DL_DRVCMD */
      dcp = (DL_DRVCMD *)mp->b_rptr;
#endif /* _ISDN */
      
      /*
       * If there is variable data present, just unchain the first
       * variable data block from the control (first) block
       * containing the DL_DRVCMD structure. If there is no variable
       * data, ensure that any empty data block(s) is(are) freed, and
       * NULL out the dbp pointer.
       */
      if ((dbp = mp->b_cont) != NULL) {
         if (datal > 0) {
            /* Ensure that part2 data length is valid */
            if (fwitype == IO_CHNFWI) {
               datal2 = dcp->dc_part2len;
               if ((datal2 > min(datal, BF_BUFSIZ)) || 
                   ((datal - datal2) > BF_BUFSIZ)) {
                  sr_reply(ldp, mp, DLE_BADPARAM);
                  return (0);
               }
            } else {
               /* Indicate no part2 data */
               dcp->dc_part2len = 0;
            }
         } else {
            /*
             * If block present but empty, release it & indicate no
             * variable data
             */
            freemsg(dbp);
            dbp = NULL;
         }
         /* Unchain data from control info */
         mp->b_cont = NULL;
      }
      
      if (datal != 0) {

         /*
          * DEEP: Patch to fix memory leak of 4K buffers reported by
          * Motorola Paging.  These buffers should have been freed
          * when the command that held these buffers completed.
          * Instead they were left on the send Q and we should free
          * them now before sndq is overwritten and the data buffers
          * are left hanging.
          */
         if (dxldp->dl_sndq != NULL) {
            freemsg(dxldp->dl_sndq);
            dxldp->dl_sndq   = NULL;
            dxldp->dl_sndcnt = 0;
         }
         /* DEEP: End of Patch */

         dxldp->dl_sndq = dbp;           /* Save data ptr in public place */
         dxldp->dl_sndcnt = datal;       /* Save data length in ldp */
      }
      
      /*
       * Send command to board if requested.
       * Remember that we return with interrupts disabled if cmd sent.
       */
#ifdef VME_SPAN
      oldspl = -1;
#endif /* VME_SPAN */
      if ((rc = sr_send2bd(ldp, dcp, &oldspl, datal)) != 0) {
         /* If error, free any variable data present */
         if ((dxldp->dl_sndq != NULL) && (datal != 0)) {
            freemsg(dxldp->dl_sndq);
            dxldp->dl_sndq = NULL;
            dxldp->dl_sndcnt = 0;
#ifdef VME_SPAN
            /*
            ** 1/6/95: Clean up temporary send buffer.
            */
            if (dxldp->dl_sndq_tmp) {
               freemsg(dxldp->dl_sndq_tmp);
               dxldp->dl_sndq_tmp = NULL;
               if (ldp->ld_blockflags & LD_BULKQUEUED)
                  ldp->ld_blockflags &= ~LD_BULKQUEUED;
            }
#endif /* VME_SPAN */
         }
#ifndef sun
         if (oldspl != -1) {
            DLGCSPLX(oldspl);
         }
#endif /* sun */
         sr_reply(ldp, mp, rc);
         return(0);
      }
      
      /* If no reply is expected, free the message packet */
      if (dcp->gn_drvhdr.cm_nreplies == 0) {
         freemsg(mp);
      }
      
#ifndef sun
      if (oldspl != -1) {
	 DLGCSPLX(oldspl);
      }
#endif /* sun */
      break;

   case SR_STOP:
#ifdef sun
   case SR_STOPSYNC:
#endif /* sun */
      if (sr_putstop(ldp, mp, STOP_SENDREPLY) < 0) {
         DLGC_MSG2(CE_WARN,"sr_device: sr_putstop failed\n" );
      }
      break;

   default:
      DLGC_MSG4(CE_NOTE,"sr_device: %s: unknown message type:%x, ignored.\n",
         ldp->ld_name, ((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr.fw_msg_ident);
      sr_reply(ldp, mp, DLE_BADCMD);
      break;
   }

   return (0);
}


/***********************************************************************
 *        NAME: sr_virt
 * DESCRIPTION: processing function for the virtual events
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS: there are no virtual events in this version
 **********************************************************************/
int sr_virt(ldp, mp)
GN_LOGDEV   *ldp;
mblk_t      *mp;
{
   int rc = 0;
   GN_CMDMSG *cmdp = &((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr;


   switch (cmdp->fw_msg_ident) {
   default:
      sr_reply(ldp, mp, DLE_BADCMD);
      rc = -1;
      break;
   }

   return (rc);
}


/***********************************************************************
 *        NAME: sr_bulk
 * DESCRIPTION: processing function for the MT_BULK commands 
 *      INPUTS: ldp - ptr to the logical struct
 *              mp - command pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int sr_bulk(ldp, mp)
GN_LOGDEV   *ldp;
mblk_t      *mp;
{
   register GN_CMDMSG *cmdp = &(((DL_DRVCMD *)mp->b_rptr)->gn_drvhdr);
   register DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;


   /*
    * We must check here for the resumable send data state where
    * the play has just terminated but the send queue still
    * retains the data to be sent.  In this state, we reject
    * all commands except MC_FLUSH.  In the future, there
    * can be a RESUME command that will restart the sending
    * of the data on the send queue.
    */

   switch (cmdp->fw_msg_ident) {
   case MC_SENDDATA:

      if (!(ldp->ld_blockflags & LD_BULKSEND) && dxldp->dl_sndq != NULL) {
         sr_reply(ldp, mp, DLE_BADSTATE);
         break;
      }
      sr_bulkstart(mp, ldp);
      break;

   case MC_MOREDATA:

      if (!(ldp->ld_blockflags & LD_BULKSEND) && dxldp->dl_sndq != NULL) {
         freemsg(mp);
         break;
      }
      sr_bulkmore(mp, ldp);
      break;

   case MC_LASTDATA:

      if (!(ldp->ld_blockflags & LD_BULKSEND) && dxldp->dl_sndq != NULL) {
         freemsg(mp);
         break;
      }
      sr_bulkdone(mp, ldp);
      break;

   case MC_RECEIVEDATA:

      sr_record(mp, ldp);
      break;

   default:
#ifdef LFS
      DLGC_MSG5(CE_WARN, "sr_bulk: bad message ldp:%p mp:%p type:%x\n",
         ldp, mp, cmdp->fw_msg_ident);
#else
      DLGC_MSG5(CE_WARN, "sr_bulk: bad message ldp:%x mp:%x type:%x\n",
         ldp, mp, cmdp->fw_msg_ident);
#endif
      sr_reply(ldp, mp, DLE_BADCMD);
      break;
   }

   return (0);

}


PRIVATE int sr_ctl_ocnt(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   *((ULONG *)(mp->b_cont->b_rptr)) = ((DX_LOGDEV *)ldp->ld_devdepp)->dl_opencnt;
   return (0);
}


PRIVATE int sr_ctl_brst(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   if (sr_bdreset(ldp->ld_gbp) < 0) {
      return (DLE_BADBOARD);
   }
   return (0);
}


PRIVATE int sr_ctl_sprm(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   register GN_BULKPARM *bpp = (GN_BULKPARM *) mp->b_cont->b_rptr;
   register DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   DLGCDECLARESPL(oldspl)

   /* validate the new parameters */
   if ((short)bpp->bp_lowater < PM_LOMARK || bpp->bp_hiwater > PM_HIMARK) {
      return (DLE_BADPARAM);
   }

   DLGCSPLSTR(oldspl);

   /* Copy the appropriate PM parameters */
   dxldp->dl_lowater = bpp->bp_lowater;
   dxldp->dl_hiwater = bpp->bp_hiwater;

   DLGCSPLX(oldspl);

   /* Discard the data portion of the message */
   if (mp->b_cont) {
      freemsg(mp->b_cont);
      mp->b_cont = NULL;
   }

   return (0);
}


PRIVATE int sr_ctl_gprm(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   register GN_BULKPARM *bpp = (GN_BULKPARM *) mp->b_cont->b_rptr;
   register DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   bpp->bp_lowater = dxldp->dl_lowater;
   bpp->bp_hiwater = dxldp->dl_hiwater;

   DLGCSPLX(oldspl);

   return (0);
}


PRIVATE int sr_ctl_gdvi(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   register  DX_DEVINFO *dvip = (DX_DEVINFO *) mp->b_cont->b_rptr;
   GN_BOARD  *bdp = ldp->ld_gbp;
   GN_LOGDEV *nldp;
   DX_LOGDEV *dxldp = (DX_LOGDEV *) ldp->ld_devdepp;
   DX_BOARD  *dxbdp = (DX_BOARD *) bdp->bd_devdepp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

#ifdef DLGC_LIS_FLAG
   __wrap_memcpy( (char *)dvip->di_name, (char *)ldp->ld_name, LD_NAMELEN);
#else
   bcopy((char *)ldp->ld_name, (char *)dvip->di_name, LD_NAMELEN);
#endif

   dvip->di_type    = dxldp->dl_type;
   dvip->bdi_paddr  = (ULONG)dxbdp->db_physaddr;
   dvip->bdi_vaddr  = (ULONG)dxbdp->db_sramaddr;
   dvip->bdi_nchans = bdp->bd_nlogdevs - 1;
   dvip->bdi_btype  = dxldp->dl_type;
   dvip->bdi_vers   = dxbdp->db_fwver;
   dvip->bdi_irq    = bdp->bd_irq;

   for (nldp = ldp; (nldp != NULL && nldp->ld_devnum < DV_MAXCHANONBD);
                                                         nldp = nldp->ld_nldp) {
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy( dvip->bdi_chnames[nldp->ld_devnum], nldp->ld_name, LD_NAMELEN);
#else
      bcopy((char *)nldp->ld_name, (char *)dvip->bdi_chnames[nldp->ld_devnum], LD_NAMELEN);
#endif
   }

   if ((dxldp->dl_type & DT_CH) || (dxldp->dl_type & DT_TS)) {
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy(dvip->chi_bdname, bdp->bd_name, GB_NAMELEN);
#else
      bcopy((char *)bdp->bd_name, (char *)dvip->chi_bdname, GB_NAMELEN);
#endif
      dvip->chi_chno = ldp->ld_devnum;
   }

#ifdef DLGC_LIS_FLAG
   __wrap_memcpy( ((char *)(&dvip->bdi_d4xsram)) + 0x100, 
      ((char *)(dxbdp->db_sramaddr)) + 0x100, sizeof(D4XSRAM) - 0x100);
#else
   bcopy(((char *)(dxbdp->db_sramaddr)) + 0x100,
      ((char *)(&dvip->bdi_d4xsram)) + 0x100, sizeof(D4XSRAM) - 0x100);
#endif

   DLGCSPLX(oldspl);

   return (0);
}


PRIVATE int sr_ctl_gtls(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   register DX_FWI *fwip = ((DX_LOGDEV *)ldp->ld_devdepp)->dl_fwip;
   DLGCDECLARESPL(oldspl)

   if (fwip == NULL) {
      return (DLE_BADCMD);
   }

   DLGCSPLSTR(oldspl);

   *((DX_REPLY *)mp->b_cont->b_rptr) = ((CHNBUFF *)fwip->fwi_addr)->bf_reply;

   DLGCSPLX(oldspl);

   return (0);
}


PRIVATE int sr_ctl_sstp(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   ((DX_LOGDEV *)ldp->ld_devdepp)->dl_stop = *((DX_DEVSTOP *)mp->b_cont->b_rptr);

   DLGCSPLX(oldspl);

   return (0);
}


PRIVATE int sr_ctl_flsh(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   DLGCDECLARESPL(oldspl)
   register GN_FLUSH  *fptr = (GN_FLUSH *) mp->b_cont->b_rptr;
   register DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;

   /* must be set to flush either or both of the data queues */
   if ((fptr->gf_cmd & (GN_FLUSHW | GN_FLUSHR)) == 0) {
      return (DLE_BADPARAM);
   }

   DLGCSPLSTR(oldspl);

   /* flush the data queue for the specific side */
   if (fptr->gf_cmd & GN_FLUSHW) {
      if (dxldp->dl_sndq) {
         freemsg(dxldp->dl_sndq);
         dxldp->dl_sndq = NULL;
      }
#ifdef VME_SPAN
      /*
      ** 1/6/95: Clean up temporary send buffer.
      */
      if (dxldp->dl_sndq_tmp) {
         freemsg(dxldp->dl_sndq_tmp);
         dxldp->dl_sndq_tmp = NULL;
         if (ldp->ld_blockflags & LD_BULKQUEUED)
            ldp->ld_blockflags &= ~LD_BULKQUEUED;
      }
#endif /* VME_SPAN */
      dxldp->dl_sndcnt = 0;
      ldp->ld_flags &= ~LD_WAITMORE;
      ldp->ld_blockflags &=
         ~(LD_STOP | LD_BULKSEND | LD_BULKSTART | LD_PUTDONE | LD_BULKDONE);
   }

   if (fptr->gf_cmd & GN_FLUSHR) {
      if (dxldp->dl_rcvq) {
         freemsg(dxldp->dl_rcvq);
         dxldp->dl_rcvq = NULL;
      }
      dxldp->dl_rcvcnt = 0;
      ldp->ld_blockflags &=
         ~(LD_STOP | LD_BULKRCV | LD_BULKSTART | LD_PUTDONE | LD_BULKDONE);
   }

   DLGCSPLX(oldspl);

   freemsg(mp);
   return (0);
}


PRIVATE int sr_ctl_dtip(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   register DTI_PARMINFO *dtip = (DTI_PARMINFO *)(mp->b_cont->b_rptr);
   register DX_LOGDEV *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   DLGCDECLARESPL(oldspl)

   if (dtip->dt_parmid >= DTIP_MAXPARM) {
      return (DLE_BADCMD);
   }

   switch(dtip->dt_devtype) {
   case DTI_BDDEV:
      if (ldp->ld_devnum != 0) {
         return (DLE_BADBOARD);
      }
      break;
   case DTI_TSDEV:
      if (ldp->ld_devnum == 0) {
         return (DLE_BADBOARD);
      }
      break;
   default:
      return(DLE_BADCMD);
      break;
   }

   DLGCSPLSTR(oldspl);

   switch(dtip->dt_cmdid) {
   case DP_READ:
      dtip->dt_data = dxldp->dl_dtiparm[dtip->dt_parmid];
      break;
   case DP_WRITE:
      dxldp->dl_dtiparm[dtip->dt_parmid] = dtip->dt_data;
      break;
   case DP_ADDBITS:
      dxldp->dl_dtiparm[dtip->dt_parmid] |= dtip->dt_data;
      break;
   case DP_DELBITS:
      dxldp->dl_dtiparm[dtip->dt_parmid] &= ~(dtip->dt_data);
      break;
   default:
      DLGCSPLX(oldspl);
      return(DLE_BADCMD);
      break;
   }

   DLGCSPLX(oldspl);
   return (0);
}

#ifdef _ISDN

PRIVATE int sr_ctl_setintfcid(ldp, mp, cmdp)
   GN_LOGDEV *ldp;
   mblk_t *mp;
   GN_CMDMSG *cmdp;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
   register USHORT *intfcidp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   if (mp->b_cont == NULL) {
      DLGCSPLX(oldspl);
      return (DLE_BADPARAM);
   }

   intfcidp = (USHORT *)(mp->b_cont->b_rptr);
   dxbdp->db_isdnintfcid = *intfcidp;

   /* Discard the data portion of the message */
   freemsg(mp->b_cont);
   mp->b_cont = NULL;

   DLGCSPLX(oldspl);
   return (0);
}


PRIVATE int sr_ctl_getintfcid(ldp, mp, cmdp)
   GN_LOGDEV *ldp;
   mblk_t *mp;
   GN_CMDMSG *cmdp;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
   register USHORT *intfcidp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   if (mp->b_cont == NULL) {
      DLGCSPLX(oldspl);
      return (DLE_BADPARAM);
   }

   intfcidp = (USHORT *)(mp->b_cont->b_rptr);
   *intfcidp = dxbdp->db_isdnintfcid;

   DLGCSPLX(oldspl);
   return (0);
}


PRIVATE int sr_ctl_setnfasbd(ldp, mp, cmdp)
   GN_LOGDEV *ldp;
   mblk_t *mp;
   GN_CMDMSG *cmdp;
{
   register DX_BOARD *dxbdp = (DX_BOARD *)(ldp->ld_gbp->bd_devdepp);
   register int *handlep;
   register GN_LOGDEV *dchanldp;
   register GN_BOARD *lastbchanbdp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   if (mp->b_cont == NULL) {
      DLGCSPLX(oldspl);
      return (DLE_BADPARAM);
   }

   handlep = (int *)(mp->b_cont->b_rptr);
   if ((dchanldp = dlgn_getldp(*handlep)) == NULL) {
      DLGCSPLX(oldspl);
      return (DLE_BADPARAM);
   }

   /*
    * Save pointer to the D Channel GN_BOARD structure and
    * make the B Channel pointer NULL.
    */
   dxbdp->db_dchanbdp = dchanldp->ld_gbp;
   dxbdp->db_bchanbdp = NULL;

   if (ldp == dchanldp) {   /* D Channel board */
      dxbdp->db_nfasflag = IN_NFAS_D;
   } else {                 /* B Channel board */
      dxbdp->db_nfasflag = IN_NFAS_B;

      /*
       * Starting at the D Channel board, traverse the list of B Channel
       * boards associated with this NFAS group and append this B Channel
       * board in the end.
       */
      lastbchanbdp = dchanldp->ld_gbp;
      while (((DX_BOARD *)lastbchanbdp->bd_devdepp)->db_bchanbdp != NULL) {
         lastbchanbdp = ((DX_BOARD *)lastbchanbdp->bd_devdepp)->db_bchanbdp;
      }
      ((DX_BOARD *)lastbchanbdp->bd_devdepp)->db_bchanbdp = ldp->ld_gbp;
   }

   DLGCSPLX(oldspl);

   /* Discard the data portion of the message */
   if (mp->b_cont) {
      freemsg(mp->b_cont);
      mp->b_cont = NULL;
   }

   return (0);
}

#endif /* _ISDN */

#ifdef LFS
#ifdef PCI_SPAN
extern int sba_mapin( unsigned short NewBoard, unsigned short PciBoardId );
#else
extern unsigned short sba_mapin( unsigned short NewBoard );
#endif /* PCI_SPAN */
#endif

#ifdef DCB
PRIVATE int sr_ctl_getatibits(ldp, mp, cmdp)
GN_LOGDEV *ldp;
mblk_t *mp;
GN_CMDMSG *cmdp;
{
   DEVBUFF1 *dvbp1; 
   register  DSP_INFO *atip = (DSP_INFO *) mp->b_cont->b_rptr;
   register    DX_LOGDEV   *dxldp = ((DX_LOGDEV *)ldp->ld_devdepp);

  DLGCDECLARESPL(oldspl) 

   SR_BDSRAM  *brp = &(((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->db_sr);

   DLGCSPLSTR(oldspl);

   if (brp->br_ioport != DX_NOPORT) {
#ifdef PCI_SPAN
     sba_mapin(brp->br_ioport, brp->br_pcibrdid);
#else
      sba_mapin(brp->br_ioport);
#endif /* PCI_SPAN */
   }
   
   dvbp1 = (DEVBUFF1 *)(brp->br_sramaddr+0x1400);
 
   if (dxldp->dl_type == DI_DCB640) {
#ifdef DLGC_LIS_FLAG
        __wrap_memcpy( ((char *)(&atip->dcb_dsp)) , dvbp1->dsp_info+4, sizeof(DSP_INFO));
#else
   	bcopy(dvbp1->dsp_info+4, ((char *)(&atip->dcb_dsp)) , sizeof(DSP_INFO));
#endif
   } else {
#ifdef DLGC_LIS_FLAG
	__wrap_memcpy( ((char *)(&atip->dcb_dsp)) , dvbp1->dsp_info, sizeof(DSP_INFO));
#else
	bcopy(dvbp1->dsp_info, ((char *)(&atip->dcb_dsp)) , sizeof(DSP_INFO));
#endif
   }
   DLGCSPLX(oldspl);

   return (0);
}
#endif

