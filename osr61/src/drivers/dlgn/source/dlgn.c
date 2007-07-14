/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgn.c
 * Description                  : Springware Generic Device Driver
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
#define _SVR4_SOURCE	1
#include <sys/os7/compat.h>
#include <stdarg.h>
#else
#include <sys/LiS/config.h>
#if (CLONE__CMAJOR_0 > 220)
#define DLGC_LIS_FLAG
#include <sys/LiS/module.h>
#endif
#endif

#include <sys/ddi.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <sys/stream.h>
#include <sys/cmn_err.h>
#ifndef LFS
#include <sys/lislocks.h>
#if LINUX_VERSION_CODE > 0x020400
#include <sys/lispci.h>
#include <sys/osif.h>
#else
#include <linux/pci.h>
#endif
#endif
#if LINUX_VERSION_CODE > 0x020600
#define LINUX26
#include <linux/moduleparam.h>
#else
#ifndef IRQ_NONE
typedef void irqreturn_t;
#define IRQ_NONE
#define IRQ_HANDLED
#define IRQ_RETVAL(x)
#endif
#endif
#define  DPCI_H

#include "dlgnextern.h"
#include "gndefs.h"
#include "gnlibdrv.h"
#include "gnmsg.h"
#include "gndrv.h"
#include "dlgcos.h"

#include "cfd.h"
#include "gnlogmsg.h"
#include "gndbg.h"
#include "gncfd.h"
#include "gndlgn.h"
#include "typedefs.h"
#include "cfd2dlgn.h"
#include "dlgn2cfd.h"
#include "dlgn.h"
#include "pmswtbl.h"
#include "drvdebug.h"

int dlgndevflag = 0;

#include "dpci.h"

#define PCI_BASE              0x10
#define GN_NOSETIDLE          0x0000
#define GN_SETIDLE            0x0001

char dlgn_version[] = "x.01";

#ifdef PCI_SPAN
int dlgn_int_hit = 0;

/* At present, gpio is not used for Antares boards */
UCHAR NumOfAllBoards = 0; 	/* all brds */
UCHAR NumOfVoiceBoards = 0;     /* span brds */
UCHAR NumOfAntaresBoards = 0;   /* antares brds */ 

int index = 0;
int in_intr_srvc = 0;
int Single_Board_SS = 0;

GP_CONTROL_BLOCK        GpControlBlock_ = { 0 };
PGP_CONTROL_BLOCK       GpControlBlock=&GpControlBlock_;

int brd = 0;
int sbavec = 0;
int antvec = 0;
int sramvec = 0;
int in_intr = 0;
int err_once = 0;
#ifdef LFS
spinlock_t intcmd_lock = SPIN_LOCK_UNLOCKED;	/* mutex for add/rmv of commands */
spinlock_t intdrv_lock = SPIN_LOCK_UNLOCKED;	/* mutex for soft intr and drivers */
spinlock_t intmod_lock = SPIN_LOCK_UNLOCKED;	/* mutex for soft intr and modules */
spinlock_t inthw_lock =  SPIN_LOCK_UNLOCKED;	/* mutex for Span hardware interrupts */
spinlock_t st_sctlock =  SPIN_LOCK_UNLOCKED;
spinlock_t st_uselock =  SPIN_LOCK_UNLOCKED;
spinlock_t st_freelock = SPIN_LOCK_UNLOCKED;
#else
lis_spin_lock_t intcmd_lock = { 0 };	/* mutex for add/rmv of commands */
lis_spin_lock_t intdrv_lock = { 0 };	/* mutex for soft intr and drivers */
lis_spin_lock_t intmod_lock = { 0 };	/* mutex for soft intr and modules */
lis_spin_lock_t inthw_lock =  { 0 };	/* mutex for Span hardware interrupts */
lis_spin_lock_t st_sctlock =  { 0 };
lis_spin_lock_t st_uselock =  { 0 };
lis_spin_lock_t st_freelock = { 0 };
#endif

/* Support for more major devices */
int DLGN_MAX_MAJOR_DEV = 1;
int DLGN_MAX_OPEN_COUNT = 256;
#ifdef LINUX26
int dlgnMajor = 0;
#else
int dlgnMajorTable[4];
#endif
#endif /* PCI_SPAN */

#ifdef DLGN_DEBUG
unsigned short dlgn_debug = 0x08;
#endif /* DLGN_DEBUG */


/*
 * Module STREAMS functions
 */
#ifdef LFS
void dlgninit(void);
int dlgnstart(void);
#else
void dlgninit();
int dlgnstart();
#endif

#ifdef LFS
extern int streamscall dlgnopen(queue_t *, dev_t *, int, int, cred_t *);
extern int streamscall dlgnclose(queue_t*, int, cred_t *);
extern int streamscall dlgnwput(queue_t *, mblk_t *);      /* Write Side Put routine */
extern int streamscall dlgnrsrv(queue_t *);                /* Read Side Service routine. */
extern int streamscall dlgnwsrv(queue_t *);                /* Read Side Service routine. */
#else
extern int dlgnopen(queue_t *, dev_t *, int, int, cred_t *);
extern int dlgnclose(queue_t*, int, cred_t *);
extern int dlgnwput(queue_t *, mblk_t *);      /* Write Side Put routine */
extern int dlgnrsrv(queue_t *);                /* Read Side Service routine. */
extern int dlgnwsrv(queue_t *);                /* Read Side Service routine. */
#endif
irqreturn_t dlgnintr(int vecnum, void *dev_id, struct pt_regs * regs);

/*
 * Module PRIVATE functions
 */
