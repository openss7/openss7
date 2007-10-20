
/*
 ****************************************************************
 *                                                              *
 *  HULA project - connectionless ISODE                         *
 *                                             			*
 *  module:  	acsaputils.c                                    *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements utility routines to support ACSE for   *
 *  A-UNIT-DATA service.                                        *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *      info2_apdu (acb, aci, data, ndata)                      *
 * 	apdu2_info (acb, aci, info, data, ndata)                *
 * 	ACU_print (pe, text, rw)                                *
 *      newacublk ()                                            *
 *      findacublk (sd)                                         *
 *      freeacublk (acb)                                        *
 * 	ps2aculose (acb, aci, event, pa)                        *
 * 	acusaplose (va_alist)                                   * 
 *      AcuErrString (code)                                     *
 *                                                              *
 *  internal routines:                                          *
 *                                                              *
 *      _acusaplose (aci, reason, ap)                           *
 *								*
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*    
 ****************************************************************
 */

/* modified from ISODE's acsaprovider.c, acsaplose.c and acsap2error.c */

/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include "ACS-types.h"
#define	ACSE
#include "acupkt.h"
#include "acusap.h"
#include "isoservent.h"
#include "tailor.h"


static int  once_only = 0;
static struct assocblk assocque;
static struct assocblk *AcuHead = &assocque;


/*    INTERNAL */
/* ARGSUSED */

/*---------------------------------------------------------------------------*/
struct type_ACS_Association__information *info2_apdu (acb, aci, data, ndata)
/*---------------------------------------------------------------------------*/
struct assocblk *acb;
struct AcSAPindication *aci;
PE     *data;
int	ndata;
{
    register PE	    pe;
    struct type_ACS_Association__information *info;
    register struct type_ACS_Association__information **pp,
						       *p;
    register struct type_UNIV_EXTERNAL *q;

    for (pp = &info; ndata-- > 0; pp = &p -> next) {
	if ((*pp = p = (struct type_ACS_Association__information *)
			calloc (1, sizeof *p)) == NULL
	        || (p -> EXTERNAL = (struct type_UNIV_EXTERNAL *)
			    calloc (1, sizeof *q)) == NULL
	        || (p -> EXTERNAL -> encoding = (struct choice_UNIV_0 *)
			malloc (sizeof (struct choice_UNIV_0))) == NULL)
	    goto out;
	q = p -> EXTERNAL;

	q -> indirect__reference = (pe = *data++) -> pe_context;
	q -> encoding -> offset = choice_UNIV_0_single__ASN1__type;
	(q -> encoding -> un.single__ASN1__type = pe) -> pe_refcnt++;
    }
    (*pp) = NULL;

    return info;

out: ;
    free_ACS_Association__information (info);
    (void) acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
    return NULL;
}


/*  */
/* ARGSUSED */

/*---------------------------------------------------------------------------*/
int	apdu2_info (acb, aci, info, data, ndata)
/*---------------------------------------------------------------------------*/
struct assocblk *acb;
struct AcSAPindication *aci;
struct type_ACS_Association__information *info;
PE     *data;
int    *ndata;
{
    register int    i;
    register PE	    pe;
    register struct type_UNIV_EXTERNAL *q;

    for (i = 0; info; info = info -> next, i++) {
	if (i > NACDATA)
	    return acusaplose ( aci, ACS_CONGEST, NULLCP,
			      "too much user information");

	q = info -> EXTERNAL;
	if (q -> encoding -> offset != choice_UNIV_0_single__ASN1__type)
	    return acusaplose ( aci, ACS_PROTOCOL, NULLCP,
			      "EXTERNAL data not single-ASN1-type");

	(pe = q -> encoding -> un.single__ASN1__type) -> pe_refcnt++;
	pe -> pe_context = q -> indirect__reference;
	
	*data++ = pe;
    }
    *ndata = i;
    return OK;
}


/*  */
#ifdef	DEBUG

/*---------------------------------------------------------------------------*/
int	ACU_print (pe, text, rw)
/*---------------------------------------------------------------------------*/
PE	pe;
char   *text;
int	rw;
{
    int	    isopen;
    FILE   *fp;

    if (strcmp (acsapfile, "-")) {
	char	file[BUFSIZ];

	(void) sprintf (file, acsapfile, getpid ());
	fp = fopen (file, "a"), isopen = 1;
    }
    else
	fp = stderr, isopen = 0, (void) fflush (stdout);

    if (fp) {
	vpushfp (fp, pe, text, rw);
	(void) print_ACS_ACSE__apdu (pe, 1, NULLIP, NULLVP, NULLCP);
	vpopfp ();

	if (isopen)
	    (void) fclose (fp);
	else
	    (void) fflush (fp);
    }
}
#endif


/*    ASSOCIATION BLOCKS */
/*---------------------------------------------------------------------------*/
struct assocblk  *newacublk () {
/*---------------------------------------------------------------------------*/
    register struct assocblk *acb;

    acb = (struct assocblk   *) calloc (1, sizeof *acb);
    if (acb == NULL)
	return NULL;

    acb -> acb_fd = NOTOK;

    if (once_only == 0) {
	AcuHead -> acb_forw = AcuHead -> acb_back = AcuHead;
	once_only++;
        }
    insque (acb, AcuHead -> acb_back);
    return acb;
}


/*  */
/*---------------------------------------------------------------------------*/
struct assocblk   *findacublk (sd)
/*---------------------------------------------------------------------------*/
register int sd;
{
    register struct assocblk *acb;

    if (once_only == 0 || sd == NOTOK)
	return NULL;

    for (acb = AcuHead -> acb_forw; acb != AcuHead; acb = acb -> acb_forw)
	if (acb -> acb_fd == sd)
	    return acb;
    return NULL;
}


