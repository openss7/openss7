/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : drvprot.c
 * Description                  : driver protocol
 *
 *
 **********************************************************************/

#ifdef WNT_386
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stream.h>
#include <stropts.h>
#include <sys/cmn_err.h>
#else
#ifdef LINUX
#include "dlgclinux.h"
#define _SCO_
#else
#include <sys/types.h>
#include <sys/stream.h>
#ifndef AIX_ANT
#include <sys/cmn_err.h>
#else
#include "ant_aix.h" 
#endif
#endif /* LINUX */
#endif

#include "dlgcos.h"
#include "include.h"
#ifdef VME_ANT
#include "andam.h"
#endif

#include "protocol.h"
#include "intrface.h"
#include "anprot.h"

#include "hardexp.h"

#include "drvprot.h"
#include "extern.h" 

#ifdef WNT_ANT
extern VOID cmn_err(ULONG,...);
#endif

/* Internal functions prototyes */
VOID  Start_BDmemory(VOID);
SHORT putrcu(RCUSTRUC far *,BYTE);
VOID  sortrcu(RCUSTRUC far *);
SHORT A4_getDSPCount(WORD); 
SHORT init_board(BYTE);
SHORT init_address(BYTE);
SHORT init_rcus(BYTE);
SHORT searchprcu(VOID);
SHORT searchrcu(RCU far *,LONG  far *,SHORT);
SHORT control_message(SHORT,SHORT,SEND_MSG far *,CHAR far *,SHORT far *); 

#ifdef _SCO_
/* Internal driver functions prototyes */
VOID far drvrintr(VOID (far *imsgp)(VOID));
VOID far cmsghdlr(VOID);
VOID far drvrmsec(VOID);
#endif /* _SCO_ */

extern void *AllocMem(LONG);
extern void  FreeMem(void*,LONG);

/* Global variables for drvprot.c */
extern SYS_INFO	sys;	 /* System information */

#ifdef _SCO_
SHORT d_irq_board;       /* current interrupt board */
#else
extern BYTE in_inter;
extern BYTE in_fatal;
#endif /* _SCO_ */

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
#endif

#ifdef LINUX
#ifdef LFS
extern spinlock_t inthw_lock;
#else
extern lis_spin_lock_t inthw_lock;
#endif
#endif /* LINUX */

#ifdef RTDL_ANT  /* oneBoardID, free_rcu, flgBoardInit */
SHORT oneBoardID = (SHORT) -1;
SHORT free_rcu=0;      /* first free rcu entry in p_rcu array 
                          (instead of sys.availrcu) */
UINT32 dbgCode;
BYTE dbg_busy[200];    /* delete after debugging... */
SHORT flgBoardInit = TRUE;   /* flag: call p_bd_close() without calling 
                                bd_close() */
		/* meaning: we only want to init driver's database    */
		/* not board's databse (we just now downloaded board) */
extern SHORT firstAnStart;
#endif

/****************************************************************
*        NAME : InitTimeValues
* DESCRIPTION : Initiates system time variables according to TMR55
*       INPUT : void
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
SHORT InitTimeValues(VOID) 
{
    DspCall = 10000 * cfg.dspcall;
    TokenCall = 10000 * cfg.asktoken;
    return(E_SUCC);
}

/****************************************************************
*        NAME : init_protocol
* DESCRIPTION : Initiates all protocol variables. This procedure is 
*               called when driver is loaded, and it performs steps:
*		1) Initiation of bulk_data local variables and of message
*		   protocol local variables.
*               2) for each board - initiation of RCUs, and 
*                  auto_increament state.
*		3) Initiation of raw array (raw array may be used before
*		   call to p_an_start() (by antdump.exe).
*		4) Initiation of addresses for Antares driver functions.
*                     
*       INPUT : Pointers to Antares driver functions
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code.
*    CAUTIONS : 
****************************************************************/
SHORT init_protocol(VOID far * far *hw_intr,VOID far * far *hw_imsg,
                    VOID far * far *timer55)
{
    SHORT rc,err;

    init_bd_address();	 /* Initiation of bulk data local variables (antbd.c) */
    init_msg_address();	 /* Initiation of messages local variables (antlow.c) */

   /* Check all boards given from configuration file */
    for (rc = 0; rc < cfg.nboards; rc++) {
 	p_board[rc].ready = BOARD_NOTREADY;

       /* Is driver exist in address */
 	err = init_board((BYTE)rc);
 	if (err != E_SUCC) {
            p_board[rc].error_code = err;
cmn_err(0, "Board %d at 0x%x failed with err=%d\n", rc, p_board[rc].port, err);
	    return(err);
	}
	p_board[rc].ready = BOARD_EXIST;
#ifndef VME_ANT
       /* Set Auto_Increament according to autoinc option */
 	if (cfg.autoinc) {
            A4_setAutoIncrement(p_board[rc].port);
	}
 	else {
    	    A4_clearAutoIncrement(p_board[rc].port);
 	}

       /* Initiation of addresses on board */
  	err = init_address((BYTE)rc);
 	if (err == E_SUCC) {
  	    p_board[rc].error_code = err;
 	    p_board[rc].ready = BOARD_ADDRESSED;
 	}

#if 0
	else {	/* TCOM */
            p_board[rc].error_code = err;
	    return(err);
	}
#endif

#endif /* VME_ANT */
    }

   /* initiates Raw_device_handle_array */
    for (rc = 0; rc < MAXOPENRAW; rc++) {
	raw_open[rc] = -1;
    }

#ifdef _SCO_
   /* Makes Driver function public */
    *hw_intr = (VOID far *)drvrintr;
    *hw_imsg = (VOID far *)cmsghdlr;
    *timer55 = (VOID far *)drvrmsec;
#endif /* _SCO_ */

    return(E_SUCC);
}
#ifdef VME_ANT
/****************************************************************
*        NAME : uninit_protocol 
* DESCRIPTION :
*       INPUT : void
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
SHORT uninit_protocol(VOID)
{
    int bn, rc;

    for (bn = 0; bn < cfg.nboards; bn++) {
        rc = A4_set_A24mode(p_board[bn].slot_id,
                            (caddr_t) p_board[bn].kva);
        cmn_err(CE_CONT,"Release kva=0x%X\n",p_board[bn].kva);
        if (rc < 0) {
            return(-1);
        }
    }
    return(E_SUCC);
}
#endif
/****************************************************************
*        NAME : call_interrupt
* DESCRIPTION : Performs a call to Antares interrupt function.
*       INPUT : void
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
VOID call_interrupt(VOID)
{
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif
#ifdef VME_ANT
    int i;
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

#ifdef _SCO_
    drvrintr(cmsghdlr);    /* Address of intrpt msg hndlr */
#else
#ifdef VME_ANT
    for (i = 0; i < cfg.nboards; i++) {
        anisr(p_board[i].slot_id);
    }
#else
    anisr(cfg.hwintr);    /* Address of intrpt msg hndlr */
#endif
#endif /* _SCO_ */

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

/****************************************************************
*        NAME : start_memory
* DESCRIPTION : Initiates bulk data addresses.
*       INPUT : void
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code 
*    CAUTIONS : 
****************************************************************/
VOID Start_BDmemory(VOID)
{
    SHORT ind;

    for (ind = 0; ind < cfg.maxbd; ind++) {
	 p_bd[ind].direction = BDCLOSE;
    }

    return;
}

