/* psaplose.c - PPM: you lose */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap2/RCS/psaplose.c,v 9.0 1992/06/16 12:29:42 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap2/RCS/psaplose.c,v 9.0 1992/06/16 12:29:42 isode Rel $
 *
 *
 * $Log: psaplose.c,v $
 * Revision 9.0  1992/06/16  12:29:42  isode
 * Release 8.0
 *
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


/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include "PS-types.h"
#include "ppkt.h"
#include "tailor.h"

/*  */
#ifndef	lint
static int  _psaplose ();
#endif

#ifndef	lint
int	ppktlose (va_alist)
va_dcl
{
    int	    len,
	    ppdu,
    	    reason,
	    result;
    char   *base;
    register struct psapblk *pb;
    PE	    pe;
    register struct PSAPindication *pi;
    register struct PSAPabort *pa;
    struct SSAPindication   sis;
    struct type_PS_ARP__PPDU pdus;
    register struct type_PS_ARP__PPDU *pdu = &pdus;
    va_list ap;

    va_start (ap);

    pb = va_arg (ap, struct psapblk *);
    pi = va_arg (ap, struct PSAPindication *);
    reason = va_arg (ap, int);
    ppdu = va_arg (ap, int);

    result = _psaplose (pi, reason, ap);

    va_end (ap);

    if ((pa = &pi -> pi_abort) -> pa_cc > 0) {
	SLOG (psap2_log, LLOG_EXCEPTIONS, NULLCP,
	      ("ppktlose [%s] %*.*s", PErrString (pa -> pa_reason),
	       pa -> pa_cc, pa -> pa_cc, pa -> pa_data));
    }
    else
	SLOG (psap2_log, LLOG_EXCEPTIONS, NULLCP,
	      ("ppktlose [%s]", PErrString (pa -> pa_reason)));

    if (pb -> pb_fd == NOTOK)
	return result;

    switch (reason) {
	case PC_NOTSPECIFIED:
	default: 
	    reason = int_PS_Abort__reason_reason__not__specified;
	    break;

	case PC_UNRECOGNIZED:
	case PC_UNEXPECTED:
	case PC_SSPRIMITIVE:
	case PC_PPPARAM1:
	case PC_PPPARAM2:
	case PC_INVALID:
	    reason -= PC_ABORT_BASE;
	    break;
    }

    pdu -> provider__reason = (struct type_PS_Abort__reason *) &reason;
    pdu -> event = ppdu != PPDU_NONE
				? (struct type_PS_Event__identifier *) &ppdu
				: NULL;

    pe = NULLPE;
    base = NULL, len = 0;
    if (encode_PS_ARP__PPDU (&pe, 1, 0, NULLCP, pdu) == NOTOK) {
	PLOGP (psap2_log,PS_ARP__PPDU, pe, "ARP-PPDU", 0);

	(void) pe2ssdu (pe, &base, &len);
    }
    if (pe)
	pe_free (pe);

    if (SUAbortRequest (pb -> pb_fd, base, len, &sis) != NOTOK)
	pb -> pb_fd = NOTOK;

    if (base)
	free (base);

    return result;
}
#else
/* VARARGS6 */

int	ppktlose (pb, pi, reason, ppdu, what, fmt)
register struct psapblk *pb;
register struct PSAPindication *pi;
int	reason,
	ppdu;
char   *what,
       *fmt;
{
    return ppktlose (pb, pi, reason, ppdu, what, fmt);
}
#endif

/*  */

#ifndef	lint
int	psaplose (va_alist)
va_dcl
{
    int     reason,
    	    result;
    struct PSAPindication *pi;
    va_list ap;

    va_start (ap);

    pi = va_arg (ap, struct PSAPindication *);
    reason = va_arg (ap, int);

    result = _psaplose (pi, reason, ap);

    va_end (ap);

    return result;
}
#else
/* VARARGS4 */

int	psaplose (pi, reason, what, fmt)
struct PSAPindication *pi;
int     reason;
char   *what,
       *fmt;
{
    return psaplose (pi, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int  _psaplose (pi, reason, ap)	/* what, fmt, args ... */
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
