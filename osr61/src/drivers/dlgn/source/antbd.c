/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antbd.c
 * Description                  : antares board functions
 *
 *
 **********************************************************************/

#ifdef WNT_ANT
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
#endif
#endif /* LINUX */

#include "dlgcos.h"
#include "include.h"

#include "hardware.h"
#include "hardexp.h"
#include "anprot.h"

#include "protocol.h"
#include "extern.h" 

#ifdef WNT_ANT
extern VOID cmn_err(ULONG,...);
#endif

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
extern BYTE in_mutex;
extern BYTE in_close;
#endif

#ifdef LINUX
#ifdef LFS
extern spinlock_t inthw_lock;
#else
extern lis_spin_lock_t inthw_lock;
#endif
#endif /* LINUX */

/****************************************************************
*  Local functions prototypes.
****************************************************************/

SHORT newbuf(BYTE,BYTE);

#if defined(_SCO_) || defined(LINUX)
VOID far fill_xbuf(VOID);

VOID far flush_xbuf(VOID);
#endif /* _SCO_ */

SHORT flagoff(WORD,BYTE,UINT32); 

SHORT ask_bd_token(BYTE,BYTE);
SHORT A4_release_bd_Token(BYTE,BYTE);

SHORT send_to_board(UINT32,SHORT,WORD);
SHORT send_from_board(UINT32,SHORT,WORD);


/****************************************************************
*  Local Macros
****************************************************************/

/* Read bulk data control block from DSP into "data" */
#define GET_BDCB(address,port,data) \
	(A4_readData(port,address,(WORD)BDCBSIZE,data))


/* Read one element of bulk data control block from DSP into "data" */
#define GET_IBDCB(address,port,index,data) \
	A4_readData(port,(address+index),1,(UINT32 far *)(&(data[index])))


/* Writes one element of bulk data control block from "data"  into DSP */
#define PUT_BDCB(address,port,index,data) \
	A4_writeData(port,(address+index),1,(UINT32 far *)(&(data[index])))

/****************************************************************
*  Global variables 
*  All with "b_" prefix
****************************************************************/

BDCB b_bdcbdata; /* sturcture for get/set hdcb information from dsp - regular */
BDCB b_bdcbintr; /* sturcture for get/set hdcb information from dsp - interrupt */
BDCB b_bdcbtoken;	/* sturcture for get/set hdcb information from dsp - get_bd_token */

UINT32 far *b_pbdcbdata; 
UINT32 far *b_pbdcbintr;	/* pointers to variables above */
UINT32 far *b_pbdcbtoken;


/****************************************************************
*  AN_BD_SEND_FILE Global variables 
****************************************************************/
BYTE b_fisemaphore = 0;
BYTE b_flsemaphore = 0;		/* Semaphors used to improve work of scheduler functions */

BYTE b_p_eod;
BYTE b_p_eodack;	 /* Bulk data protocol: end_of_data variables */
BYTE b_p_readereod;
BYTE b_p_hosteod;
WORD b_p_completion;

ULONG b_rsize,
      b_size1,
      b_size2, /* Bulk data protocol: global indexes */
      b_lblksz;

WORD b_sizeleft;

BYTE b_remain[5];	 /* For bd remains */



/* Scheduler functions cyclic algorithm */
SHORT s_sch = 0,e_sch = 0;

SHORT fs_sch = 0,fe_sch = 0;

extern SYS_INFO	sys;	 /* System information */

/****************************************************************
*                                                               *
*  Local functions:                                             *
*						                *
*****************************************************************/

/****************************************************************
*        NAME : init_bd_address
* DESCRIPTION : Initiates bulk data pointers (called by antares.c)
*       INPUT : void.
*      OUTPUT : 
*     RETURNS : void.
*    CAUTIONS : 
****************************************************************/
VOID init_bd_address(void)
{
    b_pbdcbdata = (UINT32 far *)ant_forceaddr((CHAR huge *)(&(b_bdcbdata)));
    b_pbdcbintr = (UINT32 far *)ant_forceaddr((CHAR huge *)(&(b_bdcbintr)));
    b_pbdcbtoken = (UINT32 far *)ant_forceaddr((CHAR huge *)(&(b_bdcbtoken)));

    s_sch = e_sch = fs_sch = fe_sch = 0;
    return;
}

/****************************************************************
*        NAME : ask_bd_token
* DESCRIPTION : Ask for specific bulk data token 
*       INPUT : Board's id, number of stream, id for internal use 
*      OUTPUT : 
*     RETURNS : TRUE / FALSE
*    CAUTIONS : 
****************************************************************/
SHORT ask_bd_token(BYTE board,BYTE nbdcb)
{
	UINT32 address;
	WORD port;
	WORD lport;

	LONG ctime;

	/* Calculating specific bulk data address */
	port = p_board[board].port;
	address = p_board[board].pbdcb+(nbdcb*BDCBSIZE);
#ifdef VME_ANT
        lport = port;
#else
	lport = A4_CONTROL_PORT(port);
#endif

	/* Asking for board token */
  	A4_requestToken(port);

	ctime = TokenCall;
	while (ctime > 0) 
	{
		ctime--;
#ifdef _SCO_
	if (A4_TOKEN_GRANT(((WORD)ainpw(lport)))) 
#else
	if (A4_TOKEN_GRANT(((WORD) A4_readControlPort(lport)))) 
#endif /* _SCO_ */
              { /* received global token */
			GET_IBDCB(address,port,0,b_pbdcbtoken); 	  
			if (((BDCB far *)b_pbdcbtoken)->SL) {	/* Bulk data token not free */
				/* Try another time */
				A4_releaseToken(port);
			}
			else {
				/* Gets bulk data token */
				((BDCB far *)b_pbdcbtoken)->SL = 1;
				PUT_BDCB(address,port,0,b_pbdcbtoken);
				A4_releaseToken(port);
				return(TRUE);
			}
			A4_requestToken(port);
		}

	}
	
	/* Unable to get bulk data token of specific stream */
	A4_releaseToken(port);
	return(FALSE);
}


