/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : andam.c
 * Description                  : antares device access module
 *
 *
 **********************************************************************/

#ifdef WNT_386
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stdarg.h>
#include <stream.h>
#include <stropts.h>
#include <cmn_err.h>
#elif LINUX
#include "dlgclinux.h"
#define _SCO_
#else
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#ifndef AIX_ANT
#include <sys/cmn_err.h>
#else
#include "ant_aix.h" 
#endif
#endif 
#include "dlgcos.h"
#if (defined(SVR4) || defined(UNISYS_MP) || defined(UNISYS_UP))
    #ifndef SE_NOSLP
        #ifdef SVR4
            #ifndef AIX_ANT
            #include <sys/kmem.h>
#ifndef SVR5
            #include <sys/strsubr.h>
#endif /* SVR5 */
            #endif
        #else
        #define SE_NOSLP 1
        #endif
    #endif
#endif


/* Generic Driver */
#include "gndefs.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndlgn.h"
#include "gnlibdrv.h"
#if (defined(SOL_ANT) || defined(AIX_ANT) || defined(LINUX))
#include "gndbg.h"
#include "gncfd.h"
#endif
#include "include.h"
#include "antares.ver"		 

/* from intrface sub directory */
#include "var.h"
#include "intrface.h"
#include "anprot.h"

/* from protocol sub_directory */
#include "protocol.h"
#include "antglob.h"
#include "drvprot.h"

/* from hardware sub directory */
#include "hardexp.h"

/* Antares Device Access Module */
#include "anlibdrv.h"
#include "anstart.h"
#include "andrv.h"
#include "andam.h"

#ifdef PCI_ANT
extern SHORT numPciBoards;
extern int validate_pci_port(ushort port);
extern void AntPciIntrReset(WORD port);
#endif /* PCI_ANT */

#ifdef WNT_ANT
#include "ant_wnt.h"
PM_STATE Ant_State = {0};
VOID cmn_err(ULONG,...);
#endif

#ifdef BIG_ANT
#include "byteswap.h"
#else
#define LBYTESWAP(w) w
#define WBYTESWAP(w) w
#endif /* BIG_ANT */

#ifdef RTDL_ANT /* firstAnStart */
/* flag to determine if this is the first call to an_start after antctl */
SHORT firstAnStart=TRUE;
#endif

static void InitPMValues(AN_STARTPM*);
void *AllocMem(LONG);
void  FreeMem(void*,LONG);

void an_reply(GN_LOGDEV*,mblk_t*,SHORT,SHORT,ULONG,int);

void ConfirmFill(BD_CHANNEL *,SHORT,SHORT,BYTE); 
void UpdateFill(BD_CHANNEL *,SHORT,BYTE);

SYS_INFO sys = {0,0,0};

GN_LOGDEV *main_ldp = (GN_LOGDEV *) NULL;
GN_LOGDEV *curr_ldp = (GN_LOGDEV *) NULL;
ULONG curr_gnhndl = 0;
AN_RCUCOR *rcu_cor;
AN_BDCOR *bd_cor;
int andevflag = 0;

#ifdef AIX_ANT
volatile WORD *virtual_port = (WORD *) NULL;
BYTE in_map = 0;
struct intr ant_offl_int;
#endif

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
BYTE in_mutex = FALSE;
BYTE in_close = FALSE;
#ifdef PCI_ANT
ULONG get_pci_cfg_virt_addr(ushort port);
#endif /* PCI_ANT */
#endif

#ifdef LINUX
#ifdef LFS
extern spinlock_t inthw_lock;
#else
extern lis_spin_lock_t inthw_lock;
#endif
#endif /* LINUX */

#ifdef _SCO_
#ifdef _KERNDBG_
#include "drvdebug.h"
#endif

AN_BOARD *An_Irq = NULL;

/*
 * Prototypes for this module's PRIVATE functions
 */
static void an_addirq(GN_BOARD*);
static void an_rmvirq(GN_BOARD*);

SHORT UpdateStart(BD_CHANNEL*,SHORT);
#endif /* _SCO_ */

/***********************************************************************
 *        NAME: InitPMValues
 * DESCRIPTION: DAM init global variables routine
 *      INPUTS: anspmp - ptr to AN_STARTPM area.
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
void InitPMValues(AN_STARTPM* anspmp)
{
    SHORT i;

    sys.maxdpi = anspmp->sys.maxdpi;
    sys.an_state = anspmp->sys.an_state;
    sys.raw_ind = anspmp->sys.raw_ind;
    sys.maxhookinfo = anspmp->sys.maxhookinfo;

    cfg.maxrcu = anspmp->cfg.maxrcu;
    cfg.MasterBoard = anspmp->cfg.MasterBoard;
    cfg.maxopenedrcu = anspmp->cfg.maxopenedrcu;
    cfg.maxbd = anspmp->cfg.maxbd;
    cfg.hwintr = anspmp->cfg.hwintr;
    cfg.nboards = anspmp->cfg.nboards;
    cfg.bd_iobuf = anspmp->cfg.bd_iobuf;
    cfg.anq_buffer = anspmp->cfg.anq_buffer;
    cfg.maxslots = anspmp->cfg.maxslots;
    cfg.anq_entrysize = anspmp->cfg.anq_entrysize;
    cfg.anq_items = anspmp->cfg.anq_items;
    cfg.m_message = anspmp->cfg.m_message;
    cfg.woption = anspmp->cfg.woption;
    cfg.intrdwrt = anspmp->cfg.intrdwrt;
    cfg.dspcall = anspmp->cfg.dspcall;
    cfg.asktoken = anspmp->cfg.asktoken;
    cfg.autoinc = anspmp->cfg.autoinc;

    for (i = 0; i < cfg.nboards; i++) {
#ifdef VME_ANT
        p_board[i].slot_id = anspmp->p_board[i].port;
        p_board[i].port = anspmp->p_board[i].board_id;
#else
        p_board[i].port = anspmp->p_board[i].port;
#ifdef WNT_ANT
	GnMapPhysicalIoPort(p_board[i].port);
	GnMapPhysicalIoPort(p_board[i].port+2);
	GnMapPhysicalIoPort(p_board[i].port+4);
	GnMapPhysicalIoPort(p_board[i].port+6);
#endif
#endif
        p_board[i].board_id = anspmp->p_board[i].board_id;
    }
}
#ifdef WNT_ANT
VOID unkseg(PVOID Address, ULONG ByteCount)
{
    MmFreeNonCachedMemory(Address, ByteCount);
    return;
}
#endif
/****************************************************************
*        NAME : AllocMem
* DESCRIPTION : Allocate memory from kernel area 
*       INPUT : Required memory size.
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
void *AllocMem(LONG size)
{
#ifdef LINUX
#ifdef LFS
      /* Use KM_NOSLEEP as in the regular unix case. */
      return((void *)kmem_alloc(size, KM_NOSLEEP));
#else
      /* This is an error.  This should be GFP_ATOMIC instead of GFP_KERNEL.  AllocMem() is called
       * potentially with interrupts suppressed.  This will cause a crash on current kernels. --bb */
      return((void *)lis_kmalloc(size, GFP_KERNEL));
#endif
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
      return(kseg((int)btoc(size)));
#endif
#if (defined(SVR4) || defined(UNISYS_MP) || defined (UNISYS_UP))
#ifdef AIX_ANT
      return(xmalloc(size,4,(caddr_t)pinned_heap));
#else
      return(kmem_zalloc(size, KM_NOSLEEP));
#endif
#endif
}

/****************************************************************
*        NAME : FreeMem
* DESCRIPTION : Free memory from kernel area 
*       INPUT : Required memory size.
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
void FreeMem(void *address,LONG size)
{
#ifdef LINUX
#ifdef LFS
      kmem_free(address, size);
#else
      lis_kfree(address);
#endif
#endif /* LINUX */
#ifdef WNT_ANT
      unkseg((char*)address,size);
#else
#ifdef SVR3
      unkseg((char*)address);
#endif
#endif
#if (defined(SVR4) || defined(UNISYS_MP) || defined (UNISYS_UP))
#ifdef AIX_ANT
      if (xmfree((caddr_t)address,(caddr_t)pinned_heap) == -1) {
          cmn_err(CE_WARN,"Error in xmfree() address=0x%X\n",address);
      }
#else
      kmem_free(address, size);
#endif
#endif
}

/***********************************************************************
 *        NAME: anstartpm
 * DESCRIPTION: DAM initialization routine
 *      INPUTS: gnbdp - ptr to start of GN_BOARD array for this DAM
 *              anspmp - ptr to non-genric data area.
 *              nbds - total number of BOARDS on this DAM
 *              nlds - total number of LOGDEVS on this DAM
 *              anspmsz - Total # bytes of non-generic init data area.
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: It is assumed that startbd() of the Antares ??LOAD?? DAM
 *              has executed successfully.
 **********************************************************************/
