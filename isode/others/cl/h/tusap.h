/* tusap.h - include file for connectionless transport users (TS-USER) */

/* 
 *
 * Kurt Dobbins		3/89
 *
 * Added UNITDATA for HULA connectionless service.
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 */


#ifndef	_TuSAP_
#define	_TuSAP_			

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif
#ifndef	_ISOADDRS_
#include "isoaddrs.h"
#endif

/*  */


#ifdef HULA


struct TSAPunitdata {			/* T-UNITDATA.INDICATION */
    int     tud_sd;			/* TRANSPORT descriptor */

    struct TSAPaddr tud_calling;	/* address of peer calling */
    struct TSAPaddr tud_called;		/* address of peer called */
    struct QOStype  tud_qos;		/* quality of service */
    int	    	    tud_cc;		/*   total length */
    struct qbuf     tud_qbuf;		/*   chained data */
    char 	   *tud_base;		/* base ptr for free */
};
#define	TUDFREE(tud)	QBFREE (&((tud) -> tud_qbuf))

int	TUnitDataListen();	/* Listen on a datagram socket */
int	TUnitDataBind(); 	/* Bind socket to a remote address */
int	TUnitDataUnbind(); 	/* UnBind socket to a remote address */
int	TUnitDataRequest();	/* Unit Data write on unbound socket */
int 	TUnitDataWrite();	/* Write unit data on a bound socket */
int 	TUnitDataRead();	/* Read unit data on a bound socket */
int	TUnitDataWakeUp();	/* Sync wakeup routine on kill */
int     TuSave();		/* Save a unitdata from buffer */

int     T_UnitDataWrite();	/* ISO T_UNITDATA.write */
int     T_UnitDataRead();	/* ISO T_UNITDATA.read */

#endif

#endif