/****************************************************************
*        NAME : A4_release_bd_token
* DESCRIPTION : Releases specific bulk data token 
*       INPUT : Board's id, number of stream, id for internal use 
*      OUTPUT : 
*     RETURNS : TRUE / FALSE
*    CAUTIONS : 
****************************************************************/
SHORT A4_release_bd_Token(BYTE board,BYTE nbdcb)
{
	UINT32 address;
	WORD port;
	WORD lport;

	LONG ctime;

	/* Calculating specific bulk data address */
	port = p_board[board].port;
	address = p_board[board].pbdcb+(nbdcb*BDCBSIZE);
#ifdef VME_ANT
        lport = port;
#else
	lport = A4_CONTROL_PORT(port);
#endif

	/* Asking for board token */
	A4_requestToken(port);

	ctime = TokenCall;
	while (ctime > 0) 
	{
		ctime--;
#ifdef _SCO_
	if (A4_TOKEN_GRANT(((WORD)ainpw(lport)))) 
#else
	if (A4_TOKEN_GRANT(((WORD) A4_readControlPort(lport)))) 
#endif /* _SCO_ */
	{ /* Global token received */
			/* Release bulk data token */
			((BDCB far *)b_pbdcbtoken)->SL = 0;
			PUT_BDCB(address,port,0,b_pbdcbtoken);
			A4_releaseToken(port);
			return(TRUE);
		}
	}
	
	/* Unable to release bulk data token of specific stream */
	A4_releaseToken(port);
	return(FALSE);
}

/****************************************************************
*        NAME : bd_open
* DESCRIPTION : Opens bulk data channel
*       INPUT : board number,direction,stream number,location in bd array
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT bd_open(BYTE bn,SHORT bddir,SHORT far *bdstream,SHORT loc)
#else
SHORT bd_open(BYTE bn, SHORT bddir, SHORT *bdstream,SHORT loc)
#endif /* _SCO_ */
{
    WORD port;
    UINT32 addr;
    UINT32 bdopen[2];
#ifdef LFS
    UINT32 mask = 1;
    UINT32 address = 0;
    BYTE i,j = 0;
#else
    UINT32 mask;
    UINT32 address;
    BYTE i,j;
#endif
    SHORT retcode;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif

    retcode = E_SUCC;
    port = p_board[bn].port;
    addr = A4_CFGParm(BDOPENMASK);	/* Address of bdopen mask */
	
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_enter(&inthw_lock);
    in_mutex = TRUE;
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);  /* disable interrupts */
#endif
#endif
    if (!(ask_token(port))) {	/* get dsp-pc token */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        mutex_exit(&inthw_lock);
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
  	return(E_DSPNOTOKEN);
    }

   /* Reads bulk open mask */
#ifdef _SCO_
    A4_readData(port,addr,2,(UINT32 far *)(&(bdopen[0])));
#else
    A4_readData(port,addr,2,(UINT32 *)(&(bdopen[0])));
#endif /* _SCO_ */

    if (*bdstream == AN_ANY) {	 /* Generic open */
       /* search for available stream */
	for (j = 0; j <= 1; j++) {
	    mask = 1;
	    for (i = 0; i <= 31; i++) {
	        if ((UINT32) (bdopen[j] & mask) == 0) {
		    *bdstream = (SHORT)((j * 32) + i);
		    break;
		} 
		mask <<= 1; 
	    }
	    if (*bdstream != AN_ANY) {
                break;
            }
	}

	if (*bdstream >= (SHORT) p_board[bn].nbdstream) {
	    *bdstream = AN_ANY;
	    retcode = E_BDNOFREE;
	}
	else {
	   /* get bdcb structure */
	    address = p_board[bn].pbdcb+((*bdstream)*BDCBSIZE);
        }
    }
    else { /* Open by stream number */
       /* get bdcb structure */
	address = p_board[bn].pbdcb+((*bdstream)*BDCBSIZE);
	GET_BDCB(address,port,b_pbdcbdata);
#ifdef _SCO_
        if (((BDCB far *)b_pbdcbdata)->hostOpen == 1) { /* state != closed */
#else
	if (((BDCB *)b_pbdcbdata)->hostOpen == 1) { /* state != closed */
#endif /* _SCO_ */
	    retcode = E_BDOPENED;
	}
       /* stream opened by dsp and direction requested wrong */
	else {
#ifdef _SCO_
 	    if ((((BDCB far *)b_pbdcbdata)->DSPOpen == 1) &&
 		(bddir != (SHORT)((BDCB far *)b_pbdcbdata)->direction)) {
#else
	    if ((((BDCB *) b_pbdcbdata)->DSPOpen == 1) && 
		(bddir != (SHORT) ((BDCB *)b_pbdcbdata)->direction)) {
#endif /* _SCO_ */
	        retcode = E_ILLEGALOPEN;
	    }
	    else {
		if (*bdstream > 31) {
		    j = 1;
		    mask = (1L << ((*bdstream) - 32));
		}
		else {
		    j = 0;
		    mask = (1L << (*bdstream));
		}
	    }
	}
    }

    if (retcode != E_SUCC) { /* error occured */
	A4_releaseToken(port);	/* release token */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        mutex_exit(&inthw_lock);
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
	return(retcode);
    }

   /* rewrite bulk open mask */
    bdopen[j] = (UINT32)(bdopen[j] | mask);
    A4_writeData(port,addr,2,(UINT32 far *)(&(bdopen[0])));

    A4_releaseToken(port);	/* release token */

    if (!(ask_bd_token(bn,(BYTE)(*bdstream)))) {     /* get bulk data token */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        mutex_exit(&inthw_lock);
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
  	return(E_DSPNOTOKEN);
    }

   /* Get bulk data control blobk for specific stream */
    GET_BDCB(address,port,b_pbdcbdata);

#ifdef _SCO_
        ((BDCB far *)b_pbdcbdata)->hostOpen = 1;
        /* set direction */
        ((BDCB far *)b_pbdcbdata)->direction = bddir;
#else	
    ((BDCB *)b_pbdcbdata)->hostOpen = 1;
   /* set direction */
    ((BDCB *)b_pbdcbdata)->direction = bddir;  
#endif /* _SCO_ */
    PUT_BDCB(address,port,2,b_pbdcbdata);

   /* Set Host RCU so unit contains address of bulk data stream */

#ifdef _SCO_
    ((BDCB far *)b_pbdcbdata)->hostId.unit = (BYTE)loc;
    ((BDCB far *)b_pbdcbdata)->hostId.dsp = 0xFF;
    ((BDCB far *)b_pbdcbdata)->hostId.board = 0xFF;
    ((BDCB far *)b_pbdcbdata)->hostId.rclass = 0xFF;
#else
    ((BDCB *)b_pbdcbdata)->hostId.unit = (BYTE)loc;		  
    ((BDCB *)b_pbdcbdata)->hostId.dsp = 0xFF;
    ((BDCB *)b_pbdcbdata)->hostId.board = 0xFF;
    ((BDCB *)b_pbdcbdata)->hostId.rclass = 0xFF;
#endif /* _SCO_ */

#ifdef BIG_ANT
    p_an_byteswap((&((BDCB *)b_pbdcbdata)->hostId),1, 4);
#endif
    PUT_BDCB(address,port,4,b_pbdcbdata);

    if (bddir == ANT_BDWRITE) {	       /* Zeroing end_of _data flags */
#ifdef _SCO_
        ((BDCB far *)b_pbdcbdata)->EOD = 0;
        ((BDCB far *)b_pbdcbdata)->readerEOD = 0;
#else
	((BDCB *)b_pbdcbdata)->EOD = 0;
	((BDCB *)b_pbdcbdata)->readerEOD = 0;
#endif /* _SCO_ */
    }
    else {
#ifdef _SCO_
       ((BDCB far *)b_pbdcbdata)->EODAck = 0;
       ((BDCB far *)b_pbdcbdata)->readerEOD = 0;
#else
	((BDCB *)b_pbdcbdata)->EODAck = 0;
	((BDCB *)b_pbdcbdata)->readerEOD = 0;
#endif /* _SCO_ */
    }

   /* Writes bulk data control block */
    PUT_BDCB(address,port,1,b_pbdcbdata);

   /* Release bulk data stream token */
    if (!A4_release_bd_Token(bn,(BYTE)(*bdstream))) {
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        mutex_exit(&inthw_lock);
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
  	return(E_DSPNOTOKEN);
    }

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    in_mutex = FALSE;
    mutex_exit(&inthw_lock);
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return(E_SUCC);
}

/****************************************************************
*        NAME : bd_close
* DESCRIPTION : closes bulk data channel
*       INPUT : board index, stream number
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
SHORT bd_close(BYTE board,BYTE stream)
{
    WORD port;
    UINT32 address;
    UINT32 mask,addr;
    UINT32 bdopen[2];
    BYTE bdindex;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif

    port = p_board[board].port;

   /* Finds BDCB for stream */
    address = p_board[board].pbdcb+((stream)*BDCBSIZE);
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    if (!in_close) {
        mutex_enter(&inthw_lock);
    }
    in_mutex = TRUE;
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);  /* disable interrupts */
#endif
#endif
    if (!(ask_bd_token(board,stream))) {	/* get dsp-pc token */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        if (!in_close) {
            mutex_exit(&inthw_lock);
        }
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
        return(E_DSPNOTOKEN);
    }

   /* Gets bulk data control block of the stream */
    GET_BDCB(address,port,b_pbdcbdata);

   /* Host state not opened */