/****************************************************************
*        NAME : p_an_start
* DESCRIPTION : Initiation of global variables (done at start of any
*                 application which use the driver )
*       INPUT : number of boards, number of legal boards.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code.
*    CAUTIONS : 
****************************************************************/
SHORT p_an_start(SHORT far *bstate,SHORT far *lbstate)
{
   /* Bulk data semaphors & variables to be initiated */
    extern BYTE b_fisemaphore;	  
    extern BYTE b_flsemaphore;
    extern BYTE s_sch;
    extern BYTE e_sch;
    extern BYTE fs_sch;
    extern BYTE fe_sch;

    SHORT btmp,rc;
    BYTE  val;
#ifdef RTDL_ANT /* check for hot-download 'password' */
    SHORT minBrd, maxBrd;
    SHORT firstStart = firstAnStart;

    firstAnStart = FALSE;
    /* check for hot-donload 'password' */
    if ((firstStart == (SHORT)FALSE) && (*bstate == (SHORT) START_BOARDID)) {
       /* download only one board */
	oneBoardID = *lbstate;
	*bstate = cfg.nboards;
	*lbstate = 0;
    }
    else {
       /* 'old fashion' an_start: downloading all boards */
	oneBoardID = (SHORT)-1;
#endif

   /* initiation of boards details */
    sys.an_state = NotReady; /* Not ready for work */

    startrcu = 0;
    ustartrcu = 0;
    sys.crcu = 0;     /* current number of opened rcu's) */
    sys.cdpi = 0;     /* current number of DPI registered */
    sys.cbdata = 0;   /* current number of available bulk data channels */
    sys.availrcu = 0; /* current number of RCUs available */

    sys.raw_ind = 0;  /* current number of raws opened */

    b_fisemaphore = 0;  /* Zeroing bulk data semaphors */
    b_flsemaphore = 0;

   /* Bulk data cyclic algorithm variables */
    s_sch = e_sch = fs_sch = fe_sch = 0;

   /* initiates array of pseudo rcu's */
    for (rc = 0; rc < cfg.maxopenedrcu; rc++) {
	p_orcu[rc].index = -1;
    }

   /* For special message RCU that does not exist */
    p_orcu[cfg.maxopenedrcu].Class = SPECMESS;
    p_orcu[cfg.maxopenedrcu].Board = PSEUDOVALUE;
    p_orcu[cfg.maxopenedrcu].Dsp = 3;
    p_orcu[cfg.maxopenedrcu].Unit = 0;

   /* initiates bulk data buffers */
    Start_BDmemory();

   /* initiates Raw_device_handle_array */
    for (rc = 0; rc < MAXOPENRAW; rc++) {
	raw_open[rc] = -1;
    }

    *bstate = cfg.nboards;
    *lbstate = 0;
#ifdef RTDL_ANT /* if hot-download then execute only on downloaded board */
    }
    if (oneBoardID != (SHORT) -1) {
	minBrd = oneBoardID;
	maxBrd = oneBoardID+1;
    } 
    else {
	minBrd = 0;
	maxBrd = cfg.nboards;
    }
   /* if hot-download then execute only on downloaded board, 
      else loop on all boards */
    for (rc = minBrd; rc < maxBrd; rc++) {
#else
   /* initiation of all boards in system */
    for (rc = 0; rc < cfg.nboards; rc++) {
#endif	
        p_board[rc].error_code = E_SUCC;
#ifndef VME_ANT
       /* Checking board existance */
        btmp = init_board((BYTE)rc);
	if (btmp != E_SUCC) {
	    p_board[rc].error_code = btmp;
	    p_board[rc].ready = BOARD_NOTREADY;
	    return(btmp);
	}
#endif
       /* Checking legality of addresses on board */
	btmp = init_address((BYTE)rc);
	if (btmp != E_SUCC) {
            p_board[rc].error_code = btmp;
	    p_board[rc].ready = BOARD_EXIST;
	    continue;
	}

       /* Checking RCUs list on board */
	btmp = init_rcus((BYTE)rc);
	if (btmp != E_SUCC) {
	    p_board[rc].error_code = btmp;
	    p_board[rc].ready = BOARD_ADDRESSED;
#ifdef RTDL_ANT /* hot downloaded for 1 board only or all */
	    if (oneBoardID == (SHORT) -1) {
	       /* 'old-fashion' download for ALL boards */
		continue;
	    }
            else {
	       /* hot download for 1 board only */
		return(btmp);
	    }
#else
	    continue;
#endif
	}

	p_board[rc].ready = BOARD_LOADED;
	(*lbstate)++;

	A4_releaseToken(p_board[rc].port);	/* release token */

       /* if board ready: closes all bulk data opened channels */ 
  	if (p_board[rc].ready >= BOARD_ADDRESSED) {
            for (val = 0; val<p_board[rc].nbdstream; val++ ) {
	        bd_close((BYTE)rc,val);
	    }
	} 
    }

#if 0

/* Changed because of increament index every an_open() */
/* Increament incindex in order to distinguish from former application messages */
	if ((++incindex) == 0xFF) {
		incindex = 0;
	}
#endif

   /* Determines Driver working state */
    for (rc = 0; rc < cfg.nboards; rc++) {
  	if (p_board[rc].ready > BOARD_ADDRESSED) {
	   /* At least one board is ready */
	    sys.an_state = Active;
	    break;
	}
	else {
	    if (p_board[rc].ready == BOARD_ADDRESSED) {
	        sys.an_state = Ready;
	    }
        }		
    }

   /* Performing interrupt  */
    call_interrupt();
    return ((SHORT)(E_EXSUCC<<8));
}


#ifdef _SCO_

/*******************************************************************
 *       NAME : drvrintr(imsgp)
 *DESCRIPTION : Hardware specific interrupt handler.  This is the
 *              entrypoint to the driver when a hardware interrupt
 *              occurs.
 *      INPUT : imsgp = Address of the message handler.
 *     OUTPUT : None.
 *    RETURNS : Nothing.
 *   CAUTIONS : 8259 will be cleared by IDDS.
 *******************************************************************/
BYTE in_inter = FALSE;
BYTE in_fatal = FALSE;

VOID far drvrintr(imsgp)
VOID (far *imsgp)(VOID);
{
#ifndef LFS
        int     i;
#endif

        if (in_inter) {
                /* In case of interrupt problem */
        /* Interrupt occured inside interrupts */
                in_fatal = TRUE;
#if 0
                send_error_msg(p_board[0].board_id,NO_RCU,0,E_FATALERROR,__LINE__);
#endif
                return;
        }


        in_inter = TRUE;

        if (sys.an_state == NotActive) {
                in_inter = FALSE;
                return;
        }

        /* Clear the Master Board interrupt */
        clear_irq(p_board[cfg.MasterBoard].port);
 
   for (d_irq_board = 0; d_irq_board < cfg.nboards; d_irq_board++)
        {
                if (d_irq_board != cfg.MasterBoard) {
                        clear_irq(p_board[d_irq_board].port);   /* Clear the interrupt */
                }
        }
        /* For each board: clearing inmterrupt + process messahe procedure */
   for (d_irq_board = 0; d_irq_board < cfg.nboards; d_irq_board++)
        {
        /* Process the received message */
                if (p_board[d_irq_board].ready >= BOARD_LOADED) {
                   /* if board in legal state for work */
                        (*imsgp)();
                }
        }
        /* For interrupt queue error */
        if (in_fatal) {
                in_fatal = FALSE;
                (*imsgp)();
        }
        in_inter = FALSE;
        return;
}
/*******************************************************************
 *       NAME : cmsghdlr()
 *DESCRIPTION : Hardware interrupt message processor.
 *      INPUT : None.
 *     OUTPUT : None.
 *    RETURNS : Nothing.
 *   CAUTIONS : None.
 *******************************************************************/
VOID far cmsghdlr(VOID)
{
   /*
    * Service the message passed to you from the hardware interrupt
    * handler.
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


        /* Checking DSP Fatel error */
        ErrorCode[1] = A4_read1Word(port, (A4_CFGParm(ERRORMASK)));
        if (ErrorCode[1] != 0) {        /* Error found */
                addr = A4_CFGParm(ERRORCODE);
                A4_readData(port,addr,(WORD)4,(UINT32 far *)(&(ErrorCode[2])));

                /* Saves last state of DSP's */
                for (i = 0,j = 0; i<p_board[d_irq_board].ndsp; i++) {
                        if ((p_board[d_irq_board].DSPerror[i] == E_SUCC) &&
                                 (ErrorCode[i+2] != E_SUCC))
                        {
                                p_board[d_irq_board].DSPerror[i]        = ErrorCode[i+2];
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

        if (!(ask_token(port))) {       /* get dsp-pc token */
                send_error_msg(p_board[d_irq_board].board_id,NO_RCU,0,E_DSPNOTOKEN,__LINE__);
                return;
        }

        /* Get message mask, save it, and zero board_message_mask */
        addr = A4_read1Word(port, (A4_CFGParm(MESSAGEMASK)));
        mask = (addr & 0xFFFFFF0F);
        A4_writeData(port,(A4_CFGParm(MESSAGEMASK)),1,(ULONG far *)&mask);
        A4_releaseToken(port);  /* release token */
        addr = (addr >> 4);

       /* checking available message for each dsp in board */
        for (i = 0; i<p_board[d_irq_board].ndsp; i++) {
                if (p_board[d_irq_board].DSPerror[i] != E_SUCC) {
                        continue;
                }
          if (((addr >> i) & 1) == TRUE) {
                        /* If DSP bit is on process message protocol    */
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
 
/*******************************************************************
 *       NAME : drvrmsec()
 *DESCRIPTION : 55 millisecond timer routine.
 *      INPUT : None.
 *     OUTPUT : None.
 *    RETURNS : Nothing.
 *   CAUTIONS : If a more apt timer is needed, build on to it but
 *              make sure tmr55 is decremented every 55 msec.
 *******************************************************************/
VOID far drvrmsec(VOID)
{
   if (sys.tmr55 > 0) {
                sys.tmr55--;                     /* update the timer */
        }
}

#endif /* _SCO_ */

/*******************************************************************
 *
 *       			DRIVER FUNCTIONS.          
 *		  area of driver functions + variables.
 *
 *******************************************************************/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_getDSPCount

Description: Fetch the count of DSPs on the specified Antares.

    Returns: Count of DSPs on the platform (2 or 4)

-------------------------------------------------------------------*/
SHORT A4_getDSPCount(WORD port) 
{
   ULONG   cnt;

   A4_readData(port, GIO_DSPCount, 1, &cnt);
   cnt &= 0x7;

   return (SHORT)cnt;
}


/******************************************************************
 *             P_AN_START()	service functions
 ******************************************************************/

/****************************************************************
*        NAME : putrcu
* DESCRIPTION : Locate RCU structure in sorted RCUs array 
*       INPUT : RCU structure.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error
*    CAUTIONS : 
****************************************************************/
SHORT putrcu(RCUSTRUC far *lrcu,BYTE bn)
{
#ifdef LFS
    BYTE former = 0,inc;
#else
    BYTE former,inc;
#endif
    UINT32 code;

   /* Translation of RCU into ULONG, it must be done in this
      way in case order of fields in RCU structure is different */
    code = ((((((UINT32)lrcu->grcu.rclass << 8) + 
                (UINT32)lrcu->grcu.board) << 8) + 
		(UINT32)lrcu->grcu.dsp) << 8) + 
                (UINT32)lrcu->grcu.unit;
#ifdef RTDL_ANT /* initiate RCU params for hot downloaded 1 board only or all */
    dbgCode = code;
    if (oneBoardID == (SHORT) -1) {
	/* 'old-fashion' donload for ALL boards */
	/* Initiates RCU parameters */
	 p_rcu[sys.availrcu].l_rcu = *lrcu;
	 p_rcu[sys.availrcu].code = code;
	 p_rcu[sys.availrcu].opened = 0;
	 p_rcu[sys.availrcu].board = bn;

	/* First RCU to locate in list */
	if (sys.availrcu == 0) {
	    p_rcu[sys.availrcu].next = -1;
	    return(E_SUCC);
        }
    }
    else {
       /* hot-donload for 1 board only*/
       /* Initiates RCU parameters */
	p_rcu[free_rcu].l_rcu = *lrcu;
	p_rcu[free_rcu].code = code;
	p_rcu[free_rcu].opened = 0;
	p_rcu[free_rcu].board = bn;

       /* First RCU to locate in list */
       /* actually, it can't happen in hot-download
          (other rcus already exist) ! */
	if (sys.availrcu == 0) {
	    p_rcu[free_rcu].next = -1;
	    return(E_SUCC);
        }
    }
#else
   /* Initiates RCU parameters */
    p_rcu[sys.availrcu].l_rcu = *lrcu;
    p_rcu[sys.availrcu].code = code;
    p_rcu[sys.availrcu].opened = 0;
    p_rcu[sys.availrcu].board = bn;

   /* First RCU to locate in list */
    if (sys.availrcu == 0) {
	p_rcu[sys.availrcu].next = -1;
	return(E_SUCC);
    }
#endif
    inc = startrcu;
   /* Looking for place in sorted array to locate new RCU */
    while (p_rcu[inc].code < code) {
        if (p_rcu[inc].next == -1) {  /* end of list */
#ifdef RTDL_ANT /* Looking for place in sorted array to locate new RCU */
	    if (oneBoardID == (SHORT) -1) {
	       /* 'old-fashion' donload for ALL boards */
		p_rcu[inc].next = sys.availrcu;
		p_rcu[sys.availrcu].next = -1;
	    }
            else {
	       /* hot-donload for 1 board only*/
		p_rcu[inc].next = free_rcu;
		p_rcu[free_rcu].next = -1;
	    }
#else
            p_rcu[inc].next = sys.availrcu;
	    p_rcu[sys.availrcu].next = -1;
#endif
	    return(E_SUCC);
	}
	former = inc;
	inc = (BYTE)p_rcu[inc].next;
    }

    if (p_rcu[inc].code == code) {
       /* Two identical RCUs found */
	return(E_DOUBLEDRCU);
    }
#ifdef RTDL_ANT   /* first place in list or add element to list - 1 or all */
    if (oneBoardID == (SHORT) -1) {
       /* 'old-fashion' donload for ALL boards */
	if (inc == startrcu) {  /* first place in list */
	    p_rcu[sys.availrcu].next = startrcu;
	    startrcu = sys.availrcu;
	}
	else {		 /* add element to list */
	    p_rcu[sys.availrcu].next = inc;
	    p_rcu[former].next = sys.availrcu;
	}
    } 
    else {
       /* hot-donload for 1 board only*/
        if (inc == startrcu) {  /* first place in list */
            p_rcu[free_rcu].next = startrcu;
	    startrcu = free_rcu;
	}
	else {	 /* add element to list */
	    p_rcu[free_rcu].next = inc;
	    p_rcu[former].next = free_rcu;
        }
    }
#else
    if (inc == startrcu) {  /* first place in list */
	p_rcu[sys.availrcu].next = startrcu;
	startrcu = sys.availrcu;
    }
    else {    /* add element to list */
	p_rcu[sys.availrcu].next = inc;
	p_rcu[former].next = sys.availrcu;
    }
#endif
    return(E_SUCC);
}

/****************************************************************
*        NAME : sortrcu
* DESCRIPTION : Sort RCU structure array according to UNIT parameter.
*       INPUT : VOID
*      OUTPUT : 
*     RETURNS : E_SUCC / Error
*    CAUTIONS : 
****************************************************************/
VOID sortrcu(RCUSTRUC far *lrcu)
{
#ifdef LFS
    BYTE former = 0,inc;
#else
    BYTE former,inc;
#endif
    UINT32 ucode;

   /* Translation of RCU into ULONG, it must be done in this
      way in case order of fields in RCU structure is different */
ucode = ((((((UINT32)lrcu->grcu.rclass << 8) + (UINT32)lrcu->grcu.unit) << 8) + 
             (UINT32)lrcu->grcu.board) << 8) + (UINT32)lrcu->grcu.dsp;
#ifdef RTDL_ANT /* Translation of RCU into ULONG - 1 or all */
	if (oneBoardID == (SHORT)-1) {
		/* 'old-fashion' download for ALL boards */
		p_rcu[sys.availrcu].ucode = ucode;

	   /* First RCU to locate in list */
		if (sys.availrcu == 0) {
			p_rcu[sys.availrcu].unext = -1;
			return;
		}
	} else {
		/* hot download for 1 board only */
		p_rcu[free_rcu].ucode = ucode;

		/* First RCU to locate in list */
		if (sys.availrcu == 0) {
			p_rcu[free_rcu].unext = -1;
			return;
		}
	}
#else
    p_rcu[sys.availrcu].ucode = ucode;

   /* First RCU to locate in list */
    if (sys.availrcu == 0) {
	p_rcu[sys.availrcu].unext = -1;
	return;
    }
#endif
    inc = ustartrcu;
   /* Looking for place in sorted array to locate new RCU */
    while (p_rcu[inc].ucode < ucode) {
        if (p_rcu[inc].unext == -1) {  /* end of list */
#ifdef RTDL_ANT /* Looking for place in sorted array to locate new RCU */
		if (oneBoardID == (SHORT)-1) {
			/* 'old-fashion' download for ALL boards */
			p_rcu[inc].unext = sys.availrcu;
			p_rcu[sys.availrcu].unext = -1;
		} else {
			/* hot download for 1 board only */
			p_rcu[inc].unext = free_rcu;
			p_rcu[free_rcu].unext = -1;
		}
#else
  	    p_rcu[inc].unext = sys.availrcu;
	    p_rcu[sys.availrcu].unext = -1;
#endif
	    return;
	}
	former = inc;
	inc = (BYTE)p_rcu[inc].unext;
    }
#ifdef RTDL_ANT  /* Looking for place in sorted array to locate new RCU */
	if (oneBoardID == (SHORT)-1) {
		/* 'old-fashion' download for ALL boards */
		if (inc == ustartrcu) {  /* first place in list */
			p_rcu[sys.availrcu].unext = ustartrcu;
			ustartrcu = sys.availrcu;
		}
		else { /* add element to list */
			p_rcu[sys.availrcu].unext = inc;
			p_rcu[former].unext = sys.availrcu;
		}
	} else {
		/* hot download for 1 board only */
		if (inc == ustartrcu) {  /* first place in list */
			p_rcu[free_rcu].unext = ustartrcu;
			ustartrcu = free_rcu;
		}
		else { /* add element to list */
			p_rcu[free_rcu].unext = inc;
			p_rcu[former].unext = free_rcu;
		}
	}
#else
    if (inc == ustartrcu) {  /* first place in list */
	p_rcu[sys.availrcu].unext = ustartrcu;
	ustartrcu = sys.availrcu;
    }
    else { /* add element to list */
	p_rcu[sys.availrcu].unext = inc;
	p_rcu[former].unext = sys.availrcu;
    }
#endif
    return;
}

/****************************************************************
*        NAME : init_board
* DESCRIPTION : Checking board_existance + id.
*       INPUT : board number. 
*      OUTPUT : 
*     RETURNS : 0 if ok,Otherwise error.
*    CAUTIONS :
****************************************************************/
SHORT init_board(BYTE bn)
{
#ifdef VME_ANT
   /* checks legality of board address */
    if (!A4_present(p_board[bn].slot_id)) {
       cmn_err(CE_WARN,"Board in slot %d is not Antares\n",p_board[bn].slot_id);
       return(E_NOBOARD);
    }

    if (!A4_get_memsize(bn)) { 
	return(E_GETMEMSIZE);
    }

    if (!A4_set_A32mode(p_board[bn].slot_id)) {
	return(E_SETA32);
    }

    p_board[bn].kva = (caddr_t) A4_get_kva(p_board[bn].slot_id);
    if (p_board[bn].kva == (caddr_t) NULL) {
        return(E_GETKVA);
    }
#else
    if (!A4_present(p_board[bn].port)) {  /* checks legality of board address */
	return(E_NOBOARD);
    }
#endif
    return(E_SUCC);
}

/****************************************************************
*        NAME : init_address
* DESCRIPTION : 1) Checking system state (and Error Mask if needed).
*					 2) Gets number of DSP.
*					 3) Gets addresses of HDCB.DHCB, + BDCB arrays to boards structures.
*       INPUT : board number. 
*      OUTPUT : 
*     RETURNS : 0 if ok,Otherwise error.
*    CAUTIONS : 
****************************************************************/
SHORT init_address(BYTE bn)
{
    UINT32   lword;

   /* gets number of boards DSPs and checks legality */
    p_board[bn].ndsp = (BYTE)A4_getDSPCount(p_board[bn].port);
   
    if ((p_board[bn].ndsp > A4_DSP_MAX) || (p_board[bn].ndsp < A4_DSP_MIN)) {
	return(E_BADDSPNUM);
    }

#if 0
   /* Zeroing error code */
    for (i = 0; i<p_board[bn].ndsp; i++) {
        p_board[bn].DSPerror[i] = E_SUCC;
    }
#endif

   /* reads board start_state position, should be AN_START */
    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(STARTSTATE)));

    if (lword != AN_START) {
	/*
	cmn_err(CE_CONT, "startstate = %x != %x, returning error\n",
		lword, AN_START);
	*/
	return(E_NOTSTART);
    }

   /* Reads error mask - if there is error in one of dsp */
    A4_readData(p_board[bn].port,
                A4_CFGParm(ERRORCODE),(WORD)4,
                (UINT32 far *)(&(p_board[bn].DSPerror[0])));

    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(ERRORMASK)));

    if (lword != 0) {
       /* All DSPs not active */
	if ((lword & 0x000F) == 0xF) {
	    return(E_ALLDSPS);	/* Error DSP 0 */
	}
    }

   /* reads number of configured bulk data streams */ 
    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(BDSTREAM)));
    p_board[bn].nbdstream= (BYTE)lword;			 

   /* Reads address of BDCB, HDCB & DHCB structures address in board memory */ 
    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(BDCBADDR)));

    p_board[bn].pbdcb = lword;			 
    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(HDCBADDR)));

    p_board[bn].phdcb = lword;
    lword = A4_read1Word(p_board[bn].port, (A4_CFGParm(DHCBADDR)));

    p_board[bn].pdhcb = lword;

    return(E_SUCC);
}

