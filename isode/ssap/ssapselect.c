/* ssapselect.c - SPM: map descriptors */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/ssapselect.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ssap/RCS/ssapselect.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: ssapselect.c,v $
 * Revision 9.0  1992/06/16  12:39:41  isode
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
#include <signal.h>
#include "spkt.h"

/*    map session descriptors for select() */

int	SSelectMask (sd, mask, nfds, si)
int	sd;
fd_set *mask;
int    *nfds;
register struct SSAPindication *si;
{
    SBV	    smask;
    int     result;
    register struct ssapblk *sb;
    struct TSAPdisconnect   tds;
    register struct TSAPdisconnect *td = &tds;

    missingP (mask);
    missingP (nfds);

    smask = sigioblock ();

    if ((sb = findsblk (sd)) == NULL) {
	(void) sigiomask (smask);
	return ssaplose (si, SC_PARAMETER, NULLCP,
			    "invalid session descriptor");
    }

    if ((result = TSelectMask (sb -> sb_fd, mask, nfds, td)) == NOTOK)
	if (td -> td_reason == DR_WAITING)
	    (void) ssaplose (si, SC_WAITING, NULLCP, NULLCP);
	else
	    (void) ts2sslose (si, "TSelectMask", td);

    (void) sigiomask (smask);

    return result;
}