#ifdef _SCO_
    if (((BDCB far *)b_pbdcbdata)->hostOpen == FALSE) {
#else
    if (((BDCB *)b_pbdcbdata)->hostOpen == FALSE) { 
#endif /* _SCO_ */
        if (!A4_release_bd_Token(board,stream)) {
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
            in_mutex = FALSE;
            if (!in_close) {
                mutex_exit(&inthw_lock);
            }
#elif LINUX
            DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
            DLGCSPLX(oldspl);
#endif
#endif
  	    return(E_DSPNOTOKEN);
  	}
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        if (!in_close) {
            mutex_exit(&inthw_lock);
        }
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
 	return(E_BDNOTOPEN);
    }

   /* Transfer flag is set */

#ifdef _SCO_
    if (((BDCB far *)b_pbdcbdata)->TF) {
#else
    if (((BDCB *)b_pbdcbdata)->TF) {
#endif /* _SCO_ */
        if (!A4_release_bd_Token(board,stream)) {
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
            in_mutex = FALSE;
            if (!in_close) {
                mutex_exit(&inthw_lock);
            }
#elif LINUX
            DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
            DLGCSPLX(oldspl);
#endif
#endif
  	    return(E_DSPNOTOKEN);
  	}
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        in_mutex = FALSE;
        if (!in_close) {
            mutex_exit(&inthw_lock);
        }
#elif LINUX
        DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
        DLGCSPLX(oldspl);
#endif
#endif
        return(E_BDISACTIVE);
    }

   /* Clears host id */
#ifdef _SCO_
    ((BDCB far *)b_pbdcbdata)->hostId.unit = 0;
    ((BDCB far *)b_pbdcbdata)->hostId.dsp = 0;
    ((BDCB far *)b_pbdcbdata)->hostId.board = 0;
    ((BDCB far *)b_pbdcbdata)->hostId.rclass = 0;
#else
    ((BDCB *)b_pbdcbdata)->hostId.unit = 0;
    ((BDCB *)b_pbdcbdata)->hostId.dsp = 0;
    ((BDCB *)b_pbdcbdata)->hostId.board = 0;
    ((BDCB *)b_pbdcbdata)->hostId.rclass = 0;
#endif /* _SCO_ */
    PUT_BDCB(address,port,4,b_pbdcbdata);

   /* Clears host state */
#ifdef _SCO_
    ((BDCB far *)b_pbdcbdata)->hostOpen = 0;
#else
    ((BDCB *)b_pbdcbdata)->hostOpen = 0;
#endif /* _SCO_ */

   /* Updates bulk data control block of the stream */
    PUT_BDCB(address,port,2,b_pbdcbdata);

   /* Dsp state is closed */
#ifdef _SCO_
    if (((BDCB far *)b_pbdcbdata)->DSPOpen == FALSE) {
#else
    if (((BDCB *)b_pbdcbdata)->DSPOpen == FALSE) {
#endif /* _SCO_ */

       /* Release bulk data stream token */
        if (!A4_release_bd_Token(board,stream)) {
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
            in_mutex = FALSE;
            if (!in_close) {
                mutex_exit(&inthw_lock);
            }
#elif LINUX
            DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
            DLGCSPLX(oldspl);
#endif
#endif
  	    return(E_DSPNOTOKEN);
  	}

       /********************************/
       /* Updating bdopen global mask  */
       /********************************/
	addr = A4_CFGParm(BDOPENMASK);
	mask = 1L;
	if (stream <= 31) {
	    mask = (UINT32)(mask << stream);
	    bdindex = 0;
	}
	else {
	    stream -= 32;
	    mask = (UINT32)(mask << stream);
	    bdindex = 1;
	}
  	if (!(ask_token(port))) {	/* get dsp-pc token */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
            in_mutex = FALSE;
            if (!in_close) {
                mutex_exit(&inthw_lock);
            }
#elif LINUX
            DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
            DLGCSPLX(oldspl);
#endif
#endif
  	    return(E_DSPNOTOKEN);
  	}

       /* Reading bdopen global mask  */
#ifdef _SCO_
	A4_readData(port,addr,2,(UINT32 far *)(&(bdopen[0])));
#else
	A4_readData(port,addr,2,(UINT32 *)(&(bdopen[0])));
#endif /* _SCO_ */
       /* Updating mask */
	bdopen[bdindex] = (bdopen[bdindex] & (~mask));
       /* Writing bdopen global mask  */
#ifdef _SCO_
	A4_writeData(port,addr,2,(UINT32 far *)(&(bdopen[0])));
#else
	A4_writeData(port,addr,2,(UINT32 *)(&(bdopen[0])));
#endif /* _SCO_ */

	A4_releaseToken(port);	/* release token */
    }
    else {
       /* If DSP state is open, nothing is done */
   	if (!A4_release_bd_Token(board,stream)) {
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
            in_mutex = FALSE;
            if (!in_close) {
                mutex_exit(&inthw_lock);
            }
#elif LINUX
            DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
            DLGCSPLX(oldspl);
#endif
#endif
  	    return(E_DSPNOTOKEN);
	}
    }
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    in_mutex = FALSE;
    if (!in_close) {
        mutex_exit(&inthw_lock);
    }
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return(E_SUCC);
}

#if defined(_SCO_) || defined(LINUX)
/****************************************************************
*        NAME : check_xrec
* DESCRIPTION : Checks legality of x_rec structure
*       INPUT : location in bulk data array
*      OUTPUT :
*     RETURNS : E_SUCC / Error code number
*    CAUTIONS :
****************************************************************/
SHORT check_xrec(SHORT loc, P_FILE *pfile)
{
    ULONG offs;
    SHORT rc;
 
    if (pfile->option & XPLAY_F) {    /* Record to file option */
        if (pfile->handler <= 0) {    /* Illegal input file handler */
            return(E_INVALIDHANDLER);
        }
 
       /* Seek for start position in output file */
    rc = ant_fileseek(loc,pfile->handler,0,pfile->fstart,(LONG far *)(&(offs)));
 
        if ((rc != E_SUCC)) { /* seek failed */
            return(E_ERRORSEEK);
        }
        p_bd[loc].pfstart = offs;
   }
   else {
        if (pfile->option & XPLAY_B) { /* Record to buffer option */
            return(E_SUCC);
        }
        else {
            if (pfile->option & XPLAY_END) {  /* End of record list */
                return(E_SUCC);
            }
            else {
                return(E_XPLAY_ILLEGAL);          /* Invalid option */
            }
        }      
    }
    return(E_SUCC);
}
#endif /* _SCO_ */

/*******************************************************************
*
*		 BULK_DATA INTERRUPT SERVICE UTILITIES
*
*******************************************************************/

/****************************************************************
*        NAME : newbuf
* DESCRIPTION : switches between bulk data ping_pong buffers
*       INPUT : location in bulk_data_array,process_direction
*      OUTPUT : 
*     RETURNS : TRUE / FALSE
*    CAUTIONS : 
****************************************************************/
SHORT newbuf(BYTE loc,BYTE method)

{
	SHORT rc;
	BYTE cr;

	if (p_bd[loc].fillbuf == TRUE) 

	/* Former buffer is not yet filled/cleared, it means that if we'll
		switch between ping-pong buffers, data will be currupted.
		so SINGLE overrun/underrun message is sent to message queue.
		and switching between buffers will be postponed until the proper
		scheduler function will operatae on buffer
		.async = TRUE - means application expecting messages. 
		.overrun = TRUE - means overrun message already sent for this 
		                  specific overrun state                     */
	{
	   if ((p_bd[loc].async) && (p_bd[loc].overrun == FALSE))
		{
			/* Overrun message should be sent */
			if (method == ANT_BDREAD) {
				send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,method,ANT_ASYNC_ERROR,E_OVERRUN,p_bd[loc].datalen);
			}
			else {
				send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,method,ANT_ASYNC_ERROR,E_UNDERRUN,p_bd[loc].datalen);
			}
			p_bd[loc].overrun = TRUE;	/* No more messages for this problem */
		}
	 	return(FALSE);
	}


	/* initiates current read_buffer to be filled and move to next buffer */ 
	
	cr = p_bd[loc].rcu_readbuf;
	if (method == ANT_BDWRITE) {
		p_bd[loc].offset = 0;
		p_bd[loc].rcu_buf[cr].size = 0;
	}
	p_bd[loc].rcu_readbuf = (BYTE)!(cr);

	p_bd[loc].fillbuf = TRUE;	/* fill new buffer */
	p_bd[loc].overrun = FALSE;	/* Ready for overrun message */

	sch_arr[e_sch] = loc; /* Increament cyclic array for scheduler policy */
	e_sch++;
	if (e_sch == SchSize) {
		e_sch = 0;
	}


	if (b_fisemaphore == 0)		 
	   /* means proper scheduler function is not in queue */
	{
		/* No need to fill eob declared before */
		/* Prevents two error messages         */
		if (p_bd[loc].rcu_buf[!cr].eob == TRUE) {
			p_bd[loc].fillbuf = FALSE;	/* fill new buffer */
			return(TRUE);
		}
		/* Sending proper scheduler function for multi_file process */
	rc = send_delay_request(loc,1,cr,method,cfg.bd_single,p_bd[loc].BTransAddr);
	if (rc != E_SUCC) {
		/* Error using scheduler queue */
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
		           __LINE__,loc,method,ANT_ASYNC_ERROR,E_REQSQUEUE,p_bd[loc].datalen);
			return(FALSE);
	}
	}

	return(TRUE);
}


