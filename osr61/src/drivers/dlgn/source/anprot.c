/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : anprot.c
 * Description                  : antares protocol level
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
/* The following is done to save changing
   many many lines of #ifdef _SCO_ code */
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
#endif

#include "dlgcos.h"
#include "include.h"

#include "gndefs.h"
#include "gnlibdrv.h"
#include "gndrv.h"
#include "gnmsg.h"
#include "gndlgn.h"

#include "protocol.h"
#include "intrface.h"
#include "extvar.h"
#include "anprot.h"

#include "drvprot.h"
#include "extern.h"

#include "anlibdrv.h"
#include "andrv.h"

extern void an_reply(GN_LOGDEV*,mblk_t*,SHORT,SHORT,ULONG,int);

#ifdef WNT_ANT
extern VOID cmn_err(ULONG,...);
#endif

/* System internal info */
extern SYS_INFO	sys;
extern GN_LOGDEV *main_ldp;
extern GN_LOGDEV *curr_ldp;
extern ULONG curr_gnhndl;
extern AN_RCUCOR *rcu_cor;
extern AN_BDCOR *bd_cor;

/****************************************************************
*        NAME : init_interface
* DESCRIPTION : Called by antares.c prior to registering driver
*               in order to initiate interface values.         
*       INPUT : 
*      OUTPUT : 
*     RETURNS : 
*    CAUTIONS : 
****************************************************************/
VOID init_interface(CFG_INFO far *config)
{
    d_nboards = config->nboards;       /* Number of boards in system */
    return;
}

/****************************************************************
*        NAME : ant_forceaddr
* DESCRIPTION : Forces addr to be at offset 0
*       INPUT : huge pointer
*      OUTPUT : 
*     RETURNS : settled huge pointer
*    CAUTIONS : 
****************************************************************/
CHAR *ant_forceaddr(CHAR *data)
{
    return (data);
}

VOID  ant_id_click()
{
    return;
}

VOID syserror(VOID) {
    cmn_err(CE_WARN,"syserror() illegal");
    return;
}
#ifdef _SCO_

SHORT xant_fileread(int fh,ADDR *buf,WORD *bcount,ADDR *p)
{
    cmn_err(CE_WARN,"xant_fileread() illegal");
    return (FALSE);
}
 
SHORT xant_filewrite(int fh,ADDR *buf,WORD far *bcount,ADDR *p)
{
    cmn_err(CE_WARN,"xant_filewrite() illegal");
    return (FALSE);
}
 
/**********************************************************************
 *       NAME : xant_fileseek(fh,method,rcxp,rdxp)
 *DESCRIPTION : Move file pointer to given position using the method indicated.
 **********************************************************************/
SHORT xant_fileseek(SHORT loc,int fh,WORD method,ULONG dist,LONG *offs,ADDR *p)
{
    GN_LOGDEV *genldp;
    ULONG gnhndl;
    SHORT dpi_ind;
      
    mblk_t *cmp = (mblk_t *) NULL;
 
    if (((AN_BDCOR *)p)->bdhandle != loc) {
        cmn_err(CE_WARN,"xant_fileseek() Not the same bd handle");
        return -1;
    } 
      
    genldp = (GN_LOGDEV *) ((AN_BDCOR *)p)->ldp;
    gnhndl = (ULONG) ((AN_BDCOR *)p)->gnhndl;
    dpi_ind = (SHORT) ((AN_BDCOR *)p)->dpi_ind;
      
    if (!(genldp->ld_flags & LD_OPEN)) {
#if 0 
        cmn_err(CE_WARN,"xant_fileseek() %s not open",genldp->ld_name);
#endif
        return -1;
    } 
      
    genldp->ld_cmdcnt = 0;
    cmp = dlgn_findreply(genldp,AND_BDSEEK,AN_BDCTLSZ);
    if (cmp == NULL) {
       cmn_err(CE_WARN,"xant_fileseek(): dlgn_findreply(): message pool full");
       return -1;
    } 
      
    cmp->b_datap->db_type = M_PROTO;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_BULK;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = AND_BDSEEK;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
      
    bcopy((caddr_t) (&(p_bd[loc])),
          (caddr_t) (&((AN_BDCTL *)cmp->b_rptr)->p_bd_entry),
          (size_t) BDSIZE);
 
    ((AN_BDCTL *)cmp->b_rptr)->cr = (BYTE) fh;
    ((AN_BDCTL *)cmp->b_rptr)->opt = (BYTE) method;
    ((AN_BDCTL *)cmp->b_rptr)->datalen = dist;
      
    ((AN_BDCTL *)cmp->b_rptr)->dpi_ind = dpi_ind;
 
#if 0
    cmn_err(CE_CONT,"xant_fileseek() : gnhndl=%d\n",gnhndl);
#endif
    an_reply(genldp,cmp,0,0,0,ANDT_MSG);
     
    *offs = (LONG) dist;
 
    return(E_SUCC);
}
#endif /* _SCO_ */