#ifdef RTDL_ANT  /* delete_rcus */
/****************************************************************
*        NAME : delete_rcus
* DESCRIPTION : delete rcu entries from p_rcu array of rcus on re-loaded board
*       INPUT : board number. 
*      OUTPUT : 
*     RETURNS : 0 if ok,Otherwise error.
****************************************************************/
SHORT delete_rcus(BYTE bn) 
{
	SHORT rcu_loc, rcu_prev,		/* rcu entry (C:B:D:U sort) */
		  u_rcu_loc, u_rcu_prev;	/* rcu entry (C:U:B:D sort) */
	SHORT i, j;
	SHORT rc;

#ifdef ANHD_DEBUG
	/* debug: record busy array */
	for(i=0; i<cfg.maxrcu; i++) {
		dbg_busy[i] = busy_rcu[i];
	}
#endif
        if (sys.availrcu > 0) {
	/* search for opened rcus in board bn. close all opened */
	for (i=startrcu; p_rcu[i].next != -1; i = p_rcu[i].next) {
		if ((p_rcu[i].board == bn) 		/* its our board! */
			&& (p_rcu[i].opened > 0)) {	/* physical RCU is opened */
			/* downloading to a board with opened rcus is illeagal */
			/* close all opened RCU handles pointing to this physical RCU */
			for (j=0; j<cfg.maxopenedrcu; j++) {
				if (p_orcu[j].index == i) {
					/* found a handle pointing to our physical RCU */
					/* close it */
					rc = p_an_close(j);
					if (rc != E_SUCC) {
						return E_HDCLOSERCU;
					}
				}
			}
		}
	}
	/* search for opened BD streams in board bn. close all opened */
	for (i=0; i<cfg.maxbd; i++) {
		/* if there is an open stream on our board - close it */
		if ((p_bd[i].bnumber == bn) && (p_bd[i].direction != BDCLOSE)) {
			flgBoardInit = FALSE;
			rc = p_bd_close(i);
			flgBoardInit = TRUE;
			if (rc != E_SUCC) {
			/* return E_HDCLOSEBD; */
				return rc;
			}
		}
	}

	do {
		/* 'search & destroy' rcus from board bn */

		/* do we have what to delete ? */
		if (sys.availrcu == 0) {
			/* Bug fix: when hot-downloading with only one board we leave startrcu==-1 */
			/* after deleting the last rcu. this cause a crash at the begining of next loop */
			startrcu = 0;
			ustartrcu = 0;
			break;
		}

		/* point to list start */
		rcu_loc = startrcu;
		rcu_prev = (SHORT)-1;	/* not defined at the begining */
		while (p_rcu[rcu_loc].board != bn) {
			/* this rcu is not on our board */
			if (p_rcu[rcu_loc].next == (SHORT)-1) {
				/* end of rcu list */
				return E_SUCC;
			}
			/* advance to next rcu */
			rcu_prev = rcu_loc;
			rcu_loc = p_rcu[rcu_loc].next;
		}
		/* found rcu on board bn (C:B:D:U sort) */
		/* now find the same entry in the C:U:B:D sort */
		u_rcu_loc = ustartrcu;
		u_rcu_prev = (SHORT)-1; /* not defined at the begining */
		while (u_rcu_loc != rcu_loc) { /* compare entry number */
			if (p_rcu[u_rcu_loc].unext == (SHORT)-1) {
				/* MUST find rcu. database not valid ! */
				return E_RCUDATABASE;
			}
			/* advance to next urcu */
			u_rcu_prev = u_rcu_loc;
			u_rcu_loc = p_rcu[u_rcu_loc].unext;
		}

		/* found both rcu and urcu, now delete */
		if (rcu_prev == (SHORT)-1) {
			/* delete 1st rcu */
			startrcu = p_rcu[rcu_loc].next;
		} else {
			p_rcu[rcu_prev].next = p_rcu[rcu_loc].next;
		}
		if (u_rcu_prev == (SHORT)-1) {
			/* delete 1st urcu */
			ustartrcu = p_rcu[u_rcu_loc].unext;
		} else {
			p_rcu[u_rcu_prev].unext = p_rcu[u_rcu_loc].unext;
		}
		busy_rcu[rcu_loc] = FALSE; /* mark entry as not busy */
		sys.availrcu--; /* decrease available rcus count */
	} while (TRUE);
        }

	return E_SUCC; /* never reached */
}
#endif
/****************************************************************
*        NAME : init_rcus
* DESCRIPTION : For every dsp: gets CLASS + Rcu list.Organize it in array
*       INPUT : board number. 
*      OUTPUT : 
*     RETURNS : 0 if ok,Otherwise error.
*    CAUTIONS : Checks wheter number of RCU'S not more than aloud in CFG file
****************************************************************/
SHORT init_rcus(BYTE bn)
{
    RCUSTRUC srcu;		 /* RCU single information structure */
    RCInfo   rcud;
    SHORT    rc,total;
    BYTE     i,j;
#ifdef RTDL_ANT /* init_rcus - 1 then delete_rcus */
	if (oneBoardID != (SHORT)-1) {
		/* hot donload for one board only. */
		/* remove all board's rcus from database */
		if ((rc = delete_rcus(bn)) != E_SUCC) {
			return rc;
		}
		/* locate 1st available rcu entry */
		for (free_rcu=0; free_rcu<cfg.maxrcu; free_rcu++) {
			if (busy_rcu[free_rcu] == FALSE) {
				/* found a not-busy entry for a new rcu */
				break;
			}
		}
		if (free_rcu >= cfg.maxrcu) {
			/* didn't find free entry */
			return E_RCUSNUMBER;
		} 
	} 
#endif
    total = sys.availrcu;

   /* For each DSP avaiable in board */
    for (i = 0; i < p_board[bn].ndsp; i++) {
       /* If error in DSP RCU's not available */
	if (p_board[bn].DSPerror[i] != E_SUCC) {
            continue;
	}

       /* Reading RCUs list structure */
	A4_readData(p_board[bn].port, (A4_CFGParm(RCUADDR)+ i*2), 2,
		    (UINT32 far *)(&(rcud)));

	total += rcud.RCUCount;

       /* Checking total amount of RCUs in system */
	if (total > cfg.maxrcu) {
            return(E_RCUSNUMBER);
	}

       /* Reading each RCU and locate its details in global array */
	for (j = 0; j< (BYTE)rcud.RCUCount;j++) {
#ifdef RTDL_ANT /* mark entry as busy */
		if (oneBoardID != (SHORT)-1) {
			busy_rcu[free_rcu] = TRUE; 
		} else {
			busy_rcu[sys.availrcu] = TRUE; 
		}
#endif
           /* Read RCU details */
#ifdef _SCO_
	    A4_readData(p_board[bn].port, (rcud.pRCU+(j*RCUsize)), RCUsize,
			    (UINT32 far *)(&(srcu)));
#else
	    A4_readData(p_board[bn].port, (rcud.pRCU+(j*RCUsize)), RCUsize,
			    (UINT32 *)(&(srcu)));
#endif /* _SCO_ */
#ifdef BIG_ANT
            p_an_byteswap((char *) &srcu, 1, 4);
#endif
            if (p_board[bn].board_id != srcu.grcu.board) {
               /* RCU's board must be identical to board_id */
		return(E_BOARDNUMBER);
            }

           /* Locate RCU in global list */
            rc = putrcu((RCUSTRUC far *)(&(srcu)),bn);
            if (rc != E_SUCC) {
		return(rc);
            }

           /* Sort RCU's according to unit order */
            sortrcu((RCUSTRUC far *)(&(srcu)));
#ifdef RTDL_ANT /* advance free_rcu to the next not-busy entry */
			if (oneBoardID != (SHORT)-1) {
				/* hot donload for one board only. */
				/* advance free_rcu to the next not-busy entry */
				for(; free_rcu<cfg.maxrcu; free_rcu++) {
					if (busy_rcu[free_rcu] == FALSE) {
						/* found a not-busy entry for a new rcu */
						break;
					}
				}
				if (free_rcu >= cfg.maxrcu) {
					/* didn't find free entry */
					return E_RCUSNUMBER;
				} 
			}
#endif
           /* Increament number of system available RCUs */
            sys.availrcu++;
	}
    }
    return(E_SUCC);
}