#ifdef LFS
PRIVATE void _dlgn_addcmd(GN_LOGDEV *, mblk_t *);
PRIVATE void _dlgn_rmvcmd(GN_LOGDEV *, int, int);
PRIVATE void _dlgn_putcmd(GN_LOGDEV *, mblk_t *, queue_t *);
PRIVATE int  _dlgn_procevt(mblk_t *);
PRIVATE int  _dlgn_sendevt(unsigned short, mblk_t *);
PRIVATE int  _dlgn_sendpm(mblk_t *);
PRIVATE void _dlgn_findpeak(void);
PRIVATE void _dlgn_flush(queue_t *, mblk_t *);
PRIVATE void _dlgn_control(GN_CMDMSG *, SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_unbind(SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_bind(queue_t *, mblk_t *);
PRIVATE void _dlgn_setevmask(SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_getevmask(SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_setidlestate(GN_CMDMSG *, SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_getstate(GN_CMDMSG *, SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_ipcsndevt(GN_CMDMSG *, SM_HANDLE *, queue_t *, mblk_t *);
PRIVATE void _dlgn_systime(void);
PRIVATE void _dlgn_timerset(void);
#ifdef NOSEARCH
PRIVATE void _dlgn_addldsmhp(void);
PRIVATE void _dlgn_rmvldsmhp(void);
#endif
#else
PRIVATE void _dlgn_addcmd();
PRIVATE void _dlgn_rmvcmd();
PRIVATE void _dlgn_putcmd();
PRIVATE int  _dlgn_procevt();
PRIVATE int  _dlgn_sendevt();
PRIVATE int  _dlgn_sendpm();
PRIVATE void _dlgn_findpeak();
PRIVATE void _dlgn_flush();
PRIVATE void _dlgn_control();
PRIVATE void _dlgn_unbind();
PRIVATE void _dlgn_bind();
PRIVATE void _dlgn_setevmask();
PRIVATE void _dlgn_getevmask();
PRIVATE void _dlgn_setidlestate();
PRIVATE void _dlgn_getstate();
PRIVATE void _dlgn_ipcsndevt();
PRIVATE void _dlgn_systime();
PRIVATE void _dlgn_timerset();
#ifdef NOSEARCH
PRIVATE void _dlgn_addldsmhp();
PRIVATE void _dlgn_rmvldsmhp();
#endif
#endif

/*
 * Set the low/high water marks to allow only one message to be queued
 * for the write-side service routine at a time.
 */
#define LO_WATER        1
#define HI_WATER        64 * 1024
#define MIN_PSZ         0
#define MAX_PSZ         64 * 1024

/* 
 * Read/Write module_info structures - These structures contains name 
 * and limit values. NOTE: If both sides are identical they need NOT
 * be declared twice.
 */

/* Read Side Module Information */
PRIVATE struct module_info rminfo = {
   DLGN_ID,       /* Module ID. Used for logging */
   "dlgn",        /* Module name. Same as that used in mdevice */
   0,             /* Minimum packet size. */
   0,             /* Maximum packet size. */
   0,             /* High water mark. Used in flow control. */
   0              /* Low water mark. Ditto. */
};


/* Write Side Module Information */
PRIVATE struct module_info wminfo = {
   DLGN_ID,       /* Module ID. Used for logging */
   "dlgn",        /* Module name. Same as that used in mdevice */
   MIN_PSZ,       /* Minimum packet size. */
   MAX_PSZ,       /* Maximum packet size. */
   HI_WATER,      /* High water mark. Used in flow control. */
   LO_WATER       /* Low water mark. Ditto. */
};


/*
 * Read/Write qinit structures - These structures define the module
 */

/* Read Side Queue Information */
static struct qinit rinit = {
   NULL,          /* Pointer to Put routine. */
   dlgnrsrv,      /* Pointer to Service routine. */
   dlgnopen,      /* Pointer to Open routine. */
   dlgnclose,     /* Pointer to Close routine. */
   NULL,          /* Reserved by STREAMS for future use. */
   &rminfo,       /* Pointer to module_info structure. */
   NULL           /* Pointer to optional statistics structure. */
};


/* Write Side Queue Information */
static struct qinit winit = {
   dlgnwput,      /* Pointer to Put routine. */
   dlgnwsrv,      /* Pointer to Service routine. */
   NULL,          /* Pointer to Open routine. */
   NULL,          /* Pointer to Close routine. */
   NULL,          /* Reserved by STREAMS for future use. */
   &wminfo,       /* Pointer to module_info structure. */
   NULL           /* Pointer to optional statistics structure. */
};


/*
 * Streamtab entry - Used in cdevsw and fmodsw to point to the driver
 * or module. This is typically the only public structure in a STREAMS
 * driver.
 */
static struct streamtab dlgninfo = {
   &rinit,        /* Pointer to Read Queue Init. Structure. */
   &winit,        /* Pointer to Write Queue Init. Structure. */
   NULL,          /* Multiplexer Read Queue Init. Structure. */
   NULL           /* Multiplexer Write Queue Init. Structure.*/
};


/*
 * Generic Driver internal data structures
 */
PRIVATE SM_STATE  Sm_State = {0};
#ifdef LFS
PRIVATE SM_HANDLE Sm_Handle[GN_MAXHANDLES]  = {{0,},};
#else
PRIVATE SM_HANDLE Sm_Handle[GN_MAXHANDLES]  = {0};
#endif

#ifdef LFS
PRIVATE GN_BOARD  *Gn_Irqlist[GN_MAXIRQ] = {NULL,};
#else
PRIVATE GN_BOARD  *Gn_Irqlist[GN_MAXIRQ] = {0};
#endif
typedef struct {
   unsigned short       irq;
   unsigned short       index;
   unsigned short       boardid;
   unsigned int         bustype;
   unsigned char        *virtcfg;
   unsigned char        pci_bus;
   unsigned char        pci_dev;
   unsigned char        pci_func;
} dlgninst_t ;
GLOBAL dlgninst_t *dlgn_instance[GN_MAXIRQ] = {0};	/* temporary idata */


/* Array of user specified options for ISA adapters. */
int ft_dl =  -1;
int ft_pc =  -1;
static int blt_irq = 0;
static int sba_irq = 0;
static int blt_address = 0;
static int sba_address = 0;
static int r4_feature  = 1;
static int dlgn_opencount = 1;
#ifdef LINUX26
module_param(blt_irq,int,0444);
module_param(sba_irq,int,0444);
module_param(ft_dl, int,0444);
module_param(ft_pc, int,0444);
module_param(blt_address,int,0444);
module_param(sba_address,int,0444);
module_param(r4_feature, int,0444);
module_param(dlgn_opencount, int,0444);
#else
MODULE_PARM(blt_irq,"i");
MODULE_PARM(sba_irq,"i");
MODULE_PARM(ft_dl, "i");
MODULE_PARM(ft_pc, "i");
MODULE_PARM(blt_address,"i");
MODULE_PARM(sba_address,"i");
MODULE_PARM(r4_feature, "i");
MODULE_PARM(dlgn_opencount, "i");
#endif
MODULE_AUTHOR("Intel");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Dialogic Generic Device Driver");

#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("dlgn");
MODULE_ALIAS("streams-dlgn");
MODULE_ALIAS("streams-dlgnDriver");
#endif
#endif


#ifdef _ISDN
/*
 * Global function used to keep Handle data structures private.
 */
#ifdef LFS
GN_LOGDEV *dlgn_getldp(register int);
#else
GN_LOGDEV *dlgn_getldp();
#endif
#endif /* _ISDN */


/*
 * Global data included for backward compatability
 */
#ifdef ISA_SPAN
#ifdef LFS
PRIVATE int (*cascade_intr)(void) = NULL;
#else
PRIVATE int (*cascade_intr)() = NULL;
#endif
unsigned short cascade_eoi = 0;
#endif


/*
 *  Public Global data structures moved from cfd.c to remove
 *  cyclic dependency between dlgn and cfd.
 */
 
/*
 * PUBLIC global variables
 */
int            Gn_Msglvl   = (LF_CONT | LF_NOTE | LF_WARN | LF_PANIC);
unsigned long  Gn_Numpms   = 0;     /* Total # of PM's in system */
unsigned long  Gn_Memsz    = 0;     /* Total memory alloc'd for structs */
GN_HEADER      *Gn_Headerp = NULL;  /* Base of GN_HEADER array */
GN_BOARD       *Gn_Boardp  = NULL;  /* Base of GN_BOARD array */
GN_LOGDEV      *Gn_Logdevp = NULL;  /* Base of GN_LOGDEV array */
char           *Gn_Freep   = NULL;  /* Base of dev-dep data area */
char           *Gn_Endp    = NULL;  /* End of driver's alloc'd memory */
struct map *gn_map = { 0 };

#define STRACE0(x)
#define STRACE1(x, one)	
#define STRACE2(x, one, two)
#define STRACE3(x, one, two, three)

#ifdef _STATS_
int StatisticsMonitoring = 0;
SRAM_DRV_STATISTICS DriverStats = { 0 };
#endif

/* Hard coding the switch table right in here. */

/**********************************************************************
*	This table has been hard-coded into the driver package because
*	the kernel is not rebuilt during installation. In other systems
*	since the kernel was being rebuilt, a buildspace utility was 
*	called that would create a Space.c file that would be linked
*	with the drivers during idbuild. Since this is not possible
*	with Solaris, a static data structure is created (in this
*	file) and is linked with dlgn. The same strategy as was used
*	in 3.01prod is being used.
**********************************************************************/

#ifdef LFS
#define PM_NULL  (NULL)
#else
#define PM_NULL  (int (*)()) 0
#endif

   /*
    * Create the external references for the compiler
    */

#ifdef LFS
extern int sramstartpm(GN_BOARD *, char *, ULONG, ULONG, ULONG);
extern int sramstoppm(GN_BOARD *, ULONG);
extern int sramstartbd(GN_BOARD *);
extern int sramstopbd(GN_BOARD *);
extern int sramopen(GN_LOGDEV *);
extern int sramclose(GN_LOGDEV *, ULONG);
extern int sramsendcmd(GN_LOGDEV *, mblk_t *);
extern int sramintr(int);
extern int srammemsz(ULONG *, ULONG *, char **);
extern int sramdebug(GN_DEBUG *, int *);
#ifdef _STATS_
extern int sramstats(PM_STATS *);
#endif /* _STATS_ */
#else
extern int sramstartpm();
extern int sramstoppm();
extern int sramstartbd();
extern int sramstopbd();
extern int sramopen();
extern int sramclose();
extern int sramsendcmd();
extern int sramintr();
extern int srammemsz();
extern int sramdebug();
#ifdef _STATS_
extern int sramstats();
#endif /* _STATS_ */
#endif

/* Antares protocol module entry points. */
#ifdef ANTARES
extern int anstartpm();
extern int anstoppm();
extern int anstartbd();
extern int anstopbd();
extern int anopen();
extern int anclose();
extern int ansendcmd();
extern int anintr();
extern int anmemsz();
extern int andebug();
#ifdef _STATS_
extern int anstats();
#endif /* _STATS_ */
#endif

  /*
   * Create the actual switch table
   */

PM_SWTBL pmswtbl[] = {
{sramstartpm, sramstoppm,  sramstartbd, sramstopbd,  sramopen,
sramclose,    sramsendcmd, sramintr,    srammemsz,   
#ifdef _STATS_
sramdebug,  sramstats },
#else
sramdebug },
#endif /* _STATS_ */
#ifdef ANTARES
{anstartpm, anstoppm,  anstartbd, anstopbd,  anopen,
 anclose,   ansendcmd, anintr,    anmemsz,   
#ifdef _STATS_
andebug,  anstats },
#else
andebug },
#endif /* _STATS_ */
#endif
{PM_NULL, PM_NULL, PM_NULL, PM_NULL, PM_NULL,
 PM_NULL, PM_NULL, PM_NULL, PM_NULL, 
#ifdef _STATS_
PM_NULL, PM_NULL },
#else
 PM_NULL },
#endif /* _STATS_ */
};

#ifdef ANTARES
#define	SOL_NUM_PMS   ((sizeof(pmswtbl)/sizeof(PM_SWTBL))-1)
#else
#define SOL_NUM_PMS   1
#endif /* ANTARES */

unsigned long Gn_Maxpm = SOL_NUM_PMS;

int init_module(void)
{
   int ret;
   int i;

#ifndef LINUX26
   if ((dlgn_opencount > 1) && (dlgn_opencount < 5)) {
       DLGN_MAX_MAJOR_DEV =  dlgn_opencount;
       DLGN_MAX_OPEN_COUNT = DLGN_MAX_OPEN_COUNT * dlgn_opencount;
   }

   i = DLGN_MAX_MAJOR_DEV;
#else
   i = 0;
#endif

   Sm_State.st_count = DLGN_MAX_OPEN_COUNT;

#ifdef LFS
   cmn_err(CE_NOTE, "\n");            /* Formatting Reason */
#else
   lis_printk("\n");            /* Formatting Reason */
#endif

   do {
     ret = lis_register_strdev(0, &dlgninfo, Sm_State.st_count, LIS_OBJNAME_STR);

     if (ret < 0) {
#ifdef LFS
         cmn_err(CE_WARN, "%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
#else
         lis_printk("%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
#endif
         return ret;
     }
#ifndef LINUX26
     dlgnMajorTable[--i] = ret;
#else
     dlgnMajor = ret;
#endif
   } while (i > 0);


   dlgninit();

   /* Initialization of Spinlocks */
   DLGCSPINLOCKINIT(inthw_lock,  "hw_lock");
   DLGCSPINLOCKINIT(intmod_lock, "mod_lock");
   DLGCSPINLOCKINIT(intdrv_lock, "drv_lock");
   DLGCSPINLOCKINIT(intcmd_lock, "cmd_lock");
   DLGCSPINLOCKINIT(st_sctlock,  "sct_lock");
   DLGCSPINLOCKINIT(st_uselock,  "use_lock");
   DLGCSPINLOCKINIT(st_freelock, "free_lock");

   return 0;
}

void cleanup_module(void) 
{
   int i;
   dlgninst_t *devinst;

   /* Free up per board structures and unregister interrupts. */
   for (brd=0; brd < GN_MAXIRQ; ++brd) {
        if ((devinst = dlgn_instance[brd])) {
#ifdef LFS
            cmn_err(CE_NOTE, "DLGN: Freeing brd %d\n", brd);
            free_irq(devinst->irq, devinst);
            if (dlgn_instance[brd]->virtcfg != (UCHAR *)NULL)
                vfree(dlgn_instance[brd]->virtcfg);
            kfree(devinst);
#else
            lis_printk("DLGN: Freeing brd %d\n", brd);
            lis_free_irq(devinst->irq, devinst);
            if (dlgn_instance[brd]->virtcfg != (UCHAR *)NULL)
                lis_vfree(dlgn_instance[brd]->virtcfg);
            lis_kfree(devinst);
#endif
            dlgn_instance[brd] = 0;
        }
   }

#ifndef LINUX26
   for (i=0; i<DLGN_MAX_MAJOR_DEV; i++) {
        if (lis_unregister_strdev(dlgnMajorTable[i]) < 0) {
#ifdef LFS
            cmn_err(CE_WARN, "%s: Unable to unregister module.\n", LIS_OBJNAME_STR);
#else
            lis_printk("%s: Unable to unregister module.\n", LIS_OBJNAME_STR);
#endif
        } else {
#ifdef LFS
            cmn_err(CE_NOTE, "%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
#else
            lis_printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
#endif
        }
   }
#else
#ifdef LFS
   (void) i;
#endif
   if (lis_unregister_strdev(dlgnMajor) < 0) {
#ifdef LFS
       cmn_err(CE_WARN, "%s: Unable to unregister module.\n", LIS_OBJNAME_STR);
#else
       lis_printk("%s: Unable to unregister module.\n", LIS_OBJNAME_STR);
#endif
   } else {
#ifdef LFS
       cmn_err(CE_NOTE, "%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
#else
       lis_printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
#endif
   }
#endif

   return;
}      /* end of cleanup_module() */


/*
 *  Public functions originally exported from cfd.c
 */
 
/*
 *	Note that the scope of cf_cmpstr has been made global.
 *	Due to the shuffling of functions both dlgn and cfd drivers
 *	use it currently.
 */

/***********************************************************************
 *        NAME: cf_cmpstr
 * DESCRIPTION: Compares strings up to bufsz bytes in length (including
 *              NULL terminator)
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:  0 if strings identical & <= bufsz in length
 *              -1 if strings not identical or too long
 *    CAUTIONS: Only give null pointers if you WANT to panic the system
 **********************************************************************/
#ifdef LFS
int cf_cmpstr(register unsigned char *sp, register unsigned char *dp, unsigned int bufsz)
#else
int cf_cmpstr(sp,dp,bufsz)
register unsigned char *sp;
register unsigned char *dp;
unsigned int          bufsz;
#endif
{
   register unsigned char *ep = dp + bufsz;
 
   while (*sp) {
      if ((dp >= ep) || (*dp++ != *sp++)) {
         return (-1);
      }
   }
   return ((*dp == *sp) ? 0 : -1);
}
 
/***********************************************************************
 *                   PUBLICLY AVAILABLE FUNCTIONS
 **********************************************************************/

 
/***********************************************************************
 *        NAME: cf_name2ldp
 * DESCRIPTION: Given a LOGDEV name, find the ldp for that name. Will
 *              search thru every LOGDEV on every BOARD on every PM
 *              until a match is found.
 *      INPUTS: namep - ptr to the name string to be matched.
 *     OUTPUTS: none
 *     RETURNS: returns the ldp if found, otherwise returns NULL.
 *       CALLS:
 **********************************************************************/
GN_LOGDEV *cf_name2ldp(namep)
char *namep;
{
   register GN_BOARD    *bdp;
   register GN_LOGDEV   *ldp;
   register GN_HEADER   *ghp;
   unsigned long        pmid;
 
   /* Loop thru every PM */
   for (pmid = 0; pmid < Gn_Numpms; pmid++) {
 
      /* Point to the GN_HEADER area for this PM */
      ghp = Gn_Headerp + pmid;
 
      /* If the PM is not started, skip it */
      if (!(ghp->gh_flags & PM_PMSTART)) {
         continue;
      }
 
      /* Loop thru every GN_BOARD on this PM */
      for (bdp = ghp->gh_gbbasep; bdp < ghp->gh_gbfreep; bdp++) {
         /* If the BOARD is not started, skip it */
         if (!(bdp->bd_flags & BD_START)) {
            continue;
         }
 
         /* Loop thru every GN_LOGDEV on this board */
         for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
 
            /* Check the name for a match */
            if (cf_cmpstr(namep, ldp->ld_name, LD_NAMELEN) == 0) {
               return (ldp);
            }
         }
      }
   }
 
   /* Not found, return NULL */
   return ((GN_LOGDEV *) NULL);
}
 
 
/***********************************************************************
 *        NAME: cf_ioccopy
 * DESCRIPTION: Copies multiblock STREAMS ioctl messages into a single
 *              contiguous block of memory.
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS: Ptr to contiguous message is success; NULL if fails.
 *    CAUTIONS: Memory segment MUST be freed before ioctl call returns.
 **********************************************************************/
char *cf_ioccopy(mp)
mblk_t *mp;
{
   register mblk_t   *nmp;
   register char     *curptr;
   register int  _ioc_count;
   register int      cur_count = 0;
   char              *baseptr;
   int               tot_count = 0;
 

   if (mp->b_datap->db_type != M_IOCTL) {
      return (NULL);
   }
 
   _ioc_count = ((struct iocblk *)(mp->b_rptr))->ioc_count;
 
   if ((_ioc_count == 0) || (mp->b_cont == NULL)) {
      return (NULL);
   }
 
#ifdef LFS
   /* Use KM_NOSLEEP instead of KM_SLEEP because this function is called from
    * a write put procedure. --bb */
   if ((curptr = baseptr = (char *)kmem_alloc(_ioc_count, KM_NOSLEEP)) == NULL){
      return (NULL);
   }
#else
   /* This is an error. This function is called from a put procedure and must
    * not block.  This should be GFP_ATOMIC instead of GFP_KERNEL. --bb */
   if ((curptr = baseptr = (char *)lis_kmalloc(_ioc_count, GFP_KERNEL)) == NULL){
      return (NULL);
   }
#endif

   for (nmp = mp->b_cont; nmp != NULL; nmp = nmp->b_cont) {
      cur_count = (int)(nmp->b_wptr - nmp->b_rptr);
#ifdef LFS
      /* This was dangerous because bytes were blidly copied without checking
       * if _ioc_count was less than the extent being copied.  This also
       * causes the function to succeed if there are more bytes available than
       * _ioc_count. --bb */
      if (_ioc_count < tot_count + cur_count)
	      cur_count = tot_count - _ioc_count;
#endif
#ifdef DLGC_LIS_FLAG
      __wrap_memcpy(curptr, nmp->b_rptr, cur_count);
#else
      bcopy(nmp->b_rptr, curptr, cur_count);
#endif
      tot_count += cur_count;
      curptr += cur_count;
   }
 
   if (tot_count != _ioc_count) {
#ifdef LFS
      /* Memory leak. --bb */
      kmem_free(baseptr, _ioc_count);
#endif
      return (NULL);
   }

   return (baseptr);
}
 
 
/***********************************************************************
 *        NAME: cf_iocput
 * DESCRIPTION: Copies buffer into multipart STREAMS ioctl message.
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
void cf_iocput(mp,gdp,msgsz)
mblk_t *mp;
GN_DEBUG *gdp;
int msgsz;
{
   char *bufp = (char *)gdp;
   struct iocblk *iocp = (struct iocblk *) mp->b_datap->db_base;
   mblk_t *nmp;
   int bytecnt;
   int nbytes;
 
   if (iocp->ioc_count < msgsz) {
      msgsz = iocp->ioc_count;
   }
   bytecnt = msgsz;
 
   for (nmp = mp->b_cont; bytecnt > 0; nmp = nmp->b_cont) {
      if (nmp == NULL) break;
 
      if ((nbytes = (nmp->b_datap->db_lim - nmp->b_datap->db_base)) > bytecnt) {
         nbytes = bytecnt;
      }

#ifdef DLGC_LIS_FLAG
      __wrap_memcpy(nmp->b_rptr = nmp->b_datap->db_base,bufp,nbytes);
#else 
      bcopy(bufp,nmp->b_rptr = nmp->b_datap->db_base,nbytes);
#endif
      bytecnt -= nbytes;
      bufp += nbytes;
      nmp->b_wptr = nmp->b_rptr + nbytes;
   }
}
 
 
/***********************************************************************
 *        NAME: dlgn_msg
 * DESCRIPTION:
 *      INPUTS:
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
void
dlgn_msg(int lvl, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	switch (lvl) {
	case CE_CONT:
		if (Gn_Msglvl & LF_CONT)
			vcmn_err(lvl, fmt, args);
		break;
	case CE_NOTE:
		if (Gn_Msglvl & LF_NOTE)
			vcmn_err(lvl, fmt, args);
		break;
	case CE_WARN:
		if (Gn_Msglvl & LF_WARN)
			vcmn_err(lvl, fmt, args);
		break;
	case CE_PANIC:
		if (Gn_Msglvl & LF_PANIC)
			vcmn_err(lvl, fmt, args);
		break;
#ifdef DBGPRT
	case CE_DEBUG:
		if (Gn_Msglvl & LF_DEBUG)
			vcmn_err(lvl, fmt, args);
		break;
#endif
	default:
		if (Gn_Msglvl & LF_HASBUG)
			vcmn_err(lvl, fmt, args);
		break;
	}
	va_end(args);
	return;
}
#else
void dlgn_msg(lvl,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)
int   lvl;
char  *fmt;
unsigned long arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9;
{
   switch (lvl) {
   case CE_CONT:
      if (Gn_Msglvl & LF_CONT) {
         cmn_err(lvl,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
      }
      break;
   case CE_NOTE:
      if (Gn_Msglvl & LF_NOTE) {
         cmn_err(lvl,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
      }
      break;
   case CE_WARN:
      if (Gn_Msglvl & LF_WARN) {
         cmn_err(lvl,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
      }
      break;
   case CE_PANIC:
      if (Gn_Msglvl & LF_PANIC) {
         cmn_err(lvl,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
      } else {
         cmn_err(CE_PANIC," ");
      }
      break;
#ifdef DBGPRT
   case CE_DEBUG:
      if (Gn_Msglvl & LF_DEBUG) {
         cmn_err(CE_CONT,fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
      }
      break;
#endif
   default:
      if (!(Gn_Msglvl & LF_HASBUG)) {
         cmn_err(CE_WARN,"Invalid message level: %d\n", lvl);
      }
      break;
   }
}
#endif
 

/***********************************************************************
 * NAME: linux_config_blt_device
 *
 * 1. Initialize structures that hold pci card information.
 * 2. Allocate and initialize interrupt structure.
 * 3. Bind interrupt structure and interrupt routine to interrupt number
 ***********************************************************************/
#ifdef LFS
int linux_config_blt_device(void)
#else
linux_config_blt_device()
#endif
{
   dlgninst_t *devinst;

   /* allocate per instance memory here */
#ifdef LFS
   if (!(devinst=(dlgninst_t*)kmem_alloc((sizeof(dlgninst_t)), KM_SLEEP))) {
      cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
      return(ENOMEM);
   }
#else
   if (!(devinst=(dlgninst_t*)lis_kmalloc((sizeof(dlgninst_t)), GFP_KERNEL))) {
      cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
      return(ENOMEM);
   }
#endif
   bzero((char *)devinst, sizeof(dlgninst_t));

   devinst->bustype = BUS_TYPE_ISA;
   devinst->index = blt_irq;
   devinst->irq = blt_irq;
   dlgn_instance[brd] = devinst;

   /*
    * SA_INTERRUPT when there are lots of plays happening simultaneously,
    * interrupt latency is too high causing boards to play repeated data.
    */  
#ifdef LFS
   if (request_irq(devinst->irq, (void *)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnblt", devinst ) < 0 ) {
       cmn_err(CE_WARN, "dlgnconfig: request_irq failed for blt=%d\n", devinst->irq);
       kmem_free(devinst, sizeof(*devinst));
       dlgn_instance[brd] = NULL;
   }
   brd++;		/* for SBA if any */
   return (0);
#else
   if (lis_request_irq(devinst->irq, (void *)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnblt", devinst ) < 0 ) {
       lis_printk("dlgnconfig: request_irq failed for blt=%d\n", devinst->irq);
       lis_kfree(devinst);
       dlgn_instance[brd] = NULL;
   }
   brd++;		/* for SBA if any */
#endif
}

/***********************************************************************
 * NAME: linux_config_sba_device
 *
 * 1. Initialize structures that hold pci card information.
 * 2. Allocate and initialize interrupt structure.
 * 3. Bind interrupt structure and interrupt routine to interrupt number
 ***********************************************************************/
#ifdef LFS
int linux_config_sba_device(void) 
#else
linux_config_sba_device() 
#endif
{
   dlgninst_t *devinst;

   /* allocate per instance memory here */
#ifdef LFS
   if (!(devinst=(dlgninst_t*)kmem_alloc((sizeof(dlgninst_t)), KM_SLEEP))) {
       cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
       return(ENOMEM);
   }
#else
   if (!(devinst=(dlgninst_t*)lis_kmalloc((sizeof(dlgninst_t)), GFP_KERNEL))) {
       cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
       return(ENOMEM);
   }
#endif
   bzero((char *)devinst, sizeof(dlgninst_t));

   devinst->bustype = BUS_TYPE_ISA;
   devinst->index = sba_irq;
   devinst->irq = sba_irq;
   dlgn_instance[brd] = devinst;

   /*
    * SA_INTERRUPT when there are lots of plays happening simultaneously,
    * interrupt latency is too high causing boards to play repeated data.
    */  
#ifdef LFS
   if (request_irq(devinst->irq, (void *)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnsba", devinst ) < 0 ) {
       cmn_err(CE_NOTE, "dlgnconfig: request_irq failed for sba=%d.\n", devinst->irq);
       kmem_free(devinst, sizeof(*devinst));
       dlgn_instance[brd] = NULL;
   }
   return (0);
#else
   if (lis_request_irq(devinst->irq, (void *)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnsba", devinst ) < 0 ) {
       lis_printk("dlgnconfig: request_irq failed for sba=%d.\n", devinst->irq);
       lis_kfree(devinst);
       dlgn_instance[brd] = NULL;
   }
#endif
}

/***********************************************************************
 * NAME: linux_config_pci_device
 *
 * 1. Initialize structures that hold pci card information.
 * 2. Allocate and initialize interrupt structure.
 * 3. Bind interrupt structure and interrupt routine to interrupt number
 ***********************************************************************/
#ifdef LFS
int linux_config_pci_device(struct pci_dev *pdev)
#else
int linux_config_pci_device(pdev)

struct pci_dev *pdev;
#endif
{
   unsigned long subid;
   dlgninst_t *devinst;
#ifdef LFS
   int j;
#else
   ULONG CfgPhysAddr, virtio;
   UCHAR *virtcfg;
   int id, j;
#endif
   ULONG PhysAddr;
   ULONG VirtAddr;
   ULONG CurrOffset=0;
#ifdef LFS
   uint32_t CListOffset=0;
   uint32_t NextCListOffset=0;
#else
   ULONG CListOffset=0;
   ULONG NextCListOffset=0;
#endif
   ULONG PowerScale=0xFFFFFFFF;
   ULONG PowerLimit=0xFFFFFFFF;
#ifdef LFS
   uint32_t PowerProvided=0xFFFFFFFF;
#else
   ULONG PowerProvided=0xFFFFFFFF;
#endif

   /*
    * Need to do a pci bus config cycle to get the subid because it
    * isn't saved in the dev structure.
    */
   if (pci_read_config_dword(pdev, PCI_SUBSYSTEM_VENDOR_ID, (u32 *)&subid) ) {
#ifdef LFS
       cmn_err(CE_NOTE, "dlgnconfig: Unable to read subsystem vendor id 0x%lx\n", subid);
#else
       lis_printk("dlgnconfig: Unable to read subsystem vendor id 0x%x\n", subid);
#endif
   }

   /*
    * allocate per instance memory here
    */
#ifdef LFS
   if (!(devinst=(dlgninst_t*)kmem_alloc((sizeof(dlgninst_t)), KM_SLEEP))) {
       cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
       return(ENOMEM);
   }
#else
   if (!(devinst=(dlgninst_t*)lis_kmalloc((sizeof(dlgninst_t)), GFP_KERNEL))) {
       cmn_err(CE_WARN, "dlgnconfig: no memory for dlgn instance info");
       return(ENOMEM);
   }
#endif
   bzero((char *)devinst, sizeof(dlgninst_t));

   for (brd=0; brd < GN_MAXIRQ; ++brd) {
	if (dlgn_instance[brd] &&
   	    dlgn_instance[brd]->bustype == BUS_TYPE_PCI &&
	    dlgn_instance[brd]->pci_dev == DLGC_PCI_DEV(pdev) &&
	    dlgn_instance[brd]->pci_bus == (unsigned char) pdev->bus->number) {
#ifdef LFS
	    kmem_free(devinst, sizeof(*devinst));
#else
	    lis_kfree(devinst);
#endif
	    cmn_err(CE_WARN, "dlgnconfig: instance already being used.");
	    return(EBUSY);
        }

	if (dlgn_instance[brd] == NULL) {
	    dlgn_instance[brd] = devinst;
	    break;
	}
   }
   
   if (brd >= GN_MAXIRQ) {	/* no more DLGN instances available */
#ifdef LFS
       kmem_free(devinst, sizeof(*devinst));
#else
       lis_kfree(devinst);
#endif
       cmn_err(CE_WARN, "dlgconfig: no more instances available\n");
       return(ENXIO);
   }

   if (IS_DIALOGIC_PCI(subid)) {
       /* Save BusNum, SlotNum, BusType, FuncNum, BrdType to GpControlBlock */
       SET_BUS_SLOT_TYPE_FCN_ID(NumOfVoiceBoards, pdev->bus->number, 
	   DLGC_PCI_SLOT(pdev), BUS_TYPE_PCI, DLGC_PCI_FCN(pdev), subid);
       
       devinst->bustype = BUS_TYPE_PCI;
       devinst->pci_bus = pdev->bus->number;
       devinst->pci_dev = DLGC_PCI_DEV(pdev);
       devinst->pci_func = DLGC_PCI_FCN(pdev);
       devinst->boardid = NumOfVoiceBoards;
       devinst->irq = pdev->irq;
       devinst->index = NumOfVoiceBoards + PCI_BASE;

       /* Save Level and Intr_arg to GpControlBlock->IntrBlock */
       SET_INTRBLOCK_LEVEL(NumOfVoiceBoards, NumOfVoiceBoards+PCI_BASE);
       SET_INTRBLOCK_INTR_ARG(NumOfVoiceBoards, pdev->irq);

       if (request_irq(pdev->irq, (void *)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnpci", devinst ) < 0 ) {
#ifdef LFS
	   cmn_err(CE_NOTE, "dlgnconfig: request_irq failed for pci=%d\n", pdev->irq);
	   kmem_free(devinst, sizeof(*devinst));
#else
	   lis_printk("dlgnconfig: request_irq failed for pci=%d\n", pdev->irq);
	   lis_kfree(devinst);
#endif
	   dlgn_instance[brd] = NULL;
	   return(ENXIO);
       }       

       /* PCI Express support */
       SET_TYPE_PCIe(NumOfVoiceBoards, 0);
       pci_read_config_dword(pdev, PCI_CAPABILITY_LIST_REG_OFFSET, &CListOffset);
       while (CListOffset) {
	  /* Get to the end of the capability list to determine if the board is PCIe */
	  pci_read_config_dword(pdev, CListOffset, &NextCListOffset);
	  CurrOffset = CListOffset;
          CListOffset = (NextCListOffset&PCI_CAPABILITY_LIST_OFFSET_MASK) >> PCI_CAPABILITY_LIST_OFFSET;
          NextCListOffset &= PCI_CAPABILITY_TYPE_MASK;

          /* Check for PCIe board */
          if (NextCListOffset == PCI_EXPRESS_CAPABILITY) {
              /* Get all the Power Status Values located in the register after the capability list */
              CurrOffset += 0x4;
              pci_read_config_dword(pdev, CurrOffset, &PowerProvided);
	      if (PowerProvided != 0xFFFFFFFF) {
                  /* Get the power scale and limit values to determine actual power provided */
                  PowerScale = (PowerProvided & PCI_EXPRESS_POWER_SCALE_MASK) >> PCI_EXPRESS_POWER_SCALE_OFFSET;
                  PowerLimit = (PowerProvided & PCI_EXPRESS_POWER_LIMIT_MASK) >> PCI_EXPRESS_POWER_LIMIT_OFFSET;

                  switch (PowerScale) {
		    case 0x01: SET_POWER_PROVIDED(NumOfVoiceBoards, PowerLimit/10); break;
                    case 0x10: SET_POWER_PROVIDED(NumOfVoiceBoards, PowerLimit/100); break;
                    case 0x11: SET_POWER_PROVIDED(NumOfVoiceBoards, PowerLimit/1000); break;
                    default:   SET_POWER_PROVIDED(NumOfVoiceBoards, PowerLimit); break;
                  }
              } else {
#ifdef LFS
                  cmn_err(CE_WARN, "Error: Unable to read Power Status Registers on PCIe board at %#lx\n", CurrOffset);
#else
                  printk("Error: Unable to read Power Status Registers on PCIe board at %#x\n", CurrOffset);
#endif
                  SET_POWER_PROVIDED(NumOfVoiceBoards, 0xFFFFFFFF);
              }

	      /* Read the power good value and calculate the power required by the board */
              CurrOffset=SEAVILLE_POWER_GOOD_REG_OFFSET;
	      SET_POWER_REQUIRED(NumOfVoiceBoards, 0x0);
	      PhysAddr = DLGC_PCI_MEM(pdev, 0);
	      VirtAddr = (ULONG) ioremap((size_t) PhysAddr, PCI_BASE_ADDRESS_0);
	      for (j=0; j<4; j++) {
                   PowerProvided = *(volatile ULONG * const)(VirtAddr+CurrOffset);
		   if (PowerProvided & SEAVILLE_POWER_BUDGETTING_REG_USED) {
		       /* Need to add all 4 up to calculate the required voltage */
		       SET_POWER_REQUIRED(NumOfVoiceBoards, GET_POWER_REQUIRED(NumOfVoiceBoards) +
	 				      (PowerProvided & SEAVILLE_POWER_WATTAGE_READ_MASK));
		    }
		     
		    /* Save the power good value from the first read */
		    if (!j) {
		        SET_POWER_GOOD_REG(NumOfVoiceBoards, (PowerProvided & SEAVILLE_POWER_GOOD_MASK)
			 					         >> SEAVILLE_POWER_GOOD_OFFSET);
		    }
		    CurrOffset += 0x4;
	      } 

	      /* Read in the User Override Power Management value
	         D41JCT PCIe has the power override register at a different location as it
		 operates in the J Mode unlike other boards which operate in the I mode */
	      if (subid == PCI_D41JCT_ID) {
		  CurrOffset=SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET_J_MODE;
		  PowerProvided = *(volatile ULONG * const)(VirtAddr+CurrOffset);
		  SET_POWER_OVERRIDE(NumOfVoiceBoards, (PowerProvided & SEAVILLE_USER_OVERRIDE_POWER_MASK_J_MODE) 
								   >> SEAVILLE_USER_OVERRIDE_POWER_OFFSET_J_MODE);
	      } else {	
 	          CurrOffset=SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET;
	          PowerProvided = *(volatile ULONG * const)(VirtAddr+CurrOffset);
	          SET_POWER_OVERRIDE(NumOfVoiceBoards, (PowerProvided & SEAVILLE_USER_OVERRIDE_POWER_MASK)  
							           >> SEAVILLE_USER_OVERRIDE_POWER_OFFSET);
	      }
	      /* Debug
	      printk("PCIx: PowProv=%#x, PowReq=%#x, PowGood=%#x, PowOverRide=%#x\n",
		GET_POWER_PROVIDED(NumOfVoiceBoards), GET_POWER_REQUIRED(NumOfVoiceBoards),
		GET_POWER_GOOD_REG(NumOfVoiceBoards), GET_POWER_OVERRIDE(NumOfVoiceBoards)); */
	      iounmap((void *)VirtAddr);
	      SET_TYPE_PCIe(NumOfVoiceBoards, 1);
	      CListOffset = 0;
	  }
       }

       /* Save Physical Address to GpControlBlock */
       /* The PCI Express version of D41JCT will have 2 memory resources */
       if ((subid == PCI_D41JCT_ID) && !(GET_TYPE_PCIe(NumOfVoiceBoards))) {
	   SET_PHYS_ADDR(NumOfVoiceBoards, DLGC_PCI_MEM(pdev, 0));
       } else {
	   SET_PHYS_ADDR(NumOfVoiceBoards, DLGC_PCI_MEM(pdev, 2));
	   SET_CFG_PHYS (NumOfVoiceBoards, DLGC_PCI_MEM(pdev, 0));
       }

       NumOfVoiceBoards++;
       
   } 
#ifdef ANTARES
     else if (subid == PCI_ANT_ID) {
       /* PCI Antares */
       devinst->bustype = BUS_TYPE_PCI;
       devinst->pci_bus = pdev->bus->number;
       devinst->pci_dev = DLGC_PCI_DEV(pdev);
       devinst->pci_func = DLGC_PCI_FCN(pdev);
       devinst->boardid = NumOfVoiceBoards;
       devinst->irq = pdev->irq;

       /* Note: the following works ONLY with PCI Antares! */
       devinst->index = 0;
       CfgPhysAddr = DLGC_PCI_MEM(pdev, 0);
       virtio = DLGC_PCI_MEM(pdev, 2) & ~1;
#ifdef LFS
       virtcfg = (UCHAR *)ioremap_nocache(CfgPhysAddr, 0x80);
#else
       virtcfg = (UCHAR *)lis_ioremap_nocache(CfgPhysAddr, 0x80);
#endif
       if (virtcfg == (UCHAR *)NULL) {
#ifdef LFS
           cmn_err(CE_NOTE, "dlgnconfig: PCI Antares ioremap failed cfg=0x%lx\n", CfgPhysAddr);
           kmem_free(devinst, sizeof(*devinst));
#else
           lis_printk("dlgnconfig: PCI Antares ioremap failed cfg=0x%lx\n", CfgPhysAddr);
           lis_kfree(devinst);
#endif
           dlgn_instance[brd] = NULL;
           return(ENXIO);
       }
       
       devinst->virtcfg = virtcfg;
       
       if (request_irq(pdev->irq, (void*)dlgnintr, 
		       SA_SHIRQ|SA_INTERRUPT, "dlgnant", devinst ) < 0 ) {
#ifdef LFS
           cmn_err(CE_NOTE, "dlgnconfig: request_irq failed for ant=%d\n", pdev->irq);
           kmem_free(devinst, sizeof(*devinst));
#else
           lis_printk("dlgnconfig: request_irq failed for ant=%d\n", pdev->irq);
           lis_kfree(devinst);
#endif
           dlgn_instance[brd] = NULL;
           return(ENXIO);
       }

       id = GetIdForAntPci(virtio, virtcfg);
       
       NumOfAntaresBoards++;
       PlxInterruptEnable(virtcfg);
   }
#endif
     else { 
       /* Non-Dialogic Board */
#ifdef LFS
       kmem_free(devinst, sizeof(*devinst));
#else
       lis_kfree(devinst);
#endif
       return 0;
   }

   return 1;
}

/*********************************************************************
 * NAME: dlgninit_linux
 *
 * Traverse the pci device list looking for Dialogic boards.
 * Vendor ID and Device ID could either be the ones for the PLX
 * PCI bus interface chip, or the ones for Dialogic boards.
 *
 *********************************************************************/
#ifdef LFS
void dlgninit_linux(void)
#else
int dlgninit_linux()
#endif
{
   struct pci_dev *pdev = NULL;
   int  brdfound = 0;
#ifndef LFS
   int  brdid;
#endif
   int  i;
   

#ifdef DLGC_LIS_FLAG
#ifdef LFS
   cmn_err(CE_NOTE, "LiS-2.18 Compatible Driver\n");
#else
   lis_printk("LiS-2.18 Compatible Driver\n");
#endif
#endif

   if (ft_pc != -1) {
       switch (ft_pc) {
          case 0:
#ifdef LFS
             cmn_err(CE_NOTE, "Trace All Host Commands\n");
#else
             lis_printk("Trace All Host Commands\n");
#endif
	     break;
	  default:
#ifdef LFS
	     cmn_err(CE_NOTE, "Filter %#x Host Command\n", ft_pc);
#else
	     lis_printk("Filter %#x Host Command\n", ft_pc);
#endif
	     break;
       }
   }

   if (ft_dl != -1) {
       switch (ft_dl) {
          case 0:
#ifdef LFS
             cmn_err(CE_NOTE, "Trace All Firmware Commands\n");
#else
             lis_printk("Trace All Firmware Commands\n");
#endif
	     break;
	  default:
#ifdef LFS
	     cmn_err(CE_NOTE, "Filter %#x Firmware Command\n", ft_dl);
#else
	     lis_printk("Filter %#x Firmware Command\n", ft_dl);
#endif
	     break;
       }
   }

   if (r4_feature & 1) {
#ifdef LFS
      cmn_err(CE_NOTE, "Single Board Start/Stop Enabled\n");
#else
      lis_printk("Single Board Start/Stop Enabled\n");
#endif
      Single_Board_SS = 1;
   }

   if (blt_irq && blt_address) {
       sramvec = blt_irq;
       linux_config_blt_device();
#ifdef LFS
       cmn_err(CE_NOTE, "BLT Boards Configured (irq %d, io address 0x%x)\n",
#else
       lis_printk("BLT Boards Configured (irq %d, io address 0x%x)\n",
#endif
		      				 blt_irq, blt_address);
   } 

   if (sba_irq && sba_address) {
       sbavec = sba_irq;
       linux_config_sba_device();
#ifdef LFS
       cmn_err(CE_NOTE, "SBA Boards Configured (irq %d, io address 0x%x)\n",
#else
       lis_printk("SBA Boards Configured (irq %d, io address 0x%x)\n",
#endif
		      				 sba_irq, sba_address);
   } 

#ifndef LINUX26
   if (!pci_present()) {
#ifdef LFS
       cmn_err(CE_NOTE, "No PCI Boards Found!\n\n");
#else
       lis_printk("No PCI Boards Found!\n\n");
#endif
       return;
   }
#endif

   for (i=0; pci_spring_list[i].vendor !=0; i++ ) {
        while ((pdev = pci_find_device(pci_spring_list[i].vendor,
                         pci_spring_list[i].dev_id, pdev)) != NULL) {
           // For SR 6.0PCI only BRI and JCT boards are supportted,
	   // but with the use of r4_feature AND to 2 will detect all 
          if ((r4_feature & 2) ||
              (IS_SUPPORTED_TYPE(pdev->device)) ||
              (IS_SUPPORTED_TYPE(pdev->subsystem_device))) {
	       brdfound++;
               linux_config_pci_device(pdev);
	   } else {
#ifdef LFS
	       cmn_err(CE_NOTE, "Unsupported Board Detected (Vendor Id = %#x, Device Id = %#x)\n",
			                  pdev->vendor, pdev->subsystem_device);
#else
	       lis_printk("Unsupported Board Detected (Vendor Id = %#x, Device Id = %#x)\n",
			                  pdev->vendor, pdev->subsystem_device);
#endif
	   }
        }
   }

   if (brdfound)
#ifdef LFS
       cmn_err(CE_NOTE, "PCI Boards Configured (%d)\n", brdfound);
#else
       lis_printk("PCI Boards Configured (%d)\n", brdfound);
#endif
   else
#ifdef LFS
       cmn_err(CE_NOTE, "No PCI Boards Configured\n");
#else
       lis_printk("No PCI Boards Configured\n");
#endif

#ifdef LFS
   cmn_err(CE_NOTE, "\n");
#else
   lis_printk("\n");
#endif
}


/***********************************************************************
 *        NAME: dlgninit
 * DESCRIPTION: STREAMS init routine for driver. Currently just prints
 *              the sign-on messages.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void dlgninit()
{
   static char Gn_Signonmsg[] =
      "Dialogic Generic Device Driver\n%s\n%s";

   static char Gn_Rightsmsg[] =
      "%s\nALL RIGHTS RESERVED\n\n";


   DLGC_MSG4(CE_CONT, Gn_Signonmsg, PKGVER, OSTYPEVER );

   if (LINUX_VERSION_CODE > 0x020600)
#ifdef LFS
       cmn_err(CE_NOTE, " - Kernel 2.6.x\n");
#else
       lis_printk(" - Kernel 2.6.x\n");
#endif
   else if (LINUX_VERSION_CODE > 0x020400)
#ifdef LFS
       cmn_err(CE_NOTE, " - Kernel 2.4.x\n");
#else
       lis_printk(" - Kernel 2.4.x\n");
#endif
   else
#ifdef LFS
       cmn_err(CE_NOTE, " - Kernel 2.2.x\n");
#else
       lis_printk(" - Kernel 2.2.x\n");
#endif

   DLGC_MSG3(CE_CONT, Gn_Rightsmsg, COPYRIGHT);

#ifdef PCI_SPAN
   bzero((char *)GpControlBlock, sizeof(GP_CONTROL_BLOCK));
   GpControlBlock->GpState = GP_STATE_STOPPED;
   GpControlBlock->GpFlags = 0;
#endif

   dlgninit_linux();

   return;
}


/***********************************************************************
 *        NAME: dlgnstart
 * DESCRIPTION: STREAMS Start routine for driver. Currently does nothing
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: none
 *    CAUTIONS: none
 **********************************************************************/
int dlgnstart()
{
#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x000c)
      cmn_err(CE_NOTE,"dlgnstart()");
#endif /* DLGN_DEBUG */
   return (0);
}


/***********************************************************************
 *        NAME: dlgn_drvstart
 * DESCRIPTION: Start routine called by Config Driver after a successful
 *              CF_STRUCTINIT to start the Generic Driver's Sync Module.
 *              This routine is only called once when the Driver is 
 *              started and cannot be called again until after the
 *              Driver is halted.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
dlgn_drvstart()
{
#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x000c)
      cmn_err(CE_NOTE,"dlgn_drvstart()");
#endif /* DLGN_DEBUG */
   /*
    * Since this function should only be called once after the 
    * CF_STRUCTNIT, issue a warning and exit if this routine is ever
    * called while the driver is active. It's a bug if the user ever
    * sees this message.
    */

   if (Sm_State.st_flags & ST_SMSTART) {
      DLGC_MSG2(CE_WARN,"dlgn_drvstart(): busy, cannot re-init");
      return;
   }

   /* Clear all of the Sync Module's private data structures */
   bzero(&Sm_State,sizeof(Sm_State));
   bzero(Sm_Handle,sizeof(Sm_Handle));
   bzero(Gn_Irqlist,sizeof(Gn_Irqlist));

   /* Initialize the Sync Module's state structure */
   Sm_State.st_flags = ST_SMSTART;
   Sm_State.st_count = GN_MAXHANDLES; /*Increased from 256 to 1024 */
   Sm_State.st_peak  = -1;

   return;
}


/***********************************************************************
 *        NAME: dlgn_drvstop
 * DESCRIPTION: Stop routine called by the Config Driver after 
 *              performing a successful CF_SHUTALL to close down the 
 *              Generic Driver's Sync Module. This routine is only
 *              called when the Driver is being shut down and cannot
 *              be called again until after the Driver is restarted.
 *      INPUTS: none
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void
dlgn_drvstop()
{
#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0004)
      cmn_err(CE_NOTE,"dlgn_drvstop()");
#endif /* DLGN_DEBUG */
   /*
    * Since this function should only be called once after the 
    * CF_SHUTALL, issue a warning and exit if this routine is ever
    * called while the driver is NOT active. It's a bug if the user
    * ever sees this message.
    */
   if (!(Sm_State.st_flags & ST_SMSTART)) {
      DLGC_MSG2(CE_WARN,"dlgn_drvstop(): halted");
      return;
   }

   /* Shut down the driver's timer function if it was ever started */
   if (Sm_State.st_flags & ST_SMTIMEOUT) {
#ifdef BIT64
     untimeout((timeout_id_t)Sm_State.st_tmid);
#else
     untimeout(Sm_State.st_tmid);
#endif /* BIT64 */
   }

   /* Clear all of the Sync Module's private data structures */
   bzero(&Sm_State,sizeof(Sm_State));
   bzero(Sm_Handle,sizeof(Sm_Handle));
   bzero(Gn_Irqlist,sizeof(Gn_Irqlist));

   return;
}

/***********************************************************************
 *        NAME: _dlgn_addcmd
 * DESCRIPTION: Add a command to the list of outstanding commands on the
 *              specified ldp
 *      INPUTS: ldp - pointer to desired GN_LOGDEV structure.
 *              mp - ptr to command message to be added to the list
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: none.
 *    CAUTIONS: This function does absolutely NO error checking and
 *              **MUST** be called with correct parameters!
 *              ***** MUST BE CALLED WITH INTERRUPTS DISABLED! *****
 **********************************************************************/
PRIVATE void _dlgn_addcmd(ldp,mp)
register GN_LOGDEV   *ldp;
register mblk_t      *mp;
{
   register LD_CMD         *ld_cmdp = &ldp->ld_cmds[ldp->ld_cmdcnt];
   register GN_CMDMSG      *drvhdr = (GN_CMDMSG *) mp->b_rptr;
   static   unsigned long  command_seq = 1;
#ifdef LFS
   unsigned long z_flag;
#else
   int	    z_flag;
#endif

#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0010)
      cmn_err(CE_NOTE,"dlgn_addcmd(0x%x, 0x%x)", ldp, mp);
#endif /* DLGN_DEBUG */

   /*
    * Copy the data for the command into the last entry of the 
    * outstanding command list.
    */
   DLGCSPINLOCK(intcmd_lock, z_flag);
   ld_cmdp->lc_mp = mp;
   ld_cmdp->lc_pendcnt = 0;
   ld_cmdp->lc_maxcnt = ((unsigned long)drvhdr->cm_timeout + 
          (2 * GN_TIMESECS) - 1) / GN_TIMESECS;
   /* Increment the count of outstanding commands */
   ldp->ld_cmdcnt++;

   /* Mark the command sequence number into the command */
   drvhdr->fw_commandid = command_seq;

   /*
    * Increment the command sequence count. Note that the case of
    * a zeroed sequence number is reserved for either events or 
    * commands without a reply, so handle the wraparound when it
    * occurs.
    */
   while ((++command_seq) == 0);

   /*
    * If this is a blocking command then save all the required
    * info for blocking commands into the ldp.
    */
   if (drvhdr->cm_flags & CMD_BLOCKING) {
      ldp->ld_blockflags = 0;      /* Clear all the flags */
      ldp->ld_blockmp = mp;
   }
   DLGCSPINUNLOCK(intcmd_lock, z_flag);
}


/***********************************************************************
 *        NAME: _dlgn_rmvcmd
 * DESCRIPTION: Remove a command to the list of outstanding commands on
 *              the specified ldp
 *      INPUTS: ldp - pointer to desired GN_LOGDEV structure.
 *              cmd - index (in cmd list) of command to be removed
 *              flag - GN_SETIDLE to force state to idle when removing a
 *                 blocking cmd; GN_NOSETIDLE otherwise.
 *     OUTPUTS: none.
 *     RETURNS: next LD_CMD on the list
 *       CALLS: bzero(), bcopy()
 *    CAUTIONS: This function does absolutely NO error checking and
 *              **MUST** be called with correct parameters!
 *              ***** MUST BE CALLED WITH INTERRUPTS DISABLED! *****
 **********************************************************************/
PRIVATE void _dlgn_rmvcmd(ldp,cmd,flag)
register GN_LOGDEV   *ldp;
register int         cmd;
int flag;
{
#ifdef LFS
   unsigned long z_flag;
#else
   int z_flag;
#endif
#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0010)
      cmn_err(CE_NOTE,"dlgn_rmvcmd(0x%x, %d, 0x%x)", ldp, cmd, flag);
#endif /* DLGN_DEBUG */
   /*
    * First clear the ldp's blocking cmd data structure if the command
    * to be removed is a blocking command.
    */
   DLGCSPINLOCK(intcmd_lock, z_flag);
   if (ldp->ld_cmds[cmd].lc_mp == ldp->ld_blockmp) {
      bzero(&(ldp->ld_blockcmd),sizeof(LD_BLOCKCMD));
      if (flag & GN_SETIDLE) {
         ldp->ld_genstate = ldp->ld_idlestate;
      }
   }

   /*
    * Decrement the count of outstanding commands. If any commands
    * are left on the list, then overwrite the entry for the command
    * to be removed with the last entry on the list. (ie, shrink the
    * size of the list by one).
    */

   if (--ldp->ld_cmdcnt) {
#ifdef DLGC_LIS_FLAG 
      __wrap_memcpy( &(ldp->ld_cmds[cmd]), &(ldp->ld_cmds[cmd+1]), (sizeof(LD_CMD) * (ldp->ld_cmdcnt - cmd)));
#else
      bcopy(&(ldp->ld_cmds[cmd+1]),
         &(ldp->ld_cmds[cmd]), (sizeof(LD_CMD) * (ldp->ld_cmdcnt - cmd)));
#endif
   }
   /*
    * Finally, zero out the last entry on the command list since it
    * isn't there anymore.
    */
   bzero(&(ldp->ld_cmds[ldp->ld_cmdcnt]),sizeof(LD_CMD));

   DLGCSPINUNLOCK(intcmd_lock, z_flag);
   return;
}


/***********************************************************************
 *        NAME: _dlgn_putcmd
 * DESCRIPTION: Validate a given command message and pass it on
 *              to the PM
 *      INPUTS: ldp - pointer to desired GN_LOGDEV structure.
 *              mp - ptr to command message to be added to the list
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: 
 *    CAUTIONS: This function is meant to be called ONLY from the
 *              dlgnwput() routine. This function also assumes that
 *              dlgnwput() has already validated the handle in the
 *              message so that the _dlgn_sendpm() call will never fail.
 **********************************************************************/
PRIVATE void _dlgn_putcmd(ldp,mp,q)
register GN_LOGDEV   *ldp;
register mblk_t      *mp;
queue_t              *q;
{
   register GN_CMDMSG   *drvhdr = (GN_CMDMSG *) mp->b_rptr;
   int                  i;
   DLGCDECLARESPL(oldspl)

   /* Disable interrupts while dealing with common data */
   DLGCSPLSTR(oldspl);

   /*
    * If this is a blocking command then check for another
    * blocking command already in progress or in pending.
    */
   if (drvhdr->cm_flags & CMD_BLOCKING) {
      if (ldp->ld_blockmp != NULL) {
         /*
          * A blocking command is currently in progress
          * so reject this new command.
          */
         DLGCSPLX(oldspl);          /* restore interrupts   */
         dlgn_wqreply(q, mp, GNE_BLOCKING);
         return;
      }
   }

   /*
    * If no reply is expected, don't save the command message
    * on the queue. Just send it to the PM. Note that this 
    * cannot be a blocking command if it has no reply.
    */
   if (drvhdr->cm_nreplies == 0) {
      if (drvhdr->cm_flags & CMD_BLOCKING) {
         DLGCSPLX(oldspl);          /* restore interrupts   */
         dlgn_wqreply(q, mp, GNE_BADSTATE);
         return;
      } else {
         drvhdr->fw_commandid = 0;
         goto sendit;
      }
   }

   /*
    * Make sure that the number of replies is legal
    */
   if (drvhdr->cm_nreplies > CMD_MAXREPLIES) {
      DLGCSPLX(oldspl);          /* restore interrupts   */
      dlgn_wqreply(q, mp, GNE_BADSTATE);
      return;
   }

   /*
    * Make sure that of all the replies in the list, at least one
    * is designated as IOF_FINAL.
    */
   for (i = 0; i < (int)drvhdr->cm_nreplies; i++) {
      if (drvhdr->cm_reply[i].rs_actions & IOF_FINAL) {
         break;
      }
   }

   if (i >= (int)drvhdr->cm_nreplies) {
      DLGCSPLX(oldspl);          /* restore interrupts   */
      dlgn_wqreply(q, mp, GNE_BADSTATE);
      return;
   }

   /*
    * Check if the maximum number of commands has been reached
    * and bounce this command if it has.
    */
   if (ldp->ld_cmdcnt >= GN_MAXCURCMDS) {
      DLGCSPLX(oldspl);          /* restore interrupts   */
      dlgn_wqreply(q, mp, GNE_MAXCURCMDS);
      return;
   }

   /*
    * Save the command onto the list of outstanding commands.
    * Remember that this call must be made with interrupts disabled.
    */
   _dlgn_addcmd(ldp, mp);

sendit:
   DLGCSPLX(oldspl);          /* restore interrupts   */

   /*
    * Send down to the correct PM. Assume that the handle has been
    * validated prior to this call so that there is no need to check
    * the return code for an error.
    */
   _dlgn_sendpm(mp);

   return;
}


/***********************************************************************
 *        NAME: dlgnopen
 * DESCRIPTION: STREAMS open routine for Dialogig Generic Device Driver
 *              This function sets up the appropriate driver internal
 *              data structures upon an open call.  It implements the
 *              STREAMS clone open.
 *      INPUTS: q - STREAMS queue
 *              dev -  major/minor device number
 *              flag - normal open() flag
 *              sflag - The STREAMS clone open flag
 *     OUTPUTS: none
 *     RETURNS: minor device if open was successful
 *              OPENFAIL if open failed because Sm_State.st_count was
 *              exceeded
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int streamscall dlgnopen(q, devp, flag, sflag, crp)
queue_t *q;
dev_t *devp;
int flag;
int sflag;
cred_t *crp;
{
   int	dev = 0;
#ifdef LFS
   unsigned long  z_flag1;
   unsigned long  z_flag2;
#else
   int  z_flag1;
   int  z_flag2;
#endif
   int  major;
   register queue_t  **qpp;
#ifndef LFS
   DLGCDECLARESPL(oldspl)
#endif

   DLGCSPINLOCK(intmod_lock, z_flag1);
   DLGCSPINLOCK(intdrv_lock, z_flag2);

   /*
    * For Linux, ignoring sflag for now 
    * b/c of 256 minor node limitation on LiS
    * 
    */
   for (dev %= Sm_State.st_count; dev < Sm_State.st_count; dev++)
	if (Sm_State.st_qptr[dev] == NULL)
	    break;
   
   if (dev >= Sm_State.st_count) {
      DLGCSPINUNLOCK(intdrv_lock, z_flag2);
      DLGCSPINUNLOCK(intmod_lock, z_flag1);
      return (ENXIO);
   }

   if (q->q_ptr) {    /* if already open */
      DLGCSPINUNLOCK(intdrv_lock, z_flag2);
      DLGCSPINUNLOCK(intmod_lock, z_flag1);
      return (EBUSY);
   }

   /* store private data structures */
   qpp = &Sm_State.st_qptr[dev];
   WR(q)->q_ptr = (char *) qpp;
   q->q_ptr = (char *) qpp;
   *qpp = WR(q);

#ifdef LFS
   /* FIXME: No, no, no, no, no, no, no.  Fix all this device stuff.  Linux Fast-STREAMS has never
    * been restricted to 256 minor devices, even on old 2.4 systems.  It is restricted to 256
    * DIRECTLY ACCESSIBLE MINOR DEVICES, that is, ones that can be opened directly by external
    * device number, but can have 65736 minor device numbers assignable, and the same for majors.
    */
#endif

#ifdef LINUX26
#ifdef LFS
   (void) major;
   *devp = makedevice(getmajor(*devp), dev);
#else
   *devp = makedevice(lis_getmajor(*devp), dev);
#endif
#else
   /* get major node */
   major = dev / 256;
   if (major) {
       if (major > DLGN_MAX_MAJOR_DEV) {
	   DLGCSPINUNLOCK(intdrv_lock, z_flag2);
	   DLGCSPINUNLOCK(intmod_lock, z_flag1);
	   return (ENODEV);
       }
       /* get next major number allocated */
        *devp = dlgnMajorTable[major] << 8;
   }
#ifdef DLGC_LIS_FLAG
#ifdef LFS
   *devp = makedevice(getmajor(*devp), dev % 256);
#else
   *devp = makedevice(lis_getmajor(*devp), dev % 256);
#endif
#else
#ifdef LFS
   *devp = makedevice(getmajor(*devp), dev % 256);
#else
   *devp = makedevice(MAJOR(*devp), dev % 256);
#endif
#endif

   MOD_INC_USE_COUNT;
#endif

   DLGCSPINUNLOCK(intdrv_lock, z_flag2);
   DLGCSPINUNLOCK(intmod_lock, z_flag1);

   qprocson(q);

   return (0);
}


/***********************************************************************
 *        NAME: dlgnclose
 * DESCRIPTION: STREAMS DLGN Driver close routine
 *              This function is called when a close() is done on the
 *              opened STREAM; every handle bound through this STREAMS
 *              queue is unbound.
 *      INPUTS: q - STREAMS queue
 *     OUTPUTS: none.
 *     RETURNS: 0
 *       CALLS: splstr() splx() _dlgn_findpeak() freemsg()
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int streamscall dlgnclose(queue_t *q, int dummy, cred_t *crp)
#else
int dlgnclose(queue_t *q, int dummy, cred_t *crp)
#endif
{
   queue_t        **qpp;
   SM_HANDLE      *hp;
#ifdef LFS
   GN_LOGDEV      *ldp;
#else
   GN_LOGDEV      *ldp, *tldp;
   GN_BOARD       *gbp;
   LD_CMD         *ld_cmdp, *ld_cmdp_next;
#endif
   mblk_t         *tmp;
   int            i;
#ifdef LFS
   int            j;
#else
   int            j, k;
#endif
   unsigned long  flags = 0;
#ifdef LFS
   unsigned long  z_flag1;
   unsigned long  z_flag2;
   unsigned long  z_flag3;
#else
   int 		  z_flag1;
   int		  z_flag2;
   int		  z_flag3;
#endif
   DLGCDECLARESPL(oldspl)

   DLGCSPLSTR(oldspl);

   DLGCSPINLOCK(intmod_lock, z_flag1);
   DLGCSPINLOCK(intdrv_lock, z_flag2);
   DLGCSPINLOCK(inthw_lock, z_flag3);

   /* if no handles open, just return */
   if ((Sm_State.st_nbound <= 0)  || (Sm_State.st_peak < 0)) {
      goto close_exit;
   }

   /*
    * Go through all the bound handles to find out which ones are
    * associated with this STREAM queue, close all of them
    */
   for (i = 0; i <= Sm_State.st_peak; i++) {
      hp = &Sm_Handle[i];

      /* if handle not bound */
      if (!(hp->sm_flags & SH_BOUND)) {
         continue;
      }

      ldp = hp->sm_ldp;

      /* if handle was opened in this Stream */
      if (hp->sm_rq == q) {

         for (j = 0; j < ldp->ld_cmdcnt; j++) {
            tmp = ldp->ld_cmds[j].lc_mp;

	    /* Driver Hardening */ 
            if (!(ldp->ld_cmds[j].lc_mp) || !(ldp->ld_cmds[j].lc_mp->b_rptr)) {
		if (!err_once) {
	            DLGC_MSG4(CE_CONT, "dlgn_close: NULL lc_mp %d %d\n", ldp->ld_cmdcnt, j);
		    err_once++;
		}
	        continue;
            }

            if (i == 
               ((GN_CMDMSG *)(tmp->b_rptr))->cm_address.da_handle) {

               /* Set flags for use below */
               flags = GN_DEVBUSY;

               /* Discard the command message */
               freemsg(tmp);

               /*
                * Remove the command from the list
                * Interrupts must be disabled
                */
               _dlgn_rmvcmd(ldp, j, GN_SETIDLE);

               j--;
            }
         }

#ifdef ANTARES
         /* Call the appropriate PM close routine */
         (*pmswtbl[ldp->ld_gbp->bd_pmid].close)(ldp, flags);
#else
	 /* Direct Call to SRAM PM */
	 sramclose(ldp, flags);
#endif /* ANTARES */
	 
         /* clean up data structures */
         hp->sm_ldp = (GN_LOGDEV *)NULL;
         hp->sm_flags &= ~SH_BOUND;
         hp->sm_numevents = 0;
         Sm_State.st_nbound--;
#ifdef NOSEARCH
         /* Remove this handle from the chain of handles linked to ldp */
         _dlgn_rmvldsmhp(ldp, hp);
#endif
      }
   }

   _dlgn_findpeak();

close_exit:

   /* flush queues to prevent leak during Cntrl-C */
   flushq(WR(q), FLUSHALL);
   flushq(RD(q), FLUSHALL);
   flushq(q, FLUSHALL);

   qpp  = (queue_t **)q->q_ptr;
   *qpp = NULL;

#ifndef LINUX26
   MOD_DEC_USE_COUNT;
#endif

   DLGCSPINUNLOCK(inthw_lock, z_flag3);
   DLGCSPINUNLOCK(intdrv_lock, z_flag2);
   DLGCSPINUNLOCK(intmod_lock, z_flag1);

   qprocsoff(q);

   DLGCSPLX(oldspl);          /* restore interrupts   */

   return (0);
}


/***********************************************************************
 *        NAME: dlgnintr
 * DESCRIPTION: Interrupt handler for the Generic Driver
 *              This routine actually dispatches the incoming interrupt
 *              to the correct PM interrupt handler.
 *      INPUTS: intnum - IRQ of device interrupting
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
irqreturn_t dlgnintr(int vecnum, void *dev_id, struct pt_regs * regs)
{
   dlgninst_t   *dev = dev_id;
#ifndef LFS
   register PM_SWTBL *psp;
   register int hit = 0;
#endif
#ifdef LFS
   unsigned long z_flag;
#else
   int		z_flag;
#endif

   DLGCSPINLOCK(inthw_lock, z_flag);

#ifdef ANTARES
   for (psp = pmswtbl; psp->intr; psp++) {
      /* This is the normal case in which all PM's are called
      */
      hit += (*(psp->intr))(dev->index);
   }

   /* Call cascade interrupt handler if we didn't get a hit */
   if (!hit) {
      d4_casintr();
   }
#else
   /* Direct Call to SRAM PM */
   if (!sramintr(dev->index)) {
       DLGCSPINUNLOCK(inthw_lock, z_flag);
#ifdef LFS
       return (irqreturn_t) IRQ_NONE;
#else
       return IRQ_NONE;
#endif
   }
#endif

   DLGCSPINUNLOCK(inthw_lock, z_flag);
#ifdef LFS
   return (irqreturn_t)IRQ_HANDLED;
#else
   return IRQ_HANDLED;
#endif
}


/***********************************************************************
 *        NAME: dlgnwput
 * DESCRIPTION: Write side put routine
 *      INPUTS: q - The log dev's write stream
 *              mp - The current message
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int streamscall dlgnwput(q, mp)
#else
int dlgnwput(q, mp)
#endif
queue_t *q;
mblk_t *mp;
{
#ifdef LFS
   unsigned long z_flag;
   int i;
#else
   int z_flag, i;
#endif

   switch (mp->b_datap->db_type) {
   case M_PROTO:
   {
      register GN_CMDMSG *cmp = (GN_CMDMSG *)mp->b_rptr;
      register int handle = cmp->cm_address.da_handle;
      register SM_HANDLE *hp;

      /* Validate handle; handle must be bound */
      if ((handle > Sm_State.st_peak) || (handle < 0) ||
           (!((hp = Sm_Handle + handle)->sm_flags & SH_BOUND))) {
         dlgn_wqreply(q, mp, GNE_BADHANDLE);
         break;
      }

      switch (cmp->cm_msg_type) {
      case MT_CONTROL:
         _dlgn_control(cmp,hp,q,mp);
         break;

      /* Added to deal with the situation in which the PM
      ** asks to replenish its outgoing buffer queue and is holding
      ** onto a mutex.  Since the STREAMS message is processed
      ** immediately, i.e. in the put routine, the data can come
      ** back to the PM in the same function call.  Without the
      ** exception, i.e. not gettting a mutex here, the kernel
      ** will panic due to recursive mutex_enter.  This is
      ** the best way to resolve the problem without having to 
      ** restructure the code such that every STREAMS event is processed 
      ** in the service procedure.
      */
      case MT_BULK:
         _dlgn_putcmd(hp->sm_ldp, mp, q);
         break;

      default:
         _dlgn_putcmd(hp->sm_ldp,mp,q);
         break;         
      }
      break;
   }
   case M_IOCTL:
   {
      register struct iocblk *iocp = (struct iocblk *)mp->b_rptr;

      DLGCSPINLOCK(intdrv_lock, z_flag);
      switch (iocp->ioc_cmd) {
      case MC_BIND:
         _dlgn_bind(q,mp);
         DLGCSPINUNLOCK(intdrv_lock, z_flag);
         break;
      default:
         DLGCSPINUNLOCK(intdrv_lock, z_flag);
         dlgn_wqreply(q, mp, GNE_BADIOCTL);
         break;
      }
      break;
   }
   case M_FLUSH:
      DLGCSPINLOCK(intdrv_lock, z_flag);
      _dlgn_flush(q,mp);
      DLGCSPINUNLOCK(intdrv_lock, z_flag);
      break;

   default:
      DLGCSPINLOCK(intdrv_lock, z_flag);
      i = _dlgn_sendpm(mp); 
      DLGCSPINUNLOCK(intdrv_lock, z_flag);
      if (i == -1) {
         dlgn_wqreply(q, mp, GNE_BADHANDLE);
      }
      break;
   }

   return (0);
}


/***********************************************************************
 *        NAME: _dlgn_flush
 * DESCRIPTION: Canonical flush handling
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_flush(q,mp)
queue_t *q;
mblk_t  *mp;
{

   DLGCDECLARESPL(oldspl)
   DLGCSPLSTR(oldspl);

   /* Check to see if Write queue should be flushed */
   if ((*mp->b_rptr & FLUSHW) || (*mp->b_rptr & FLUSHRW)) {
      flushq(q, FLUSHALL);
   }

   /* Check to see if Read queue should be flushed */
   if ((*mp->b_rptr & FLUSHR) || (*mp->b_rptr & FLUSHRW)) {
      flushq(RD(q), FLUSHALL);
      /*
       * We must insure that this message is sent to all modules on the
       * stream. Because we are a driver we need only worry about the
       * upstream bound (Read queue) messages. First we clear the write
       * bit (So it isn't sent back downstream by stream head) then send
       * it upstream with qreply()
       */
      *mp->b_rptr |= FLUSHR;  /* Set read queue flush */
      *mp->b_rptr &= ~FLUSHW; /* Clear write queue flush */
      qreply(q, mp);
   }

   DLGCSPLX(oldspl);
}


/***********************************************************************
 *        NAME: _dlgn_control
 * DESCRIPTION: MT_CONTROL message processing
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
void _dlgn_control(cmp, hp, q, mp)
GN_CMDMSG *cmp;
SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   switch (cmp->fw_msg_ident) {
   case MC_UNBIND:
      _dlgn_unbind(hp,q,mp);
      break;
   case MC_SETEVMASK:
      _dlgn_setevmask(hp,q,mp);
      break;
   case MC_GETEVMASK:
      _dlgn_getevmask(hp, q, mp);
      break;
   case MC_SETIDLESTATE:
      _dlgn_setidlestate(cmp, hp, q, mp);
      break;
   case MC_GETSTATE:
      _dlgn_getstate(cmp, hp, q, mp);
      break;
   case MC_SENDEVT:
      _dlgn_ipcsndevt(cmp, hp, q, mp);
      break;
   default:
      _dlgn_putcmd(hp->sm_ldp,mp,q);
      break;   
   }
}


/***********************************************************************
 *        NAME: _dlgn_unbind
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_unbind(hp, q, mp)
register SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_LOGDEV *ldp;
   unsigned long closeflags = 0;
#ifndef LFS
   LD_CMD *ld_cmdp, *ld_cmdp_next;
   GN_BOARD *gbp;
   mblk_t *tmp;
#endif
   int handle;
   int indx;
#ifdef LFS
   unsigned long z_flag;
#else
   int z_flag;
#endif
   DLGCDECLARESPL(oldspl)

   /* Unbind must be from the same STREAM that handle was bound on */
   if (hp->sm_rq != RD(q)) {
#ifdef LFS
      DLGC_MSG5(CE_WARN,"UNBIND: rq mismatch,q=%p,hp->sm_rq=%p, RD(q)=%p\n",
         q, hp->sm_rq, RD(q));
#else
      DLGC_MSG5(CE_WARN,"UNBIND: rq mismatch,q=%x,hp->sm_rq=%x, RD(q)=%x\n",
         q, hp->sm_rq, RD(q));
#endif
      dlgn_wqreply(q, mp, GNE_BADHANDLE);
      return;
   }

   DLGCSPLSTR(oldspl);
   DLGCSPINLOCK(inthw_lock, z_flag);

   ldp = hp->sm_ldp;
   handle = hp - Sm_Handle;

   /* If handle matches, remove any outstanding commands from the list */
   for (indx = 0; indx < ldp->ld_cmdcnt; indx++) {

      /* Driver Hardening */ 
      if (!(ldp->ld_cmds[indx].lc_mp) || !(ldp->ld_cmds[indx].lc_mp->b_rptr)) {
	  if (!err_once) {
              DLGC_MSG4(CE_CONT, "dlgn_unbind: NULL lc_mp %d %d\n", ldp->ld_cmdcnt, indx);
	      err_once++;
	  }
	  continue;
      }

      if (handle == ((GN_CMDMSG *)((ldp->ld_cmds[indx].lc_mp)->b_rptr))->cm_address.da_handle) {
         closeflags |= GN_DEVBUSY;
         freemsg(ldp->ld_cmds[indx].lc_mp);
         _dlgn_rmvcmd(ldp,indx,GN_NOSETIDLE);
         indx--;
      }
   }


#ifdef ANTARES
   /* Call the PM close routine. */
   (*pmswtbl[ldp->ld_gbp->bd_pmid].close)(ldp, closeflags);
#else
   /* Direct Call to SRAM PM */
   sramclose(ldp, closeflags);
#endif /* ANTARES */
   
   /* clean up data structures */
   hp->sm_ldp = (GN_LOGDEV *)NULL;
   hp->sm_flags &= ~SH_BOUND;
   hp->sm_numevents = 0;
   Sm_State.st_nbound--;
#ifdef NOSEARCH
   /* Remove this handle from the chain of handles linked to ldp */
   _dlgn_rmvldsmhp(ldp, hp);
#endif
   _dlgn_findpeak();

   DLGCSPLX(oldspl);
   DLGCSPINUNLOCK(inthw_lock, z_flag);
   dlgn_wqreply(q, mp, 0);
}


/***********************************************************************
 *        NAME: _dlgn_setevmask
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_setevmask(hp,q,mp)
register SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_EVSTRUCT *evp;
   DLGCDECLARESPL(oldspl)

   /* More Driver Hardening */
   if (!(mp->b_cont) || !(mp->b_cont->b_rptr)) {
       cmn_err(CE_WARN, "dlgn: Corrupt Message (setevmask)\n");
       dlgn_wqreply(q, mp, GNE_BADPARM);
       return;
   }

   evp = (GN_EVSTRUCT *)mp->b_cont->b_rptr;
  
   /* Validate the number of events */
   if (evp->gv_numevents > GN_MAXEVENTS) {
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   DLGCSPLSTR(oldspl);

   if ((hp->sm_numevents = evp->gv_numevents) == 0) {
      hp->sm_flags &= ~SH_NOTIFY_NONE;
      hp->sm_flags |= SH_NOTIFY_ALL;
      goto exit;
   }

   /* copy in event masks */
#ifdef DLGC_LIS_FLAG
    __wrap_memcpy (&hp->sm_event[0], evp,sizeof (unsigned char) + hp->sm_numevents * sizeof (GN_EVENT));
#else
    bcopy (evp, &hp->sm_event[0], sizeof (unsigned char) + hp->sm_numevents * sizeof (GN_EVENT));
#endif 

   /*
    * This feature is implemented in case all but a small list of events
    * are to be notified; to set it up, a NULL event mask array is sent
    * down to signify SH_NOTIFY_ALL, then a list of exceptions (those to
    * NOT notify) is sent down
    */
   if (hp->sm_flags & SH_NOTIFY_ALL) {
      hp->sm_flags &= ~SH_NOTIFY_ALL;
      hp->sm_flags |= SH_NOTIFY_BUT;
   }

exit:

   DLGCSPLX(oldspl);
   dlgn_wqreply (q, mp, 0);
}


/***********************************************************************
 *        NAME: _dlgn_getevmask
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_getevmask(hp, q, mp)
register SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_EVSTRUCT *evp;
   int count;

   /* More Driver Hardening */
   if (!(mp->b_cont) || !(mp->b_cont->b_rptr)) {
       cmn_err(CE_WARN, "dlgn: Corrupt Message (getevmask)\n");
       dlgn_wqreply(q, mp, GNE_BADPARM);
       return;
   }

   evp = (GN_EVSTRUCT *)mp->b_cont->b_rptr;

   count = sizeof(unsigned char) + hp->sm_numevents * sizeof (GN_EVENT);
#ifdef DLGC_LIS_FLAG
   __wrap_memcpy (evp, &hp->sm_event[0], count);
#else
   bcopy (&hp->sm_event[0], evp, count);
#endif /* LINUX */
   mp->b_cont->b_wptr += count; /* EFN: is this OK? b_wptr already set? */

   dlgn_wqreply (q, mp, 0);
}


/***********************************************************************
 *        NAME: _dlgn_setidlestate
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_setidlestate(cmp, hp, q, mp)
GN_CMDMSG *cmp;
SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_STATEINFO *stp = (GN_STATEINFO *)(mp->b_cont->b_rptr);
   register GN_LOGDEV *ldp;
   DLGCDECLARESPL(oldspl)

   if (msgdsize(mp->b_cont) != sizeof(GN_STATEINFO)) {
      cmp->fw_errcode = GNE_BADPARM;
      cmp->cm_msg_type = MT_ERRORREPLY;   /* EFN: want this? */
      qreply (q, mp);                     /* EFN: want dlgn_wqreply()? */
      return;
   }

   DLGCSPLSTR(oldspl);

   ldp = hp->sm_ldp;

   stp->st_retstate = ldp->ld_genstate;
   ldp->ld_idlestate = ldp->ld_genstate = stp->st_idlestate;

   DLGCSPLX(oldspl);

   /* Normally no reply */
   freemsg(mp);
}