int anstartpm(
GN_BOARD *gnbdp,
char     *anspmp,
ULONG    nbds,
ULONG    nlds,
ULONG    anspmsz)
{
    AN_STARTPM *tanspmp;     /* ptr to init info configured for a dam */
    GN_BOARD  *tgnbdp;    /* ptr to generic board struct */
    AN_BOARD  *anbp;     /* ptr to device-dependant board struct */
    GN_LOGDEV *ldp;      /* ptr to generic logical dev struct */
    WORD    nlogdevs; /* number of logical dev configured on a board */

#ifndef LFS
    SHORT rc,val;
    LONG memsize = 0;
#else
    SHORT rc;
#endif
    LONG tlength;
    VOID *p1;
    VOID *p2;
    VOID *p3;
    int i;
#ifdef AIX_ANT
    struct io_map iomap;
#endif

#if 0
cmn_err(CE_CONT, "anstartpm: brdsz=%x, gnbdp=%x, spmp=%x, b=%d, l=%d, sz=%x\n",
sizeof(GN_BOARD), gnbdp, anspmp, nbds, nlds, anspmsz);
#endif

   /* Validate the device dependent data sizes */
    if (nbds * sizeof(AN_STARTPM) != anspmsz) {
        cmn_err(CE_WARN,"anstartpm: Bad size, cannot start DAM");
        return (-1);
    }
   
   /* See if the DAM has already been started */
    if (sys.an_state != NotActive) {
        cmn_err(CE_WARN,"anstartpm: Busy, cannot re-start");
        return (-1);
    }
 
#ifdef SPARC_PCI
   /* See if one of the PCI brds has been found */
    if (!numPciBoards) {
        cmn_err(CE_WARN,"anstartpm: cannot start, no PCI brds found");
        return (-1);
    }
#endif /* SPARC_PCI */
 
   /* Get driver current version */
    Antares_Hverp = (ULONG)HVERP;
    Antares_Lverp = (ULONG)LVERP;

    incindex = 0;	   /* increament board number in pseudo array */

   /* Get ptrs to device-dependent memory area */
    tgnbdp = gnbdp;
    tanspmp = ((AN_STARTPM *)anspmp);

    InitPMValues(tanspmp);

#ifdef PCI_ANT
    for (i = 0; i < cfg.nboards; i++) {
#ifndef SPARC_PCI /* only pci boards */
       if (p_board[i].port & _A4_16BIT_PORT) {
	  continue;
       }
#endif /* SPARC_PCI */
       if (validate_pci_port(p_board[i].port)) {
           cmn_err(CE_WARN,
		"anstartpm: Board %d: bad port id 0x%x, cannot start DAM",
		i, p_board[i].port);
           return (-1);
       }
    }
#endif /* PCI_ANT */

#if 0
   /* Initiates addresses for hardware */
    init_hardware(cfg.intrdwrt);

   /* Initiates addresses for protocols */
    rc = init_protocol((VOID **)(&(p1)),(VOID **)(&(p2)),(VOID **)(&(p3)));
    if (rc != E_SUCC) {
        cmn_err(CE_WARN,"Antares_Board declared wrong in configuration file !");
        return(-1);
    }

   /* init intrface variables */
    init_interface((CFG_INFO *)&cfg);
#endif /* _SCO_ */

   /* Allocate memory for bulk data buffers */
    if (cfg.bd_iobuf > 0x4000) {
        cmn_err(CE_WARN,"\nAntares driver supports bulk data buffers \
                         of up to 16K\n");
        return(-1);
    }

   /* Initiate size of half ping-pong buffer */
    cfg.bd_single = (LONG)(cfg.bd_iobuf >> 1);

   /* allocate memory for bulk data body array */
    tlength = (LONG)((LONG)BDSIZE * (LONG)cfg.maxbd);
    p_bd = (BD_CHANNEL *)AllocMem(tlength);
    if (p_bd == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_bd fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_bd,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_bd,ctob(btoc(tlength)));
#endif

   /* determines the address of recv_message buffer in driver memory   */ 
    tlength = (LONG)cfg.m_message;
    p_message = (BYTE *)AllocMem(tlength);
    if (p_message == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_message fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_message,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_message,ctob(btoc(tlength)));
#endif

   /* address for ROUTE_BUFFER */
    if ((TS_SIZE * 2) >= MAPSLOT_SIZE) {
        tlength = (LONG)((cfg.maxslots + 2) * 2 * TS_SIZE * sizeof(ULONG));
    }
    else {
        tlength = (LONG)((cfg.maxslots + 2) * MAPSLOT_SIZE * sizeof(ULONG));
    }
    tlength += 24;

    routebuffer = (ULONG *)AllocMem(tlength);
    if (routebuffer == NULL) {
        cmn_err(CE_WARN,"AllocMem() routebuffer fail");
        return -1;
    }
#ifdef LINUX
    bzero(routebuffer,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)routebuffer,ctob(btoc(tlength)));
#endif

   /* Buffer For SYNC answers */
    tlength = (LONG)MAXSYNCANSWER;
    p_reply = (BYTE *)AllocMem(tlength);
    if (p_reply == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_reply fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_reply,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_reply,ctob(btoc(tlength)));
#endif

   /* allocate memory for process array */
    tlength = (LONG)((LONG)PRCSIZE * (LONG)cfg.maxbd);
    p_data = (PRC *)AllocMem(tlength);
    if (p_data == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_data fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_data,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_data,ctob(btoc(tlength)));
#endif
  
 /* allocate memory for unix translate bd array */
    tlength = (LONG)((LONG)(sizeof(AN_BDCOR)) * ((LONG)cfg.maxbd + 2));
    bd_cor = (AN_BDCOR *)AllocMem(tlength);
    if (bd_cor == NULL) {
        cmn_err(CE_WARN,"AllocMem() bd_cor fail");
        return -1;
    }
#ifdef LINUX
    bzero(bd_cor,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)bd_cor,ctob(btoc(tlength)));
#endif

   /* allocate memory for physical rcu array */
    tlength = (LONG)((LONG)PRCUSIZE * (LONG)cfg.maxrcu);
    p_rcu = (PRCU *)AllocMem(tlength);
    if (p_rcu == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_rcu fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_rcu,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_rcu,ctob(btoc(tlength)));
#endif

#ifdef RTDL_ANT /* allocate memory for busy-rcus array */
	tlength = (LONG)cfg.maxrcu;
	busy_rcu = (BYTE *)AllocMem(tlength);
	if (busy_rcu == NULL) {
		cmn_err(CE_WARN, "AllocMem() busy_rcu fail");
		return -1;
	}
	for (i = 0; i < cfg.maxrcu; i++) {
		busy_rcu[i] = FALSE;
	}
#ifdef LINUX
    bzero(busy_rcu,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(NT_386))
    bzero((caddr_t)busy_rcu,ctob(btoc(tlength)));
#endif
    firstAnStart = TRUE;
#endif

   /* allocate memory for pseudo rcu array */
    tlength = (LONG)((LONG)OPRCUSIZE * ((LONG)cfg.maxopenedrcu + 2));
    p_orcu = (O_PRCU *)AllocMem(tlength);
    if (p_orcu == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_orcu fail");
        return -1;
    }
#ifdef LINUX 
    bzero(p_orcu,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_orcu,ctob(btoc(tlength)));
#endif

   /* allocate memory for unix translate pseudo rcu array */
    tlength = (LONG)((LONG)(sizeof(AN_RCUCOR)) * ((LONG)cfg.maxopenedrcu + 2));
    rcu_cor = (AN_RCUCOR *)AllocMem(tlength);
    if (rcu_cor == NULL) {
        cmn_err(CE_WARN,"AllocMem() rcu_cor fail");
        return -1;
    }

#ifdef LINUX
    bzero(rcu_cor,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)rcu_cor,ctob(btoc(tlength)));
#endif
#if 0
   /* allocate memory for dpi array */
    tlength = (LONG)((LONG)RDPISIZE * (LONG)sys.maxdpi);
    p_rdpi = (RDPI *)AllocMem(tlength);
    if (p_rdpi == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_rdpi fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_rdpi,tlength);
#endif 
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_rdpi,ctob(btoc(tlength)));
#endif /* LINUX */

   /* allocate memory for hook info array */
    sys.maxhookinfo = cfg.maxopenedrcu;
    tlength = (LONG)((LONG)HOOKINFOSIZE * (LONG)sys.maxhookinfo);
    p_hookinfo = (HOOK_INFO *)AllocMem(tlength);
    if (p_hookinfo == NULL) {
        cmn_err(CE_WARN,"AllocMem() p_hookinfo fail");
        return -1;
    }
#ifdef LINUX
    bzero(p_hookinfo,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)p_hookinfo,ctob(btoc(tlength)));
#endif
#endif

   /* allocate memory for Cyclic algorithm for scheduler functions */

   /* Scheduler cycle array size */
    SchSize = (SHORT)(cfg.maxbd * 2);

    tlength = (LONG)SchSize;
    sch_arr = (BYTE *)AllocMem(tlength);
    if (sch_arr == NULL) {
        cmn_err(CE_WARN,"AllocMem() sch_arr fail");
        return -1;
    }
#ifdef LINUX
    bzero(sch_arr,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)sch_arr,ctob(btoc(tlength)));
#endif
    flsh_arr = (BYTE *)AllocMem(tlength);
    if (flsh_arr == NULL) {
        cmn_err(CE_WARN,"AllocMem() flsh_arr fail");
        return -1;
    }
#ifdef LINUX
    bzero(flsh_arr,tlength);
#endif /* LINUX */
#if (defined(SVR3) || defined(WNT_ANT))
    bzero((caddr_t)flsh_arr,ctob(btoc(tlength)));
#endif
/**************************************************************/
/* End of allocating memory for ANTARES variables             */
/**************************************************************/

   /* Initiates time constants according to system rate */
    rc = InitTimeValues();
    if (rc != E_SUCC) {
        cmn_err(CE_WARN,"Error initiating ANTARES timer interrupt routine: ");
        return(-1);
    }
#ifdef AIX_ANT
    pin((caddr_t) &ant_offl_int, sizeof(struct intr));

    if (in_map == 0) {
        in_map = 1;

        iomap.key = IO_MEM_MAP;
        iomap.flags = 0;                                
        iomap.size = 128;                            
        iomap.bid = BID_VAL(IO_ISA,ISA_IOMEM,BID_NUM(DLGN_BID));
        iomap.busaddr = (long long) A4_MIN_PORT;

        virtual_port = (WORD *) iomem_att(&iomap);
        if (virtual_port == (WORD *) NULL) {
            cmn_err(CE_WARN,"iomem_atti() fail in anstartpm()");
        }
    }
    else {
        in_map++;
    }
