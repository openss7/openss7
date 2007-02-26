/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : driver.c
 * Description                  : driver misc
 *
 *
 **********************************************************************/
 
#ifdef WNT_ANT
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stream.h>
#include <stropts.h>
#include <cmn_err.h>
#else
#ifdef LINUX
#include "dlgclinux.h"
#define _SCO_
#else
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/stream.h>
#ifndef AIX_ANT
#include <sys/cmn_err.h>
#else
#include "ant_aix.h" 
#endif
#include <sys/errno.h>
#endif /* LINUX */
#endif

#include "dlgcos.h"
#include "gndefs.h"
#include "gndrv.h"

#include "include.h"
#include "intrface.h"
#include "extvar.h"
#include "drvprot.h"
#include "protocol.h"
#include "extern.h"

#include "andrv.h"

#ifdef WNT_ANT
extern VOID cmn_err(ULONG,...);
#endif

extern SYS_INFO	sys;
#ifdef _SCO_
extern void *AllocMem(LONG);
extern void  FreeMem(void*,LONG);

SHORT an_open(ADDR *,mblk_t *);
SHORT an_close(ADDR *);
SHORT an_send_msg(ADDR *, mblk_t *);
SHORT an_put_queue(ADDR *, mblk_t *);
SHORT an_bd_open(ADDR *, mblk_t *);
SHORT an_bd_close(ADDR *);
SHORT an_bd_setuio(ADDR *, mblk_t *);
SHORT an_bd_getchan(ADDR *, mblk_t *);
SHORT an_bd_xplayf(ADDR *);
SHORT an_bd_xrecf(ADDR *, mblk_t *);
SHORT an_bd_stop(ADDR *);
SHORT an_raw_open(ADDR *);
SHORT an_raw_close(ADDR *);
SHORT an_raw_cmd(ADDR *,mblk_t *);
SHORT an_get_info(ADDR *,mblk_t *);
SHORT an_get_version(ADDR *,mblk_t *);
SHORT an_route(ADDR *);
SHORT an_routerxts(ADDR *);
SHORT an_routelist(ADDR *, mblk_t *);
SHORT an_mapslot(ADDR *, mblk_t *);
SHORT an_get_dongle(ADDR *, mblk_t *);
SHORT an_get_eeprom(ADDR *, mblk_t *);
SHORT an_getxmitslot(ADDR *, mblk_t *);
SHORT an_listen(ADDR *, mblk_t *);
SHORT an_unlisten(ADDR *);
SHORT an_assignxmitslot(ADDR *, mblk_t *);
SHORT an_unassignxmitslot(ADDR *);
SHORT an_setxmitslot(ADDR *, mblk_t *);
SHORT an_unsetxmitslot(ADDR *);
#else
SHORT an_open(ADDR *,BYTE *);
SHORT an_close(ADDR *);
SHORT an_send_msg(ADDR *, BYTE *);
SHORT an_put_queue(ADDR *, BYTE *);
SHORT an_bd_open(ADDR *, BYTE *);
SHORT an_bd_close(ADDR *);
SHORT an_bd_setuio(ADDR *, BYTE *);
SHORT an_bd_getchan(ADDR *, BYTE *);
SHORT an_bd_xplayf(ADDR *, BYTE *);
SHORT an_bd_xrecf(ADDR *, BYTE *);
SHORT an_bd_stop(ADDR *);
SHORT an_raw_open(ADDR *);
SHORT an_raw_close(ADDR *);
SHORT an_raw_cmd(ADDR *,BYTE *);
SHORT an_get_info(ADDR *,BYTE *);
SHORT an_get_version(ADDR *,BYTE *);
SHORT an_route(ADDR *);
SHORT an_routerxts(ADDR *);
SHORT an_routelist(ADDR *, BYTE *);
SHORT an_mapslot(ADDR *, BYTE *);
SHORT an_get_dongle(ADDR *, BYTE *);
SHORT an_get_eeprom(ADDR *, BYTE *);
SHORT an_getxmitslot(ADDR *, BYTE *);
SHORT an_listen(ADDR *, BYTE *);
SHORT an_unlisten(ADDR *);
SHORT an_assignxmitslot(ADDR *, BYTE *);
SHORT an_unassignxmitslot(ADDR *);
SHORT an_setxmitslot(ADDR *, BYTE *);
SHORT an_unsetxmitslot(ADDR *);
#endif /* _SCO_ */

/*******************************************************************
 *       NAME : drvrcmnd(fcn,blksz,parmp,retregsp)
 *DESCRIPTION : Command handler entrypoSHORT (from IDDS).
 *      INPUT : fcn = Driver function number.
 *              blksz = Size of the parameter block in bytes.
 *              parmp = Pointer to the parameter block.
 *              dmp = Data message pointer.
 *     OUTPUT : None.
 *    RETURNS : pass/fail completion status of command.
 *   CAUTIONS : None.
 *******************************************************************/
#ifdef _SCO_
SHORT far drvrcmnd(WORD fcn,WORD blksz,ADDR far *parmp,mblk_t *retregsp)
#else
 SHORT drvrcmnd(WORD fcn,WORD blksz,ADDR *parmp,BYTE *dmp)
