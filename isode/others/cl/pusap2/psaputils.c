

/*
 ****************************************************************
 *                                                              *
 *  ISODECL  -  HULA project for connectionless ISODE           *
 *                                             			*
 *  module:  	psaputils.c                                     *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements utility routines to support the        *
 *  P-UNIT-DATA service.                                        *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 * 	ppdu2data (pb, pi, ps, info)                            *
 * 	qb2_info (qb, pe)           				*
 *      info2_qb (pe, qp, pi)					*
 *      info2_ppdu (pb, pi, data, ndata, ppdu)			*
 * 	ss2pulose (pb, pi, event, sa)				*
 * 	pusaplose (va_alist)					*
 *      PuErrString (code)					*
 * 	PS_print (pe, text, rw, fnx)				*
 *	newpublk ()						*
 *	findpublk (sd)						*
 *	freepublk (sd)						*
 *                                                              *
 *  internal routines:                                          *
 *                                                              *
 *      _pusaplose (pi, reason, ap)                             *
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

/* HULA P-UNIT-DATA Service */
/* psaputils.c - PPM: provider utility routines */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/psap2/RCS/psapinitiate.c,v 5.0 88/07/21 14:42:56 mrose Rel $";
#endif

/* 
 * $Header: /f/iso/psap2/RCS/psapinitiate.c,v 5.0 88/07/21 14:42:56 mrose Rel $
 *
 *
 * $Log$
 */


/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include <signal.h>
#include "PS-types.h"
#include "pupkt.h"
#include "isoservent.h"
#include "tailor.h"

/* HULA UD - the entire module */

static 	int    once_only = 0;
static  struct psapblk   psapque;
static  struct psapblk   *PuHead = &psapque;


/*  */
/*----------------------------------------------------------------------------*/
int	ppdu2data (pb, pi, ps, info)
/*----------------------------------------------------------------------------*/
register struct psapblk *pb;
struct PSAPindication *pi;
struct PuSAPstart *ps;
struct type_PS_User__data *info;
{
    register int    i, j;
    int	    nctx, ctx, result;
    PE	    pe;
    register struct type_PS_Fully__encoded__data *full;
    PE     *data;
    int    *ndata;

    data = ps->ps_info;
    ndata = &ps->ps_ninfo;

    *ndata = 0;
    *data = NULLPE;
    if (info == NULL) return OK;

    i = 0;
    switch (info -> offset) {
	default:
	    return pusaplose ( pi, PC_INVALID, NULLCP,
				"neither Simply- nor Fully-encoded-data");
	case type_PS_User__data_simple:
	    if (pb -> pb_ncontext < 1 )
	 	ctx = PE_DFLT_CTX;
	    else
	 	{
		if (pb -> pb_ncontext > 1)
		    return pusaplose ( pi, PC_INVALID, NULLCP,
				     "unexpected Simply-encoded-data");
		else
		    ctx = pb -> pb_contexts[0].pc_id;
		}
	    while ((result = qb2_info (info -> un.simple, &pe)) == PS_ERR_NONE){
		if (i++ >= NPDATA) {
		    pe_free (pe);
		    return pusaplose ( pi, PC_CONGEST, NULLCP,
				     "too much user information");
		    }
		(*data++ = pe) -> pe_context = ctx;
	        }
	    if (result != PS_ERR_EOF)
		return pusaplose ( pi, result != PS_ERR_NMEM ? PC_INVALID
				 : PC_CONGEST, NULLCP, "%s", ps_error (result));
	    break;

	case type_PS_User__data_complex:
	    for (full = info -> un.complex; full; full = full -> next) {
		struct qbuf *qb;
		register struct PSAPcontext *qp;
		register struct type_PS_PDV__list *pdv = full -> PDV__list;

		ctx = pdv -> identifier;
		nctx = ps -> ps_ctxlist.pc_nctx;
		for (j = 0, qp = ps -> ps_ctxlist.pc_ctx; j < nctx; j++, qp++)
		    if (qp -> pc_id == ctx && qp -> pc_result == PC_ACCEPT)
			break;
		if (j >= nctx)
		    return pusaplose ( pi, PC_INVALID, NULLCP,
				     "unexpected use of context %d", ctx);
		switch (pdv -> presentation__data__values -> offset) {
		    case choice_PS_0_single__ASN1__type:
		        if (i++ >= NPDATA)
			    return pusaplose ( pi, PC_CONGEST, NULLCP,
					     "too much user information");
			pe = pdv -> presentation__data__values ->
			    	un.single__ASN1__type;
			pdv -> presentation__data__values ->
			   un.single__ASN1__type = NULLPE;
			(*data++ = pe) -> pe_context = ctx;
			break;

		    case choice_PS_0_octet__aligned:
			qb = pdv -> presentation__data__values ->
			    un.octet__aligned;
			while ((result = qb2_info (qb, &pe)) == PS_ERR_NONE) {
			    pe -> pe_context = ctx;
			    if (i++ >= NPDATA) {
				pe_free (pe);
				return pusaplose ( pi, PC_CONGEST, NULLCP,
						 "too much user information");
			        }
			    (*data++ = pe) -> pe_context = ctx;
			    }
			if (result != PS_ERR_EOF)
			    return pusaplose ( pi, result != PS_ERR_NMEM
					     ? PC_INVALID : PC_CONGEST,
					     NULLCP, "%s", ps_error (result));
			break;

		    default:
			return pusaplose ( pi, PC_INVALID, NULLCP,
					 "not expecting non-ASN.1 encoding");
		    }
	        }
	    break;
        }
    *ndata = i;
    return OK;
}


