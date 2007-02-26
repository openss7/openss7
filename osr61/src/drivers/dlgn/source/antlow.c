/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antlow.c
 * Description                  : antares low level
 *
 *
 **********************************************************************/

#ifdef WNT_ANT
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stream.h>
#include <stropts.h>
#else
#ifdef LINUX
#include "dlgclinux.h"
#define _SCO_
#else
#include <sys/types.h>
#include <sys/stream.h>
#endif /* LINUX */
#endif

#include "dlgcos.h"
#include "include.h"

#include "hardexp.h"
#include "hardware.h"
#include "anprot.h"

#include "protocol.h"

#include "extern.h" 

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
extern BYTE in_mutex;
#endif

#ifdef LINUX
#ifdef LFS
extern spinlock_t inthw_lock;
#else
extern lis_spin_lock_t inthw_lock;
#endif
#endif /* LINUX */

extern SYS_INFO	sys;

/****************************************************************
 *			                                        *
 *     GLOBAL VARIABLES  + MACROS                               *             
 *				 				*
 *   (all global variables of antlow.c will get prefix l_)	*
 ***************************************************************/
/* get host->dsp control block macro */
#define GET_HDCB(address,port,data) \
        (A4_readData(port,address,(WORD)HDCBSIZE,data))

/* get dsp->host control block macro */
#define GET_DHCB(address,port,data) \
        (A4_readData(port,address,(WORD)DHCBSIZE,data))

/* writes host->dsp control block macro */
#ifdef _SCO_
#define PUT_HDCB(address,port,index,data) \
	A4_writeData(port,(address+index),1,(UINT32 far *)(&(data[index])))
#else
#define PUT_HDCB(address,port,index,data) \
	A4_writeData(port,(address+index),1,(UINT32 *)(&(data[index])))
#endif /* _SCO_ */
/* writes dsp->host control block macro */
#ifdef _SCO_
#define PUT_DHCB(address,port,index,data) \
	A4_writeData(port,(address+index),1,(UINT32 far *)(&(data[index])))
#else
#define PUT_DHCB(address,port,index,data) \
	A4_writeData(port,(address+index),1,(UINT32 *)(&(data[index])))
#endif /* _SCO_ */
/* get specific field from control block */
#ifdef _SCO_
#define GET_ICB(address,port,index,data) \
	A4_readData(port,(address+index),1,(UINT32 far *)(&(data[index])))
#else
#define GET_ICB(address,port,index,data) \
	A4_readData(port,(address+index),1,(UINT32 *)(&(data[index])))
#endif /* _SCO_ */

MHdr l_smessh;	/* global message header for sending message algorithm */
MHdr l_rmessh;	/* global message header for receiving message algorithm */
#ifdef _SCO_
UINT32 far *l_psmessh; 	/* pointer to l_smessh */
UINT32 far *l_prmessh; 	/* pointer to l_rmessh */
#else
UINT32 *l_psmessh; 	/* pointer to l_smessh */
UINT32 *l_prmessh; 	/* pointer to l_rmessh */
#endif /* _SCO_ */

HDCB l_hdcbdata; /* sturcture for get/set hdcb information from dsp */
DHCB l_dhcbdata; /* structure for get/set dhcb information from dsp */
#ifdef _SCO_
UINT32 far *l_phdcbdata; /* pointer tp l_hdcbdata */
UINT32 far *l_pdhcbdata; /* pointer tp l_dhcbdata */
#else
UINT32 *l_phdcbdata; /* pointer tp l_hdcbdata */
UINT32 *l_pdhcbdata; /* pointer tp l_dhcbdata */
#endif /* _SCO_ */
/****************************************************************
*        NAME : init_msg_address
* DESCRIPTION : Initiates message pointers (called by init_protocol() in
*               drvprot.c).
*       INPUT : void
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
VOID init_msg_address(VOID)
{
#ifdef _SCO_
    l_psmessh = (UINT32 far *)ant_forceaddr((CHAR *)(&(l_smessh)));
    l_prmessh = (UINT32 far *)ant_forceaddr((CHAR *)(&(l_rmessh)));
    l_phdcbdata = (UINT32 far *)ant_forceaddr((CHAR *)(&(l_hdcbdata)));
    l_pdhcbdata = (UINT32 far *)ant_forceaddr((CHAR *)(&(l_dhcbdata)));
#else
    l_psmessh = (UINT32 *)ant_forceaddr((CHAR *)(&(l_smessh)));
    l_prmessh = (UINT32 *)ant_forceaddr((CHAR *)(&(l_rmessh)));
    l_phdcbdata = (UINT32 *)ant_forceaddr((CHAR *)(&(l_hdcbdata)));
    l_pdhcbdata = (UINT32 *)ant_forceaddr((CHAR *)(&(l_dhcbdata)));
#endif /* _SCO_ */
    return;
}

