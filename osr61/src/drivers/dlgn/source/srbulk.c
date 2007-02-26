/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srbulk.c
 * Description                  : SRAM protocol - bulk data functions
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
#include "fln.h"
#include "fx12low.h"
#include "srmap.h"

#include "dxdrv.h"

#include "srdefs.h"
#include "srbulk.h"
#include "srcmd.h"
#include "srmisc.h"

#include "dlgcos.h"
#include "srextern.h"
#include "drvdebug.h"

#ifdef LFS
PRIVATE void sr_data2q(register mblk_t *, register GN_LOGDEV *);
#else
PRIVATE void sr_data2q();
#endif
#ifdef _STATS_
extern int StatisticsMonitoring;
extern SRAM_DRV_STATISTICS DriverStats;
#endif

/***********************************************************************
 *        NAME: sr_bulkstart(mp,ldp)
 * DESCRIPTION: this function starts off the send command.
 *      INPUTS: mp - pointer to message containing the command/reply
 *              packet.
 *              ldp - pointer to the associated logical device struct
 *     OUTPUTS: 
 *     RETURNS: none
 *       CALLS: msgdsize() splstr() splx() dlgn_msg() sr_reply()
 *    CAUTIONS: This function is only to be called once to start up the
 *              play.
 **********************************************************************/
void sr_bulkstart(mp, ldp)
mblk_t      *mp;
GN_LOGDEV   *ldp;
{
   DL_DRVCMD   *dcp = (DL_DRVCMD *)ldp->ld_blockmp->b_rptr;
#ifndef LFS
   DL_DRVCMD   *ndcp;
   mblk_t      *nmp;
#endif
   DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   int         oldspl;
   int         rc;
   ULONG       datal;
#ifndef LFS
   ULONG       missing;
#endif


   /* first verify the parameters are ok */
   if (msgdsize(mp) == 0) {
      rc = DLE_BADSIZE;
      goto error;
   }

   /* Indicate we're in SEND mode */
   ldp->ld_blockflags |= LD_BULKSEND;

   /* queue the data blocks onto a local queue */
#ifdef _STATS_
   if (StatisticsMonitoring){
       DriverStats.TotalBytesWrittenCounter += msgdsize(mp->b_cont);
   }
#endif /* _STATS_ */
   sr_data2q(mp->b_cont, ldp);
   mp->b_cont = 0;

   DLGCSPLSTR(oldspl);

   /* Compute variable data length */
   if (dxldp->dl_sndq != NULL) {
      datal = msgdsize((mblk_t *)dxldp->dl_sndq);
   } else {
      datal = 0;
   }

   DLGCSPLX(oldspl);

   /*
    * If play not actually started, set up the initial variable data
    * block and call sr_send2bd(). If this first segment is less that
    * BF_BUFSIZ * 2, fill the remainder with silence so that we can
    * start the play right now.
    */
   if (datal > BF_BUFSIZ * 2) {
      dcp->dc_part2len = BF_BUFSIZ;
   } else {
      if (datal > BF_BUFSIZ) {
         dcp->dc_part2len = datal - BF_BUFSIZ;
      } else {
         dcp->dc_part2len = 0;
      }
   }

   STRACE2("sr_bulkstart: datal=0x%lx, dc_part2len=0x%lx",
	datal, dcp->dc_part2len);

   /* Send cmd to board; return with interrupts disabled if cmd sent. */
   if ((rc = sr_send2bd(ldp, dcp, &oldspl, datal)) != 0) {
      freemsg(dxldp->dl_sndq);

      dxldp->dl_sndq = NULL;
      dxldp->dl_sndcnt = 0;
      rc = DLE_BADCMD;

      /* If command is not sent, interrupt is still enabled. */
      DLGCSPLX(oldspl);
      goto error;
   }

   /* Mark us as started */
   ldp->ld_blockflags |= LD_BULKSTART;

   DLGCSPLX(oldspl);

   return;

error:

   ldp->ld_blockflags &= ~LD_BULKSEND;

   sr_reply(ldp, mp, rc);

}