#endif
   /* Initiates addresses for hardware */
    init_hardware(cfg.intrdwrt);
 
   /* Initiates addresses for protocols */
    rc = init_protocol((VOID **)(&(p1)),(VOID **)(&(p2)),(VOID **)(&(p3)));
    if (rc != E_SUCC) {
        cmn_err(CE_WARN,"Antares_Board declared wrong in configuration file !");
 
#ifdef AIX_ANT
        if (in_map == 1) {
            iomem_det((WORD *) virtual_port);
            virtual_port = (WORD *) NULL;
            in_map = 0;
        }
        else {
            in_map--;
        }
#endif
        return(-1);
    }

   /* init intrface variables */
    init_interface((CFG_INFO *)&cfg);

   /* Point to the AN_BOARD structure for this BOARD */
    anbp = Gbdp2Anbp(tgnbdp);	/* = tgnbdp->bd_devdepp */

   /* Initialize the required AN_BOARD members */
    anbp->genbdp = tgnbdp;

    call_interrupt();
#ifdef AIX_ANT
    if (in_map == 1) {
        iomem_det((WORD *) virtual_port);
        virtual_port = (WORD *) NULL;
        in_map = 0;
    }
    else {
        in_map--;
    }
#endif
#if (!defined(VME_ANT) && !defined(SPARC_PCI))
   /*
    * Check interrupt number specified for board by 
    * GENworld in the form that the hardware uses.
    */
#ifdef DBGPRNT
       cmn_err(CE_CONT,"DAM anstartpm: tgnbdp->bd_irq: %d\n",tgnbdp->bd_irq);
#endif
    if (AN_INVALID_IRQ_LVL(tgnbdp->bd_irq)) {
        cmn_err(CE_WARN,"anstartpm: bad irq level, cannot start DAM");
        return (-1);
    }
#endif

   /*
    * Check number of logical devices on this BOARD:
    */
    for (ldp = tgnbdp->bd_ldp, nlogdevs = 0;
         ldp != NULL;
         ldp = ldp->ld_nldp, nlogdevs++)
    ;

    if (tgnbdp->bd_nlogdevs != nlogdevs) {
        cmn_err(CE_WARN,
                "anstartpm: Invalid logdev count, cannot start DAM");
        return (-1);
    }

#ifdef WNT_ANT
    Ant_State.AntDpcState[cfg.hwintr] = ANT_DPC_STATE_INITED;

    for (i = 2; i < MAX_INTR_VECS; i++) {
	KeInitializeDpc(&Ant_State.AntDpc[i], AntInterruptDpc, (PVOID)0);
    }
#endif

    sys.an_state = NotReady;          /* Is device active or not */
    
   /* Return SUCCESS */
   return (0);
}

/***********************************************************************
 *        NAME: anstoppm
 * DESCRIPTION: DAM shutdown routine
 *      INPUTS: bdp - pointer to generic BOARD structure
 *            : nbds - number of boards on this DAM
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int
anstoppm(
   GN_BOARD *gnbdp,
   ULONG    nbds
)
{
   GN_BOARD    *tgnbdp;
   GN_BOARD    *egnbdp;
   GN_LOGDEV   *ldp;

#ifndef LFS
    SHORT rc,val;
    LONG memsize = 0;
#endif
    LONG tlength;
#ifndef LFS
    int i;
#endif

   /* Make sure DAM is running */
   if (sys.an_state == NotActive) {
      cmn_err(CE_WARN,"anstoppm: inactive, cannot halt");
      return (-1);
   }

   /* Make sure all LOGDEVs on all BOARDs are closed and stopped */
   egnbdp = gnbdp + nbds;
   for (tgnbdp = gnbdp; tgnbdp < egnbdp; tgnbdp++) {
      for (ldp = tgnbdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
         if (ldp->ld_flags & (LD_START | LD_OPEN)) {
            cmn_err(CE_WARN,"anstoppm: %s active, cannot halt DAM",
               ldp->ld_name);
            return (-2);
         }
      }
   }

   /* Mark the DAM as halted */
    sys.an_state = NotActive;

   /* deallocate memory for bulk data body array */
    tlength = (LONG)((LONG)BDSIZE * (LONG)cfg.maxbd);
    FreeMem((void *)p_bd,tlength);

   /* deallocate the address of recv_message buffer in driver memory   */ 
    tlength = (LONG)cfg.m_message;
    FreeMem((void *)p_message,tlength);

   /* deallocate address for ROUTE_BUFFER */
    if ((TS_SIZE * 2) >= MAPSLOT_SIZE) {
        tlength = (LONG)((cfg.maxslots + 2) * 2 * TS_SIZE * sizeof(ULONG));
    }
    else {
        tlength = (LONG)((cfg.maxslots + 2) * MAPSLOT_SIZE * sizeof(ULONG));
    }
    tlength += 24;
    FreeMem((void *)routebuffer,tlength);

   /* deallocate Buffer For SYNC answers */
    tlength = (LONG)MAXSYNCANSWER;
    FreeMem((void *)p_reply,tlength);

   /* deallocate memory for process array */
    tlength = (LONG)((LONG)PRCSIZE * (LONG)cfg.maxbd);
    FreeMem((void *)p_data,tlength);

   /* deallocate memory for physical rcu array */
    tlength = (LONG)((LONG)PRCUSIZE * (LONG)cfg.maxrcu);
    FreeMem((void *)p_rcu,tlength);

#ifdef RTDL_ANT /* deallocate memory for busy-rcus array */
	tlength = (LONG)cfg.maxrcu;
	FreeMem((void *)busy_rcu, tlength);
#endif

   /* deallocate memory for pseudo rcu array */
    tlength = (LONG)((LONG)OPRCUSIZE * ((LONG)cfg.maxopenedrcu + 2));
    FreeMem((void *)p_orcu,tlength);

   /* deallocate memory for unix translate pseudo rcu array */
    tlength = (LONG)((LONG)(sizeof(AN_RCUCOR)) * ((LONG)cfg.maxopenedrcu + 2));
    FreeMem((void *)rcu_cor,tlength);

   /* deallocate memory for Cyclic algorithm for scheduler functions */
   /* Scheduler cycle array size */
    SchSize = (SHORT)(cfg.maxbd * 2);
    tlength = (LONG)SchSize;
    FreeMem((void *)sch_arr,tlength);
    FreeMem((void *)flsh_arr,tlength);
#ifdef VME_ANT   
   /* Initiates addresses for protocols */
    rc = uninit_protocol();
    if (rc != E_SUCC) {
        cmn_err(CE_WARN,"Error reset Antares_Board!");
        return(-1);
    }
#endif
#ifdef WNT_ANT
    Ant_State.AntDpcState[cfg.hwintr] = FALSE;

    for (i = 0; i < cfg.nboards; i++) {
	GnUnmapPhysicalIoPort(p_board[i].port);
	GnUnmapPhysicalIoPort(p_board[i].port+2);
	GnUnmapPhysicalIoPort(p_board[i].port+4);
	GnUnmapPhysicalIoPort(p_board[i].port+6);
    }
#endif
#ifdef AIX_ANT
    unpin((caddr_t) &ant_offl_int, sizeof(struct intr));
#endif

  /* Return SUCCESS */
   cmn_err(CE_CONT, "Antares PM stopped\n");
   return (0);
}

/***********************************************************************
 *        NAME: anstartbd
 * DESCRIPTION: DAM board startup routine
 *      INPUTS: bdp - ptr to GN_BOARD for board to be started
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int anstartbd(bdp)
GN_BOARD *bdp;
{
   GN_LOGDEV   *ldp;
   AN_BOARD    *anbp;
   AN_LOGDEV   *anldp;

   /*
    * Get ptr to the device-dependent BOARD structure
    */
   anbp = Gbdp2Anbp(bdp);
   
   /*
    * Initialize all AN_LOGDEV structures associated with this board
    *
    * 1) Link back to its GN_LOGDEV
    * 2) Link back to parent AN_BOARD
    * 3) For logical device zero (ld_devnum == 0), set its device type
    *    to a raw (physical) device (dtype = ANDT_RAW) and all others
    *    to an unspecified type (dtype = ANDT_UNBOUND).  Their type will
    *    be set by DAM binding.
    * 4) Link AN_BOARD to list of AN_LOGDEVS to facilitate
    *    speedy Irq_list processing
    */
   main_ldp = bdp->bd_ldp;
   for (ldp = main_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
      anldp = Gldp2Anldp(ldp);
      anldp->genldp = ldp;
      anldp->anbp = anbp;
      anldp->dtype = ANDT_UNBOUND;
#ifndef _SCO_
      anldp->gnhndl = -1;
#endif /* _SCO_ */
      ldp->ld_flags |= LD_START;
      anldp->next_anld = (ldp->ld_nldp) ? Gldp2Anldp(ldp->ld_nldp) : NULL;    
   }
   anbp->anld_list = Gldp2Anldp(bdp->bd_ldp);

#ifdef _SCO_
   an_addirq(bdp);
#endif /* _SCO_ */

   /* Display the board start-up message */
   cmn_err(CE_CONT, "Antares PM started\n");

   return (0);
}

/***********************************************************************
 *        NAME: anstopbd
 * DESCRIPTION: DAM board shutdown routine
 *      INPUTS: bdp - ptr to GN_BOARD for board to be stopped
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS: 
 *    CAUTIONS:
 **********************************************************************/
int
anstopbd(
   GN_BOARD *bdp
) {
   GN_LOGDEV *ldp;

   /* Make sure that all LOGDEVs on this board are closed */
   for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
      if (ldp->ld_flags & LD_OPEN) {
         cmn_err(CE_WARN,"anstoppm: %s busy, cannot halt board",
                 ldp->ld_name);
         return (-2);
      }

   }

#ifdef _SCO_
   an_rmvirq(bdp);
#endif /* _SCO_ */
 

  /* Mark each LOGDEV as being stopped */
   for (ldp = bdp->bd_ldp; ldp != NULL; ldp = ldp->ld_nldp) {
      ldp->ld_flags &= ~LD_START;
   }

   return (0);
}

