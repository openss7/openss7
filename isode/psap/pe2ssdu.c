/* pe2ssdu.c - write a PE to a SSDU */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/pe2ssdu.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/pe2ssdu.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: pe2ssdu.c,v $
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"
#include "tailor.h"

/*  */

int	pe2ssdu (pe, base, len)
register PE pe;
char  **base;
int    *len;
{
    register int plen, ret;

    *len = 0;
    plen  = ps_get_abs (pe);
    Qcp = (char *)malloc((unsigned)plen);
    *base = Qcp;

    if (Qcp == NULL)
        return NOTOK;

    Len = 0;
    Ecp = Qcp + plen;
    if ((ret = pe2qb_f(pe)) != plen) {
        (void) printf("pe2ssdu: bad length returned %d should be %d\n",
                ret, plen);
	return NOTOK;
    }
    *len = plen;

#ifdef	DEBUG
    if (psap_log -> ll_events & LLOG_PDUS)
	pe2text (psap_log, pe, 0, *len);
#endif

    return OK;
}