/*******************************************************************
 *       NAME : ant_memmov(s1p,s2p,count)
 *DESCRIPTION : Move memory data from one buffer to another.
 *      INPUT : s1p = Address of destination.
 *            : s2p = Address of source.
 *            : count = Number of bytes to move.
 *     OUTPUT : None.
 *    RETURNS : Nothing.
 *   CAUTIONS : None.
 *******************************************************************/
VOID ant_memmov(ADDR *s1p,ADDR *s2p,WORD count)
{
    bcopy((caddr_t) s2p,(caddr_t) s1p,(size_t) count);	
}

/****************************************************************
*        NAME : send_delay_request
* DESCRIPTION : Place function call in scheduler to be executed 
*               in scheduler time (fill_buffers in bulk_data process etc).   
*       INPUT : Function data_segment,function address.
*      OUTPUT : 
*     RETURNS : E_SUCC / error_code
*    CAUTIONS : 
****************************************************************/
SHORT send_delay_request(SHORT loc, BYTE opt, BYTE cr, 
                         BYTE dir, LONG datalen, ADDR *p)
{
    GN_LOGDEV *genldp;
    ULONG gnhndl;
    LONG dataleft, bsize;

    mblk_t *cmp = (mblk_t *) NULL; 
    mblk_t *dmp = (mblk_t *) NULL; 

    if (sys.an_state != Active) {
        return(E_SUCC);
    }

    if (((AN_BDCOR *)p)->bdhandle != loc) {
        cmn_err(CE_WARN,"xsend_delay_request() Not the same bd handle");
        return -1;
    } 

    genldp = (GN_LOGDEV *) ((AN_BDCOR *)p)->ldp;
    gnhndl = (ULONG) ((AN_BDCOR *)p)->gnhndl;   

    if (!(genldp->ld_flags & LD_OPEN)) {
#if 0
        cmn_err(CE_WARN,"xsend_delay_request() %s not open",genldp->ld_name);
#endif
        return -1;
    } 

    genldp->ld_cmdcnt = 0;
    cmp = dlgn_findreply(genldp,ANC_BDCTL,AN_BDCTLSZ);
    if (cmp == NULL) {
       cmn_err(CE_WARN,"send_delay: dlgn_findreply() : message pool full"); 
       return -1;
    }

    cmp->b_datap->db_type = M_PROTO;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_BDCTL;
    ((AN_BDCTL *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;

    bcopy((caddr_t) (&(p_bd[loc])), 
          (caddr_t) (&((AN_BDCTL *)cmp->b_rptr)->p_bd_entry),
          (size_t) BDSIZE);	
    
    ((AN_BDCTL *)cmp->b_rptr)->loc = loc;
    ((AN_BDCTL *)cmp->b_rptr)->opt = opt;
    ((AN_BDCTL *)cmp->b_rptr)->dir = dir;
    ((AN_BDCTL *)cmp->b_rptr)->cr = cr;
    ((AN_BDCTL *)cmp->b_rptr)->datalen = datalen;
    ((AN_BDCTL *)cmp->b_rptr)->dpi_ind = (SHORT) ((AN_BDCOR*)p)->dpi_ind;
    
   /* Entering message to Antares local queue */
    if (dir == ANT_BDREAD) {
        dataleft = datalen; 
        while ((dataleft > 0) && (dataleft <= datalen)) {
            if (dataleft >= 4096) {
                bsize = 4096;
            }
            else {
                bsize = dataleft;
            }

#ifdef LFS
	    /* Do not use BPRI_HI: it can fail too easily. -- bb */
            dmp = (mblk_t *) allocb(bsize,BPRI_MED);
#else
            dmp = (mblk_t *) allocb(bsize,BPRI_HI);
#endif
            if (dmp == NULL) {
                cmn_err(CE_WARN,"allocb() fail in xsend_delay_request()");
                return -1;
            } 
            dmp->b_datap->db_type = M_DATA;
            bcopy((caddr_t) (&(p_bd[loc].rcu_buf[cr].buf[datalen-dataleft])), 
                  (caddr_t) dmp->b_rptr, (size_t) bsize);	
            dmp->b_wptr += bsize; 
            linkb(cmp,dmp);
            dataleft -= bsize;
        }
    }
#if 0
    cmn_err(CE_CONT,"xsend_delay_request() : gnhndl=%d\n",gnhndl); 
#endif
    an_reply(genldp,cmp,0,0,0,ANDT_BDSTR);
    return(E_SUCC);
}

/****************************************************************
*        NAME : xsend_bulkdata_msg
* DESCRIPTION : Sending messages from bulk_data protocol.
*               Means one event_block to local queue, + one event to 
*               Dialogic main queue using send_system_queue().
*       INPUT : board_id, Line in code, bulk data device handler,
*               bulk data protocol direction,bulk data type of message,
					 Error code if exists.
*      OUTPUT : 
*     RETURNS : E_SUCC / Error_code.
*    CAUTIONS : 
****************************************************************/
SHORT xsend_bulkdata_msg(BYTE inter,SHORT board_id,SHORT bd_line,
                         SHORT bd_channel,SHORT bd_direction,
                         SHORT bd_msgtype,SHORT bd_errorcode,
                         ULONG bd_datalen, ADDR *p)
{
    GN_LOGDEV *genldp;
    ULONG gnhndl;
    PRC proc;
    SHORT dpi_ind;

    mblk_t *cmp = (mblk_t *) NULL; 
    mblk_t *dmp = (mblk_t *) NULL; 

    if (sys.an_state != Active) {
        return(E_SUCC);
    }
    
    if (((AN_BDCOR *)p)->bdhandle != bd_channel) {
        cmn_err(CE_WARN,"xsend_bd_msg() Not the same bd handle");
        return -1;
    } 

    genldp = (GN_LOGDEV *) ((AN_BDCOR *)p)->ldp;
    gnhndl = (ULONG) ((AN_BDCOR *)p)->gnhndl;   
    proc.proc_id = ((AN_BDCOR *)p)->bdproc.proc_id;   
    proc.dpi_id = ((AN_BDCOR *)p)->bdproc.dpi_id;   
    dpi_ind = ((AN_BDCOR *)p)->dpi_ind; 

    if (!(genldp->ld_flags & LD_OPEN)) {
#if 0
        cmn_err(CE_WARN,"xsend_bulkdata_msg() %s not open",genldp->ld_name);
#endif
        return -1;
    }   

    genldp->ld_cmdcnt = 0;
    cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
    if (cmp == NULL) {
       cmn_err(CE_WARN,"xsend_bd_msg(): dlgn_findreply(): message pool full"); 
       return -1;
    }
    
    cmp->b_datap->db_type = M_PROTO;
    ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_BULK;
    ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
    ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;

    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = bd_channel;
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = 12; 
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = bd_msgtype;
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.rclass = NO_RCU; 
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.board = NO_RCU; 
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.dsp = NO_RCU; 
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.unit = NO_RCU; 
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG) BDMESSAGE;   
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;   
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;   
    ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = bd_errorcode;   

    ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;   

   /* Entering message to Antares local queue */
    dmp = (mblk_t *) allocb(12,BPRI_MED);
    if (dmp == NULL) {
        cmn_err(CE_WARN,"allocb() fail in xsend_bd_msg()");
        return -1;
    } 

    dmp->b_datap->db_type = M_DATA;
    bzero(dmp->b_datap->db_base,12);
   /* More details on process are inserted to local event buffer */
    *((SHORT *)(&(dmp->b_rptr[0]))) = (SHORT) ((AN_BDCOR *)p)->bd_id;
    *((SHORT *)(&(dmp->b_rptr[2]))) = (SHORT) ((AN_BDCOR *)p)->bdstream;
    *((SHORT *)(&(dmp->b_rptr[4]))) = bd_direction;
#ifdef BIG_ANT
    *((SHORT *)(&(dmp->b_rptr[6]))) = bd_line;
    *((ULONG *)(&(dmp->b_rptr[8]))) = bd_datalen;
#else
    *((ULONG *)(&(dmp->b_rptr[6]))) = bd_datalen;
    *((SHORT *)(&(dmp->b_rptr[10]))) = bd_line;
#endif
    dmp->b_wptr += 12; 
    
    linkb(cmp,dmp);
#if 0
    cmn_err(CE_CONT,"xsend_bulkdata_msg() : gnhndl=%d\n",gnhndl); 
#endif
    an_reply(genldp,cmp,0,0,0,ANDT_MSG);
    
    return(E_SUCC);
}

/****************************************************************
*        NAME : xsend_error_msg
* DESCRIPTION : Sending Fatal Error messages; This function uses 
*               only system main queue
*       INPUT : 
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
****************************************************************/
SHORT far xsend_error_msg(SHORT board_id,SHORT rcu,SHORT dsp,
                          SHORT errorcode,SHORT eline,ADDR *p)
{
    GN_LOGDEV *genldp;
#ifdef _SCO_
    ULONG gnhndl;
#else
    AN_LOGDEV *anldp;
    LONG gnhndl;
#endif /* _SCO_ */
    PRC proc;
    SHORT dpi_ind = 0;

    mblk_t *cmp = (mblk_t *) NULL; 
    mblk_t *dmp = (mblk_t *) NULL; 

    if (sys.an_state != Active) {
        return(E_SUCC);
    }
    
   /* Attention p may be NULL pointer of no specific RCU is 
      involved (rcu == NO_RCU)  */	

    if (p == ((ADDR *) NULL)) {
#ifdef _SCO_
        gnhndl = 0;
        proc.proc_id = 0;   
        proc.dpi_id = SYSTEMDPI_ID;      
        for (genldp = main_ldp->ld_nldp; (genldp->ld_flags & LD_OPEN);
             genldp = genldp->ld_nldp) {

            genldp->ld_cmdcnt = 0;
            cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
            if (cmp == NULL) {
                cmn_err(CE_WARN,"xsend_error: dlgn_findreply() : message full");
                return -1;
            }
            cmp->b_datap->db_type = M_PROTO;

            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_EVENT;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;
   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcu;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = 2;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = E_FATALERROR;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.rclass = NO_RCU;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.board = (BYTE) board_id;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.dsp = (BYTE) dsp;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.unit = NO_RCU;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG) E_FATALERROR;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = errorcode;

            ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;

           /* Entering message to Antares local queue */
            dmp = (mblk_t *) allocb(2,BPRI_MED);
            if (dmp == NULL) {
                cmn_err(CE_WARN,"allocb() fail in xsend_error_msg()");
                return -1;
            } 

            dmp->b_datap->db_type = M_DATA;
            bzero(dmp->b_datap->db_base,2);
            *((SHORT *)(&(dmp->b_rptr[0]))) = eline;
            dmp->b_wptr += 2;
   
            linkb(cmp,dmp);
#if 0
            cmn_err(CE_CONT,"xsend_error_msg() : gnhndl=%d\n",gnhndl);
#endif   
            an_reply(genldp,cmp,0,0,0,ANDT_MSG);
        }   
    }    

#else
        proc.proc_id = 0;   
        proc.dpi_id = ERRORDPI_ID;   
        for (genldp = main_ldp->ld_nldp; genldp != NULL; 
             genldp = genldp->ld_nldp) {
            if (genldp->ld_flags & LD_OPEN) {
            genldp->ld_cmdcnt = 0;
            anldp = Gldp2Anldp(genldp);
            gnhndl = anldp->gnhndl;
            if (gnhndl == -1) {
                continue;
            }
            cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
            if (cmp == NULL) {
                cmn_err(CE_WARN,"xsend_error: dlgn_findreply() : message full");
                return -1;
            }
            cmp->b_datap->db_type = M_PROTO;
 
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;
    
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcu; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = 2; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = E_FATALERROR;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.rclass = NO_RCU; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.board = (BYTE) board_id; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.dsp = (BYTE) dsp; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.unit = NO_RCU; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG) E_FATALERROR;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = errorcode;   

            ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;   

           /* Entering message to Antares local queue */
            dmp = (mblk_t *) allocb(2,BPRI_MED);
            if (dmp == NULL) {
                cmn_err(CE_WARN,"allocb() fail in xsend_error_msg()");
                return -1;
            } 

            dmp->b_datap->db_type = M_DATA;
            bzero(dmp->b_datap->db_base,2);
            *((SHORT *)(&(dmp->b_rptr[0]))) = eline;
            dmp->b_wptr += 2;
    
            linkb(cmp,dmp);
#if 0
            cmn_err(CE_CONT,"xsend_error_msg() : gnhndl=%d\n",gnhndl); 
#endif
            an_reply(genldp,cmp,0,0,0,ANDT_MSG);
        }   
        }
    }
