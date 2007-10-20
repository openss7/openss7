/* psapselect.c - PPM: map descriptors */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap2/RCS/psapselect.c,v 9.0 1992/06/16 12:29:42 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap2/RCS/psapselect.c,v 9.0 1992/06/16 12:29:42 isode Rel $
 *
 *
 * $Log: psapselect.c,v $
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
#include <signal.h>
#include "ppkt.h"

/*    map presentation descriptors for select() */

int	PSelectMask (sd, mask, nfds, pi)
int	sd;
fd_set *mask;
int    *nfds;
struct PSAPindication *pi;
{
    SBV     smask;
    register struct psapblk *pb;
    struct SSAPindication   sis;
    register struct SSAPabort  *sa = &sis.si_abort;

    missingP (mask);
    missingP (nfds);
    missingP (pi);

    smask = sigioblock ();

    if ((pb = findpblk (sd)) == NULL) {
	(void) sigiomask (smask);
	return psaplose (pi, PC_PARAMETER, NULLCP,
			    "invalid presentation descriptor");
    }

    if (SSelectMask (pb -> pb_fd, mask, nfds, &sis) == NOTOK)
	switch (sa -> sa_reason) {
	    case SC_WAITING: 
		(void) sigiomask (smask);
		return psaplose (pi, PC_WAITING, NULLCP, NULLCP);

	    default: 
		(void) ss2pslose (pb, pi, "SSelectMask", sa);
		freepblk (pb);
		(void) sigiomask (smask);
		return NOTOK;
	}

    (void) sigiomask (smask);

    return OK;
}