/****************************************************************
*        NAME : flagoff
* DESCRIPTION : Turns off bit in bd_mask
*       INPUT : port address, bd stream number, bit mask
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : Token should be taken and released by calling procedure
****************************************************************/
SHORT flagoff(WORD port,BYTE nbdcb,UINT32 mask)
{
	UINT32 address;
   UINT32 bdopen[2];
	BYTE   bdindex;

	/* address of bulk data buffer mask */
	address = A4_CFGParm(BDBUFMASK);
	if (nbdcb <= 31) { 
		bdindex = 0;
	}
	else {
		bdindex = 1;
	}
  	if (!(ask_token(port))) {	/* get dsp-pc token */
  		return(E_DSPNOTOKEN);
  	}

   /* clears buffer mask */
	A4_readData(port,address,2,(UINT32 far *)(&(bdopen[0])));
	bdopen[bdindex] = (bdopen[bdindex] & (~mask));
	A4_writeData(port,address,2,(UINT32 far *)(&(bdopen[0])));

	A4_releaseToken(port);	/* release token */

	return(TRUE);
}


/*******************************************************************
*
*	BULK_DATA INTERRUPT FUNCTION:
*
*******************************************************************/

/******************************************************************/
/* BD_PROTOCOL macro: Common end of process - defined as macro    */
/* ****************************************************************/
#define ENDPROCESS(port,board,nbdcb,mask,address) \
	if (!(ask_bd_token(board,nbdcb))) {			  \
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id, \
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	\
		return;  										  \
  	}				   									  \
	GET_IBDCB(address,port,1,b_pbdcbintr); 	  \
   ((BDCB far *)b_pbdcbintr)->EOD = b_p_eod;						  \
	((BDCB far *)b_pbdcbintr)->EODAck = b_p_eodack;				  \
	((BDCB far *)b_pbdcbintr)->readerEOD = b_p_readereod;		  \
	((BDCB far *)b_pbdcbintr)->IPF = t_ipf;						  \
	((BDCB far *)b_pbdcbintr)->TF = 0;			            	  \
	((BDCB far *)b_pbdcbintr)->HostEOD = b_p_hosteod;   	  \
	((BDCB far *)b_pbdcbintr)->completion = b_p_completion; 	  \
	PUT_BDCB(address,port,1,b_pbdcbintr);		                 \
   if (!A4_release_bd_Token(board,nbdcb)) {            \
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,  \
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	\
		return;										                    \
  	}													  

