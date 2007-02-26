/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dvbm.c
 * Description                  : Bulk Data Module
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
#include <linux/module.h>
#include <linux/version.h>
#ifndef LFS
#include <sys/LiS/config.h>
#endif

#include "dlgclinux.h"
#include "gndefs.h"
#include "gnmsg.h"
#include "gndrv.h"
#include "gndlgn.h"
#include "gnbulk.h"
#include "gnlibdrv.h"
#include "typedefs.h"
#include "dvbm.h"
#include "dvbmextern.h"
#include "drvdebug.h"
#include "dlgcos.h"

int dvbmdevflag = 0;

/* DEFINES */
#define DVBM_ID_                  299    /*  Driver ID */
#define DVBM_CNT                 1024    /* Increased to 1000 on sparc systems */
#if LINUX_VERSION_CODE > 0x020600
#define LINUX26
#endif
/* Module Global Variables */
int         dvbm_cnt = GN_MAXHANDLES;
BDM_INFO    bdm_info[GN_MAXHANDLES] = { {0} };

#ifndef LFS
extern lis_spin_lock_t intmod_lock;
#else
extern spinlock_t intmod_lock;
#endif
MODULE_AUTHOR("Intel");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dialogic Bulk Data Module");

#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("dvbm");
MODULE_ALIAS("streams-dvbm");
MODULE_ALIAS("streams-dvbmDriver");
#endif
#endif

/* PRIVATE FUNCTION DECLARATIONS */
int dvbmopen(queue_t *, dev_t *, int, int, cred_t *);
int dvbmclose(queue_t*, int, cred_t *);
int dvbmrput(queue_t *, mblk_t *);       /* Read Side Put routine. */
int dvbmwput(queue_t *, mblk_t *);       /* Write Side Put routine */
int dvbmrsrv(queue_t *);                 /* Read Side Service routine. */
int dvbmwsrv(queue_t *);                 /* Read Side Service routine. */

/*
 * Set the low/high water marks to allow only one message to be queued
 * for the write-side service routine at a time.
 */
#define LO_WATER        1
#define HI_WATER        64 * 1024
#define MIN_PSZ         0
#define MAX_PSZ         64 * 1024

/* Read Side Module Information */
struct module_info dvrminfo = {
   DVBM_ID_,      /* Module ID. Used for logging */
   "dvbm",        /* Module name. Same as that used in mdevice */
   0,             /* Minimum packet size. */
   0,             /* Maximum packet size. */
   0,             /* High water mark. Used in flow control. */
   0              /* Low water mark. Ditto. */
};

/* Write Side Module Information */
struct module_info dvwminfo = {
   DVBM_ID_,      /* Module ID. Used for logging */
   "dvbm",        /* Module name. Same as that used in mdevice */
   MIN_PSZ,       /* Minimum packet size. */
   MAX_PSZ,       /* Maximum packet size. */
   HI_WATER,      /* High water mark. Used in flow control. */
   LO_WATER       /* Low water mark. Ditto. */
};

/* Read Side Queue Information */
struct qinit dvrinit = {
   dvbmrput,      /* Pointer to Put routine. */
   dvbmrsrv,      /* Pointer to Service routine. */
   dvbmopen,      /* Pointer to Open routine. */
   dvbmclose,     /* Pointer to Close routine. */
   NULL,          /* Reserved by STREAMS for future use. */
   &dvrminfo,     /* Pointer to module_info structure. */
   NULL           /* Pointer to optional statistics structure. */
};

/* Write Side Queue Information */
struct qinit dvwinit = {
   dvbmwput,      /* Pointer to Put routine. */
   dvbmwsrv,      /* Pointer to Service routine. */
   NULL,          /* Pointer to Open routine. */
   NULL,          /* Pointer to Close routine. */
   NULL,          /* Reserved by STREAMS for future use. */
   &dvwminfo,     /* Pointer to module_info structure. */
   NULL           /* Pointer to optional statistics structure. */
};

/*
 * Streamtab entry - Used in cdevsw and fmodsw to point to the driver or
 * module. This is typically the only public structure in a STREAMS
 * driver.
 */
static struct streamtab dvbminfo = {
   &dvrinit,      /* Pointer to Read Queue Init. Structure. */
   &dvwinit,      /* Pointer to Write Queue Init. Structure.*/
   NULL,          /* Multiplexer Read Queue Init. Structure. */
   NULL           /* Multiplexer Write Queue Init. Structure. */
};

void dvbminit(void)
{
   static char   Cf_Signon[] =
      "Dialogic Bulk Data Module \n%s\n%s";
   static char   Cf_Rights[] =
      "%s\nALL RIGHTS RESERVED\n\n";

   DLGC_MSG4(CE_CONT, Cf_Signon, PKGVER, OSTYPEVER );

   if (LINUX_VERSION_CODE > 0x020600)
       printk(" - Kernel 2.6.x\n");
   else if (LINUX_VERSION_CODE > 0x020400)
       printk(" - Kernel 2.4.x\n");
   else
       printk(" - Kernel 2.2.x\n");

   DLGC_MSG3(CE_CONT, Cf_Rights, COPYRIGHT);

   return;
}

int init_module(void)
{
   int ret;

   ret = lis_register_strmod(&dvbminfo, LIS_OBJNAME_STR);
   if (ret < 0) {
       printk("%s: Unable to register module, error %d.\n",
                                LIS_OBJNAME_STR, -ret);
       return ret;
   }   

   dvbminit();
       
   return 0;
}

void cleanup_module(void)
{
   int err;

   err = lis_unregister_strmod(&dvbminfo);
   if (err < 0)
       printk("%s: Unable to unregister module, error %d.\n",
                        LIS_OBJNAME_STR, -err);
   else
       printk("%s: Unregistered, ready to be unloaded.\n",
                        LIS_OBJNAME_STR);
   return;
}       /* end of cleanup_module() */
 

/***********************************************************************
 *        NAME: int dvbmopen(q, dev, flag, sflag)
 * DESCRIPTION: Module open routine.
 *              If first call, zero out data structures and set default
 *              bdm parameters.
 *      INPUTS: q, dev, flag, sflag - generic STREAMS open args
 *     OUTPUTS:
 *     RETURNS: minor number if successful
 *              OPENFAIL if dev out of range
 *       CALLS: DLGC_MSG() minor()
 *    CAUTIONS:
 **********************************************************************/
int init_bulkparam(GN_BULKPARM *);
int dvbmopen(q, dev, flag, sflag, crp)
queue_t  *q;
dev_t *dev;
int      flag;
int      sflag;
cred_t	*crp;
{
   BDM_INFO    *bdmsp;
   BDM_INFO    *ebdmsp;
#ifndef LFS
   GN_BULKPARM *bmp;
   int         i;
#endif
   int          minordevice;
   static unsigned long once = 0;

   DLGC_DEBUG_MSG4(CE_DEBUG, "dvbmopen() q:%x dev:%x\n", q, dev);
   minordevice = MINOR(*dev);
   if ((minordevice < 0) || (minordevice >= dvbm_cnt)) {
        return(ENXIO);
        }

   if (once == 0) {
      bdmsp = bdm_info;
      ebdmsp = &bdm_info[GN_MAXHANDLES];
      for (; bdmsp <  ebdmsp; bdmsp++) {
         bzero((char *)bdmsp, sizeof(BDM_INFO));
         init_bulkparam(&bdmsp->bi_bp);
      }
      once++;
   }

#ifndef LINUX26
  MOD_INC_USE_COUNT;
#endif
  return(0);
}