#endif /* _SCO_ */
{
    WORD rc;

#ifdef _SCO_
    mblk_t *dmp = (mblk_t *) retregsp;
 
    SHORT far *bstate;
    SHORT far *lbstate;

#else
    SHORT *bstate;
    SHORT *lbstate;
#endif /* _SCO_ */

    if (fcn > EXMAXFCN) {
        return ((SHORT)(E_EXBADPRM));
    }

   /* no boards found */
    if (d_nboards == 0) {
	return(E_NOLEGALBOARD);
    }

   /* Debugging Functions allowed always */
    switch (fcn) {
       /****************************************************************
        *        NAME : AN_RAW_OPEN
        * DESCRIPTION : This function opens an antares raw board device
        *     RETURNS : Return code (device handle or failure error code)
        ****************************************************************/
	case F_ANRAWOPEN:
	    return(an_raw_open(parmp));

       /****************************************************************
        *        NAME : AN_RAW_CLOSE
        * DESCRIPTION : This function closes an open antares raw board device
        *     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANRAWCLOSE:
	    return(an_raw_close(parmp));

       /****************************************************************
        *       NAME : AN_RAW_CMD
        *DESCRIPTION : This function issues a debug command to the antares board
        *    RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANRAWCMD:
	    return(an_raw_cmd(parmp,dmp));

       /****************************************************************
        *        NAME : AN_GET_INFO
        * DESCRIPTION : This function returns system information in three 
        *               different structures: configuration_info, driver_info,
	*               system_info.      
	*     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANGETINFO:
	    return(an_get_info(parmp,dmp));

       /****************************************************************
        *        NAME : AN_GET_DONGLE
        * DESCRIPTION : This function returns dongle information. 
	*     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANDONGLE:
	    return(an_get_dongle(parmp,dmp));

       /****************************************************************
        *        NAME : AN_GET_EEPROM
        * DESCRIPTION : This function returns eeprom information. 
	*     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANEEPROM:
	    return(an_get_eeprom(parmp,dmp));

       /********************************************************************
 	*        NAME : AN_GET_VERSION
 	* DESCRIPTION : Get ANTARES device driver version number.
	*     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANGETVER:
	    return(an_get_version(parmp,dmp));

	default:
        break;
    }

   /* Q. Is the system already active? */
    if (sys.an_state != Active) {

       /* Functions that may be performed prior to system start */
        switch(fcn) {

           /*************************************************************
            *        NAME : EXSTART
            * DESCRIPTION : This function starts the Example system.
            *     RETURNS : Return code.
            *************************************************************/
            case F_EXSTART:                         
#ifdef _SCO_
                bstate =  (SHORT *)(&(dmp->b_rptr[0]));
                lbstate = (SHORT *)(&(dmp->b_rptr[2]));
#else
	        bstate =  (SHORT *) ((int) dmp + 0);
	        lbstate = (SHORT *) ((int) dmp + 2);
#endif /* _SCO_ */

               /* Gets information from boards */
                rc = p_an_start(bstate,lbstate);

                return(rc);

           /*************************************************************
            *       NAME : Bad Function.
            *DESCRIPTION : Returns a bad function return code.
            *    RETURNS : Return code.
            *************************************************************/
            default:
                return((SHORT)(E_EXSNACT));
        }
    }

   /* Functions allowable after system start */
    switch (fcn) {

       /****************************************************************
        *        NAME : AN_OPEN
        * DESCRIPTION : This function opens Available rcu
        *     RETURNS : Return code (channel number or error code).
        ****************************************************************/
  	case F_ANOPEN:
	    return(an_open(parmp,dmp));	

       /****************************************************************
        *        NAME : AN_CLOSE
        * DESCRIPTION : This function closes rcu
        *    RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
  	case F_ANCLOSE:
	    return(an_close(parmp));	

       /****************************************************************
        *        NAME : AN_SEND_MSG
        * DESCRIPTION : This function sends message from host to dsp
        *     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANSENDMSG:
	    return(an_send_msg(parmp,dmp));

       /****************************************************************
        *        NAME : AN_PUT_QUEUE
        * DESCRIPTION : This function simulate message sent from kernel
                        acting in same protocol like get_message;
        *     RETURNS : Return code (E_SUCC or failure error code)
        ****************************************************************/
	case F_ANPUTQUEUE:
	    return(an_put_queue(parmp,dmp));

       /****************************************************************
        *        NAME : AN_BD_OPEN
        * DESCRIPTION : This function opens an antares bulk data transfer stream
        *    RETURNS : Return code: >= 0 = handle; <0 failure error code
        ****************************************************************/
	case F_ANBDOPEN:
	    return(an_bd_open(parmp,dmp));

       /****************************************************************
        *       NAME : AN_BD_CLOSE
        *DESCRIPTION : This function closes an antares bulk data transfer stream
        *    RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANBDCLOSE:
	    return(an_bd_close(parmp));

       /****************************************************************
        *        NAME : AN_BD_SETUIO
        * DESCRIPTION : This function seer I/O functions for open
                        bulk data stream
        *    RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANBDSETUIO:
            return(an_bd_setuio(parmp,dmp));

       /****************************************************************
        *        NAME : AN_BD_GETCHAN
        * DESCRIPTION : This function seer I/O functions for open
                        bulk data stream
        *     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANBDGETCHAN:
            return(an_bd_getchan(parmp,dmp));

       /****************************************************************
        *        NAME : AN_XPLAYF
        * DESCRIPTION : This function sends files through an antares 
                        bulk data transfer stream
        *    RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANXPLAYF:
#ifdef _SCO_
	    return(an_bd_xplayf(parmp));
#else
	    return(an_bd_xplayf(parmp,dmp));
#endif /* _SCO_ */

       /****************************************************************
        *        NAME : AN_XRECF
        * DESCRIPTION : This function gets files through an antares
                        bulk data transfer stream
        *    RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANXRECF:
	    return(an_bd_xrecf(parmp,dmp));

       /****************************************************************
        *        NAME : AN_BD_STOP
        * DESCRIPTION : This function stops an antares bulk data protocol
        *     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
        case F_ANBDSTOP:
            return(an_bd_stop(parmp));

       /****************************************************************
        *        NAME : AN_ROUTE
        * DESCRIPTION : This function assigns time slots to RCU.
	*     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANROUTE:
	    return(an_route(parmp));

       /****************************************************************
        *        NAME : AN_ROUTERXTS
        * DESCRIPTION : This function assigns individual time slots to RCU.
        *     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANROUTERXTS:
            return(an_routerxts(parmp));

       /****************************************************************
        *        NAME : AN_ROUTELIST
        * DESCRIPTION : This function assigns a list of individual time 
                        slots to RCU.
        *     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANROUTELIST:
            return(an_routelist(parmp,dmp));

       /****************************************************************
        *        NAME : AN_MAPLIST
        * DESCRIPTION : This function maps external time slot to an antares
        *               internal time slots.
        *     RETURNS : Return code: = 0 if O.K; <0 failure error code
        ****************************************************************/
	case F_ANMAPSLOT:
            return(an_mapslot(parmp,dmp));

       /****************************************************************
        *        NAME : AN_GETXMITSLOT
        * DESCRIPTION : This function finds what time slots are assigned to
        *               specific RCU.
        *     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANGETXMITSLOT:
	    return(an_getxmitslot(parmp,dmp));

       /****************************************************************
        *        NAME : AN_LISTEN
        * DESCRIPTION : This function performs route assign operation to 
        *               receive time slot for specific RCU.
        *     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANLISTEN:
	    return(an_listen(parmp,dmp));

       /****************************************************************
        *        NAME : AN_UNLISTEN
        * DESCRIPTION : This function performs route deassign operation to
        *               receive time slot for specific RCU.
        *     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANUNLISTEN:
	    return(an_unlisten(parmp));

       /****************************************************************
        *        NAME : AN_ASSIGNXMITSLOT
        * DESCRIPTION : This function Connect timeslot transmit to SCBUS.
	*     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANASSIGNXMITSLOT:
		 return(an_assignxmitslot(parmp,dmp));

       /****************************************************************
        *        NAME : AN_UNASSIGNXMITSLOT
        * DESCRIPTION : This function DisConnect timeslot transmit from
	*               SCBUS.
	*     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANUNASSIGNXMITSLOT:
	    return(an_unassignxmitslot(parmp));

       /****************************************************************
        *        NAME : AN_ASSIGNXMITSLOT
        * DESCRIPTION : This function Connect timeslot transmit to External PEB.
	*    RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
        case F_ANSETXMITSLOT:
	    return(an_setxmitslot(parmp, dmp));

       /****************************************************************
        *        NAME : AN_UNASSIGNXMITSLOT
        * DESCRIPTION : This function DisConnect timeslot transmit from
	*               External PEB.
	*     RETURNS : Return code: = 0 if O.K; != 0 failure error code
        ****************************************************************/
	case F_ANUNSETXMITSLOT:
            return(an_unsetxmitslot(parmp));


       /****************************************************************
        *        NAME : EXSTOP
        * DESCRIPTION : This function deactivates the Antares system.
        *     RETURNS : Return code.
        ****************************************************************/
        case F_EXSTOP:
#ifdef RTDL_ANT /* p_stop with bstate and lbstate */
	    bstate =  (SHORT *) ((int) dmp + 0);
	    lbstate = (SHORT *) ((int) dmp + 2);

	    return(p_stop(bstate,lbstate));
#else
	    return(p_stop());
#endif
#ifdef RTDL_ANT
       /* now an_start is allowed even when system is active */
       /*************************************************************
        *       NAME : EXSTART
        *DESCRIPTION : This function starts the Example system.
        *    RETURNS : Return code.
        *************************************************************/
        case F_EXSTART:                         
	    bstate =  (SHORT *) ((int) dmp + 0);
	    lbstate = (SHORT *) ((int) dmp + 2);

           /* Gets information from boards */
            rc = p_an_start(bstate,lbstate);

            return(rc);
#endif
       /*************************************************************
        *        NAME : Bad Function.
        * DESCRIPTION : Returns a bad function return code.
        *     RETURNS : Return code.
        *************************************************************/
        default:
            return((SHORT)(E_EXSACT));
    }
}

/*********************************************************************
 * Driver Functions: Most of driver functions have the same format:  *
 *                   they decript parameters out of "param" buffer   *
 *                   according with the way parameters were          *
 *                   encripted in "antlib.c" functions.              *
 *                   Then they call the proper function of "protocol *
 *                   area" drvprot.c.                                *
 *	             So dirver.c is organized as pipe between antares*
 *                   application library and protocol functions.     *
 *********************************************************************/

/****************************************************************
*        NAME : anopen
* DESCRIPTION : Opens resource class unit;
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : handler on success ( > 0) otherwise error (<0))
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_open(ADDR *param, mblk_t *dmp)
#else
SHORT an_open(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
   /************************************************************ 
     rcu:  pointer to an RCU structure;
     flags: Set to ANT_UNS if this device is to recieve
            unsolicited event messages identifiers;
     proc_id: pointer to a process identifier structure.	
    *************************************************************/

    SHORT oflags;	 /* an_open parameters */

#ifdef _SCO_
    RCU  far *rcuid;
    PRC  far *process;
    LONG far *Attarray;
#else
    RCU  *rcuid;
    PRC  *process;
    LONG *Attarray;
#endif /* _SCO_ */

    LONG *pattr;
    SHORT rcc, loc_dpi;
#ifndef LFS
    int ret;
#endif

#ifdef _SCO_
    rcuid = ((RCU *) (&(dmp->b_rptr[0])));
    oflags = *(SHORT far *)(&(param[4]));
    process = ((PRC *)(&(dmp->b_rptr[RCUSIZE])));
    Attarray = ((LONG *)(&(dmp->b_rptr[RCUSIZE+PRCSIZE])));
    pattr = *(LONG **)(&(param[10]));
    loc_dpi = *((SHORT *)(&(dmp->b_rptr[RCUSIZE+PRCSIZE+(2*sizeof(LONG))])));
#else
    rcuid = ((RCU *) ((LONG) dmp + 0));
    oflags = *(SHORT *)(&(param[4]));
    process = ((PRC *) ((LONG) dmp + RCUSIZE));
    Attarray = ((LONG *) ((LONG) dmp + RCUSIZE + PRCSIZE));
#ifdef BIG_ANT
    pattr = *(LONG **)(&(param[12]));
#else
    pattr = *(LONG **)(&(param[10]));
#endif
    loc_dpi = *((SHORT *) ((LONG) dmp + RCUSIZE + PRCSIZE + (2*sizeof(LONG))));
#endif /* _SCO_ */

    if (pattr != (LONG *) NULL) {
       /* Call drvprot.c p_an_open() function */
        rcc = p_an_open(rcuid,oflags,process,Attarray);
    }
    else {
       /* Call drvprot.c p_an_open() function */
        rcc = p_an_open(rcuid,oflags,process,pattr); 
    }

    if (rcc >= 0) {
	((AN_RCUCOR *) (p_orcu[rcc].RTransAddr))->dpi_ind = loc_dpi; 
    }

    return(rcc);
}