/****************************************************************
*        NAME : bd_protocol
* DESCRIPTION : Called by interrupt procedure, activates read/write operation
*       INPUT : board number,stream number,bdopen mask.
*      OUTPUT : 
*     RETURNS : operation result
*    CAUTIONS : 
****************************************************************/

VOID far bd_protocol(BYTE board,BYTE nbdcb,UINT32 mask)
{
	UINT32 address;
#ifndef LFS
	SHORT loc,err,rtcode,cr,rc;
#else
	SHORT loc,err,cr,rc;
#endif
	WORD port;
	SHORT  t_ipf;			/* local IPF */

	/* initiation of return codes & variables */
 	port = p_board[board].port;
	address = p_board[board].pbdcb+(nbdcb*BDCBSIZE);

	loc = 0;

	/* bulk_data token should be taken before possible use of flagoff */
  	if (!(ask_bd_token(board,nbdcb))) {
		/* Fatal error message */
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
			 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 	return;
  	}
	
	/* Set bd_open flag off */
	if (!flagoff(port,nbdcb,mask)) {
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
   	if (!A4_release_bd_Token(board,nbdcb)) {
	 		return;
		}
	 	return;
	}

	/* Gets bulk data control block */
	GET_BDCB(address,port,b_pbdcbintr);
#ifdef BIG_ANT
        p_an_byteswap((&((BDCB *)b_pbdcbintr)->hostId),1, 4);
#endif
	/* HostId.unit includes location in bulk data array */
	loc = ((BDCB far *)b_pbdcbintr)->hostId.unit;

	/* TF not set means DSP error */

	if (((BDCB far *)b_pbdcbintr)->TF == FALSE) {
   	if (!A4_release_bd_Token(board,nbdcb)) {
			send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 		return;
		}
	 	return;
  	}													  


	/****************************************************************
	 *  Checking Antares inner data + variables 							 *
	 ****************************************************************/
  
	/* Next three checks are of REMEMBER options, means DSP is not yet 
	   ready for bulk_data protocol. process should be postponed until 
		DSP is ready 
	*/
		
	if ((loc < 0) || (loc >= cfg.maxbd)) {


   	if (!A4_release_bd_Token(board,nbdcb)) {
			send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 		return;
		}
	 	return;
	}
	
	/* Illegal bulk data control block values */
	if ((p_bd[loc].direction == BDCLOSE) || (p_bd[loc].snumber != nbdcb)) 
	{
		if (!A4_release_bd_Token(board,nbdcb)) {
			send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 		return;
		}
	 	return;
	}
	
	 /* no outstanding write */
	if (p_bd[loc].state_memory == FALSE) {

   	
		if (!A4_release_bd_Token(board,nbdcb)) {
			send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
	  			 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 		return;
		}
	 	return;
	}

	/* System_error because bd_open should have opened 
	   hostopen if direction != -1  */

	if (((BDCB far *)b_pbdcbintr)->hostOpen == FALSE) {
		/* Fatal error message */


		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_BDHOSTNOTOPEN,p_bd[loc].datalen);	
		/* turn off bit flag */
   	if (!A4_release_bd_Token(board,nbdcb)) {
			send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 		return;
		}
	 	return;
	}

	 /****************************************************************
	 *  Checking Dsp   data + variables 							 		 *
	 ****************************************************************/
	/* Set in process flag */
	t_ipf = 1;						/* set the local IPF */


	/* set the global IPF */
	((BDCB far *)b_pbdcbintr)->IPF = t_ipf;		
	PUT_BDCB(address,port,1,b_pbdcbintr);

   if (!A4_release_bd_Token(board,nbdcb)) {
		send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,p_bd[loc].direction,ANT_ASYNC_ERROR,E_DSPNOTOKEN,p_bd[loc].datalen);	
	 	return;
	}

	/* Init end of data flags */
   b_p_eod  =   (BYTE)((BDCB far *)b_pbdcbintr)->EOD;
	b_p_eodack = (BYTE)((BDCB far *)b_pbdcbintr)->EODAck;
	b_p_readereod = (BYTE)((BDCB far *)b_pbdcbintr)->readerEOD;

	b_p_completion = (WORD)((BDCB far *)b_pbdcbintr)->completion;
	b_p_hosteod = 0;