/*  */
/*----------------------------------------------------------------------------*/
int	qb2_info (qb, pe)
/*----------------------------------------------------------------------------*/
register struct qbuf *qb;
PE     *pe;
{
    int	    result;
#ifdef	DEBUG
    int	    len;
#endif
    PE	    p;
    register PS	    ps;

    *pe = NULLPE;

    if ((ps = ps_alloc (qbuf_open)) == NULLPS)
	return PS_ERR_NMEM;
#ifdef	DEBUG
    len = ps -> ps_byteno;
#endif
    if (qbuf_setup (ps, qb) == NOTOK || (p = ps2pe (ps)) == NULLPE) {
	if ((result = ps -> ps_errno) == PS_ERR_NONE)
	    result = PS_ERR_EOF;
    }
    else {
	result = PS_ERR_NONE;
	*pe = p;
    }

    ps -> ps_addr = NULL;	/* so ps_free doesn't free remainder of qbuf */
#ifdef	DEBUG
    len = ps -> ps_byteno - len;
#endif
    ps_free (ps);

#ifdef	DEBUG
    if (p && (psaplevel & ISODELOG_PDUS)) {
	int	isopen;
	FILE   *fp;

	if (strcmp (psapfile, "-")) {
	    char    file[BUFSIZ];

	    (void) sprintf (file, psapfile, getpid ());
	    fp = fopen (file, "a"), isopen = 1;
	}
	else
	    fp = stderr, isopen = 0, (void) fflush (stdout);

	if (fp) {
	    pe2text (fp, p, 1, len);
	    if (isopen)
		(void) fclose (fp);
	    else
		(void) fflush (fp);
	}
    }
#endif

    return result;
}


/*  */
/*----------------------------------------------------------------------------*/
struct qbuf *info2_qb (pe, qp, pi)
/*----------------------------------------------------------------------------*/
register PE pe;
register struct qbuf *qp;
struct PSAPindication *pi;
{
    int	    len;
    register struct qbuf *qb;
    register PS	    ps;

    if ((qb = qp) == NULL) {
	if ((qb = (struct qbuf *) malloc ((unsigned) sizeof *qb
					  + (len = ps_get_abs (pe))))
		== NULL) {
no_mem: ;
	    (void) pusaplose (pi, PC_CONGEST, NULLCP, NULLCP);
	    goto out;
	}	

	qb -> qb_data = qb -> qb_base, qb -> qb_len = len;
    }

    if ((ps = ps_alloc (str_open)) == NULLPS)
	goto no_mem;
    if (str_setup (ps, qb -> qb_data, qb -> qb_len, 1) == NOTOK
	    || pe2ps_aux (ps, pe, 0) == NOTOK) {
	(void) pusaplose (pi, PC_CONGEST, NULLCP, "error encoding user-info");
	ps_free (ps);
	goto out;
    }

    len = ps -> ps_ptr - ps -> ps_base;
    if (qp)
	qp -> qb_data += len, qp -> qb_len -= len;
    else
	qb -> qb_len = len;

#ifdef	DEBUG
    if (psaplevel & ISODELOG_PDUS) {
	int	isopen;
	FILE   *fp;

	if (strcmp (psapfile, "-")) {
	    char    file[BUFSIZ];

	    (void) sprintf (file, psapfile, getpid ());
	    fp = fopen (file, "a"), isopen = 1;
	}
	else
	    fp = stderr, isopen = 0, (void) fflush (stdout);

	if (fp) {
	    pe2text (fp, pe, 0, len);
	    if (isopen)
		(void) fclose (fp);
	    else
		(void) fflush (fp);
	}
    }
#endif

