/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : scttimer.c
 * Description                  : Pushable Timer module
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
#include "dlgcos.h"
#include "sctextern.h"

#ifndef SE_NOSLP
#define SE_NOSLP 1
#endif

#define	MODULE_NAME	"scttimer.c"

#include "diagkern.h"
#include "gndefs.h"
#include "gnmsg.h"
#include "gndrv.h"
#include "gndlgn.h"
#include "gnlibdrv.h"
#include "typedefs.h"
#include "sctqueue.h"
#include "scttimer.h"
#include "timerlibmod.h"
#include "drvdebug.h"

/* FOR DEBUG PURPOSES ONLY */
int	time_resolution = 1;

static void sct_print_queue(TIMER_INFO *);

/* DEFINES */
#define TIMER_ID	301	/*  Driver ID */
#define	TIMER_CNT	1024
#if LINUX_VERSION_CODE > 0x020600
#define LINUX26
#endif

/* Module Global Variables */
long	  sct_count = 0;
SCT_INFO  sct_info[ GN_MAXHANDLES ] = { {0} };	/* Could have this many open's */
#ifdef LFS
extern spinlock_t st_sctlock;
extern spinlock_t st_uselock;
#else
extern lis_spin_lock_t st_sctlock;
extern lis_spin_lock_t st_uselock;
#endif

/*
 * The function timeout() can return an integer as an id,
 * and in unusual circumstances return a 0 as well. Hence
 * we use a flag to determine if we have started a timeout().
 * The id returned by timeout() will be kept in  sct_tmoid,
 * and  sct_tmoflag  will be used to indicate that timeout()
 * has been called.
 */
long		sct_tmoid = 0;
unsigned long	sct_tmoflag = 0;

/*
 * Keep a copy of a message block ready to use whenever it is
 * required. The initial copy is allocated when the first open
 * is done and from then onwards an allocated copy is always
 * there!!!!!!
 */
mblk_t	*sct_freemp = NULL;	/* Pointer to a free message block for use */

/* PRIVATE FUNCTION DECLARATIONS */
#ifdef LFS
int streamscall sctmropen(queue_t *, dev_t *, int, int, cred_t *);	/* Driver Open routine. */
int streamscall sctmrclose(queue_t *, int, cred_t *);	/* Driver Close routine. */
int streamscall sctmrrput(queue_t *, mblk_t *);	/* Read Side Put routine. */
int streamscall sctmrrsrv(queue_t *);	/* Read Side Service routine. */
int streamscall sctmrwput(queue_t *, mblk_t *);	/* Write Side Put routine. */
int streamscall sctmrwsrv(queue_t *);	/* Write Side Service routine */
#else
int sctmropen();	/* Driver Open routine. */
int sctmrclose();	/* Driver Close routine. */
int sctmrrput();	/* Read Side Put routine. */
int sctmrrsrv();	/* Read Side Service routine. */
int sctmrwput();	/* Write Side Put routine. */
int sctmrwsrv();	/* Write Side Service routine */
#endif

/*
 * Set the low/high water marks to allow only one message to be queued
 * for the write-side service routine at a time.
 */
#define LO_WATER        1
#define HI_WATER        64 * 1024
#define MIN_PSZ         0
#define MAX_PSZ         64 * 1024

/* Read Side Module Information */
struct module_info sctmrrminfo = {
   TIMER_ID,	/* Module ID. Used for logging */
   "sctmr",	/* Module name. Same as that used in mdevice */
   0,		/* Minimum packet size. */
   0,		/* Maximum packet size. */
   0,		/* High water mark. Used in flow control. */
   0		/* Low water mark. Ditto. */
};

/* Write Side Module Information */
struct module_info sctmrwminfo = {
   TIMER_ID,	/* Module ID. Used for logging */
   "sctmr",	/* Module name. Same as that used in mdevice */
   MIN_PSZ,	/* Minimum packet size. */
   MAX_PSZ,	/* Maximum packet size. */
   HI_WATER,	/* High water mark. Used in flow control. */
   LO_WATER	/* Low water mark. Ditto. */
};

/* Read Side Queue Information */
struct qinit sctmrrinit = {
   sctmrrput,		/* Pointer to Put routine. */
   sctmrrsrv,		/* Pointer to Service routine. */
   sctmropen,		/* Pointer to Open routine. */
   sctmrclose,		/* Pointer to Close routine. */
   NULL,		/* Reserved by STREAMS for future use. */
   &sctmrrminfo,	/* Pointer to module_info structure. */
   NULL			/* Pointer to optional statistics structure. */
};