/***********************************************************************
 *        NAME: anopen
 * DESCRIPTION: DAM open routine
 *      INPUTS: ldp - ptr to GN_LOGDEV structure
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int anopen(GN_LOGDEV *ldp)
{
    AN_LOGDEV *anldp = Gldp2Anldp(ldp);
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif

   /* Return error if device not started */
    if (!(ldp->ld_flags & LD_START)) {
        cmn_err(CE_NOTE,"anopen: ld:%s not started", ldp->ld_name);
        return (-1);
    }

   /*
    * Antares logical devices can not have multiple opens
    */
    if (ldp->ld_flags & LD_OPEN) {
#if 0
        cmn_err(CE_NOTE,"anopen: ld=%s already open", ldp->ld_name);
#endif
        return (-1);
    }
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_enter(&inthw_lock);
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    ldp->ld_flags |= LD_OPEN;
    anldp->dtype = ANDT_RAW;

#ifdef _SCO_
    if (ldp->ld_devnum == ldp->ld_gbp->bd_ldp->ld_devnum) {
        main_ldp = ldp;
    }
#endif /* _SCO_ */

#if 0
    cmn_err(CE_CONT,"anopen: ldp=%s opened\n", ldp->ld_name);
#endif
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_exit(&inthw_lock);
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return (0);
}

/***********************************************************************
 *        NAME: anclose
 * DESCRIPTION: DAM close routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int anclose(
   GN_LOGDEV   *ldp,
   ULONG       flags)
{
    AN_LOGDEV   *anldp =  Gldp2Anldp(ldp);
    SHORT i, rc = 0;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif
#ifdef AIX_ANT
    struct io_map iomap;
#endif
   /* Return SUCCESS if already closed */
    if (!(ldp->ld_flags & LD_OPEN)) {
        cmn_err(CE_CONT,"anclose: ld:%s already closed\n",ldp->ld_name);
        return (0);
    }

   /* Clean up the queues on this LOGDEV */
#ifdef AIX_ANT
    oldspl = disable_lock(DLGN_HWPRI, &DLGN_INTHW_LOCK);
#else
#ifdef SOL_ANT
    mutex_enter(&inthw_lock);
    in_close = TRUE;
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    switch (anldp->dtype) {

      case ANDT_MSG:
         break;

      case ANDT_RAW:
         /* clean antares dependent logical device structures */
#ifdef AIX_ANT
          if (in_map == 0) {
              in_map = 1;

              iomap.key = IO_MEM_MAP;
              iomap.flags = 0;                                
              iomap.size = 128;                            
              iomap.bid = BID_VAL(IO_ISA,ISA_IOMEM,BID_NUM(DLGN_BID));
              iomap.busaddr = (long long) A4_MIN_PORT;

              virtual_port = (WORD *) iomem_att(&iomap);
              if (virtual_port == (WORD *) NULL) {
                  cmn_err(CE_WARN,"iomem_att() fail in anclose()");
              }
          }
          else {
              in_map++;
          }
#endif
          for (i = 0; i < cfg.maxbd; i++) {
              if (bd_cor[i].ldp == ldp) {
                  rc = p_bd_close(bd_cor[i].bdhandle);
              }
          } 

          for (i = 0; i < cfg.maxopenedrcu; i++) {
              if (rcu_cor[i].ldp == ldp) {
                  rc = p_an_close(rcu_cor[i].rhandle);
              }
          } 
#ifdef AIX_ANT
          if (in_map == 1) {
              iomem_det((WORD *) virtual_port);
              virtual_port = (WORD *) NULL;
              in_map = 0;
          }
          else {
              in_map--;
          }
#endif
          ldp->ld_flags &= ~LD_OPEN;
          anldp->dtype = ANDT_UNBOUND;
#ifndef _SCO_
          anldp->gnhndl = -1;
#endif /* _SCO_ */
#if 0
          cmn_err(CE_CONT,"DAM anclose: ld:%s closed rc=%d\n",ldp->ld_name,rc);
#endif
      break;

      case ANDT_UNBOUND:
         /* never made it far enough, DPI DAM bind failed */
         break;

   }

   /* Force the state to idle */
   ldp->ld_genstate = ldp->ld_idlestate;
#ifdef AIX_ANT
    unlock_enable(oldspl, &DLGN_INTHW_LOCK);
#else
#ifdef SOL_ANT
    in_close = FALSE;
    mutex_exit(&inthw_lock);
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
   DLGCSPLX(oldspl);
#endif
#endif
   return (rc);
}

/***********************************************************************
 *        NAME: anmemsz
 * DESCRIPTION: DAM memory size routine
 *      INPUTS: bdszp - ptr to variable for dev-dep BOARD struct size.
 *              ldszp - ptr to variable for dev-dep LOGDEV struct size.
 *     OUTPUTS: Stores sizes in locations given by the ptr args.
 *     RETURNS: always zero.
 *       CALLS: none
 *    CAUTIONS: none
 **********************************************************************/
int anmemsz(bdszp,ldszp,pmnamep)
ULONG *bdszp;
ULONG *ldszp;
char  **pmnamep;
{
   static char anname[] = "an";

#ifdef DBGPRNT
   cmn_err(CE_CONT,"anmemesz: name:%s bdsz:%d  ldsz:%d\n",
      anname, sizeof(AN_BOARD), sizeof(AN_LOGDEV));
#endif

   *bdszp = (ULONG) sizeof(AN_BOARD);
   *ldszp = (ULONG) sizeof(AN_LOGDEV);
   *pmnamep = anname;

   return (0);
}

#ifdef _STATS_
/***********************************************************************
 *        NAME: anstats
 * DESCRIPTION: DAM stats routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int anstats(mpp)
mblk_t **mpp;
{
#ifdef LFS
   cmn_err(CE_CONT,"anstats: mp:%p\n", *mpp);
#else
   cmn_err(CE_CONT,"anstats: mp:%x\n", *mpp);
#endif

   return (0);
}
#endif /* _STATS_ */
/***********************************************************************
 *        NAME: andebug
 * DESCRIPTION: DAM debug routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int andebug(mpp, sizep)
mblk_t **mpp;
int    *sizep;
{
#ifdef LFS
   cmn_err(CE_CONT,"andebug: mp:%p size:%d bytes\n", *mpp, *sizep);
#else
   cmn_err(CE_CONT,"andebug: mp:%x size:%d bytes\n", *mpp, *sizep);
#endif

   return (0);
}

#if (defined(SOL_ANT) || defined(AIX_ANT))
/***********************************************************************
 *        NAME: anioccmd
 * DESCRIPTION: Antares DAM write-side processing routine
 *      INPUTS: ldp - ptr to GN_LOGDEV device to receive command
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: *** MAY BE ENTERED AT INTERRUPT TIME ***
 **********************************************************************/
int anioccmd(
GN_LOGDEV *ldp,
mblk_t    *mp)
{    
    AN_LOGDEV *anldp = Gldp2Anldp(ldp);

    WORD fcn;
    WORD blksz;
    ADDR *parmp;
    SHORT rc;
    SHORT dev;
#ifdef AIX_ANT
    struct io_map iomap;
#endif
   /* Exit if no message */
    if (mp == NULL) {
        return (0);
    }

   /*
    * Exit if device not opened
    */
    if (!(ldp->ld_flags & LD_OPEN)) {
        cmn_err(CE_WARN,"anioccmd: Device: %s not open", ldp->ld_name);
        return (0);
    }

    switch (mp->b_datap->db_type) {
        default:
        break;
            
       /*
        * Process M_IOCTL here
        */
        case M_IOCTL:
            switch (((GN_CMDMSG *)mp->b_cont->b_rptr)->fw_msg_ident) {
                default:
                break;

                case AN_IOC_CMD:
                {
                    AN_IOCCMD *dcp;
                    struct iocblk *iocp = (struct iocblk *)mp->b_rptr;
                    mblk_t *dmp = (mblk_t *) mp->b_cont;
#ifdef AIX_ANT
                    mblk_t *ump = (mblk_t *) NULL;
#endif
                    mblk_t *tmp;
                    BYTE *usr_buf = (BYTE *) NULL; 
                    BYTE *data_buf = (BYTE *) NULL;
                    size_t dsize;
                    size_t datalen;

                    curr_gnhndl = ((GN_CMDMSG *)mp->b_cont->b_rptr)->cm_address.da_handle;
#ifdef AIX_ANT
                    ump = (mblk_t *) allocb(iocp->ioc_count,BPRI_HI);
                    if (ump == NULL) {
                        cmn_err(CE_WARN,"allocb() fail in anioccmd()");
                        an_reply(ldp,mp,(SHORT) curr_gnhndl,
                                 E_ALLOCMEM,0,ANDT_IOC);
                        return(0);
                    } 
                    usr_buf = (BYTE *) ump->b_rptr;
                    ump->b_wptr += iocp->ioc_count; 
#else
                    usr_buf = (BYTE *) AllocMem(iocp->ioc_count);
                    if (usr_buf == (BYTE *) NULL) {
                        cmn_err(CE_WARN,"AllocMem() fail in ansendcmd()");
                        an_reply(ldp,mp,(SHORT) curr_gnhndl,
                                 E_ALLOCMEM,0,ANDT_IOC);
                        return (0);
                    }
#endif
#ifdef SVR3
                    bzero((caddr_t)usr_buf,ctob(btoc(iocp->ioc_count)));
#endif
                    datalen = iocp->ioc_count;
                    for (tmp = dmp; tmp != (mblk_t *) NULL; tmp = tmp->b_cont) {
                        dsize = (size_t) tmp->b_wptr - (size_t) tmp->b_rptr;
                        bcopy((caddr_t)tmp->b_rptr,
                              (caddr_t) (&(usr_buf[iocp->ioc_count-datalen])),
                              (size_t) dsize);
                        datalen -= dsize;
                    }   

                    dcp = (AN_IOCCMD *) (&(usr_buf[0]));
                    fcn = dcp->andpicmd.cmd; 
                    blksz = dcp->andpicmd.cnt; 
                    parmp = dcp->andpicmd.msgp;

                    curr_gnhndl = dcp->gncmdmsg.cm_address.da_handle;
                    curr_ldp = ldp;

                    if (iocp->ioc_count > IOCMDS) {
                        data_buf = (BYTE *) (&(usr_buf[IOCMDS]));
                    }
#ifdef AIX_ANT
                    if (in_map == 0) {
                        in_map = 1;

                        iomap.key = IO_MEM_MAP;
                        iomap.flags = 0;                                
                        iomap.size = 128;                            
                        iomap.bid = BID_VAL(IO_ISA,ISA_IOMEM,BID_NUM(DLGN_BID));
                        iomap.busaddr = (long long) A4_MIN_PORT;

                        virtual_port = (WORD *) iomem_att(&iomap);
                        if (virtual_port == (WORD *) NULL) {
                            cmn_err(CE_WARN,"iomem_att() fail in anioccmd()");
                        }
                    }
                    else {
                        in_map++;
                    }
#endif
                    rc = drvrcmnd(fcn,blksz,parmp,data_buf);
#ifdef AIX_ANT
                    if (in_map == 1) {
                        iomem_det((WORD *) virtual_port);
                        virtual_port = (WORD *) NULL;
                        in_map = 0;
                    }
                    else {
                        in_map--;
                    }
#endif
                    datalen = iocp->ioc_count;
                    for (tmp = dmp; tmp != (mblk_t *) NULL; tmp = tmp->b_cont) {
                        dsize = tmp->b_wptr - tmp->b_rptr;
                        bcopy((caddr_t) (&(usr_buf[iocp->ioc_count-datalen])),
                              (caddr_t)tmp->b_rptr,
                              (size_t) dsize);
                        datalen -= dsize;
                    }   
#ifdef AIX_ANT
                    freeb(ump);
                    ump = (mblk_t *) NULL;
#else
                    FreeMem((void *) usr_buf,iocp->ioc_count);
#endif
                    usr_buf = (BYTE *) NULL; 

                    an_reply(ldp,mp,(SHORT) curr_gnhndl,
                             rc,0,ANDT_IOC);

                    break;
                }
            }
        break;
    }
    return(0);
}