/****************************************************************
*        NAME : ask_token
* DESCRIPTION : Ask for token from board
*       INPUT : Board's port
*      OUTPUT : 
*     RETURNS : TRUE / FALSE
*    CAUTIONS : 
****************************************************************/
SHORT ask_token(WORD port)
{
    WORD lport;
    LONG ctime;

#ifdef VME_ANT
    lport = port;
#else
   /* Get port for token request */
    lport = A4_CONTROL_PORT(port);
#endif

    A4_requestToken(port); /* Ask for hardware token */

    ctime = DspCall;
    while (ctime > 0) { 
        ctime--;
       /* Checking if token request si O.K */
#ifdef _SCO_
	if (A4_TOKEN_GRANT(((WORD)ainpw(lport)))) {

#else
        if (A4_TOKEN_GRANT(((WORD) A4_readControlPort(lport)))) {
#endif /* _SCO_ */
            return(TRUE);      /* request token from dsp */
	}
    }

   /* Token is not available  */
    A4_releaseToken(port);
    return(FALSE);
}

/****************************************************************
*        NAME : send_message
* DESCRIPTION : Sends Message to board
*       INPUT : board number,message buffer,message length, message id
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT send_message(SHORT prcu, SHORT board, RCU destrcu,
		   SEND_MSG far *msg, SYNC_REPLY far *reply,
		   ULONG msgtype, BYTE acktype)
#else
SHORT send_message(SHORT prcu, SHORT board, RCU destrcu,
		   SEND_MSG *msg, SYNC_REPLY *reply,
		   ULONG msgtype, BYTE acktype)
#endif /* _SCO_ */
{
    WORD sblksz;
    UINT32 addr;	/* address variable */
#ifdef _SCO_
    UINT32 far *setparamp;	/* pointer to message */
#else
    UINT32 *setparamp;	/* pointer to message */
#endif /* _SCO_ */
    UINT32 address;	/* control block address */
    BYTE dsp;		/* number of dsp involved */
    WORD port;		/* board access port    */
    SHORT err;		/* replay error */
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

    LONG timecount;

   /* initiation of local variables */
    dsp = destrcu.dsp;
    port = p_board[board].port;
    address = p_board[board].phdcb+(dsp*HDCBSIZE);

    timecount = DspCall;
    while (timecount > 0) { 
        timecount--;
	GET_ICB(address,port,0,l_phdcbdata);
       /* Got DSP response */
#ifdef _SCO_
	if (((HDCB far *)l_phdcbdata)->TF) {
#else
	if (((HDCB *)l_phdcbdata)->TF) {
#endif /* _SCO_ */
            break;
        }
    }

#ifdef _SCO_
    if (!((HDCB far *)l_phdcbdata)->TF) {
#else
    if (!((HDCB *)l_phdcbdata)->TF) {
#endif /* _SCO_ */
	return(E_DSPNORESPONSE);  /* no response from dsp */
    }

    GET_ICB(address,port,4,l_phdcbdata);

   /* DSP buffer size is too short */
#ifdef _SCO_
    if (((HDCB far *)l_phdcbdata)->messageSize < (ULONG)msg->msgsize) {	
#else
    if (((HDCB *)l_phdcbdata)->messageSize < (ULONG)msg->msgsize) {	
#endif /* _SCO_ */

       /* Not enough place allocated by DSP - asking for bigger buffer*/
#ifdef _SCO_
        ((HDCB far *)l_phdcbdata)->dataLength = msg->msgsize;
#else
        ((HDCB *)l_phdcbdata)->dataLength = msg->msgsize;
#endif /* _SCO_ */
	PUT_HDCB(address,port,5,l_phdcbdata);
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
        mutex_enter(&inthw_lock);
        in_mutex = TRUE;
#elif LINUX
	DLGCSPINLOCK(inthw_lock, oldspl);
#else
        DLGCSPLSTR(oldspl);
#endif
#endif
	if (!(ask_token(port))) {
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
       /*
        * Ask kernel for more allocation in protocol determined to be 
	* between driver to DSP
        */ 
	GET_ICB(address,port,0,l_phdcbdata);
#ifdef _SCO_
	((HDCB far *)l_phdcbdata)->SF = 1;	 /* set sf */
        ((HDCB far *)l_phdcbdata)->TF = 0;
#else
	((HDCB *)l_phdcbdata)->SF = 1;	 /* set sf */
        ((HDCB *)l_phdcbdata)->TF = 0;
#endif /* _SCO_ */
	PUT_HDCB(address,port,0,l_phdcbdata);

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
   	A4_interruptDSP(port,dsp);   /* interrupt dsp */

       /* Waiting for DSP answer */
	timecount = DspCall;
	while (timecount > 0) { 
	    timecount--;
	    GET_ICB(address,port,0,l_phdcbdata);
	   /* We got DSP answer */
#ifdef _SCO_
	    if (((HDCB far *)l_phdcbdata)->TF == 1) {
#else
	    if (((HDCB *)l_phdcbdata)->TF == 1) {
#endif /* _SCO_ */
		break;
	    }
	}

       /* No answer from DSP */
	if (((HDCB far *)l_phdcbdata)->TF == 0) {
	    return(E_DSPNORESPONSE);  /* no response from dsp */
	}

       /* Not enough size offered by dsp */
#ifdef _SCO_
	if ((((HDCB far *)l_phdcbdata)->NB) || (((HDCB far *)l_phdcbdata)->SF == 1)) { 
#else
	if ((((HDCB *)l_phdcbdata)->NB) || (((HDCB *)l_phdcbdata)->SF == 1)) { 
#endif /* _SCO_ */

	    return(E_NOSIZE);
	}
    }

    GET_HDCB(address,port,l_phdcbdata);

   /* verify message length */
#ifdef _SCO_
    if (((HDCB far *)l_phdcbdata)->messageSize < (ULONG)msg->msgsize) {
#else
    if (((HDCB *)l_phdcbdata)->messageSize < (ULONG)msg->msgsize) {
#endif /* _SCO_ */
	/* not enough place allocated */
	 return(E_DSPSIZEERROR);
    }

   /* fills message header */
#ifdef _SCO_
    ((MHdr far *)l_psmessh)->messageType = msgtype;

    ((MHdr far *)l_psmessh)->source.rclass = p_orcu[prcu].Class;
    ((MHdr far *)l_psmessh)->source.board = p_orcu[prcu].Board;
    ((MHdr far *)l_psmessh)->source.dsp =   p_orcu[prcu].Dsp;
    ((MHdr far *)l_psmessh)->source.unit =  p_orcu[prcu].Unit;

    ((MHdr far *)l_psmessh)->destination = destrcu;
    ((MHdr far *)l_psmessh)->messageId = msg->msgid;
    ((MHdr far *)l_psmessh)->dataLength = (ULONG)msg->msgsize;
#else
    ((MHdr *)l_psmessh)->messageType = msgtype;

    ((MHdr *)l_psmessh)->source.rclass = p_orcu[prcu].Class;
    ((MHdr *)l_psmessh)->source.board = p_orcu[prcu].Board;
    ((MHdr *)l_psmessh)->source.dsp =   p_orcu[prcu].Dsp;
    ((MHdr *)l_psmessh)->source.unit =  p_orcu[prcu].Unit;

    ((MHdr *)l_psmessh)->destination = destrcu;
    ((MHdr *)l_psmessh)->messageId = msg->msgid;
    ((MHdr *)l_psmessh)->dataLength = (ULONG)msg->msgsize;
#endif /* _SCO_ */

#ifdef BIG_ANT 
   /* 
    *  source and destination fields are of the type RCU, which
    *  consists four byte fields.  Since the driver performs wholesale
    *  byteswaping.  To nullify the byteswapping done by the driver,
    *  we have to byteswap them here.
    */
    p_an_byteswap((char *)&((MHdr far *)l_psmessh)->source, 1, 4);
    p_an_byteswap((char *)&((MHdr far *)l_psmessh)->destination, 1, 4);
#endif
   /* send message + data to dsp */
#ifdef _SCO_
    addr = (UINT32)((HDCB far *)l_phdcbdata)->pHeader;
#else
    addr = (UINT32)((HDCB *)l_phdcbdata)->pHeader;
#endif /* _SCO_ */
    A4_writeData(port,addr,(WORD)MHDRSIZE,l_psmessh);

#ifdef _SCO_
    addr = (UINT32)((HDCB far *)l_phdcbdata)->pMessage;
#else
    addr = (UINT32)((HDCB *)l_phdcbdata)->pMessage;
#endif /* _SCO_ */
    sblksz = (WORD)BTOWORD(msg->msgsize);
#ifdef _SCO_
    setparamp = (UINT32 far *)ant_forceaddr((CHAR huge *)(&(msg->pmessage[0])));
#else
    setparamp = (UINT32 *)ant_forceaddr((CHAR huge *)(&(msg->pmessage[0])));
#endif /* _SCO_ */
	
#ifdef BIG_ANT 
    switch  (msgtype) {
        case STD_MESSAGE:
            A4_writeData(port|_A4_NO_SWAP,addr,(WORD)sblksz,setparamp);
        break;
        case CONTROL_MESSAGE:
            p_an_byteswap((char *)&setparamp[1], 1,4);
            A4_writeData(port,addr,(WORD)sblksz,setparamp);
        break;
    }
#else
    A4_writeData(port,addr,(WORD)sblksz,setparamp);
#endif
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    mutex_enter(&inthw_lock);
    in_mutex = TRUE;
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
   /* Interrupt dsp with send parameters and waiting for results */
    if (!(ask_token(port))) {
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
   /* Write to control block message size */
#ifdef _SCO_
    ((HDCB far *)l_phdcbdata)->dataLength = msg->msgsize;
#else
    ((HDCB *)l_phdcbdata)->dataLength = msg->msgsize;
#endif /* _SCO_ */

    PUT_HDCB(address,port,5,l_phdcbdata);

    GET_ICB(address,port,0,l_phdcbdata);
#ifdef _SCO_
    ((HDCB *)l_phdcbdata)->TF = 0;
    ((HDCB *)l_phdcbdata)->SF = 0;	         /* set sf */
    ((HDCB *)l_phdcbdata)->Syncres = acktype;	 /* set sf */
#else
    ((HDCB *)l_phdcbdata)->TF = 0;
    ((HDCB *)l_phdcbdata)->SF = 0;	         /* set sf */
    ((HDCB *)l_phdcbdata)->Syncres = acktype;	 /* set sf */
#endif /* _SCO_ */

    PUT_HDCB(address,port,0,l_phdcbdata);

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
    A4_interruptDSP(port,dsp);   /* interrupt dsp */

   /* Waiting for DSP acknowledge for message sent */
    timecount = DspCall;
    while (timecount > 0) { 
	timecount--;
	GET_HDCB(address,port,l_phdcbdata);
#ifdef _SCO_
	if (((HDCB far *)l_phdcbdata)->TF == 1) { /* DO we have confirmation ?? */
#else
	if (((HDCB *)l_phdcbdata)->TF == 1) { /* DO we have confirmation ?? */
#endif /* _SCO_ */
	    break;	/* YES */
	}
    }

    err = E_SUCC;

#ifdef _SCO_
    if (((HDCB far *)l_phdcbdata)->TF == 0) {    /* Did we have confirmation ?? */
#else
    if (((HDCB *)l_phdcbdata)->TF == 0) {    /* Did we have confirmation ?? */
#endif /* _SCO_ */
	return(E_DSPNOCONFIRM);	/* no, so error out */
    }

#ifdef _SCO_
    if (((HDCB *)l_phdcbdata)->EF == 1) {    /* error found */
#else
    if (((HDCB *)l_phdcbdata)->EF == 1) {    /* error found */
#endif /* _SCO_ */
	if (((HDCB far *)l_phdcbdata)->Error != 0) {	 
	    err = (SHORT)((HDCB far *)l_phdcbdata)->Error;
	}
	else {
	    return(E_ERRORFOUND);
	}
    }
    if (acktype != REPSYNC) {
	/* Acknoledge is not expected */
	 return(err);
    }

   /* Checking whether reply exists */
    GET_ICB(address,port,7,l_phdcbdata);
#ifdef _SCO_
    if (((HDCB far *)l_phdcbdata)->RepLength != 0) {
#else
    if (((HDCB *)l_phdcbdata)->RepLength != 0) {
#endif /* _SCO_ */

       /* Checking reply length */
	if (((HDCB far *)l_phdcbdata)->RepLength <= (ULONG)reply->smsgsize) { 
  	    reply->smsgsize = (SHORT)((HDCB far *)l_phdcbdata)->RepLength;
	}
	else {
	    err = E_MSGTRUNC;
	}

       /* Get responsep */
	GET_ICB(address,port,6,l_phdcbdata);
#if 0
       /* Zeroing size */
	((HDCB far *)l_phdcbdata)->RepLength = 0;
	PUT_HDCB(address,port,7,l_phdcbdata);
#endif
	A4_readData(port,
	            ((HDCB far *)l_phdcbdata)->ResponseP,
		    (WORD)BTOWORD(reply->smsgsize),
		    (UINT32 far *)(&(reply->smessage[0])));
    } 
    else {
   	reply->smsgsize = 0;
    };

   /* Send message completed successfully */
    return(err);
}

/****************************************************************
*        NAME : get_message MACROS
****************************************************************/
/* Macro which is end_of get_message protocol */
#define END_GET_MESSAGE(rcu,address,port,dsp) \
	if (!(ask_token(port))) {	                \
		send_error_msg(p_board[board].board_id,rcu,dsp,(SHORT)E_DSPNOTOKEN,(SHORT)__LINE__); \
		return(-1);												 \
	}																 \
	GET_ICB(address,port,0,l_pdhcbdata);				 \
	((DHCB far *)l_pdhcbdata)->TF = 0;										 \
	PUT_DHCB(address,port,0,l_pdhcbdata);				 \
	A4_releaseToken(port);									 \
   A4_interruptDSP(port,dsp);                       \
	return(0);

/* Macro which finds if to RCUs are identical */
#define RCU_EQUAL(rcu1,rcu2)								 \
	((rcu1.rclass == rcu2.rclass) && (rcu1.board == rcu2.board) \
	 && (rcu1.dsp == rcu2.dsp)	&& (rcu1.unit == rcu2.unit))


/****************************************************************
*        NAME : get_message
* DESCRIPTION : function called by interrupt to check whetehr to
*               move message dsp -> host
*       INPUT : void
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
SHORT far get_message(BYTE board,BYTE dsp) 
{
	
	/* get message variables */
	WORD port;
	WORD blksz,l_lblksz;
	UINT32 addr;
	UINT32 address;
	SHORT pi,un_s;
	SHORT l_local_error,l_rcf;

	/* Get Message Control block */
	port = p_board[board].port;
	address = p_board[board].pdhcb+(dsp*DHCBSIZE);
	GET_DHCB(address,port,l_pdhcbdata);


   /* no message available for now */
	if (((DHCB far *)l_pdhcbdata)->TF == 0) {
		return(E_SUCC);	  
	}

	l_local_error = E_SUCC;
	l_rcf = E_SUCC;



	/* Driver not started; notify DSP that message is not delivered */
   if (sys.an_state != Active) {

		((DHCB far *)l_pdhcbdata)->Error = E_NODELIVER;	/* notify the DSP */
		PUT_DHCB(address,port,5,l_pdhcbdata);
		if (!(ask_token(port))) {
			send_error_msg(p_board[board].board_id,(SHORT)NO_RCU,0,(SHORT)E_DSPNOTOKEN,(SHORT)__LINE__);

			return(-1);
		}
		((DHCB far *)l_pdhcbdata)->TF = 0;
		PUT_DHCB(address,port,0,l_pdhcbdata);
		A4_releaseToken(port);	/* release token */
   	A4_interruptDSP(port,dsp);   /* interrupt dsp */
		return(E_SUCC);
	}

	/* Immediate acknowledge from driver is not possible */
	if (((DHCB far *)l_pdhcbdata)->Syncres == REPSYNC) {
		((DHCB far *)l_pdhcbdata)->Error = E_IMMEDIATEACK;	/* notify the DSP */
		/* Error message to user */
		send_error_msg(p_board[board].board_id,(SHORT)NO_RCU,0,(SHORT)E_IMMEDIATEACK,(SHORT)__LINE__);

		PUT_DHCB(address,port,5,l_pdhcbdata);
		if (!(ask_token(port))) {
			send_error_msg(p_board[board].board_id,(SHORT)NO_RCU,0,(SHORT)E_DSPNOTOKEN,(SHORT)__LINE__);

			return(-1);
		}
		((DHCB far *)l_pdhcbdata)->TF = 0;
		PUT_DHCB(address,port,0,l_pdhcbdata);
		A4_releaseToken(port);	/* release token */
   	A4_interruptDSP(port,dsp);   /* interrupt dsp */
		return(E_SUCC);
	}


	/* Reads message header */
	addr = (UINT32)((DHCB far *)l_pdhcbdata)->pHeader;
	A4_readData(port,addr,(WORD)MHDRSIZE,l_prmessh);
#ifdef BIG_ANT
        /* source and destination fields are of the type RCU, which
           consists four byte fields.  Since the driver performs wholesale
           byteswaping.  To nullify the byteswapping done by the driver,
           we have to byteswap them here.
         */
	p_an_byteswap((char *)&((MHdr far *)l_prmessh)->source, 1,4);
	p_an_byteswap((char *)&((MHdr far *)l_prmessh)->destination, 1,4);
#endif
	blksz = (WORD)((MHdr far *)l_prmessh)->dataLength;
	l_lblksz = (WORD)BTOWORD(blksz);
	if (blksz > (WORD)cfg.m_message) {
		 /* Message is too long */
		l_local_error = E_RCV_TOO_LONG;  	/* mark the error */
		blksz = cfg.m_message;			/* Truncate the message */
		l_lblksz = (WORD)BTOWORD(blksz);
	}

 	addr = (UINT32)((DHCB far *)l_pdhcbdata)->pMessage;



	/* Unsolicited events: message is duplicated and is sent to 
	   Every open RCU for unsolicited events   */
	if (((MHdr far *)l_prmessh)->messageType == UNS_EVENT)
	{
		un_s = -1;
		/* Read message from DSP */
#ifdef BIG_ANT
		A4_readData(port|_A4_NO_SWAP,addr,(WORD)l_lblksz,(UINT32 far *)(&(p_message[0])));
#else
		A4_readData(port,addr,(WORD)l_lblksz,(UINT32 far *)(&(p_message[0])));
#endif
		/* Searcing all opened RCUs */
		for (pi = 0; pi<cfg.maxopenedrcu; pi++) {
	 		if ((p_orcu[pi].index != -1) && 
				 (p_orcu[pi].solo == ANT_UNS)	&&
				 /* ALL RCUs must be the same physical RCU */ 
				 RCU_EQUAL(p_rcu[p_orcu[pi].index].l_rcu.grcu,((MHdr far *)l_prmessh)->source)) 
			{
				un_s = E_SUCC;

				/* Send message to RCU found fit for unsolicited message */
				l_rcf = send_dsp_msg(TRUE,p_board[board].board_id,
												blksz,
											  (WORD)((MHdr far *)l_prmessh)->messageType,
											  (WORD)((MHdr far *)l_prmessh)->messageId,
											  (ADDR *)(&(p_message[0])),
											  pi,
											  l_local_error,
											  ((MHdr far *)l_prmessh)->source);

				if (l_rcf != E_SUCC) { /* Error sending message to driver */
					break;
				}
	 		}
	 	}
		if ((l_rcf != E_SUCC) || (l_local_error != E_SUCC) || (un_s == -1)) {
			/* Error sending message to driver - notify DSP */
			((DHCB far *)l_pdhcbdata)->Error = E_NODELIVER;						 
			PUT_DHCB(address,port,5,l_pdhcbdata);				 
		}
		END_GET_MESSAGE(NO_RCU,address,port,dsp);

	}

	/* Standard event */

	/* bad parameters in destination rcu */

	pi = ((MHdr far *)l_prmessh)->destination.unit;
	if ((pi < 0) || (pi >= cfg.maxopenedrcu)) {	/* Illegal device number */
		l_rcf = E_UNITNUM;
	}
	else 
		if ((p_orcu[pi].index == -1)	|| (p_rcu[p_orcu[pi].index].opened == 0) ||
			(RCU_EQUAL(p_rcu[p_orcu[pi].index].l_rcu.grcu,((MHdr far *)l_prmessh)->source) == FALSE))
		{
			l_rcf = E_UNITNUM;
		}
		else 
			if (((MHdr far *)l_prmessh)->destination.rclass != p_orcu[pi].Class) {
				/* Illegal RCU.rclass value */
				l_rcf = E_CLASSNUM;
			}
			else {
				if (((MHdr far *)l_prmessh)->destination.board != p_orcu[pi].Board) {
				/* Illegal RCU.board value */
				l_rcf = E_BOARDNUM;
			}
			else {
				if (((MHdr far *)l_prmessh)->destination.dsp != p_orcu[pi].Dsp) {
				/* Illegal RCU.DSP value */
				l_rcf = E_DSPNUM;
			}
			/* Message parameters O.K */
			else {
				/* Reading message:
				   We use p_message for the case of unsolicited events 
				*/
#ifdef BIG_ANT
				A4_readData(port|_A4_NO_SWAP,addr,(WORD)l_lblksz,(UINT32 far *)(&(p_message[0])));
#else
				A4_readData(port,addr,(WORD)l_lblksz,(UINT32 far *)(&(p_message[0])));
#endif
			 	/* Insert message in local queue */
				/* standard events */
				if (((MHdr far *)l_prmessh)->messageType == STD_MESSAGE) {
			  			l_local_error = send_dsp_msg(TRUE,p_board[board].board_id,
			         									  blksz,
			  		  										  (WORD)((MHdr far *)l_prmessh)->messageType,
			  		  										  (WORD)((MHdr far *)l_prmessh)->messageId,
			  		  										  (ADDR *)(&(p_message[0])),
			  		  										  ((MHdr far *)l_prmessh)->destination.unit,
			  		  										  l_local_error,
			  		  										  ((MHdr far *)l_prmessh)->source);
			  	}
			}
		}
	}

	if ((l_rcf != E_SUCC) || (l_local_error != E_SUCC)) {
		/* Error sending message to driver - notify DSP */
		((DHCB far *)l_pdhcbdata)->Error = E_NODELIVER;						 
		PUT_DHCB(address,port,5,l_pdhcbdata);				 
		if (l_rcf != E_SUCC) {									 
			send_error_msg(p_board[board].board_id,((MHdr far *)l_prmessh)->destination.unit,dsp,l_rcf,__LINE__);
		}
	}
	/* End of get_message protocol */
	END_GET_MESSAGE(((MHdr far *)l_prmessh)->destination.unit,address,port,dsp);
}

/****************************************************************
*        NAME : put_queue
* DESCRIPTION : Gets message from user and simulates get_message protocol.
*       INPUT : RCU handler,msgtype,msgid,message size,message buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code;
*    CAUTIONS : put_queue is not an interrupt functions so part of it 
*               are protected by CLI / STI.    
****************************************************************/
SHORT put_queue(WORD orcu,WORD msgtype,WORD msgid,WORD l_pblksz,ADDR *pmess)
{
   /* Push queue variables */
    SHORT pi;
    SHORT l_plocal_error,l_prcf;

    l_plocal_error = E_SUCC;
    l_prcf = E_SUCC;

    if (l_pblksz > (WORD)cfg.m_message) {
        l_plocal_error = E_RCV_TOO_LONG;  /* mark the error */
        l_pblksz = (WORD) cfg.m_message;  /* Truncate the message */
    }

   /* Unsolicited event */
    if (msgtype == UNS_EVENT) {
        for (pi = 0; pi<cfg.maxopenedrcu; pi++) {
	    if ((p_orcu[pi].index != -1) && 
		(p_orcu[pi].solo == ANT_UNS) &&
		(p_orcu[pi].index == p_orcu[orcu].index)) {
	       /* Rcu found matched for event */

	       /* Sending message to driver */
		l_prcf = send_dsp_msg((BYTE)FALSE,(SHORT)NO_BOARD,l_pblksz,
 				      msgtype,
 				      msgid,
 				      (ADDR *)(&(pmess[0])),
				      pi,
				      l_plocal_error,
				      p_rcu[p_orcu[orcu].index].l_rcu.grcu);
		if (l_prcf != E_SUCC) {
		    break;
		}
	    }
        }
    }
    else {
       /* standard event */
        if (msgtype == STD_MESSAGE) {
           /* Sending message to driver */
	    send_dsp_msg((BYTE)FALSE,(SHORT)NO_BOARD,l_pblksz,
		         msgtype,
		         msgid,
		         (ADDR *)(&(pmess[0])),
		         orcu,
		         l_plocal_error,
		         p_rcu[p_orcu[orcu].index].l_rcu.grcu);
	}
    }
    return(l_plocal_error);
}

/****************************************************************
*        NAME : raw_cmd
* DESCRIPTION : Gets information from board, Used by debug utilities
*               to get board parameters in real_time, and not through
*               direct I/O to board port.
*       INPUT : Board port,Command type, I/O address, Output value,
*               Data size, Data buffer 
*      OUTPUT : 
*     RETURNS : Value / -1 if successless request 
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
UINT32 raw_cmd(WORD port,UINT32 type,UINT32 address,UINT32 value,
               UINT32 count,UINT32 far *buffer)
#else
UINT32 raw_cmd(WORD port,UINT32 type,UINT32 address,UINT32 value,
               UINT32 count,UINT32 *buffer)
#endif /* _SCO_ */
{

#ifdef _SCO_
        ADDR *saddr;
#endif /* _SCO_ */
   /* Type includes kind of request */
    switch(type) {
	case ANGETADDRESS:	  /* Get current address in DSP memory */
            return(A4_getAddress(port));

	case ANSETADDRESS:	  /* Set address in DSP memory */
	    A4_setAddress(port,address);
	    return(value);

	case ANREADCONTROL:	  /* Read control port current value */ 
	    return((ULONG)A4_readControlPort(port));

	case ANWRITECONTROL:	  /* Write control port */ 
	    A4_writeControlPort(port,(WORD)value);
	    return(value);
#ifndef VME_ANT		  
	case ANREAD16BITS:        /* Read WORD from DSP memory */
	    return((ULONG)A4_read16Bits(port));

	case ANWRITE16BITS:	  /* Write WORD to DSP memory */
	    A4_write16Bits(port,(WORD)value);
	    return(value);
#endif
	case ANREADWORD:          /* Read LONG (UINT32) from DSP memory */
	    return((ULONG)A4_readWord(port));

	case ANWRITEWORD:	  /* Write LONG (UINT32) to DSP memory */
	    A4_writeWord(port,value);
	    return(value);

	case ANREADDATA:	  /* Read data from DSP */
#ifdef _SCO_
            saddr = (BYTE *) ant_forceaddr((CHAR huge *)(buffer));
            A4_readData(port,address,(WORD)count,(UINT32 far *)saddr);
#else
	    A4_readData(port,address,(WORD)count,(UINT32 *)buffer);
#endif /* _SCO_ */
	    return(value);

	case ANWRITEDATA:	 /* Write data to DSP */
#ifdef _SCO_
            saddr = (BYTE *) ant_forceaddr((CHAR huge *)(buffer));
            A4_writeData(port,address,(WORD)count,(UINT32 far *)saddr);
#else
	    A4_writeData(port,address,(WORD)count,(UINT32 *)buffer);
#endif /* _SCO_ */
	    return(value);
#ifdef RTDL_ANT 
	case 0xfedcba98:
	   /* used at begining of antload to prevent interrupt function */
	   /* from executing on downloaded board */

	   /* addede interrupt protection */
	    p_board[value].ready = BOARD_EXIST;
	    return E_SUCC;
#endif
	default:
	    return(-1);
    }
    return(E_SUCC);
}
