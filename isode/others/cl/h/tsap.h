/* tsap.h - include file for transport users (TS-USER) */

/* 
 * $Header: /f/iso/h/RCS/tsap.h,v 5.0 88/07/21 14:39:41 mrose Rel $
 *
 *
 * $Log$
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 */


#ifndef	_TSAP_
#define	_TSAP_			

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

struct TSAPstart {		/* T-CONNECT.INDICATON */
    int     ts_sd;		/* TRANSPORT descriptor */

    struct TSAPaddr ts_calling;	/* address of peer calling */
    struct TSAPaddr ts_called;	/* address of peer called */

    int     ts_expedited;	/* EXPEDITED DATA ok */

    int	    ts_tsdusize;	/* largest atomic TSDU */

    struct QOStype ts_qos;	/* quality of service */

				/* initial DATA from peer */
#define	TS_SIZE		32
    int	    ts_cc;		/*   length */
    char    ts_data[TS_SIZE];	/*   data */
};


struct TSAPconnect {		/* T-CONNECT.CONFIRMATION */
    int     tc_sd;		/* TRANSPORT descriptor */

    struct TSAPaddr tc_responding;/* address of peer responding */

    int     tc_expedited;	/* EXPEDITED DATA ok */

    int	    tc_tsdusize;	/* largest atomic TSDU */

    struct QOStype tc_qos;	/* quality of service */

				/* initial DATA from peer */
#define	TC_SIZE		32
    int	    tc_cc;		/*   length */
    char    tc_data[TC_SIZE];	/*   data */
};


struct TSAPdata {		/* T-READ.INDICATION */
    int     tx_expedited;

				/* DATA from peer */
#define	TX_SIZE		16	/* EXPEDITED DATA only */
    int	    tx_cc;		/*   total length */
    struct qbuf tx_qbuf;	/*   chained data */
};
#define	TXFREE(tx)	QBFREE (&((tx) -> tx_qbuf))


struct TSAPdisconnect {		/* T-DISCONNECT.INDICATION */
    int     td_reason;		/* reason for DISCONNECT, from ISO8072: */
#define	DR_BASE		0x80
#define	DR_NORMAL	(DR_BASE + 0)	/* NORMAL disconnect by SESSION
					   entity */
#define	DR_REMOTE	(DR_BASE + 1)	/* Remote TRANSPORT entity congested at
					   connect request time */
#define	DR_CONNECT	(DR_BASE + 2)	/* Connection negotiation failed */
#define	DR_DUPLICATE	(DR_BASE + 3)	/* Duplicate source reference detected
				           for the same pair of NSAPs */
#define	DR_MISMATCH	(DR_BASE + 4)	/* Mismatched references */
#define	DR_PROTOCOL	(DR_BASE + 5)	/* Protocol error */
#define	DR_OVERFLOW	(DR_BASE + 7)	/* Reference overflow */
#define	DR_REFUSED	(DR_BASE + 8)	/* Connect request refused on this
				           network connection */
#define	DR_LENGTH	(DR_BASE + 10)	/* Header or parameter length
					   invalid */

					/* begin UNOFFICIAL */
#define	DR_NETWORK	(DR_BASE + 11)	/* Network disconnect */
#define	DR_PARAMETER	(DR_BASE + 12)	/* Invalid parameter */
#define	DR_OPERATION	(DR_BASE + 13)	/* Invalid operation */
#define	DR_TIMER	(DR_BASE + 14)	/* Timer expired */
#define	DR_WAITING	(DR_BASE + 15)	/* Indications waiting */
					/* end UNOFFICIAL */

#define	DR_UNKNOWN	0		/* Reason not specifed */
#define	DR_CONGEST	1		/* Congestion at TSAP */
#define	DR_SESSION	2		/* Session entity not attached to
					   TSAP */
#define	DR_ADDRESS	3		/* Address unknown */

#ifdef	notdef
#define	DR_FATAL(r)	((r) < DR_BASE || (r) < DR_PARAMETER)
#define	DR_OFFICIAL(r)	((r) < DR_BASE || (r) < DR_NETWORK)
#else
#define	DR_FATAL(r)	((r) < DR_PARAMETER)
#define	DR_OFFICIAL(r)	((r) < DR_NETWORK)
#endif

				/* disconnect DATA from peer */
#define	TD_SIZE		64
    int	    td_cc;		/*   length */
    char    td_data[TD_SIZE];	/*   data */
};

/*  */

extern char *tsapversion;


#ifdef	TCP
int	T_TCP_Exec ();		/* SERVER only */
#endif
#ifdef	X25
int	T_X25_Exec ();		/*   .. */
#endif
#ifdef	BRIDGE_X25
int	T_Bridge_Exec ();		/*   .. */
#endif
#ifdef	TP4
int	T_TP4_Exec ();			/*   .. */
#endif
int	TInit ();		/* T-CONNECT.INDICATION */

int	TConnResponse ();	/* T-CONNECT.RESPONSE */
				/* T-CONNECT.REQUEST (backwards-compatible) */
#define	TConnRequest(a1,a2,a3,a4,a5,a6,a7,a8) \
	TAsynConnRequest(a1,a2,a3,a4,a5,a6,a7,a8,0)
int	TAsynConnRequest ();	/* T-(ASYN-)CONNECT.REQUEST */
int	TAsynRetryRequest ();	/* T-ASYN-RETRY.REQUEST (pseudo) */
int	TDataRequest ();	/* T-DATA.REQUEST */
int	TWriteRequest ();	/* T-WRITE.REQUEST (pseudo) */
int	TExpdRequest ();	/* T-EXPEDITED-DATA.REQUEST */
int	TReadRequest ();	/* T-READ.REQUEST (pseudo) */
int	TDiscRequest ();	/* T-DISCONNECT.REQUEST */

int	TSetIndications ();	/* define vectors for INDICATION events */
int	TSelectMask ();		/* map transport descriptors for select() */

char   *TErrString ();		/* return TSAP error code in string form */

int	TNetListen ();		/* start listenting on an TSAP */
int	TNetUnique ();		/* start listenting on a set of unique TSAPs */
int	TNetAccept ();		/* accept a call on an TSAP */
int	TNetClose ();		/* stop listening on an TSAP */

#define	TLocalHostName	getlocalhost
char   *TLocalHostName ();	/* return name of local host (sigh) */
#endif