/* Write Side Queue Information */
struct qinit sctmrwinit = {
   sctmrwput,		/* Pointer to Put routine. */
   sctmrwsrv,		/* Pointer to Service routine. */
   NULL,		/* Pointer to Open routine. */
   NULL,		/* Pointer to Close routine. */
   NULL,		/* Reserved by STREAMS for future use. */
   &sctmrwminfo,	/* Pointer to module_info structure. */
   NULL			/* Pointer to optional statistics structure. */
};


/*
 * Streamtab entry - Used in cdevsw and fmodsw to point to the driver or
 * module. This is typically the only public structure in a STREAMS
 * driver.
 */
static struct streamtab sctmrinfo = {
   &sctmrrinit,		/* Pointer to Read Queue Init. Structure. */
   &sctmrwinit,		/* Pointer to Write Queue Init. Structure.*/
   NULL,		/* Multiplexer Read Queue Init. Structure. */
   NULL			/* Multiplexer Write Queue Init. Structure. */
};


/* Functions needed for loadable Linux streams modules.  */
#ifdef LFS
int sctmrinit(void);
#endif
int sctmrinit()
{
   static char   Cf_Signon[] =
      "Dialogic Timer Module \n%s\n%s";
   static char   Cf_Rights[] =
      "%s\nALL RIGHTS RESERVED\n\n";
       
   printk((Cf_Signon), (PKGVER), (OSTYPEVER));
   if (LINUX_VERSION_CODE > 0x020600)
       printk(" - Kernel 2.6.x\n");
   else if (LINUX_VERSION_CODE > 0x020400)
       printk(" - Kernel 2.4.x\n");
   else
       printk(" - Kernel 2.2.x\n");
   printk((Cf_Rights), (COPYRIGHT));
 
   return (0);
}

