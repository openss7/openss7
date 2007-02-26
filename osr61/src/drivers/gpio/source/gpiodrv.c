/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gpiodrv.c
 * Description                  : Generic Port I/O Device Driver
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
#define _SVR4_SOURCE	1
#define _OSF_SOURCE	1
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
#include <sys/lismem.h>
#endif

#include "dlgclinux.h"
#include "gndefs.h"
#include "gnlibdrv.h"
#include "gnmsg.h"
#include "gndrv.h"
#include "gentypedefs.h"
#include "dpci.h"


#include <sys/kmem.h>
#include <sys/ddi.h>

static int gpio_major = 0;
extern int index;  /* Changed for SCO need verification on Solaris */

#ifdef PCI_SPAN
extern PGP_CONTROL_BLOCK GpControlBlock;
extern UCHAR NumOfVoiceBoards;
#endif /* PCI_SPAN */

typedef VOID *  Q_PTR;


/* Local Header Files */
#include "gentypedefs.h"
#include "gnlogmsg.h"
#include "gpiodrv.h"
#include "gpiolib.h"
#include "drvdebug.h"
#include "cfd.h"
#include "dlgcos.h"

extern lis_spin_lock_t inthw_lock;	/* mutex for Span hardware interrupts */
MODULE_AUTHOR("Intel");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dialogic Generic Port I/O Device Driver");

#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("gpio");
MODULE_ALIAS("streams-gpio");
MODULE_ALIAS("streams-gpioDriver");
#endif
#endif

/* DEFINES */
#define GPIO_ID         0       /* Generic Port Driver ID */
#define DR_MINOR        255     /* Driver Minor device number */
#define LO_WATER        1         	/* Packet Size Minimum */
#define HI_WATER        64 * 1024	/* Packet Size Maximum */ 
#define MIN_PSZ         0	        /* High Water Mark */	  
#define MAX_PSZ         64 * 1024	/* Low Water Mark */ 
#if LINUX_VERSION_CODE > 0x020600
#define LINUX26
#endif
/* Prototypes for STREAMS functions */
VOID gpioinit(void);
INT  gpioopen(queue_t *, dev_t *, int, int, cred_t *),
     gpioclose(queue_t *, int, cred_t *);
int  gpiowput(queue_t *, mblk_t *);


/* PRIVATE Function Prototypes */
PRIVATE INT    _gpio_setflags(GPIO_LOGDATA *, UINT),
               _gpio_readport(GPIO_DATA *, UINT),
               _gpio_writeport(GPIO_DATA *, UINT),
               _gpio_readmemory(GPIO_DATA *, UINT),
               _gpio_writememory(GPIO_DATA *, UINT),
               _gpio_locateboards(GPIO_DATA *, UINT),
               _gpio_writeccmdata(GPIO_DATA *, UINT),
               _gpio_mapboard(GPIO_MAPDATA *, UINT),           
               _gpio_enableinterrupts(void),
               _gpio_disableinterrupts(void),
               _gpio_getinterruptcount(GPIO_INTDATA *, UINT),
	       _gpio_getpcibrdinfo(GPIO_DATA *, UINT),
	       _gpio_getnumpcibrds(GPIO_DATA *, UINT),
	       _gpio_getpcibrdbusslot(GPIO_DATA *, UINT),
	       _gpio_setpcibusnum(GPIO_DATA *, UINT),
               _gpio_resetinterruptcount(void);
#ifdef LFS
PRIVATE VOID   _gpio_logmsg(SHORT, const CHAR *, ...) __attribute__ ((__format__(__printf__, 2, 3)));
PRIVATE VOID   _gpio_wqreply(queue_t *, mblk_t *, INT),
#else
PRIVATE VOID   _gpio_logmsg(),
               _gpio_wqreply(queue_t *, mblk_t *, INT),
#endif
               _gpio_putioctldata(mblk_t *, UCHAR *),
               _gpio_freemsg(mblk_t *),
               _gpio_rlsvirtmem(UCHAR *, ULONG, ULONG),
               _gpio_systime(VOID),
               _gpio_busywait(ULONG),
               _gpio_boardinterrupthandler(void);
PRIVATE UCHAR *_gpio_getioctldata(mblk_t *),
              *_gpio_getvirtmem(ULONG, ULONG);

/* Read Side Module Information */
PRIVATE struct module_info gpiorminfo = {
   GPIO_ID,       /* Module ID.  Used for logging               */
   "gpio",        /* Module name.  Same as that used in mdevice */
   0,             /* Minimum packet size                        */
   0,             /* Maximum packet size                        */
   0,             /* High water mark.  Used in flow control     */
   0              /* Low water mark.   Ditto                    */
};

/* Write Side Module Information */
PRIVATE struct module_info gpiowminfo = {
   GPIO_ID,       /* Module ID.  Used for logging               */
   "gpio",        /* Module name.  Same as that used in mdevice */
   MIN_PSZ,       /* Minimum packet size                        */
   MAX_PSZ,       /* Maximum packet size                        */
   HI_WATER,      /* High water mark.  Used in flow control     */
   LO_WATER       /* Low water mark.   Ditto                    */
};

/* Read Side Queue Information */
PRIVATE struct qinit gpiorinit = {
   NULL,          /* Pointer to Put Routine                   */
   NULL,          /* Pointer to Service Routine               */
   gpioopen,      /* Pointer to Open Routine                  */
   gpioclose,     /* Pointer to Close Routine                 */
   NULL,          /* Reserved by STREAMS for future use       */
   &gpiorminfo,   /* Pointer to module_info structure         */
   NULL           /* Pointer to optional statistics structure */
};

/* Write Side Queue Information */
PRIVATE struct qinit gpiowinit = {
   gpiowput,      /* Pointer to Put Routine                   */
   NULL,          /* Pointer to Service Routine               */
   NULL,          /* Pointer to Open Routine                  */
   NULL,          /* Pointer to Close Routine                 */
   NULL,          /* Reserved by STREAMS for future use       */
   &gpiowminfo,   /* Pointer to module_info structure         */
   NULL           /* Pointer to optional statistics structure */
};


/*
 * Streamtab entry - Used in cdevsw and fmodsw to point to the driver
 * or module.  This is typically the only public structure in a STREAMS
 * driver.
 */
struct streamtab gpioinfo = {
   &gpiorinit,    /* Pointer to Read Queue Init. Structure   */
   &gpiowinit,    /* Pointer to Write Queue Init. Structure  */
   NULL,          /* Multiplexer Read Queue Init. Structure  */
   NULL           /* Multiplexer Write Queue Init. Structure */
};



/*
 * Global flag for Multi-Processing OS.  A value of 0 binds the driver
 * to the first processor in the system and signifies that the driver
 * is not multi-threaded.
 */
int gpiodevflag = 0;


/* PRIVATE Variables */
/* Message Log flag */
PRIVATE USHORT gpiolog = LF_CONT | LF_NOTE | LF_WARN | LF_PANIC;

/* Signon and Initialization Messages */
PRIVATE char
   gpiosignonmsg[] = "Dialogic Generic Port I/O Device Driver\n%s\n%s",
   gpiorightsmsg[] = "%s\nALL RIGHTS RESERVED\n\n";