/***********************************************************************
 *        NAME: _dlgn_getstate
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_getstate(cmp, hp, q, mp)
GN_CMDMSG *cmp;
SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_STATEINFO *stp;
   register GN_LOGDEV *ldp;
   DLGCDECLARESPL(oldspl)

   /* More Driver Hardening */
   if (!(mp->b_cont) || !(mp->b_cont->b_rptr)) {
       cmn_err(CE_WARN, "dlgn: Corrupt Message (getstate)\n");
       dlgn_wqreply(q, mp, GNE_BADPARM);
       return;
   }

   stp = (GN_STATEINFO *)(mp->b_cont->b_rptr);

   if (msgdsize(mp->b_cont) != sizeof(GN_STATEINFO)) {
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   DLGCSPLSTR(oldspl);
   ldp = hp->sm_ldp;

   STRACE3("dlgn_setstate: ldp=0x%lx, genstate=0x%x, newstate=0x%x\n",
      ldp, ldp->ld_genstate, stp->st_newstate);

   stp->st_retstate = ldp->ld_genstate;
   if ((stp->st_idlestate != stp->st_newstate) &&
       (stp->st_idlestate == ldp->ld_genstate)) {
      ldp->ld_genstate = stp->st_newstate;
   }

   DLGCSPLX(oldspl);

   dlgn_wqreply(q, mp, 0);
}