/****************************************************************
*        NAME : an_close
* DESCRIPTION : Closes rcu
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC on success / otherwise error code
*    CAUTIONS : 
****************************************************************/
SHORT an_close(ADDR *param)
{
    SHORT dev;   
    SHORT rc;

   /************************************************************ 
     dev: pointer to device handle of an open Antares RCU 
   ************************************************************/ 
#ifdef _SCO_
    dev = *(SHORT  far *)(&(param[0]));
#else
    dev = *(SHORT *)(&(param[0]));
#endif /* _SCO_ */

    rc = p_an_close(dev);
#if 0
    if (rc >= 0) {
	/* Empties entry in hook_functions array */
	p_hookinfo[dev].dpi_id = (SHORT)0xFF;
    }
#endif

    return(rc);
}

/******************************************************************
 *                 AN_SEND_MSG option functions
 ******************************************************************/

/****************************************************************
*        NAME : an_send_message
* DESCRIPTION : Sends message to board
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_send_msg(ADDR *param, mblk_t *dmp)
#else
SHORT an_send_msg(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
#ifdef _SCO_
    SEND_MSG far *msg;
    SYNC_REPLY far *reply;
#else
    SEND_MSG *msg;
    SYNC_REPLY *reply;
#endif /* _SCO_ */

    SHORT prcu;	              /* AN_SEND_MSG() local variables */
    SHORT reply_method;
    ULONG msgtype;

    SHORT res;

    BYTE *usr_msg;
    BYTE *usr_rep;

    extern CFG_INFO cfg;

   /* get send_msg parameters */

   /************************************************************ 
    prcu: RCU device handle;
    msg:  Pointer to message structure.
    reply: Pointer to DSP reply structure.
   ************************************************************/ 
#ifdef _SCO_
    prcu = *(SHORT far *)(&(param[0]));
    msg = (SEND_MSG *) (&(dmp->b_rptr[0]));
    reply = (SYNC_REPLY *)(&(dmp->b_rptr[SENDMSGSIZE+msg->msgsize]));
    reply_method = *(SHORT far *)(&(param[10]));
#else
    prcu = *(SHORT *)(&(param[0]));
    msg = (SEND_MSG *) ((LONG) dmp + 0);
    reply = (SYNC_REPLY *) ((LONG) dmp + SENDMSGSIZE + msg->msgsize);
#ifdef BIG_ANT
    reply_method = *(SHORT *)(&(param[12]));
#else
    reply_method = *(SHORT *)(&(param[10]));
#endif
#endif /* _SCO_ */
    msgtype = STD_MESSAGE;             /* Local assignment */

    if ((prcu < 0) || (prcu >= cfg.maxopenedrcu)) {    /* dev not found */
	return(E_ILLEGALRCU);
    }

    usr_msg = msg->pmessage; 
    usr_rep = reply->smessage; 
#ifdef _SCO_
    msg->pmessage = (BYTE *) (&(dmp->b_rptr[SENDMSGSIZE]));
    reply->smessage = (BYTE *) (&(dmp->b_rptr[SENDMSGSIZE+msg->msgsize+SYNCREPLYSIZE]));
#else
    msg->pmessage = (BYTE *) ((LONG) dmp + SENDMSGSIZE);
    reply->smessage = (BYTE *) ((LONG) dmp+SENDMSGSIZE+msg->msgsize+SYNCREPLYSIZE);
#endif /* _SCO_ */
   /* drvprot.c send message function */
    res = p_send_msg(prcu,msg,reply,msgtype,reply_method);

    msg->pmessage = usr_msg;
    reply->smessage = usr_rep;

    return(res);
}