/******************************************************************
 *                   ANOPEN 	option functions
 ******************************************************************/

/****************************************************************
*        NAME : searchprcu
* DESCRIPTION : searches for available pseudo rcu
*       INPUT : void
*      OUTPUT : 
*     RETURNS : place found in array or -1
*    CAUTIONS : 
****************************************************************/
SHORT searchprcu(VOID)
{
    SHORT i;

    for (i = 0; i<cfg.maxopenedrcu; i++) {
	/* Loop of all opened RCUs */
	if (p_orcu[i].index == -1) {
	    return(i);
        }
    }
    return(-1);		 /* no available pseudo rcu */
}

#define RCUFIELD(f1,f2)  ((f1 == AN_ANY) || (f1 == f2))

/****************************************************************
*        NAME : searchrcu
* DESCRIPTION : searches for matching between user request rcu
*		and rcu's available array.
*       INPUT : User rcu request
*      OUTPUT : 
*     RETURNS : location if found / otherwise -1
*    CAUTIONS : 
****************************************************************/
SHORT searchrcu(RCU far *p1,LONG  far *Attarray, SHORT RCUflags)
{
    SHORT inc,next;
    BYTE equal,attr,an_any;
    BYTE nmode;
    BYTE attrflag = FALSE;

    if ((sys.availrcu == 0) || (p1->rclass == AN_ANY)) {
       /* No RCUs at least */
 	return(E_RCUNOTAVAIL);
    }

    nmode = (BYTE)(RCUflags & UNITSORT);

   /* finds shift size according to an_any location */
    if ((p1->board == AN_ANY) || (p1->dsp == AN_ANY) || (p1->unit == AN_ANY)) {
        an_any = TRUE;
    }
    else {
        an_any = FALSE;
    }

    if (nmode) {
        inc = ustartrcu; /* pointer to first cell */
    }
    else {
        inc = startrcu; /* pointer to first cell */
    }

    while (TRUE) {
        equal = (BYTE)(RCUFIELD(p1->rclass,p_rcu[inc].l_rcu.grcu.rclass) &&
                       RCUFIELD(p1->board,p_rcu[inc].l_rcu.grcu.board) &&
                       RCUFIELD(p1->dsp,p_rcu[inc].l_rcu.grcu.dsp)     &&
                       RCUFIELD(p1->unit,p_rcu[inc].l_rcu.grcu.unit)); 

        if (equal) {
            if (Attarray != NULL) { 
                attr = (BYTE)(((p_rcu[inc].l_rcu.AttrA & (ULONG)Attarray[0]) == (ULONG)Attarray[0]) &&
			      ((p_rcu[inc].l_rcu.AttrB & (ULONG)Attarray[1]) == (ULONG)Attarray[1])); 
                if (attr == FALSE) {
                    attrflag = TRUE;
                }
            }
            else {
                attr = TRUE;
            }

           /* RCU request in detail */
            if (!an_any) {
                if (attr) {
	            *p1 = p_rcu[inc].l_rcu.grcu;
		    return(inc);
                }
                else {
                    if (attrflag) {
                        return(E_ATTRFLAG);
                    }
                    return(E_RCUNOTFOUND);
                }
            }

           /* RCU - AN_ANY request */
	    if ((p_rcu[inc].opened == 0) && (attr)) {
		*p1 = p_rcu[inc].l_rcu.grcu;
		return(inc);
            }
        }

        if (nmode) {
            next = p_rcu[inc].unext;
        }
        else {
            next = p_rcu[inc].next;
        }

	if (next == -1) {
            if (attrflag) {
                return(E_RCUFLAGS);
            }
	   /* Nothing found, end of list */
	    return(E_RCUFIELDS);
	}
        inc = next;
    }
    return(-1); /* Never reached */
}

/****************************************************************
*        NAME : p_an_open
* DESCRIPTION : Open an antares RCU.
*       INPUT : rcu: 	 far pointer to an RCU structure;
*		flags: 	 Set to ANT_UNS if this device is to recieve
*		  	 unsolicited event messages identifiers;
*		 proc_id: far pointer to a process identifier structure.	
*     RETURNS : >= 0 rcu_dev.
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_an_open(RCU far *rcuid,SHORT oflags,
	             PRC far *process,LONG  far *Attarray)
{

    SHORT loc_rcu,
          loc_prcu;

    if (sys.crcu >= (BYTE)cfg.maxopenedrcu) {  /* maximum rcu's are opened */
	return(E_RCUNOTAVAIL);
    }

   /* finds available place in pseudo RCU array */
    loc_prcu = searchprcu();
    if (loc_prcu == -1) {  /* avaiable place not found */
	return(E_RCUNOTAVAIL);
    }

   /* finds matching place in RCUs avaiable array */
    loc_rcu = searchrcu(rcuid,Attarray,oflags);
    if (loc_rcu < 0) {		 /* not found */
 	return(loc_rcu);
    }

    if (Attarray != NULL) {
       /* Attributes are requested */
	Attarray[0] = p_rcu[loc_rcu].l_rcu.AttrA;
	Attarray[1] = p_rcu[loc_rcu].l_rcu.AttrB;
    }

   /* initiates pseudo RCU array + increment rcu open variable */
    p_rcu[loc_rcu].opened++;

    p_orcu[loc_prcu].solo = (BYTE) oflags;

   /* only at end, now channel is ready for get message */
    p_orcu[loc_prcu].index = loc_rcu;

    p_orcu[loc_prcu].Class = PSEUDOVALUE;
    p_orcu[loc_prcu].Board = PSEUDOVALUE;
    p_orcu[loc_prcu].Dsp = incindex;
    if (++incindex == 0xFF) {
	incindex = 0;
    }

    p_orcu[loc_prcu].Unit = (BYTE)loc_prcu;

   /* Call to x_an_open brings back pointer to more data (used for translation
      to other operating systems
    */
    p_orcu[loc_prcu].RTransAddr = xx_an_open(loc_prcu,rcuid,oflags,process,Attarray);

    sys.crcu++;
    return(loc_prcu);
}

 /******************************************************************
  *                   ANCLOSE	option functions                   */
/****************************************************************
*        NAME : p_an_close
* DESCRIPTION : Closes an open  resource rclass unit.
*       INPUT : rcu_dev: Device handle of an open Antares RCU 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_an_close(SHORT dev)
{
	if ((dev < 0) || (dev >= cfg.maxopenedrcu)) {	/* dev not found */
		return(E_ILLEGALRCU);
	}

	if (p_orcu[dev].index == -1) { /* dev not found */
		return(E_CLOSESYSERROR);
	}

	if (p_rcu[p_orcu[dev].index].opened == 0) { /* dev is closed */
		return(E_CLOSESYSERROR);
	}
	
	/* close operation on RCUs database */
	p_rcu[p_orcu[dev].index].opened -= 1;
	p_orcu[dev].index = -1;

	sys.crcu -= 1;
	return(xx_an_close(dev));

}


/****************************************************************
*        NAME : p_send_msg
* DESCRIPTION : Send a message to an RCU.
*       INPUT : prcu:    RCU device handle;
*		          msg:     Far pointer to message structure.
*		          reply:   Far pointer to DSP reply structure.
*					 msgtype: Message type.
*
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_send_msg(SHORT prcu,SEND_MSG far *msg,SYNC_REPLY far *reply,
                                     ULONG msgtype,SHORT reply_method)
{	
	SHORT frcu,board;
	RCU destrcu;

	/* Finds location in RCUs array */
	frcu = p_orcu[prcu].index;

	if ((frcu == -1) || (p_rcu[frcu].opened == 0)) {
  		return(E_ILLEGALRCU);
  	}

	destrcu = p_rcu[frcu].l_rcu.grcu;
	board = p_rcu[frcu].board;

	return(send_message(prcu,board,destrcu,msg,reply,msgtype,(BYTE)reply_method));
}