int init_module(void)
{
   int ret;

   ret = lis_register_strmod(&sctmrinfo, LIS_OBJNAME_STR);
   if (ret < 0) {
       printk("%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
       return ret;
   }

   sctmrinit();

   return 0;
}

void cleanup_module(void)
{
   int err;

   err = lis_unregister_strmod(&sctmrinfo);
   if (err < 0)
       printk("%s: Unable to unregister module, error %d.\n", LIS_OBJNAME_STR, -err);
   else
       printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
   
   return;
}      /* end of cleanup_module() */


/*****************************************************************************
 *        NAME: VOID sct_getfreemp( freemp )
 * DESCRIPTION: Function to acquire a basic reply packet for an unsolicited
 *              event.
 *      INPUTS: long	freemp;		- pointer to message block
 *     OUTPUTS: Saves a pointer to the allocated message block
 *     RETURNS: None
 *    CAUTIONS: If the call to allocb() fails, bufcall() will be called to
 *              so that sct_getfreemp() will be re-called later when the
 *              memory is available. However, if the call to bufcall()
 *              should fail, a console message will be issued, and the
 *              driver will be dead in the water. Should this occur, the
 *              system administrator should reconfigure and rebuild UNIX
 *              with larger values for NSTREVENT and/or MAXSEPGCNT.
 ****************************************************************************/

#ifdef LFS
void streamscall sct_getfreemp(long freemp)
#else
void sct_getfreemp( freemp )
   void *freemp;
#endif
{
   DLGCDECLARESPL(oldspl)
   long      size = sizeof( TM_DRVCMD );

   /* Get an unsolicited msg block for general use */
   DLGCSPLSTR(oldspl);

   if ( ( sct_freemp = (mblk_t *) allocb( size, BPRI_HI ) ) == NULL ) {
      if ( !bufcall( size, BPRI_HI, sct_getfreemp, (long) freemp ) ) {
         DLGC_MSG2( CE_WARN, "TMR: sct_getfreemp(): Unable to recover from allocb failure!" );
      }
      DLGCSPLX(oldspl);
      return;
   }

   /*
    * Zero out the message buffer and adjust b_wptr accordingly
    */
   bzero( (char *)sct_freemp->b_datap->db_base, size );
   sct_freemp->b_wptr += size;
   sct_freemp->b_datap->db_type = M_PROTO;

   /*
    * Set the size of the variable-length data field
    */
   ((GN_CMDMSG *)(sct_freemp->b_rptr))->fw_datalen[0] =
		sizeof( TM_DRVCMD ) - sizeof( GN_CMDMSG );

   DLGCSPLX(oldspl);
}


/***********************************************************************
 *        NAME: int sctmropen(q, dev, flag, sflag)
 * DESCRIPTION: Module open routine.
 *              If first call, zero out data structures and set default
 *              bdm parameters.
 *      INPUTS: q, dev, flag, sflag - generic STREAMS open args
 *     OUTPUTS:
 *     RETURNS: minor number if successful
 *              OPENFAIL if dev out of range
 *       CALLS: dlgn_msg() minor()
 *    CAUTIONS:
 **********************************************************************/
int sctmropen(q, devp, flag, sflag, credp)
   queue_t  *q;
   dev_t    *devp;
   int       flag;
   int       sflag;
   cred_t   *credp;
{
   dev_t dev;
   SCT_INFO    *sctmsp;
   SCT_INFO    *esctmsp;
   static unsigned long once = 0;
#ifdef LFS
   unsigned long z_flag;
#else
   int		z_flag;
#endif
   unsigned long ulong_dev;
   DLGCDECLARESPL(oldspl)

   DLGC_MSG4(CE_DEBUG, "sctmropen() q:%x dev:%x\n", q, dev);

   dev = MINOR( *devp );

   ulong_dev = dev;
   if ( (ulong_dev < 0) || (ulong_dev >= TIMER_CNT) ) {
      return( OPENFAIL );
   }

   DLGCSPLSTR(oldspl);

   DLGCMUTEX_ENTER(&st_sctlock);
   DLGCSPINLOCK(st_sctlock, z_flag);
   if ( once == 0 ) {
      /*
       * Initialise the Queues
       */
      DLGC_MSG2(CE_DEBUG, "sctmropen(): Initialising the timer queue\n" );
      if ( st_InitQueue( 0 ) == -1 ) {
	 DLGCSPLX(oldspl);
	 DLGCMUTEX_EXIT(&st_sctlock);
         DLGCSPINUNLOCK(st_sctlock, z_flag);
	 DLGC_MSG2( CE_CONT, "SCSAtmr: sctmropen(): Cannot allocate memory" );
	 return( -1 );
      }

      sctmsp = sct_info;
      esctmsp = &sct_info[GN_MAXHANDLES];
      for (; sctmsp <  esctmsp; sctmsp++) {
         bzero((char *)sctmsp, sizeof(SCT_INFO));
      }

      sct_count = 0;
#ifdef LFS
      sct_getfreemp( (long)sct_freemp );
#else
      sct_getfreemp( sct_freemp );
#endif
      once++;
   }

   /*
    * If greater than GN_MAXHANDLES streams are opened, then
    * return an error.
    */
   if ( sct_count > GN_MAXHANDLES ) {
      DLGCSPLX(oldspl);
      DLGCMUTEX_EXIT(&st_sctlock);
      DLGCSPINUNLOCK(st_sctlock, z_flag);
      DLGC_MSG2(CE_DEBUG, "sctmropen(): sct_count exceeded\n" );
      return( -1 );
   }

   /*
    * Need to Update the SCT_INFO structure for each time the
    * open is called as a new stream is created.
    */
   sctmsp = sct_info;
   esctmsp = &sct_info[GN_MAXHANDLES];
   for ( ; sctmsp < esctmsp; sctmsp++ ) {
      if ( sctmsp->tinfo_wqp == (queue_t *) NULL ) {
	 sctmsp->tinfo_wqp = q;
	 q->q_ptr = (char *) sctmsp;
	 sct_count++;
	 DLGC_MSG3( CE_DEBUG, "sctmropen() sctmsp:%x\n", sctmsp );
	 break;
      }
   }

#ifndef LINUX26
   MOD_INC_USE_COUNT;
#endif /* LINUX */

   DLGCSPLX(oldspl);
   DLGCMUTEX_EXIT(&st_sctlock);
   DLGCSPINUNLOCK(st_sctlock, z_flag);
   DLGC_MSG2(CE_DEBUG, "sctmropen(): returning from open()\n" );

   return( 0 );
}


/***********************************************************************
 *        NAME: int sctmrclose(q)
 * DESCRIPTION: Module close routine.  Clean up data structures
 *              associated with handles bound through this STREAMS
 *              instance.
 *      INPUTS: q - write queue pointer
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: bzero() freemsg() dlgn_msg()
 *    CAUTIONS:
 **********************************************************************/
int sctmrclose(q, flag, credp)
   queue_t *q;
   int      flag;
   cred_t  *credp;
{
#ifdef LFS
   unsigned long z_flag;
#else
   int		 z_flag;
#endif
   DLGCDECLARESPL(oldspl)
   TIMER_INFO	*tmp;

   DLGC_MSG3(CE_DEBUG, "sctmrclose() q:%x\n", q);

   /* Clean up only the structs corresponding to this close */ 
   DLGCSPLSTR(oldspl);
   DLGCMUTEX_ENTER(&st_sctlock);
   DLGCSPINLOCK(st_sctlock, z_flag);
   bzero( (char *) q->q_ptr, sizeof( SCT_INFO ) );
   sct_count--;
   DLGCSPLX(oldspl);
   DLGCMUTEX_EXIT(&st_sctlock);
   DLGCSPINUNLOCK(st_sctlock, z_flag);

   DLGC_MSG3(CE_DEBUG, "sctmrclose() cleanup q:%x\n", q);
   /*
    * Move all UsedQ entries for this streams device, to the FreeQ 
    * WHAT ABOUT UNTIMEOUT() if the first in USED Q is DELETED?????????
    */
   DLGCSPINLOCK(st_uselock, z_flag);
   DLGCMUTEX_ENTER(&st_uselock);
   DLGCSPLSTR(oldspl);
#ifdef SKIP
   if(sct_tmoid)
    {
	untimeout(sct_tmoid);
	sct_tmoid=0;
    }
#endif
   tmp = quse;
   while (tmp) {
      if ( tmp->ti_qptr == WR( q ) ) {
	 if ( tmp->next ) {
	    tmp = tmp->next;
	    if ( st_Add2FreeQ( tmp->prev ) == -1 ) {
               DLGC_MSG2(CE_CONT,"sctmrclose(): Unable to move to Free Q");
	    }
	 } else {
	    if ( st_Add2FreeQ( tmp ) == -1 ) {
               DLGC_MSG2(CE_CONT,"sctmrclose(): Unable to move to Free Q");
	    }
	    tmp = (TIMER_INFO *) NULL;
	 }
      } else {
	 tmp = tmp->next;
      }
   }

#ifndef LINUX26
   MOD_DEC_USE_COUNT;
#endif 

   DLGCSPINUNLOCK(st_uselock, z_flag);
   DLGCMUTEX_EXIT(&st_uselock);
   DLGCSPLX(oldspl);
   return( 0 );
}


/*************************************************************************
 *        NAME: static void sct_print_queue( prq )
 * DESCRIPTION: Print Queue in kernel space
 *      INPUTS: TIMER_INFO *prq;
 *     OUTPUTS: Queue
 *     RETURNS: N/A
 *    CAUTIONS: None
 *************************************************************************/

static void sct_print_queue( prq )
   TIMER_INFO *prq;
{
   DIAGENTER1( "sct_print_queue", "prq:%x", prq )
#ifdef LFS
	   (void) _diag_FuncName;
#endif

   if ( prq ) {
      while ( prq ) {
	 DIAGINFO4( 3, "Timeout = %d  Timeleft = %d  ddd = %d   Id = %d",
		prq->ti_timeout, prq->ti_timeleft, prq->ti_ddd, prq->ti_id );
	 prq = prq->next;
      }
   } else {
      DIAGINFO( 3, "Queue is Empty\n" );
   }

   DIAGEXIT()
}


/*************************************************************************
 *        NAME: int sct_genmsg( tinfop )
 * DESCRIPTION: This function takes care of generating a timeout message
 *		and sends it to the stream head.
 *      INPUTS: TIMER_INFO *tinfop;
 *     OUTPUTS: N/A
 *     RETURNS:  0 = successful send
 *		-1 = no free messages
 *    CAUTIONS: None
 *************************************************************************/

#ifdef LFS
int sct_genmsg( TIMER_INFO *tinfop );
#endif
int sct_genmsg( tinfop )
   TIMER_INFO *tinfop;
{
   register GN_CMDMSG	*cmp;
   register TM_DATA	*dptr;
   mblk_t		*msgp;
   TIMER_INFO		*tmp;

   DIAGENTER1( "sctmrrput", "tinfop:%x", tinfop )
#ifdef LFS
	   (void) _diag_FuncName;
#endif

   DEBUG_PRINT_QUEUE(tinfop);

   /*
    * Get a message block
    */
   if ( ( msgp = sct_freemp ) == (mblk_t *) NULL ) {
      DLGC_MSG2( CE_CONT,
		"sct_genmsg(): Null message block - Event not generated\n" );
      DIAGEXIT()
      return( -1 );
   }

   /* Get a message block for future use  */
   sct_freemp = (mblk_t *)NULL;
#ifdef LFS
   sct_getfreemp( (long) sct_freemp );
#else
   sct_getfreemp( sct_freemp );
#endif

   /* Setup some useful pointers into the message block */
   cmp  = &(((TM_DRVCMD *)msgp->b_rptr)->gn_drvhdr);
   dptr = &(((TM_DRVCMD *)msgp->b_rptr)->tm_data);

   /*
    * Create an TM_EVENT message
    */
   cmp->cm_msg_type  = MT_EVENT;
   cmp->fw_msg_ident = TM_EVENT;

   dptr->tm_ddd = tinfop->ti_ddd;
   dptr->tm_id = tinfop->ti_id;
   dptr->tm_timeout = 0;
   if ( tinfop->ti_timeleft ) {
      /*
       * Timer was Cancelled, hence also need to return the
       * number of ticks left.
       */
      tmp = tinfop;
      while ( tmp ) {
	 dptr->tm_timeout += tmp->ti_timeleft;
	 tmp = tmp->prev;
      }
   }

   /*
    * Everything has been set-up, now need to send the message
    * to the stream head and enable the read side service queue
    */
   DIAGINFO2( 1, "putq() for genmsg  q:%x mp:%x", RD( tinfop->ti_qptr ), msgp )
   putq( RD( tinfop->ti_qptr ), msgp );
#ifndef LFS
   /* This qenable() is unnecessary. -bb */
   qenable( RD( tinfop->ti_qptr ) );
#endif

   DIAGEXIT()
   return( 0 );
}


/*************************************************************************
 *        NAME: void sct_timer( tmoflag )
 * DESCRIPTION: This function takes care of the timer generation once
 *		the initial timer has started. It will also generate
 *		the timer event messages which will be send to the
 *		stream head.
 *      INPUTS: long tmoflag;	- not used by this module
 *     OUTPUTS: N/A
 *     RETURNS: N/A
 *    CAUTIONS: None
 *************************************************************************/

#ifdef LFS
void streamscall sct_timer( caddr_t tmoflag )
#else
void sct_timer( tmoflag )
   long tmoflag;
#endif
{
#ifdef LFS
   unsigned long z_flag;
#else
   int	z_flag;
#endif
   DLGCDECLARESPL(oldspl)
   DIAGENTER1( "sct_timer", "tmoflag:%x", tmoflag )
#ifdef LFS
	   (void) _diag_FuncName;
#endif

   DLGCSPLSTR(oldspl);
   DLGCMUTEX_ENTER(&st_uselock);
   DLGCSPINLOCK(st_uselock, z_flag);
   if ( quse ) {
      if ( --quse->ti_timeleft > 0 ) {
	 /*
	  * Need to process the timer further
	  */
	 sct_tmoid = timeout(sct_timer, (caddr_t)tmoflag, drv_usectohz(time_resolution*10000) );
      } else {

	 DLGC_MSG2(CE_CONT, "Timeleft is 0 or negative");
	 DEBUG_PRINT_QUEUE(quse);

	 /*
	  * If by CHANCE, there is a BUG and the timeleft is
	  * a negative value, we must catch it before it runs
	  * away!!!!!!!!!
	  */
	 if ( quse->ti_timeleft < 0 ) {
	    /*
	     * It is a timer which has some invalid timeleft!!
	     * This message must be seen by ALL, regardless of
	     * whether the customer is AT+T or not.
	     */
	    cmn_err( CE_WARN,
		"sct_timer(): Timeleft is a negative value and running!!\n" );
	    cmn_err( CE_WARN, "q:%p  ddd:%lx  id:%lx  timeleft:%ld\n",
		quse->ti_qptr, quse->ti_ddd, quse->ti_id, quse->ti_timeleft );
	    cmn_err( CE_WARN, "Report this problem to Dialogic Tech Support\n");
	 }

	 /*
	  * Generate messages for all the timers for which
	  * the timer has expired. NOTE that the global
	  * variable "quse" gets updated by the st_Add2FreeQ()
	  * function.
	  */
	 while ( quse ) {

	    if ( quse->ti_timeleft <= 0 ) {
	       /*
		* It is time to generate a timed-out message
		*/
	       if ( sct_genmsg( quse ) == -1 ) {
		  DLGC_MSG2( CE_CONT,
			"sct_timer(): Cannot Generate Timeout Message!!\n" );
	       } else {
		  if ( st_Add2FreeQ( quse ) == -1 ) {
		     /* Should Never happen */
		     DLGC_MSG2( CE_CONT,
			"sct_timer(): Unable to add to Free Queue!!\n" );
		  }

		  DLGC_MSG2(CE_CONT, "After Generating the message" );
		  DEBUG_PRINT_QUEUE(quse);
	       }
	    }

	    /*
	     * Condition to break out of the while loop
	     * If  quse  is NULL or timeleft > 0
	     */
	    if ( quse ) {
	       if ( quse->ti_timeleft > 0 ) {
		  break;
	       }
	    } else {
	       break;
	    }
	 }

	 /*
	  * Now the  quse  has been modified, so we need to make sure
	  * that we can continue with the timeouts
	  */
	 if ( quse ) {
	 sct_tmoid = timeout(sct_timer, (caddr_t)tmoflag, drv_usectohz(time_resolution*10000) );
	 } else {
	    /* No more timeout()'s */
	    sct_tmoflag = 0;
	 }
      }
   } else {
      /* No more timeout()'s */
      sct_tmoflag = 0;
   }

   DIAGEXIT()
   DLGCSPLX(oldspl);
   DLGCMUTEX_EXIT(&st_uselock);
   DLGCSPINUNLOCK(st_uselock, z_flag);
   return;
}


/***********************************************************************
 *        NAME: int sctmrwput(q, mp)
 * DESCRIPTION: Write side put procedure.
 *      INPUTS: q - write queue
 *              mp - STREAMS message pointer
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
int sctmrwput(q, mp)
   queue_t  *q;
   mblk_t   *mp;
{
#ifndef LFS
   unsigned char *startp;
   SCT_INFO	 *sctmsp;
#endif
   TM_DATA	 *tmdatap;
   TIMER_INFO	 *tinfop, *tmp;
   GN_CMDMSG	 *drvhdr;
#ifndef LFS
   int		 handle;
#endif
   int		 count;
#ifdef LFS
   unsigned long z_flag;
#else
   int		 z_flag;
#endif
   DLGCDECLARESPL(oldspl)
#ifndef LFS
   int		 rc;
#endif
   long		 timeleft;

   DLGC_MSG4(CE_DEBUG, "sctmrwput() q:%x mp:%x\n", q, mp);

   switch ( mp->b_datap->db_type ) {
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
         DLGCSPLX(oldspl);              /* restore interrupts */
      }
      break;

   case M_PROTO:
      drvhdr  = &(((TM_DRVCMD *)mp->b_rptr)->gn_drvhdr);
      tmdatap = &(((TM_DRVCMD *)mp->b_rptr)->tm_data);

      switch ( drvhdr->cm_msg_type ) {
      case MT_DEVICE:
         switch ( drvhdr->fw_msg_ident ) {
         case TM_SETTIMER:
	    DLGCSPLSTR(oldspl);

	    if ( st_Add2UsedQ( q, (mblk_t *) NULL, tmdatap->tm_timeout,
			tmdatap->tm_ddd, tmdatap->tm_id) == -1 ) {
	       DLGCSPLX(oldspl);
	       SCT_WQREPLY( q, mp, TMERR_NOFREEQ );
	       break;
	    }

	    DLGCMUTEX_ENTER(&st_uselock);
            DLGCSPINLOCK(st_uselock, z_flag);
	    DEBUG_PRINT_QUEUE(quse);

	    /*
	     * If we are here then we have successfully added the
	     * timer to the queue. If the timer has not already
	     * been started then it should be started. The timer
	     * will be started in units of 1 tick. Hence the
	     * timeout routine will be called every 1 tick. It
	     * needs to be done every tick because we would also
	     * like to send the number of ticks remaining if any
	     * timer has been cancelled.
	     */
	    if ( quse ) {
	       /*
		* We have something in the timer queue
		*/
	       if ( sct_tmoflag == 0 ) {
		  /*
		   * The timer has not been started, so lets start it!
		   * The timer will be started every 1 tick so that
		   * we can also return the timeleft.
		   */
		  sct_tmoflag = 1;
	          sct_tmoid = timeout(sct_timer, (caddr_t)sct_tmoflag, drv_usectohz(time_resolution*10000) );
	       }
	    }

	    DLGCSPLX(oldspl);
	    DLGCMUTEX_EXIT(&st_uselock);
            DLGCSPINUNLOCK(st_uselock, z_flag);
	    SCT_WQREPLY( q, mp, 0 );
	    break;

         case TM_CANCELTMR:
	    DLGCSPLSTR(oldspl);
	    DLGCMUTEX_ENTER(&st_uselock);
            DLGCSPINLOCK(st_uselock, z_flag);

	    if ( tmdatap->tm_id == -1 ) {
	       /*
		* A wildcard cancellation
		*/
	       while ( (tinfop = st_FindEntry( q, tmdatap->tm_ddd,
				tmdatap->tm_id) ) ) {
		  /*
		   * If the first in the Used Q is found and the timer
		   * has been started, untimeout()
		   */
		  if ( (tinfop == quse) && sct_tmoflag ) {
	             DLGCMUTEX_EXIT(&st_uselock);
                     DLGCSPINUNLOCK(st_uselock, z_flag);
		     untimeout( sct_tmoid );
		     sct_tmoflag = 0;
	    	     DLGCMUTEX_ENTER(&st_uselock);
                     DLGCSPINLOCK(st_uselock, z_flag);
		  }

		  if ( st_Add2FreeQ( tinfop ) == -1 ) {
		     DLGC_MSG2(CE_CONT, "sctmrwput(): Unable to move to Free Q");
		  }

		  DEBUG_PRINT_QUEUE(quse);
	       }

	       if ( quse && sct_tmoflag == 0 ) {
		  sct_tmoflag = 1;
	          sct_tmoid = timeout( sct_timer, (caddr_t)sct_tmoflag, drv_usectohz(time_resolution*10000) );
	       }

	       DLGCMUTEX_EXIT(&st_uselock);
               DLGCSPINUNLOCK(st_uselock, z_flag);
	       /*
		* Send a message to indicate successful deletion.
		*/
	       tmdatap->tm_timeout = -1;
	       SCT_WQREPLY( q, mp, 0 );

	    } else {
	       /*
		* Delete an explicit timer
		*/
	       timeleft = 0;

	       if ( (tinfop = st_FindEntry(q, tmdatap->tm_ddd, tmdatap->tm_id)) ){
		  /*
		   * Found an entry and calculate timeleft.
		   * If first in Used Q, than also untimeout()
		   */
		  if ( tinfop == quse ) {
		     timeleft += tinfop->ti_timeleft;
		     if ( sct_tmoflag ) {
	                DLGCMUTEX_EXIT(&st_uselock);
                        DLGCSPINUNLOCK(st_uselock, z_flag);
			untimeout( sct_tmoid );
			sct_tmoflag = 0;
	    	        DLGCMUTEX_ENTER(&st_uselock);
                        DLGCSPINLOCK(st_uselock, z_flag);
		     }
		  } else {
		     tmp = tinfop;
		     while ( tmp ) {
			timeleft += tmp->ti_timeleft;
			tmp = tmp->prev;
		     }
		  }

		  if ( st_Add2FreeQ( tinfop ) == -1 ) {
		     DLGC_MSG2(CE_CONT, "sctmrwput(): Unable to move to Free Q");
		  }

		  /*
		   * Re-start the timer if necessary
		   */
		  if ( quse && sct_tmoflag == 0 ) {
		     sct_tmoflag = 1;

	             sct_tmoid = timeout( sct_timer, (caddr_t)sct_tmoflag, drv_usectohz(time_resolution*10000));
		  }

		  DLGCMUTEX_EXIT(&st_uselock);
                  DLGCSPINUNLOCK(st_uselock, z_flag);
		  /*
		   * Send a message to indicate successful deletion.
		   */
		  tmdatap->tm_timeout = timeleft;
		  SCT_WQREPLY( q, mp, 0 );

	       } else {
		  DLGCMUTEX_EXIT(&st_uselock);
                  DLGCSPINUNLOCK(st_uselock, z_flag);
		  /*
		   * No entry found
		   */
		  SCT_WQREPLY( q, mp, TMERR_NOTIMERS );
	       }
	    }

	    DLGCSPLX(oldspl);
	    break;

	 default:
	    /* pass thru everything we don't understand */
	    putnext(q, mp);
	    break;
	 }
	 break;

      case MT_CONTROL:

         switch ( drvhdr->fw_msg_ident ) {
         case TM_SETPARM:
	    DLGCSPLSTR(oldspl);

            /* Get the number of entries from the message */
#ifdef DLGC_LIS_FLAG
	    __wrap_memcpy(&count, mp->b_cont->b_rptr, sizeof( long ) );
#else
	    bcopy( (char *)mp->b_cont->b_rptr, (char *)&count, sizeof( long ) );
#endif

	    time_resolution = count;
            DLGCSPLX(oldspl);
#ifndef LFS
	    SCT_WQREPLY( q, mp, rc );
#else
	    SCT_WQREPLY( q, mp, 0 );
#endif
            break;

         case TM_GETPARM:
            /* Copy the number of entries into the message */
#ifdef DLGC_LIS_FLAG
	    __wrap_memcpy(mp->b_cont->b_rptr, &time_resolution, sizeof( long ) );
#else
	    bcopy( (char *)&time_resolution, (char *)mp->b_cont->b_rptr, sizeof( long ) );
#endif
	    SCT_WQREPLY( q, mp, 0 );
            break;

         default:
            /* Pass Thru Everything we Don't Understand */
            putnext(q, mp);
            break;
	 }
	 break;

      default:
         /* Pass Thru Everything we Don't Understand */
         putnext(q, mp);
         break;
      }
      return(0);
   default:       /* Pass on Anything Else to SM */
      putnext(q, mp);
      break;
   }

   return( 0 );
}