/***********************************************************************
 *        NAME: _dlgn_ipcsndevt
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_ipcsndevt(cmp, hp, q, mp)
GN_CMDMSG *cmp;
SM_HANDLE *hp;
queue_t *q;
mblk_t *mp;
{
   register GN_SENDEVTINFO *sndevtp;
   register USHORT snd_evtflags;
   register int handle;
   register GN_LOGDEV *ldp;
   register int i;
#ifdef NOSEARCH
   register SM_HANDLE *ldsmhp;
#endif
   mblk_t  *tmp;
   DLGCDECLARESPL(oldspl)

   /* More Driver Hardening */
   if (!(mp->b_cont) || !(mp->b_cont->b_rptr)) {
       cmn_err(CE_WARN, "dlgn: Corrupt Message (ipcsndevt)\n");
       dlgn_wqreply(q, mp, GNE_BADPARM);
       return;
   }

   sndevtp = (GN_SENDEVTINFO *)(mp->b_cont->b_rptr);
   snd_evtflags = sndevtp->snd_evtflags & EVFL_SENDALL;
   handle = cmp->cm_address.da_handle;

   if (msgdsize(mp->b_cont) != sizeof(GN_SENDEVTINFO)) {
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   DLGCSPLSTR(oldspl);

   ldp = hp->sm_ldp;

#ifdef NOSEARCH
   /*
    * For each SM_HANDLE pointer linked to ldp, send a copy of this event
    * message up on
    * - each handle, if the flags are set to EVFL_SENDALL
    * - each handle except itself, if the flags are set to EVFL_SENDOTHERS
    * - own handle, if the flags are set to EVFL_SENDSELF
    */
   ldsmhp = (SM_HANDLE *)(ldp->ld_smhp);
   while (ldsmhp != NULL) {
      if ( (snd_evtflags == EVFL_SENDALL) ||
          ((snd_evtflags == EVFL_SENDOTHERS) && (ldsmhp != hp)) ||
          ((snd_evtflags == EVFL_SENDSELF)   && (ldsmhp == hp)) ) {

         if ((tmp = copymsg(mp)) == NULL) {
            dlgn_msg(CE_WARN, "_dlgn_ipcsndevt: copymsg() failure!");
            dlgn_wqreply(q, mp, GNE_STRAYREPLY);
            return;
         } else {
            tmp->b_datap->db_type = mp->b_datap->db_type;

            ((GN_CMDMSG *)(tmp->b_rptr))->cm_address.da_handle =
                                                        ldsmhp - Sm_Handle;
            ((GN_CMDMSG *)(tmp->b_rptr))->cm_msg_type = MT_EVENT;
            ((GN_CMDMSG *)(tmp->b_rptr))->fw_msg_ident = MC_IPCEVENT;
            putq(ldsmhp->sm_rq, tmp);
         }
      }
      ldsmhp = ldsmhp->sm_nxthp;
   }
#else
   /*
    * Search through the Sm_Handle array for all entries containing an ldp
    * matching the current one.  Send a copy of this event message up on
    * - each matching ldp, if the flags are set to EVFL_SENDALL
    * - each matching ldp except itself, if the flags are set to EVFL_SENDOTHERS
    * - its own ldp, if the flags are set to EVFL_SENDSELF
    */
   for (i = 0; i <= Sm_State.st_peak; i++) {
      if (Sm_Handle[i].sm_ldp == ldp) {

         if ( (snd_evtflags == EVFL_SENDALL) ||
             ((snd_evtflags == EVFL_SENDOTHERS) && (handle != i)) ||
             ((snd_evtflags == EVFL_SENDSELF) && (handle == i))) {

            if ((tmp = copymsg(mp)) == NULL) {
               dlgn_msg(CE_WARN, "_dlgn_ipcsndevt: copymsg() failure!");
               dlgn_wqreply(q, mp, GNE_STRAYREPLY);
               DLGCSPLX(oldspl);
               return;
            } else {
               tmp->b_datap->db_type = mp->b_datap->db_type; 

               ((GN_CMDMSG *)(tmp->b_rptr))->cm_address.da_handle = i;
               ((GN_CMDMSG *)(tmp->b_rptr))->cm_msg_type = MT_EVENT;
               ((GN_CMDMSG *)(tmp->b_rptr))->fw_msg_ident = MC_IPCEVENT;
               putq(Sm_Handle[i].sm_rq,tmp);
            }
         }
      }
   }
#endif

   DLGCSPLX(oldspl);

   freemsg(mp->b_cont);
   mp->b_cont = NULL;
   dlgn_wqreply(q, mp, 0);
}

