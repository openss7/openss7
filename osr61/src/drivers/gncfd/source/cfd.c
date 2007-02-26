/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : cdf.c
 * Description                  : Generic Configuration Device Driver
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
#ifndef LFS
#include <sys/lismem.h>
#endif

#include "gndefs.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndbg.h"
#include "gnerrno.h"
#include "gnlogmsg.h"
#include "gnstart.h"
#include "gnstatus.h"
#include "gncfd.h"
#include "gndlgn.h"
#include "typedefs.h"
#include "dlgn2cfd.h"
#include "pmswtbl.h"
#include "cfmsg.h"
#include "cfd.h"
#include "cfdextern.h"
#include "drvdebug.h"
#include "dlgcos.h"
#include "cfd2dlgn.h"

static int gncfd_major = 0;
MODULE_AUTHOR("Intel");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dialogic Configuration Driver");

#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("gncfd");
MODULE_ALIAS("streams-gncfd");
MODULE_ALIAS("streams-gncfdDriver");
#endif
#endif

int gn_devflag = 0;

#define PKTSZ_MIN   0      /* Packet Size Minimum */
#define PKTSZ_MAX   0      /* Packet Size Maximum */
#define HI_WATER    0      /* High Water Mark */
#define LO_WATER    0      /* Low Water Mark */

#if LINUX_VERSION_CODE > 0x020600
#define LINUX26
#endif

/* Prototypes for STREAMS functions */
#ifdef LFS
void gn_init(void);
int  gn_start(void);
int  streamscall gn_open(queue_t *, dev_t *, int, int, cred_t *);
int  streamscall gn_close(queue_t *, int, cred_t *);
int  gn_wput(queue_t *, mblk_t *);
#else
void gn_init();
int  gn_start();
int  gn_open();
int  gn_close();
int  gn_wput();
#endif

/* Prototypes for functions PRIVATE to this module */
#ifdef LFS
PRIVATE void cf_startgndrv(queue_t *, mblk_t *);
PRIVATE void cf_stopgndrv(queue_t *, mblk_t *);
PRIVATE void cf_startpm(queue_t *, mblk_t *);
PRIVATE void cf_stoppm(queue_t *, mblk_t *);
PRIVATE void cf_startbd(queue_t *, mblk_t *);
PRIVATE void cf_stopbd(queue_t *, mblk_t *);
PRIVATE int  cf_clearpm(GN_HEADER *, int);
PRIVATE void cf_setlog(queue_t *, mblk_t *);
PRIVATE void cf_mklogname(GN_LOGDEV *, GN_BOARD *, unsigned long);
PRIVATE void cf_copystr(register unsigned char *, register unsigned char *, unsigned long);
PRIVATE void cf_status(queue_t *, mblk_t *);
PRIVATE void cf_stats(queue_t *, mblk_t *);
PRIVATE void cf_debug(queue_t *, mblk_t *);
PRIVATE char *cf_ckbdname(GN_INITBD *, unsigned long);
extern int  cf_cmpstr(register unsigned char *, register unsigned char *, unsigned int);
PRIVATE void cf_testlog(void);
#else
PRIVATE void cf_startgndrv();
PRIVATE void cf_stopgndrv();
PRIVATE void cf_startpm();
PRIVATE void cf_stoppm();
PRIVATE void cf_startbd();
PRIVATE void cf_stopbd();
PRIVATE int  cf_clearpm();
PRIVATE void cf_setlog();
PRIVATE void cf_mklogname();
PRIVATE void cf_copystr();
PRIVATE void cf_status();
PRIVATE void cf_stats();
PRIVATE void cf_debug();
PRIVATE char *cf_ckbdname();
extern int  cf_cmpstr();
PRIVATE void cf_testlog();
#endif

/* Read Side Module Information */
static struct module_info rinfo = {   
   GNCFG_ID,            /* Module ID. Used for logging */
   "gncfd",             /* Module name. Same as that used in mdevice */
   PKTSZ_MIN,           /* Minimum packet size. */
   PKTSZ_MAX,           /* Maximum packet size. */
   HI_WATER,            /* High water mark. Used in flow control.*/
   LO_WATER             /* Low water mark. Ditto. */
};

/* Write Side Module Information */
static struct module_info cfdinfo = {   
   GNCFG_ID,            /* Module ID. Used for logging */
   "gncfd",             /* Module name. Same as that used in mdevice */
   PKTSZ_MIN,           /* Minimum packet size. */
   16 * 1024,           /* Maximum packet size. */
   16 * 1024,           /* High water mark. Used in flow control. */
   1                    /* Low water mark. Ditto. */
};

/* Module Read Side Queue */
PRIVATE struct qinit rinit = {   
   NULL,                /* Pointer to Put routine. */
   NULL,                /* Pointer to Service routine. */
   gn_open,             /* Pointer to Open routine. */
   gn_close,            /* Pointer to Close routine. */
   NULL,                /* Reserved by STREAMS for future use. */
   &rinfo,              /* Pointer to module_info structure. */
   NULL                 /* Pointer to optional statistics structure.*/
};

/* Module Write Queue */
PRIVATE struct qinit winit = {   
   gn_wput,             /* Pointer to Put routine. */
   NULL,                /* Pointer to Service routine. */
   NULL,                /* Pointer to Open routine. */
   NULL,                /* Pointer to Close routine. */
   NULL,                /* Reserved by STREAMS for future use. */
   &cfdinfo,            /* Pointer to module_info structure. */
   NULL                 /* Pointer to optional statistics structure.*/
};


/*
 * Streamtab entry - Used in cdevsw and fmodsw to point to the driver or
 * module. This is typically the only public structure in STREAMS
 */
struct streamtab gn_info = {   
   &rinit,              /* Pointer to Read Queue Init. Structure. */
   &winit,              /* Pointer to Write Queue Init. Structure. */
   NULL,                /* Multiplexor Read Queue Init. Structure. */
   NULL                 /* Multiplexor Write Queue Init. Structure. */
};


/* PRIVATE global variables */
PRIVATE CF_DEV Gn_Dev[CD_MAXDEV] = { {0,}, };
PRIVATE int    Cf_Sanity = 0;

/*
 *  Include this file to get access to data structures and public
 *  functions that were originally in this file but are currently in
 *  dlgn.c for the solaris version.
 */
extern int            Gn_Msglvl   ;
extern unsigned int   Gn_Numpms   ;  /* Total # of PM's in system */
extern unsigned int   Gn_Memsz    ;  /* Total memory alloc'd for structs */
extern GN_HEADER      *Gn_Headerp ;  /* Base of GN_HEADER array */
extern GN_BOARD       *Gn_Boardp  ;  /* Base of GN_BOARD array */
extern char           *Gn_Endp    ;  /* End of driver's alloc'd memory */
 

int gncfdinit(void)
{
    gn_init();
    return 0;
}
 