/***********************************************************************
 *        NAME: sr_bulkdone
 * DESCRIPTION: process the send-done command
 *      INPUTS: mp - command pointer
 *              ldp - logical struct pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void sr_bulkdone(mp, ldp)
register mblk_t      *mp;
register GN_LOGDEV   *ldp;
{
#ifndef LFS
   register    DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
#endif
   DLGCDECLARESPL(oldspl)


   if (msgdsize(mp) > 0) {
      /* queue the data blocks onto a local queue */
#ifdef _STATS_
   if (StatisticsMonitoring){
       DriverStats.TotalBytesWrittenCounter += msgdsize(mp->b_cont);
   }
#endif /* _STATS_ */
      sr_data2q(mp->b_cont, ldp);
      mp->b_cont = 0;
      freemsg(mp);      /* free the control block only */
   } else {
      freemsg(mp);      /* free the entire message */
   }

   DLGCSPLSTR(oldspl);

   ldp->ld_flags &= ~LD_WAITMORE;

   /*
    * Set the LD_BULKDONE flag as a signal
    * to the interrupt handler to play to completion.
    */
   ldp->ld_blockflags |= LD_BULKDONE;

   /* Tell interrupt handler we are up to date */
   ldp->ld_blockflags |= LD_PUTDONE;

   DLGCSPLX(oldspl);

   return;
}


/***********************************************************************
 *        NAME: sr_bulkmore
 * DESCRIPTION: process additional data received for the play
 *      INPUTS: mp
 *              ldp
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS: The message packet sent to this routine must specify
 *              no expected replies since this routine will discard the
 *              message when done without issuing a reply.
 **********************************************************************/
void sr_bulkmore(mp, ldp)
register mblk_t      *mp;
register GN_LOGDEV   *ldp;
{
#ifndef LFS
   register DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
#endif
   DLGCDECLARESPL(oldspl)


   /* Hold interrupts */
   DLGCSPLSTR(oldspl);

   /*
    * If the interrupt handler has already recorded the arrival of a
    * play complete event (per LD_BULKDONE already being set), just
    * ignore the message & discard it without sending a reply.
    * But for now also issue a warning to the console.
    */
   if (ldp->ld_blockflags & LD_BULKDONE) {
      DLGCSPLX(oldspl);
      freemsg(mp);
      DLGC_MSG3(CE_NOTE,"sr_bulkmore: %s: excess data message ignored.",
         ldp->ld_name);
      return;
   }

   /*
    * If no data is in the message then discard it.
    * But for now also issue a warning to the console.
    */
   if (msgdsize(mp) == 0) {
      DLGCSPLX(oldspl);
      freemsg(mp);
      DLGC_MSG3(CE_NOTE,"sr_bulkmore: %s: empty data message ignored.",
         ldp->ld_name);
      return;
   }

   /* Add the data portion of the message to the send queue */
#ifdef _STATS_
   if (StatisticsMonitoring){
       DriverStats.TotalBytesWrittenCounter += msgdsize(mp->b_cont);
   }
#endif /* _STATS_ */
   sr_data2q(mp->b_cont, ldp);
   mp->b_cont = NULL;

   /* reset the BUFMT semaphore */
   ldp->ld_flags &= ~LD_WAITMORE;

   /* Reenable interrupts */
   DLGCSPLX(oldspl);

   /* Free the non-data portion of the message */
   freemsg(mp);

   return;
}


/***********************************************************************
 *        NAME: sr_record(mp,ldp)
 * DESCRIPTION: This function sets up all the necessary parameters
 *              for starting a record and then starts the record.
 *      INPUTS: mp - pointer to message containing the command/reply
 *              packet.
 *              ldp - pointer to the associated logical device structure
 *     OUTPUTS: Command has been validated and the record has been
 *              started.If an error is detected, an appropriate reply is
 *              returned.
 *     RETURNS: none
 *       CALLS: splx() sr_reply() sr_send2bd() 
 *    CAUTIONS: This function is only to be called once to start up the
 *              record.
 **********************************************************************/