#endif /* _SCO_ */
    else {
        if (((AN_RCUCOR *)p)->rhandle != rcu) {
            cmn_err(CE_WARN,"xsend_error_msg() Not the same rcu handle");
            return -1;
        } 

        genldp = (GN_LOGDEV *) ((AN_RCUCOR *)p)->ldp;
        gnhndl = (ULONG) ((AN_RCUCOR *)p)->gnhndl;
        proc.proc_id = ((AN_RCUCOR *)p)->proc.proc_id;   
        proc.dpi_id = ((AN_RCUCOR *)p)->proc.dpi_id;   
        dpi_ind = ((AN_RCUCOR *)p)->dpi_ind;   

        if (!(genldp->ld_flags & LD_OPEN)) {
#if 0
            cmn_err(CE_WARN,"xsend_error_msg() %s not open",genldp->ld_name);
#endif
            return -1;
        }   

        genldp->ld_cmdcnt = 0;
        cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
        if (cmp == NULL) {
            cmn_err(CE_WARN,"xsend_error: dlgn_findreply() : msg pool full"); 
            return -1;
        }

        cmp->b_datap->db_type = M_PROTO;

        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;
    
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcu; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = 2; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = E_FATALERROR;
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.rclass = NO_RCU; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.board = (BYTE) board_id; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.dsp = (BYTE) dsp; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu.unit = NO_RCU; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG) E_FATALERROR;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = errorcode;   

        ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;   

       /* Entering message to Antares local queue */
        dmp = (mblk_t *) allocb(2,BPRI_MED);
        if (dmp == NULL) {
            cmn_err(CE_WARN,"allocb() fail in xsend_error_msg()");
            return -1;
        } 

        dmp->b_datap->db_type = M_DATA;
        bzero(dmp->b_datap->db_base,2);
        *((SHORT *)(&(dmp->b_rptr[0]))) = eline;
        dmp->b_wptr += 2;
    
        linkb(cmp,dmp);