/***********************************************************************
 *        NAME: ansendcmd
 * DESCRIPTION: Antares DAM write-side processing routine
 *      INPUTS: ldp - ptr to GN_LOGDEV device to receive command
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: *** MAY BE ENTERED AT INTERRUPT TIME ***
 **********************************************************************/
int ansendcmd(
GN_LOGDEV *ldp,
mblk_t    *mp)
{    
    AN_LOGDEV *anldp = Gldp2Anldp(ldp);

    WORD fcn;
    WORD blksz;
    ADDR *parmp;
    SHORT rc;
    SHORT dev;
    SHORT bd_dev;

    int dsize;
    LONG dataleft, bufsize;
    mblk_t *tmp;

   /* Exit if no message */
    if (mp == NULL) {
        return (0);
    }

   /*
    * Exit if device not opened
    */
    if (!(ldp->ld_flags & LD_OPEN)) {
        cmn_err(CE_WARN,"ansendcmd: Device: %s not open", ldp->ld_name);
        return (0);
    }

    switch (mp->b_datap->db_type) {
        default:
        break;
            
       /*
        * Process M_PROTO here
        */
        case M_PROTO:
            switch (((GN_CMDMSG *)mp->b_rptr)->fw_msg_ident) {
                default:
                break;

                case ANC_SETPARM:
                {
                    register GN_CMDMSG *ghp = (GN_CMDMSG *)mp->b_rptr;

                    an_reply(ldp,mp,ANE_ACK,
                             (SHORT) ghp->cm_address.da_handle,
                             0,ANDT_CFG);
                    break;
                }

                case ANC_BDCTL:
                {
                    register AN_BDCTL *bdctl = (AN_BDCTL *)mp->b_rptr;

                    if (bdctl->dir == ANT_BDWRITE) {
                        dataleft = bufsize = bdctl->datalen;
                        for (tmp = mp->b_cont; tmp != NULL; tmp = tmp->b_cont) {
                            dsize = tmp->b_wptr - tmp->b_rptr;
                            bcopy((caddr_t)tmp->b_rptr,
                                  (caddr_t) &(p_bd[bdctl->loc].rcu_buf[bdctl->cr].buf[bufsize-dataleft]),
                                  (size_t) dsize);
                            dataleft -= dsize;
                        } 
                    } 
                    ConfirmFill(&bdctl->p_bd_entry,bdctl->loc,
                                (SHORT)(*(SHORT *)bdctl->gncmdmsg.cm_rfu),
                                 bdctl->cr); 
                    freemsg(mp);
                    break;
                } 
            }
        break;
   }
   return (0);
}

/***********************************************************************
 *        NAME: anioctl
 * DESCRIPTION: DAM ioctl routine
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int anioctl(char *device, int cmd, char *arg, char **arg2)
{
    PM_MEMSZ   *pm_memszp;
    PM_DEBUG   *pm_debugp;
    GN_LOGDEV  *ldp;

    switch(cmd) {
        case PM_GET_PENDING_QUEUE_BOARD:
	    ldp = (GN_LOGDEV *)device;
#if 0
                *arg2 = (char *)
		   ((DX_BOARD *)(ldp->ld_gbp->bd_devdepp))->gbp_same_fwi_addr;
#endif
	break;
	case PM_DEQUEUE_DPC:
#if 0
		sr_reset_device((GN_LOGDEV *)device);
#endif
	break;
	case PM_MEMSZ_CMD:
	    pm_memszp = (PM_MEMSZ *)arg;
	    anmemsz(&pm_memszp->bdsz, &pm_memszp->ldsz, 
						(char **)&pm_memszp->pmname);
	break;
	case PM_DEBUG_CMD:
	    pm_debugp = (PM_DEBUG *)arg;
	    andebug(pm_debugp->gn_debug, (int *)&pm_debugp->size); 
	break;
	default:
	return;
    }
}
#else
/***********************************************************************
 *        NAME: ansendcmd
 * DESCRIPTION: Antares DAM write-side processing routine
 *      INPUTS: ldp - ptr to GN_LOGDEV device to receive command
 *              mp - buffer containing the command
 *     OUTPUTS:
 *     RETURNS: 
 *       CALLS: 
 *    CAUTIONS: *** MAY BE ENTERED AT INTERRUPT TIME ***
 **********************************************************************/
int ansendcmd(
GN_LOGDEV *ldp,
mblk_t    *mp)
{    
#ifndef LFS
    AN_LOGDEV *anldp = Gldp2Anldp(ldp);
#endif
 
    WORD fcn;
    WORD blksz;
    ADDR *parmp; 
    SHORT rc;
#ifndef LFS
    SHORT dev;
#endif
    SHORT bd_dev;
 
    int dsize;
    LONG dataleft, bufsize;
    mblk_t *tmp; 
 
   /* Exit if no message */
    if (mp == NULL) {
        return (0);
    }
 
   /*
    * Exit if device not opened
    */
    if (!(ldp->ld_flags & LD_OPEN)) {
        cmn_err(CE_WARN,"ansendcmd: Device: %s not open", ldp->ld_name);
        return (0);
    }
 
    switch (mp->b_datap->db_type) {
        default:
            cmn_err(CE_WARN,"ansendcmd: %s: Unknown message type, ignored",
                    ldp->ld_name);
            freemsg(mp); 
        break;
       /*
        * Process FLUSH here: always ignored
        */
        case M_FLUSH:
            freemsg(mp); 
        break;
 
       /*
        * Process IOCTL here: always NAK'ed
        */
        case M_IOCTL:
        break;
 
       /*
        * All DAM primitives are M_PROTO
        */
        case M_PROTO:
            switch (((GN_CMDMSG *)mp->b_rptr)->cm_msg_type) {
                default:
                break;
            
               /*
                * Process MT_CONTROL here
                */
                case MT_CONTROL:
                    switch (((GN_CMDMSG *)mp->b_rptr)->fw_msg_ident) {
                        default:
                        break;
 
                        case ANC_SETPARM:
                        {
                            register GN_CMDMSG *ghp = (GN_CMDMSG *)mp->b_rptr;
 
                            an_reply(ldp,mp,ANE_ACK,
                                     (SHORT) ghp->cm_address.da_handle,
                                     				0,ANDT_CFG);
                            break;
                        }

                        case ANC_CMD:
                        {
                            register AN_SNDCMD *dcp = (AN_SNDCMD *)mp->b_rptr;
                            register mblk_t *dmp;

                            fcn = dcp->andpicmd.cmd;
                            blksz = dcp->andpicmd.cnt;
                            parmp = dcp->andpicmd.msgp;
                            curr_gnhndl = dcp->gncmdmsg.cm_address.da_handle;
                            curr_ldp = ldp;
                            dmp = mp->b_cont;

                            rc = drvrcmnd(fcn,blksz,parmp,(mblk_t *) dmp);
                            an_reply(ldp,mp,(SHORT) curr_gnhndl,
                                     rc,dcp->rep,ANDT_RAW);

                            break;
                        }

                        case ANC_PLAYCMD:
                        {
                            register AN_SND_PLAYCMD *playcmd = (AN_SND_PLAYCMD *)mp->b_rptr;

                            fcn = playcmd->andpicmd.cmd;
                            blksz = playcmd->andpicmd.cnt;
                            parmp = playcmd->andpicmd.msgp;
                            curr_gnhndl = playcmd->gncmdmsg.cm_address.da_handle;
                            curr_ldp = ldp;
                            bd_dev = *((SHORT *) (&(parmp[0])));

                            rc = UpdateStart(&playcmd->anbdchan,bd_dev);
                            if (rc != E_SUCC) {
                                an_reply(ldp,mp,(SHORT) curr_gnhndl,
                                         rc,playcmd->rep,ANDT_PRAW);
                                break;
                            }

                            dataleft = bufsize = p_bd[bd_dev].rcu_buf[0].size;
                            for (tmp = mp->b_cont; tmp != NULL; tmp = tmp->b_cont) {
                                dsize = tmp->b_wptr - tmp->b_rptr;
                                bcopy((caddr_t)tmp->b_rptr,
                                      (caddr_t) &(p_bd[bd_dev].rcu_buf[0].buf[bufsize-dataleft]),
                                      (size_t) dsize);
                                dataleft -= dsize;
                            }

                            rc = drvrcmnd(fcn,blksz,parmp,(mblk_t *) NULL);

                            an_reply(ldp,mp,(SHORT) curr_gnhndl,
                                     rc,playcmd->rep,ANDT_PRAW);
                            break;
                        }

                        case ANC_BDCTL:
                        {
				    register AN_BDCTL *bdctl = (AN_BDCTL *)mp->b_rptr;

                            if (bdctl->dir == ANT_BDWRITE) {
                                dataleft = bufsize = bdctl->datalen;
                                for (tmp = mp->b_cont; tmp != NULL; tmp = tmp->b_cont) {
                                    dsize = tmp->b_wptr - tmp->b_rptr;
                                bcopy((caddr_t)tmp->b_rptr,
                                      (caddr_t) &(p_bd[bdctl->loc].rcu_buf[bdctl->cr].buf[bufsize-dataleft]),
                                      (size_t) dsize);
                                    dataleft -= dsize;
                                }
                            }
                            ConfirmFill(&bdctl->p_bd_entry,bdctl->loc,
                                        (SHORT)(*(SHORT *)bdctl->gncmdmsg.cm_rfu),                                         bdctl->cr);
                            freemsg(mp);
                            break;
                        }
                    }
                break;
           }
       break;
   }
   return (0);
}