/* Functions needed for loadable Linux streams drivers. */
int init_module(void)
{
   int ret;
   
   ret = lis_register_strdev(gncfd_major, &gn_info, CD_MAXDEV, LIS_OBJNAME_STR);

   if (ret < 0) {
       printk("%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
       return ret;
   }
   
   if (gncfd_major == 0) {
       gncfd_major = ret ;
   }

   gn_init();

   return 0;
}
 
void cleanup_module(void)
{
   int err;
 
   err = lis_unregister_strdev(gncfd_major);
   if (err < 0)
       printk("%s: Unable to unregister module, error %d.\n", LIS_OBJNAME_STR, -err);
   else
       printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
   
   return; 
}
 
struct map     *gn_map       = NULL;/* Generic Driver dynamic memory pool */
int            gn_map_cnt    = 0;   /* Number of entries for the map table */
int            gn_map_sz     = 0;   /* gn_mapcnt * sizeof(struct map) */
char           *gn_buffer    = NULL;/* Generic Driver dynamic memory pool */
int            gn_buffer_sz  = 0;   /* gn_map_cnt * sizeof(HOST_CMDS) */


/***********************************************************************
 *        NAME: gn_init
 * DESCRIPTION: Config Driver Init routine. Displays sign-on and
 *              copyright messages on console.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *    CAUTIONS: none
 **********************************************************************/
void gn_init()
{
   static char   Cf_Signon[] =
      "Dialogic Configuration Device Driver\n%s\n%s";
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
}


/***********************************************************************
 *        NAME: gn_start
 * DESCRIPTION: Config Driver start routine.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 **********************************************************************/
int gn_start()
{
   PM_SWTBL       *psp;
   unsigned int  pscnt = 0;

   /*
    * Verify the count of installed PM's. If there was an undetected
    * error somewhere in the installation process, or if the .mastercfg
    * file was corrupted, we don't want to crash the system by calling a
    * nonexistent PM routine.
    */
   for (psp = pmswtbl; psp->startpm != NULL; psp++, pscnt++)
      ;

   /*
    * The count of entries in the pmswtbl must match the Gn_Maxpm
    * count that was determined at install time. If not, then the 
    * pmswtbl is invalid and unusable. Users should never see this
    * message.
    */
   if (pscnt != Gn_Maxpm) {
      Cf_Sanity = -1;     /* Shut us off forever */
      DLGC_MSG2(CE_WARN,"gn_start: installation error detected.");
      DLGC_MSG2(CE_WARN,"Dialogic Generic Driver must be re-installed\n");
   } else {
      Cf_Sanity = 0;
   }

   return (0);
}

/***********************************************************************
 *        NAME: gn_open
 * DESCRIPTION: This open entry routine supports both control device and
 *              clone devices. Control device is used to configure the
 *              hardware & system data structures. The clone devices are
 *              used to manage system information, i.e. queries, ...etc.
 *      INPUTS: readq - read queue
 *              dev - device number
 *              flag, sflag - open flags from user and system
 *     OUTPUTS: minor() or getminor() (SVR4)
 *     RETURNS: OPENFAIL and device number if success
 **********************************************************************/
int gn_open(readq, devp, flag, sflag, crp)
queue_t	*readq;
dev_t	*devp;
int	flag;
int	sflag;
cred_t	*crp;
{
   int dev;

   /*
    * Cannot permit any opens if startup sanity check failed.
    * Should never happen. (Famous last words) :-)
    */
   if (Cf_Sanity) {
      DLGC_MSG2(CE_WARN,"gn_open: installation error detected.");
      DLGC_MSG2(CE_WARN,"Dialogic Generic Driver must be re-installed\n");
      return (ENXIO);
   }

   /*
    * CLONEOPEN for applications to query, or non-inited related commands
    */
   if (sflag == CLONEOPEN)              /* return minor dev if clone open */
   {
      /* find a free entry */

      for (dev = 0; dev < CD_MAXDEV; dev++) {
         if (Gn_Dev[dev].rq == (queue_t *) NULL) {
            break;
         }
      }
      
      if (dev == CD_MAXDEV) {
         return (ENXIO);
      }
   } else {
      dev = getminor(*devp);
   }

   if (Gn_Dev[dev].rq == (queue_t *) NULL) {
      Gn_Dev[dev].rq = readq;
   }
   
   if ((dev == CD_GNCFG) && (Gn_Dev[dev].rq != readq)) {
      DLGC_MSG4(CE_CONT,"cf_open(): CTL DEV rq conflicts 0x%p,0x%p\n",
         readq, Gn_Dev[dev].rq);
   }
   
   /* initialization if any */
   Gn_Dev[dev].id = dev;

#ifdef DLGC_LIS_FLAG
   *devp = makedevice(getmajor(*devp), dev);
#else
   *devp = makedevice(MAJOR(*devp), dev);
#endif

#ifndef LINUX26
   MOD_INC_USE_COUNT;
#endif
   
   /* set up queue connections */
   readq->q_ptr = (char *)&Gn_Dev[dev];

   return (0);
}
      

/***********************************************************************
 *        NAME: gn_close
 * DESCRIPTION: This routine close a stream connection set up by gn_open
 *      INPUTS: readq - read side queue pointer
 *     OUTPUTS: none
 *     RETURNS: 0
 *       CALLS: none
 **********************************************************************/
#ifdef LFS
int streamscall gn_close(readq, oflag, crp)
queue_t *readq;
int oflag;
cred_t *crp;
#else
int gn_close(readq)
queue_t *readq;
#endif
{
   CF_DEV   *dvp;
   
   dvp = (CF_DEV *)readq->q_ptr;

   dvp->id = 0;
   dvp->rq = (queue_t *) NULL;

#ifndef LINUX26
   MOD_DEC_USE_COUNT;
#endif 

   return (0);
}


/***********************************************************************
 *        NAME: gn_wput
 * DESCRIPTION: Driver write-put routine
 *      INPUTS: wq - write queue
 *              mp - input message block
 *     OUTPUTS: 
 *     RETURNS: 
 **********************************************************************/
int gn_wput(wq, mp)
queue_t *wq;
mblk_t  *mp;
{
   DLGCDECLARESPL(oldspl)

   struct iocblk  *iocp;

   switch(mp->b_datap->db_type) {
   case M_FLUSH:
      /* canonical flush handling */
      /* check to see if write queue should be flushed */
      if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
         DLGCSPLSTR(oldspl);     /* protect the critical region  */
         flushq(wq, FLUSHALL);
         DLGCSPLX(oldspl);       /* restore interrupts   */
      }
      
      /* check to see if read queue should be flushed */
      if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {
         DLGCSPLSTR(oldspl);        /* protect the critical region  */
         flushq(RD(wq), FLUSHALL);  /* flush this modules read queue */
         
         /*
          * we must insure that this message is sent to all modules on
          * the stream. because we are a driver we need only worry 
          * about the upstream bound (read queue) messages.
          * first we clear the write bit (so it isn't sent back
          * downstream by stream head) then send it upstream with
          * qreply()
          */
         *mp->b_rptr &= ~FLUSHW;    /* clear write queue flush */
         qreply(wq, mp);            /* send it on it's way */
         DLGCSPLX(oldspl);          /* restore interrupts   */

      } else {

         /*
          * we have now finished with the M_FLUSH message as we don't
          * have to send it upstream. ie no need to flush read queue.
          * So throw this message away and free up its allocated memory.
          */
         freemsg(mp);
      }

      break;
      
   case M_IOCTL:

      iocp = (struct iocblk *) mp->b_rptr;
      switch (iocp->ioc_cmd) {
      case CF_STARTGNDRV:
         cf_startgndrv(wq,mp);
         break;
    
      case CF_STOPGNDRV:
         cf_stopgndrv(wq,mp);
         break;

      case CF_STARTPM:
         cf_startpm(wq,mp);
         break;

      case CF_STOPPM:
         DLGCMUTEX_ENTER(&intdrv_lock);
         DLGCMUTEX_ENTER(&inthw_lock);
         cf_stoppm(wq,mp);
         DLGCMUTEX_EXIT(&inthw_lock);
         DLGCMUTEX_EXIT(&intdrv_lock);
         break;

      case CF_STARTBD:
         cf_startbd(wq,mp);
         break;

      case CF_STOPBD:
         DLGCMUTEX_ENTER(&intdrv_lock);
         DLGCMUTEX_ENTER(&inthw_lock);
         cf_stopbd(wq,mp);
         DLGCMUTEX_EXIT(&inthw_lock);
         DLGCMUTEX_EXIT(&intdrv_lock);
         break;

      case CF_GETNPMS:
         mp->b_datap->db_type = M_IOCACK;
         iocp->ioc_rval  = Gn_Maxpm;
         iocp->ioc_error = 0;
         iocp->ioc_count = 0;
         qreply(wq, mp);
         break;

      case CF_SETLOG:
         cf_setlog(wq,mp);
         break;

      case CF_STATUS:
         cf_status(wq,mp);
         break;

      case CF_TESTLOG:
         cf_testlog();
         break;

      case CF_DEBUG:
         cf_debug(wq,mp);
         break;

#ifdef _STATS_
      case CF_STATS:
         DLGCMUTEX_ENTER(&inthw_lock);
         cf_stats(wq,mp);
         DLGCMUTEX_EXIT(&inthw_lock);
         break;
#endif /* _STATS_ */

      default:
         mp->b_datap->db_type = M_IOCACK;
         iocp->ioc_rval = -1;
         iocp->ioc_error = EINVAL;
         qreply(wq, mp);
         break;

      }
      break;
       
   default:
      mp->b_datap->db_type = M_ERROR;
      mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
      *mp->b_rptr++ = EINVAL;
      qreply(wq, mp);
      break;
   }
   return (0);
}