/****************************************************************
 *  Four processes are available (Driver = D, Kernel+Dsp = K) : *
 *  PLAY:                                                       *
 *  -----                                                       *
 * 1. EOD + Int (D)	-> EODACK + Int (K) -> end process (D). *
 * 2. ReaderEod + Int (K) -> EODACK + Int + end_process(D).     *
 *							        *
 *  RECORD:                                                     *
 *  -------                                                     *
 * 3. EOD + Int (K) -> EODACK + end process + Int (D) ->.       *
 * 4. Getdata + ReaderEod + Int (D) -> EODACK + Int (K) ->.	*
 *    Always reciever of EODACK zeroing all three               *
 ****************************************************************/

   /****************************************************************
    *  Starting bulk data protocol ANT_BDREAD                      *
    ****************************************************************/

    if (((BDCB far *)b_pbdcbintr)->direction == ANT_BDREAD) {
       /* Kernel EODACK after ReaderEod - Protocol 4: */
        if (b_p_eodack) {	
            b_p_eod = 0;
            b_p_eodack = 0;
            b_p_readereod = 0;
            b_p_completion = 0;
            p_bd[loc].state_memory = FALSE;
            t_ipf = 0;    /* reset IPF */
            ENDPROCESS(port,board,nbdcb,mask,address);

           /* Call to flush buffer protocol */
            cr = p_bd[loc].rcu_readbuf;  /* current read buffer */
            p_bd[loc].rcu_buf[cr].eob = 0xFF;

           /* Increament cyclic array for scheduler policy */
            flsh_arr[fe_sch] = (BYTE)loc;
            fe_sch++;
            if (fe_sch == SchSize) {
		fe_sch = 0;
            }

           /* No active flush function in scheduler */
            if (b_flsemaphore == 0) {
               /* Asking for flush_buffer scheduler functions */
	        rc = send_delay_request(loc,0,(BYTE)(cr),
                                (BYTE)((BDCB far *)b_pbdcbintr)->direction,
	                        cfg.bd_single,p_bd[loc].BTransAddr);
            }
            return;
        }

       /* Perform read_data protocol */
		err = send_from_board(address,loc,port);

		b_p_eodack = b_p_eod;

		/* state 3 */
		if (b_p_eod) {
			/* Saves completion code for later debugging */
			p_bd[loc].Completion = b_p_completion;
			p_bd[loc].state_memory = FALSE;

			/* Call to flush buffer protocol */
			cr = p_bd[loc].rcu_readbuf;  /* current read buffer */
			p_bd[loc].rcu_buf[cr].eob = 0xFF;

			flsh_arr[fe_sch] = (BYTE)loc;  /* Increament cyclic array for scheduler policy */
			fe_sch++;
			if (fe_sch == SchSize) {
				fe_sch = 0;
			}

			if (b_flsemaphore == 0) { /* No active flush function in scheduler */
				/* Asking for flush_buffer scheduler functions */
	rc = send_delay_request(loc,0,(BYTE)(cr),(BYTE)((BDCB far *)b_pbdcbintr)->direction,
	                                      cfg.bd_single,p_bd[loc].BTransAddr);
			}

		}
		else {
			if (p_bd[loc].bdstop) {	/* Stopped by user */
				p_bd[loc].Completion = b_p_completion;	  /* Gets DSP competion code */
				if (p_bd[loc].HostStop != 0) {
					b_p_hosteod = p_bd[loc].HostStop;		  /* Gets Host side end of data reason */
				}
				else {
					b_p_hosteod = HOSTSTOP;
				}

				b_p_readereod = TRUE;
			}
			/* error occurred: if async then like reader eod */
			else {
				if (err != E_SUCC) {				/* Probably queue problems */
					if (p_bd[loc].click) {
						ant_id_click();
					}
					/* if process should be stopped while error */
					if (p_bd[loc].enderror) {	
						b_p_readereod = TRUE;
						

						b_p_hosteod = HOSTSTOP; /* Gets Host side end of data reason */

						p_bd[loc].state_memory = FALSE;
						p_bd[loc].onprocess = FALSE; 

						if (p_bd[loc].async) {
							p_bd[loc].Completion = b_p_completion;
							send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 			__LINE__,loc,ANT_BDREAD,ANT_ASYNC_CMPLT,p_bd[loc].Completion,p_bd[loc].datalen);

						}
					}
				}
		   }
		}
	}

	/****************************************************************
	 *  Starting bulk data protocol ANT_BDWRITE                     *       
	 ****************************************************************/
	else {
		if (((BDCB far *)b_pbdcbintr)->direction == ANT_BDWRITE) 
		{
			/* State 1 (EOD) :DSP EODACK: got after driver eod   */
			if (b_p_eodack) {	
     			b_p_eod = 0;
				b_p_eodack = 0;
	      	b_p_readereod = 0;
				t_ipf = 0;						/* reset IPF */
        		ENDPROCESS(port,board,nbdcb,mask,address);
				if (p_bd[loc].async) 
				{
					if ((p_bd[loc].state_memory	== (BYTE)ANT_ASYNC_CMPLT) ||
						 (p_bd[loc].state_memory	== (BYTE)ANT_ASYNC_READEREOD))
					{
						send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,ANT_BDWRITE,p_bd[loc].state_memory,b_p_completion,p_bd[loc].datalen);

					}
					else {
						send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_ERROR,E_BADSTATEMEMORY,p_bd[loc].datalen);
					}
				}
				b_p_completion = 0;
				p_bd[loc].state_memory = FALSE;
			   p_bd[loc].onprocess = FALSE; 
	 			return;
			}
			
			/* State2 : Stop request from DSP */
			if (b_p_readereod) {
				b_p_eodack = 1;
				p_bd[loc].fillbuf = FALSE;
				p_bd[loc].state_memory = FALSE;
			   p_bd[loc].onprocess = FALSE; 

        		ENDPROCESS(port,board,nbdcb,mask,address);
				A4_interruptDSP(port,(SHORT)((BDCB far *)b_pbdcbintr)->DSPNumber);   /* interrupt dsp */

				if (p_bd[loc].async)
				{
					send_bulkdata_msg(TRUE,p_board[p_bd[loc].bnumber].board_id,
				 __LINE__,loc,ANT_BDWRITE,ANT_ASYNC_READEREOD,b_p_completion,p_bd[loc].datalen);
				}
	 			return;
			}
		   /* Perform write_data protocol */
			err = send_to_board(address,loc,port);

			/* end of data or error occured */
			if (err != E_SUCC) {
				if (p_bd[loc].click) {
					ant_id_click();
				}
				/* if process should be stopped while error */
				if (p_bd[loc].enderror) {
					b_p_eod = TRUE;
					b_p_hosteod = HOSTSTOP; /* Gets Host side end of data reason */
					p_bd[loc].state_memory	= ANT_ASYNC_CMPLT;
					/* ???? */
					p_bd[loc].fillbuf = FALSE;
				}
			}
			else {
				if (b_p_eod)
				{
		 				p_bd[loc].state_memory	= ANT_ASYNC_CMPLT;
						/* Gets Host side end of data reason */
						b_p_hosteod = HOSTEOD; 

		}
                else {
		    if (p_bd[loc].bdstop) { 
                       /* Gets Host side end of data reason */
			b_p_hosteod = HOSTSTOP;
		  	b_p_eod = TRUE;
		 	p_bd[loc].state_memory	= ANT_ASYNC_CMPLT;
			p_bd[loc].fillbuf = FALSE;
		    }
		}
            }
        }
    }
   /* common end of protocol */
    ENDPROCESS(port,board,nbdcb,mask,address);
   /* Announe DSP for end of protocol */
   /* interrupt dsp */
    A4_interruptDSP(port,(SHORT)((BDCB far *)b_pbdcbintr)->DSPNumber);
    return;
}
							  