/*  */
/*---------------------------------------------------------------------------*/
freeacublk (acb)
/*---------------------------------------------------------------------------*/
register struct assocblk *acb;
{
    if (acb == NULL) return;

    if (acb -> acb_context)
	oid_free (acb -> acb_context);

    if (acb -> acb_audtpci)
	oid_free (acb -> acb_audtpci);

    if (acb -> acb_apdu)
	pe_free (acb -> acb_apdu);

    if ( acb -> acb_callingtitle ) {
        AEIFREE ( acb -> acb_callingtitle ); 
        free ( acb -> acb_callingtitle );  
	}
    if ( acb -> acb_calledtitle ) {
        AEIFREE ( acb -> acb_calledtitle ); 
        free ( acb -> acb_calledtitle );  
        }
    remque (acb);
    free ((char *) acb);
}



/*---------------------------------------------------------------------------*/
/*    PSAP interface */
/*---------------------------------------------------------------------------*/
int	ps2aculose (acb, aci, event, pa)
/*---------------------------------------------------------------------------*/
register struct assocblk *acb;
register struct AcSAPindication *aci;
char   *event;
register struct PSAPabort *pa;
{
    int     reason;
    char   *cp,
            buffer[BUFSIZ];

    if ((acsaplevel & ISODELOG_EXCEPTIONS) && event)
	xsprintf (NULLCP, NULLCP,
		pa -> pa_cc > 0 ? "%s: %s\n\t%*.*s": "%s: %s", event,
		PuErrString (pa -> pa_reason), pa -> pa_cc, pa -> pa_cc,
		pa -> pa_data);

    cp = "";
    switch (pa -> pa_reason) {
	case PC_ADDRESS: 
	    reason = ACS_ADDRESS;
	    break;

	case PC_REFUSED:
	    reason = ACS_REFUSED;
	    break;

	case PC_CONGEST: 
	    reason = ACS_CONGEST;
	    break;

	case PC_PARAMETER:
	    reason = ACS_PARAMETER;
	    break;
	    
	case PC_OPERATION:
	    reason = ACS_OPERATION;
	    break;

	default: 
	    (void) sprintf (cp = buffer, " (%s at presentation)",
			PuErrString (pa -> pa_reason));
	case PC_SESSION:
	    reason = ACS_PRESENTATION;
	    break;
    }

    if (pa -> pa_cc > 0)
        return acusaplose (aci, reason, NULLCP, "%*.*s%s",
	        pa -> pa_cc, pa -> pa_cc, pa -> pa_data, cp);
    else
        return acusaplose (aci, reason, NULLCP, "%s",
	        *cp ? cp + 1 : cp);
}



/*  */
#ifndef	lint

/*---------------------------------------------------------------------------*/
int	acusaplose (va_alist)
/*---------------------------------------------------------------------------*/
va_dcl
{
    int	    reason,
	    result;
    struct AcSAPindication *aci;
    va_list ap;

    va_start (ap);
    aci = va_arg (ap, struct AcSAPindication *);
    reason = va_arg (ap, int);
    result = _acusaplose (aci, reason, ap);
    va_end (ap);
    return result;
}
#else
/* VARARGS */
int	acusaplose (aci, reason, what, fmt)
struct AcSAPindication *aci;
int	reason;
char   *what,
       *fmt;
{
    return acusaplose (aci, reason, what, fmt);
}
#endif



/*  */
#ifndef	lint

/*---------------------------------------------------------------------------*/
static int  _acusaplose (aci, reason, ap)  /* what, fmt, args ... */
/*---------------------------------------------------------------------------*/
register struct AcSAPindication *aci;
int     reason;
va_list	ap;
{
    register char  *bp;
    char    buffer[BUFSIZ];
    register struct AcSAPabort *aca;

    if (aci) {
	bzero ((char *) aci, sizeof *aci);
	aci -> aci_type = ACI_ABORT;
	aca = &aci -> aci_abort;

	asprintf (bp = buffer, ap);
	bp += strlen (bp);

	aca -> aca_source = ACA_LOCAL;
	aca -> aca_reason = reason;
	copyAcSAPdata (buffer, bp - buffer, aca);
    }

    return NOTOK;
}
#endif



/*  */
/* stolen from acsap2error.c - return AcuSAP error code in string form */

/*---------------------------------------------------------------------------*/
static char *reject_err0[] = {
/*---------------------------------------------------------------------------*/
    "unknown error code 0",
    "Permanent",
    "Transient",
    "Rejected by service-user: null",
    "Rejected by service-user: no reason given",
    "Application context name not supported",
    "Calling AP title not recognized",
    "Calling AP invocation-ID not recognized",
    "Calling AE qualifier not recognized",
    "Calling AE invocation-ID not recognized",
    "Called AP title not recognized",
    "Called AP invocation-ID not recognized",
    "Called AE qualifier not recognized",
    "Called AE invocation-ID not recognized",
    "Rejected by service-provider: null",
    "Rejected by service-provider: no reason given",
    "No common acse version",
    "Address unknown",
    "Connect request refused on this network connection",
    "Local limit exceeded",
    "Presentation disconnect",
    "Protocol error",
    "Peer aborted association",
    "Invalid parameter",
    "Invalid operation"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];


/*  */
/*---------------------------------------------------------------------------*/
char   *AcuErrString (code)
/*---------------------------------------------------------------------------*/
register int code;
{
    static char buffer[BUFSIZ];

    if (code < reject_err0_cnt)
	return reject_err0[code];

    (void) sprintf (buffer, "unknown error code %d", code);
    return buffer;
}