/***********************************************************************
 *        NAME: int dvbmclose(q)
 * DESCRIPTION: Module close routine.  Clean up data structures
 *              associated with handles bound through this STREAMS
 *              instance.
 *      INPUTS: q - write queue pointer
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: bzero() freemsg() DLGC_MSG() init_bulkparam()
 *    CAUTIONS:
 **********************************************************************/
int
dvbmclose(queue_t *q, int dummy, cred_t *crp)
{
#ifndef LFS
   int         i;
#endif
   BDM_INFO    *bdmsp;
   BDM_INFO    *ebdmsp;
#ifndef LFS
   GN_BULKPARM *bmp;
#endif
#ifdef LFS
   unsigned long z_flag;
#else
   int		z_flag;
#endif
   DLGCDECLARESPL(oldspl)

   DLGC_DEBUG_MSG3(CE_DEBUG, "dvbmclose() q:%x\n", q);

   DLGCQPROCSOFF(q);
   DLGCMUTEX_ENTER(&intmod_lock);
   DLGCSPINLOCK(intmod_lock, z_flag);

   /* clean up only the structs corresponding to this close */
   bdmsp = bdm_info;
   ebdmsp = &bdm_info[dlgn_getpeak()];

   for (; bdmsp <= ebdmsp; bdmsp++) {

      if (bdmsp->bi_qptr == q) {

         DLGC_DEBUG_MSG3(CE_DEBUG, "dvbmclose() cleanup bdmsp:%x\n", bdmsp);

         DLGCSPLSTR(oldspl);

         /* if read queue or write queue allocated, deallocate */
         if (bdmsp->bi_wq) {
            freemsg(bdmsp->bi_wq);
         }

         if (bdmsp->bi_rq) {
            freemsg(bdmsp->bi_rq);
         }

	 /* clearing is causing problem due to user configurable parms */
#if 0
         bzero((char *)bdmsp, sizeof(BDM_INFO));

         /* reset transfer parameters */
         init_bulkparam(&bdmsp->bi_bp);
#endif
         DLGCSPLX(oldspl);
      }
   }

   DLGCMUTEX_EXIT(&intmod_lock);
   DLGCSPINUNLOCK(intmod_lock, z_flag);

#ifndef LINUX26
   MOD_DEC_USE_COUNT;
#endif
   return(0);
}


/***********************************************************************
 *        NAME: int dvbmwput(q, mp)
 * DESCRIPTION: Write side put procedure.
 *      INPUTS: q - write queue
 *              mp - STREAMS message pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: dlgn_wqreply()
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int gn_splitmsg(mblk_t **, mblk_t **, unsigned int);
int send_bmt2usr(queue_t *, mblk_t *, int, BDM_INFO *);
int queue_data(mblk_t **, mblk_t *, int *);
int send_more2pm(queue_t *, BDM_INFO *, mblk_t *, int);
int send_last2pm(queue_t *, BDM_INFO *, mblk_t *, int, int);
#endif
int dvbmwput(q, mp)
queue_t  *q;
mblk_t   *mp;
{
   GN_FLUSH       *gfp;
#ifndef LFS
   struct iocblk  *iocp;
#endif
   unsigned char  *startp;
#ifdef LFS
   BDM_INFO       *bdmsp = NULL;
#else
   BDM_INFO       *bdmsp;
#endif
   GN_BULKPARM    *ep;
   GN_BULKPARM    *bmp;
#ifdef LFS
   GN_CMDMSG      *drvhdr = NULL;
#else
   GN_CMDMSG      *drvhdr;
#endif
#ifndef LFS
   GN_LOGDEV      *ldp;
   mblk_t         *nmp;
#endif
#ifdef LFS
   int            handle = 0;
#else
   int            handle;
#endif
   int            count;
   unsigned int   flusharg;
   unsigned int   sendbytes;
#ifdef LFS
   unsigned long z_flag;
#else
   int		  z_flag;
#endif
   DLGCDECLARESPL(oldspl)

   DLGC_DEBUG_MSG4(CE_DEBUG, "dvbmwput() q:%x mp:%x\n", q, mp);

   DLGCMUTEX_ENTER(&intmod_lock);
   DLGCSPINLOCK(intmod_lock, z_flag);

   /* do sanity checking for all commands; only BIND's are in M_IOCTL */
   if (mp->b_datap->db_type != M_IOCTL) {

      drvhdr = (GN_CMDMSG *)mp->b_rptr;
      handle = drvhdr->cm_address.da_handle;

      /* For M_FLUSH the handle is not 0 - so let it through */
      if (mp->b_datap->db_type != M_FLUSH) {
          /* handle not in range, error */
          if ((handle > dlgn_getpeak()) || (handle < 0)) {
	      DLGCMUTEX_EXIT(&intmod_lock);
              DLGCSPINUNLOCK(intmod_lock, z_flag);
              DLGN_WQREPLY(q, mp, GNE_BADHANDLE);
              return(0);
          }

          bdmsp = &bdm_info[handle];

          /* handle not in INITED state, error */
          if (!(bdmsp->bi_state & BDM_INITED)) {
	      DLGCMUTEX_EXIT(&intmod_lock);
              DLGCSPINUNLOCK(intmod_lock, z_flag);
              DLGN_WQREPLY(q, mp, GNE_BADHANDLE);
              return(0);
          }
      }
   }

   switch (mp->b_datap->db_type) {
   default:       /* pass on anything else to SM */
      DLGCSPINUNLOCK(intmod_lock, z_flag);
      putnext(q, mp);
      break;

   case M_FLUSH:
      /* Check to see if Write queue should be flushed */
      if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
         DLGCSPLSTR(oldspl);
         flushq(q, FLUSHALL);
         DLGCSPLX(oldspl);
      }

      /* Check to see if Read queue should be flushed */
      if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {

         DLGCSPLSTR(oldspl);

         flushq(RD(q), FLUSHALL); /* Flush this modules read queue */

         /*
          * We must insure that this message is sent to all modules on
          * the stream. Because we are a driver we need only worry 
          * about the upstream bound (Read queue) messages.
          * First we clear the write bit (So it isn't sent back
          * downstream by stream head) then send it upstream with
          * qreply()
          */
         *mp->b_rptr |= FLUSHR;     /* Set read queue flush */
         *mp->b_rptr &= ~FLUSHW;    /* Clear write queue flush */

         qreply(q, mp);

         DLGCSPLX(oldspl);
      }
      DLGCSPINUNLOCK(intmod_lock, z_flag);
      break;

   case M_PROTO:
      switch (drvhdr->cm_msg_type) {
      default:
         /* pass thru everything we don't understand */
         DLGCSPINUNLOCK(intmod_lock, z_flag);
         putnext(q, mp);
         break;

      case MT_CONTROL:
         switch (drvhdr->fw_msg_ident) {
         default:
	    /* SMP check for corrupt b_cont */
	    if (drvhdr->fw_msg_ident == MC_SETIDLESTATE) {
                if (mp->b_cont == NULL) {
	 	    dlgn_msg(CE_CONT, "dvbm: MC_SETIDLESTATE NULL b_cont\n");
   		    DLGCSPINUNLOCK(intmod_lock, z_flag);
		    DLGN_WQREPLY(q, mp, GNE_BADPARM);
   		    return (0);
                }
            }

            /* pass thru everything we don't understand */
            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);
            break;

         case MC_SETPARM:
	    DLGCSPLSTR(oldspl);

            /* check for previous BIND */
            if (!(bdmsp->bi_state & BDM_INITED)) {
	       DLGCSPLX(oldspl);
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADSTATE);
               break;
            }

            /* copy in bi_bp structure */
            startp = mp->b_cont->b_rptr;

