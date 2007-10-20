/* ppkt.h - include file for presentation providers (PS-PROVIDER) */

/* 
 * $Header: /f/iso/h/RCS/ppkt.h,v 5.0 88/07/21 14:39:10 mrose Rel $
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
 *
 */


#ifndef	_PSAP2_
#include "psap2.h"		/* definitions for PS-USERs */
#endif

#include "ssap.h"		/* definitinos for SS-USERs */

/*  */

#define	psapPsig(pb, sd) \
{ \
    if ((pb = findpblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "invalid presentation descriptor"); \
    } \
    if (!(pb -> pb_flags & PB_CONN)) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not connected"); \
    } \
    if (pb -> pb_flags & PB_FINN) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor finishing"); \
    } \
}

#define	psapFsig(pb, sd) \
{ \
    if ((pb = findpblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "invalid presentation descriptor"); \
    } \
    if (!(pb -> pb_flags & PB_CONN)) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not connected"); \
    } \
    if (!(pb -> pb_flags & PB_FINN)) { \
	(void) sigsetmask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not finishing"); \
    } \
}

#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "too many %s user data elements", p); \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
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


#define	pylose(p) \
	ppktlose (pb, pi, PC_UNRECOGNIZED, (p), NULLCP, "%s", PY_pepy)


#ifdef HULA
#define psaplose pusaplose
int	pusaplose ();
#endif

int	ppktlose (), psaplose ();


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

#ifdef HULA
#define PB_PUDT         0x20    /* p unit data service */

    struct PSAPaddr  *pb_calling; /* local user address */
    struct PSAPaddr  *pb_called;  /* remote user address */

#endif

    char   *pb_retry;		/* initial ppkt */

    int	    pb_ncontext;	/* presentation context set */
    struct PSAPcontext pb_contexts[NPCTX];

    OID	    pb_asn;		/* default: abstract syntax name */
    OID	    pb_atn;		/*   ..     abstract transfer name */
    int	    pb_result;		/* 	    response */
    
    int	    pb_prequirements;	/* presentation requirements */

    int	    pb_srequirements;	/* our session requirements */
    int	    pb_urequirements;	/* user's session requirements */
    int	    pb_owned;		/* session tokens we own */
    int	    pb_avail;		/* session tokens available */
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

int	freepblk ();
struct psapblk *newpblk (), *findpblk ();

#ifdef HULA
int	freepublk ();
struct psapblk *newpublk (), *findpublk ();
#endif

#define	PC_PROV_BASE		PC_NOTSPECIFIED
#define	PC_ABORT_BASE \
	(PC_UNRECOGNIZED - int_PS_Abort__reason_unrecognized__ppdu)
#define	PC_REASON_BASE \
	(PC_ABSTRACT - int_PS_provider__reason_abstract__syntax__not__supported)


struct type_PS_User__data *info2ppdu ();

#ifdef HULA
struct type_PS_User__data *info2_ppdu ();
#endif

int	ppdu2info ();
int	info2ssdu (), ssdu2info (), qbuf2info ();

struct qbuf *info2qb ();
int	qb2info ();

#ifdef HULA
struct qbuf *info2_qb ();
int	qb2_info ();
#endif


struct type_PS_Identifier__list *silly_list ();

int	ss2pslose (), ss2psabort ();

int	print_PS_CP__type (), print_PS_CPA__type (), print_PS_CPR__type (),
	print_PS_Abort__type (), print_PS_ARU__PPDU (), print_PS_ARP__PPDU (),
	print_PS_RS__PPDU (), print_PS_RSA__PPDU (), print_PS_User__data (),
	print_PS_Fully__encoded__data ();
int	vunknown ();


struct pair {
    int	    p_mask;
    int	    p_bitno;
};

extern struct pair preq_pairs[], sreq_pairs[];

/*  */

#define	REASON_BASE	PC_NOTSPECIFIED

#define	PPDU_NONE	(-1)
#define	PPDU_CP		0
#define	PPDU_CPA	1
#define	PPDU_CPR	2
#define	PPDU_ARU	3
#define	PPDU_ARP	4
#define	PPDU_TD		7
#define	PPDU_RS	       12
#define	PPDU_RSA       13
#ifdef HULA
#define	PPDU_UD        14
#endif