/****************************************************************
*        NAME : an_put_queue
* DESCRIPTION : Simulates get message from kernel by putting given
*               message into system queue through protocol like get_message
*       INPUT : Board,RCU handler,msgtype,msgid,message size,message buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code;
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_put_queue(ADDR *param, mblk_t *dmp)
#else
SHORT an_put_queue(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
 
    WORD porcu;
    WORD pmsgtype;
    WORD pmsgid;
    WORD pblksz;
    ADDR *pmess;

   /************************************************************
    porcu :   RCU device handler.
    pmsgtype: message type.
    pmsgid  : message identifier.
    pblksz  : block size.
    pmess   : far pointer to a message.
   ************************************************************/ 
	
#ifdef _SCO_
    porcu  =   *(WORD far *)(&(param[0]));
    pmsgtype = *(WORD far *)(&(param[2]));
    pmsgid =   *(WORD far *)(&(param[4]));
    pblksz =   *(WORD far *)(&(param[6]));
    pmess = ((ADDR *)(&(dmp->b_rptr[0])));
#else
    porcu  =   *(WORD *)(&(param[0]));
    pmsgtype = *(WORD *)(&(param[2]));
    pmsgid =   *(WORD *)(&(param[4]));
    pblksz =   *(WORD *)(&(param[6]));
    pmess = ((ADDR *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    rc = p_put_queue(porcu,pmsgtype,pmsgid,pblksz,pmess);

    return(rc);
}

/****************************************************************
*        NAME :  an_bd_open
* DESCRIPTION : Opens an antares bulk data transfer stream 
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : 
*    CAUTINS : 
****************************************************************/
#ifdef _SCO_
SHORT an_bd_open(ADDR *param, mblk_t *dmp)
#else
SHORT an_bd_open(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rcc, loc_dpi;
    SHORT bd_id;
    SHORT bdmode;
#ifdef _SCO_
    SHORT  far *bdstream;
    PRC  far *bdproc;
#else
    SHORT *bdstream;
    PRC *bdproc;
#endif /* _SCO_ */

   /****************************************************************
    bd_id: Far pointer to - Board_id or an RCU handler.
  	   determined according to 5th bit of mode parameter.
    bdmode: Far pointer Mode of operation (bit mask).
    bdstream: Far pointer to stream id of assigned stream 
    bdproc: Far pointer to a process identifier.
    ****************************************************************/

#ifdef _SCO_
    bd_id = *(SHORT far *)(&(param[0]));
    bdmode = *(SHORT far *)(&(param[2]));
    bdstream = ((SHORT *) (&(dmp->b_rptr[0])));
    bdproc = ((PRC *) (&(dmp->b_rptr[sizeof(SHORT)])));
    loc_dpi = *((SHORT *)(&(dmp->b_rptr[sizeof(SHORT)+PRCSIZE])));
#else
    bd_id = *((SHORT *) (&(param[0])));
#ifdef BIG_ANT
    bdmode = *((SHORT *) (&(param[4])));
    bdstream = ((SHORT *) ((LONG) dmp + PRCSIZE));
    bdproc = ((PRC *) ((LONG) dmp + 0));
#else
    bdmode = *((SHORT *) (&(param[2])));
    bdstream = ((SHORT *) ((LONG) dmp + 0));
    bdproc = ((PRC *) ((LONG) dmp + sizeof(SHORT)));
#endif
    loc_dpi = *((SHORT *) ((LONG) dmp + sizeof(SHORT) + PRCSIZE));
#endif /* _SCO_ */

    rcc = p_bd_open(bd_id,bdmode,bdstream,bdproc);
    if (rcc >= 0) {
#ifdef _SCO_
        p_data[rcc] = *bdproc;
#else
        p_data[rcc].dpi_id = bdproc->dpi_id;
        p_data[rcc].proc_id = bdproc->proc_id;
#endif /* _SCO_ */
	((AN_BDCOR *) (p_bd[rcc].BTransAddr))->dpi_ind = loc_dpi; 
    }

    return(rcc);
}

/****************************************************************
*        NAME : an_bd_close
* DESCRIPTION : Closes an antares bulk data transfer stream 
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
SHORT an_bd_close(ADDR *param)
{
    SHORT dev;
    SHORT rc;

   /****************************************************************
    dev: Far pointer to device handle of an open Bulk Data Stream.
    ****************************************************************/
    dev = *(SHORT far *)(&(param[0]));

    rc = p_bd_close(dev);

    return(rc);
}

/****************************************************************
*        NAME : an_bd_setuio
* DESCRIPTION : This function set user I/O functions for open bulk data stream
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_bd_setuio(ADDR *param, mblk_t *dmp)
#else
SHORT an_bd_setuio(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT dev;
#ifdef _SCO_
    AN_UIO far *bduio;
#else
    AN_UIO *bduio;
#endif /* _SCO_ */

   /*******************************************************
    dev: Far pointer to Handle of an open Bulk Data Stream.
    bduio: Far pointer to list of user I/O functions. 	
   *******************************************************/
#ifdef _SCO_
    dev = *((SHORT far *)(&(param[0])));
    bduio = ((AN_UIO *) (&(dmp->b_rptr[0])));
#else
    dev = *((SHORT *)(&(param[0])));
    bduio = ((AN_UIO *) ((LONG) dmp + 0));
#endif /* _SCO_ */
	
    rc = p_bd_setuio(dev,bduio);
    return(rc);
}

/****************************************************************
*        NAME : an_bd_getchan
* DESCRIPTION : Get fist bulk data channel structure before start play  
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_bd_getchan(ADDR *param, mblk_t *dmp)
#else
SHORT an_bd_getchan(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT dev;
    PRC *bdprc;
#ifdef _SCO_
    BD_CHANNEL far *bx;  
    P_FILE far *pfile;
    P_FILE far *usr_pfile;
#else
    BD_CHANNEL *bx;
    P_FILE *pfile;
    P_FILE *usr_pfile;
#endif /* _SCO_ */

   /*******************************************************
    dev: Far pointer to Handle of an open Bulk Data Stream.
   *******************************************************/
#ifdef _SCO_
    dev = *((SHORT far *)(&(param[0])));
    usr_pfile = *((P_FILE far *(far *))(&param[2]));
    pfile = ((P_FILE far *)(&(dmp->b_rptr[0])));
    bx = ((BD_CHANNEL *) (&(dmp->b_rptr[PFILESIZE])));
    bdprc = ((PRC *) (&(dmp->b_rptr[PFILESIZE+BDSIZE])));
#else
    dev = *((SHORT *) (&(param[0])));
#ifdef BIG_ANT
    usr_pfile = *((P_FILE **) (&(param[4])));
#else
    usr_pfile = *((P_FILE **) (&(param[2])));
#endif
    pfile = ((P_FILE *) ((LONG) dmp + 0));
    bx = ((BD_CHANNEL *) ((LONG) dmp + PFILESIZE));
    bdprc = ((PRC *) ((LONG) dmp + PFILESIZE + BDSIZE));
#endif /* _SCO_ */
	
    rc = p_bd_getchan(dev,pfile);
    if (rc == E_SUCC) {
        p_bd[dev].pfile = usr_pfile; 
        bcopy((caddr_t) (&(p_bd[dev])), (caddr_t) bx, BDSIZE);
        bcopy((caddr_t) (&(p_data[dev])), (caddr_t) bdprc, PRCSIZE);
    } 
    return(rc);
}