#endif

#if (defined(SOL_ANT) || defined(AIX_ANT))

BYTE in_inter = FALSE;
BYTE in_fatal = FALSE;

/*******************************************************************
 *       NAME : anproccmd()
 *DESCRIPTION : Hardware interrupt message processor.
 *      INPUT : None.
 *     OUTPUT : None.
 *    RETURNS : Nothing.
 *   CAUTIONS : None.                                 
 *******************************************************************/
VOID anproccmd(int d_irq_board)
{
   /*
    ** Service the message passed to you from the hardware interrupt
    ** handler.
    */
    WORD port;
    BYTE i,j,nbdcb,limit;
    UINT32 addr;
    UINT32 bdopen[2];
    UINT32 mask;
    UINT32 ErrorCode[6];

   /* Get board port */
    port = p_board[d_irq_board].port;

    ErrorCode[0] = p_board[d_irq_board].board_id;
#if 0
    cmn_err(CE_CONT,"anproccmd:d_irq_board=%d\n",d_irq_board);
#endif
   /* Checking DSP Fatel error */
    ErrorCode[1] = A4_read1Word(port, (A4_CFGParm(ERRORMASK)));
    if (ErrorCode[1] != 0) {	/* Error found */
	addr = A4_CFGParm(ERRORCODE);
	A4_readData(port,addr,(WORD)4,(UINT32 far *)(&(ErrorCode[2])));

       /* Saves last state of DSP's */
	for (i = 0,j = 0; i < p_board[d_irq_board].ndsp; i++) {
	    if ((p_board[d_irq_board].DSPerror[i] == E_SUCC) && 
		(ErrorCode[i+2] != E_SUCC)) { 
	  	p_board[d_irq_board].DSPerror[i] = ErrorCode[i+2];
		j++;
	    }
	}

	if (j > 0) {
	    send_dsp_msg(TRUE,p_board[d_irq_board].board_id,
		          24,
			 E_FATALDSPERROR,
			 1000,
			 (ADDR *)(&(ErrorCode[0])),
			 NO_RCU,
			 E_FATALDSPERROR,
			 p_rcu[0].l_rcu.grcu
			 );
	}
    }

    if (!(ask_token(port))) {	/* get dsp-pc token */
	send_error_msg(p_board[d_irq_board].board_id,
                       NO_RCU,0,E_DSPNOTOKEN,__LINE__);
	return;
    }

   /* Get message mask, save it, and zero board_message_mask */
    addr = A4_read1Word(port, (A4_CFGParm(MESSAGEMASK)));
    mask = (addr & 0xFFFFFF0F);
    A4_writeData(port,(A4_CFGParm(MESSAGEMASK)),1,(ULONG far *)&mask);
    A4_releaseToken(port); /* release token */
    addr = (addr >> 4);

   /* checking available message for each dsp in board */
    for (i = 0; i<p_board[d_irq_board].ndsp; i++) {
  	if (p_board[d_irq_board].DSPerror[i] != E_SUCC) {
	    continue;
	}

        if (((addr >> i) & 1) == TRUE) {
	   /* If DSP bit is on process message protocol	*/
  	    get_message((BYTE)d_irq_board,i);
        }
    }

   /* Only if ready or active states */
    if (sys.an_state < (BYTE) Ready) {  
	return;
    }

   /* checking bulk data */
   /* Reads Bulk data control block ready_mask */
    addr = A4_CFGParm(BDBUFMASK);
    A4_readData(port,addr,(WORD)2,(UINT32 far *)(&(bdopen[0])));

    limit = p_board[d_irq_board].nbdstream;
   /* checking bd_buffer mask */
    nbdcb = 0;

    j = 0;
    if (bdopen[j] == 0) {   /* No calls at ULONG of bd_mask */
	j++;
	nbdcb += 32;
	if (bdopen[j] == 0) {
	    return;
	}
    }

   /* For each bit in the mask, if ready performs the bulk data protocol */
    for ( ; j <= 1; j++) {
	mask = 1L;
	for (i = 0; i<= 31; i++,nbdcb++) {
	    if (i >= limit) {
	       /* Not checking more than available bulk data streams */
	  	return;
	    }
	   /* finds ready bit in mask */
	    if ((bdopen[j] & mask) != 0) {
	       /* Call bulk data protocol procedure */
	        bd_protocol((BYTE)d_irq_board,nbdcb,mask);
	    }
	    mask <<= 1;
	}
	if (bdopen[1] == 0) {
	    break;
	}
	limit -= 32;
    }
    return;
}

/**********************************************************************
 *        NAME:anrecvcmd 
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:  
 **********************************************************************/
int anrecvcmd(int irq_board)
{
#ifndef VME_ANT
    int d_irq_board;
#endif
#ifdef AIX_ANT
    struct io_map iomap;
#if 1
    int ipri;
    ipri = disable_lock(DLGN_HWPRI, &DLGN_INTHW_LOCK);
#endif
    lock_write(&DLGN_INTDRV_LOCK);

    if (in_map == 0) {
        in_map = 1;

        iomap.key = IO_MEM_MAP;
        iomap.flags = 0;                                
        iomap.size = 128;                            
        iomap.bid = BID_VAL(IO_ISA,ISA_IOMEM,BID_NUM(DLGN_BID));
        iomap.busaddr = (long long) A4_MIN_PORT;

        virtual_port = (WORD *) iomem_att(&iomap);
        if (virtual_port == (WORD *)NULL) {
            printf("ERROR - iomem_atti() returned NULL");
        }
    }
    else {
        in_map++;
    }
#endif /* AIX */

#ifdef VME_ANT
    if (p_board[irq_board].ready >= BOARD_LOADED) { 
       /* if board in legal state for work */
       /* Schedule a software interrupt to handle the event. */
        anproccmd(irq_board);
    }
#else /* not VME */
#ifdef RTDL_ANT  /* clear master brd intr only if BOARD_LOADED */
    if (p_board[cfg.MasterBoard].ready >= BOARD_LOADED) { 
#ifndef SPARC_PCI_ANT /* Fix clearing interrupt twice problem */
       /* Clear the Master Board interrupt */
        clear_irq(p_board[cfg.MasterBoard].port);   
#endif
    }
#else
#ifndef SPARC_PCI_ANT /* Fix clearing interrupt twice problem */
   /* Clear the Master Board interrupt */
    clear_irq(p_board[cfg.MasterBoard].port);   
#endif
#endif /* RTDL */

    for (d_irq_board = 0; d_irq_board < cfg.nboards; d_irq_board++) {
         if (d_irq_board != cfg.MasterBoard) {
#ifdef RTDL_ANT  /* clear all brds intr only if BOARD_LOADED */
             if (p_board[d_irq_board].ready >= BOARD_LOADED)
#endif
	     {
#ifndef SPARC_PCI_ANT /* Fix clearing interrupt twice problem */
                 clear_irq(p_board[d_irq_board].port); /* Clear the interrupt */
#endif
             }
         }
    }

    for (d_irq_board = 0; d_irq_board < cfg.nboards; d_irq_board++) {
       /* Process the received message */
#ifdef RTDL_ANT /* Process the received message only if BOARD_LOADED */
        if (p_board[d_irq_board].ready >= BOARD_LOADED)
#endif
	{
           /* if board in legal state for work */
           /* Schedule a software interrupt to handle the event. */
            anproccmd(d_irq_board);
        }
    }
#endif /* not VME */
#ifdef AIX_ANT
    if (in_map == 1) {
        iomem_det((WORD *) virtual_port);
        virtual_port = (WORD *) NULL;
        in_map = 0;
    }
    else {
        in_map--;
    }

    lock_done(&DLGN_INTDRV_LOCK);
#if 1
    unlock_enable(ipri, &DLGN_INTHW_LOCK);
#endif
#endif
    return(0);
}
#endif 

#ifdef WNT_ANT
/***********************************************************************
 *        NAME: AntInterruptDpc
 * DESCRIPTION: Antares Deferred Procedure Call Function
 *      INPUTS: vecnum - the IRQ vector number
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
VOID
AntInterruptDpc(PKDPC Dpc,
PVOID DeferredContext,
PVOID SystemArgument1,
PVOID SystemArgument2)
{
    int vecnum;
    KIRQL oldspl;

    vecnum = (int) SystemArgument1;

    DLGCSPLSTR(oldspl);
#if 0
    cmn_err(CE_NOTE, "\nIn AntInterruptDpc, vec %d\n", vecnum);
#endif
    if (DlgnExecutingDpc[vecnum] == TRUE) {
	cmn_err(CE_CONT, "AntInterruptDpc: Already Executing...\n");
	DLGCSPLX(oldspl);
	return;
    }
    anrecvcmd(vecnum);
}
#endif

#if (defined(SOL_ANT) || defined(AIX_ANT))
/***********************************************************************
 *        NAME: anintr
 * DESCRIPTION: Antares ISA interrupt handler routine
 *      INPUTS: vecnum - the IRQ vector number
 *     OUTPUTS: none
 *     RETURNS: The number of devices serviced; 0 if no devices serviced
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int anintr(int vecnum)
{
    int d_irq_board, rc = 0;

#ifdef PCI_ANT
    /* vecnum configured for antares isr is 0 for pci and isa */
    if (vecnum)
        return(0);

    vecnum = cfg.hwintr;

    /* for now, reset edge/level flip-flop for all antares boards */
    for (d_irq_board = 0; d_irq_board < cfg.nboards; d_irq_board++) {
	if (!(p_board[d_irq_board].port & _A4_16BIT_PORT))
            clear_irq_isr(p_board[d_irq_board].port);
    }