/****************************************************************
*        NAME : p_bd_open
* DESCRIPTION : Opens an antares bulk data transfer stream.
*       INPUT : id: Board_id or an RCU handler.
		    determined according to 5th bit of mode parameter.
*		mode:  Mode of operation (bit mask).
*		streamid: Far pointer to stream id of assigned stream 
*		proc_id : far pointer to a process identifier.
*      OUTPUT : 
*     RETURNS : >= 0 Bulk data handler if successful.
*		< 0 Error number if failure.
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT p_bd_open(SHORT bd_id, SHORT bdmode, SHORT far *bdstream, PRC far *bdproc)
#else
SHORT p_bd_open(SHORT bd_id, SHORT bdmode, SHORT *bdstream, PRC *bdproc)
#endif /* _SCO_ */
{
    BYTE bn;
    SHORT res;
    SHORT i,loc;
    BYTE *BDAddress;

    if (sys.cbdata >= (BYTE)cfg.maxbd) {    /* bulk data array is full */
        return(E_FULLBD);
    }

    if ((*bdstream <= 0) || 
        ((*bdstream > MAXBDPERBOARD) && (*bdstream != AN_ANY))) { 
        return(E_BDILLEGAL);
    }

   /* This is needed because streams are checked between 0 to 63 
      but for user are between 1 to 64 */
    if (*bdstream != AN_ANY) {
	*bdstream -= 1;	
    }

    for (i = 0,loc = -1; i < cfg.maxbd; i++) {
       if (p_bd[i].direction == BDCLOSE) {
           loc = i;
	   break;
       }
    }							  
    if (loc == -1) {
	return(E_FULLBD);     /* no available bulk data channel  */
    }

   /* Is ANT_BD_ID option is active */
    res = (((bdmode) & ((SHORT)ANT_BD_ID)) ? (SHORT)TRUE : (SHORT)FALSE);
    if (res) {	/* bd_id is equal to rcu handler got from an_open */
	if ((bd_id < 0) || (bd_id >= cfg.maxopenedrcu)) {   /* dev not found */
	    return(E_ILLEGALRCU);
	}
	if ((p_orcu[bd_id].index == -1) ||
	    (p_rcu[p_orcu[bd_id].index].opened == 0)) {
  		return(E_ILLEGALRCU);
  	}
	bn = (BYTE)p_rcu[p_orcu[bd_id].index].board;
    }
    else {	 /* bd_id is board_id */
	for (i = 0; i<cfg.nboards; i++) {
	    if (p_board[i].board_id == (BYTE)bd_id) {
		break;
	    }
	}
       /* board not found */
	if (i >= cfg.nboards) {
	    return(E_BOARDNOTEXIST);
	}
	bn = (BYTE)i;
    }
  	
    if (p_board[bn].ready < BOARD_LOADED) {
	/* Board is not reay for work */
	return(E_BOARDNOTLOADED);
    }

    res = bd_open(bn,(SHORT)((bdmode) & (WORD)0x0001),bdstream,loc); 
    if (res < (SHORT) 0) {
       /* Bulk data open procedure */
	return(res);
    }
#ifdef AIX_ANT
    p_bd[loc].rcu_buf[0].ump = (mblk_t *) allocb(cfg.bd_single+32,BPRI_HI);
    if (p_bd[loc].rcu_buf[0].ump == (mblk_t *) NULL) {
        cmn_err(CE_WARN,"allocb(0) fail in p_bd_open()");
        return(E_ALLOCMEM);
    } 
    p_bd[loc].rcu_buf[0].ump->b_wptr += cfg.bd_single+32; 
    p_bd[loc].rcu_buf[0].buf = (BYTE *) p_bd[loc].rcu_buf[0].ump->b_rptr+16;   

    p_bd[loc].rcu_buf[1].ump = (mblk_t *) allocb(cfg.bd_single+32,BPRI_HI);
    if (p_bd[loc].rcu_buf[1].ump == (mblk_t *) NULL) {
        cmn_err(CE_WARN,"allocb(1) fail in p_bd_open()");
        freeb(p_bd[loc].rcu_buf[0].ump);
        return(E_ALLOCMEM);
    } 
    p_bd[loc].rcu_buf[1].ump->b_wptr += cfg.bd_single+32; 
    p_bd[loc].rcu_buf[1].buf = (BYTE *) p_bd[loc].rcu_buf[1].ump->b_rptr+16;   
#else
    BDAddress = (BYTE *) NULL;
    BDAddress = (BYTE *) AllocMem(cfg.bd_single+32);
    if (BDAddress == (BYTE *) NULL) {
        return(E_ALLOCMEM);
    }
    p_bd[loc].rcu_buf[0].buf = (BYTE *) (BDAddress+16);   
    BDAddress = (BYTE *) NULL;
    BDAddress = (BYTE *) AllocMem(cfg.bd_single+32);
    if (BDAddress == (BYTE *) NULL) {
        FreeMem((BYTE *) (p_bd[loc].rcu_buf[0].buf-16),cfg.bd_single+32);   
        return(E_ALLOCMEM);
    }
    p_bd[loc].rcu_buf[1].buf = (BYTE *) (BDAddress+16);   
#endif
   /* Initiation of bulk data database */
    p_bd[loc].rcu_readbuf = BD_NOTREADY;
    p_bd[loc].state_memory = FALSE;
    p_bd[loc].onprocess = FALSE;
    p_bd[loc].Completion = 0;
    p_bd[loc].HostStop = 0;
    p_bd[loc].bnumber = bn;
    p_bd[loc].snumber = (BYTE)*bdstream;
   /* This is needed because streams are checked between 0 to 63 
      but for user are between 1 to 64 */
    if (*bdstream != AN_ANY) {
	*bdstream += 1;	
    }
    p_bd[loc].fillbuf = FALSE;
    p_bd[loc].overrun = FALSE;
    p_bd[loc].direction = (BYTE)((bdmode)  & ((SHORT)ANT_BDREAD));
    p_bd[loc].click = (BYTE)(((bdmode) & ((SHORT)ANT_BDCLICK)) ? 1 : 0);
    p_bd[loc].enderror = (BYTE)(((bdmode) & ((SHORT)ANT_BDSTOP))  ? 1 : 0);
    p_bd[loc].async = (BYTE)(((bdmode) & ((SHORT)ANT_BDASYNC)) ? 1 : 0);
    p_bd[loc].bufswap = (BYTE)(((bdmode) & ((SHORT)ANT_BDSWAP)) ? 1 : 0);

    p_bd[loc].bduio.u_write = NULL;
    p_bd[loc].bduio.u_read = NULL;
    p_bd[loc].bduio.u_seek = NULL;

   /* Gets pointer to developer data */
    p_bd[loc].BTransAddr = xx_bd_open(loc,bd_id,bdmode,bdstream,bdproc); 

   /* Increament number of open bulk_data channels in system */
    sys.cbdata++;

    return(loc);
}

/****************************************************************
*        NAME : p_bd_close
* DESCRIPTION : Close an antares bulk data transfer stream.
*       INPUT : bd_dev: Device handle of an open Bulk Data Stream.
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_close(SHORT dev)
{
	SHORT res;

	/* Illegal buk data stream number */
	if ((dev < 0) || (dev >= cfg.maxbd)) {
		return(E_ILLEGALBD);
	}

	/* Bulk data is already closed */
	if (p_bd[dev].direction == BDCLOSE) {
		return(E_CLOSEDBD);
	}					 

	/* Bulk data protocol is in process */
	if (p_bd[dev].onprocess) 
	{
		return(E_BDISACTIVE);
	}
#ifdef RTDL_ANT /* no bd_close in hot download */
	/* if we are in hot-downloading process, we just downloaded board. */
	/* no need to close board's BD streams, but only to clean the */
	/* driver's database. The flag is turned off only in hot download. */
	if (flgBoardInit) {
		/* Bulk data close operation */
		if ((res = bd_close(p_bd[dev].bnumber,p_bd[dev].snumber)) < 0) {
			 return(res);
		}
	}
#else
	/* Bulk data close operation */
	if ((res = bd_close(p_bd[dev].bnumber,p_bd[dev].snumber)) < 0) {
		 return(res);
	}
#endif
#ifdef AIX_ANT
        freeb(p_bd[dev].rcu_buf[0].ump);
        freeb(p_bd[dev].rcu_buf[1].ump);
#else
        FreeMem((BYTE *) (p_bd[dev].rcu_buf[0].buf-16),cfg.bd_single+32);   
        FreeMem((BYTE *) (p_bd[dev].rcu_buf[1].buf-16),cfg.bd_single+32);   
#endif
	p_bd[dev].direction = BDCLOSE;

	/* Decreament number of open bulk_data channels in system */
	sys.cbdata--;
	return(xx_bd_close(dev));
}

/****************************************************************
*        NAME : p_bd_xplayf
* DESCRIPTION : Send multiple files/buffers to bulk data stream.
*       INPUT : bd_dev: Handle of an open Bulk Data Stream.
*               playf:  Far pointer to list of P_FILE structures.            
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_xplayf(SHORT dev,P_FILE *pfile)
{
    SHORT rc;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

    BYTE board;	  /* board number */
    BYTE nbdcb;	 /* number of bulk data control block */
    UINT32 mask;  /* bit mask to clear flag */

   /* Direction is not for play operation */
    if (p_bd[dev].direction != ANT_BDWRITE) {
        return(E_BDDIRECTION);
    }

   /* 
    *  Remember protocol - try to activate process, if DSP side is ready
    *  process will be running otherwise it is remember protocol, means 
    *  waiting for DSP interrupt 
    */
    board = p_bd[dev].bnumber;
    nbdcb = p_bd[dev].snumber;
    if (nbdcb > 31) {
        mask = (1L << ((nbdcb) - 32));
    }
    else {
	mask = (1L << (nbdcb));
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
   /* Aloud interrupt handler to read from I/O buffers */
    p_bd[dev].fillbuf = TRUE;
    p_bd[dev].overrun = FALSE;
    p_bd[dev].rcu_readbuf = 0;

    p_bd[dev].state_memory = TRUE;

    if (p_bd[dev].rcu_buf[0].eob != TRUE) {  /* End Of Block to read */
        rc = send_delay_request(dev,1,1,ANT_BDWRITE,cfg.bd_single,
                                 p_bd[dev].BTransAddr);
        if (rc != E_SUCC) {
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
	    return(E_REQSQUEUE);
        }
    }

    in_inter = TRUE;
    bd_protocol(board,nbdcb,mask);
    in_inter = FALSE;

#ifdef _SCO_
        DLGCSPLX(oldspl);
        DLGCSPLSTR(oldspl);
#endif /* _SCO_ */

    if (in_fatal) {
        in_fatal = FALSE;
#ifdef _SCO_
            drvrintr(cmsghdlr);    /* Address of intrpt msg hndlr */
#else
#ifdef VME_ANT
        anisr(p_board[board].slot_id);
#else
        anisr(cfg.hwintr);    /* Address of intrpt msg hndlr */
#endif
#endif /* _SCO_ */
    }
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
   /* check here global error variables */
    return(E_SUCC);
}

/****************************************************************
*        NAME : p_bd_setuio
* DESCRIPTION : Set user I/O function for open bulk data stream.
*       INPUT : bd_dev:  Handle of an open Bulk Data Stream.
*					 handler: Input file handler.	
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_setuio(SHORT dev, AN_UIO far *bduio)
{
	/* Illegal bulk data handler */
	if ((dev < 0) || (dev >= cfg.maxbd)) {
		return(E_ILLEGALBD);
	}

	/* Bulk data stream is closed */
	if (p_bd[dev].direction == BDCLOSE) {
		return(E_CLOSEDBD);
	}

	p_bd[dev].bduio.u_write = bduio->u_write;
	p_bd[dev].bduio.u_read = bduio->u_read;
	p_bd[dev].bduio.u_seek = bduio->u_seek;

	return(E_SUCC);
}

/****************************************************************
*        NAME : p_bd_getchan
* DESCRIPTION : Set user I/O function for open bulk data stream.
*       INPUT : bd_dev:  Handle of an open Bulk Data Stream.
*					 handler: Input file handler.	
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_getchan(SHORT dev, P_FILE *pfile)
{
   /* Illegal bulk data handler */
    if ((dev < 0) || (dev >= cfg.maxbd)) {
        return(E_ILLEGALBD);
    }

   /* Bulk data stream is closed */
    if (p_bd[dev].direction == BDCLOSE) {
        return(E_CLOSEDBD);
    }

#ifdef _SCO_
   /* Direction is not for play operation */
    if (p_bd[dev].direction != ANT_BDWRITE) {
        return(E_BDDIRECTION);
    }
#endif /* _SCO_ */
   /* Checking if xplayf list is not empty */
    if (pfile->option & XPLAY_END) { 
        return(E_EMPTYLIST);
    }

    p_bd[dev].bdstop  = FALSE;
    p_bd[dev].pfstart = pfile->fstart;
    p_bd[dev].poffset = pfile->offset;

    return(E_SUCC);
}