/****************************************************************
*        NAME : an_bd_xplayf
* DESCRIPTION : This function sends files through an antares bulk data transfer stream
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_bd_xplayf(ADDR *param)
#else
SHORT an_bd_xplayf(ADDR *param,BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT dev;
#ifdef _SCO_
    P_FILE far *pfile;
#else
    P_FILE *pfile;
    BYTE cr;
    BYTE *rcu_buf;
    BD_CHANNEL *bx;
#endif /* _SCO_ */

   /*******************************************************
    dev: Far pointer to Handle of an open Bulk Data Stream.
    pfile: Far pointer to list of P_FILE structures. 	
   *******************************************************/
#ifdef _SCO_
    dev = *((SHORT far *)(&(param[0])));
    pfile = *((P_FILE far *(far *))(&param[2]));
#else
    dev = *((SHORT *) (&(param[0])));
    pfile = ((P_FILE *) ((LONG) dmp + 0));
    bx = ((BD_CHANNEL *) ((LONG) dmp + PFILESIZE));
    rcu_buf = ((BYTE *) ((LONG) dmp + PFILESIZE + BDSIZE));
	
    for (cr = 0; cr <= 1; cr++) {
        p_bd[dev].rcu_buf[cr].eob = bx->rcu_buf[cr].eob;
	p_bd[dev].rcu_buf[cr].size = bx->rcu_buf[cr].size;
    }
	
    bcopy((caddr_t) rcu_buf,
          (caddr_t) &(p_bd[dev].rcu_buf[0].buf[0]),
          (size_t) p_bd[dev].rcu_buf[0].size);

    p_bd[dev].pfile = bx->pfile;
    p_bd[dev].pfstart = bx->pfstart;
    p_bd[dev].poffset = bx->poffset;
    p_bd[dev].offset = bx->offset;
    p_bd[dev].datalen = bx->datalen;

    p_bd[dev].bdstop = bx->bdstop;
    p_bd[dev].HostStop = bx->HostStop;
    p_bd[dev].Completion = bx->Completion;
#endif /* _SCO_ */

    rc = p_bd_xplayf(dev,pfile);

    return(rc);
}

/****************************************************************
*        NAME : an_bd_xrecf
* DESCRIPTION : This function gets files through an antares bulk data transfer stream
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_bd_xrecf(ADDR *param, mblk_t *dmp)
#else
SHORT an_bd_xrecf(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT dev;
#ifdef _SCO_
    P_FILE far *pfile;
    P_FILE far *usr_pfile;
#else
    P_FILE *pfile;
#endif /* _SCO_ */

   /*******************************************************
    dev: Handle of an open Bulk Data Stream.
    pfile: Far pointer to list of P_FILE structures. 	
    *******************************************************/
#ifdef _SCO_
    dev = *((SHORT far *)(&(param[0])));
    usr_pfile = *((P_FILE far *(far *))(&param[2]));
    pfile = ((P_FILE far *)(&(dmp->b_rptr[0])));
    p_bd[dev].pfile = usr_pfile;
#else
    dev = *((SHORT *) (&(param[0])));
    pfile = ((P_FILE *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    rc = p_bd_xrecf(dev,pfile);

    return(rc);
}

/****************************************************************
*        NAME : an_bd_stop
* DESCRIPTION : Stopes an antares bulk data protocol 
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
SHORT an_bd_stop(ADDR *param)
{
    SHORT dev;
    SHORT rc;

   /*******************************************************
    dev: Far pointer to Handle of an open Bulk Data Stream.
   *******************************************************/
    dev = *(SHORT far *)(&(param[0]));

    rc = p_bd_stop(dev);

    return(rc);
}

/******************************************************************
 *                 ANTARES DEBUG COMMANDS FUNCTIONS
 ******************************************************************/

/****************************************************************
*        NAME : an_raw_open
* DESCRIPTION : Opens an antares raw_board device
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : device handle / Error (< 0)
*    CAUTIONS : 
****************************************************************/
SHORT an_raw_open(ADDR *param)
{
    SHORT bd_id;
    SHORT rc;

   /*******************************************************
    bd_id: Far pointer to board identification code.
   *******************************************************/
#ifdef _SCO_
    bd_id = *(SHORT far *) (&(param[0]));
#else
    bd_id = *((SHORT *) (&(param[0])));
#endif /* _SCO_ */

    rc = p_raw_open(bd_id);

    return(rc);
}

/****************************************************************
*        NAME : an_raw_close
* DESCRIPTION : Closes an open antares raw device
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error (< 0)
*    CAUTIONS : 
****************************************************************/
SHORT an_raw_close(ADDR *param)
{
    SHORT dev;
    SHORT rc;

   /***********************************************************
    dev: Far pointer to Device handle	of an open board device
    ***********************************************************/
#ifdef _SCO_
    dev = *((SHORT far *) (&(param[0])));
#else
    dev = *((SHORT *) (&(param[0])));
#endif /* _SCO_ */

    rc = p_raw_close(dev);

    return(rc);
}

/****************************************************************
*        NAME : an_raw_cmd
* DESCRIPTION : Issue a debug command to the antares board
*       INPUT : parameters buffer.
*      OUTPUT : Fills command block according to request
*     RETURNS : E_SUCC / Error (< 0)
*    CAUTIONS : Calls function raw_cmd (driveio.c)
****************************************************************/
#ifdef _SCO_
SHORT an_raw_cmd(ADDR *param, mblk_t *dmp)
#else
SHORT an_raw_cmd(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT dev;
#ifdef _SCO_
    ANT_DB_CMD far *comm;
    SHORT rc;
#ifdef LFS
    ULONG *usr_buf = NULL;
#else
    ULONG *usr_buf;
#endif
    size_t dsize;
    size_t datalen;
    mblk_t *tmp;
#else
    ANT_DB_CMD *comm;
    SHORT rc;
    ULONG *usr_buf = (ULONG *) NULL;
#endif /* _SCO_ */

   /***************************************************************
    dev  : Pointer to Device handle of associated Antares board.
    comm : Pointer to a debug command structure.             
    ****************************************************************/

#ifdef _SCO_
    dev = *(SHORT far *) (&(param[0]));
    comm = (ANT_DB_CMD *) (&(dmp->b_rptr[0]));
#else
    dev = *((SHORT *) (&(param[0])));
    comm = (ANT_DB_CMD *) ((LONG) dmp + 0);
#endif /* _SCO_ */

    if ((comm->type == ANWRITEDATA) || (comm->type == ANREADDATA)) {
        usr_buf = comm->data.buf;
#ifdef _SCO_
        comm->data.buf = (ULONG *) NULL;
        comm->data.buf = (ULONG *) AllocMem(comm->count*4);
        if (comm->data.buf == NULL) {
            cmn_err(CE_WARN,"AllocMem() databuf fail");
            return (E_ALLOCMEM);
        }
#ifdef SVR3
        bzero((caddr_t)comm->data.buf,ctob(btoc(comm->count*4)));
#endif    
        dmp->b_rptr += sizeof(ANT_DB_CMD);
        if (comm->type == ANWRITEDATA) {
            datalen = comm->count*4;
            for (tmp = dmp; tmp != NULL; tmp = tmp->b_cont) {
                dsize = tmp->b_wptr - tmp->b_rptr;
                bcopy((caddr_t)tmp->b_rptr,
                      (caddr_t) (&(comm->data.buf[(comm->count*4)-datalen])),
                      (size_t) dsize);
                datalen -= dsize;
            }  
        }
#else
        comm->data.buf = ((ULONG *) ((LONG) dmp + DBCMDSIZE));
#endif /* _SCO_ */
    }
 
    rc = p_raw_cmd(dev,comm);

    if ((comm->type == ANWRITEDATA) || (comm->type == ANREADDATA)) {
#ifdef _SCO_
        if (comm->type == ANREADDATA) {
            datalen = comm->count*4;
            for (tmp = dmp; tmp != NULL; tmp = tmp->b_cont) {
                dsize = tmp->b_wptr - tmp->b_rptr;
                bcopy((caddr_t) (&(comm->data.buf[(comm->count*4)-datalen])),
                      (caddr_t)tmp->b_rptr,
                      (size_t) dsize);
                datalen -= dsize;
            }  
        }
        dmp->b_rptr -= sizeof(ANT_DB_CMD);
 
        FreeMem((void *) comm->data.buf,comm->count*4);
#endif /* _SCO_ */
        comm->data.buf = usr_buf;
        usr_buf = (ULONG *) NULL; 
    }

    return(rc);
}