/****************************************************************
*        NAME : send_to_board
* DESCRIPTION : Sends data to board	protocol
*       INPUT : BDCB address, location in local bdbc array, Board port
*      OUTPUT : 
*     RETURNS : E_SUCC / error
*    CAUTIONS : 
****************************************************************/
SHORT send_to_board(UINT32 address,SHORT loc,WORD port)
{

	BYTE cr;	 /* current read */
	CHAR far *psrc1;
	CHAR far *ptmp;
	ULONG far *psrc;
	ULONG ptar;

	/****************************************************************
	 *  Overrun state is active                   						 *
	 ****************************************************************/

	/* Overrun state == true; Overrun message already sent */
	if ((p_bd[loc].fillbuf == TRUE) && (p_bd[loc].overrun == TRUE))
	{
		((BDCB far *)b_pbdcbintr)->dataLength = 0;
		PUT_BDCB(address,port,9,b_pbdcbintr);
		return(E_SUCC);
	} 

	/* finds current read buffer */
	cr = p_bd[loc].rcu_readbuf;

	psrc = (ULONG far *)((CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].offset])));
	
	/* determines addresses for process */
	ptar = (ULONG)((BDCB far *)b_pbdcbintr)->pBuffer;


	/* DSP buffer size */
	b_rsize = ((BDCB far *)b_pbdcbintr)->bufferSize;

	/* not aloud, size bigger than page size */
	if (b_rsize >= EMSPAGESIZE)
	{
		b_rsize = EMSPAGESIZE;
	}		

	/* not aloud, size bigger than driver buffer size */
	if (b_rsize > (ULONG)cfg.bd_single)
	{
		b_rsize = (ULONG)cfg.bd_single;
	}		


	/* remind left */
	b_size1 = p_bd[loc].rcu_buf[cr].size - p_bd[loc].offset;

	/* simple action: move data from single I/O buffer */
	if ((b_size1 >= b_rsize) ||	(p_bd[loc].rcu_buf[cr].eob == TRUE)) {

		if (b_size1 < b_rsize) {	 /* means end of blobk */
			b_p_eod = TRUE;
		}
		else {
			b_size1 = b_rsize;
		}
		/* check for zero case */
		b_lblksz = BTOWORD(b_size1);


		/* Write data to board */
#ifdef BIG_ANT
		A4_writeData(port|_A4_NO_SWAP,ptar,(WORD)b_lblksz,psrc);
#else
		A4_writeData(port,ptar,(WORD)b_lblksz,psrc);
#endif
		/* increament bulk_data buffer offset */
		p_bd[loc].offset += b_size1;

		((BDCB far *)b_pbdcbintr)->dataLength = b_size1;
	   PUT_BDCB(address,port,9,b_pbdcbintr);

		/* Reached end of local buffer -> means switch buffers or end_of_data */
	   if (p_bd[loc].offset == p_bd[loc].rcu_buf[cr].size) {
			if (p_bd[loc].rcu_buf[cr].eob == TRUE) {
				/* end of data */
				b_p_eod = TRUE;
			}
			else {
				if (newbuf((BYTE)loc,ANT_BDWRITE) == FALSE) {
					/* overrun problem */
					return(-1);
				}
			}
		}
	}

	/* Data to be moved is more then exist in current buffer */
	else {

		/* number of BYTEs reading from current buffer */
		b_size2 = b_rsize - b_size1;
		/* remainder value */
		b_sizeleft = (WORD)((WORD)b_size1 & 0x0003);
		b_size1 -= b_sizeleft;
		b_lblksz = 0;
		if (b_size1 > 0) {
			b_lblksz = BTOWORD(b_size1);  /* convertion to UINT32 */
#ifdef BIG_ANT
			A4_writeData(port|_A4_NO_SWAP,ptar,(WORD)b_lblksz,psrc);
#else
			A4_writeData(port,ptar,(WORD)b_lblksz,psrc);
#endif
			p_bd[loc].offset += b_size1;
		}

		/* copy reminder to preface of next buffer */
		if (b_sizeleft > 0) {
			psrc1 = (CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].offset]));
			ptmp = (CHAR far *)(&(b_remain[0]));
			ant_memmov((ADDR *)ptmp,(ADDR *)psrc1,b_sizeleft); 
		}
		/* switch I/O buffers */
		if (newbuf((BYTE)loc,ANT_BDWRITE) == FALSE) {
			/* overrun problem */
			((BDCB far *)b_pbdcbintr)->dataLength = b_size1;
	   	PUT_BDCB(address,port,9,b_pbdcbintr);
			return(-1);
		}

		/* starts read from next buffer */
		cr = p_bd[loc].rcu_readbuf;  /* current read buffer */

		if (b_sizeleft > 0) {
		      psrc1 = (CHAR far *)((CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].offset])));
			ptmp = (CHAR far *)(&(b_remain[0]));
			ant_memmov((ADDR *)psrc1,(ADDR *)ptmp,b_sizeleft); 
		}

		psrc = (ULONG far *)((CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].offset])) + b_sizeleft);
		ptar = (ULONG)((BDCB far *)b_pbdcbintr)->pBuffer + (ULONG)b_lblksz;

   /* Moving what is left from data to new buffer */ 
    if ((p_bd[loc].offset + b_size2) < p_bd[loc].rcu_buf[cr].size) {
	b_rsize = b_size2 + b_sizeleft;
	((BDCB far *)b_pbdcbintr)->dataLength =  b_size1 + b_size2 + b_sizeleft;
    }
    else {
       /* End of data state */
	b_rsize = p_bd[loc].rcu_buf[cr].size + b_sizeleft;
	b_p_eod = TRUE;
	((BDCB far *)b_pbdcbintr)->dataLength =  b_size1 + p_bd[loc].rcu_buf[cr].size + b_sizeleft;
    }

		b_lblksz = BTOWORD(b_rsize);