#if 0
        cmn_err(CE_CONT,"xsend_error_msg() : gnhndl=%d\n",gnhndl); 
#endif
        an_reply(genldp,cmp,0,0,0,ANDT_MSG);
    }
    return(E_SUCC);
}

/****************************************************************
*        NAME : xsend_dsp_msg
* DESCRIPTION : Routine procedure of sending message from DSP to driver:
*               1: Sending message in slices through driver local_queue  
*                  number of slices is determined according to message length.
*               2: Sending message to main queue (send_system_queue) which
*                  direct function an_recv_msg how to reorganize message.
*       INPUT : board_id,message size, message type, data buffer, destination 
*               RCU, Process global_error.   
*      OUTPUT : 
*     RETURNS : E_SUCC / Error code
*    CAUTIONS : 
**************************************************************/
SHORT xsend_dsp_msg(
BYTE inter,SHORT board_id,WORD blk,WORD msgtype,WORD msgid,
ADDR *data,SHORT rcudest,SHORT l_error,RCU srcrcu,ADDR *p)
{
    GN_LOGDEV *genldp;
#ifdef _SCO_
    ULONG gnhndl;
#else
    AN_LOGDEV *anldp;
    LONG gnhndl;
#endif
    PRC proc;
    SHORT dpi_ind = 0;

    mblk_t *cmp = (mblk_t *) NULL; 
    mblk_t *dmp = (mblk_t *) NULL; 

    if (sys.an_state != Active) {
        return(E_SUCC);
    }
    
   /* Attention p may be NULL pointer of no specific RCU is 
      involved (rcu == NO_RCU)  */	

    if (p == ((ADDR *) NULL)) {
#ifdef _SCO_
        gnhndl = 0;   
        proc.proc_id = 0;
        proc.dpi_id = SYSTEMDPI_ID;
        for (genldp = main_ldp->ld_nldp; (genldp->ld_flags & LD_OPEN);
             genldp = genldp->ld_nldp) {
 
            genldp->ld_cmdcnt = 0;
            cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
            if (cmp == NULL) {
                cmn_err(CE_WARN,"xsend_dsp: dlgn_findreply() : message full");
                return -1;
            }
    
            cmp->b_datap->db_type = M_PROTO;
 
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_EVENT;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;
 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcudest;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = blk;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = msgtype;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu = srcrcu;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG)msgid;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = l_error;
 
            ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;
 
           /* Entering message to Antares local queue */
            dmp = (mblk_t *) allocb(blk,BPRI_MED);
            if (dmp == NULL) {
                cmn_err(CE_WARN,"allocb() fail in xsend_dsp_msg()");
                return -1;
            }
 
            dmp->b_datap->db_type = M_DATA;
            bzero(dmp->b_datap->db_base,blk);
            bcopy((caddr_t) (&(data[0])), (caddr_t) dmp->b_rptr, (size_t) blk);
            dmp->b_wptr += blk;
    
            linkb(cmp,dmp);
#if 0
            cmn_err(CE_CONT,"xsend_dsp_msg() : gnhndl=%d\n",gnhndl);
#endif
            an_reply(genldp,cmp,0,0,0,ANDT_MSG);
        }
    }