/****************************************************************
*        NAME : an_get_info
* DESCRIPTION : This function returns system information in two 
*               different structures: configuration_info, system_info.      
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_get_info(ADDR *param, mblk_t *dmp)
#else
SHORT an_get_info(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    WORD dev;
#ifdef _SCO_
    CFG_INFO far *info_cfg;
    SYS_INFO far *info_sys;
    DSP_INFO far *info_dsp;
#else
    CFG_INFO *info_cfg;
    SYS_INFO *info_sys;
    DSP_INFO *info_dsp;
#endif /* _SCO_ */

   /***************************************************************
    dev:         Far pointer to board identification code 
    info_cfg:    Far pointer to structure containing 
                     Antares configuration variables.      
    info_sys:  = Far pointer to structure containing 
                          Antares system variables.  
    info_dsp:  = Far pointer to structure containing 
                          DSP variables.  
   ***************************************************************/
#ifdef _SCO_
    dev = *(WORD far *)(&(param[0]));
    info_cfg = ((CFG_INFO far *)(&(dmp->b_rptr[0])));
    info_sys = ((SYS_INFO far *)(&(dmp->b_rptr[CFGINFOSIZE])));
    info_dsp = ((DSP_INFO far *)(&(dmp->b_rptr[CFGINFOSIZE+SYSINFOSIZE])));
#else
    dev = *(WORD *) (&(param[0]));
    info_cfg = ((CFG_INFO *) ((LONG) dmp + 0));
    info_sys = ((SYS_INFO *) ((LONG) dmp + CFGINFOSIZE));
    info_dsp = ((DSP_INFO *) ((LONG) dmp + CFGINFOSIZE + SYSINFOSIZE));
#endif /* _SCO_ */

    rc = p_get_info(dev,info_cfg,info_sys,info_dsp);
    return(rc);
}

/****************************************************************
*        NAME : an_get_dongle
* DESCRIPTION : This function returns dongle information 
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_get_dongle(ADDR *param, mblk_t *dmp)
#else
SHORT an_get_dongle(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    WORD dev;
#ifdef _SCO_
    DONGLE_INFO far *dongle_info;

    dev = *((WORD far *)(&(param[0])));
    dongle_info = ((DONGLE_INFO *)(&(dmp->b_rptr[0])));
#else
    DONGLE_INFO *dongle_info;

    dev = *((WORD *) (&(param[0])));
    dongle_info = ((DONGLE_INFO *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    rc = p_get_dongle(dev,dongle_info);
    return(rc);
}

/****************************************************************
*        NAME : an_get_eeprom
* DESCRIPTION : This function returns eeprom information 
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_get_eeprom(ADDR *param, mblk_t *dmp)
#else
SHORT an_get_eeprom(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    WORD dev;
#ifdef _SCO_
    EEPROM_INFO far *eeprom_info;
         
    dev = *((WORD far *)(&(param[0])));
    eeprom_info = ((EEPROM_INFO far *)(&(dmp->b_rptr[0])));
#else
    EEPROM_INFO *eeprom_info;
	
    dev = *((WORD *) (&(param[0])));
    eeprom_info = ((EEPROM_INFO *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    rc = p_get_eeprom(dev,eeprom_info);

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
#ifdef _SCO_
SHORT an_get_version(ADDR *param, mblk_t *dmp)
#else
SHORT an_get_version(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
#ifdef _SCO_
    ULONG far *hverp;
    ULONG far *lverp;
 
    hverp = ((ULONG far *)(&(dmp->b_rptr[0])));
    lverp = ((ULONG far *)(&(dmp->b_rptr[4])));
#else
    ULONG *hverp;
    ULONG *lverp;

    hverp = ((ULONG *) ((LONG) dmp + 0));
    lverp = ((ULONG *) ((LONG) dmp + 4));
#endif /* _SCO_ */

    rc = p_get_version(hverp,lverp);

    return(rc);
}

/****************************************************************
*        NAME : an_getxmitslot
* DESCRIPTION : Get transmit time slot command
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_getxmitslot(ADDR *param, mblk_t *dmp)
#else
SHORT an_getxmitslot(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT prcu;

    SHORT rc;
    long *usr_buf;
#ifdef _SCO_
    SC_TSINFO far *scbusts;
 
    prcu = *(SHORT far *)(&(param[0]));
    scbusts = ((SC_TSINFO *)(&(dmp->b_rptr[0])));
 
    usr_buf = scbusts->sc_tsarrayp;
    scbusts->sc_tsarrayp = ((long *) (&(dmp->b_rptr[sizeof(SC_TSINFO)])));
#else
    SC_TSINFO *scbusts;

    prcu = *(SHORT *) (&(param[0]));
    scbusts = ((SC_TSINFO *) ((LONG) dmp + 0));

    usr_buf = scbusts->sc_tsarrayp;
    scbusts->sc_tsarrayp = ((long *) ((LONG) dmp + sizeof(SC_TSINFO)));
#endif /* _SCO_ */

    rc = an_get_scbus(prcu,scbusts,GETXMITSLOT);

    scbusts->sc_tsarrayp = usr_buf;

    return(rc);
}

/****************************************************************
*        NAME : an_listen
* DESCRIPTION : Listen command
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_listen(ADDR *param, mblk_t *dmp)
#else
SHORT an_listen(ADDR *param, BYTE *dmp)	
#endif /* _SCO_ */
{
    SHORT prcu;
    long *usr_buf;
    SHORT rc;
#ifdef _SCO_
    SC_TSINFO far *scbusts;
 
    prcu = *(SHORT far *)(&(param[0]));
    scbusts = ((SC_TSINFO *)(&(dmp->b_rptr[0])));
 
    usr_buf = scbusts->sc_tsarrayp;
    scbusts->sc_tsarrayp = ((long *) (&(dmp->b_rptr[sizeof(SC_TSINFO)])));
#else
    SC_TSINFO *scbusts;

    prcu = *(SHORT *) (&(param[0]));
    scbusts = ((SC_TSINFO *) ((LONG) dmp + 0));

    usr_buf = scbusts->sc_tsarrayp;
    scbusts->sc_tsarrayp = ((long *) ((LONG) dmp + sizeof(SC_TSINFO)));
#endif /* _SCO_ */

   /* scbusts->sc_numts basically initiaited */
    rc = an_all_scbus(prcu,scbusts,LISTEN,0,FALSE);

    scbusts->sc_tsarrayp = usr_buf;

    return(rc);
}