#endif /* PCI_ANT */

    rc = anisr(vecnum);

    return(rc);
}

int anisr(int vecnum)
{
    int irq_board = -1; 
#ifdef SOL_ANT
    int rc;
#endif
#ifdef VME_ANT
    int i;

    for (i = 0; i < cfg.nboards; i++) {
        if (p_board[i].slot_id == vecnum) { 
            irq_board = i;
            break;
        }
    }
    if (irq_board == -1) {
        return(0);
    }
#else
    if (cfg.hwintr != vecnum) {
        return(0);
    }
    else {
        irq_board = vecnum;
    }
#endif

    if (in_inter) {
       /* In case of interrupt problem */
       /* Interrupt occured inside interrupts */
	in_fatal = TRUE;
#ifdef DBGPRNT
#if 0
	send_error_msg(p_board[0].board_id,NO_RCU,0,E_FATALERROR,__LINE__);
#endif
        cmn_err(CE_NOTE,"Got into nested interrupt situation!!!");
#endif
	return(0);
    }

    in_inter = TRUE;

    if (sys.an_state == NotActive) {
	in_inter = FALSE;
	return(0);
    }
#ifdef SOL_ANT
#ifdef VME_ANT
    clear_irq(p_board[irq_board].port);   /* Clear the interrupt */
#endif
    rc = dlgn_queue_DPC(anrecvcmd,irq_board);
    if (rc == 1) {
        cmn_err(CE_NOTE, "anisr: unable to schedule software interrupt.");
        return(0);
    }            
#elif AIX_ANT /* not SOL_ANT */
    ANT_INIT_OFFL(&ant_offl_int,anrecvcmd);

    i_sched(&ant_offl_int); 
#elif WNT_ANT /* not SOL_ANT and not AIX_ANT */
    if (Ant_State.AntDpcState[irq_board] == ANT_DPC_STATE_INITED) {
       KeInsertQueueDpc(&Ant_State.AntDpc[irq_board],(PVOID)irq_board,(PVOID)0);
    }
#else /* not SOL_ANT and not AIX_ANT and not WNT_ANT */
    anrecvcmd(irq_board);
#endif /* SOL_ANT */

   /* For interrupt queue error */
    if (in_fatal) {
	in_fatal = FALSE;
#ifdef SOL_ANT
#ifdef VME_ANT
        clear_irq(p_board[irq_board].port);   /* Clear the interrupt */
#endif
        rc = dlgn_queue_DPC(anrecvcmd,irq_board);
        if (rc == 1) {
            cmn_err(CE_NOTE, "anisr: unable to schedule software interrupt.");
            return(0);
        }            
#elif AIX_ANT /* not SOL_ANT */
        ANT_INIT_OFFL(&ant_offl_int,anrecvcmd);

        i_sched(&ant_offl_int); 
#elif WNT_ANT /* not SOL_ANT and not AIX_ANT */
        if (Ant_State.AntDpcState[irq_board] == ANT_DPC_STATE_INITED) {
	   KeInsertQueueDpc(&Ant_State.AntDpc[vecnum],(PVOID)vecnum,(PVOID)0);
        }
#else /* not SOL_ANT and not AIX_ANT and not WNT_ANT */
        anrecvcmd(irq_board);
#endif /* SOL_ANT */
    }
    in_inter = FALSE;

    return(1);
}

#else /* __SCO__ */
 
/***********************************************************************
 *        NAME: anintr
 * DESCRIPTION: Antares interrupt handler routine
 *      INPUTS: vecnum - the IRQ vector number
 *     OUTPUTS: none
 *     RETURNS: The number of devices serviced; 0 if no devices serviced
 *       CALLS:  
 *    CAUTIONS:  
 **********************************************************************/
int anintr(int vecnum)
{
    int rc;
    register AN_BOARD *anbp = An_Irq;
 
   /*
    * Assume all antares board use the same interrupt,
    * which is always true from 'antctl start'.
    */
    if (anbp == NULL) {
       return 0; 
    }
 
#ifdef PCI_ANT
    /* vecnum configured for antares isr is 0 for pci and isa */
    if (vecnum)
        return(0);
 
#ifndef PCI_INTR_DEBUG
    /* for now, reset edge/level flip-flop for all antares boards */
    for (rc = 0; rc < cfg.nboards; rc++) {
        if (!(p_board[rc].port & _A4_16BIT_PORT))
                AntPciIntrReset(p_board[rc].port);
    }
#endif /* no PCI_INTR_DEBUG */
 
    vecnum = anbp->genbdp->bd_irq;
 
#else
    if (anbp->genbdp->bd_irq != vecnum) {
        return 0;
    }
#endif /* PCI_ANT */
 
#ifdef DBGPRNT
     cmn_err(CE_CONT,"DAM: Got into anintr vecnum=%d bd_irq=%d\n",
                          vecnum,anbp->genbdp->bd_irq);
#endif
 
     drvrintr(cmsghdlr);
#if 1 /* Interrupt Sharing Fix */
     return 0;
#else
     return 1;
#endif
}
         

/***********************************************************************
 *        NAME: an_addirq
 * DESCRIPTION:  
 *      INPUTS:  
 *     OUTPUTS:  
 *     RETURNS:  
 *       CALLS:  
 *    CAUTIONS:  
 **********************************************************************/
void
an_addirq(
   GN_BOARD *bdp 
) {
   register AN_BOARD  *anbdp = Gbdp2Anbp(bdp);
   int                 vec = bdp->bd_irq;
   AN_BOARD           *irqp;
#ifndef LFS
   int                 oldspl;
#endif

#ifdef LFS
   DLGCDECLARESPL(oldspl);
#endif
 
#ifdef _MY_COMOUT_
   /*
    * If this board is already on a list, or if this board has
    * no IRQ number, or board is not in the active state, then
    * exit without adding it to the list.
    */
   if ((anbdp->next_irq != NULL)
       || (vec == 0)
       || (anbdp->state != ANBS_ACTIVE)) {
      return;
   }
#endif
 
#ifdef DBGPRNT
      cmn_err(CE_NOTE,"an_addirq: vec %d, anbdp->next_irq: %x",vec,anbdp->next_irq);
#endif
 
   if ((anbdp->next_irq != NULL)
       || (vec == 0)) {
      return;
   }
 
   DLGCSPLSTR(oldspl);
 
   /*
    * Put board at head of list
    */
   irqp = anbdp;
   irqp->next_irq = An_Irq;
   An_Irq = anbdp;
 
   DLGCSPLX(oldspl);
}

/***********************************************************************
 *        NAME: an_rmvirq
 * DESCRIPTION:
 *      INPUTS: bdp - GN_BOARD to remove
 *     OUTPUTS:
 *     RETURNS:
 *       CALLS:
 *    CAUTIONS:
 **********************************************************************/
void an_rmvirq(bdp)
GN_BOARD *bdp;
{
   register AN_BOARD **irqpp;
   register AN_BOARD  *target;  /* target AN_BOARD */
#ifndef LFS
   register AN_BOARD  *anbdp = Gbdp2Anbp(bdp);
   int vec = bdp->bd_irq;
   int oldspl;
#endif

#ifdef LFS
   DLGCDECLARESPL(oldspl);
#endif
 
#ifdef _MY_COMOUT_ /* could not tell a board on the end of list or
                    * not, and vec == 0 are tested before board
                    * is added to list.
                    */
   /*
    * If this board is NOT on the list, or if this board has
    * no IRQ number, then exit without removing it from the list.
    */
   if ((anbdp->next_irq == NULL) || (vec == 0)) {
      return;
   }
#endif
 
#ifdef DBGPRNT
   cmn_err(CE_NOTE,"Got in an_rmvirq()");
#endif
   DLGCSPLSTR(oldspl);
     
   /*
    * Search for matching GN_BOARD pointer on list
    */
   for (irqpp = &An_Irq; *irqpp; irqpp = &((*irqpp)->next_irq)) {
      if ((*irqpp)->genbdp == bdp) {
         target = *irqpp;
         *irqpp = (*irqpp)->next_irq;
         target->next_irq = NULL;
         break;
      }
   }
      
   DLGCSPLX(oldspl);
}

#endif /* __SCO__ */

/***********************************************************************
 *        NAME: an_reply
 * DESCRIPTION: This function returns a reply to the DLGN module via
 *              dlgn_reply().
 *      INPUTS: ldp - pointer to the ldp
 *              mp - pointer to the user request message
 *              rc - return code to return to the user
 *     OUTPUTS: reply has been sent to the user
 *     RETURNS: none
 *       CALLS: freemsg() an_setrc() dlgn_reply()
 *    CAUTIONS: 
 **********************************************************************/