/***********************************************************************
 *        NAME: _dlgn_bind
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
PRIVATE void _dlgn_bind(q, mp)
queue_t *q;
mblk_t *mp;
{
   SM_HANDLE *hp;
   GN_LOGDEV *ldp;
   GN_BIND *bindp;
   int len;
#ifdef LFS
   unsigned long z_flag;
#else
   int z_flag;
#endif
   int handle;
   DLGCDECLARESPL(oldspl)

   /* Must have some ioctl data */
   if (mp->b_cont == NULL) {
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   bindp = (GN_BIND *)(((GN_CMDMSG *)mp->b_cont->b_rptr) + 1);

   if ((len = bindp->gb_datalen) <= 0) {
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   DLGCSPLSTR(oldspl);
   DLGCSPINLOCK(inthw_lock, z_flag);

   /* Get first free handle; error if none */
   for (handle = 0; handle < GN_MAXHANDLES; handle++) {
      if (Sm_Handle[handle].sm_ldp == (GN_LOGDEV *)NULL) {
         break;
      }
   }

   if (handle >= GN_MAXHANDLES) {
      DLGCSPLX(oldspl);
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      dlgn_wqreply(q, mp, GNE_NOHANDLE);
      return;
   }

   /* Get ldp from the given name & save ldp in Sm_Handle */
   hp = Sm_Handle + handle;
   if ((hp->sm_ldp = ldp = cf_name2ldp(bindp->gb_devname)) == NULL) {
      DLGCSPLX(oldspl);
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      dlgn_wqreply(q, mp, GNE_NOLDP);
      return;
   }

   hp->sm_numevents = 0;
   hp->sm_rq = RD(q);
   hp->sm_flags = SH_NOTIFY_NONE | SH_BOUND;
   bindp->gb_handle = handle;
   Sm_State.st_nbound++;

   if (handle == Sm_State.st_peak + 1) {
      Sm_State.st_peak++;
   }

#ifdef NOSEARCH
   _dlgn_addldsmhp(ldp, hp);
#endif /* NOSEARCH */

   DLGCSPLX(oldspl);
   
#ifdef ANTARES
   /* Call the PM's open routine */
   if ((*pmswtbl[ldp->ld_gbp->bd_pmid].open)(ldp) != 0) {
#else
   if (sramopen(ldp) != 0) {
#endif /* ANTARES */
      DLGCSPLSTR(oldspl);
      Sm_State.st_nbound--;
      hp->sm_rq    = 0;
      hp->sm_ldp   = 0;
      hp->sm_flags = 0;
      hp->sm_numevents = 0;
#ifdef NOSEARCH
      /* Remove this handle from the chain of handles linked to ldp */
      _dlgn_rmvldsmhp(ldp, hp);
#endif
      _dlgn_findpeak();
      DLGCSPLX(oldspl);
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      dlgn_wqreply(q, mp, GNE_BADPARM);
      return;
   }

   DLGCSPINUNLOCK(inthw_lock, z_flag);

   dlgn_wqreply(q, mp, 0);
}


/***********************************************************************
 *        NAME: dlgnwsrv
 * DESCRIPTION: Write side service routine
 *      INPUTS: q - The log dev's write stream
 *     OUTPUTS: none.
 *     RETURNS: 0
 *       CALLS: 
 *    CAUTIONS: If a queued message has an invalid or unbound handle,
 *              _dlgn_sendpm CANNOT send the message to the PM!
 **********************************************************************/
#ifdef LFS
int streamscall dlgnwsrv(q)
#else
int dlgnwsrv(q)
#endif
queue_t *q;
{
   mblk_t      *mp;
#ifdef LFS
   unsigned long z_flag1;
   unsigned long z_flag2;
#else
   int		z_flag1;
   int		z_flag2;
#endif

   DLGCSPINLOCK(intmod_lock, z_flag1);
   DLGCSPINLOCK(intdrv_lock, z_flag2);

   while ((mp = getq(q)) != (mblk_t *)NULL) {
      /*
       * A switch statement can be used here in the future when
       * there are different message types which we need to process
       * in this service procedure. For now we can just send everything
       * to the appropriate PM.
       */
      _dlgn_sendpm(mp);
   }

   DLGCSPINUNLOCK(intdrv_lock, z_flag2);
   DLGCSPINUNLOCK(intmod_lock, z_flag1);


   return (0);
}


/* dlgnrput() not needed, dlgn_reply does the work */

/***********************************************************************
 *        NAME: dlgnrsrv
 * DESCRIPTION: Read side service procedure
 *              This function only processes event messages that were
 *              queued by dlgn_reply
 *      INPUTS: q - The log dev's read stream
 *     OUTPUTS: none.
 *     RETURNS: none.
 *       CALLS: freemsg() _dlgn_procevt()
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int streamscall dlgnrsrv(q)
#else
int dlgnrsrv(q)
#endif
queue_t *q;
{
#ifndef LFS
   int       i, eventcnt;
#endif
   mblk_t    *mp;
#ifndef LFS
   GN_LOGDEV *ldp;
#endif
   GN_CMDMSG *drvhdr;
#ifndef LFS
   int	     z_flag1;
   int	     z_flag2;
   DLGCDECLARESPL(oldspl)
#endif

   while ((mp = getq(q)) != (mblk_t *)NULL) {

      switch (mp->b_datap->db_type) {
      default:
	 putnext(q, mp);
         break;

      case M_PROTO:
         drvhdr = (GN_CMDMSG *)mp->b_rptr;

         switch (drvhdr->cm_msg_type) {
         default:
            freemsg(mp);   /* throw away message for now */
            break;
         case MT_EVENT:
            _dlgn_procevt(mp);
            break;
         case MT_REPLY:
	 case MT_BULK:
	 case MT_VIRTEVENT: 
	 case MT_ERRORREPLY:
	    putnext(q, mp);
	    break;
         }
      }
   }

   return (0); 
}