/* Driver open flag */
PRIVATE UINT gpio_drvopen = FALSE;

/* Driver data buffer pointer */
PRIVATE UCHAR *_gpio_datap = NULL;

/* Used to determine and setup number of loops for msec and usec busywait */
PRIVATE ULONG _gpio_loopstate = 0, _gpio_msecloop = 0, _gpio_usecloop = 0;

/* Used to save the offset of an address within a segment */
PRIVATE ULONG _gpio_offset = 0;

/* interrupt counter */
PRIVATE USHORT gpio_intcnt = 0;

/* from GD */
extern GN_HEADER *Gn_Headerp;
#ifdef LFS
extern void dl_setintr(void(*)(void));
#else
extern void dl_setintr();
#endif
#ifdef SPARC_PCI
extern void sr_copy();
#endif


/* Functions needed for loadable Linux streams drivers.  */
int init_module(void)
{
   int ret;
  
   ret = lis_register_strdev(gpio_major, &gpioinfo, 1, LIS_OBJNAME_STR);
   if (ret < 0) {
       printk("%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
       return ret;
   }

   if (gpio_major == 0) {
       gpio_major = ret;
   }

   gpioinit();

   return 0;
}

void cleanup_module(void)
{
   int err = lis_unregister_strdev(gpio_major);
   if (err < 0)
       printk("%s: Unable to unregister module, error %d.\n", LIS_OBJNAME_STR, -err);
   else
       printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
   
   return;
}       /* end of cleanup_module() */


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_setflags(dip, count)
 * DESCRIPTION: This function sets the PRIVATE global variable gpiolog
 *            : using the action specified and the new value of the bit-mask
 *            : sent in by the user.
 *      INPUTS: dip   - GPIO_LOGDATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: gpiolog bit-mask set to a new value using the bitmask sent
 *            : to the driver.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_setflags(GPIO_LOGDATA *dip, UINT count)
{
   if (sizeof(GPIO_LOGDATA) != count) {
      return EINVAL;
   }

   if (dip->mode == LM_ENABLE) {
      gpiolog |= dip->flags;
   } else if (dip->mode == LM_DISABLE) {
      gpiolog &= ~(dip->flags);
   } else {
      return EINVAL;
   }

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_logmsg(msglevel, fmt, arg1, arg2,
 *            :                       arg3, arg4, arg5, arg6, arg7, arg8)
 * DESCRIPTION: This function controls console messages.  The user can
 *            : optionally enable or disable all or certain levels of
 *            : console messages.  This function will only print those
 *            : levels that have been enabled by setting the bit in gpiolog.
 *      INPUTS: msglevel - one of the message levels defined in <cmn_err.h>
 *            : fmt      - character pointer to the format string
 *            : argn     - arguments to be printed on the console
 *     OUTPUTS: Prints messages to console, if enabled.  Will also cause
 *            : PANIC if CE_PANIC is the message level.
 *     RETURNS: Nothing.
 *       CALLS: cmn_err()
 *    CAUTIONS: None.
 ***************************************************************************/
#ifdef LFS
PRIVATE void _gpio_logmsg(short msglevel, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	switch (msglevel) {
	case CE_CONT:
		if (gpiolog & LF_CONT)
			vcmn_err(msglevel, fmt, args);
		break;
	case CE_NOTE:
		if (gpiolog & LF_NOTE)
			vcmn_err(msglevel, fmt, args);
		break;
	case CE_WARN:
		if (gpiolog & LF_WARN)
			vcmn_err(msglevel, fmt, args);
		break;
	case CE_PANIC:
		if (gpiolog & LF_PANIC)
			vcmn_err(msglevel, fmt, args);
		break;
	}
	va_end(args);
	return;
}
#else
PRIVATE VOID _gpio_logmsg(msglevel, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
   SHORT msglevel;
   CHAR *fmt;
   ULONG arg1;
   ULONG arg2;
   ULONG arg3;
   ULONG arg4;
   ULONG arg5;
   ULONG arg6;
   ULONG arg7;
   ULONG arg8;
{
   switch (msglevel) {
   case CE_CONT:
      if(gpiolog & LF_CONT) {
         cmn_err(msglevel,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
      }
      break;
   case CE_NOTE:
      if(gpiolog & LF_NOTE) {
         cmn_err(msglevel,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
      }
      break;
   case CE_WARN:
      if(gpiolog & LF_WARN) {
         cmn_err(msglevel,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
      }
      break;
   case CE_PANIC:
      if(gpiolog & LF_PANIC) {
         cmn_err(msglevel,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
      } else {
         cmn_err(msglevel," ");
      }
   }
}
#endif


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_wqreply(wq, mp, rc)
 * DESCRIPTION: This function returns a reply to the user.
 *            : Called while processing a user request in the write-side
 *            : put routine.
 *      INPUTS: wq - pointer to the write queue
 *            : mp - pointer to the user request message
 *            : rc - return code to return to the user
 *     OUTPUTS: Message is sent upstream to user process.
 *     RETURNS: Nothing.
 *       CALLS: qreply()
 *   CAUTIONS : None.
 ***************************************************************************/
PRIVATE VOID _gpio_wqreply(queue_t *wq, mblk_t *mp, INT rc)
{
   register UCHAR *type = &mp->b_datap->db_type;


   if (*type == M_IOCTL) {                   /* If reply to IOCTL */
      register IOCBLK *iocblkp = (IOCBLK *)mp->b_rptr;

      *type = M_IOCACK;                      /* Set message type to ACK */
      if ((iocblkp->ioc_rval = rc) != 0) {   /* Set return value of ioctl */
         iocblkp->ioc_count = 0;             /* No data to user if error */
      }
      iocblkp->ioc_error = 0;
   } else if (*type == M_PROTO) {            /* If reply to PUTMSG */
      *type = M_PCPROTO;                     /* Set message to hi-pri GETMSG */
   } else {                                  /* Unsolicited event M_DATA */
      *type = M_PROTO;                       /* Set message to lo-pri GETMSG */
   }

   qreply(wq, mp);
}


/***************************************************************************
 *        NAME: void gpioinit
 * DESCRIPTION: The purpose of this routine is to display the signon and
 *            : copyright messages at system init time.
 *      INPUTS: none.
 *     OUTPUTS: none.
 *     RETURNS: none.
 *    CAUTIONS: none.
 ***************************************************************************/
void gpioinit(void)
{
    /*
     * Display the signon message
     */
    _gpio_logmsg(CE_CONT, gpiosignonmsg, PKGVER, OSTYPEVER);

    if (LINUX_VERSION_CODE > 0x020600)
        printk(" - Kernel 2.6.x\n");
    else if (LINUX_VERSION_CODE > 0x020400)
        printk(" - Kernel 2.4.x\n");
    else
        printk(" - Kernel 2.2.x\n");

    _gpio_logmsg(CE_CONT, gpiorightsmsg, COPYRIGHT);
}


/***************************************************************************
 *        NAME: int gpioopen
 * DESCRIPTION: STREAMS Open Routine for the Dialogic Generic Port I/O Device
 *            : Driver.  This function allocates a contiguous region of
 *            : memory to be used as kernel data buffer and sets the driver
 *            : open flag after appropriate checks.
 *      INPUTS: ptr to read queue
 *            : maj/min dev number (0 for module)
 *            : file open flags (0 for modules)
 *            : stream open flags
 *     OUTPUTS: none.
 *     RETURNS: Failure, if this is an abnormal driver open.
 *       CALLS: kseg() (SVR3) or kmem_zalloc()
 *    CAUTIONS: none.
 ***************************************************************************/
INT gpioopen(queue_t *rq, dev_t *devp, int flag, int sflag, cred_t *credp)
{
   dev_t    dev;
   queue_t *wq = WR(rq);       /* Get a pointer to the write queue */

   /*
    * Make sure this is a normal driver open.
    */
#ifdef ISA_SOLDEB   
   cmn_err(CE_WARN, "gpioopen: sflag = %d\n", sflag);
#endif

   if (sflag) {
      return EINVAL;
   }

   /* Obtain the device minor number  */
   dev = MINOR(*devp);
   
   /* Make sure this is a driver call */
   if (dev == DR_MINOR) {

      /* Return an error if already opened */
      if (gpio_drvopen == TRUE) {
         return EBUSY;
      }

      /*
       * Allocate a data buffer of the largest possible size needed by
       * this driver.  By doing the allocation here we are ensuring a
       * one time allocation as this driver can only be opened once.
       * We will free up this space in the close routine.
       */
      if ((_gpio_datap = (UCHAR *)lis_get_free_pages(GFP_KERNEL, GPIOMAXSIZE)) == NULL) {
         _gpio_logmsg(CE_WARN, "GPIO: Cannot allocate 0x%x bytes", GPIOMAXSIZE);
         return ENOMEM;
      }

      bzero(_gpio_datap,GPIOMAXSIZE);

#ifndef LINUX26
      MOD_INC_USE_COUNT;
#endif

      /*
       * Save driver 'ptr' for future calls,
       * raise the open flag and return success
       */
      wq->q_ptr = (Q_PTR)DR_MINOR;
      gpio_drvopen = TRUE;
      qprocson(rq);
      return 0;
   }

   /* Only driver open is allowed.  */
   return EINVAL;
}


/***************************************************************************
 *        NAME: int gpioclose
 * DESCRIPTION: STREAMS Close Routine for the Dialogic Generic Port I/O
 *            : Device Driver.  This function re-sets the driver open flag
 *            : after appropriate checks.
 *      INPUTS: ptr to read queue
 *            : file open flags (0 for modules)
 *     OUTPUTS: none.
 *     RETURNS: Failure, if error.
 *       CALLS: unkseg() (SVR3) or kmem_free()
 *    CAUTIONS: none.
 ***************************************************************************/
INT gpioclose(queue_t *rq, int flag, cred_t *credp)
{
   qprocsoff(rq);

   /* Make sure this is a driver call */
   if ((WR(rq))->q_ptr == (Q_PTR)DR_MINOR) {
      /* Free the allocated memory segment.  */
      lis_free_pages((void *)_gpio_datap);

#ifndef LINUX26
      MOD_DEC_USE_COUNT;
#endif
      /* Lower the open flag and return success.  */
      gpio_drvopen = FALSE;
      rq->q_ptr = WR(rq)->q_ptr = NULL;
      return 0;
   }

   /* Only driver can be closed.  */
   return EINVAL;
}


/***************************************************************************
 *        NAME: int gpiowput
 * DESCRIPTION: Write side put routine.  This routine typically receives
 *            : M_IOCTL/M_PROTO messages from upstream.  The message contains
 *            : the command portion of the SBA message. If the board is
 *            : ready to receive a message, the actual message is built at
 *            : this time and sent to the board. If not, the message is queued
 *            : for future sending. As is standard protocol this routine also
 *            : provides canonical support for M_FLUSH messages.
 *      INPUTS: queue_t *wq - Pointer to the writes side STREAMS queue.
 *            : mblk_t  *mp - Pointer to the message block.
 *     OUTPUTS: Message sent to board if ready or queued.
 *     RETURNS: none.
 *    CAUTIONS: none.
 ***************************************************************************/
#ifdef LFS
int _gpio_getboardpowerstatus(GPIO_DATA *, UINT);
#endif
INT gpiowput(queue_t *wq, mblk_t *mp)
{
   IOCBLK *iocblkp;
   UCHAR  *datap = NULL;
   INT     rc = 0;
#ifndef LFS
   int 	   z_flag;
#endif

   /* Make sure this is a driver call */
      switch (mp->b_datap->db_type) {
      default:
         _gpio_logmsg(CE_WARN, "GPIO: gpiowput: Unexpected message block type");
         freemsg(mp);
         break;

      case M_FLUSH:
         /*
          * Canonical flush handling
          */

         /* Check to see if Write queue should be flushed */
         if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
            flushq(wq, FLUSHALL);
         }

         /* Check to see if Read queue should be flushed */
         if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {
            flushq(RD(wq), FLUSHALL);

            /*
             * We must insure that this message is sent to all modules on
             * the stream because we are a driver we need only worry
             * about the upstream bound (Read queue) messages.
             * First we clear the write bit (So it isn't sent back
             * downstream by stream head) then send it upstream with
             * qreply().
             */
            *mp->b_rptr |= FLUSHR;     /* Ensure Read queue flush is set */
            *mp->b_rptr &= ~FLUSHW;    /* Clear Write queue flush */
            qreply(wq, mp);            /* Send it on it's way */

         } else {

            /*
             * We have now finished with the M_FLUSH message as we don't
             * have to send it upstream.  ie no need to flush read queue.
             * So throw this message away and free up its allocated memory.
             */
            freemsg(mp);
         }
         break;

      case M_IOCTL:
         iocblkp = (IOCBLK *)mp->b_rptr;        /* get pointer to ioctl block */
         switch (iocblkp->ioc_cmd) {

         default:
            _gpio_logmsg(CE_WARN, "GPIO: gpiowput: %x :Invalid IOCTL command", iocblkp->ioc_cmd);
            _gpio_wqreply(wq, mp, EINVAL);      /* Invalid command */
            break;

         case GPIO_LOGMSG:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_setflags((GPIO_LOGDATA *)datap, iocblkp->ioc_count);

            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_READ_PORT:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_readport((GPIO_DATA *)datap, iocblkp->ioc_count);

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_WRITE_PORT:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_writeport((GPIO_DATA *)datap, iocblkp->ioc_count);

            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_READ_MEMORY:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_readmemory((GPIO_DATA *)datap, iocblkp->ioc_count);

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_WRITE_MEMORY:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_writememory((GPIO_DATA *)datap, iocblkp->ioc_count);

            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_LOCATE_BOARDS:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            /*
             * If the delay loops for busywait are not initialized, then do
             * it now.  A delay of 1 msec is caused when counting from 1 to
             * _gpio_msecloop.  Re-enable interrupts to allow timer interrupts
             * before calling systime.
             */
            if (_gpio_msecloop == 0) {
               _gpio_systime();
            }
            rc = _gpio_locateboards((GPIO_DATA *)datap, iocblkp->ioc_count);
            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_WRITE_CCM_DATA:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_writeccmdata((GPIO_DATA *)datap, iocblkp->ioc_count);

            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_GET_VERSION_NUM:
            break;
         case GPIO_MAP_BOARD:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_mapboard((GPIO_MAPDATA *)datap, iocblkp->ioc_count);

            _gpio_freemsg(mp);                  /* Free message data */
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;
         case GPIO_ENABLE_INTERRUPTS:
            rc = _gpio_enableinterrupts();
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;
         case GPIO_DISABLE_INTERRUPTS:
            rc = _gpio_disableinterrupts();
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;         
         case GPIO_GET_INTERRUPT_COUNT:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_getinterruptcount((GPIO_INTDATA *)datap, iocblkp->ioc_count);

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }
            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_RESET_INTERRUPT_COUNT:
            rc = _gpio_resetinterruptcount();

            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;
         
         case GPIO_GET_PCI_BRD_INFO:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_getpcibrdinfo((GPIO_DATA *)datap, iocblkp->ioc_count);
            
            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }

            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_GET_NUM_PCI_BRDS:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_getnumpcibrds((GPIO_DATA *)datap, iocblkp->ioc_count);

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }

            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_GET_PCI_BRD_BUS_SLOT:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_getpcibrdbusslot((GPIO_DATA *)datap, iocblkp->ioc_count);
            
            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }

            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;

         case GPIO_SET_PCI_BUS_NUM:
            if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_setpcibusnum((GPIO_DATA *)datap, iocblkp->ioc_count);
            
            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               _gpio_putioctldata(mp, datap);   /* Copy data to M_DATA blocks */
            }

            _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;
	
	 case GPIO_GET_PCIe_POWER_STATUS:
	    if (mp->b_cont == NULL) {
               _gpio_wqreply(wq, mp, EINVAL);   /* Error if no data */
               break;
            }

            if (mp->b_cont->b_cont != NULL) {   /* If two or more M_DATA block*/
               datap = _gpio_getioctldata(mp);  /* Copy ioctl data to buffer */
               if (datap == NULL) {
                  _gpio_wqreply(wq, mp, EINVAL);/* Error if invalid data */
                  break;
               }
            } else {
               datap = mp->b_cont->b_rptr;      /* Get ptr to ioctl data */
            }

            rc = _gpio_getboardpowerstatus((GPIO_DATA *)datap, iocblkp->ioc_count);

	    _gpio_wqreply(wq, mp, rc);          /* Report result to caller */
            break;
          
         }

         break;
      }

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE UCHAR *_gpio_getioctldata(mp)
 * DESCRIPTION: This function consolidates data from multiple M_DATA blocks
 *            : sent down with an M_IOCTL into a contiguos region of memory.
 *      INPUTS: mp - pointer to the user request message
 *     OUTPUTS: Data from multiple M_DATA blocks copied into allocated memory.
 *     RETURNS: Pointer to contiguous memory or NULL if failed.
 *       CALLS: bcopy()
 *   CAUTIONS : None.
 ***************************************************************************/
PRIVATE UCHAR *_gpio_getioctldata(mblk_t *mp)
{
   ULONG    count;
   register mblk_t *curmp;
   register ULONG   cur_count = 0;
   register ULONG   tot_count = 0;
   register UCHAR  *memp = _gpio_datap;

   /*
    * The message must be of the type M_IOCTL.
    */
   if (mp->b_datap->db_type != M_IOCTL) {
      return NULL;
   }

   /*
    * Get the count of bytes received from user space.
    */
   count = ((IOCBLK *)(mp->b_rptr))->ioc_count;
   if ((count == 0) || (mp->b_cont == NULL)) {
      return NULL;
   }

   /*
    * Copy the data from the M_DATA blocks into the allocated memory by
    * successively following the b_cont link.
    */
   for (curmp = mp->b_cont;  curmp != NULL;  curmp = curmp->b_cont) {
      /*
       * Get the count of bytes in this M_DATA block and copy these bytes
       * into the allocated memory and update the total count and memory
       * pointer.
       */
      cur_count = (ULONG)(curmp->b_wptr - curmp->b_rptr);
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy(memp, curmp->b_rptr, cur_count);
#else
      bcopy(curmp->b_rptr, memp, cur_count);
#endif
      tot_count += cur_count;
      memp += cur_count;
   }

   /*
    * The total count should be equal to count.  If not, there
    * was an error retrieving data.  Return NULL.
    */
   if (tot_count != count) {
      _gpio_logmsg(CE_WARN, "GPIO: Error retrieving IOCTL data");
      return NULL;
   }

   return _gpio_datap;
}


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_putioctldata(mp, memp)
 * DESCRIPTION: This function copies data into multiple M_DATA blocks sent
 *            : down with an M_IOCTL from contiguos consolidated region of
 *            : memory.
 *      INPUTS: mp   - pointer to the user request message
 *            : memp - pointer to the data in memory
 *     OUTPUTS: Data from contiguous memory is copied into multiple M_DATA
 *            : blocks.
 *     RETURNS: Nothing.
 *       CALLS: bcopy()
 *   CAUTIONS : None.
 ***************************************************************************/
PRIVATE VOID _gpio_putioctldata(mblk_t *mp, UCHAR *memp)
{
   ULONG    count;
   register mblk_t *curmp;
   register ULONG   cur_count = 0;
   register ULONG   tot_count = 0;


   /*
    * The message must be of the type M_IOCTL.
    */
   if (mp->b_datap->db_type != M_IOCTL) {
      return;
   }

   /*
    * Get the count of bytes to be sent up to user space.
    */
   count = ((IOCBLK *)(mp->b_rptr))->ioc_count;
   if ((count == 0) || (mp->b_cont == NULL)) {
      return;
   }

   /*
    * Copy the data from the allocated memory into the M_DATA blocks
    * successively following the b_cont link.
    */
   for (curmp = mp->b_cont;  curmp != NULL;  curmp = curmp->b_cont) {
      /*
       * Get the count of bytes to be copied into this M_DATA block and
       * copy these bytes from the allocated memory and update the total
       * count and memory pointer.
       */

      cur_count = (ULONG)(curmp->b_wptr - curmp->b_rptr);
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy(curmp->b_rptr, memp, cur_count);
#else
      bcopy(memp, curmp->b_rptr, cur_count);
#endif
      tot_count += cur_count;
      memp += cur_count;
   }

   /*
    * The total count should be equal to count.  If not, there
    * was an error writing data.
    */
   if (tot_count != count) {
      _gpio_logmsg(CE_WARN, "GPIO: Error writing IOCTL data");
   }
}


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_freemsg(mp)
 * DESCRIPTION: This function frees the data associated with a message block,
 *            : sets the M_DATA pointer to NULL and sets the count to 0.
 *      INPUTS: mp - pointer to the user request message
 *     OUTPUTS: Memory is freed.
 *     RETURNS: Nothing.
 *       CALLS: freemsg()
 *   CAUTIONS : None.
 ***************************************************************************/
PRIVATE VOID _gpio_freemsg(mblk_t *mp)
{
   register IOCBLK *iocblkp;


   /*
    * The message must be of the type M_IOCTL.
    */
   if (mp->b_datap->db_type != M_IOCTL) {
      return;
   }

   iocblkp = (IOCBLK *)mp->b_rptr;

   /*
    * If there is no data to be freed, then return.
    */
   if ((iocblkp->ioc_count == 0) || (mp->b_cont == NULL)) {
      return;
   }

   /*
    * Free the M_DATA blocks, 0 the count and set b_cont to NULL.
    */
   freemsg(mp->b_cont);
   iocblkp->ioc_count = 0;
   mp->b_cont = NULL;
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_readport(dip, count)
 * DESCRIPTION: This function reads the I/O port and stores the value in
 *            : the message block to be sent upstream to the user process.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: inb()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_readport(GPIO_DATA *dip, UINT count)
{
   if ((sizeof(ULONG) + sizeof(ULONG) + dip->bufsize) != count) {
      return EINVAL;
   }

   dip->buffer[0] = (UCHAR)inb(dip->address);

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_writeport(dip, count)
 * DESCRIPTION: This function writes the value in the message block buffer
 *            : to the I/O port.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Value written to the I/O port.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: outb()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_writeport(GPIO_DATA *dip, UINT count)
{
   if ((sizeof(ULONG) + sizeof(ULONG) + dip->bufsize) != count) {
      return EINVAL;
   }

   outb(dip->address, dip->buffer[0]);
   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE UCHAR *_gpio_getvirtmem(dip)
 * DESCRIPTION: This function maps the physical address to the virtual
 *            : address.
 *      INPUTS: paddr - Physical Address.
 *            : size  - Amount of physical memory.
 *     OUTPUTS: Nothing.
 *     RETURNS: Virtual address or NULL, if map fails.
 *       CALLS: sptalloc() or physmap()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE UCHAR *_gpio_getvirtmem(ULONG paddr, ULONG size)
{
   UCHAR *vaddr;
#ifndef LFS
   int Pflag = 0;
#endif


   /*
    * Save the offset of the address before calculating the segment.
    * The virtual address will be obtained at the segment boundary
    * since sptalloc operates at page boundaries (4K).  We will add
    * the offset value to size to obtain memory upto the last possible
    * address to be used.
    * ioremap does not really map ISA addresses, but it does check
    * for validity.  So we don't free ISA addresses.
    */
   _gpio_offset = paddr & OFFSET;
   paddr &= 0xFFFFE000;
   size += _gpio_offset;
   if ((vaddr = (UCHAR *)lis_ioremap_nocache((ULONG)paddr, size)) == NULL)
   {  
#ifdef LFS
      _gpio_logmsg(CE_WARN, "GPIO: Cannot map 0x%lx bytes at segment 0x%lx",
                                                                  size, paddr);   
#else
      _gpio_logmsg(CE_WARN, "GPIO: Cannot map 0x%x bytes at segment 0x%x",
                                                                  size, paddr);   
#endif
   }

   vaddr += _gpio_offset;

   return vaddr;
}


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_rlsvirtmem(vaddr, size, paddr)
 * DESCRIPTION: This function releases the virtual address mapped earlier.
 *      INPUTS: vaddr - Virtual address.
 *            : size  - Size of virtual memory.
 *	      : paddr - Physical address  -- Added just for Solaris
 *     OUTPUTS: Nothing.
 *     RETURNS: Nothing.
 *       CALLS: sptfree() or physmap_free()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE VOID _gpio_rlsvirtmem(UCHAR *vaddr, ULONG size, ULONG paddr)
{
  if (paddr & 0xfff00000) { /*Check for PCI, don't need to free ISA*/
      vaddr -= _gpio_offset;
      (void)vfree(vaddr);
  }
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_readmemory(dip, count)
 * DESCRIPTION: This function reads the memory starting at the physical
 *            : address specified in the message block and stores the values
 *            : in the message block buffer to be sent upstream.  The number
 *            : of bytes read is specified in the bufsize field.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: bcopy()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_readmemory(GPIO_DATA *dip, UINT count)
{
   UCHAR *vaddr;

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

   /*
    * Get the virtual memory from the physical address specified in the
    * message packet.
    */
   if ((vaddr = _gpio_getvirtmem(dip->address, dip->bufsize)) == NULL) {
      return ENOMEM;
   }

   /*
    * Copy the requested amount of memory into the message buffer to be
    * sent upstream.
    */
   memcpy_fromio(dip->buffer, vaddr, dip->bufsize);

   /* Release the virtual memory obtained earlier.  */
   _gpio_rlsvirtmem(vaddr, dip->bufsize, dip->address);

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_writememory(dip, count)
 * DESCRIPTION: This function writes into the memory starting at the physical
 *            : address specified in the message block.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Data sent by user process is copied into memory.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: bcopy()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_writememory(GPIO_DATA *dip, UINT count)
{
   UCHAR *vaddr;

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

   /*
    * Get the virtual memory from the physical address specified in the
    * message packet.
    */
   if ((vaddr = _gpio_getvirtmem(dip->address, dip->bufsize)) == NULL) {
      return ENOMEM;
   }

   /* Copy the requested amount from the message buffer into the memory.  */
   memcpy_toio(vaddr, dip->buffer, dip->bufsize);

   /* Release the virtual memory obtained earlier.  */
   _gpio_rlsvirtmem(vaddr, dip->bufsize, dip->address);
   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_mapboard(mdip, count)
 * DESCRIPTION: This function maps in a board.
 *      INPUTS: mdip   - GPIO_MAPDATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Board ID sent by user process is copied into memory.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: bcopy()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_mapboard(GPIO_MAPDATA *mdip, UINT count)
{
   UCHAR *vaddr;

   if (sizeof(GPIO_MAPDATA) != count) {
         return EINVAL;
   }
   /* If the Generic Driver is not stopped */
   if (Gn_Headerp != (GN_HEADER *)NULL) {
      /* Call Map-in function of Generic Driver */      
   } else {      
      /*
       * Get the virtual memory address corresponding to physical 
       * memory (base + 0xE0). (The base physical address is specified 
       * in the message packet.
       */
       if ((vaddr = _gpio_getvirtmem((mdip->address + 0xE0), sizeof(UCHAR))) 
           == NULL) {
          return ENOMEM;
       }
       /* Copy the board id from the message buffer into the memory */
       memcpy_toio(vaddr, &(mdip->id),  sizeof(UCHAR));

       /* Release the virtual memory obtained earlier */
       _gpio_rlsvirtmem(vaddr, sizeof(UCHAR), (mdip->address + 0xE0));
   }

   return 0;
}

/***************************************************************************
 *        NAME: _gpio_boardinterrupthandler(void)
 * DESCRIPTION: This is an interrupt handler for board interrupts (board
 *              interrupts are cascaded though the Generic Driver).
 *      INPUTS: Nothing.
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: 
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE void _gpio_boardinterrupthandler(void)
{
   gpio_intcnt++;           /* increment interrupt count */
       
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_enable interrupts()
 * DESCRIPTION: This function cascades interrupts from the Generic Driver.
 *      INPUTS: Nothing.
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: 
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_enableinterrupts(void)
{
   /* If the Generic Driver is not stopped */
   if (Gn_Headerp != (GN_HEADER *)NULL) {
      /* return error  */
      return -1;
   } else {      
      gpio_intcnt = (USHORT)0;                   /* Reset interrupt count */
      dl_setintr(_gpio_boardinterrupthandler);   /* Cascade GDD interrupts */
   }

   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_disableinterrupts(void)
 * DESCRIPTION: This function "decascades" board interrupts from the Generic
 *              Driver.
 *      INPUTS: Nothing.
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: 
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_disableinterrupts(void)
{

   /* If the Generic Driver is not stopped */
   if (Gn_Headerp != (GN_HEADER *)NULL) {
      /* return error */
      return -1;
   } else {      
      dl_setintr(NULL);   /* Cascade GDD interrupts */
   }
   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_getinterruptcount(intcount, count)
 * DESCRIPTION: This function reads the interrupt count from gpio_intcnt
 *              and stores it in the message block buffer to be sent upstream. 
 *      INPUTS: idip   -  data packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: 
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_getinterruptcount(GPIO_INTDATA *idip, UINT count)
{

   if (sizeof(GPIO_INTDATA) != count) {
      return EINVAL;
   }

   /* Copy the interrupt count into the message buffer to be sent upstream. */
   idip->int_cnt = (USHORT)gpio_intcnt;
   
   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_resetinterruptcount(void)
 * DESCRIPTION: This function resets the interrupt count to zero. 
 *              (gpio_intcnt is reset to zero). 
 *      INPUTS: Nothing.
 *     OUTPUTS: Nothing.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: 
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_resetinterruptcount(void)
{

   /* Reset interrupt count (to zero) */
   gpio_intcnt = (USHORT)0;
   
   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_systime(VOID)
 * DESCRIPTION: Calculate the number of loops for 1 msec/usec busywait delay.
 *            : This routine shall be called only once to setup the
 *            : _gpio_msecloop and _gpio_usecloop variables which are then
 *            : used for calculating msec/usec delays.
 *      INPUTS: None.
 *     OUTPUTS: None.
 *     RETURNS: Nothing.
 *       CALLS: timeout() (SVR3) or drv_getparm().
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE VOID _gpio_systime(VOID)
{
   ULONG lbolt_c, lbolt_n;
   ULONG i;
   ULONG count = 10000;  /* Changed from 100 to 10000 to fix faster systems */

   /* Obtain the number of current clock ticks since the last system rboot  */
  lbolt_c = lbolt();
   /*
    * Wait until the next clock tick for a fresh clock tick boundary.
    * Then set lbolt_c to the new value plus hz/10 clock ticks (100 msec).
    */
   do {
      lbolt_n = lbolt();
   } while (lbolt_c == lbolt_n);

   lbolt_c = lbolt_n + drv_usectohz(1000000)/10;

   /*
    * During this 100 millisecond time-span, until _gpio_loopstate becomes 2
    * or lbolt_n reaches the value stored in lbolt_c, the number of times
    * we count from 1-100 is stored in _gpio_msecloop.
    * Counting from 1 to _gpio_msecloop will cause a delay of 1 msec.
    */
   do {
      lbolt_n = lbolt();
      _gpio_msecloop++;
      for (i = 0; i < count; i++);
   } while (lbolt_c != lbolt_n);

   /*
    * Since counting to _gpio_msecloop causes a 1 msec delay, we can divide
    * the value by 1000 to cause a 1 microsecond delay.
    */
   if ((_gpio_usecloop = _gpio_msecloop / 1000) == 0) {
      _gpio_usecloop++;
   }
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_locateboards(dip, count)
 * DESCRIPTION: This function writes the board locating sequence at the
 *            : appropriate memory locations of D4XE and SPAN boards.  The
 *            : minimum time to locate a board is 65 msec.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Locating sequence to the memory address of D4XE and SPAN boards.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_locateboards(GPIO_DATA *dip, UINT count)
{
   UCHAR *vaddr;
#ifndef LFS
   ULONG  locatetime;
#endif
   ULONG  indx;
   UCHAR  bdid;
#ifndef LFS
   int 	  i,j;
   int	  z_flag;
   DLGCDECLARESPL(oldspl)
#endif
   ULONG lbolt_n;

   volatile UCHAR * addr1;
   volatile UCHAR * addr2;
   volatile UCHAR * addr3;
   volatile UCHAR * addr4;

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

   /*
    * Get the virtual memory from the physical address specified in the
    * message packet.
    */
   if ((vaddr = _gpio_getvirtmem(dip->address, SRAM_CTL_AREA_SIZE)) == NULL) {
      return ENOMEM;
   }
   
   /*
    * In order to locate BLT boards, we need to write a pre-specified
    * at the given address+offset for 65 msec.  Since, we loop within
    * the timed loop for the number of bdids to locate, we will divide
    * locatetime by bufsize which is the number of bdids.  This will
    * prevent us from looping for bdids*65 msec.
    * To ensure that we enter the outer loop atleast once, we add 1 to
    * locatetime, if the integer division result was 0 (highly unlikely).
    */

   addr1 = vaddr + D4XE_LOCATE1;
   addr2 = vaddr + D4XE_LOCATE1;
   addr3 = vaddr + D4XE_LOCATE2;
   addr4 = vaddr + D4XE_LOCATE2;

   /* Delay is added here to make faster systems to locate the boards */
 
   //lbolt_n = (65 * HZ)/1000; /* Convert 65msec to TICKS */
   lbolt_n = drv_usectohz(65000);
   lbolt_n++;
   lbolt_n += lbolt();
 
   /*
    * Interrupts must be enabled here, because we use jiffies
    * which will never change unless timer interrupts are enabled.
    */
    while( lbolt() < lbolt_n) {
      for (indx = 0;  indx < dip->bufsize;  indx++) {
         bdid = dip->buffer[indx] | 0xE0;
 
         writeb(0x64, addr1);
         writeb(0x60, addr2);
         writeb(0x18, addr3);
         writeb(bdid, addr4);
      }
    }

   /* Release the virtual memory obtained earlier.  */
   _gpio_rlsvirtmem(vaddr, SRAM_CTL_AREA_SIZE, dip->address);

   return 0;
}

INT GpLocateSequenceForPci(PUCHAR vaddr);
/***************************************************************************
 *        NAME: PRIVATE INT _gpio_getpcibrdinfo(dip, count)
 * DESCRIPTION: This function get pci board info
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: 
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_getpcibrdinfo(GPIO_DATA *dip, UINT count)
{
   DLGC_PCIBRD_REC ppcirec;
   UCHAR *vaddr, *cfgvaddr, *vaddr1;
   UCHAR brd_id  = 0;

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }
#ifdef PCI_SPAN
   index = dip->buffer[0]; 

   /* Disable the interrupt for Python only */
   if (GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID) {
       if ((cfgvaddr = _gpio_getvirtmem(GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr , 0x80)) == NULL) {
           return ENOMEM;
       }  

       cfgvaddr = cfgvaddr + PCI_PLX_INTR_REG;
       *cfgvaddr = 0x40;
       cfgvaddr = cfgvaddr - PCI_PLX_INTR_REG;

      _gpio_rlsvirtmem(cfgvaddr, 0x80, GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr);
   }

   if ((vaddr = _gpio_getvirtmem(GpControlBlock->GpVirtMapBlock[index].PhysAddr, SRAM_CTL_AREA_SIZE)) == NULL) {
        return ENOMEM;
   }

 if (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID) {
   if ((vaddr1 = _gpio_getvirtmem(GpControlBlock->GpVirtMapBlock[index].PhysAddr+0x2000 , SRAM_CTL_AREA_SIZE)) == NULL) {
        return ENOMEM;
   }

   brd_id = *vaddr1;
   GpControlBlock->BoardAttrs[index].BoardId=((~brd_id & 0x0F) & 0x0F);
   _gpio_rlsvirtmem(vaddr1, SRAM_CTL_AREA_SIZE, GpControlBlock->GpVirtMapBlock[index].PhysAddr+0x2000);
 } else {
   if (GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) {
      brd_id = *(vaddr+0xe0);
      GpControlBlock->BoardAttrs[index].BoardId=((~brd_id & 0x0F) & 0x0F);
   } else
     GpControlBlock->BoardAttrs[index].BoardId=(UCHAR)GpLocateSequenceForPci(vaddr);
 }
   _gpio_rlsvirtmem(vaddr, SRAM_CTL_AREA_SIZE, GpControlBlock->GpVirtMapBlock[index].PhysAddr);

   ppcirec.pbr_brd_id     = GpControlBlock->BoardAttrs[index].BoardId;  
   ppcirec.pbr_type 	  = GpControlBlock->BoardAttrs[index].BoardType >> 16;  
   ppcirec.pbr_slotnum	  = GpControlBlock->BoardAttrs[index].SlotNumber;
   ppcirec.pbr_address	  = GpControlBlock->GpVirtMapBlock[index].PhysAddr;
   ppcirec.pbr_cfgaddress = GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr;
   ppcirec.pbr_irq        = GpControlBlock->IntrBlock[index].Intr_arg;
   /* PCI index start from 0x10 to avoid conflict with ISA */
   ppcirec.pbr_instance   = index+0x10;

   /* Copy the board info from board structure to the message buffer */
#ifdef DLGC_LIS_FLAG
   __wrap_memcpy(dip->buffer, &ppcirec, dip->bufsize);
#else
   bcopy(&ppcirec, dip->buffer, dip->bufsize);
#endif

#endif /* PCI_SPAN */

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_getnumpcibrds(dip, count)
 * DESCRIPTION: This function gets the number of pci boards
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: 
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_getnumpcibrds(GPIO_DATA *dip, UINT count)
{

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

#ifdef PCI_SPAN
   /* Copy the board count from the struct to message buffer */
#ifdef DLGC_LIS_FLAG
    __wrap_memcpy(dip->buffer, &NumOfVoiceBoards, dip->bufsize);
#else
    bcopy(&NumOfVoiceBoards, dip->buffer, dip->bufsize);
#endif
#ifdef XPRT
	cmn_err(CE_WARN, "gpio(0): PADDR = 0x%lx,CADDR = 0x%lx",
                GpControlBlock->GpVirtMapBlock[0].PhysAddr,
                GpControlBlock->GpVirtMapBlock[0].CfgPhysAddr);
	cmn_err(CE_WARN, "gpio(1): PADDR = 0x%lx,CADDR = 0x%lx",
                GpControlBlock->GpVirtMapBlock[1].PhysAddr,
                GpControlBlock->GpVirtMapBlock[1].CfgPhysAddr);
	cmn_err(CE_WARN, "gpio(2): PADDR = 0x%lx,CADDR = 0x%lx",
                GpControlBlock->GpVirtMapBlock[2].PhysAddr,
                GpControlBlock->GpVirtMapBlock[2].CfgPhysAddr);
   cmn_err(CE_NOTE,"_gpio_getnumpcibrds: Num of PCI Boards = %d\n", dip->buffer[0]);
#endif
#endif /* PCI_SPAN */

   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_getpcibrdbusslot(dip, count)
 * DESCRIPTION: This function gets the number of pci boards
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: 
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_getpcibrdbusslot(GPIO_DATA *dip, UINT count)
{

   UINT  bus_slot[2];
   int  indx;

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

#ifdef PCI_SPAN
   indx = dip->buffer[0]; 

   bus_slot[0] = (ULONG)GpControlBlock->BoardAttrs[indx].BusNumber;  
   bus_slot[1] = (ULONG)GpControlBlock->BoardAttrs[indx].SlotNumber;  

/* Copy the board count from the struct to message buffer */
#ifdef DLGC_LIS_FLAG
    __wrap_memcpy(dip->buffer, &bus_slot[0], dip->bufsize);
#else
    bcopy(&bus_slot[0],dip->buffer, dip->bufsize);
#endif
#ifdef XPRT
   cmn_err(CE_NOTE,"_gpio_getpcibrdbusslot: Bus_Num = %d, slot_num = %d\n", dip->buffer[3], dip->buffer[7]);
#endif
#endif /* PCI_SPAN */

   return 0;
}

/***************************************************************************
 *        NAME: PRIVATE INT _gpio_setpcibusnum(dip, count)
 * DESCRIPTION: This function sets the pci bus no.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: 
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_setpcibusnum(GPIO_DATA *dip, UINT count)
{

ULONG bus_num = 0;
ULONG InterfaceType = 0; /* 0 for ISA; 1 for PCI */

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
      return EINVAL;
   }

   bus_num = dip->buffer[0];
   InterfaceType = dip->buffer[1];

#ifdef XPRT 
  cmn_err(CE_NOTE,"_gpio_setpcibusnum: Num of PCI Boards = %d\n", dip->buffer[0]);
#endif

   return 0;
}


/***************************************************************************
 *        NAME: PRIVATE VOID _gpio_busywait(usec)
 * DESCRIPTION: Busywait for 'usec' microseconds.
 *      INPUTS: usec - Number of microseconds to wait.
 *     OUTPUTS: None.
 *     RETURNS: Nothing.
 *       CALLS: drv_usecwait()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE VOID _gpio_busywait(ULONG usec)
{
#ifdef LFS
   drv_usecwait(usec);
#else
   udelay(usec);
#endif
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_writeccmdata(dip, count)
 * DESCRIPTION: This function writes data to the CCM I/O port address
 *            : specified in the message block.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Data sent by user process is copied to CCM I/O port.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: inb() and outb()
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_writeccmdata(GPIO_DATA *dip, UINT count)
{
   ULONG indx, nticks = 0;

   if ((sizeof(ULONG) + sizeof(ULONG) + dip->bufsize) != count) {
      return EINVAL;
   }

   /* We have to write each byte at a time to the CCM I/O port. */
   for (indx = 0;  indx < dip->bufsize;  indx++) {

      /* We wait in a loop as long as the CCM Input Buffer Full bit is set. */
      while ((inb(dip->address) & CCM_IBF) != 0) {
         _gpio_busywait(2);               /* Wait for 2 usec */

         /*
          * If we have already waited our max wait time (10 secs)
          * for Input Buffer to be read, then return an error.
          */
         if (nticks++ >= CCM_WAIT_TICKS) {
            _gpio_logmsg(CE_WARN, "\nGPIO: writeccmdata: CCM_IBF timed out\n");
            return EBUSY;
         }
      }

      /* Write the byte to the CCM I/O port. */
      outb(dip->address, dip->buffer[indx]);
   }
   return 0;
}


/***************************************************************************
 *        NAME: INT GpLocateSequenceForPci(vaddr)
 * DESCRIPTION: This function writes the board locating sequence at the
 *            : appropriate memory locations of PCI boards.  The
 *            : minimum time to locate a board is 65 msec.
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS: Locating sequence to the memory address of PCI boards.
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
INT GpLocateSequenceForPci(vaddr)
   PUCHAR vaddr;
{
   ULONG  locatetime;
   UCHAR  id_num;
#ifndef LFS
   UCHAR  bdid;
#endif
   int 	  j;
#ifndef LFS
   int    z_flag;
#endif
   unsigned int lbolt_n;
#ifndef LFS
   DLGCDECLARESPL(oldspl)
#endif
   volatile UCHAR * addr1;
   volatile UCHAR * addr2;
   volatile UCHAR * addr3;
   volatile UCHAR * addr4;
   volatile UCHAR * addr5;

   addr1 = vaddr + PCI_LOCATE1;
   addr2 = vaddr + PCI_LOCATE1;
   addr3 = vaddr + PCI_LOCATE2;
   addr4 = vaddr + PCI_LOCATE2;
   addr5 = vaddr + PCI_LOCATE2;

   /*
    * LINUX NOTE:  Need to check for timing loops being initialized
    * (gpio_msecloop == 0).  Don't need to do this for Linux,
    * since we use jiffies directly
    */

   for (j = 0; j < 3; j++ ) {  
       /* Delay is added here to make it faster to locate boards */
       lbolt_n = drv_usectohz(65000);
       lbolt_n++;
       lbolt_n += lbolt();
       locatetime = 0;
        /*
         * Interrupts must be enabled here, because we use jiffies
         * which will never change unless timer interrupts are enabled.
         */
        while(lbolt() < lbolt_n) {
             for (id_num = 0;  id_num < GP_MAX_CONFIG_ID_COUNT;  id_num++) {
                 * addr1 =  0x64;
                 * addr2 =  0x60;
                 * addr3 =  0x18;
                 * addr4 =  id_num | 0xE0;
 
              }
        
	      locatetime++;
        }
   }

   for (id_num = 0;  id_num < GP_MAX_CONFIG_ID_COUNT;  id_num++) {
        *addr5 = (UCHAR)id_num;
        writeb(0xAA, vaddr);
 
        if (readb(vaddr) == 0xAA) {
           writeb(0x55, vaddr);
 
           if (readb(vaddr) == 0x55)
              break;
        }

   }
 
   if (id_num >= GP_MAX_CONFIG_ID_COUNT) {
      cmn_err(CE_CONT, "GpLocateSequenceForPci:No PCI board Located!\n");
      return (-1);
   }  

   return ((INT)id_num);
}


/***************************************************************************
 *        NAME: PRIVATE INT _gpio_getboardpowerstatus(dip, count)
 * DESCRIPTION: This function get pci board info
 *      INPUTS: dip   - GPIO_DATA packet pointer
 *            : count - number of total bytes sent to the driver
 *     OUTPUTS:
 *     RETURNS: 0 = success; else error code to be returned to user.
 *       CALLS: Nothing.
 *    CAUTIONS: None.
 ***************************************************************************/
PRIVATE INT _gpio_getboardpowerstatus(GPIO_DATA *dip, UINT count)
{
   POWER_STATUS_STRUCT pow_status;
#ifndef LFS
   UCHAR *vaddr, *cfgvaddr, *vaddr1;
   UCHAR brd_id  = 0;
#endif

   if ((sizeof(long) + sizeof(UINT) + dip->bufsize) != count) {
       return EINVAL;
   }

   index = dip->buffer[0]; 

#if 0
   /* Debug for OAM integration */
   printk("gpio: debug on %d input\n", index);
   switch (index) {
	case 1:
	pow_status.powerGoodRegValue = 1;
	pow_status.powerOverrideRegValue = 0;
	pow_status.actualPowerProvidedValue = 25;
	pow_status.powerRequiredByBoard = 10;
	pow_status.busNumber = 1;
	pow_status.slotNumber =1;
	break;
	case 2:
	pow_status.powerGoodRegValue = 0;
	pow_status.powerOverrideRegValue = 0;
	pow_status.actualPowerProvidedValue = 10;
	pow_status.powerRequiredByBoard = 25;
	pow_status.busNumber = 1;
	pow_status.slotNumber =1;
	break;
	case 3:
	pow_status.powerGoodRegValue = 0;
	pow_status.powerOverrideRegValue = 1;
	pow_status.actualPowerProvidedValue = 25;
	pow_status.powerRequiredByBoard = 10;
	pow_status.busNumber = 1;
	pow_status.slotNumber =1;
	break;
	case 4:
	pow_status.powerGoodRegValue = 1;
	pow_status.powerOverrideRegValue = 1;
	pow_status.actualPowerProvidedValue = 10;
	pow_status.powerRequiredByBoard = 25;
	pow_status.busNumber = 1;
	pow_status.slotNumber =1;
	break;
	default:
	pow_status.powerGoodRegValue = 1;
	pow_status.powerOverrideRegValue = 0;
	pow_status.actualPowerProvidedValue = 25;
	pow_status.powerRequiredByBoard = 10;
	pow_status.busNumber = 1;
	pow_status.slotNumber =1;
	break;
   }

#ifdef DLGC_LIS_FLAG
   __wrap_memcpy(dip->buffer, &pow_status, dip->bufsize);
#else
   bcopy(&pow_status, dip->buffer, dip->bufsize);
#endif

#endif

   /* Check for PCIe board */
   if (!GpControlBlock->BoardAttrs[index].TypePCIExpress) {
       return ENOMEM;
   }

   pow_status.boardIndex = index;
   pow_status.powerGoodRegValue = GET_POWER_GOOD_REG(index);
   pow_status.powerOverrideRegValue = GET_POWER_OVERRIDE(index);
   pow_status.actualPowerProvidedValue = GET_POWER_PROVIDED(index);
   pow_status.powerRequiredByBoard = GET_POWER_REQUIRED(index);
   pow_status.busNumber = GpControlBlock->BoardAttrs[index].BusNumber;
   pow_status.slotNumber= GpControlBlock->BoardAttrs[index].SlotNumber;

#ifdef DLGC_LIS_FLAG
   __wrap_memcpy(dip->buffer, &pow_status, dip->bufsize);
#else
   bcopy(&pow_status, dip->buffer, dip->bufsize);
#endif

   return 0;
}