void an_reply(
GN_LOGDEV *ldp,
mblk_t    *mp,
SHORT     rc,
SHORT     damrc,
ULONG      rep,
int dtype)
{
    mblk_t *nmp  = (mblk_t *) NULL;
    
    if (dtype == ANDT_CFG) {
       freemsg(mp);
       ldp->ld_cmdcnt = 0;
       if ((nmp = dlgn_findreply(ldp, ANC_SETPARM, 0)) == NULL) {
           cmn_err(CE_WARN,"an_reply: dlgn_findreply() : message pool full"); 
           return;
       }

       nmp->b_datap->db_type = M_PROTO;
       ((GN_CMDMSG *)nmp->b_rptr)->cm_msg_type = MT_VIRTEVENT;
       ((GN_CMDMSG *)nmp->b_rptr)->fw_msg_ident = ANC_SETPARM;
       ((GN_CMDMSG *)nmp->b_rptr)->fw_errcode = rc;
       ((GN_CMDMSG *)nmp->b_rptr)->cm_address.da_handle = damrc;

      *((LONG *)(&(((GN_CMDMSG *)nmp->b_rptr)->cm_rfu[0]))) = cfg.bd_single;
      *((SHORT *)(&(((GN_CMDMSG *)nmp->b_rptr)->cm_rfu[4]))) = cfg.maxbd;
      *((SHORT *)(&(((GN_CMDMSG *)nmp->b_rptr)->cm_rfu[6]))) = cfg.maxrcu;
      *((SHORT *)(&(((GN_CMDMSG *)nmp->b_rptr)->cm_rfu[8]))) = sys.maxdpi;

       dlgn_reply(ldp, nmp);
       return;
    }
#ifdef _SCO_
    if (dtype == ANDT_PRAW) {
       freemsg(mp);
       if ((nmp = dlgn_findreply(ldp, ANC_RC, AN_DRVRETSZ)) == NULL) {
           cmn_err(CE_WARN,"an_reply: dlgn_findreply() : message pool full");
           return;
       } 
 
       nmp->b_datap->db_type = M_PROTO;
       ((AN_SNDCMD *)nmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
       ((AN_SNDCMD *)nmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_RC;
       ((AN_SNDCMD *)nmp->b_rptr)->gncmdmsg.cm_address.da_handle = rc;
       ((AN_SNDCMD *)nmp->b_rptr)->gncmdmsg.fw_errcode = 0;
 
       ((AN_SNDCMD *)nmp->b_rptr)->ret = damrc;
       ((AN_SNDCMD *)nmp->b_rptr)->rep = rep;
 
       curr_ldp = (GN_LOGDEV *) NULL;
       curr_gnhndl = 0;
       ldp->ld_cmdcnt = 0;
       dlgn_reply(ldp,nmp);
       return;
    }

    if (dtype == ANDT_RAW) {
       mp->b_datap->db_type = M_PROTO;
       ((AN_SNDCMD *)mp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
       ((AN_SNDCMD *)mp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_RC;
       ((AN_SNDCMD *)mp->b_rptr)->gncmdmsg.cm_address.da_handle = rc;
       ((AN_SNDCMD *)mp->b_rptr)->gncmdmsg.fw_errcode = 0;

       ((AN_SNDCMD *)mp->b_rptr)->ret = damrc;
       ((AN_SNDCMD *)mp->b_rptr)->rep = rep;

       curr_ldp = (GN_LOGDEV *) NULL;
       curr_gnhndl = 0;
       ldp->ld_cmdcnt = 0;
       dlgn_reply(ldp,mp);
       return;
   }

#else
    if (dtype == ANDT_IOC) {
       mp->b_datap->db_type = M_IOCACK;
       ((AN_IOCCMD *)mp->b_cont->b_rptr)->gncmdmsg.cm_msg_type = MT_REPLY;
       ((AN_IOCCMD *)mp->b_cont->b_rptr)->gncmdmsg.fw_msg_ident = ANC_RC;
       ((AN_IOCCMD *)mp->b_cont->b_rptr)->gncmdmsg.cm_address.da_handle = rc;
       ((AN_IOCCMD *)mp->b_cont->b_rptr)->gncmdmsg.fw_errcode = 0;

       ((struct iocblk *)mp->b_rptr)->ioc_rval = damrc;
       ((struct iocblk *)mp->b_rptr)->ioc_error = 0;

       curr_ldp = (GN_LOGDEV *) NULL;
       curr_gnhndl = 0;
       ldp->ld_cmdcnt = 0;
       dlgn_reply(ldp,mp);
       return;
   }
#endif /* _SCO_ */

   if (dtype == ANDT_BDSTR) {
       ((AN_BDCTL *)mp->b_rptr)->gncmdmsg.fw_errcode = rc;
       dlgn_reply(ldp,mp);
       return;
   }

   if (dtype == ANDT_MSG) {
       ((AN_DRVCMD *)mp->b_rptr)->gncmdmsg.fw_errcode = rc;
       dlgn_reply(ldp,mp);
       return;
   }
}

void UpdateFill(BD_CHANNEL *bx,SHORT loc,BYTE cr)
{
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_enter(&inthw_lock);
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    p_bd[loc].rcu_buf[cr].eob = bx->rcu_buf[cr].eob;
    p_bd[loc].rcu_buf[cr].size = bx->rcu_buf[cr].size;

    p_bd[loc].pfile = bx->pfile;
    p_bd[loc].pfstart = bx->pfstart;
    p_bd[loc].poffset = bx->poffset;
    p_bd[loc].datalen = bx->datalen;

    if (p_bd[loc].bdstop == FALSE) {
	p_bd[loc].bdstop = bx->bdstop;
	p_bd[loc].HostStop = bx->HostStop;
    }

    if (p_bd[loc].onprocess == TRUE) {
	p_bd[loc].onprocess = bx->onprocess;
    }

    p_bd[loc].fillbuf = bx->fillbuf;
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_exit(&inthw_lock);
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

void ConfirmFill(BD_CHANNEL *bx,SHORT loc,SHORT rc,BYTE cr) 
{
#ifndef LFS
    SHORT err; 
#endif
#if 0
    if (rc > 0) {
	send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                          __LINE__,loc,bx->direction,ANT_ASYNC_ERROR,
                          rc,bx->datalen);

	UpdateFill(bx,loc,cr);

	return;
    }
#endif
    switch(rc) {
        case 0:              /* ??? nothing to do */
	break;

        case -1:             /* ??? nothing to do */
	break;

        case -2:
           /* Driver error: filling not_empty buffer */
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	                    __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_ERROR,
                            E_BDWRITE,bx->datalen);
	break;

        case -3:             /* Error seek in file */
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                          __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_ERROR,
                          E_ERRORSEEK,bx->datalen);
	break;

        case -4:             /* Error write to file */
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                         __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_ERROR,
                         E_BDREADFILE,bx->datalen);
	break;

        case  -5:            /* Pointer is null - sending erro message */
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                         __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_ERROR,
                         E_XPLAY_LINK,bx->datalen);
	break;

        case -6:
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                         __LINE__,loc,ANT_BDREAD,ANT_ASYNC_ERROR,
                         E_BDWRITEFILE,bx->datalen);
	break;

        case -7:
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                          __LINE__,loc,ANT_BDREAD,ANT_ASYNC_ERROR,
                          E_XPLAY_LINK,bx->datalen);
	break;

       /* Flush buffer errors */
	case -8:
            if (bx->bdstop) {
               /* record process stopped by user = readereod message*/
		send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	            __LINE__,loc,ANT_BDREAD,ANT_ASYNC_READEREOD,
                    bx->Completion,bx->datalen);
            }
            else {
               /* record process stopped by DSP = async cmplt message*/
		send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	                __LINE__,loc,ANT_BDREAD,ANT_ASYNC_CMPLT,
                        bx->Completion,bx->datalen);
            }
	break;

	case -9:
           /* Error message in case of I/O error */
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	                  __LINE__,loc,ANT_BDREAD,ANT_ASYNC_ERROR,
                          E_BDFLUSHBUF,bx->datalen);
	break;

	case -10:
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
                          __LINE__,loc,ANT_BDREAD,ANT_ASYNC_ERROR,
                          E_XPLAY_LINK,bx->datalen);
        break;

        case -100:
               /* Bulk data message when BDSWAP mode is on */
		send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	                __LINE__,loc,bx->direction,ANT_ASYNC_BDSWAP,
                        0,bx->datalen);
        break;

        default:
            send_bulkdata_msg(FALSE,p_board[bx->bnumber].board_id,
	                         __LINE__,loc,bx->direction,ANT_ASYNC_ERROR,
                                 rc,bx->datalen);
        break;
    }

    UpdateFill(bx,loc,cr);
    return;
}

#ifdef _SCO_
SHORT UpdateStart(BD_CHANNEL *bx,SHORT loc)
{
    BYTE cr;
 
    for (cr = 0; cr <= 1; cr++) {
        p_bd[loc].rcu_buf[cr].eob = bx->rcu_buf[cr].eob;
        p_bd[loc].rcu_buf[cr].size = bx->rcu_buf[cr].size;
    }
         
    p_bd[loc].pfile = bx->pfile;
    p_bd[loc].pfstart = bx->pfstart;
    p_bd[loc].poffset = bx->poffset;
    p_bd[loc].offset = bx->offset;
    p_bd[loc].datalen = bx->datalen;
 
    p_bd[loc].bdstop = bx->bdstop;
    p_bd[loc].HostStop = bx->HostStop;
    p_bd[loc].Completion = bx->Completion;
 
    return(E_SUCC);
}
#endif /* _SCO_ */
#ifdef WNT_ANT
VOID cmn_err(ULONG Level, ...)
{
    char  *fmt;
    ULONG   arg0;
    ULONG arg1;
    ULONG arg2;
    ULONG arg3;
    ULONG arg4;
    ULONG arg5;
    ULONG arg6;
    ULONG arg7;
    ULONG arg8;
    ULONG arg9;
    va_list marker;
    va_start(marker,Level);

    fmt = va_arg(marker,char*);
    arg0 = va_arg(marker,ULONG);
    arg1 = va_arg(marker,ULONG);
    arg2 = va_arg(marker,ULONG);
    arg3 = va_arg(marker,ULONG);
    arg4 = va_arg(marker,ULONG);
    arg5 = va_arg(marker,ULONG);
    arg6 = va_arg(marker,ULONG);
    arg7 = va_arg(marker,ULONG);
    arg8 = va_arg(marker,ULONG);
    arg9 = va_arg(marker,ULONG);
    va_end(marker);

    DbgPrint(fmt,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);

    return;
}
#endif