/****************************************************************
*        NAME : p_bd_xrecf
* DESCRIPTION : Gets multiple files/buffers frpm bulk data stream.
*       INPUT : bd_dev: Handle of an open Bulk Data Stream.
*               playf:  Far pointer to list of P_FILE structures.            
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_xrecf(SHORT dev,P_FILE far *pfile)
{
    SHORT rc;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

    BYTE board;	  /* board number */
    BYTE nbdcb;	 /* number of bulk data control block */
    UINT32 mask;  /* bit mask to clear flag */

#ifdef _SCO_
        /* Illegal bulk data handler */
        if ((dev < 0) || (dev >= cfg.maxbd)) {
            return(E_ILLEGALBD);
        }
 
        /* Bulk data stream is closed */
        if (p_bd[dev].direction == BDCLOSE) {
                return(E_CLOSEDBD);
        }
#endif /* _SCO_ */

   /* Direction is not for record operation */
    if (p_bd[dev].direction != ANT_BDREAD) {
        return(E_BDDIRECTION);
    }

#ifdef _SCO_
        /* Checking if xrecf list is not empty */
        if (pfile->option & XPLAY_END) {
            return(E_EMPTYLIST);
        }
 
        p_bd[dev].pfstart = pfile->fstart;
        p_bd[dev].poffset  = pfile->offset;
#endif /* _SCO_ */

   /* Initiation of xrecf process parameters */
    p_bd[dev].datalen = 0;
    p_bd[dev].bdstop  = FALSE;
    p_bd[dev].Completion = 0;
    p_bd[dev].HostStop = 0;

#ifdef _SCO_
        /* Checking legality of xrecf structure */
        rc = check_xrec(dev,pfile);
        if (rc < 0) {
            return(rc);
        }
#endif /* _SCO_ */

   /* initiates data variables */
    p_bd[dev].offset = 0;

    p_bd[dev].rcu_buf[0].size = 0;
    p_bd[dev].rcu_buf[0].eob = FALSE;
    p_bd[dev].rcu_buf[1].size = 0;
    p_bd[dev].rcu_buf[1].eob = FALSE;
    p_bd[dev].fillbuf = FALSE;
    p_bd[dev].overrun = FALSE;

   /* Alowes interrupt handler to read from I/O buffers */
    p_bd[dev].state_memory = TRUE;
    p_bd[dev].rcu_readbuf = 0;

   /* 
    *  Remember protocol - try to activate process, if DSP side is ready
    *  process will be running otherwise it is remember protocol, means 
    *  waiting for DSP interrupt
    */
    board = p_bd[dev].bnumber;
    nbdcb = p_bd[dev].snumber;
    if (nbdcb > 31) {
	mask = (1L << ((nbdcb) - 32));
    }
    else {
	mask = (1L << (nbdcb));
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
    in_inter = TRUE;
    bd_protocol(board,nbdcb,mask);
    in_inter = FALSE;

#ifdef _SCO_
        DLGCSPLX(oldspl);
        DLGCSPLSTR(oldspl);
#endif /* _SCO_ */

    if (in_fatal) {
	in_fatal = FALSE;
#ifdef _SCO_
    drvrintr(cmsghdlr);    /* Address of intrpt msg hndlr */
#else
#ifdef VME_ANT
        anisr(p_board[board].slot_id);
#else
        anisr(cfg.hwintr);    /* Address of intrpt msg hndlr */
#endif
#endif /* _SCO_ */
    }
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
   /* check here global error variables */
    return(E_SUCC);
}

/****************************************************************
*        NAME : p_bd_stop
* DESCRIPTION : Stop bulk data protocol.
*       INPUT : bd_dev: Handle of an open Bulk Data Stream.
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_bd_stop(SHORT dev)
{

	/* Illegal bulk data handler */
	if ((dev < 0) || (dev >= cfg.maxbd)) {
		return(E_ILLEGALBD);
	}

	/* Bulk data stream is closed */
	if (p_bd[dev].direction == BDCLOSE) {
		return(E_CLOSEDBD);
	}					 

	/* Bulk data protocol already stopped */
	if (p_bd[dev].bdstop == TRUE) {
	 	return(E_EOD);
	}


	p_bd[dev].bdstop = TRUE;

	return(E_SUCC);
}

/****************************************************************
*        NAME : p_put_queue
* DESCRIPTION : Simulates get message from kernel by putting given
*               message into system queue through protocol like get_message
*       INPUT : Board,RCU handler,msgtype,msgid,message size,message buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code;
*    CAUTIONS : 
****************************************************************/
SHORT p_put_queue(WORD porcu,WORD pmsgtype,WORD pmsgid,WORD pblksz,ADDR *pmess)
{
    /* Call antlow.c function put_queue */
     return(put_queue(porcu,pmsgtype,pmsgid,pblksz,pmess));
}

/****************************************************************
*        NAME : p_raw_open
* DESCRIPTION : Open an antares board device handle.
*       INPUT : board_dev: board identification code.
*      OUTPUT : 
*     RETURNS : >=0 board device handle. 
*             	  <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_raw_open(SHORT bd_id)
{
    SHORT i,j;

   /* Number opened board raws handlers reached maximum alowed */
    if (sys.raw_ind >= MAXOPENRAW) {
	return(E_MAXRAW);
    }

    for (i = 0; i<cfg.nboards; i++) {   /* Searching for board_id */
        if (p_board[i].board_id == (WORD)(bd_id)) {
           /* Board is not ready for work */
	    if (p_board[i].ready < BOARD_EXIST) {
		return(E_BOARDNOTEXIST);
	    }

	    for (j = 0; j < MAXOPENRAW; j++) {
	       /* Looking for available handler */
	        if (raw_open[j] == -1) {
#ifdef _SCO_
	            raw_open[j] = (BYTE)i;
#else
	            raw_open[j] = i;
#endif /* _SCO_ */
		    sys.raw_ind++;
		    return(j);
	        }
	    }
        }
    }

   /* Board not found */
    return(E_BOARDNOTEXIST);
}

/****************************************************************
*        NAME : p_raw_close
* DESCRIPTION : Close an open antares board device handle
*       INPUT : board_dev: Device handle of an open board device
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_raw_close(SHORT dev)
{
   /* Illegal device handler */
    if ((dev < 0) || (dev >= MAXOPENRAW)) {
	return(E_ILLEGALRAW);
    }

   /* Device handler is already closed */
    if (raw_open[dev] == -1) {
        return(E_CLOSEDRAW);
    }

    raw_open[dev] = -1;
    sys.raw_ind--;
    return(E_SUCC);
}

/****************************************************************
*        NAME : p_raw_cmd
* DESCRIPTION : Issue a debug command to the Antares board.
*       INPUT : board_dev: Device handle of associated Antares board.
*               command  : Far pointer to a debug command structure.
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT p_raw_cmd(SHORT dev,ANT_DB_CMD far *comm)
#else
SHORT p_raw_cmd(SHORT dev,ANT_DB_CMD *comm)
#endif /* _SCO_ */
{
   /* Illegal device handler */
    if ((dev < 0) || (dev >= MAXOPENRAW)) {
	return(E_ILLEGALRAW);
    }

   /* Device handler is closed */
    if (raw_open[dev] == -1) {
	return(E_CLOSEDRAW);
    }

   /* Call antlow.c function raw_cmd() */
#ifdef LFS
    comm->data.value = raw_cmd(p_board[(int)raw_open[dev]].port,comm->type,
                               comm->address,comm->data.value,
                               comm->count,comm->data.buf);
#else
    comm->data.value = raw_cmd(p_board[raw_open[dev]].port,comm->type,
                               comm->address,comm->data.value,
                               comm->count,comm->data.buf);
#endif

    return(E_SUCC);
}

/****************************************************************
*        NAME : p_get_info
* DESCRIPTION : Recieve antares board configuration 
*       INPUT : board_id = board identification code 
*               get_cfg  = far pointer to structure containing 
*                          Antares configuration variables.      
*               get_sys  = far pointer to structure containing 
*                          Antares system variables.            
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
SHORT p_get_info(WORD dev,CFG_INFO far *info_cfg,
                 SYS_INFO	far *info_sys,DSP_INFO far *info_dsp)
{
	SHORT i;
	UINT32 lword;

    for (i = 0; i<cfg.nboards; i++) {	/* Searching for board_id */
	if (p_board[i].board_id == dev) {
#if 1
	/* Maybe I should call here Error code also and not only error mask */
	   /* Read DSP error state */
	    A4_readData(p_board[i].port,A4_CFGParm(ERRORCODE),(WORD)4,
	                (UINT32 far *)(&(p_board[i].DSPerror[0])));
#endif
	    sys.single_board = p_board[i];	/* Assign board parameters */

	   /* Read coff file names */
	    A4_readData(p_board[i].port,COFFNAMEADDRESS,(WORD)(MAXFILENAME/4),
	                (UINT32 far *)(&(sys.fwlfile[0][0])));

	    A4_readData(p_board[i].port,COFFNAMEADDRESS+8,(WORD)(MAXFILENAME/4),
	                (UINT32 far *)(&(sys.fwlfile[1][0])));

	   A4_readData(p_board[i].port,COFFNAMEADDRESS+16,(WORD)(MAXFILENAME/4),
	                (UINT32 far *)(&(sys.fwlfile[2][0])));

	   A4_readData(p_board[i].port,COFFNAMEADDRESS+24,(WORD)(MAXFILENAME/4),
	               (UINT32 far *)(&(sys.fwlfile[3][0])));
#ifdef BIG_ANT 
		p_an_byteswap((char *)&(sys.fwlfile[0][0]), MAXFILENAME/4,4);
		p_an_byteswap((char *)&(sys.fwlfile[1][0]), MAXFILENAME/4,4);
		p_an_byteswap((char *)&(sys.fwlfile[2][0]), MAXFILENAME/4,4);
		p_an_byteswap((char *)&(sys.fwlfile[3][0]), MAXFILENAME/4,4);
#endif
			/* Initiate DSP variables - See antares.h for details */
			lword = A4_read1Word(p_board[i].port, (A4_CFGParm(LINECONFIG)));
			info_dsp->LineConfig = (BYTE)lword;

			lword = A4_read1Word(p_board[i].port, (A4_CFGParm(PCMCONFIG)));
		   info_dsp->PcmConfig = (BYTE)lword;

			lword = A4_read1Word(p_board[i].port, (A4_CFGParm(ENCODING)));
			info_dsp->Encoding = (BYTE)lword;

			lword = A4_read1Word(p_board[i].port, (A4_CFGParm(SYNCH)));
			info_dsp->Sync = (BYTE)lword;

			lword = A4_read1Word(p_board[i].port, (A4_CFGParm(MAXSCSA)));
			info_dsp->MaxScsa = (WORD)lword;

			A4_readData(p_board[i].port,(A4_CFGParm(RXPOOLS)),(WORD)4,
							  (UINT32 far *)(&(info_dsp->rxDSPPools[0])));

			A4_readData(p_board[i].port,(A4_CFGParm(TXPOOLS)),(WORD)4,
							  (UINT32 far *)(&(info_dsp->txDSPPools[0])));



			/* Assign global info structures */
			*info_cfg = cfg;
			*info_sys = sys;

			return(E_SUCC);
		}
	}
	/* Board_id not found */
	return(E_BOARDNOTEXIST);
}