void sr_record(mp, ldp)
mblk_t      *mp;
GN_LOGDEV   *ldp;
{
   register DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   register DL_DRVCMD   *dcp = (DL_DRVCMD *)mp->b_rptr;
   int      rc;
   int      oldspl;

   if (msgdsize(mp) != 0) {
      rc = DLE_BADSIZE;
      goto error;
   }

   /* JCT: check for max receive count. Need absolute max size here */
   if (dcp->dx_maxrepsize == 0 ) {
      rc = DLE_BADPARAM;
      goto error;
   }

   /*
    * Send command to board. Remember that we return with interrupts
    * disabled if command was successfully sent.
    */

   if ((rc = sr_send2bd(ldp, dcp, &oldspl, 0)) != 0) {
      goto error;
   }

   /* Mark us as started. (Tell interrupt handler we are up to date) */
   ldp->ld_blockflags |= (LD_BULKRCV | LD_PUTDONE);

   /* save the maximum receive byte count */
   dxldp->dl_maxrcvsz = dcp->dx_maxrepsize;

   /* reset total receive count */
   dxldp->dl_totrcvcnt = 0;

   /* Re-enable interrupts */
   DLGCSPLX(oldspl);

   return;

error:

   /* Send error reply to user now! */
   sr_reply(ldp, mp, rc);

   return;
}


/***********************************************************************
 *        NAME: sr_lastbulk(mp,ldp,0);
 * DESCRIPTION: Duplicates the given mp and converts it to an MT_BULK,
 *              MC_LASTDATA message and sends it up to the SM.
 *      INPUTS: 
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int sr_lastbulk(mp,ldp,dvbp)
mblk_t      *mp;
GN_LOGDEV   *ldp;
DEVBUFF     *dvbp;
{
   register mblk_t      *tmp;
   register GN_CMDMSG   *cmp;
   register DX_LOGDEV   *dxldp = (DX_LOGDEV *) ldp->ld_devdepp;


   /* Start by getting a duplicate copy of the given mp */
   if ((tmp = dlgn_copymp(mp,PM_DRVMSGSIZE)) == NULL) {
      ldp->ld_lostevents++;
      DLGC_MSG4(CE_WARN,"sr_lastbulk: %s: No msg packet for type %d!",
         ldp->ld_name, DL_LASTBUFFULL);
      return (-1);
   }

   /*
    * Tell the BDM that we're done with receive and it should
    * expect no more data from us.
    */
   cmp = (GN_CMDMSG *) tmp->b_rptr;
   tmp->b_datap->db_type = M_PROTO;
   cmp->cm_msg_type      = MT_BULK;
   cmp->fw_msg_ident     = MC_LASTDATA;

   if (dvbp) {
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy( (char *)&(((DL_DRVCMD *)(tmp->b_rptr))->dc_reply),
        (char *)&(dvbp->dv_reply),
         sizeof(DX_REPLY));
#else
      bcopy((char *)&(dvbp->dv_reply),
         (char *)&(((DL_DRVCMD *)(tmp->b_rptr))->dc_reply), 
         sizeof(DX_REPLY));
#endif
   }
      
   if (dxldp->dl_rcvq) {
      linkb(tmp, dxldp->dl_rcvq);
   }

   dxldp->dl_rcvq = NULL;
   dxldp->dl_rcvcnt = 0;

   return (sr_sendreply(ldp, tmp, 0));
}


/***********************************************************************
 *        NAME: sr_data2q(dbp,ldp)
 * DESCRIPTION: 
 *      INPUTS: dbp - pointer to raw voice data to be played
 *              ldp - pointer to the associated logical device structure
 *     OUTPUTS: Raw voice data has been privately queued for subsequent
 *              processing in the interrupt routine.
 *     RETURNS: none
 *       CALLS: splstr() splx() freemsg() linkb()
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void sr_data2q(dbp, ldp)
register mblk_t      *dbp;
register GN_LOGDEV   *ldp;
{
   register DX_LOGDEV   *dxldp = (DX_LOGDEV *)ldp->ld_devdepp;
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   /* put data on the local send queue */
   if (dxldp->dl_sndq == NULL) {
      dxldp->dl_sndq = (mblk_t *)dbp;
   } else {
      linkb(dxldp->dl_sndq, (mblk_t *)dbp);
   }
   dxldp->dl_sndcnt += msgdsize((mblk_t *)dbp);

   DLGCSPLX(oldspl);
}