#ifdef DLGC_LIS_FLAG
	    __wrap_memcpy( &bdmsp->bi_bp, startp, sizeof(GN_BULKPARM));
#else
            bcopy(startp, &bdmsp->bi_bp, sizeof(GN_BULKPARM));
#endif

	    DLGCSPLX(oldspl);
            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);   /* let PM read it too */
            break;

         case MC_GETPARM:
            /* check for previous BIND */
            if (!(bdmsp->bi_state & BDM_INITED)) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADSTATE);
               break;
            }

            ep = (GN_BULKPARM *)mp->b_cont->b_rptr;
            bmp = &bdmsp->bi_bp;
            count = sizeof (GN_BULKPARM);

            /* copy out values we are responsible for */
            ep->bp_sndlo = bmp->bp_sndlo;
            ep->bp_sndhi = bmp->bp_sndhi;
            ep->bp_rcvlo = bmp->bp_rcvlo;
            ep->bp_rcvhi = bmp->bp_rcvhi;
            ep->bp_sndmax = bmp->bp_sndmax;
            ep->bp_rcvmax = bmp->bp_rcvmax;
            ep->bp_2pmsize = bmp->bp_2pmsize;

            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);   /* let PM fill in the rest */
            break;

         case MC_FLUSH:
   	    if (mp->b_cont == NULL) {
#ifdef LFS
                dlgn_msg(CE_CONT, "dvbmwput() MC_FLUSH Null messagep 0x%p\n", mp);
#else
                dlgn_msg(CE_CONT, "dvbmwput() MC_FLUSH Null messagep 0x%x\n", mp);
#endif
            } else {

            /* MC_FLUSH is used to flush internal read/write queues */
            gfp = (GN_FLUSH *)mp->b_cont->b_rptr;
            flusharg = gfp->gf_cmd;

            /* flush read queue */
            if (flusharg & GN_FLUSHR) {

               if (bdmsp->bi_rq) {
                  freemsg(bdmsp->bi_rq);
                  bdmsp->bi_rq = 0;
               }

               bdmsp->bi_rqcnt = 0;

	       /* for prompted record set to receive */
               if (bdmsp->bi_state & BDM_PRSTATUS) {
                   bdmsp->bi_state = RECEIVING | BDM_INITED | SEND_IDLE;
               } else {
                   bdmsp->bi_state &= ~(RECEIVING | RECEIVE_LAST);
                   bdmsp->bi_state |= RECEIVE_IDLE;
               }
            }

            /* flush write queue */
            if (flusharg & GN_FLUSHW) {

               if (bdmsp->bi_wq) {
                  freemsg(bdmsp->bi_wq);
                  bdmsp->bi_wq = 0;
               }

               bdmsp->bi_wqcnt = 0;
               bdmsp->bi_state &=
                 ~(SENDING | SEND_LAST | BDM_BUFMT | BDM_WAIT_MOREDATA);
               bdmsp->bi_state |= SEND_IDLE;

            }
   }

            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);   /* let PM read it too */
            break;
         }

         break;

      case MT_BULK:
         switch (drvhdr->fw_msg_ident) {
         default:
            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);
            break;

         case MC_SENDDATA:
            /* must be in SEND_IDLE state */
            if ((bdmsp->bi_state & (SEND_IDLE | SENDING | SEND_LAST |
               BDM_BUFMT | BDM_WAIT_MOREDATA)) != SEND_IDLE) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADSTATE);
               break;
            }

            /* Must have some data attached */
            if (msgdsize(mp) == 0 || mp->b_cont == 0) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADPARM);
               break;
            }

            /* there should not be data on write queue */
            if (bdmsp->bi_wq) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADSTATE);
               break;
            }

            /*
             * Send bdm2pm_size bytes of data to PM. If we have less
             * than bdm2pm_size bytes, send all we have.
             */
            sendbytes = bdmsp->bi_bp.bp_2pmsize;

            DLGC_DEBUG_MSG3(CE_DEBUG, "dvbmwput() MC_SENDDATA sendbytes %d\n", sendbytes); 
            gn_splitmsg(&mp->b_cont, &bdmsp->bi_wq, sendbytes);

            if (bdmsp->bi_wq) {
               bdmsp->bi_wqcnt = msgdsize(bdmsp->bi_wq);
            }

            DLGC_DEBUG_MSG3(CE_DEBUG, "dvbmwput() MC_SENDDATA data message %d bytes\n", msgdsize(mp));

#ifdef ISA_SPAN
            DLGCSPINUNLOCK(intmod_lock, z_flag);
            putnext(q, mp);
            DLGCSPINLOCK(intmod_lock, z_flag);