/***********************************************************************
 *        NAME: dlgn_reply
 * DESCRIPTION: Called by PM to pass a message to the SM.
 *              This is the PM's 'rput' routine.
 *      INPUTS: mp - Current message to send to SM
 *              ldp - The logical device pointer for this device
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: splstr() splx() freemsg() putq() DLGC_MSG() 
 *    CAUTIONS:
 **********************************************************************/
void dlgn_reply(ldp, mp)
GN_LOGDEV *ldp;
mblk_t *mp;
{
   register GN_CMDMSG   *drvhdr;
   register int         handle;
   register SM_HANDLE   *hp;
#ifndef LFS
   register LD_CMD      *ld_cmdp;
#endif
   int                  i;
   mblk_t               *tmp;
#ifdef NOSEARCH
   register SM_HANDLE   *ldsmhp;
#endif
   DLGCDECLARESPL(oldspl)

   /* Set up some heavily-used variables */
   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   handle = drvhdr->cm_address.da_handle;
   hp = &Sm_Handle[handle];
   switch (mp->b_datap->db_type) {
   default:

      STRACE2("dlgn_reply: default ldp=0x%lx, mp=0x%lx", ldp, mp);

      /* if the handle is not bound, it's a stray message, discard */
      if (!(hp->sm_flags & SH_BOUND)) {
         DLGC_MSG3(CE_CONT, "dlgn_reply(): stray message received. handle=%x\n", handle);
         freemsg(mp);
         return;
      }

      if (hp->sm_rq) {
         putq(hp->sm_rq, mp);
      } else {
         DLGC_MSG2(CE_CONT, "dlgn_reply(): hp->sm_rq is NULL\n");
         freemsg(mp);
      }
      break;

   case M_PROTO:

      switch (drvhdr->cm_msg_type) {
      default:

         STRACE2("dlgn_reply: M_PROTO BULK/VIRT ldp=0x%lx, mp=0x%lx", ldp, mp);

         /* pass thru MT_BULK, MT_VIRTEVENT, etc. */
         if (hp->sm_rq) {
            putq(hp->sm_rq, mp);
         } else {
            DLGC_MSG2(CE_CONT, "dlgn_reply(): hp->sm_rq is NULL\n");
            freemsg(mp);
         }
         break;

      case MT_EVENT:

#ifdef NOSEARCH
         /*
          * For each Sm_Handle structure linked to this ldp, send a copy of
          * this MT_EVENT message.
          */
         ldsmhp = (SM_HANDLE *)(ldp->ld_smhp);
         if (ldp->ld_bindcnt == 1) {
            drvhdr->cm_address.da_handle = ldsmhp - Sm_Handle;

#ifdef IST
            /* IST start timer to time */
            if (Tmr_fcn[TM_START] != NULL) {
               (*Tmr_fcn[TM_START])(TM_PQTRS);
            }
#endif

            putq(ldsmhp->sm_rq, mp);
         } else {
            while (ldsmhp != NULL) {
               if ((tmp = copymsg(mp)) == NULL) {
                  dlgn_msg(CE_WARN,"dlgn_reply: copymsg() failure!");
               } else {
                  ((GN_CMDMSG *)(tmp->b_rptr))->cm_address.da_handle =
                                                           ldsmhp - Sm_Handle;
                   tmp->b_datap->db_type = mp->b_datap->db_type;

#ifdef IST
                  /* IST start timer to time */
                  if (Tmr_fcn[TM_START] != NULL) {
                     (*Tmr_fcn[TM_START])(TM_PQTRS);
                  }
#endif

                  putq(ldsmhp->sm_rq, tmp);
               }
               ldsmhp = ldsmhp->sm_nxthp;
            }
            freemsg(mp);
         }
#else
         STRACE2("dlgn_reply: M_PROTO MT_EVENT ldp=0x%lx, mp=0x%lx", ldp, mp);

         /*
          * Search through the Sm_Handle array for all entries containing
          * an ldp matching the current one. Send a copy of this MT_EVENT
          * message up on each matching one, then discard the original
          * message.
          */
         for (i = 0; i <= Sm_State.st_peak; i++) {
            if (Sm_Handle[i].sm_ldp == ldp) {
               if ((tmp = copymsg(mp)) == NULL) {
                  DLGC_MSG2(CE_WARN,"dlgn_reply: copymsg() failure!");
               } else {
                   tmp->b_datap->db_type = mp->b_datap->db_type; 

                  ((GN_CMDMSG *)(tmp->b_rptr))->cm_address.da_handle = i;
#ifdef IST
                  /* IST start timer to time */
                  if (Tmr_fcn[TM_START] != NULL) {
                     (*Tmr_fcn[TM_START])(TM_PQTRS);
                  }
#endif

                  putq(Sm_Handle[i].sm_rq,tmp);
               }
            }
         }
         freemsg(mp);
#endif
         return;

      case MT_ERRORREPLY:

	 DLGCSPLSTR(oldspl);
         STRACE2("dlgn_reply: M_PROTO MT_ERROR ldp=0x%lx, mp=0x%lx", ldp, mp);

         /*
          * Search thru the list of outstanding commands on this ldp to
          * see if this error reply is for one of those commands and
          * remove the command from the list AND free the command
          * message packet if so.
          */
         for (i = 0; i < ldp->ld_cmdcnt; i++) {
            /* Get a pointer to the command message packet */
            tmp = ldp->ld_cmds[i].lc_mp;
	    
	    /* Driver Hardening */ 
            if (!(ldp->ld_cmds[i].lc_mp) || !(ldp->ld_cmds[i].lc_mp->b_rptr)) {
		if (!err_once) {
	            DLGC_MSG4(CE_CONT, "dlgn_reply: MT_ERRORREPLY NULL lc_mp %d %d\n", ldp->ld_cmdcnt, i);
		    err_once++;
		}
	        continue;
            }

            if ( ((GN_CMDMSG *)(tmp->b_rptr))->fw_commandid ==
               drvhdr->fw_commandid) {
               /*
                * The commandid matched, so remove it from the list.
                * Interrupts must be disabled.
                */
               _dlgn_rmvcmd(ldp,i,GN_NOSETIDLE);
               /*
                * If the message currently on mp is a copy of the 
                * original command message packet, then we need to
                * free the original command message packet.
                */
               if (tmp != mp) {
                  freemsg(tmp);
               }

               break;
            }
         }

	 DLGCSPLX(oldspl);        /* Let those interrupt dudes loose */

         /* Send up the reply */
         putq(hp->sm_rq, mp);
         return;

      case MT_REPLY:

         STRACE2("dlgn_reply: M_PROTO MT_REPLY ldp=0x%lx, mp=0x%lx", ldp, mp);

         /*
          * Only expected replies should get here, the rest
          * should have been converted to MT_EVENT. If this is
          * true then the commandid field should never be zero.
          */
         if (drvhdr->fw_commandid == 0) {
            ldp->ld_lostevents++;
            DLGC_MSG3(CE_WARN,
               "dlgn_reply: %s: Invalid reply type. Message discarded.",
               ldp->ld_name);

            STRACE2("dlgn_reply MT_REPLY: ldp=0x%lx: 0 sequence number: mp=0x%lx. Message discarded.", ldp, mp);

            /* Discard the message and exit */
            freemsg(mp);

            return;
         }


         /*
          * Verify that a correct number of replies exists
          */
         if (drvhdr->cm_nreplies == 0 ||
             drvhdr->cm_nreplies > CMD_MAXREPLIES) {
            ldp->ld_lostevents++;
            DLGC_MSG4(CE_WARN,
               "dlgn_reply: %s: Bad reply count: %d. Message discarded.",
               ldp->ld_name, drvhdr->cm_nreplies);

            STRACE3("dlgn_reply MT_REPLY: ldp=0x%lx: Bad reply count: %d mp=0x%lx. Message discarded.", ldp, drvhdr->cm_nreplies, mp);

            /* Discard the message and exit */
            freemsg(mp);

            return;
         }

         /* Search for the reply that was marked as 'received' */
         for (i = 0; i < (int)drvhdr->cm_nreplies; i++) {
            if (drvhdr->cm_reply[i].rs_actions & IOF_RECEIVED) {
               break;      /* Found it! */
            }
         }

         /*
          * If no reply is marked as 'received' then this is bad
          * news. Log error and discard the message.
          */
         if (i >= (int)drvhdr->cm_nreplies) {
            ldp->ld_lostevents++;
            DLGC_MSG3(CE_WARN,
               "dlgn_reply: %s: Bad reply message, message discarded.",
               ldp->ld_name);

            STRACE3("dlgn_reply MT_REPLY: ldp=0x%lx, seq=0x%lx, mp=0x%lx: Bad reply message, message discarded", ldp, drvhdr->fw_commandid, mp);

            /* Discard the message and exit */
            freemsg(mp);

            return;
         }


	 DLGCSPLSTR(oldspl);

         /*
          * Perform state maintenance based upon the value given
          * with the reply.
          */
         if (drvhdr->cm_reply[i].rs_actions & IOF_NEWSTATE) {
            ldp->ld_genstate = drvhdr->cm_reply[i].rs_info;
         }

         /*
          * See if this is a FINAL reply. If so, mark the command
          * as terminated and send it upstream.
          */
         if (drvhdr->cm_reply[i].rs_actions & IOF_FINAL) {

            /* Mark it as the reply that terminated the command */
            drvhdr->cm_reply[i].rs_actions |= IOF_TERMINATOR;

            /*
             * Remove the command from the list of outstanding commands
             * and free the message packet that it came on.
             */
            for (i = 0; i < ldp->ld_cmdcnt; i++) {
               /* Get a pointer to the command message packet */
               tmp = ldp->ld_cmds[i].lc_mp;

	       /* Driver Hardening */ 
               if (!(ldp->ld_cmds[i].lc_mp) || !(ldp->ld_cmds[i].lc_mp->b_rptr)) {
		   if (!err_once) {
	               DLGC_MSG4(CE_CONT, "dlgn_reply: MT_REPLY NULL lc_mp %d %d\n", ldp->ld_cmdcnt, i);
		       err_once++;
		   }
	           continue;
               }

               if ( ((GN_CMDMSG *)(tmp->b_rptr))->fw_commandid ==
                  drvhdr->fw_commandid) {
                  /*
                   * The commandid matched, so remove it from the list
                   * Interrupts must be disabled
                   */
                  _dlgn_rmvcmd(ldp,i,GN_NOSETIDLE);

                  /* Free the command message packet */
                  if (tmp != mp) {
                     freemsg(tmp);
                  }
                  break;
               }
            }

            /*
             * I'm not sure that unconditionally clearing the error
             * code field here is such a great thing to do but the 
             * driver as it presently exists needs it done in order
             * to work so here it is.
             */
            drvhdr->fw_errcode = 0;

            /* Reenable interrupts */
	    DLGCSPLX(oldspl);

            /* send final reply upstream */
            putq(hp->sm_rq, mp);
            return;
         }

         /* Reenable interrupts */
	 DLGCSPLX(oldspl);

         /*
          * See if this is an INTERMEDIATE reply. If so,
          * send it upstream.
          */
         if (drvhdr->cm_reply[i].rs_actions & IOF_INTERMEDIATE) {
 	     drvhdr->fw_errcode = 0;
             putq(hp->sm_rq, mp);
            return;
         }

         /*
          * See if this is an IGNORE reply. If so, just discard it
          * and do not send it upstream.
          */
         if (drvhdr->cm_reply[i].rs_actions & IOF_IGNORE) {
            freemsg(mp);
            return;
         }

         /*
          * If this point is reached, it is an error and the 
          * reply message will be discarded
          */
         ldp->ld_lostevents++;
         DLGC_MSG3(CE_WARN,
            "dlgn_reply: %s: Reply in bad state, message discarded.",
            ldp->ld_name);

         /* Toss it */
         freemsg(mp);

         break;

      }              /* end M_PROTO switch */

   }                 /* end db_type switch */

   return;
}