    ps_free (ps);

    return qb;

out: ;
    if (qb && qb != qp)
	free ((char *) qb);

    return NULL;
}

/*  */
/*----------------------------------------------------------------------------*/
struct type_PS_User__data *info2_ppdu (pb, pi, data, ndata, ppdu)
/*----------------------------------------------------------------------------*/
register struct psapblk *pb;
struct PSAPindication *pi;
PE     *data;
int	ndata,
	ppdu;
{
    register int    i,
		    j;
    register PE	   *d,
		    pe;
    register struct qbuf *qb;
    register struct PSAPcontext *qp;
    OID	    atn;
    struct type_PS_User__data *pdu;
    register struct type_PS_Simply__encoded__data *simple;
    register struct type_PS_Fully__encoded__data **complex,
						  *full;

    if ((pdu = (struct type_PS_User__data *) calloc (1, sizeof *pdu))
	    == NULL) {
no_mem: ;
	(void) pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	goto out;
    }

    pdu -> offset = type_PS_User__data_simple;
    for (d = data, i = 0; i < ndata; i++) {
	if ((pe = *d++) == NULLPE) {
	    (void) pusaplose (pi, PC_PARAMETER, NULLCP,
		    "missing %d%s PSDU", i + 1,
		    i == 0 ? "st" : i == 1 ? "nd" : i == 2 ? "rd" : "th");
	    goto out;
	    }

/*  for now and perhaps forever the choice of simply vs fully encoded */
/*  is the presence of more than one context since it is presumed that */
/*  AUDT is present and we don't want to force fully encoded just because */
/*  there is a context for the AUDT pdu */

	if (pb -> pb_ncontext > 1) {
	    if ( pe -> pe_context == PE_DFLT_CTX) {
	        (void) pusaplose (pi, PC_PARAMETER, NULLCP,
				 "default context not permitted");
		goto out;
	        }
	    pdu -> offset = type_PS_User__data_complex;
	    }
        }

    if (pdu -> offset == type_PS_User__data_simple) {
	if ((qb = (struct qbuf *) malloc (sizeof *qb)) == NULL)
	    goto no_mem;
	simple = pdu -> un.simple = qb;
	qb -> qb_forw = qb -> qb_back = qb;
	qb -> qb_data = NULL, qb -> qb_len = 0;

	j = 0;
	for (d = data, i = 0; i < ndata; i++)
	    j += ps_get_abs (*d++);
	qb -> qb_len = j;
	if ((qb = (struct qbuf *) malloc (sizeof *qb + ((unsigned)j))) == NULL)
	    goto no_mem;
	qb -> qb_data = qb -> qb_base, qb -> qb_len = j;
	insque (qb, simple -> qb_back);
    }
    else
	complex = &pdu -> un.complex;

    for (d = data, i = 0; i < ndata; i++) {
	pe = *d++;
	switch (pe -> pe_context) {
	    case PE_DFLT_CTX:
		atn = pb -> pb_atn;
		break;

	    default:
		for (j = 0, qp = pb -> pb_contexts;
			j < pb -> pb_ncontext;
			j++, qp++)
		    if (qp -> pc_id == pe -> pe_context)
			break;
		if (j >= pb -> pb_ncontext) {
		    (void) pusaplose (pi, PC_PARAMETER, NULLCP,
				"context %d is undefined", pe -> pe_context);
		    goto out;
		}
		if (qp -> pc_result != PC_ACCEPT) {
		    (void) pusaplose (pi, PC_PARAMETER, NULLCP,
				"context %d is unsupported", pe -> pe_context);
		    goto out;
		}
		atn = qp -> pc_atn;
		break;
	}

	if (!atn_is_asn1 (atn)) {
	    (void) pusaplose (pi, PC_PARAMETER, NULLCP, "not ASN.1");
	    goto out;
	}

	if (pdu -> offset == type_PS_User__data_simple) {
	    if (info2_qb (pe, qb, pi) == NULL)
		goto out;
	}
	else {
	    if ((full = (struct type_PS_Fully__encoded__data *)
		 	    calloc (1, sizeof *full)) == NULL)
		goto no_mem;
	    *complex = full;
	    complex = &full -> next;
	    if ((full -> PDV__list = (struct type_PS_PDV__list *)
				    calloc (1, sizeof *full -> PDV__list))
		    == NULL)
		goto no_mem;
	    full -> PDV__list -> identifier = pe -> pe_context;
	    if ((full -> PDV__list -> presentation__data__values =
		     (struct choice_PS_0 *)
		     	calloc (1, sizeof (struct choice_PS_0))) == NULL)
		goto no_mem;

	    if (ndata == 1) {
		full -> PDV__list -> presentation__data__values ->
		    offset = choice_PS_0_single__ASN1__type;
		(full -> PDV__list -> presentation__data__values ->
		    un.single__ASN1__type = pe) -> pe_refcnt++;
	    }
	    else {
		register struct qbuf *qb2;

		full -> PDV__list -> presentation__data__values ->
		    offset = choice_PS_0_octet__aligned;
		if ((qb2 = (struct qbuf *) malloc (sizeof *qb2)) == NULL)
		    goto no_mem;
		full -> PDV__list -> presentation__data__values ->
		    un.octet__aligned = qb2;
		qb2 -> qb_forw = qb2 -> qb_back = qb2;
		qb2 -> qb_data = NULL, qb2 -> qb_len = 0;
		if ((qb = info2_qb (pe, (struct qbuf *) NULL, pi)) == NULL)
		    goto out;
		qb2 -> qb_len = qb -> qb_len;
		insque (qb, qb2);
	    }
	}
    }

    return pdu;
    
out: ;
    if (pdu)
	free_PS_User__data (pdu);

    return NULL;
}