/***********************************************************************
 *        NAME: cf_startgndrv
 * DESCRIPTION: Generic Driver startup routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_startgndrv(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   int            retval = 0;
   int            bcnt, pcp_cnt;
   int            pdc;
   unsigned int   pmid;
#ifdef LFS
   ULONG   bdsz;
   ULONG   ldsz;
#else
   unsigned int   bdsz;
   unsigned int   ldsz;
#endif
   unsigned int   gn_numbds = 0;
   unsigned int   gn_numlds = 0;
   struct iocblk  *iocp;
   GN_PMCNT       *pcp = NULL;
#ifdef LFS
   GN_PMDEVCNT    *pdp = NULL;
#else
   GN_PMDEVCNT    *pdp;
#endif
   GN_HEADER      *ghp;
   GN_BOARD       *gn_boardp;
   GN_LOGDEV      *gn_logdevp;
   char           *gn_freep;
   char           *pmname;


   /* Set up some required pointers */
   iocp = (struct iocblk *) mp->b_rptr;
   pcp_cnt = iocp->ioc_count;

   if ((pcp = (GN_PMCNT *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto startgndrv_exit;
   }

   /* Exit if driver has already been started */
   if (Gn_Headerp != 0) {
      retval = EGN_DRVRSTARTED;
      goto startgndrv_exit;
   }

   /* Record & check the PM count */
   if ((Gn_Numpms = pcp->gc_pmcnt) != Gn_Maxpm) {
      Gn_Numpms = 0;
      retval = EGN_BADPMCNT;
      goto startgndrv_exit;
   }

   /* Check the size of the ioctl data block */
   bcnt = sizeof(GN_PMCNT) + Gn_Numpms * sizeof(GN_PMDEVCNT);
   if (iocp->ioc_count != bcnt) {
      retval = EGN_BADMSGSIZE;
      goto startgndrv_exit;
   }

   /*
    * Loop through each PM and calculate the total number of bytes
    * required by all PM's
    */
   for (Gn_Memsz = 0, pmid = 0; pmid < Gn_Numpms; pmid++) {
      /*
       * Call the PM memsize routine to get the PM's name string as well
       * as the device-dependent BOARD and LOGDEV structure sizes.
       */
      (*pmswtbl[pmid].memsz)(&bdsz, &ldsz, &pmname);

      /* Find the GN_PMDEVCNT structure in the message for this PM. */
      for (pdc = 0; pdc < Gn_Maxpm; pdc++) {
         pdp = ((GN_PMDEVCNT *)(pcp + 1)) + pdc;
         if (cf_cmpstr(pdp->gd_pmname, pmname, PM_NAMELEN) == 0) {
            break;
         }
      }

      /* Error if didn't find the corresponding GN_PMDEVCNT structure */
      if (pdc >= Gn_Maxpm) {
         retval = EGN_BADNAME;
         goto startgndrv_exit;
      }

      /*
       * Force the device-dependent structure sizes up to the next 
       * quad-boundary alignment. This is to insure that all pointers
       * are always aligned on a 4-byte boundary.
       */
      QUAD_ALIGN(bdsz);
      QUAD_ALIGN(ldsz);

      /*
       * Add up the total #bytes required by the PM. This consists of
       * the PM header area plus the device-independent and device-
       * dependent BOARD and LOGDEV structures.
       */
      Gn_Memsz += sizeof(GN_HEADER);
      Gn_Memsz += ((sizeof(GN_BOARD)  + bdsz) * pdp->gd_npmbds);
      Gn_Memsz += ((sizeof(GN_LOGDEV) + ldsz) * pdp->gd_npmlds);

      /* Increment the total BOARD and LOGDEV counts. */
      gn_numbds += pdp->gd_npmbds;
      gn_numlds += pdp->gd_npmlds;

   }

   /*
    * ALLOCATE AND ZERO-FILL THE STRUCTURE SPACE 
   if ((Gn_Headerp = (GN_HEADER *)
    */
   if ((Gn_Headerp = (GN_HEADER *)(void *) lis_get_free_pages(GFP_KERNEL, Gn_Memsz)) == NULL) {
      retval = EGN_NOMEM;
      goto init_err;
   }
   
   bzero(Gn_Headerp,Gn_Memsz);

    
   /* Set up the global and local pointers */
   gn_boardp  = Gn_Boardp  = (GN_BOARD *) (Gn_Headerp + Gn_Numpms);
   gn_logdevp = Gn_Logdevp = (GN_LOGDEV *) (Gn_Boardp + gn_numbds);
   gn_freep   = Gn_Freep   = (char *) (Gn_Logdevp + gn_numlds);
   Gn_Endp = ((char *)Gn_Headerp) + Gn_Memsz;

   /* Initialize the GN_HEADER structures for each PM. */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {

      /* Point to the GN_HEADER structure for this PM */
      ghp = Gn_Headerp + pmid;

      /*
       * Call the PM memsize routine to get the PM's name string as well
       * as the device-dependent BOARD and LOGDEV structure sizes.
       */
      (*pmswtbl[pmid].memsz)(&(ghp->gh_bd_devdepsz), &(ghp->gh_ld_devdepsz), &pmname);

      /* Find the GN_PMDEVCNT structure in the message for this PM. */
      for (pdc = 0; pdc < Gn_Maxpm; pdc++) {
         pdp = ((GN_PMDEVCNT *)(pcp + 1)) + pdc;
         if (cf_cmpstr(pdp->gd_pmname, pmname, PM_NAMELEN) == 0) {
            break;
         }
      }

      /* Error if didn't find the corresponding GN_PMDEVCNT structure. */
      if (pdc >= Gn_Maxpm) {
         retval = EGN_BADNAME;
         goto init_err2;
      }

      /* Copy the PM name into the GN_HEADER structure for this PM */
      cf_copystr(pmname,ghp->gh_pmname,PM_NAMELEN);

      /*
       * Force the device-dependent structure sizes up to the next 
       * quad-boundary alignment.
       */
      QUAD_ALIGN(ghp->gh_bd_devdepsz);
      QUAD_ALIGN(ghp->gh_ld_devdepsz);

      /*
       * Record the maximum number of BOARD devices and LOGDEV
       * devices that will fit into this PM's areas.
       */
      ghp->gh_maxbds = pdp->gd_npmbds;
      ghp->gh_maxlds = pdp->gd_npmlds;

      /* Record the number of protocols supported by the PM */
      ghp->gh_nprotos = pdp->gd_nprotos;

      /*
       * Set up the base, end, and free pointers for the generic
       * BOARD structures used with this PM. The gn_boardbp is
       * incremented for the next pass thru this loop.
       */
      ghp->gh_gbfreep = ghp->gh_gbbasep = gn_boardp;
      ghp->gh_gbendp = (gn_boardp += ghp->gh_maxbds);
       
      /*
       * Set up the base, end, and free pointers for the generic
       * LOGDEV structures used with this PM. The gn_logdevp is
       * incremented for the next pass thru this loop.
       */
      ghp->gh_glfreep = ghp->gh_glbasep = gn_logdevp;
      ghp->gh_glendp = (gn_logdevp += ghp->gh_maxlds);

      /*
       * Set up the base, end, and free pointers for the device-
       * dependent BOARD structures used with this PM. The gn_freep is
       * incremented for use below.
       */
      ghp->gh_dbfreep = ghp->gh_dbbasep = gn_freep;
      ghp->gh_dbendp =
         (gn_freep += (ghp->gh_bd_devdepsz * ghp->gh_maxbds));
       
      /*
       * Set up the base, end, and free pointers for the device-
       * dependent LOGDEV structures used with this PM. The gn_freep is
       * incremented for the next pass thru this loop.
       */
      ghp->gh_dlfreep = ghp->gh_dlbasep = gn_freep;
      ghp->gh_dlendp =
         (gn_freep += (ghp->gh_ld_devdepsz * ghp->gh_maxlds));

      /* Mark this PM as having a successful structinit */
      ghp->gh_flags = PM_STRUCTINIT;

   }


   /* Sanity check our work -- pointers should match */
   if (Gn_Endp != gn_freep) {
      retval = EGN_INITERR;

init_err2:
      lis_free_pages((void *) Gn_Headerp);

init_err:
      Gn_Numpms   = 0;
      Gn_Memsz    = 0;
      Gn_Headerp  = NULL;
      Gn_Boardp   = NULL;
      Gn_Logdevp  = NULL;
      Gn_Freep    = NULL;
      Gn_Endp     = NULL;

      goto startgndrv_exit;
   }

   /* Call the Sync Module's drvstart routine */
   dlgn_drvstart();

startgndrv_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (pcp != NULL) {
      kfree(pcp);
#ifndef LINUX26
      if (!retval) MOD_INC_USE_COUNT;  /* Don't allow driver to be unloaded */
#endif
   }

   return;
}


/***********************************************************************
 *        NAME: cf_stopgndrv
 * DESCRIPTION: Generic Driver shutdowm routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_stopgndrv(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   GN_HEADER      *ghp;
   struct iocblk  *iocp = (struct iocblk *) mp->b_rptr;
   int            retval = 0;
   int            already_stopped = 0;

   /*
    * If the Generic Driver is already stopped, just return success.
    */
   if (Gn_Headerp == (GN_HEADER *)NULL) {
     already_stopped = 1;
      goto stopgndrv_exit;
   }

   /*
    * Verify that all PM's have been shut down. (This must have already
    * been done by calling the CF_STOPPM ioctl for each PM in the
    * system.) An initialized, halted PM will have only its
    * PM_STRUCTINIT flag set.
    */
   for (ghp = Gn_Headerp; ghp < (GN_HEADER *)Gn_Boardp; ghp++) {
      if (ghp->gh_flags != PM_STRUCTINIT) {
         retval = EGN_PMSTARTED;
         goto stopgndrv_exit;
      }
   }

   /*
    * FREE THE MEMORY SEGMENT(S) ALLOCATED AT STRUCTINIT TIME
    */
   lis_free_pages((void *) Gn_Headerp);


   /* Reset all the global variables */
   Gn_Numpms   = 0;
   Gn_Memsz    = 0;
   Gn_Headerp  = NULL;
   Gn_Boardp   = NULL;
   Gn_Logdevp  = NULL;
   Gn_Freep    = NULL;
   Gn_Endp     = NULL;

   /* Call the Sync Module's drvstop routine */
   dlgn_drvstop();

stopgndrv_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /*If successful stop, driver canbe unloaded*/ 
#ifndef LINUX26
   if (!retval && !already_stopped) MOD_DEC_USE_COUNT;
#endif

   return;
}    