/***********************************************************************
 *        NAME: dlgn_findreply
 * DESCRIPTION: Searches for the expected reply by looking through the
 *              cm_reply[] struct array in the command messages saved on
 *              the ld_cmds[] list in the given ldp. If a match is
 *              found, then a pointer to a copy of the message is
 *              returned. If no match is found, then an event message
 *              is created & returned. A matched reply is marked as
 *              IOF_RECEIVED.
 *      INPUTS: ldp = ptr to the ldp containing the cmd list to search.
 *              newreply = the code for the reply to be matched.
 *              datasize = size of the variable-length datablock that
 *              accompanies the driver-specific portion of the 
 *              command message.
 *     OUTPUTS: None
 *     RETURNS: Pointer to the reply message if successful.
 *              NULL pointer if fails.
 *       CALLS: dlgn_copymp(), freemsg(), splstr(), splx()
 *    CAUTIONS: If commands on the list have duplicate replies, then
 *              the first one found is returned.
 **********************************************************************/
mblk_t *dlgn_findreply (ldp, newreply, datasize)
GN_LOGDEV      *ldp;
unsigned short newreply;
unsigned long  datasize;
{
   register    GN_CMDMSG   *cmdmsgp;
#ifndef LFS
   register    LD_CMD      *ld_cmdp;
   register    int         j;
#endif
   register    int         j = 0;
   register    int         i;
   mblk_t      *mp = NULL;
   mblk_t      *nmp;
   int         foundreply = 0;
#ifndef LFS
   int         copymp_flag = 0;
#endif
   DLGCDECLARESPL(oldspl)


   DLGCSPLSTR(oldspl);

   /*
    * Search through each command given on the list of outstanding
    * commands.
    */
   for (i = 0; i < ldp->ld_cmdcnt; i++) {

      /* Driver Hardening */
      if (!(ldp->ld_cmds[i].lc_mp) || !(ldp->ld_cmds[i].lc_mp->b_rptr)) {
	  if (!err_once) {
	      DLGC_MSG4(CE_CONT, "dlgn_findreply NULL lc_mp %d %d\n", ldp->ld_cmdcnt, i);
	      err_once++;
	  }
	  continue;
      }
      
      /* Get a pointer to the header for this outstanding command */
      cmdmsgp = (GN_CMDMSG *) (ldp->ld_cmds[i].lc_mp->b_rptr);

      /*
       * If this command is a blocking command, then save its mp to
       * be copied below. This needs to be done because an
       * intermediate message for the blocking command may have no replies
       * specified, but we still need to be sure to send it up on the
       * correct handle (as an MT_EVENT).
       */
      if (cmdmsgp->cm_flags & CMD_BLOCKING) {
         mp = ldp->ld_cmds[i].lc_mp;
      }

      /*
       * Search through the list of replies for this command
       */
      for (j = 0; j < (int)cmdmsgp->cm_nreplies; j++) {
         /*
          * If the reply code matches, get a pointer to the message
          */
         if (cmdmsgp->cm_reply[j].rs_ident == newreply) {

            /* Save the mp for this command message for use below */
            mp = ldp->ld_cmds[i].lc_mp;

            /* Mark that we matched a reply */
            foundreply = 1;

#ifdef CONSERVE_MEMORY
            /* If this is the final reply to command, re-use the cmd message */
            if (cmdmsgp->cm_reply[j].rs_actions & IOF_FINAL) {
               nmp = mp;
               goto do_finalreply;
            }
#endif

            /* Done here so break out of search loop */
            goto do_copymsg;
         }
      }
   }


do_copymsg:

   /*
    * If no command was found or if the reply failed to match, then
    * we should have mp = NULL at this point. If a matching reply
    * was found, or if this is a blocking command and no matching reply
    * was found, then mp will point to the command message containing
    * the matching reply.  To optimize the code, should not duplicate the
    * message if it is to be used once, i.e. when there is a match.
    * By the way, do this iff the reply is IOF_FINAL.
    */
   if ((nmp = dlgn_copymp(mp,datasize)) == NULL) {
      /* Copy failed. Log the error */  
      ldp->ld_lostevents++;
      DLGC_MSG3(CE_WARN,
         "dlgn_findreply: %s: No message packet for reply!",
         ldp->ld_name);
      DLGCSPLX(oldspl);
      return (NULL);
   }


#ifdef CONSERVE_MEMORY
do_finalreply:
#endif
   /*
    * If a reply was matched above, then set the message type to 
    * MT_REPLY and mark the reply in the new message as 'received'.
    * Otherwise, set the message type to MT_EVENT and save the
    * event id into the new message.
    */
   if (foundreply != 0) {
      ((GN_CMDMSG *) (nmp->b_rptr))->cm_msg_type = MT_REPLY;
      ((GN_CMDMSG *) (nmp->b_rptr))->cm_reply[j].rs_actions |=
         IOF_RECEIVED;
   } else {
      ((GN_CMDMSG *) (nmp->b_rptr))->cm_msg_type = MT_EVENT;
      ((GN_CMDMSG *) (nmp->b_rptr))->cm_event.rs_ident = newreply;
   }

   DLGCSPLX(oldspl);

   return (nmp);

}


/***********************************************************************
 *        NAME: dlgn_loopback
 * DESCRIPTION: Routine for PM to loop a message back to itself.
 *              This is like the putbq for the PM.
 *      INPUTS: mp - Message to route back to PM
 *              ldp - The logical device pointer
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: DLGC_MSG() putq() freemsg()
 *    CAUTIONS:
 **********************************************************************/
int dlgn_loopback(ldp, mp)
GN_LOGDEV *ldp;
mblk_t *mp;
{
   register SM_HANDLE *hp;

   hp = &Sm_Handle[((GN_CMDMSG *)mp->b_rptr)->cm_address.da_handle];

   STRACE1("dlgn_loopback: handle=0x%x\n", hp);

   /* put mp onto SM write queue */
   if (hp->sm_rq) {
      putq(WR(hp->sm_rq), mp);
   } else {
      DLGC_MSG2(CE_CONT, "dlgn_loopback(): hp->sm_rq is NULL\n");
      freemsg(mp);
      return (-1);
   }

   return (0);
}


/***********************************************************************
 *        NAME: _dlgn_procevt
 * DESCRIPTION: Process MT_EVENT messages based on event masks
 *              previously set.
 *      INPUTS: mp - The current message to be sent to the board
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: DLGC_MSG() _dlgn_sendevt()
 *    CAUTIONS:
 **********************************************************************/
PRIVATE int _dlgn_procevt(mp)
mblk_t *mp;
{
   SM_HANDLE         *hp;
   GN_CMDMSG         *drvhdr;
#ifndef LFS
   mblk_t            *nmp;
#endif
   int               nevents;
   int               handle;
   int               sendflag = 0;
   int               j;
   unsigned short    event_type;

   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   handle = drvhdr->cm_address.da_handle;
   event_type = drvhdr->cm_event.rs_ident;

   hp = &Sm_Handle[handle];

   if (hp->sm_flags & SH_NOTIFY_ALL)   /* send event always */ {
      STRACE1("_dlgn_procevt SH_NOTIFY_ALL: mp=0x%lx", mp);
      _dlgn_sendevt(handle, mp);
      sendflag++;
      goto exit;
   }

   /* Do not notify of any event */
   if (hp->sm_flags & SH_NOTIFY_NONE) {
      STRACE1("_dlgn_procevt SH_NOTIFY_NONE: mp=0x%lx", mp);
      goto exit;
   }

   nevents = hp->sm_numevents;

   if (hp->sm_flags & SH_NOTIFY_BUT)   /* notify all except those on list */ {
      STRACE1("_dlgn_procevt SH_NOTIFY_BUT: mp=0x%lx", mp);
      for (j = 0; j < nevents; j++) {
         if (hp->sm_event[j].ge_event_id == event_type) /* if on list */ {
            if (hp->sm_event[j].ge_disposition == NOTIFY) { /* but NOTIFY anyway */
               _dlgn_sendevt(handle, mp);
               sendflag++;
            }
            goto exit;
         }

      }

      if (j == nevents) {   /* not on exclude list */
         _dlgn_sendevt(handle, mp);
         sendflag++;
      }
      goto exit;
   }

   /*
    * "normal" processing, process according to event mask for this
    * event
    */
   for (j = 0; j < nevents; j++) {
      STRACE1("_dlgn_procevt normal processing: mp=0x%lx", mp);
      if (hp->sm_event[j].ge_event_id == event_type) {
         if (hp->sm_event[j].ge_disposition == NOTIFY) {
            _dlgn_sendevt(handle, mp);
            sendflag++;
         }
         break;
      }
   }

exit:

   /* Free the message here if it was never given to _dlgn_sendevt */
   if (sendflag == 0) {
      freemsg (mp);
   }

   return (0);
}


/***********************************************************************
 *        NAME: _dlgn_sendevt
 * DESCRIPTION: Send an event message upstream
 *
 *      INPUTS: handle_id - SM's internal handle number
 *    mp - The current message to be sent to user space
 *     OUTPUTS: none.
 *     RETURNS: 0.
 *       CALLS: putnext()
 *    CAUTIONS:
 **********************************************************************/
PRIVATE int _dlgn_sendevt(handle_id, mp)
unsigned short handle_id;
mblk_t         *mp;
{
   GN_CMDMSG   *drvhdr;
#ifndef LFS
   int         handle;
   SM_HANDLE   *hp;
#endif

   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   drvhdr->cm_address.da_handle = handle_id; /* change address in mp */

   putnext(Sm_Handle[handle_id].sm_rq, mp); /* send it up */

   return (0);
}


/***********************************************************************
 *        NAME: dlgn_copymp
 * DESCRIPTION: Duplicates the given putmsg() control packet by using
 *              the copymsg() utility, and returns a pointer to the
 *              copy. If mp == NULL, then allocates a message packet of
 *              size equal to sizeof(GN_CMDMSG) + size, zeroes it out,
 *              and retuns a pointer to the message.
 *      INPUTS: mp - control message packet to copy, or NULL
 *              size - size of driver-dependent portion of message.
 *     OUTPUTS: none
 *     RETURNS: New message pointer if successful; else NULL.
 *       CALLS: bzero() copymsg()
 *    CAUTIONS: This function assumes it is either called by the
 *              interrupt handler or under splstr() protection.
 **********************************************************************/