#else
        proc.proc_id = 0;   
        proc.dpi_id = ERRORDPI_ID;   
        for (genldp = main_ldp->ld_nldp; genldp != NULL; 
             genldp = genldp->ld_nldp) {
            if (genldp->ld_flags & LD_OPEN) {
            genldp->ld_cmdcnt = 0;
            anldp = Gldp2Anldp(genldp);
            gnhndl = anldp->gnhndl;
            if (gnhndl == -1) {
                continue;
            }

            cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
            if (cmp == NULL) {
                cmn_err(CE_WARN,"xsend_dsp_msg(): message full"); 
                return -1;
            }
    
            cmp->b_datap->db_type = M_PROTO;

            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
            ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;

            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcudest; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = blk; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = msgtype;
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu = srcrcu; 
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG)msgid;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;   
            ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = l_error;   

            ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;   

           /* Entering message to Antares local queue */
            dmp = (mblk_t *) allocb(blk,BPRI_MED);
            if (dmp == NULL) {
                cmn_err(CE_WARN,"allocb() fail in xsend_dsp_msg()");
                return -1;
            } 

            dmp->b_datap->db_type = M_DATA;
            bzero(dmp->b_datap->db_base,blk);
            bcopy((caddr_t) (&(data[0])), (caddr_t) dmp->b_rptr, (size_t) blk);	
            dmp->b_wptr += blk;
    
            linkb(cmp,dmp);
#if 0
            cmn_err(CE_CONT,"xsend_dsp_msg() : gnhndl=%d\n",gnhndl); 
#endif
            an_reply(genldp,cmp,0,0,0,ANDT_MSG);
        } 
        }
    }