#endif
            bdmsp->bi_state &= ~SEND_IDLE;
            bdmsp->bi_state |= SENDING;

            /* CSP fix for less than 1.5K transfer buffers */
            if ( (bdmsp->bi_bp.bp_2pmsize <= 1536)
               && !(bdmsp->bi_state & SEND_LAST) ) {
               bdmsp->bi_state |= BDM_BUFMT;
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               send_bmt2usr(RD(q), NULL, handle, bdmsp);
               DLGCSPINLOCK(intmod_lock, z_flag);
            }

            /* If below low water mark, send BUFFMT message to user */
	    DLGCSPLSTR(oldspl);
            if ( (bdmsp->bi_wqcnt != 0) && 
                 ((unsigned long)bdmsp->bi_wqcnt <= 
                  (unsigned long)bdmsp->bi_bp.bp_sndlo) && 
                  !(bdmsp->bi_state & BDM_WAIT_MOREDATA) ) {
               DLGC_DEBUG_MSG2(CE_DEBUG,
                  "dvbmwput() MC_SENDDATA below low water mark\n");

               DLGCSPINUNLOCK(intmod_lock, z_flag);
               send_bmt2usr(RD(q), NULL, handle, bdmsp);
            } else {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
  	    }

	    DLGCSPLX(oldspl);
            break;

         case MC_MOREDATA:
            DLGC_DEBUG_MSG2(CE_DEBUG, "dvbmwput() MC_MOREDATA\n");

            /* must be in SENDING state */
            if (!(bdmsp->bi_state & SENDING)) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADSTATE);
               break;
            }

            /* we must turn off the BDM_WAIT_MOREDATA flag here
               so BUFMT messages can go up */
            bdmsp->bi_state &= ~BDM_WAIT_MOREDATA;

            if (msgdsize(mp) == 0 || mp->b_cont == 0) {
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               DLGN_WQREPLY(q, mp, GNE_BADPARM);
               break;
            }

            /* If we've reached threshold, throw away data */
            if ((unsigned long)bdmsp->bi_wqcnt > (unsigned long)bdmsp->bi_bp.bp_sndmax) {
               DLGC_MSG2(CE_CONT,"dvbmwput(): MC_MOREDATA above threshold. ");
               DLGC_MSG3(CE_CONT,"%lu bytes discarded\n", (ulong)msgdsize(mp));
               DLGCSPINUNLOCK(intmod_lock, z_flag);
               freemsg(mp);
               break;
            }

            /* put data onto internal queue */
            queue_data(&bdmsp->bi_wq, mp->b_cont, &bdmsp->bi_wqcnt);

            mp->b_cont = 0;

            /* send more data if we ran out before */
	    DLGCSPLSTR(oldspl);

	    if (bdmsp->bi_state & BDM_BUFMT) {
	      /* send more data to PM */
              DLGCSPINUNLOCK(intmod_lock, z_flag);
	      send_more2pm(q, bdmsp, mp, handle);
              DLGCSPINLOCK(intmod_lock, z_flag);
	    } else {
	      freemsg(mp);
	    }

	    /*
	     * Check WAIT flag because rput() could have 
	     * set it in the mean time
	     */
	    if ((unsigned long)bdmsp->bi_wqcnt <= (unsigned long)bdmsp->bi_bp.bp_sndlo
		&& !(bdmsp->bi_state & BDM_WAIT_MOREDATA)) {
              DLGCSPINUNLOCK(intmod_lock, z_flag);
	      send_bmt2usr(RD(q), NULL, handle, bdmsp);
	    } else {
              DLGCSPINUNLOCK(intmod_lock, z_flag);
 	    }
	    
	    DLGCSPLX(oldspl);
	    break;
	    
	 case MC_LASTDATA:
	   /* must be in SENDING state */
	   if (!(bdmsp->bi_state & SENDING)) {

/* Lucent fix  for lngjmp */
             DLGCSPINUNLOCK(intmod_lock, z_flag);
#ifdef NO_FIX 
	     DLGN_WQREPLY(q, mp, GNE_BADSTATE);
#else
	     freemsg(mp);
#endif
	     break;
	   }
	   
	   bdmsp->bi_state &= ~BDM_WAIT_MOREDATA;
	   
	   if (msgdsize(mp) != 0 && mp->b_cont == 0) {
             DLGCSPINUNLOCK(intmod_lock, z_flag);
	     DLGN_WQREPLY(q, mp, GNE_BADPARM);
	     break;
	   }

	   if (msgdsize(mp) == 0 && mp->b_cont != 0) {
	     freemsg(mp->b_cont);
	     mp->b_cont = 0;
	   }
	   
	   if (msgdsize(mp) > 0) {
	     queue_data(&bdmsp->bi_wq, mp->b_cont, &bdmsp->bi_wqcnt);
	   }
	   
	   mp->b_cont = 0;
	   
	   DLGCSPLSTR(oldspl);
	   
	   if ((unsigned long)bdmsp->bi_wqcnt <= 
	       (unsigned long)bdmsp->bi_bp.bp_2pmsize) {
	     
	     /* send last data to PM */
             DLGCSPINUNLOCK(intmod_lock, z_flag);
	     send_last2pm(q, bdmsp, mp, handle, 0); /* rwt VME base bug was 1 */
             DLGCSPINLOCK(intmod_lock, z_flag);
	     
	   } else {
	     if (bdmsp->bi_state & BDM_BUFMT) {
	       
	       /* send more data to PM */
               DLGCSPINUNLOCK(intmod_lock, z_flag);
	       send_more2pm(q, bdmsp, mp, handle);
               DLGCSPINLOCK(intmod_lock, z_flag);
	       
	     } else {
	       freemsg(mp);
	     }
	     
	     bdmsp->bi_state &= ~SENDING;
	     bdmsp->bi_state |= SEND_LAST;
	   }
	   DLGCSPLX(oldspl);
           DLGCSPINUNLOCK(intmod_lock, z_flag);
	   break;
	   
	 case MC_RECEIVEDATA:
	   /* must be in RECEIVE_IDLE state */
	   if (!(bdmsp->bi_state & RECEIVE_IDLE)) {
             DLGCSPINUNLOCK(intmod_lock, z_flag);
	     DLGN_WQREPLY(q, mp, GNE_BADSTATE);
	     break;
	   }
	   
	   /* there should not be data on receive queue */
	   if (bdmsp->bi_rq) {
             DLGCSPINUNLOCK(intmod_lock, z_flag);
	     DLGN_WQREPLY(q, mp, GNE_BADSTATE);
	     break;
	   }
	   
	   bdmsp->bi_state &= ~RECEIVE_IDLE;
	   bdmsp->bi_state |= RECEIVING;
	   
           DLGCSPINUNLOCK(intmod_lock, z_flag);
	   putnext(q, mp);
	   break;
	 }
      }
      break;
      
   case M_IOCTL:
     DLGCSPINUNLOCK(intmod_lock, z_flag);
     putnext(q, mp);
     break;
   }
   
   return(0);
}


/***********************************************************************
 *        NAME: queue_data(que, dbp, cnt)
 * DESCRIPTION: Link data onto our dvbm local queue.
 *      INPUTS: que - our internal read/write queue pointer
 *              dbp - data to be put onto queue
 *              cnt - number of bytes already on que 
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: linkb() splstr() splx() msgdsize()
 *    CAUTIONS:
 **********************************************************************/
int queue_data(que, dbp, cnt)
mblk_t   **que;
mblk_t   *dbp;
int      *cnt;
{
   DLGCDECLARESPL(oldspl)

   DLGC_DEBUG_MSG3(CE_DEBUG, "queue_data() cnt before %d\n", *cnt);

   DLGCSPLSTR(oldspl);

   /* put data on our local queue */
   if (*que) {
      linkb(*que, dbp);
   } else {
      *que = dbp;
   }

   *cnt += msgdsize(dbp);

   DLGCSPLX(oldspl);
   
   DLGC_DEBUG_MSG4(CE_DEBUG,"queue_data() cnt after %d msgdsize %d\n", 
      *cnt, msgdsize(*que));

   return(0);
}


/***********************************************************************
 *        NAME: int dvbmwsrv(q)
 * DESCRIPTION: Write service procedure.  Currently not necessary.
 *      INPUTS: q - module write queue
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: DLGC_MSG() getq()
 *    CAUTIONS:
 **********************************************************************/
int dvbmwsrv(q)
queue_t * q;
{
   mblk_t      *mp;
   GN_CMDMSG   *drvhdr;

   DLGC_DEBUG_MSG3(CE_DEBUG, "dvbmwsrv() q:%x\n", q);

   while ((mp = getq(q)) != (mblk_t *) NULL) {
      /* Need a service procedure in case next STREAM queue is full */
      drvhdr = (GN_CMDMSG *)mp->b_rptr;
      switch (mp->b_datap->db_type) {
      default:
         break;
      }
   }
   return(0);
}


