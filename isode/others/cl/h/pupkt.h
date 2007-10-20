/* HULA */
/* pupkt.h - include file for Unit Data presentation providers (PS-PROVIDER) */
/* subject to ISODE license agreement */


#ifndef	_PUSAP2_
#include "pusap2.h"		/* definitions for PS-USERs */
#endif

#include "ssap.h"		/* definitinos for SS-USERs */
#include "susap.h"		/* definitinos for SS-USERs */

/*  */


#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return pusaplose (pi, PC_PARAMETER, NULLCP, \
			    "too many %s user data elements", p); \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return pusaplose (pi, PC_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}

#ifndef	lint
#ifndef	__STDC__
#define	copyPSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/* */_cc = min (i, sizeof d -> d/* */_data)) > 0) \
	bcopy (base, d -> d/* */_data, d -> d/* */_cc); \
}
#else
#define	copyPSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyPSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

int	pusaplose ();


/*  */

#define	DFLT_ASN	"iso asn.1 abstract syntax"
#define	DFLT_ATN	"iso asn.1 abstract transfer"


#define	atn_is_ok(pb,atn)	((atn) ? 1 : 0) /* it's all ISO8825, right? */
#define	atn_is_asn1(atn)	((atn) ? 1 : 0)	/*   .. */


struct psapblk {
    struct psapblk *pb_forw;	/* doubly-linked list */
    struct psapblk *pb_back;	/*   .. */

    int	    pb_fd;		/* session descriptor */

    short   pb_flags;		/* our state */
#define	PB_NULL		0x00
#define	PB_CONN		0x01	/* connected */
#define	PB_FINN		0x02	/* other side wants to finish */
#define	PB_ASYN		0x04	/* asynchronous */
#define	PB_DFLT		0x10	/* respond with default context result */

#define PB_PUDT         0x20    /* p unit data service */

    struct PSAPaddr  *pb_calling; /* local user address */
    struct PSAPaddr  *pb_called;  /* remote user address */

    char   *pb_retry;		/* initial ppkt */

    int	    pb_ncontext;	/* presentation context set */
    struct PSAPcontext pb_contexts[NPCTX];

    OID	    pb_asn;		/* default: abstract syntax name */
    OID	    pb_atn;		/*   ..     abstract transfer name */
    int	    pb_result;		/* 	    response */
    
    int	    pb_ssdusize;	/* largest atomic SSDU */

    struct PSAPaddr pb_responding;	/* responder */

    IFP	    pb_DataIndication;		/* INDICATION handlers */
    IFP	    pb_TokenIndication;		/*   .. */
    IFP	    pb_SyncIndication;		/*   .. */
    IFP	    pb_ActivityIndication;	/*   .. */
    IFP	    pb_ReportIndication;	/*   .. */
    IFP	    pb_ReleaseIndication;	/*   .. */
    IFP	    pb_AbortIndication;		/*   .. */

};
#define	NULLPB		((struct psapblk *) 0)

int	freepublk ();
struct psapblk *newpublk (), *findpublk ();

#define	PC_PROV_BASE		PC_NOTSPECIFIED
#define	PC_ABORT_BASE \
	(PC_UNRECOGNIZED - int_PS_Abort__reason_unrecognized__ppdu)
#define	PC_REASON_BASE \
	(PC_ABSTRACT - int_PS_provider__reason_abstract__syntax__not__supported)

int     ppdu2data ();
struct qbuf *info2_qb ();
int	qb2_info ();
struct type_PS_User__data *info2_ppdu ();

int	print_PS_UD__type (), print_PS_User__data (),
	print_PS_Fully__encoded__data ();
int	vunknown ();



/*  */

#define	REASON_BASE	PC_NOTSPECIFIED
#define	PPDU_UD        14