#endif /* _SCO_ */
    else {
        if (((AN_RCUCOR *)p)->rhandle != rcudest) {
            cmn_err(CE_WARN,"xsend_dsp_msg() Not the same rcu handle");
            return -1;
        } 

        genldp = (GN_LOGDEV *) ((AN_RCUCOR *)p)->ldp;
        gnhndl = (ULONG) ((AN_RCUCOR *)p)->gnhndl;
        proc.proc_id = ((AN_RCUCOR *)p)->proc.proc_id;   
        proc.dpi_id = ((AN_RCUCOR *)p)->proc.dpi_id;   
        dpi_ind = ((AN_RCUCOR *)p)->dpi_ind; 

        if (!(genldp->ld_flags & LD_OPEN)) {
#if 0
            cmn_err(CE_WARN,"xsend_dsp_msg() %s not open",genldp->ld_name);
#endif
            return -1;
        }   

        genldp->ld_cmdcnt = 0;
        cmp = dlgn_findreply(genldp,ANC_MSGRECV,AN_DRVMSGSZ);
        if (cmp == NULL) {
            cmn_err(CE_WARN,"xsend_dsp: dlgn_findreply() : message pool full"); 
            return -1;
        }
    
        cmp->b_datap->db_type = M_PROTO;

        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_msg_type = MT_VIRTEVENT;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.fw_msg_ident = ANC_MSGRECV;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_address.da_handle = gnhndl;
        ((AN_DRVCMD *)cmp->b_rptr)->gncmdmsg.cm_nreplies = 0;

        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.dev = rcudest; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.pmessage = (VOID *) NULL;
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgsize = blk; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgtype = msgtype;
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.src_rcu = srcrcu; 
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.msgid = (ULONG)msgid;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.proc_id = proc.proc_id;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.proc.dpi_id = proc.dpi_id;   
        ((AN_DRVCMD *)cmp->b_rptr)->ioevt.rtcode = l_error;   

        ((AN_DRVCMD *)cmp->b_rptr)->dpi_ind = dpi_ind;   

       /* Entering message to Antares local queue */
        dmp = (mblk_t *) allocb(blk,BPRI_MED);
        if (dmp == NULL) {
            cmn_err(CE_WARN,"allocb() fail in xsend_dsp_msg()");
            return -1;
        } 

        dmp->b_datap->db_type = M_DATA;
        bzero(dmp->b_datap->db_base,blk);
        bcopy((caddr_t) (&(data[0])), (caddr_t) dmp->b_rptr, (size_t) blk);	
        dmp->b_wptr += blk;
    
        linkb(cmp,dmp);

        an_reply(genldp,cmp,0,0,0,ANDT_MSG);
    }   
    return(E_SUCC);
}