/*----------------------------------------------------------------------------*/
/*    SSAP interface */
/*----------------------------------------------------------------------------*/
int	ss2pulose (pb, pi, event, sa)
register struct psapblk *pb;
register struct PSAPindication *pi;
char   *event;
register struct SSAPabort *sa;
{
    int     reason;
    char   *cp,
            buffer[BUFSIZ];

    if ((psap2level & ISODELOG_EXCEPTIONS)
	    && event
	    && SC_FATAL (sa -> sa_reason))
	xsprintf (NULLCP, NULLCP,
		sa -> sa_cc > 0 ? "%s: %s\n\t%*.*s": "%s: %s", event,
		SuErrString (sa -> sa_reason), sa -> sa_cc, sa -> sa_cc,
		sa -> sa_data);

    cp = "";
    switch (sa -> sa_reason) {
	case SC_SSAPID: 
	case SC_SSUSER: 
	case SC_ADDRESS: 
	    reason = PC_ADDRESS;
	    break;

	case SC_REFUSED:
	    reason = PC_REFUSED;
	    break;

	case SC_CONGEST: 
	    reason = PC_CONGEST;
	    break;

	case SC_TRANSPORT:
	case SC_ABORT:
	    reason = PC_SESSION;
	    break;

	default: 
	    reason = PC_SESSION;
	    if (pb == NULLPB)
		switch (sa -> sa_reason) {
		    case SC_PARAMETER:
			reason = PC_PARAMETER;
			break;

		    case SC_OPERATION:
			reason = PC_OPERATION;
			break;

		    case SC_TIMER:
			reason = PC_TIMER;
			break;

		    case SC_WAITING:
			reason = PC_WAITING;
			break;
		}
	    (void) sprintf (cp = buffer, " (%s at session)",
			SuErrString (sa -> sa_reason));
	    break;
    }

    if (sa -> sa_cc > 0)
        return pusaplose (pi, reason, NULLCP, "%*.*s%s",
	        sa -> sa_cc, sa -> sa_cc, sa -> sa_data, cp);
    else
        return pusaplose (pi, reason, NULLCP, "%s",
	        *cp ? cp + 1 : cp);
}


/*  */

#ifndef	lint
/*----------------------------------------------------------------------------*/
int	pusaplose (va_alist)
/*----------------------------------------------------------------------------*/
va_dcl
{
    int     reason,
    	    result;
    struct PSAPindication *pi;
    va_list ap;

    va_start (ap);
    pi = va_arg (ap, struct PSAPindication *);
    reason = va_arg (ap, int);
    result = _pusaplose (pi, reason, ap);
    va_end (ap);
    return result;
}
#else
/* VARARGS */
int	pusaplose (pi, reason, what, fmt)
struct PSAPindication *pi;
int     reason;
char   *what,
       *fmt;
{
    return pusaplose (pi, reason, what, fmt);
}
#endif