/***********************************************************************
 *        NAME: int dvbmrput(q, mp)
 * DESCRIPTION: Read side put procedure; receive message from SM.
 *      INPUTS: q - read queue
 *              mp - message from SM.
 *     OUTPUTS:
 *     RETURNS: 0.
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int send_more2usr(queue_t *, BDM_INFO *, mblk_t *, int);
int send_last2usr(queue_t *, BDM_INFO *, mblk_t *, int);
#endif
int dvbmrput(q, mp)
queue_t  *q;
mblk_t   *mp;
{
   int            handle;
   GN_CMDMSG      *drvhdr;
   BDM_INFO       *bdmsp;
   int            sendbytes;
   struct iocblk  *iocp;
   GN_BIND        *bindp;
#ifndef LFS
   mblk_t         *nmp;
#endif
   GN_BULKPARM    *bmp;
   DLGCDECLARESPL(oldspl)

   DLGC_DEBUG_MSG4(CE_DEBUG, "dvbmrput() q:%x  mp:%x", q, mp);

   switch (mp->b_datap->db_type) {
   default:
      putnext(q, mp);
      break;

   case M_FLUSH:
      /* Check to see if Read queue should be flushed */
      if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {
	 DLGCSPLSTR(oldspl);
         flushq(q, FLUSHALL);
	 DLGCSPLX(oldspl);
      }

      /* Check to see if Write queue should be flushed */
      if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
	 DLGCSPLSTR(oldspl);
         flushq(WR(q), FLUSHALL);
         /*
          * We must insure that this message is sent to all modules on
          * the stream. Because we are a driver we need only worry 
          * about the upstream bound (Read queue) messages.
          * First we clear the write bit (So it isn't sent back
          * downstream by stream head) then send it upstream with
          * qreply()
          */
         *mp->b_rptr |= FLUSHW;     /* Set read queue flush */
         *mp->b_rptr &= ~FLUSHR;    /* Clear write queue flush */
	 DLGCSPLX(oldspl);
         break;
      }
      break;

   case M_IOCACK:
      /* check for successful binds, set up bdm_info */
      iocp = (struct iocblk *)mp->b_rptr;
      drvhdr = (GN_CMDMSG *)mp->b_cont->b_rptr;
      if (drvhdr->fw_msg_ident != MC_BINDCMPLT || iocp->ioc_rval != 0) {
         putnext(q, mp);
         break;
      }

      /* process bind complete */
      bindp = (GN_BIND *)(&((GN_CMDMSG *)mp->b_cont->b_rptr)[1]);
      handle = bindp->gb_handle;

      /* initialize bdm_info for this handle */
      bdmsp = &bdm_info[handle];
      bdmsp->bi_state = SEND_IDLE | RECEIVE_IDLE | BDM_INITED;

      /* discard left-over data */
      if (bdmsp->bi_wq) {
         freemsg(bdmsp->bi_wq);
      }

      bdmsp->bi_wq = (mblk_t *)NULL;

      /* discard left-over data */
      if (bdmsp->bi_rq) {
         freemsg(bdmsp->bi_rq);
      }

      bdmsp->bi_rq = (mblk_t *)NULL;
      bdmsp->bi_wqcnt = 0;
      bdmsp->bi_rqcnt = 0;

      /* save the rq so close() can clean up the proper structures */
      bdmsp->bi_qptr = q;

      /* clearing is causing problem due to user configurable parms */
      bmp = &bdmsp->bi_bp;
      if (!bmp->bp_sndlo && !bmp->bp_sndhi && !bmp->bp_rcvlo &&
          !bmp->bp_rcvhi && !bmp->bp_sndmax && !bmp->bp_rcvmax && !bmp->bp_2pmsize) {
      	  /* copy default bulk params */
          init_bulkparam(&bdmsp->bi_bp);
      }

      DLGC_DEBUG_MSG4(CE_DEBUG,"dvbmrput(): IOCACK on handle:%d q:%x\n", handle, q);

      putnext(q,mp);
      break;

   case M_PROTO:

      drvhdr = (GN_CMDMSG *)mp->b_rptr;
      handle = drvhdr->cm_address.da_handle;
      bdmsp = &bdm_info[handle];

      /* check for prompted record */
      if (drvhdr->fw_msg_type & DL_PRSTATUS) {
          drvhdr->fw_msg_type &= ~DL_PRSTATUS;
          /* set flag so when FLUSH occurs state will be receiving */
          bdmsp->bi_state |= BDM_PRSTATUS;
      } 

      switch (drvhdr->cm_msg_type) {
      default:
         putnext(q, mp);
         break;

      case MT_CONTROL:
         /* release everything associated with this handle */
         if (drvhdr->fw_msg_ident == MC_UNBINDCMPLT && 
             drvhdr->fw_errcode == 0) {
            if (bdmsp->bi_wq) {
               freemsg(bdmsp->bi_wq);
            }
            if (bdmsp->bi_rq) {
               freemsg(bdmsp->bi_rq);
            }
            bzero((char *)bdmsp, sizeof(BDM_INFO));
         }
         putnext(q, mp);
         break;

      case MT_BULK:
         /* for prompted record set to receive */
	 if (bdmsp->bi_state & BDM_PRSTATUS) {
             bdmsp->bi_state = RECEIVING | BDM_INITED | SEND_IDLE;
         }
         switch (drvhdr->fw_msg_ident) {
         default:    /* pass through everything else */
            putnext(q, mp);
            break;

         case MC_MOREDATA:    /* receive data */
            /* must be in RECEIVING state */
            if (!(bdmsp->bi_state & RECEIVING)) {
               DLGC_MSG2(CE_CONT, "dvbmrput() MC_MOREDATA in wrong state\n");
               /* wrong state for message, drop it */
               freemsg(mp);
               break;
            }

            if (msgdsize(mp) == 0 || mp->b_cont == 0) {
               DLGC_MSG2(CE_CONT, "dvbmrput() MC_MOREDATA with no data\n");
               /* discard mp */
               freemsg(mp);
               break;
            }

            /* If we've reached threshold, throw away data */
            if ((unsigned long)bdmsp->bi_rqcnt >= (unsigned long)bdmsp->bi_bp.bp_rcvmax) {
               DLGC_MSG2(CE_CONT, "dvbmrput() rcvthreshhold exceeded; MC_MOREDATA discarded\n");
               freemsg(mp);
               break;
            }

            /* put data on internal queue */
            queue_data(&bdmsp->bi_rq, mp->b_cont, &bdmsp->bi_rqcnt);
            mp->b_cont = 0;

            /* If we've reached high water mark, send data to user */
            if ((unsigned long)bdmsp->bi_rqcnt >= (unsigned long)bdmsp->bi_bp.bp_rcvhi) {
               send_more2usr(q, bdmsp, mp, handle);
            } else {
               freemsg(mp);
            }

            break;

         case MC_LASTDATA:
            /* must be in RECEIVING state */
            if (!(bdmsp->bi_state & RECEIVING)) {
               DLGC_MSG2(CE_CONT, "dvbmrput() MC_LASTDATA in wrong state\n");
               /* throw away message */
               freemsg(mp);
               break;
            }

            if (msgdsize(mp) != 0 && mp->b_cont == 0) {
               DLGC_MSG2(CE_CONT, "dvbmrput() MC_LASTDATA with no data\n");
               break;
            }

            bdmsp->bi_state &= ~RECEIVING;
            bdmsp->bi_state |= RECEIVE_LAST;
            if (msgdsize(mp) > 0) {
               queue_data(&bdmsp->bi_rq, mp->b_cont, &bdmsp->bi_rqcnt);
            }
            mp->b_cont = 0;

            if (!(bdmsp->bi_state & RECEIVE_IDLE)) {
	       while (!(bdmsp->bi_state & RECEIVE_IDLE)) {
		  if ((unsigned long)bdmsp->bi_rqcnt <= (unsigned long)bdmsp->bi_bp.bp_rcvhi) {
		     /* send last data to PM */
		     if (send_last2usr(q, bdmsp, mp, handle) < 0) {
			DLGC_MSG2(CE_CONT, "dvbmrput() send_last2usr() aborted.\n");
			break;
		     }
		     mp = NULL;
		     break;
		  } else {
		     /* send more data to PM */
		     if (send_more2usr(q, bdmsp, mp, handle) < 0) {
			DLGC_MSG2(CE_CONT, "dvbmrput() send_more2usr() aborted.\n");
			break;
		     }
		     mp = NULL;
		     continue;
		  }
	       }
	    }
            break;
         }
         break;

      case MT_VIRTEVENT:

         switch (drvhdr->fw_msg_ident) {
         default:    /* pass through anything else */
            putnext(q, mp);
            break;

         case MC_BUFFMT:   /* buffer empty */
            /* must not be in SEND_IDLE state */
            if ((bdmsp->bi_state & SEND_IDLE)) {
               freemsg(mp);
               break;
            }
            /* nothing to send */
            if ((bdmsp->bi_wqcnt <= 0) &&
               !(bdmsp->bi_state & SEND_LAST)) {
               /* 
                * have to remember to send data down
                * when we get data from user
                */
               bdmsp->bi_state |= BDM_BUFMT;
               if (!(bdmsp->bi_state & BDM_WAIT_MOREDATA)) {
                  if (send_bmt2usr(q, mp, handle, bdmsp) == 0) {
                     mp = NULL;
                  }
               }
               DLGC_DEBUG_MSG2(CE_DEBUG, " MC_BUFMT: bi_wqcnt = 0\n");
               if (mp) {
                  freemsg(mp);
               }
               return(0);
            }

            sendbytes = bdmsp->bi_bp.bp_2pmsize;
            if (bdmsp->bi_wqcnt <= sendbytes && 
                (bdmsp->bi_state & SEND_LAST)) {

               /* send last data to PM */
               send_last2pm(WR(q), bdmsp, mp, handle, 0);
               break;
            }

            send_more2pm(WR(q), bdmsp, mp, handle);

            /*
             * check if we've fallen below low water mark
             * if yes, send BUFFMT message to user
             */
           if ((unsigned long)bdmsp->bi_wqcnt <= (unsigned long)bdmsp->bi_bp.bp_sndlo
                 && !(bdmsp->bi_state & BDM_WAIT_MOREDATA)
                 && !(bdmsp->bi_state & SEND_LAST)) {

               send_bmt2usr(q, NULL, handle, bdmsp);
	   }
            break;

         case MC_BUFFUL:   /* Should not get this on receive */
            DLGC_MSG2(CE_CONT, "dvbmrput() illegal MC_BUFFUL\n");
            freemsg(mp);
            break;
         }
      }
   }

   return(0);
}