/***********************************************************************
 *        NAME: int sctmrwsrv(q)
 * DESCRIPTION: Write service procedure.  Currently not necessary.
 *      INPUTS: q - module write queue
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: dlgn_msg() getq()
 *    CAUTIONS:
 **********************************************************************/
int sctmrwsrv(q)
   queue_t * q;
{
   mblk_t      *msgp;

   DLGC_MSG3(CE_DEBUG, "sctmrwsrv() q:%x\n", q);

   /*
    * Service queued messages
    */
   while ( ( msgp = getq( q ) ) != NULL ) {
      if ( msgp->b_datap->db_type < QPCTL || canput( q->q_next ) ) {
         putnext( q, msgp );
         continue;
      } else {
         putbq( q, msgp );
         break;
      }
   }
   return( 0 );
}


/***********************************************************************
 *        NAME: int sctmrrput(q, mp)
 * DESCRIPTION: Read side put procedure; receive message from SM.
 *		Only interested in passing it through
 *      INPUTS: q - read queue
 *              mp - message from SM.
 *     OUTPUTS:
 *     RETURNS: 0.
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
int sctmrrput(q, mp)
   queue_t  *q;
   mblk_t   *mp;
{
#ifndef LFS
   int            handle;
#endif
   DLGCDECLARESPL(oldspl)
#ifndef LFS
   GN_CMDMSG      *drvhdr;
   SCT_INFO       *sctmsp;
   int            sendbytes;
   struct iocblk  *iocp;
   GN_BIND        *bindp;
#endif

   DLGC_MSG4(CE_DEBUG, "sctmrrput() q:%x  mp:%x", q, mp);

   switch (mp->b_datap->db_type) {
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

   case M_PROTO:
   default:
      putnext(q, mp);
      break;
   }
   return(0);
}


/***********************************************************************
 *        NAME: int sctmrrsrv(q)
 * DESCRIPTION: Read side service procedure.  Should not reach here.
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
int sctmrrsrv(q)
   queue_t *q;
{
   mblk_t      *msgp;

   DLGC_MSG3(CE_DEBUG, "sctmrrsrv() q:%x\n", q);

#ifdef LFS
(void) sct_print_queue;
#endif
   /*
    * Service queued messages
    */
   while ( ( msgp = getq( q ) ) != NULL ) {
      if ( msgp->b_datap->db_type < QPCTL || canput( q->q_next ) ) {
         putnext( q, msgp );
         continue;
      } else {
         putbq( q, msgp );
         break;
      }
   }

   return( 0 );
}