#ifndef	lint
/*----------------------------------------------------------------------------*/
static int  _pusaplose (pi, reason, ap)	/* what, fmt, args ... */
/*----------------------------------------------------------------------------*/
register struct PSAPindication *pi;
int     reason;
va_list	ap;
{
    register char  *bp;
    char    buffer[BUFSIZ];
    register struct PSAPabort *pa;

    if (pi) {
	bzero ((char *) pi, sizeof *pi);
	pi -> pi_type = PI_ABORT;
	pa = &pi -> pi_abort;

	asprintf (bp = buffer, ap);
	bp += strlen (bp);

	pa -> pa_peer = 0;
	pa -> pa_reason = reason;
	pa -> pa_ninfo = 0;
	copyPSAPdata (buffer, bp - buffer, pa);
    }

    return NOTOK;
}
#endif


/*  */
/* stolen from psap2error.c */
/*----------------------------------------------------------------------------*/
static char *reject_err0[] = {
/*----------------------------------------------------------------------------*/
    "Rejected by peer",
    "Reason not specified",
    "Temporary congestion",
    "Local limit exceeded",
    "Called presentation address unknown",
    "Protocol version not supported",
    "Default context not supported",
    "User-data not readable",
    "No PSAP available",
    "Unrecognized PPDU",
    "Unexpected PPDU",
    "Unexpected session service primitive",
    "Unrecognized PPDU parameter",
    "Unexpected PPDU parameter",
    "Invalid PPDU parameter value",
    "Abstract syntax not supported",
    "Proposed transfer syntaxes not supported",
    "Local limit on DCS exceeded",
    "Connect request refused on this network connection",
    "Session disconnect",
    "Protocol error",
    "Peer aborted connection",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];


/*  */
/*---------------------------------------------------------------------------*/
char   *PuErrString (code)
/*---------------------------------------------------------------------------*/
register int code;
{
    static char buffer[BUFSIZ];

    if (code < reject_err0_cnt)
	return reject_err0[code];
    (void) sprintf (buffer, "unknown error code %d", code);
    return buffer;
}


/*   INTERNAL */
/*----------------------------------------------------------------------------*/
struct psapblk  *newpublk ()
/*----------------------------------------------------------------------------*/
{
    register struct psapblk *pb;

    pb = (struct psapblk   *) calloc (1, sizeof *pb);
    if (pb == NULL)
	return NULL;

    pb -> pb_fd = NOTOK;

    if (once_only == 0) {
	PuHead -> pb_forw = PuHead -> pb_back = PuHead;
	once_only++;
        }
    insque (pb, PuHead -> pb_back);
    return pb;
}


/*  */
/*----------------------------------------------------------------------------*/
struct psapblk   *findpublk (sd)
/*----------------------------------------------------------------------------*/
register int sd;
{
    register struct psapblk *pb;

    if (once_only == 0)
	return NULL;

    for (pb = PuHead -> pb_forw; pb != PuHead; pb = pb -> pb_forw)
	if (pb -> pb_fd == sd)
	    return pb;

    return NULL;
}


/*  */
/*----------------------------------------------------------------------------*/
int	freepublk (pb)
/*----------------------------------------------------------------------------*/
register struct psapblk *pb;
{
    register int    i;
    register struct PSAPcontext *qp;

    if (pb == NULL) return;

    if ( pb -> pb_calling ) free (pb -> pb_calling);
    if ( pb -> pb_called ) free (pb -> pb_called);

    if (pb -> pb_retry) free (pb -> pb_retry);

    for (qp = pb -> pb_contexts, i = pb -> pb_ncontext - 1;
	    i >= 0;
	    qp++, i--) {
	if (qp -> pc_asn)
	    oid_free (qp -> pc_asn);
	if (qp -> pc_atn)
	    oid_free (qp -> pc_atn);
        }

    if (pb -> pb_asn) oid_free (pb -> pb_asn);
    if (pb -> pb_atn) oid_free (pb -> pb_atn);


    remque (pb);
    free ((char *) pb);
}



/*  */
#ifdef	DEBUG
/*----------------------------------------------------------------------------*/
int	PS_print (pe, text, rw, fnx)
/*----------------------------------------------------------------------------*/
PE	pe;
char   *text;
int	rw;
IFP	fnx;
{
    int	    isopen;
    FILE   *fp;

    if (strcmp (psap2file, "-")) {
	char	file[BUFSIZ];

	(void) sprintf (file, psap2file, getpid ());
	fp = fopen (file, "a"), isopen = 1;
    }
    else
	fp = stderr, isopen = 0, (void) fflush (stdout);

    if (fp) {
	vpushfp (fp, pe, text, rw);
	(void) (*fnx) (pe, 1, NULLIP, NULLVP, NULLCP);
	vpopfp ();

	if (isopen)
	    (void) fclose (fp);
	else
	    (void) fflush (fp);
    }
}
#endif