/***********************************************************************
 *        NAME: cf_startpm
 * DESCRIPTION: Routine to start up a given PM.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: This routine requires the exact same number of board and
 *              logdev devices as were given to cf_startgndrv.
 *     CAVEATS: The above statement does not apply to VME_LIVE_INSERTION
 *              changes.
 **********************************************************************/
PRIVATE void cf_startpm(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   GN_STARTPM     *spp = NULL;
#ifdef LFS
   GN_HEADER      *ghp = NULL;
#else
   GN_HEADER      *ghp;
#endif
   GN_INITBD      *gibp;
   GN_BOARD       *nbdp;
   GN_INITBD      *basegibp;
#ifdef LFS
   GN_LOGDEV      *ldp = NULL;
#else
   GN_LOGDEV      *ldp;
#endif
   struct iocblk  *iocp;
   char           *tnamep;
   int            retval = 0;
   int            ldnum;
   int            bdnum;
   int            bcnt, spp_cnt;
   unsigned int  pmid;
   unsigned int  nlogdevs = 0;


   /* Set up some required pointers */
   iocp = (struct iocblk *) mp->b_rptr;
   spp_cnt = iocp->ioc_count;

   /* Check that the driver has been started. */
   if (Gn_Headerp == NULL) {
      retval = EGN_DRVRSTOPPED;
      goto startpm_exit;
   }

   /* Read in the message block */
   if ((spp  = (GN_STARTPM *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto startpm_exit;
   }

   basegibp = (GN_INITBD *) (spp + 1);

   /* Find the pmid for the given PM */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {
      ghp = Gn_Headerp + pmid;
      if (cf_cmpstr(ghp->gh_pmname, spp->gp_pmname, PM_NAMELEN) == 0) {
         break;
      }
   }

   /* Error if could not locate the given PM */
   if (pmid >= Gn_Numpms) {
      retval = EGN_BADNAME;
      goto startpm_exit;
   }

   /* Check that this PM has had the structinit done */
   if (!(ghp->gh_flags & PM_STRUCTINIT)) {
      retval = EGN_BADSTATE0;
      goto startpm_exit;
   }

   /* Exit with an error if this PM is already started */
   if (ghp->gh_flags & PM_PMSTART) {
      retval = EGN_PMSTARTED;
      goto startpm_exit;
   }

   /* Get the expected message data block size */
   bcnt = spp->gp_ngsz +
             (sizeof(GN_STARTPM) + (sizeof(GN_INITBD) * spp->gp_numbds));

   /* Verify the size of the ioctl message. */
   if (iocp->ioc_count != bcnt) {
      retval = EGN_BADMSGSIZE;
      goto startpm_exit;
   }

   /*
    * Check the number of available boards vs. the requested number
    * of boards; they must match.
    */
   if ((ghp->gh_maxbds - ghp->gh_ncurbds) != spp->gp_numbds) {
      retval = EGN_BADBDCNT;
      goto startpm_exit;
   }

   /*
    * Check the number of available logdevs vs. the requested number
    * of logdevs; they must match.
    */
   if ((ghp->gh_maxlds - ghp->gh_ncurlds) != spp->gp_numlds) {
      retval = EGN_BADLDCNT;
      goto startpm_exit;
   }

   /*
    * Make sure this PM has enough space for the additional generic
    * BOARD (GN_BOARD) structure(s).
    */
   if ((ghp->gh_gbendp - ghp->gh_gbfreep) != (int) spp->gp_numbds) {
      retval = EGN_NOGBSPACE;
      goto startpm_exit;
   }

   /*
    * Make sure this PM has enough space for the additional generic
    * LOGDEV (GN_LOGDEV) structure(s).
    */
   if ((ghp->gh_glendp - ghp->gh_glfreep) != (int) spp->gp_numlds) {
      retval = EGN_NOGLSPACE;
      goto startpm_exit;
   }

   /*
    * Make sure this PM has enough space for the additional device-
    * dependent BOARD structure(s).
    */
   if (((ghp->gh_dbendp - ghp->gh_dbfreep) / (int) ghp->gh_bd_devdepsz)
      != (int) spp->gp_numbds) {
      retval = EGN_NODBSPACE;
      goto startpm_exit;
   }

   /*
    * Make sure this PM has enough space for the additional device-
    * dependent LOGDEV structure(s).
    */
   if (((ghp->gh_dlendp - ghp->gh_dlfreep) / (int) ghp->gh_ld_devdepsz)
      != (int) spp->gp_numlds) {
      retval = EGN_NODLSPACE;
      goto startpm_exit;
   }

   /*
    * Check each GN_INITBD structure for a valid IRQ level. Also add
    * up the number of logdevs specified in each GN_INITBD for
    * validation below.
    */
   for (gibp = basegibp + spp->gp_numbds - 1; gibp >= basegibp; gibp--) {
      if (gibp->gib_irq >= GN_MAXIRQ) {
         retval = EGN_BADIRQ;
         goto startpm_exit;
      }
      nlogdevs += gibp->gib_nlogdevs;
   }

   /* Validate the count of logdev devices */
   if (nlogdevs != spp->gp_numlds) {
      retval = EGN_BADLDCNT2;
      goto startpm_exit;
   }

   /* Exit with error if a duplicate board name is detected */
   if ((tnamep = cf_ckbdname(basegibp,spp->gp_numbds)) != NULL) {
      retval = EGN_DUPNAME;
      goto startpm_exit;
   }

   /*
    * For each board on this PM, initialize the generic board structure
    * and the generic logdev structures.
    */
   for (bdnum = 0; bdnum < spp->gp_numbds; bdnum++) {

      /* Get a pointer to the given GN_INITBD structure */
      gibp = basegibp + bdnum;

      /*
       * Set up the GN_BOARD structure. The structure is first cleared
       * (zeroed) and then all members are filled in as appropriate. The
       * device-dependent area is also cleared.
       */
      nbdp = ghp->gh_gbfreep;
      bzero(nbdp,sizeof(GN_BOARD));
      cf_copystr(gibp->gib_name, nbdp->bd_name, GB_NAMELEN);
      cf_copystr(gibp->gib_sub,  nbdp->bd_sub,  GB_SUBLEN);
      nbdp->bd_boardid    = (unsigned short) bdnum;
      nbdp->bd_nlogdevs   = gibp->gib_nlogdevs;
      nbdp->bd_pmid       = (unsigned short) pmid;
      nbdp->bd_nprotos    = ghp->gh_nprotos;
      nbdp->bd_irq        = gibp->gib_irq;
      nbdp->bd_nxtirqp    = (GN_BOARD *) NULL;
      nbdp->bd_ldp        = ghp->gh_glfreep;
      nbdp->bd_devdepp    = ghp->gh_dbfreep;
      nbdp->bd_devdepsz   = ghp->gh_bd_devdepsz;

      /*
       * Clear the device-dependent BOARD area. This will be filled in
       * later by the PM's startpm routine.
       */
      bzero(nbdp->bd_devdepp, nbdp->bd_devdepsz);

      /*
       * Increment the free pointers to the generic and device-dependent
       * areas on this PM's GN_HEADER.
       */
      ghp->gh_gbfreep++;
      ghp->gh_dbfreep += ghp->gh_bd_devdepsz;

      /*
       * Set up the LOGDEV structures for this BOARD. For each LOGDEV,
       * clear the GN_LOGDEV structure, fill in all members as 
       * appropriate, and clear the device-dependent LOGDEV area.
       */
      for (ldnum = 0; ldnum < (int) nbdp->bd_nlogdevs; ldnum++) {

         ldp = ghp->gh_glfreep;   /* Get ptr to GN_LOGDEV area to use */
         ghp->gh_glfreep++;       /* Point to next free GN_LOGDEV area */

         /* Zero out this GN_LOGDEV */
         bzero(ldp,sizeof(GN_LOGDEV));

         /*
          * Create the logical device name. If this is logical device:0 on
          * this board, then the logdev name is the same as the board name;
          * otherwise the logdev name is the board name with the subname
          * and number appended.
          */
         if (ldnum == 0) {
            cf_copystr(nbdp->bd_name, ldp->ld_name, LD_NAMELEN);
         } else {
            cf_mklogname(ldp,nbdp,ldnum);
         }

         /* Fill in the rest of the GN_LOGDEV structure for this LOGDEV */
         ldp->ld_devnum    = (unsigned short) ldnum;
         ldp->ld_gbp       = nbdp;
         ldp->ld_nldp      = ghp->gh_glfreep;
         ldp->ld_devdepp   = ghp->gh_dlfreep;
         ldp->ld_devdepsz  = ghp->gh_ld_devdepsz;

         /*
          * Clear the device-dependent LOGDEV area. This will be filled in
          * later by the PM's startpm routine.
          */
         bzero(ldp->ld_devdepp, ldp->ld_devdepsz);

         /* Point to next free device-dependent LOGDEV area */
         ghp->gh_dlfreep += ghp->gh_ld_devdepsz;
      }

      /* Set the ld_nldp in the last GN_LOGDEV for this board to NULL */
      ldp->ld_nldp = NULL;
   }

   /*
    * Increment the counts of the current number of BOARDS and LOGDEVS
    * on this PM.
    */
   ghp->gh_ncurbds += spp->gp_numbds;
   ghp->gh_ncurlds += spp->gp_numlds;
   /*
    * Call the PM's start routine with the data on this board.
    */

   if ((*pmswtbl[pmid].startpm)
         (ghp->gh_gbbasep, (char *) (basegibp + ghp->gh_ncurbds),
          ghp->gh_ncurbds, ghp->gh_ncurlds, spp->gp_ngsz) != 0) {
      /* 
       * If the PM's start routine fails, then clean up this PM's
       * GN_HEADER area before exiting.
       */
      cf_clearpm(ghp, CF_NOSHUT);
      retval = EGN_PMSTARTFAIL;
      goto startpm_exit;
   }

   /* Mark this PM as started */
   ghp->gh_flags |= PM_PMSTART;
   
startpm_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (spp != NULL) {
      kfree(spp);
   }

   return;
}


/***********************************************************************
 *        NAME: cf_stoppm
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_stoppm(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   GN_STARTPM     *spp = NULL;
#ifdef LFS
   GN_HEADER      *ghp = NULL;
#else
   GN_HEADER      *ghp;
#endif
   struct iocblk  *iocp;
   int            retval = 0;
   int            flag, spp_cnt;
   unsigned int  pmid;


   /* Set up some required pointers */
   iocp = (struct iocblk *) mp->b_rptr;
   spp_cnt = iocp->ioc_count;
   /* Error if driver was not started */
   if (Gn_Headerp == (GN_HEADER *)NULL) {
      retval = EGN_DRVRSTOPPED;
      goto stoppm_exit;
   }

   /* Read in the message block */
   if ((spp = (GN_STARTPM *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto stoppm_exit;
   }

   /* Verify the size of the ioctl message. */
   if (iocp->ioc_count != sizeof(GN_STARTPM)) {
      retval = EGN_BADMSGSIZE;
      goto stoppm_exit;
   }

   /* Find the pmid for the given PM */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {
      ghp = Gn_Headerp + pmid;
      if (cf_cmpstr(ghp->gh_pmname, spp->gp_pmname, PM_NAMELEN) == 0) {
         break;
      }
   }

   /* Error if could not locate the given PM */
   if (pmid >= Gn_Numpms) {
      retval = EGN_BADNAME;
      goto stoppm_exit;
   }

   /*
    * If the PM has already been stopped, tell cf_clearpm to NOT clear
    * the BOARD and LOGDEV structures.
    */
   flag = (ghp->gh_flags & PM_PMSTART) ? CF_DOSHUT : (CF_DOSHUT | CF_NOCLEAR);

   /*
    * Call cf_clearpm() to do all the dirty work of shutting down
    * this PM & clearing its structures.
    */
   if ((retval = cf_clearpm(ghp, flag)) != 0) {
      /* cf_clearpm prints its own error messages */
      goto stoppm_exit;
   }

stoppm_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (spp != NULL) {
      kfree(spp);
   }

   return;
}    


/***********************************************************************
 *        NAME: cf_startbd
 * DESCRIPTION: This function is called to start an individual board.
 *              The PM's startbd routine is invoked perform the actual
 *              board start.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_startbd(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   struct iocblk  *iocp;
   GN_STARTBD     *sbp = NULL;
   GN_HEADER      *ghp;
   GN_BOARD       *bdp;
   int            retval = 0;
   int            sbp_cnt;
#ifndef LFS
   unsigned int  bdid;
#endif

   iocp = (struct iocblk *) mp->b_rptr;
   sbp_cnt = iocp->ioc_count;

   /* Exit with an error if the driver is not started */
   if (Gn_Headerp == NULL) {
      retval = EGN_DRVRSTOPPED;
      goto startbd_exit;
   }

   /* Read in the message block */
   if ((sbp = (GN_STARTBD *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto startbd_exit;
   }

   /* Verify the size of the ioctl message */
   if (iocp->ioc_count != sizeof(GN_STARTBD)) {
      retval = EGN_BADMSGSIZE;
      goto startbd_exit;
   }

   /* Locate the requested BOARD */
   for (bdp = Gn_Boardp; bdp < (GN_BOARD *) Gn_Logdevp; bdp++) {
      if (cf_cmpstr(bdp->bd_name, sbp->gb_bdname, GB_NAMELEN) == 0) {
         break;
      }
   }
 
   /* Error if requested board could not be found */
   if (bdp >= (GN_BOARD *) Gn_Logdevp) {
      retval = EGN_BADNAME;
      goto startbd_exit;
   }
 
   /* EFN: This is probably a good place to validate the PMID */
 
   /* Get a pointer to the GN_HEADER structure for this PM */
   ghp = Gn_Headerp + bdp->bd_pmid;

   /* Exit with error if this boards PM is not started */
   if ((ghp->gh_flags & PM_PMSTART) == 0) {
      retval = EGN_PMSTOPPED;
      goto startbd_exit;
   }
 
   /* Exit with error if this board is already started */
   if (bdp->bd_flags & BD_START) {
      retval = EGN_BDSTARTED;
      goto startbd_exit;
   }
 
   /* Call the PM's startbd routine for this board */
   if ((*pmswtbl[bdp->bd_pmid].startbd)(bdp) != 0) {
      /* dlgn_rmvirq(bdp); */
      bdp->bd_flags = 0;
      retval = EGN_BDSTARTFAIL;
      goto startbd_exit;
   }
 
   /* Mark the board as started */
   bdp->bd_flags = BD_START;
 
startbd_exit:
 
   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);
 
   /* Must free any memory from cf_ioccopy! */
   if (sbp != NULL) {
      kfree(sbp);
   }
 
   return;
}    
 
 
/***********************************************************************
 *        NAME: cf_stopbd
 * DESCRIPTION: This function is called to stop an individual board.
 *              The PM's stopbd routine is invoked perform the actual
 *              board shutdown.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_stopbd(wq,mp)
queue_t *wq;
mblk_t  *mp;
{
   struct iocblk  *iocp;
   GN_STARTBD     *sbp = NULL;
   GN_HEADER      *ghp;
   GN_BOARD       *bdp;
   int            retval = 0;
   int            sbp_cnt;
#ifndef LFS
   unsigned int  bdid;
#endif
 
 
   /* Set up some required pointers */
   iocp = (struct iocblk *) mp->b_rptr;
   sbp_cnt = iocp->ioc_count;
 
   /* Exit with an error if the driver is not started */
   if (Gn_Headerp == NULL) {
      retval = EGN_DRVRSTOPPED;
      goto stopbd_exit;
   }
 
   /* Read in the message block */
   if ((sbp = (GN_STARTBD *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto stopbd_exit;
   }
 
   /* Verify the size of the ioctl message */
   if (iocp->ioc_count != sizeof(GN_STARTBD)) {
      retval = EGN_BADMSGSIZE;
      goto stopbd_exit;
   }
 
   /* Locate the requested BOARD */
   for (bdp = Gn_Boardp; bdp < (GN_BOARD *) Gn_Logdevp; bdp++) {
      if (cf_cmpstr(bdp->bd_name, sbp->gb_bdname, GB_NAMELEN) == 0) {
         break;
      }
   }

   /* Error if requested board could not be found */
   if (bdp >= (GN_BOARD *) Gn_Logdevp) {
      retval = EGN_BADNAME;
      goto stopbd_exit;
   }

   /* Get a pointer to the GN_HEADER structure for this PM */
   ghp = Gn_Headerp + bdp->bd_pmid;

   /* Exit with error if this PM is not started */
   if ((ghp->gh_flags & PM_PMSTART)  == 0) {
      retval = EGN_PMSTOPPED;
      goto stopbd_exit;
   }

   /* If board is already stopped, just return success. */
   if ((bdp->bd_flags & BD_START) == 0) {
      goto stopbd_exit;
   }

   /* Call the PM's stopbd routine for this board */
   switch ((*pmswtbl[bdp->bd_pmid].stopbd)(bdp)) {
   case 0:
      break;
   case -2:
      retval = EGN_BDBUSY;
      goto stopbd_exit;
      break;
   default:
      retval = EGN_BDSTOPFAIL;
      break;
   }

   /* Remove the board from the linked list of BOARDS on this IRQ */
   /* dlgn_rmvirq(bdp); */

   /* Clear the board start flag */
   bdp->bd_flags &= ~BD_START;

stopbd_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (sbp != NULL) {
      kfree(sbp);
   }

   return;
}    


/***********************************************************************
 *        NAME: cf_clearpm
 * DESCRIPTION: This function will call the PM's stoppm routine and
 *              restore the GN_HEADER structure for the PM to the same
 *              state it was in immediately after CF_STARTGNDRV
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE int cf_clearpm(ghp, flag)
GN_HEADER   *ghp;
int         flag;
{
   GN_BOARD       *bdp;
   GN_LOGDEV      *ldp;
   GN_LOGDEV      *nldp;
   unsigned int  pmid;
   int            retval = 0;


   /* Verify that the GN_HEADER pointer is legit */
   if ((pmid = (unsigned int) (ghp - Gn_Headerp)) >= Gn_Numpms) {
      return (EGN_BADPMID);
   }

   /* Check that this PM has had the structinit done */
   if (!(ghp->gh_flags & PM_STRUCTINIT)) {
      return (EGN_BADSTATE1);
   }

   /* Make sure that no boards on this PM are marked as started */
   for (bdp = ghp->gh_gbbasep; bdp < ghp->gh_gbendp; bdp++) {
      if (bdp->bd_flags & BD_START) {
         return (EGN_BDSTARTED);
      }
   }

   /*
    * If this PM is marked as having been started, then call the
    * PM's stoppm routine to halt the PM.
    */
   if ((ghp->gh_flags & PM_PMSTART) && (flag & CF_DOSHUT)) {
      switch((*pmswtbl[pmid].stoppm)(ghp->gh_gbbasep, ghp->gh_ncurbds)) {
      case 0:   /* Success */
         break;
      case -2:  /* PM busy */
         return (EGN_PMBUSY);
         break;
      default:
         retval = EGN_PMSTARTFAIL;
         break;
      }
   }
   
   /*
    * For each board on this PM, clear the generic and device-
    * dependent board and logdev areas if we were told to.
    */
   if (!(flag & CF_NOCLEAR)) {
      for (bdp = ghp->gh_gbbasep; bdp < ghp->gh_gbendp; bdp++) {
         ldp = bdp->bd_ldp;
         do {
            nldp = ldp->ld_nldp; 
            bzero(ldp->ld_devdepp,ldp->ld_devdepsz);
            bzero(ldp,sizeof(GN_LOGDEV));
         } while ((ldp = nldp) != (GN_LOGDEV *) NULL);
         bzero(bdp->bd_devdepp,bdp->bd_devdepsz);
         bzero(bdp,sizeof(GN_BOARD));
      }
   }

   /* Reset all the free pointers on this PM's GN_HEADER */
   ghp->gh_gbfreep = ghp->gh_gbbasep;
   ghp->gh_dbfreep = ghp->gh_dbbasep;
   ghp->gh_glfreep = ghp->gh_glbasep;
   ghp->gh_dlfreep = ghp->gh_dlbasep;

   /* Reset the count of BOARD and LOGDEV devices */
   ghp->gh_ncurbds = 0;
   ghp->gh_ncurlds = 0;

   /* Mark the GN_HEADER as having a successful structinit */
   ghp->gh_flags = PM_STRUCTINIT;

   return retval;
}    


/***********************************************************************
 *        NAME: cf_setlog
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_setlog(wq,mp)
queue_t  *wq;
mblk_t   *mp;
{
   struct iocblk  *iocp;
   GN_LOGDATA     *glp = NULL;
   int            glp_cnt;
   int            retval = 0;


   iocp = (struct iocblk *) mp->b_rptr;
   glp_cnt = iocp->ioc_count;

   /* Read in the message block */
   if ((glp = (GN_LOGDATA *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto setlog_exit;
   }

   /* Verify the size of the ioctl message */
   if (iocp->ioc_count != sizeof(GN_LOGDATA)) {
      retval = EGN_BADMSGSIZE;
      goto setlog_exit;
   }

   /* Remove any invalid bits */
   glp->gl_mode &= (LF_CONT | LF_NOTE | LF_WARN | LF_PANIC | LF_DEBUG |
      LF_HASBUG | LM_ENABLE | LM_DISABLE);

   /* Set the given message level */
   if (glp->gl_mode & LM_DISABLE) {
      Gn_Msglvl &= ~glp->gl_mode;   /* Clear the selected bits */
   } else {
      Gn_Msglvl |= glp->gl_mode;    /* Set the selected bits */
   }

setlog_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = 0;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (glp != NULL) {
      kfree((char*)glp);
   }

   return;

}


/***********************************************************************
 *        NAME: cf_status
 * DESCRIPTION: Retrieves the current status of the driver, all PM's,
 *              and all boards in the system and returns it to the user.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_status(wq, mp)
queue_t  *wq;
mblk_t   *mp;
{
   GN_DRVSTATUS   *gdrvsp;
   GN_PMSTATUS    *gpmsp;
   GN_BDSTATUS    *gbdsp;
   GN_HEADER      *ghp;
   GN_BOARD       *gnbdp;
   struct iocblk  *iocp;
   mblk_t         *nmp;
   char           *pmnamep;
   unsigned int  numpms;
   unsigned int  numbds = 0;
   unsigned int  memsz;
   unsigned int  pmid;
   unsigned int  bdid;
   int            copysz = 0;
   int            retval = 0;


   iocp = (struct iocblk *) mp->b_rptr;

   numpms = Gn_Maxpm;
   numbds = ((GN_BOARD *)Gn_Logdevp) - Gn_Boardp;

   memsz = sizeof(GN_DRVSTATUS) +
           numpms * sizeof(GN_PMSTATUS) +
           numbds * sizeof(GN_BDSTATUS);

   if ((nmp = allocb(memsz,BPRI_HI)) == NULL) {
      retval = EGN_NOMEM;
      goto status_exit;
   }

   /* Set up the data block pointers */
   gdrvsp = (GN_DRVSTATUS *)nmp->b_rptr;
   gpmsp  = (GN_PMSTATUS *)(gdrvsp + 1);
   gbdsp  = (GN_BDSTATUS *)(gpmsp + numpms);
   bzero(gdrvsp,memsz);

   /* Record the driver status info */
   gdrvsp->gds_numpms = numpms;
   gdrvsp->gds_numbds = numbds;
   gdrvsp->gds_status = (Gn_Headerp == NULL) ? 0 : GN_STARTED;
   copysz += sizeof(GN_DRVSTATUS);

   /*
    * Go thru each PM on the system and record its statistics. If the
    * PM is started, then also record the statistics on each board on
    * the PM.
    */
   for (pmid = 0; pmid < numpms; pmid++, gpmsp++) {

      /* Call PM's memsz fn to get name & dev-dep sizes */
      (*pmswtbl[pmid].memsz)(&gpmsp->gps_bdsz,&gpmsp->gps_ldsz,&pmnamep);

      /* Record the PM's name */
      cf_copystr(pmnamep, gpmsp->gps_pmname, PM_NAMELEN);
      copysz += sizeof(GN_PMSTATUS);

      /* Skip remaining stats on this PM if driver not started */
      if (Gn_Headerp == NULL) {
         continue;
      }

      /* Get the regular PM stats */
      ghp = Gn_Headerp + pmid;
      gpmsp->gps_status = (ghp->gh_flags & PM_PMSTART) ? GN_STARTED : 0;
      gpmsp->gps_numbds = ghp->gh_maxbds;
      gpmsp->gps_numlds = ghp->gh_maxlds;

      /* Skip board stats if the PM is not started */
      if ((ghp->gh_flags & PM_PMSTART) == 0) {
         continue;
      }

      /* Record stats on each board on the PM */
      for (bdid = 0; bdid < gpmsp->gps_numbds; bdid++, gbdsp++) {

         /* Get the board info */
         gnbdp = ghp->gh_gbbasep;
         cf_copystr(gnbdp->bd_name, gbdsp->gbs_bdname, GB_NAMELEN);
         gbdsp->gbs_status = (gnbdp->bd_flags & BD_START) ? GN_STARTED : 0;
         gbdsp->gbs_nlogdevs = gnbdp->bd_nlogdevs;
         gbdsp->gbs_irq = gnbdp->bd_irq;
         copysz += sizeof(GN_BDSTATUS);
      }
   }

   /* Attach the new data block to the ioctl message */
   if (mp->b_cont) {
      freemsg(mp->b_cont);
   }
   mp->b_cont = nmp;

status_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = copysz;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   return;
}


#ifdef _STATS_
/***********************************************************************
 *        NAME: cf_stats
 * DESCRIPTION: statistics counter for SNMP
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *    CAUTIONS:
 **********************************************************************/
PRIVATE void cf_stats(wq,mp)
queue_t  *wq;
mblk_t   *mp;
{
   struct iocblk  *iocp;
   PM_STATS       *gdbp;
   GN_HEADER      *ghp;
   int            pmid;
   int            retval = 0;


   iocp = (struct iocblk *) mp->b_rptr;

   /* Read in the message block */
   if ((gdbp = (PM_STATS *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto debug_exit; 
   }

   /* Verify the size of the ioctl message */
   if (iocp->ioc_count !=  sizeof(PM_STATS)) {
      retval = EGN_BADMSGSIZE;
      goto debug_exit;
   }

   /* Find the pmid for the given PM */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {
      ghp = Gn_Headerp + pmid;
      if (cf_cmpstr(ghp->gh_pmname, gdbp->pmname, PM_NAMELEN) == 0) {
         break;
      }
   }

   /* Error if could not locate the given PM */
   if (pmid >= Gn_Numpms) {
      retval = EGN_BADNAME;
      goto debug_exit;
   }

   /* Pass the message to the statistics routine of the selected PM */
   retval = (*pmswtbl[pmid].stats)(gdbp);

   /* Copy the message back into ioctl form */
#ifdef LFS
   cf_iocput(mp, (void *)gdbp, sizeof(PM_STATS));
#else
   cf_iocput(mp, gdbp, sizeof(PM_STATS));
#endif

debug_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = sizeof(PM_STATS);
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (gdbp != NULL) {
   }
   return;
}
#endif /* _STATS_ */


/***********************************************************************
 *        NAME: cf_debug
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_debug(wq,mp)
queue_t  *wq;
mblk_t   *mp;
{
   struct iocblk  *iocp;
   GN_DEBUG       *gdbp;
   GN_HEADER      *ghp;
   int            pmid;
   int            lmsgsz, msgsz;
   int            retval = 0;


   iocp = (struct iocblk *) mp->b_rptr;

   /* Read in the message block */
   if ((gdbp = (GN_DEBUG *) cf_ioccopy(mp)) == NULL) {
      retval = EGN_NOMEM;
      goto debug_exit;
   }

   /* Verify the size of the ioctl message */
   if ((lmsgsz = iocp->ioc_count) != (msgsz = gdbp->gd_msgsz)) {
      retval = EGN_BADMSGSIZE;
      goto debug_exit;
   }

   /* Find the pmid for the given PM */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {
      ghp = Gn_Headerp + pmid;
      if (cf_cmpstr(ghp->gh_pmname, gdbp->gd_pmname, PM_NAMELEN) == 0) {
         break;
      }
   }

   /* Error if could not locate the given PM */
   if (pmid >= Gn_Numpms) {
      retval = EGN_BADNAME;
      goto debug_exit;
   }

   /* Pass the message to the debug routine of the selected PM */
   retval = (*pmswtbl[pmid].debug)(gdbp, &msgsz);

   /* Copy the message back into ioctl form */
   cf_iocput(mp, gdbp, msgsz);

debug_exit:

   /* Done, send ACK reply back upstream */
   mp->b_datap->db_type = M_IOCACK;
   iocp->ioc_count = msgsz;
   iocp->ioc_rval  = retval;
   iocp->ioc_error = 0;
   qreply(wq, mp);

   /* Must free any memory from cf_ioccopy! */
   if (gdbp != NULL) {
      kfree((char*)gdbp);
   }

   return;
}


/***********************************************************************
 *        NAME: cf_mklogname
 * DESCRIPTION: Builds a LOGDEV name and stores it in the ld_name area
 *              of the given ldp. The LOGDEV name created by this
 *              routine consists of the BOARD name with the subdev name
 *              and subdev number appended to it.
 *      INPUTS: ldp - ptr to GN_LOGDEV stucture to receive the name.
 *              bdp - ptr to the GN_BOARD structure assiciated with the
 *                    given GN_LOGDEV structure.
 *              ldnum - logical device number for this logdev -- should
 *                    correspond to the ldp's ld_devnum value.
 *     OUTPUTS: logdev name into ldp's ld_name area.
 *     RETURNS: none
 *    CAUTIONS: This funtion checks the number of characters copied and
 *              will truncate the name if it is too long for the buffer.
 **********************************************************************/
PRIVATE void cf_mklogname(ldp,bdp,ldnum)
GN_LOGDEV      *ldp;
GN_BOARD       *bdp;
unsigned long  ldnum;
{
   register unsigned char  *sp = bdp->bd_name;
   register unsigned char  *dp = ldp->ld_name;
   register unsigned char  *ep = dp + LD_NAMELEN - 1;

   /* Copy the bd_name into ld_name */
   while (*sp && (dp < ep)) {
      *dp++ = *sp++;
   }

   /* Quit if we've reached the end of the buffer */
   if (dp == ep) {
      goto end_name;
   }

   /* Copy the bd_sub into ld_name */
   sp = bdp->bd_sub;
   while (*sp && (dp < ep)) {
      *dp++ = *sp++;
   }

   /* Quit if we've reached the end of the buffer */
   if (dp == ep) {
      goto end_name;
   }

   /* Convert the logdev number into ASCII & copy it into the buffer */
   sp = ep;
   do {
      *sp-- = (unsigned char) ((ldnum % 10) + '0');
      ldnum /= 10;
   } while ((ldnum != 0) && (sp > dp));

   sp++;
   do {
      *dp++ = *sp++;
   } while (sp <= ep);

end_name:

   /* Null-terminate the string */
   *dp = (unsigned char) 0;

   return;
}


/***********************************************************************
 *        NAME: cf_ckbdname
 * DESCRIPTION: Check every board name on the given list for uniqueness
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 **********************************************************************/
PRIVATE char *cf_ckbdname(gibp,numbds)
GN_INITBD      *gibp;
unsigned long  numbds;
{
   GN_INITBD      *ibp;
   GN_INITBD      *eibp = gibp + numbds;
   GN_INITBD      *nibp;
   GN_BOARD       *bdp;
   GN_HEADER      *ghp;
   unsigned int  pmid;


   /* First check for duplicate names on the given list */
   for (ibp = gibp; ibp < eibp; ibp++) {
      for (nibp = ibp + 1; nibp < eibp; nibp++) {
         if (cf_cmpstr(ibp->gib_name,nibp->gib_name,GB_NAMELEN) == 0) {
            return ((char *)ibp->gib_name);
         }
      }
   }

   /* Check eack name on the list vs. the names on each started PM */
   for (ibp = gibp; ibp < eibp; ibp++) {
      for (pmid = 0; pmid < Gn_Numpms; pmid++) {
         ghp = Gn_Headerp + pmid;
         if (!(ghp->gh_flags & PM_PMSTART)) {
            continue;
         }
         for (bdp = ghp->gh_gbbasep; bdp < ghp->gh_gbfreep; bdp++) {
            if (cf_cmpstr(ibp->gib_name,bdp->bd_name,GB_NAMELEN) == 0) {
               return ((char *)ibp->gib_name);
            }
         }
      }
   }

   return ((char *) NULL);
}


/***********************************************************************
 *        NAME: cf_copystr
 * DESCRIPTION: Copies up to bufsz - 1 bytes and adds a null terminator
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void cf_copystr(sp,dp,bufsz)
register unsigned char *sp;
register unsigned char *dp;
unsigned long          bufsz;
{
   register unsigned char *ep = dp + bufsz - 1;

   while (*sp && (dp < ep)) {
      *dp++ = *sp++;
   }

   *dp = (unsigned char) 0;

   return;
}


PRIVATE void cf_testlog()
{
   cmn_err(CE_CONT,"Testing console message level\n");
   DLGC_MSG2(CE_CONT,"This is a CE_CONT message\n");
   DLGC_MSG2(CE_NOTE,"This is a CE_NOTE message");
   DLGC_MSG2(CE_WARN,"This is a CE_WARN message");
   DLGC_MSG2(CE_CONT,"This would have been a CE_PANIC message\n");
   DLGC_DEBUG_MSG2(CE_DEBUG,"This is a CE_DEBUG message\n");
   cmn_err(CE_CONT,"Console message level test completed\n");
}