/***********************************************************************
 *        NAME: int dvbmrsrv(q)
 * DESCRIPTION: Read side service procedure.  Should not reach here.
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 ***********************************************************************/
int dvbmrsrv(q)
queue_t *q;
{
   mblk_t *mp;

   /* NO LOCKS ARE PUT HERE BECAUSE WE WILL NEVER GET HERE */

   DLGC_MSG2(CE_CONT, "dvbmrsrv(): SHOULD NEVER GET HERE\n");

   while ((mp = getq(q)) != (mblk_t *) NULL) {
      DLGC_MSG3(CE_CONT, "dvbmrsrv(): Freeing mp %p\n", mp);
      freemsg(mp);
   }
   return(0);
}

/***********************************************************************
 *        NAME: PRIVATE int gn_splitmsg(m1pp,m2pp,m1len)
 * DESCRIPTION: Function to split a multi-block data message into two
 *              parts. The original message block pointer is passed in
 *              m1pp. The pointer to the new m1len-byte 1st message
 *              block is returned in m1pp, and the pointer to the 2nd
 *              remainder message is returned in m2pp.
 *      INPUTS: m1pp - pointer to original mblk_t message pointer.
 *              m2pp - pointer to 2nd mblk_t message pointer.
 *              m1len - length of final 1st message.
 *     OUTPUTS: m1pp - pointer to new m1len-byte 1st message pointer.
 *     RETURNS: 0 if successful; else -1.
 *       CALLS: dupb()
 *    CAUTIONS: This function assumes it is either called by the
 *              interrupt handler or under splstr() protection.
 **********************************************************************/
PRIVATE int gn_splitmsg(m1pp, m2pp, m1len)
mblk_t   **m1pp;
mblk_t   **m2pp;
unsigned int   m1len;
{
   mblk_t         *m1p = *m1pp;
   int            diff;
#ifdef LFS
   unsigned int   size = 0;
#else
   unsigned int   size;
#endif

   DLGC_DEBUG_MSG3(CE_DEBUG, "gn_splitmsg() len %d\n", m1len);

   /* Handle special case of leading split */
   if (m1len == 0) {
      *m2pp = m1p;
      *m1pp = NULL;
   }

   /* Find the boundary position in the message */
   diff = -m1len;
   for ( ; m1p != NULL; m1p = m1p->b_cont) {
      diff += size = m1p->b_wptr - m1p->b_rptr;
      if (diff >= 0) {
         break;
      }
   }

   if (m1p == NULL)     /* If m1len is longer than the message */
      return(-1);       /* Declare error */

   if (diff == 0) {
      /* If inter-block split, make m1's next block m2's first block */
      *m2pp = m1p->b_cont;
   } else {
      /* Else, if intra-block split, duplicate block in question */
      if ((*m2pp = dupb(m1p)) == NULL) {
         return(-1);    /* Declare error of dupb fails */
      }

      /* Adjust wptr of m1's 'last' block */
      m1p->b_wptr -= diff;

      /* Adjust rptr of m2's first block */
      (*m2pp)->b_rptr += (size - diff);

      /* Make m1's next block m2's 2nd block */
      (*m2pp)->b_cont = m1p->b_cont;
   }

   /* Make m1's current block its last block */
   m1p->b_cont = NULL;

   return(0);
}

/***********************************************************************
 *        NAME: int send_bmt2usr(q, mp, handle)
 * DESCRIPTION: Send MC_BUFFMT message to user.
 *      INPUTS: q - read queue
 *              mp - mp to copy from
 *              handle - smhandle index
 *     OUTPUTS: a BUFFMT message sent upstream.
 *     RETURNS: 0 if successful
 *              -1 if dlgn_copymp failed
 *       CALLS: dlgn_copymp() DLGC_MSG() freemsg() bzero()
 *    CAUTIONS:
 **********************************************************************/