/****************************************************************
*        NAME : an_unlisten
* DESCRIPTION : UnListen command
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT an_unlisten(ADDR *param)	
{
    SHORT rc;
    SHORT prcu;
    SC_TSINFO scbusts;

#ifdef _SCO_
    prcu = *(SHORT far *) (&(param[0]));
#else
    prcu = *(SHORT *) (&(param[0]));
#endif /* _SCO_ */

    scbusts.sc_numts = 0;

    rc = an_all_scbus(prcu,(SC_TSINFO far *)(&(scbusts)),UNLISTEN,0,FALSE);
    return(rc);
}

/****************************************************************
*        NAME : an_assignxmitslot
* DESCRIPTION : Connect timeslot transmit to SCBUS.
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_assignxmitslot(ADDR *param, mblk_t *dmp)
#else
SHORT an_assignxmitslot(ADDR *param, BYTE *dmp)	
#endif /* _SCO_ */
{
    SHORT prcu;
    ULONG passwd;

    SHORT rc;
    long *usr_buf;

#ifdef _SCO_
    SC_TSINFO far *scbusts;
 
    prcu = *(SHORT far *)(&(param[0]));
    scbusts = ((SC_TSINFO *)(&(dmp->b_rptr[0])));
    passwd = *(ULONG far *)(&(param[6]));
#else
    SC_TSINFO *scbusts;

    prcu = *(SHORT *) (&(param[0]));
    scbusts = ((SC_TSINFO *) ((LONG) dmp + 0));
#ifdef BIG_ANT
    passwd = *(ULONG *) (&(param[8]));
#else
    passwd = *(ULONG *) (&(param[6]));
#endif
#endif /* _SCO_ */
    usr_buf = scbusts->sc_tsarrayp;
#ifdef _SCO_
    scbusts->sc_tsarrayp = ((long *) (&(dmp->b_rptr[sizeof(SC_TSINFO)])));
#else
    scbusts->sc_tsarrayp = ((long *) ((LONG) dmp + sizeof(SC_TSINFO)));
#endif /* _SCO_ */

   /* scbusts->sc_numts basically initiaited */
    rc = an_all_scbus(prcu,scbusts,ASSIGNXMIT,passwd,TRUE);

    scbusts->sc_tsarrayp = usr_buf;

    return(rc);
}

/****************************************************************
*        NAME : an_unassignxmitslot
* DESCRIPTION : DisConnect timeslot transmit from SCBUS.
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT an_unassignxmitslot(ADDR *param)	
{
    SHORT rc;
    SHORT prcu;
    ULONG passwd;
    SC_TSINFO scbusts;

    prcu = *(SHORT far *)(&(param[0]));
#ifdef BIG_ANT
    passwd = *(ULONG far *)(&(param[4]));
#else
    passwd = *(ULONG far *)(&(param[2]));
#endif
    scbusts.sc_numts = 0;

 rc = an_all_scbus(prcu,(SC_TSINFO far *)(&(scbusts)),UNASSIGNXMIT,passwd,TRUE);
    return(rc);
}

/****************************************************************
*        NAME : an_setxmitslot
* DESCRIPTION : Connect timeslot transmit to External PEB timeslot.
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_setxmitslot(ADDR *param, mblk_t *dmp)	
#else
SHORT an_setxmitslot(ADDR *param, BYTE *dmp)	
#endif /* _SCO_ */
{
   SHORT prcu;
   SHORT rc;
   long *usr_buf;
#ifdef _SCO_
   SC_TSINFO far *scbusts;
 
   prcu = *(SHORT far *)(&(param[0]));
   scbusts = ((SC_TSINFO *)(&(dmp->b_rptr[0])));
 
   usr_buf = scbusts->sc_tsarrayp;
   scbusts->sc_tsarrayp = ((long *) (&(dmp->b_rptr[sizeof(SC_TSINFO)])));
#else
   SC_TSINFO *scbusts;

   prcu = *(SHORT *) (&(param[0]));
   scbusts = ((SC_TSINFO *) ((LONG) dmp + 0));

   usr_buf = scbusts->sc_tsarrayp;
   scbusts->sc_tsarrayp = ((long *) ((LONG) dmp + sizeof(SC_TSINFO)));
#endif /* _SCO_ */

  /* scbusts->sc_numts basically initiaited */
   rc = an_all_scbus(prcu,scbusts,SETXMITSLOT,0,FALSE);

   scbusts->sc_tsarrayp = usr_buf;

   return(rc);
}

/****************************************************************
*        NAME : an_unsetxmitslot
* DESCRIPTION : DisConnect timeslot transmit from External PEB.
*       INPUT : parameters buffer
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT an_unsetxmitslot(ADDR *param)	
{
    SHORT rc;
    SHORT prcu;
    SC_TSINFO scbusts;

#ifdef _SCO_
    prcu = *(SHORT far *)(&(param[0]));
 
    scbusts.sc_numts = 0;
 
    rc = an_all_scbus(prcu,(SC_TSINFO far *)(&(scbusts)),UNSETXMITSLOT,0,FALSE);
#else
    prcu = *(SHORT *) (&(param[0]));

    scbusts.sc_numts = 0;

    rc = an_all_scbus(prcu,(SC_TSINFO *) (&(scbusts)),UNSETXMITSLOT,0,FALSE);
#endif /* _SCO_ */
    return(rc);
}

/****************************************************************
*        NAME : an_route
* DESCRIPTION : Assigns time slots to a resource class unit.
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number.
*    CAUTIONS : 
****************************************************************/
SHORT an_route(ADDR *param) 
{
  SHORT prcu;
  LONG lrts,ltts;
  TS_INFO rtsinfo;
  TS_INFO ttsinfo;

  /***************************************************************
   prcu:    RCU device handle
   tsinfo : Far pointer to a time slot structure
   ***************************************************************/
#ifdef _SCO_
  prcu = *(SHORT far *)(&(param[0]));
  lrts = *(LONG far *)(&(param[2]));
#else
  prcu = *(SHORT *) (&(param[0]));
#ifdef BIG_ANT
  lrts = *(LONG *) (&(param[4]));
#else
  lrts = *(LONG *) (&(param[2]));
#endif
#endif /* _SCO_ */
  ltts = lrts;

  /* Deassign operation */
  if (lrts == -1) {
  	rtsinfo.listlength = TS_DEASSIGN;
  	ttsinfo.listlength = TS_DEASSIGN;
  }
  else {
  	if (lrts == 0) {
            rtsinfo.listlength = 0;
            ttsinfo.listlength = 0;
  	}
	else {
  	    rtsinfo.listlength = 1;
  	    ttsinfo.listlength = 1;
	}
  }
#ifdef _SCO_
  rtsinfo.slots = (LONG far *)(&(lrts));
  ttsinfo.slots = (LONG far *)(&(ltts));
 
  return(an_all_route(prcu,(TS_INFO far *)&rtsinfo,(TS_INFO far *)&ttsinfo));
#else
  rtsinfo.slots = (LONG *) (&(lrts));
  ttsinfo.slots = (LONG *) (&(ltts));

  return(an_all_route(prcu,(TS_INFO *) &rtsinfo,(TS_INFO *) &ttsinfo));
#endif /* _SCO_ */
}