/****************************************************************
*        NAME : control_message
* DESCRIPTION : Send and check control sync messages to the driver
*       INPUT : 
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
SHORT control_message(SHORT prcu,SHORT board,SEND_MSG far *msg,
                      CHAR far *ansbuf,SHORT far *anslen)
{

   SYNC_REPLY reply;
   ULONG msgtype;
	RCU destrcu;
	SHORT rc;


	/* Special message type */
  	msgtype = CONTROL_MESSAGE;

	/* Generic - no specific RCU */
	if (prcu == cfg.maxopenedrcu) {
  		destrcu.board = (BYTE)board;
		destrcu.dsp = 3;
	}
	else {
  		destrcu = p_rcu[p_orcu[prcu].index].l_rcu.grcu;
	}

  	/* Special RCU for route messages */
  	destrcu.rclass = COMMAND_CLASS;
  	destrcu.unit = 0;

	/* Global reply buffer and size */
  	reply.smessage = p_reply;


  	reply.smsgsize = MAXSYNCANSWER - 1;


  	/* Sending route message */
  	rc = send_message(prcu,board,destrcu,
                         msg,
								 (SYNC_REPLY far *)(&reply),
								 msgtype,(BYTE)REPSYNC);



	if (rc != E_SUCC) {
		return(rc);
	}

	if (((UINT32 far *)(reply.smessage))[1] != E_SUCC) {
		return((SHORT)((UINT32 far *)(reply.smessage))[1]);
	}

	if ((SHORT)(((UINT32 far *)(reply.smessage))[2]) > *anslen) {
#ifdef LFS
cmn_err(0, "control_msg: reply.smessage[2] (0x%lx) is > *anslen (0x%x)\n", 
	(((UINT32 far *)(reply.smessage))[2]), *anslen);
#else
cmn_err(0, "control_msg: reply.smessage[2] (0x%x) is > *anslen (0x%x)\n", 
	(((UINT32 far *)(reply.smessage))[2]), *anslen);
#endif
		rc = E_MSGTRUNC;
	}
	else {
		*anslen = (SHORT)((UINT32 far *)(reply.smessage))[2];
	}

	if (*anslen > 0) {
		ant_memmov((BYTE *)ansbuf,
	           (ADDR *)(&(((UINT32 far *)(reply.smessage))[3])),
				  *anslen);
	}

	return(rc);
}

/****************************************************************
*        NAME : p_get_dongle
* DESCRIPTION : Gets dongle information
*       INPUT : Board identicication code;
*               Far pointer to dongle structure.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code.
*    CAUTIONS : 
****************************************************************/
SHORT p_get_dongle(WORD dev,DONGLE_INFO far *dongle_info)
{
    SHORT board;
    SEND_MSG msg;
    SHORT count;

 	for (board = 0; board<cfg.nboards; board++) {	/* Searching for board_id */
		if (p_board[board].board_id == dev) 
		{

			dongle_info->msgid = DONGLEVAR;
  			msg.msgsize = DONGLE_REQUEST;
  			/* Special message id for route messages */
  			msg.msgid = DONGLEID; /* not in use for now */
  			msg.pmessage = (BYTE far *)(&(dongle_info->msgid));
			count = DONGLE_ANSWER;
#ifdef BIG_ANT
                        p_an_byteswap((char *) &msg.pmessage[4], 1, 4);
#endif
			return(control_message(cfg.maxopenedrcu,
			                       board,
										  (SEND_MSG far *)(&msg),
										  (CHAR far *)(&(dongle_info->PartData1[0])),
										  (SHORT far *)(&count)));
		}
	}
	/* Board_id not found */
	return(E_BOARDNOTEXIST);
}

/****************************************************************
*        NAME : p_get_eeprom
* DESCRIPTION : Gets EEPROM information
*       INPUT : Board identicication code;
*               Far pointer to eeprom structure.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code.
*    CAUTIONS : 
****************************************************************/
SHORT p_get_eeprom(WORD dev,EEPROM_INFO far *eeprom_info)
{
   SHORT board;
   SEND_MSG msg;
   SHORT count;

 	for (board = 0; board<cfg.nboards; board++) {	/* Searching for board_id */
		if (p_board[board].board_id == dev) 
		{


			if (eeprom_info->msgid == EEPROMDATA)
			/* Data to send to DSP no answer */
			{
  				msg.msgsize = EEPROM_REQUEST + EEPROM_ANSWER;
			}
			else {
  				msg.msgsize = EEPROM_REQUEST;
			}

  			/* Special message id for route messages */
  			msg.msgid = EEPROMID;
  			msg.pmessage = (BYTE far *)(&(eeprom_info->msgid));
#ifdef BIG_ANT
                        p_an_byteswap((char *) &msg.pmessage[4], 1, 4);
#endif
			count = EEPROM_ANSWER;

			return(control_message(cfg.maxopenedrcu,
			                       board,
										  (SEND_MSG far *)(&msg),
										  (CHAR far *)(&(eeprom_info->EepromBuffer[0])),
										  (SHORT far *)(&count)));
		}
	}
	/* Board_id not found */
	return(E_BOARDNOTEXIST);
}


/****************************************************************
*        NAME : an_all_scbus
* DESCRIPTION : Prepares correct message for all SCBUS Functions.
*               accept those which require SYNC answer (an_getxmitslot)   
*       INPUT : RCU handler,
* 					 Pointer to SCBUS structure.
*               SCBUS option id.      
*					 SCBUS password (if needed)
*      OUTPUT : 
*     RETURNS : E_SUCC / error_code
*    CAUTIONS : 
****************************************************************/
SHORT an_all_scbus(SHORT prcu,SC_TSINFO far *scbusts,LONG  msgid,
                                         ULONG passwd,BYTE passok)
{
  SHORT frcu,board;
  SEND_MSG msg;
  RCU destrcu;

  LONG stind;

  if ((prcu < 0) || (prcu >= cfg.maxopenedrcu)) {	/* dev not found */
		return(E_ILLEGALRCU);
	}

  frcu = p_orcu[prcu].index;

  /* Illegal RCU index - RCU not opened */
  if ((frcu == -1) || (p_rcu[frcu].opened == 0)) {
  		return(E_ILLEGALRCU);
  }
  /* Finding destination RCU */
  destrcu = p_rcu[frcu].l_rcu.grcu;
  board = p_rcu[frcu].board;


  msg.msgsize = 0;
  routebuffer[msg.msgsize] = msgid;
  msg.msgsize++;

  /* Destination RCU */
  *((RCU far *)(&(routebuffer[msg.msgsize]))) = destrcu;
  msg.msgsize++;

  /* Add password if needed */
  if (passok) {
  		routebuffer[msg.msgsize] = passwd;
  		msg.msgsize++;
  }

  routebuffer[msg.msgsize] = scbusts->sc_numts;
  msg.msgsize++;


  /* Number of structures in list more than aloud by driver */
  if (scbusts->sc_numts > cfg.maxslots) {
  		return(E_ROUTELIST);
  }


  /* Fills Receive_time_slots list */
  for (stind = 0; stind < scbusts->sc_numts; stind++) {
  		routebuffer[msg.msgsize] = (scbusts->sc_tsarrayp)[stind];
  		msg.msgsize++;
  }


  msg.msgsize *= 4;

  /* Special message id for route messages */
  msg.msgid = SCBUSID;
  msg.pmessage = (BYTE far *)routebuffer;

  stind = 0; /* Reply is unexpected */

  return(control_message(prcu,
			                board,
								 (SEND_MSG far *)(&msg),
								 (CHAR far *)(&(scbusts->sc_tsarrayp[0])),
								 (SHORT far *)(&stind)));


}

/****************************************************************
*        NAME : an_get_scbus
* DESCRIPTION : Prepares correct message for all SCBUS Functions
*               which require answer (an_getxmitslot for now)   
*       INPUT : RCU handler,
* 					 Pointer to SCBUS structure.
*               SCBUS option id.      
*					 SCBUS password (if needed)
*      OUTPUT : 
*     RETURNS : E_SUCC / error_code
*    CAUTIONS : 
****************************************************************/
SHORT an_get_scbus(SHORT prcu,SC_TSINFO far *scbusts,LONG  msgid)
{
  SHORT frcu,board,rc,count;
  SEND_MSG msg;
  RCU destrcu;

  if ((prcu < 0) || (prcu >= cfg.maxopenedrcu)) {	/* dev not found */
		return(E_ILLEGALRCU);
	}


  frcu = p_orcu[prcu].index;

  /* Illegal RCU index - RCU not opened */
  if ((frcu == -1) || (p_rcu[frcu].opened == 0)) {
  		return(E_ILLEGALRCU);
  }
  /* Finding destination RCU */
  destrcu = p_rcu[frcu].l_rcu.grcu;
  board = p_rcu[frcu].board;

  msg.msgsize = 0;

  routebuffer[msg.msgsize] = msgid;
  msg.msgsize++;

  /* Destination RCU */
  *((RCU far *)(&(routebuffer[msg.msgsize]))) = destrcu;
  msg.msgsize++;

  msg.msgsize *= 4;

  /* Special message id for route messages */
  msg.msgid = SCBUSID;
  msg.pmessage = (BYTE far *)routebuffer;



  if (scbusts->sc_numts > 0) {
  	/* Change list length (longs) to bytes */
  		count = (SHORT)(scbusts->sc_numts << 2);
  }
  else {
	count = 0;
  }
  rc = control_message(prcu,
			              board,
							  (SEND_MSG far *)(&msg),
							  (CHAR far *)(&(scbusts->sc_tsarrayp[0])),
						     (SHORT far *)(&count));

  if (count > 0)	{
  		scbusts->sc_numts = (count >> 2);
  }
  else {
  		scbusts->sc_numts = 0;

  }
  return(rc);
}


/********************************************************************
 *       NAME : AN_GET_VERSION
 *DESCRIPTION : Get ANTARES device driver version number.
 *      INPUT : none.
 *     OUTPUT : hverp = major version number.
 *              lverp = minor version number.
 *    RETURNS : Return code (E_SUCC or failure error code)
 ****************************************************************/
SHORT p_get_version(ULONG far *hverp,ULONG far *lverp)
{
	*hverp = Antares_Hverp;
	*lverp = Antares_Lverp;
	return(E_SUCC);
}



/****************************************************************
*        NAME : an_all_route
* DESCRIPTION : Prepares correct message for all ROUTE Functions.
*       INPUT : RCU handler,
* 					 Pointer to Recieve_Time_Slot structure.
*					 Number of items in recieve list.
* 					 Pointer to Transmit_Time_Slot structure
*					 Number of items in transmit list.
*      OUTPUT : 
*     RETURNS : E_SUCC / error_code
*    CAUTIONS : 
****************************************************************/
SHORT an_all_route(SHORT prcu,TS_INFO far *rts,TS_INFO far *tts)
{
  SHORT frcu,board;
  SEND_MSG msg;
  ULONG msgtype;
  RCU destrcu;
  LONG stind;
  SHORT count;

  if ((prcu < 0) || (prcu >= cfg.maxopenedrcu)) {	/* dev not found */
		return(E_ILLEGALRCU);
	}

  /* Special message type for an_route() functions and for an_mapslot() function */
  msgtype = CONTROL_MESSAGE;

  frcu = p_orcu[prcu].index;

  /* Illegal RCU index - RCU not opened */
  if ((frcu == -1) || (p_rcu[frcu].opened == 0)) {
  		return(E_ILLEGALRCU);
  }
  /* Finding destination RCU */
  destrcu = p_rcu[frcu].l_rcu.grcu;
  board = p_rcu[frcu].board;

  /* AN_ROUTE start code */
  msg.msgsize = 0;
  routebuffer[msg.msgsize] = RSTART;
  msg.msgsize++;

  /* Destination RCU */
  *((RCU far *)(&(routebuffer[msg.msgsize]))) = destrcu;
  msg.msgsize++;

  /* Number of structures in list more than aloud by driver */
  if (rts->listlength > cfg.maxslots) {
  		return(E_ROUTELIST);
  }

  routebuffer[msg.msgsize] = rts->listlength;
  msg.msgsize++;

  if (rts->listlength != TS_DEASSIGN) {
  		/* Fills Receive_time_slots list */
  		for (stind = 0; stind < rts->listlength; stind++) {
  			routebuffer[msg.msgsize] = (rts->slots)[stind];
  			msg.msgsize++;
  		}
  }

  /* Number of structures in list more than aloud by driver */
  if (tts->listlength > cfg.maxslots) {
  		return(E_ROUTELIST);
  }

  routebuffer[msg.msgsize] = tts->listlength;
  msg.msgsize++;

  if (tts->listlength != TS_DEASSIGN) {
  		/* Fills Receive_time_slots list */
  		for (stind = 0; stind < tts->listlength; stind++) {
  			routebuffer[msg.msgsize] = (tts->slots)[stind];
  			msg.msgsize++;
  		}
  }

  /* AN_ROUTE end code */
  routebuffer[msg.msgsize] = REND;
  msg.msgsize++;

  /* Special message id for route messages */
  msg.msgid = ROUTEID;

  msg.msgsize *= 4;
  msg.pmessage = (BYTE far *)routebuffer;

  count = 0;
  return(control_message(prcu,
			              board,
							  (SEND_MSG far *)(&msg),
							  NULL,
						     (SHORT far *)(&count)));

}


