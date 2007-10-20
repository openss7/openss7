/* ftamselect.c - FPM: map descriptors */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ftam/RCS/ftamselect.c,v 9.0 1992/06/16 12:14:55 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ftam/RCS/ftamselect.c,v 9.0 1992/06/16 12:14:55 isode Rel $
 *
 *
 * $Log: ftamselect.c,v $
 * Revision 9.0  1992/06/16  12:14:55  isode
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
#include "fpkt.h"

/*    map ftam descriptors for select() */

int	FSelectMask (sd, mask, nfds, fti)
int	sd;
fd_set *mask;
int    *nfds;
struct FTAMindication *fti;
{
    SBV     smask;
    register struct ftamblk *fsb;
    struct PSAPindication   pis;
    register struct PSAPabort  *pa = &pis.pi_abort;

    missingP (mask);
    missingP (nfds);
    missingP (fti);

    smask = sigioblock ();

    ftamPsig (fsb, sd);

    if (fsb -> fsb_data.px_ninfo > 0)
	goto waiting;
    if (PSelectMask (fsb -> fsb_fd, mask, nfds, &pis) == NOTOK)
	switch (pa -> pa_reason) {
	    case PC_WAITING: 
waiting: ;
		(void) sigiomask (smask);
		return ftamlose (fti, FS_GEN_WAITING, 0, NULLCP, NULLCP);

	    default: 
		(void) ps2ftamlose (fsb, fti, "PSelectMask", pa);
		freefsblk (fsb);
		(void) sigiomask (smask);
		return NOTOK;
	}

    (void) sigiomask (smask);

    return OK;
}