int send_bmt2usr(q, mp, handle, bdmsp)
queue_t  *q;
mblk_t   *mp;
int      handle;
BDM_INFO *bdmsp;
{
   mblk_t      *nmp;
   GN_CMDMSG   *drvhdr;

   if ((nmp = mp) == NULL) {
      if ((nmp = dlgn_copymp(NULL,0)) == (mblk_t *)NULL) {
         DLGC_MSG2(CE_WARN, "send_bmt2usr: cannot copy mp!\n");
         return(-1);
      }
   } else {
      bzero(nmp->b_rptr, sizeof(GN_CMDMSG));
   }

   drvhdr = (GN_CMDMSG *)nmp->b_rptr;
   drvhdr->cm_msg_type = MT_VIRTEVENT;
   drvhdr->fw_msg_ident = MC_BUFFMT;
   drvhdr->cm_address.da_handle = handle;
   nmp->b_datap->db_type = M_PROTO;
   if (nmp->b_cont) {
      freemsg(nmp->b_cont);
   }
   nmp->b_cont = 0;            
   bdmsp->bi_state |= BDM_WAIT_MOREDATA;

   putnext(q, nmp);

   return(0);
}

/***********************************************************************
 *        NAME: send_more2pm(q, bdmsp, mp, handle)
 * DESCRIPTION: Send more data (bp_2pmsize bytes) via MC_MOREDATA
 *              to PM. If less than bp_2pmsize bytes, send all we
 *              have.
 *      INPUTS: q - write queue
 *              bdmsp - bdm internal structure
 *              mp - message to make copy of
 *              handle - smhandle index
 *     OUTPUTS: MC_MOREDATA message to SM.
 *     RETURNS:  0 if successful
 *              -1 if dlgn_copymp failed
 *       CALLS: dlgn_copymp() DLGC_MSG() bzero() splstr() splx()
 *    CAUTIONS:
 **********************************************************************/
int send_more2pm(q, bdmsp, mp, handle)
queue_t  *q;
BDM_INFO *bdmsp;
mblk_t   *mp;
int      handle;
{
   GN_CMDMSG   *drvhdr;
   int         sendbytes;
   DLGCDECLARESPL(oldspl)

   if (mp == NULL) {
      if ((mp = dlgn_copymp(NULL,0)) == (mblk_t *)NULL) {
         DLGC_MSG2(CE_WARN, "BM: send_more2pm: cannot copy mp!\n");
         return(-1);
      }
   } else {
      bzero(mp->b_rptr, sizeof(GN_CMDMSG));
   }

   if (mp->b_cont) {
      freemsg(mp->b_cont);
   }

   sendbytes = bdmsp->bi_bp.bp_2pmsize;

   /*
    * Send bdm2pm_size bytes of data to PM. If we have less than
    * bdm2pm_size bytes, send all we have
    */

   DLGCSPLSTR(oldspl);

   if (bdmsp->bi_wqcnt <= sendbytes) {
      /*
       * We have to remember to send data down when we get data
       * from user
       */
      mp->b_cont = bdmsp->bi_wq;
      bdmsp->bi_wq = NULL;
      bdmsp->bi_wqcnt = 0;
      bdmsp->bi_state |= BDM_BUFMT;

   } else {

      mp->b_cont = bdmsp->bi_wq;
      bdmsp->bi_wq = (mblk_t *)NULL;
      if (gn_splitmsg(&mp->b_cont, &bdmsp->bi_wq, sendbytes) == -1) {
         /* can't split message, error */
         DLGC_MSG2(CE_CONT, "send_more2pm() gn_splitmsg failed.\n");
	 DLGCSPLX(oldspl);
         freemsg(mp);
         return(-1);
      }
      bdmsp->bi_wqcnt = msgdsize(bdmsp->bi_wq);
   }

   DLGCSPLX(oldspl);

   DLGC_DEBUG_MSG3(CE_DEBUG,
      "send_more2pm(): data message %d bytes\n", msgdsize(mp));

   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   drvhdr->cm_msg_type = MT_BULK;
   drvhdr->fw_msg_ident = MC_MOREDATA;
   drvhdr->cm_address.da_handle = handle;
   mp->b_datap->db_type = M_PROTO;
   bdmsp->bi_state &= ~BDM_BUFMT;
   /* give it to SM */
   putnext(q, mp);

   return(0);
}


/***********************************************************************
 *        NAME: send_last2pm(q, bdmsp, mp, handle)
 * DESCRIPTION: send MC_LASTDATA to PM with attached send data.
 *      INPUTS: q - write queue
 *              bdmsp - bdm internal structure
 *              mp - message to make into MC_LASTDATA
 *              handle - smhandle index
 *     OUTPUTS: MC_LASTDATA with data attached
 *     RETURNS:  0 if successful
 *              -1 if dlgn_copymp failed
 *       CALLS: dlgn_copymp() DLGC_MSG() freemsg() splstr() splx()
 *    CAUTIONS:
 **********************************************************************/
int send_last2pm(q, bdmsp, mp, handle, setsendlast)
queue_t  *q;
BDM_INFO *bdmsp;
mblk_t   *mp;
int      handle;
int      setsendlast;
 {
   GN_CMDMSG   *drvhdr;
   DLGCDECLARESPL(oldspl)

   if (mp == NULL) {
      if ((mp = dlgn_copymp(NULL,0)) == (mblk_t *)NULL) {
         DLGC_MSG2(CE_WARN, "BM: send_last2pm: cannot copy mp!\n");
         return(-1);
      }
   } else {
      bzero(mp->b_rptr, sizeof(GN_CMDMSG));
   }

   if (mp->b_cont) {
      freemsg(mp->b_cont);
   }

   DLGCSPLSTR(oldspl);

   mp->b_cont = bdmsp->bi_wq;
   bdmsp->bi_wq = NULL;
   bdmsp->bi_wqcnt = 0;

   DLGCSPLX(oldspl);

   DLGC_DEBUG_MSG3(CE_DEBUG, "send_last2pm(): data message %d bytes\n", msgdsize(mp));

   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   drvhdr->cm_msg_type = MT_BULK;
   drvhdr->fw_msg_ident = MC_LASTDATA;
   drvhdr->cm_address.da_handle = handle;
   mp->b_datap->db_type = M_PROTO;
   bdmsp->bi_state &= ~(SENDING | SEND_LAST | BDM_BUFMT);
   if (setsendlast) {
     bdmsp->bi_state |= (SEND_LAST | SEND_IDLE);
   } else {
     bdmsp->bi_state |= SEND_IDLE;
   }
   putnext(q, mp);

   return(0);
}