/*******************************************************************/
/*             OPERATING SYSTEM TRANSLATION FUNCTIONS              */
/*******************************************************************/

/****************************************************************
*        NAME : xx_an_open
* DESCRIPTION : Returns pointer to area determined by optional 
*               other operating systems device drivers.
*       INPUT : an_open parameters + an_open return handler
*      OUTPUT : 
*     RETURNS : pointer to developer data
*    CAUTIONS : 
****************************************************************/
ADDR *xx_an_open(SHORT rhandle,RCU *rcuid,SHORT oflags,/* an_open parameters */
                 PRC *proc,LONG *Attarray)
{
#ifndef _SCO_
    AN_LOGDEV *anldp;

    anldp = Gldp2Anldp(curr_ldp);
    anldp->gnhndl = curr_gnhndl;
#endif /* _SCO_ */

    rcu_cor[rhandle].ldp = curr_ldp; 
    rcu_cor[rhandle].gnhndl = curr_gnhndl; 
    rcu_cor[rhandle].rhandle = rhandle; 
    rcu_cor[rhandle].proc.proc_id = proc->proc_id;
    rcu_cor[rhandle].proc.dpi_id = proc->dpi_id;
    return((ADDR *) &(rcu_cor[rhandle]));
}

/****************************************************************
*        NAME : xx_bd_open
* DESCRIPTION : Returns pointer to area determined by optional 
*	        other operating systems device drivers.
*       INPUT : bd_open parameters + bd_open return handler
*      OUTPUT : 
*     RETURNS : pointer to developer data
*    CAUTIONS : 
****************************************************************/
ADDR *xx_bd_open(SHORT bhandle,SHORT bd_id,SHORT bdmode,SHORT far *bdstream, PRC *bdproc)
{
    bd_cor[bhandle].ldp = curr_ldp; 
    bd_cor[bhandle].gnhndl = curr_gnhndl; 
    bd_cor[bhandle].bd_id = bd_id; 
    bd_cor[bhandle].bdstream = *bdstream; 
    bd_cor[bhandle].bdhandle = bhandle; 
    bd_cor[bhandle].bdproc.proc_id = bdproc->proc_id;
    bd_cor[bhandle].bdproc.dpi_id = bdproc->dpi_id;
    return((ADDR *) &(bd_cor[bhandle]));
}