/****************************************************************
*        NAME : an_routerxts
* DESCRIPTION : Assigns individual time slots to a resource class unit.
*       INPUT : parameters buffer.
*       INPUT : RCU device handle
* 		Pointer to Recieve_Time_Slot structure
* 		Pointer to Transmit_Time_Slot structure
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number.
*    CAUTIONS : 
****************************************************************/
SHORT an_routerxts(ADDR *param) 
{
    SHORT rc;
    SHORT prcu;
    LONG lrts,ltts;
    TS_INFO rtsinfo;
    TS_INFO ttsinfo;

#ifdef _SCO_
    prcu = *(SHORT far *)(&(param[0]));
    lrts = *(LONG far *)(&(param[2]));
    ltts = *(LONG far *)(&(param[6]));
#else
    prcu = *(SHORT *) (&(param[0]));
#ifdef BIG_ANT
    lrts = *(LONG *) (&(param[4]));
    ltts = *(LONG *) (&(param[8]));
#else
    lrts = *(LONG *) (&(param[2]));
    ltts = *(LONG *) (&(param[6]));
#endif
#endif /* _SCO_ */
   /* Deassign operation */
    if (lrts == -1) {
  	rtsinfo.listlength = TS_DEASSIGN;
    }
    else {
	if (lrts == 0) {
  	    rtsinfo.listlength = 0;
	}
	else {
            rtsinfo.listlength = 1;
	}
    }
#ifdef _SCO_
    rtsinfo.slots = (LONG far *) (&(lrts));
#else
    rtsinfo.slots = (LONG *) (&(lrts));
#endif /* _SCO_ */

   /* Deassign operation */
    if (ltts == -1) {
  	ttsinfo.listlength = TS_DEASSIGN;
    }
    else {
	if (ltts == 0) {
  	    ttsinfo.listlength = 0;
	}
	else {
  	    ttsinfo.listlength = 1;
	}
    }
#ifdef _SCO_
    ttsinfo.slots = (LONG far *) (&(ltts));
    rc = an_all_route(prcu,(TS_INFO far *) &rtsinfo,(TS_INFO far *) &ttsinfo);
#else
    ttsinfo.slots = (LONG *) (&(ltts));
    rc = an_all_route(prcu,(TS_INFO *) &rtsinfo,(TS_INFO *) &ttsinfo);
#endif /* _SCO_ */

    return(rc);
}

/****************************************************************
*        NAME : an_routelist
* DESCRIPTION : Assigns a list of individual time slots to a resource
*		class unit.
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number.
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_routelist(ADDR *param, mblk_t *dmp) 
#else
SHORT an_routelist(ADDR *param, BYTE *dmp) 
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT prcu;

#ifdef _SCO_
    TS_INFO far *rtsinfo;
    TS_INFO far *ttsinfo;
#else
    TS_INFO *rtsinfo;
    TS_INFO *ttsinfo;
#endif /* _SCO_ */

    LONG *usr_rts = (LONG *) NULL;
    LONG *usr_tts = (LONG *) NULL;

   /***************************************************************
    prcu:    RCU device handle
    rtsinfo : far pointer to receive  time slot structure list
    ttsinfo : far pointer to transmit time slot structure list
  ***************************************************************/
#ifdef _SCO_
    prcu = *(SHORT far *)(&(param[0]));
    rtsinfo = ((TS_INFO far *)(&(dmp->b_rptr[0])));
#else
    prcu = *(SHORT *) (&(param[0]));
    rtsinfo = ((TS_INFO *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    if (rtsinfo->listlength == -1) {
  	rtsinfo->listlength = TS_DEASSIGN;
#ifdef _SCO_
        ttsinfo = ((TS_INFO far *)(&(dmp->b_rptr[TSINFOSIZE])));
#else
        ttsinfo = ((TS_INFO *) ((LONG) dmp + TSINFOSIZE));
#endif /* _SCO_ */
    }
    else {
        usr_rts = rtsinfo->slots;
#ifdef _SCO_
        rtsinfo->slots = (LONG *) (&(dmp->b_rptr[TSINFOSIZE]));
        ttsinfo = ((TS_INFO far *)(&(dmp->b_rptr[TSINFOSIZE+(rtsinfo->listlength*sizeof(LONG))])));
#else
        rtsinfo->slots = (LONG *) ((LONG) dmp + TSINFOSIZE);
        ttsinfo = ((TS_INFO *) ((LONG) dmp+TSINFOSIZE+(rtsinfo->listlength*sizeof(LONG))));
#endif /* _SCO_ */
    }

    if (ttsinfo->listlength == -1) {
  	ttsinfo->listlength = TS_DEASSIGN;
    }
    else {
        usr_tts = ttsinfo->slots;
        if (rtsinfo->listlength == TS_DEASSIGN) {
#ifdef _SCO_
            ttsinfo->slots = (LONG *) (&(dmp->b_rptr[(2*TSINFOSIZE)]));
#else
            ttsinfo->slots = (LONG *) ((LONG) dmp + (2*TSINFOSIZE));
#endif /* _SCO_ */
        }
        else {
#ifdef _SCO_
            ttsinfo->slots = (LONG *) (&(dmp->b_rptr[(2*TSINFOSIZE)+(rtsinfo->listlength*sizeof(LONG))]));
#else
            ttsinfo->slots = (LONG *) ((LONG) dmp+(2*TSINFOSIZE)+(rtsinfo->listlength*sizeof(LONG)));
#endif /* _SCO_ */
        }
    }

    rc = an_all_route(prcu,rtsinfo,ttsinfo);

    if (usr_rts != (LONG *) NULL) {
        rtsinfo->slots = usr_rts;
    }

    if (usr_tts != (LONG *) NULL) {
        ttsinfo->slots = usr_tts;
    }

    return(rc);
}

/****************************************************************
*        NAME : an_mapslot
* DESCRIPTION : Maps external time slots to an antares internal time slots.
*       INPUT : parameters buffer.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error number.
*    CAUTIONS : 
****************************************************************/
#ifdef _SCO_
SHORT an_mapslot(ADDR *param, mblk_t *dmp)
#else
SHORT an_mapslot(ADDR *param, BYTE *dmp)
#endif /* _SCO_ */
{
    SHORT rc;
    SHORT prcu;
#ifdef _SCO_
    MAP_SLOT far *mpslot;
#else
    MAP_SLOT *mpslot;
#endif /* _SCO_ */

  /***************************************************************
   prcu:    RCU device handle
   mpslot:  Far pointer to mapslot structure	list
   ***************************************************************/
#ifdef _SCO_
    prcu = *(SHORT far *)(&(param[0]));
    mpslot = ((MAP_SLOT far *)(&(dmp->b_rptr[0])));
#else
    prcu = *(SHORT *) (&(param[0]));
    mpslot = ((MAP_SLOT *) ((LONG) dmp + 0));
#endif /* _SCO_ */

    rc = p_mapslot(prcu,mpslot);
    return(rc);
}