/****************************************************************
*        NAME : p_mapslot
* DESCRIPTION : Map external SCBUS time slots to internal Antares time slots.
*       INPUT : rcu_dev: RCU device handle,
*					 mpslot:  Far pointer to a list of MAP_SLOT structures.
*      OUTPUT : 
*     RETURNS : =0 if successful; 
*             	 <0 if failure (error code).
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT p_mapslot(SHORT prcu,MAP_SLOT far *mpslot)
#else
SHORT p_mapslot(SHORT prcu,MAP_SLOT *mpslot)
#endif /* _SCO_ */
{
    SHORT frcu,board;
    SEND_MSG msg;
    SHORT stind,count;
    RCU destrcu;

    if ((prcu < 0) || (prcu >= cfg.maxopenedrcu)) {	/* dev not found */
	return(E_ILLEGALRCU);
    }

    frcu = p_orcu[prcu].index;

   /* Illegal RCU index - RCU not opened */
    if ((frcu == -1) || (p_rcu[frcu].opened == 0)) {
  	return(E_ILLEGALRCU);
    }

   /* Finding destination RCU */
    destrcu = p_rcu[frcu].l_rcu.grcu;
    board = p_rcu[frcu].board;

   /* An_mapslot() start code */
    msg.msgsize = 0;
    routebuffer[msg.msgsize] = MSTART;
    msg.msgsize++;


   /* Destination RCU */
#ifdef _SCO_
    *((RCU far *)(&(routebuffer[msg.msgsize]))) = destrcu;
#else
    *((RCU *)(&(routebuffer[msg.msgsize]))) = destrcu;
#endif /* _SCO_ */
    msg.msgsize++;

   /* Fills mapslot list */
    for (stind = 0; stind <= cfg.maxslots; stind++) {
#ifdef _SCO_
  	*((MAP_SLOT far *)(&(routebuffer[msg.msgsize]))) = mpslot[stind];
#else
  	*((MAP_SLOT *)(&(routebuffer[msg.msgsize]))) = mpslot[stind];
#endif /* _SCO_ */
  	msg.msgsize += MAPSLOT_SIZE;
	if (mpslot[stind].operation == MAP_EOL) {
	    break;
	}
    }

   /* Number of structures in list more than aloud by driver */
    if ((stind > cfg.maxslots) && (mpslot[stind].operation != MAP_EOL)) {
  	return(E_MAPSLOTLIST);
    }

   /* AN_ROUTE end code */
    routebuffer[msg.msgsize] = REND;
    msg.msgsize++;

   /* Special message id for mapslot messages */
    msg.msgid = MAPID;

    msg.msgsize *= 4;
    msg.pmessage = (BYTE far *)routebuffer;

    count = 0;
#ifdef _SCO_
    return(control_message(prcu,
                           board,
                           (SEND_MSG far *)(&msg),
                           NULL,
                           (SHORT far *)(&count)));
#else
    return(control_message(prcu,
                           board,
                           (SEND_MSG *)(&msg),
                           NULL,
                           (SHORT *)(&count)));
#endif /* _SCO_ */
}

/****************************************************************
*        NAME : p_stop
* DESCRIPTION : Stops system + All bulk data active channels
*       INPUT : 
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
#ifdef RTDL_ANT /* p_stop with bstate and lbstate */
SHORT p_stop(SHORT *bstate,SHORT *lbstate)
{
	SHORT i;
	BYTE board;

#ifdef DEBUG
cmn_err(0, "p_stop: RTDL bstate at %x, lbstate at %x\n", bstate, lbstate);
delay(50);
if ((!bstate) || (!lbstate))
   return(E_SUCC);
#endif /* DEBUG */

	/* check for hot-donload 'password' */
	if (*bstate == (SHORT)START_BOARDID) {
		/* stop only one board */
		board = *lbstate;
		for (i = 0; i< cfg.maxbd; i++) {
			if (p_bd[i].bnumber == (BYTE)board) {
				p_bd[i].bdstop = TRUE;
			}
		}

		/* Determines Driver working state */
		for (i = 0; i < cfg.nboards; i++) {
  			if (p_board[i].ready > BOARD_ADDRESSED) {
			   /* At least one board is ready */
				sys.an_state = Active;
				break;
			}
			else {
				if (p_board[i].ready == BOARD_ADDRESSED) {
					sys.an_state = Ready;
				}
			}		
		}
		return(E_SUCC);

	} else {

		/* old fashion stop - all boards */
		if (sys.an_state == Active) {
			for (i = 0; i< cfg.maxbd; i++) {
				p_bd[i].bdstop = TRUE;
			}

  			sys.an_state = Ready;
		}
		return(E_SUCC);
	}

}
#else
SHORT p_stop(VOID)
{
    SHORT i;

    if (sys.an_state == Active) {
        for (i = 0; i< cfg.maxbd; i++) {
            p_bd[i].bdstop = TRUE;
	}

        sys.an_state = Ready;
    }
    return(E_SUCC);
}
#endif
/*******************************************************************/
/*								   */
/*             OPERATING SYSTEM TRANSLATION FUNCTIONS              */           
/*								   */
/*******************************************************************/

#ifdef _SCO_
/**********************************************************************
 *       NAME : ant_filewrite
 *DESCRIPTION : writes data to output file from driver rcu data area
 *              Finds Data Pointer and call INTRFACE.xant_filewrite
 *                                       function.
 *    RETURNS : zero if successful,otherwise error number.
 *      CALLS :
 *   CAUTIONS : none.
 **********************************************************************/
SHORT ant_filewrite(SHORT bdhandle,int fh,ADDR *buf,WORD far *bcount)
{
        return(xant_filewrite(fh,buf,bcount,p_bd[bdhandle].BTransAddr));
}
 

/**********************************************************************
 *       NAME : ant_fileread
 *DESCRIPTION : reads data from input file to driver rcu data area
 *              Finds Data Pointer and call INTRFACE.xant_fileread
 *                                       function.
 *    RETURNS : zero if successful,otherwise error number.
 *      CALLS :
 *   CAUTIONS : none.
 **********************************************************************/
SHORT ant_fileread(SHORT bdhandle,int fh,ADDR *buf,WORD far *bcount)
{
        return(xant_fileread(fh,buf,bcount,p_bd[bdhandle].BTransAddr));
}
 
/**********************************************************************
 *       NAME : ant_fileseek(fh,method,rcxp,rdxp)
 *DESCRIPTION : Move file pointer to given position using the method indicated.
 *              Finds Data Pointer and call INTRFACE.xant_seekfile
 *                                       function.
 **********************************************************************/
SHORT ant_fileseek(SHORT bdhandle,int fh,WORD method,
                             LONG dist,LONG far *offs)
{
  return(xant_fileseek(bdhandle,fh,method,dist,offs,p_bd[bdhandle].BTransAddr));
}

#endif /* _SCO_ */
/****************************************************************
*        NAME : send_bulkdata_msg
* DESCRIPTION : Sending messages from bulk_data protocol.
*               Finds Data Parameter and call INTRFACE.send_bulkdata
*					 function.
*       INPUT : board_id, Line in code, bulk data device handler,
*               bulk data protocol direction,bulk data type of message,
					 Error code if exists.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error_code.
*    CAUTIONS : 
****************************************************************/
SHORT send_bulkdata_msg(BYTE inter,SHORT board_id,SHORT bd_line,
                        SHORT bd_channel,SHORT bd_direction,
	                SHORT bd_msgtype,SHORT bd_errorcode,ULONG bd_datalen)
{
    if ((bd_channel >= 0) && (bd_channel <= cfg.maxbd)) {
        return(xsend_bulkdata_msg(inter,board_id,bd_line,
                                  bd_channel,bd_direction,bd_msgtype,
                                  bd_errorcode,bd_datalen,
                                  p_bd[bd_channel].BTransAddr));
    }
#ifdef LFS
    return (0);
#endif
}

/****************************************************************
*        NAME : send_error_msg
* DESCRIPTION : Sending Fatal Error messages; 
*               Finds Data Parameter and call INTERFACE.send_error_msg
*					 function.    
*       INPUT : 
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT send_error_msg(SHORT board_id,SHORT rcu,SHORT dsp,
                     SHORT errorcode,SHORT eline)
{
    if ((rcu >= 0) && (rcu <= cfg.maxopenedrcu)) {
        return(xsend_error_msg(board_id,rcu,dsp,errorcode,eline,
			       p_orcu[rcu].RTransAddr));
    }
    else {
        return(xsend_error_msg(board_id,rcu,dsp,errorcode,eline,NULL));
    }
}

/****************************************************************
*        NAME : send_dsp_msg
* DESCRIPTION : Routine procedure of sending message from DSP to driver.
*               Finds Data Parameter and call INTERFACE.send_dsp_msg
*					 function.    
*       INPUT : board_id,message size, message type, data buffer, destination 
*               RCU, Process global_error.   
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT send_dsp_msg(BYTE inter,SHORT board_id,WORD blk,WORD msgtype,
                   WORD msgid,ADDR *data,SHORT rcudest,SHORT l_error,
                   RCU srcrcu)
{
    if ((rcudest >= 0) && (rcudest <= cfg.maxopenedrcu)) {
        return(xsend_dsp_msg(inter,board_id,blk,msgtype,msgid,data,rcudest,
	                     l_error,srcrcu,p_orcu[rcudest].RTransAddr));
    }
    else {
        return(xsend_dsp_msg(inter,board_id,blk,msgtype,msgid,data,
                             rcudest,l_error,srcrcu,NULL));
    }
}
#ifdef BIG_ANT
/****************************************************************
*        NAME : p_an_byteswap
* DESCRIPTION : byteswap an arrary of data units.
*       INPUT : data   -  pointer to the data units.
*               length - number of data units.
*               size   - size of the data units. if must be 2 or 4,
*                        where 2 means (type) USHORT, while 4 stands
*                        for (type) 32-bits integer.
*      OUTPUT : byteswaped data units.
*     RETURNS : E_SUCC.
*    CAUTIONS :
****************************************************************/
void p_an_byteswap(char *data, int size, int unit_size)
{
        char    *ptr,ctmp;
        int     i;


        /* byteswap the data units */
        ptr = data;

        switch (unit_size) {
        case    2:   /* unit type is SHORT */
                for (i = 0; i < size; i++) {
                        ctmp = ptr[0];
                        ptr[0] = ptr[1];
                        ptr[1] = ctmp;
                        ptr = ptr + 2;  /* advance to the next unit */
                }
                break;
        case    4:   /* unit type is INT, LONG, etc (32 bits wide) */
                for (i = 0; i < size; i++) {
                        ctmp = ptr[0];
                        ptr[0] = ptr[3];
                        ptr[3] = ctmp;

                        ctmp = ptr[1];
                        ptr[1] = ptr[2];
                        ptr[2] = ctmp;

                        ptr = ptr + 4;  /* advance to the next unit */
                }
                break;
        }
}
#endif