/****************************************************************
*        NAME : xx_an_close
* DESCRIPTION : Returns code determined and used by optional
*               other operating systems device drivers.
*       INPUT : RCU handler
*      OUTPUT : 
*     RETURNS : Return code.
*    CAUTIONS : 
****************************************************************/
SHORT xx_an_close(SHORT rhandle)
{
    rcu_cor[rhandle].ldp = (GN_LOGDEV *) NULL;
    rcu_cor[rhandle].rhandle = -1;
    rcu_cor[rhandle].proc.proc_id = 0;
    rcu_cor[rhandle].proc.dpi_id = SYSTEMDPI_ID;
    return(E_SUCC);
}

/****************************************************************
*        NAME : xx_bd_close
* DESCRIPTION : Returns code determined and used by optional
*               other operating systems device drivers.
*       INPUT : Bulk Data stream handler
*      OUTPUT : 
*     RETURNS : Return code.
*    CAUTIONS : 
****************************************************************/
SHORT xx_bd_close(SHORT bhandle)
{
    bd_cor[bhandle].ldp = (GN_LOGDEV *) NULL; 
    bd_cor[bhandle].gnhndl = 0; 
    bd_cor[bhandle].bd_id = -1; 
    bd_cor[bhandle].bdhandle = -1; 
    bd_cor[bhandle].bdstream = -1; 
    bd_cor[bhandle].bdproc.proc_id = 0;
    bd_cor[bhandle].bdproc.dpi_id = SYSTEMDPI_ID;

    return(E_SUCC);
}
