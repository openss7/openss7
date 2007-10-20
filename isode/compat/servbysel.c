/* servbysel.c - getisoserventbyselector */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/servbysel.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/servbysel.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: servbysel.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "isoservent.h"
#include "tailor.h"

/*  */

struct isoservent *getisoserventbyselector (provider, selector, selectlen)
char   *provider,
       *selector;
int	selectlen;
{
    register struct isoservent *is;

    isodetailor (NULLCP, 0);
    DLOG (addr_log, LLOG_TRACE,
	   ("getisoserventbyselector \"%s\" %s",
	    provider, sel2str (selector, selectlen, 1)));

    (void) setisoservent (0);
    while (is = getisoservent ())
	if (selectlen == is -> is_selectlen
		&& bcmp (selector, is -> is_selector, is -> is_selectlen) == 0
		&& strcmp (provider, is -> is_provider) == 0)
	    break;
    (void) endisoservent ();

    if (is) {
#ifdef	DEBUG
	if (addr_log -> ll_events & LLOG_DEBUG)
	    _printsrv (is);
#endif
    }
    else
	SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
	      ("lookup of local service %s %s failed",
	       provider, sel2str (selector, selectlen, 1)));

    return is;
}