/***********************************************************************
 *        NAME: int send_more2usr(q, bdmsp, mp, handle)
 * DESCRIPTION: send MC_MOREDATA to user with attached recorded data.
 *      INPUTS: q - read queue
 *              bdmsp - bdm internal structure
 *              mp - message to make into MC_MOREDATA
 *              handle - smhandle index
 *     OUTPUTS: MC_MOREDATA with data attached
 *     RETURNS:  0 if successful
 *              -1 if dlgn_copymp failed
 *       CALLS: dlgn_copymp() DLGC_MSG() freemsg() splstr() splx()
 *              soft_split()
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int soft_split(mblk_t **, mblk_t **, int);
#endif
int send_more2usr(q, bdmsp, mp, handle)
queue_t  *q;
BDM_INFO *bdmsp;
mblk_t   *mp;
int      handle;
{
   GN_CMDMSG   *drvhdr;
   int         sendbytes;
   DLGCDECLARESPL(oldspl)

   if (mp == NULL) {
      if ((mp = dlgn_copymp(NULL,0)) == (mblk_t *)NULL) {
#ifndef LFS
	 DLGCSPLX(oldspl);
#endif
         DLGC_MSG2(CE_WARN, "BM: send_more2usr: cannot copy mp!\n");
         return(-1);
      }
   } else {
      bzero(mp->b_rptr, sizeof(GN_CMDMSG));
   }

   if (mp->b_cont) {
      freemsg(mp->b_cont);
   }

   sendbytes = bdmsp->bi_bp.bp_rcvhi;

   DLGCSPLSTR(oldspl);

   if (bdmsp->bi_rqcnt <= sendbytes) {
      /*
       * We have to remember to send data down when we get data
       * from user
       */
      mp->b_cont = bdmsp->bi_rq;
      bdmsp->bi_rq = NULL;
      bdmsp->bi_rqcnt = 0;
   } else {
      mp->b_cont = bdmsp->bi_rq;
      bdmsp->bi_rq = (mblk_t *)NULL;
      if (soft_split(&mp->b_cont, &bdmsp->bi_rq, sendbytes) < 0) {
	 DLGCSPLX(oldspl);
         /* can't soft split message, error */
         DLGC_MSG2(CE_CONT, "send_more2usr() soft_split failed.\n");
         freemsg(mp);
         return(-1);
      }

      if (bdmsp->bi_rq) {
         bdmsp->bi_rqcnt = msgdsize(bdmsp->bi_rq);
      } else {
         bdmsp->bi_rqcnt = 0;
      }
   }

   DLGCSPLX(oldspl);

   DLGC_DEBUG_MSG3(CE_DEBUG, "send_more2usr(): data message %d bytes\n", msgdsize(mp));
   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   drvhdr->cm_msg_type = MT_BULK;
   drvhdr->fw_msg_ident = MC_MOREDATA;
   drvhdr->cm_address.da_handle = handle;
   mp->b_datap->db_type = M_PROTO;

   putnext(q, mp);

   return(0);
}


/***********************************************************************
 *        NAME: int send_last2usr(q, bdmsp, mp, handle)
 * DESCRIPTION: send MC_LASTDATA of record data to user
 *      INPUTS: q - read queue
 *              bdmsp - bdm internal structure
 *              mp - message to make into MC_LASTDATA
 *              handle - smhandle index
 *     OUTPUTS: MC_LASTDATA message with record data attached.
 *     RETURNS:  0 if successful
 *              -1 if dlgn_copymp failed
 *       CALLS: dlgn_copymp() DLGC_MSG() freemsg() splstr() splx()
 *    CAUTIONS:
 **********************************************************************/
int send_last2usr(q, bdmsp, mp, handle)
queue_t  *q;
BDM_INFO *bdmsp;
mblk_t   *mp;
int      handle;
{
   GN_CMDMSG   *drvhdr;
   DLGCDECLARESPL(oldspl)

   if (mp == NULL) {
      if ((mp = dlgn_copymp(NULL,0)) == (mblk_t *)NULL) {
         DLGC_MSG2(CE_WARN, "BM: send_last2usr: cannot copy mp!\n");
         return(-1);
      }
   } else {
      bzero(mp->b_rptr, sizeof(GN_CMDMSG));
   }

   if (mp->b_cont) {
      freemsg(mp->b_cont);
   }

   DLGCSPLSTR(oldspl);

   /* attach data */
   mp->b_cont = bdmsp->bi_rq;
   bdmsp->bi_rq = NULL;
   bdmsp->bi_rqcnt = 0;
   bdmsp->bi_state &= ~(RECEIVING | RECEIVE_LAST);
   bdmsp->bi_state |= RECEIVE_IDLE;

   DLGCSPLX(oldspl);

   DLGC_DEBUG_MSG3(CE_DEBUG, "send_last2usr(): data message %d bytes\n", msgdsize(mp));

   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   drvhdr->cm_msg_type = MT_BULK;
   drvhdr->fw_msg_ident = MC_LASTDATA;
   drvhdr->cm_address.da_handle = handle;
   mp->b_datap->db_type = M_PROTO;

   /* send upstream */
   putnext(q, mp);

   return(0);
}


/***********************************************************************
 *        NAME: int soft_split(mp1, mp2, len)
 * DESCRIPTION: split message into 2 parts, first part about len bytes
 *      INPUTS: mp1 - source mp
 *              mp2 - destination mp, also holds remainder
 *              len - approximate size of resultant mp1
 *     OUTPUTS:
 *     RETURNS:  0 if split successful
 *              -1 otherwise
 *       CALLS: msgdsize()
 *    CAUTIONS:
 **********************************************************************/
int soft_split(mp1, mp2, len)
mblk_t   **mp1;
mblk_t   **mp2;
int      len;
 {
   int         sz1;
   int         sz2;
   mblk_t      *ptr = *mp1;

   /* no split necessary */
   if (len >= msgdsize(ptr)) {
      return(0);
   }

   /* less than len bytes, can't do */
   if ((ptr->b_wptr - ptr->b_rptr) > len) {
      return (-1);
   }

   sz1 = 0;

   while (ptr->b_cont != NULL) {

      sz1 += ptr->b_wptr - ptr->b_rptr;
      sz2 = ptr->b_cont->b_wptr - ptr->b_cont->b_rptr;

      if (sz1 <= len && (sz1 + sz2) > len) {
         *mp2 = ptr->b_cont;
         ptr->b_cont = 0;
         return(0);
      }

      ptr = ptr->b_cont;
   }

   return (-1);
}


/***********************************************************************
 *        NAME: int init_bulkparam(bmp)
 * DESCRIPTION: Set initial default bulk data transfer parameters
 *      INPUTS: bmp - pointer to a bi_bp structure
 *     OUTPUTS: bmp filled in with default values
 *     RETURNS: 0.
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
int init_bulkparam(bmp)
GN_BULKPARM *bmp;
{
   bmp->bp_sndlo   = DEF_BDM_SENDLOW;
   bmp->bp_sndhi   = DEF_BDM_SENDHIGH;
   bmp->bp_rcvlo   = DEF_BDM_RECEIVELOW;
   bmp->bp_rcvhi   = DEF_BDM_RECEIVEHIGH;
   bmp->bp_sndmax  = DEF_BDM_SENDTHRESHOLD;
   bmp->bp_rcvmax  = DEF_BDM_RCVTHRESHOLD;
   bmp->bp_2pmsize = DEF_BDM2PM_XFERSIZE;

   return(0);
}