#ifdef BIG_ANT
		A4_writeData(port|_A4_NO_SWAP,ptar,(WORD)b_lblksz,psrc);
#else
		A4_writeData(port,ptar,(WORD)b_lblksz,psrc);
#endif
		p_bd[loc].offset += b_rsize;
	   PUT_BDCB(address,port,9,b_pbdcbintr);
	}

	return(E_SUCC);
}

/****************************************************************
*        NAME : send_from_board
* DESCRIPTION : recieves data from board
*       INPUT : BDCB address, location in local bdbc array, Board port
*      OUTPUT : 
*     RETURNS : E_SUCC / error
*    CAUTIONS : 
****************************************************************/
SHORT send_from_board(UINT32 address,SHORT loc,WORD port)
{

	BYTE cr;	 /* current read */
	ULONG far *ptar;
	ULONG psrc;

	/* Overrun state == true; Overrun message already sent */
	if ((p_bd[loc].fillbuf == TRUE) && (p_bd[loc].overrun == TRUE))
	{
		((BDCB far *)b_pbdcbintr)->dataLength = 0;
		PUT_BDCB(address,port,9,b_pbdcbintr);
		return(E_SUCC);
	} 

	/* determines addresses for process */
	cr = p_bd[loc].rcu_readbuf;

	ptar = (ULONG far *)((CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].rcu_buf[cr].size])));
	psrc = (ULONG)((BDCB far *)b_pbdcbintr)->pBuffer;

	/* DSP buffer size */
	b_rsize = ((BDCB far *)b_pbdcbintr)->bufferSize;


	/* not aloud, size bigger than page size */
	if (b_rsize >= EMSPAGESIZE)
	{
		b_rsize = EMSPAGESIZE;
	}		

	/* not aloud size bigger than buffer size */
	if (b_rsize > (ULONG)cfg.bd_single) {
		b_rsize = (ULONG)cfg.bd_single;
	}


	b_size1 = cfg.bd_single - p_bd[loc].rcu_buf[cr].size;

	/* simple action: move data from single I/O buffer */
	if (b_size1 >= b_rsize) {
		b_size1 = b_rsize;
		b_lblksz = BTOWORD(b_size1);
#ifdef BIG_ANT
		A4_readData(port|_A4_NO_SWAP,psrc,(WORD)b_lblksz,ptar);
#else
		A4_readData(port,psrc,(WORD)b_lblksz,ptar);
#endif
		p_bd[loc].rcu_buf[cr].size += b_size1;
		
		((BDCB far *)b_pbdcbintr)->dataLength = b_size1;
	   PUT_BDCB(address,port,9,b_pbdcbintr);
	}
	/* move more then exist in buffer */
	else {
  		/* number of BYTEs reading from next buffer */
		b_size2 = b_rsize - b_size1;
		/* calculating remainder */
		b_sizeleft = (WORD)((WORD)b_size1 & 0x0003);
		b_size1 -= b_sizeleft;
		b_size2 += b_sizeleft;
		b_lblksz = 0;
		if (b_size1 > 0) {
			b_lblksz = BTOWORD(b_size1);
#ifdef BIG_ANT
			A4_readData(port|_A4_NO_SWAP,psrc,(WORD)b_lblksz,ptar);
#else
			A4_readData(port,psrc,(WORD)b_lblksz,ptar);
#endif
			p_bd[loc].rcu_buf[cr].size += b_size1;
		}

		/* switch I/O buffers */
		if (newbuf((BYTE)loc,ANT_BDREAD) == FALSE) {
			/* overrun problem */
			if (b_size1 > 0) {
				((BDCB far *)b_pbdcbintr)->dataLength = b_size1;
	   		PUT_BDCB(address,port,9,b_pbdcbintr);
			}
			return(-1);
		}

		/* starts read from next buffer */
		cr = p_bd[loc].rcu_readbuf;  /* current read buffer */

		ptar = (ULONG far *)((CHAR far *)(&(p_bd[loc].rcu_buf[cr].buf[p_bd[loc].rcu_buf[cr].size])));

		psrc = (ULONG)((BDCB far *)b_pbdcbintr)->pBuffer + (ULONG)b_lblksz;
		b_lblksz = BTOWORD(b_size2);
#ifdef BIG_ANT
		A4_readData(port|_A4_NO_SWAP,psrc,(WORD)b_lblksz,ptar);
#else
		A4_readData(port,psrc,(WORD)b_lblksz,ptar);
#endif
		p_bd[loc].rcu_buf[cr].size += b_size2;
		((BDCB far *)b_pbdcbintr)->dataLength = b_rsize;
	   PUT_BDCB(address,port,9,b_pbdcbintr);

	}
 	if ((p_bd[loc].rcu_buf[cr].size == (ULONG)cfg.bd_single)) {
		if ((b_p_eod != TRUE) && (p_bd[loc].bdstop != TRUE)) {	/* Stopped by user */
		/* Reached end of local buffer -> means switch buffers or end_of_data */
			/* switch I/O buffers */
			if (newbuf((BYTE)loc,ANT_BDREAD) == FALSE) {
				return(-1);
			}

	 	}
	}
	return(E_SUCC);
}