mblk_t *dlgn_copymp(mp,size)
register mblk_t         *mp;
register unsigned long  size;
{
   register mblk_t         *tmp;
   register unsigned long  tot_size;
   DLGCDECLARESPL(oldspl)


   DLGCSPLSTR(oldspl);

   if (mp == NULL) {
      /*
       * Allocate a message block for the new message. Remember that
       * allocb returns M_DATA messages by default.
       */
      tot_size = size + sizeof(GN_CMDMSG);
#ifdef LFS
      /* Do not use BPRI_HI: it can fail too easily. --bb */
      if ((tmp = (mblk_t *) allocb(tot_size, BPRI_MED)) == NULL) {
         DLGC_MSG2(CE_WARN,
            "dlgn_copymp(): Cannot copy message: STREAMS allocb failure!");
         goto exit;
      }
#else
      if ((tmp = (mblk_t *) allocb(tot_size, BPRI_HI)) == NULL) {
         DLGC_MSG2(CE_WARN,
            "dlgn_copymp(): Cannot copy message: STREAMS allocb failure!");
         goto exit;
      }
#endif

      /*
       * Zero out the message buffer and adjust b_wptr accordingly
       */
      bzero (tmp->b_datap->db_base,tot_size);

      tmp->b_wptr += tot_size;

      /*
       * Set the size of the variable-length data field
       */
      ((GN_CMDMSG *)(tmp->b_rptr))->fw_datalen[0] = size;

   } else {

      /* Create a duplicate of the given message. */
      if ((tmp = copymsg(mp)) == NULL) {
         DLGC_MSG2(CE_WARN,
            "dlgn_copymp(): Cannot copy message: STREAMS copymsg failure!");
         goto exit;
      }

      /*
       * Set the types in the new message to M_DATA, in order to match
       * what allocb does above.
       */
      tmp->b_datap->db_type = M_DATA;
   }

exit:

   DLGCSPLX(oldspl);

   return (tmp);
}
   
   
/***********************************************************************
 *        NAME: dlgn_wqreply
 * DESCRIPTION: This function returns a reply to the user. Called while
 *              processing a user request in the write-side put routine.
 *      INPUTS: wq - pointer to the write queue
 *              mp - pointer to the user request message
 *              rc - return code to return to the user
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *   CAUTIONS : 
 **********************************************************************/
void dlgn_wqreply(wq, mp, rc)
queue_t *wq;
register mblk_t *mp;
int rc;
{

   if (mp->b_datap->db_type == M_IOCTL) {
      register struct iocblk *iocp = (struct iocblk *)mp->b_rptr;

      ((GN_CMDMSG *)mp->b_cont->b_rptr)->cm_msg_type = MT_REPLY;
      mp->b_datap->db_type = M_IOCACK;
      if ((iocp->ioc_rval = rc)) {
         iocp->ioc_count = 0;    /* No data to user if error */
      }
      iocp->ioc_error = 0;
   } else {
      register GN_CMDMSG *drvhdr = (GN_CMDMSG *)mp->b_rptr;
      drvhdr->fw_errcode = rc;
      drvhdr->cm_msg_type = MT_REPLY;
      if (mp->b_datap->db_type == M_DATA) {
         mp->b_datap->db_type = M_PROTO;
      }
   }

   qreply(wq, mp);
}


/***********************************************************************
 *        NAME: _dlgn_findpeak
 * DESCRIPTION: This function searches the Sm_Handle structure to find
 *              the highest numbered handle id that's bound.
 *              As BIND's and UNBIND's happen, handle id's are allocated
 *              and deallocated; we only need to search to the highest
 *              allocated handle id, and not the entire Sm_Handle list.
 *              This function serves as a "peak meter" function.
 *      INPUTS: none.
 *     OUTPUTS: none.
 *     RETURNS: 0, with the side effect of setting Sm_State.st_peak
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
PRIVATE void _dlgn_findpeak()
{
   int   i;

   /*
    * Start from the top. If the handle is not bound, decrement
    * Sm_State.st_peak
    */
   for (i = Sm_State.st_peak; i >= 0; i--) {
      if (Sm_Handle[i].sm_flags & SH_BOUND)
         break;
      Sm_State.st_peak--;
   }

}


/***********************************************************************
 *        NAME: dlgn_getpeak
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
long dlgn_getpeak()
{
   return (Sm_State.st_peak);
}


/***********************************************************************
 *        NAME: dlgn_busywait
 * DESCRIPTION: Busy wait for 'msec' milliseconds. This routine is
 *              public and available for PM use.
 *      INPUTS: msec - number of milliseconds to wait
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: none
 *   CAUTIONS : This routine assumes that _dlgn_systime() has been
 *              called at driver startup to initialize the variable
 *              'Sm_State.st_loop'
 **********************************************************************/
void dlgn_busywait(msec)
int msec;
{
#ifdef LFS
        drv_usecwait(msec * 1000);
#else
        lis_udelay(msec * 1000);
#endif

}


#ifdef LFS
PRIVATE void streamscall _dlgn_replyck(caddr_t dummy)
#else
PRIVATE void _dlgn_replyck()
#endif
{
   register GN_LOGDEV   *ldp;
   register LD_CMD      *lcp;
   register GN_CMDMSG   *drvhdr;
   register int         i;
#ifndef LFS
   int                  oldspl;
#endif
   int                  handle;

#ifdef LFS
   DLGCDECLARESPL(oldspl);
#endif

   DLGCSPLSTR(oldspl);

   /* Go through every ldp in the system */
   for (ldp = Gn_Logdevp; ldp < (GN_LOGDEV *)Gn_Freep; ldp++) {

      /* If the ldp is unopened then skip it */
      if (!(ldp->ld_flags & LD_OPEN)) {
         continue;
      }

      /* Search thru the list of outstanding commands on this ldp. */
      for (i = 0; i < ldp->ld_cmdcnt; i++) {
         lcp = &ldp->ld_cmds[i];

         if (++lcp->lc_pendcnt >= lcp->lc_maxcnt) {

            /* Command has timed out. */
            DLGC_MSG3(CE_WARN,
               "DLGN: _dlgn_replyck: Pending reply on %s timed out!",
               ldp->ld_name);

            /* Convert command to a timeout message */
            drvhdr = (GN_CMDMSG *) lcp->lc_mp->b_rptr;
            handle = drvhdr->cm_address.da_handle;
            drvhdr->fw_errcode = GNE_TIMEOUT;
            drvhdr->cm_msg_type = MT_REPLY;

            /* Send timeout message to user */
            putq(Sm_Handle[handle].sm_rq, lcp->lc_mp);

            /*
             * Remove the command message from the list.
             * Interrupts must be disabled.
             * Do **NOT** free the message packet!
             */
            _dlgn_rmvcmd(ldp,i,GN_NOSETIDLE);
            i--;
         }
      }
   }

   /* Reestablish ACK/NAK timeout routine. */
   Sm_State.st_tmid = timeout(_dlgn_replyck, 0, GN_TIMEOUT);

   DLGCSPLX(oldspl);

   return;
}

/***********************************************************************
 *        NAME: _dlgn_sendpm
 * DESCRIPTION: Send message to the appropriate PM via protocol id
 *      INPUTS: mp - The current message to be sent to the PM
 *     OUTPUTS: none
 *     RETURNS: 0 if msg sent; -1 if msg NOT sent
 *       CALLS: xxx_sendcmd() in the xxx PM.
 *    CAUTIONS: This routine will NOT attempt to end the given message
 *              to the PM if the given handle is invalid or not bound.
 **********************************************************************/
PRIVATE int _dlgn_sendpm(mp)
mblk_t  *mp;
{
#ifndef LFS
   int      rcode;
#endif
#ifdef LFS
   unsigned long z_flag;
#else
   int	    z_flag;
#endif
   register GN_LOGDEV      *ldp;
   register GN_CMDMSG      *drvhdr;
   register unsigned long  handle;

   DLGCSPINLOCK(inthw_lock, z_flag);
   
   drvhdr = (GN_CMDMSG *)mp->b_rptr;
   handle = drvhdr->cm_address.da_handle;

   /* Make sure that we have been given a valid, bound handle */
   if ((handle > Sm_State.st_peak) ||
      (!(Sm_Handle[handle].sm_flags & SH_BOUND)) ) {
      DLGC_MSG2(CE_WARN,"_dlgn_sendpm(): bad handle, message not sent\n");
      DLGCSPINUNLOCK(inthw_lock, z_flag);
      return (-1);
   }

   ldp = Sm_Handle[handle].sm_ldp;

#ifdef ANTARES
   /* Vector to the PM's sendcmd routine */
   (*pmswtbl[ldp->ld_gbp->bd_pmid].sendcmd)(ldp, mp);
#else
   /* Direct Call to SRAM PM */
   sramsendcmd(ldp, mp);
#endif /* ANTARES */
   
   DLGCSPINUNLOCK(inthw_lock, z_flag);
   return (0);
}


/***********************************************************************
 *        NAME: dl_setintr
 * DESCRIPTION: Global interrupt registration function
 *      INPUTS: function pointer
 *     OUTPUTS: 
 *     RETURNS: none
 *       CALLS: NOTHING
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
void dl_setintr(int(*intr_handler)(void))
#else
void dl_setintr(intr_handler)
int (*intr_handler)();
#endif
{
   cascade_intr = intr_handler;

   return;
}

/***********************************************************************
 *        NAME: d4_casintr
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
void d4_casintr()
{
   if (cascade_intr) {
      cascade_eoi = ( ((*cascade_intr)() == 0) ? 1 : 0 );
   }

   return;
}


#ifdef _ISDN
/***********************************************************************
 *        NAME: dlgn_getldp
 * DESCRIPTION: Return ldp for the handle received as parameter.
 *      INPUTS: handle - The handle for the device whose generic logdev
 *                       pointer is to be returned.
 *     OUTPUTS: none
 *     RETURNS: LDP for the handle; NULL if error
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
GN_LOGDEV *dlgn_getldp(handle)
   register int handle;
{
   register SM_HANDLE *hp;


   /* Validate handle; handle must be bound */
   if ((handle > Sm_State.st_peak) || (handle < 0) ||
           (!((hp = Sm_Handle + handle)->sm_flags & SH_BOUND))) {
      return NULL;
   }

   return hp->sm_ldp;
}
#endif /* _ISDN */

#ifdef NOSEARCH
/***********************************************************************
 *        NAME: _dlgn_addldsmhp
 * DESCRIPTION: Add the handle to the chain of handles linked to ldp
 *      INPUTS: ldp - Logical Device Pointer
 *            : hp  - The handle pointer for the device associated with ldp
 *     OUTPUTS: none
 *     RETURNS: Nothing
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
PRIVATE void _dlgn_addldsmhp(ldp, hp)
   register GN_LOGDEV *ldp;
   register SM_HANDLE *hp;
{
   register SM_HANDLE *ldsmhp;


   if (ldp->ld_bindcnt == 0) {
      ldp->ld_smhp = (void *)hp;
   } else {
      ldsmhp = (SM_HANDLE *)(ldp->ld_smhp);
      while (ldsmhp->sm_nxthp != NULL) {
         ldsmhp = ldsmhp->sm_nxthp;
      }
      ldsmhp->sm_nxthp = hp;
   }
   ldp->ld_bindcnt++;
}


/***********************************************************************
 *        NAME: _dlgn_rmvldsmhp
 * DESCRIPTION: Remove the handle from the chain of handles linked to ldp
 *      INPUTS: ldp - Logical Device Pointer
 *            : hp  - The handle pointer for the device associated with ldp
 *     OUTPUTS: none
 *     RETURNS: Nothing
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
PRIVATE void _dlgn_rmvldsmhp(ldp, hp)
   register GN_LOGDEV *ldp;
   register SM_HANDLE *hp;
{
   register SM_HANDLE *ldsmhp;


   if (ldp->ld_bindcnt == 0) {
      dlgn_msg(CE_WARN, "_dlgn_rmvldsmhp(): No Handles");
      return;
   }

   if (ldp->ld_smhp == (void *)hp) {
      ldp->ld_smhp = (void *)(hp->sm_nxthp);
      hp->sm_nxthp = NULL;
      ldp->ld_bindcnt--;
   } else {
      ldsmhp = (SM_HANDLE *)(ldp->ld_smhp);
      while ((ldsmhp != NULL) && (ldsmhp->sm_nxthp != hp)) {
         ldsmhp = ldsmhp->sm_nxthp;
      }
      if (ldsmhp != NULL) {
         ldsmhp->sm_nxthp = hp->sm_nxthp;
         hp->sm_nxthp = NULL;
         ldp->ld_bindcnt--;
      } else {
         dlgn_msg(CE_WARN, "_dlgn_rmvldsmhp(): Handle non-existant");
         return;
      }
   }
}
#endif /* NOSEARCH */


/***********************************************************************
 *        NAME: dlgn_hangup
 * DESCRIPTION: 
 *      INPUTS: ldp - GN_LOGDEV pointer
 *              rc  - return code
 *     OUTPUTS: none.
 *     RETURNS: 0
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
#ifdef LFS
int dlgn_hangup(GN_LOGDEV *ldp, int rc)
#else
int dlgn_hangup(ldp, rc)
GN_LOGDEV *ldp;
int rc;
#endif
{
   GN_CMDMSG *drvhdr;
   SM_HANDLE *hp;
   mblk_t *mp;
   int handle;

   /* Search through all the bound handles to find the
	* handle associated with this logical device.
    */
   for (handle = 0; handle <= Sm_State.st_peak; handle++) {
      hp = Sm_Handle + handle;
      if (!(hp->sm_flags & SH_BOUND))
         continue;
      if (hp->sm_ldp == ldp)
		 break;
   }

   /* No logical device match found. */
   if (handle > Sm_State.st_peak)
	  return(ENODEV);

   /* Allocate a message block for M_HANGUP. */
   if ((mp = dlgn_copymp(NULL, 0L)) == NULL) {
      return(ENOMEM);
   }

   /* Change the message type. */
   mp->b_datap->db_type = M_HANGUP;

   /* Set up the command header. */
   drvhdr = (GN_CMDMSG *) mp->b_rptr;
   drvhdr->cm_address.da_handle = handle;
   drvhdr->fw_errcode = rc;
   drvhdr->cm_msg_type = MT_ERRORREPLY;

   /* Send the hangup upstream. */
   dlgn_reply(ldp, mp);

   return (0);
}

#ifdef LFS
EXPORT_SYMBOL_GPL(dlgn_msg);
EXPORT_SYMBOL_GPL(dlgn_copymp);
EXPORT_SYMBOL_GPL(dlgn_getpeak);
EXPORT_SYMBOL_GPL(intmod_lock);
EXPORT_SYMBOL_GPL(dlgn_wqreply);
EXPORT_SYMBOL_GPL(Gn_Endp);
EXPORT_SYMBOL_GPL(Gn_Boardp);
EXPORT_SYMBOL_GPL(pmswtbl);
EXPORT_SYMBOL_GPL(cf_ioccopy);
EXPORT_SYMBOL_GPL(cf_cmpstr);
EXPORT_SYMBOL_GPL(Gn_Numpms);
EXPORT_SYMBOL_GPL(cf_iocput);
EXPORT_SYMBOL_GPL(Gn_Freep);
EXPORT_SYMBOL_GPL(Gn_Logdevp);
EXPORT_SYMBOL_GPL(Gn_Headerp);
EXPORT_SYMBOL_GPL(Gn_Maxpm);
EXPORT_SYMBOL_GPL(Gn_Memsz);
EXPORT_SYMBOL_GPL(dlgn_drvstop);
EXPORT_SYMBOL_GPL(Gn_Msglvl);
EXPORT_SYMBOL_GPL(dlgn_drvstart);
EXPORT_SYMBOL_GPL(NumOfVoiceBoards);
EXPORT_SYMBOL_GPL(index);
EXPORT_SYMBOL_GPL(dl_setintr);
EXPORT_SYMBOL_GPL(GpControlBlock);
EXPORT_SYMBOL_GPL(st_sctlock);
EXPORT_SYMBOL_GPL(st_freelock);
EXPORT_SYMBOL_GPL(st_uselock);
EXPORT_SYMBOL_GPL(StatisticsMonitoring);
EXPORT_SYMBOL_GPL(DriverStats);
#else				/* LFS */
#ifdef LINUX26
EXPORT_SYMBOL(dlgn_msg);
EXPORT_SYMBOL(dlgn_copymp);
EXPORT_SYMBOL(dlgn_getpeak);
EXPORT_SYMBOL(intmod_lock);
EXPORT_SYMBOL(dlgn_wqreply);
EXPORT_SYMBOL(Gn_Endp);
EXPORT_SYMBOL(Gn_Boardp);
EXPORT_SYMBOL(pmswtbl);
EXPORT_SYMBOL(cf_ioccopy);
EXPORT_SYMBOL(cf_cmpstr);
EXPORT_SYMBOL(Gn_Numpms);
EXPORT_SYMBOL(cf_iocput);
EXPORT_SYMBOL(Gn_Freep);
EXPORT_SYMBOL(Gn_Logdevp);
EXPORT_SYMBOL(Gn_Headerp);
EXPORT_SYMBOL(Gn_Maxpm);
EXPORT_SYMBOL(Gn_Memsz);
EXPORT_SYMBOL(dlgn_drvstop);
EXPORT_SYMBOL(Gn_Msglvl);
EXPORT_SYMBOL(dlgn_drvstart);
EXPORT_SYMBOL(NumOfVoiceBoards);
EXPORT_SYMBOL(index);
EXPORT_SYMBOL(dl_setintr);
EXPORT_SYMBOL(GpControlBlock);
EXPORT_SYMBOL(st_sctlock);
EXPORT_SYMBOL(st_freelock);
EXPORT_SYMBOL(st_uselock);
EXPORT_SYMBOL(StatisticsMonitoring);
EXPORT_SYMBOL(DriverStats);
#endif
#endif				/* LFS */
