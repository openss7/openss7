/* acsapfindpci.c - find PCI for ACSE */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/acsapfindpci.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/acsapfindpci.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: acsapfindpci.c,v $
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#define	ACSE
#include "acpkt.h"

/*  */

int	AcFindPCI (sd, pci, aci)
int	sd;
int    *pci;
struct AcSAPindication *aci;
{
    SBV     smask;
    register struct assocblk  *acb;

    missingP (pci);
    missingP (aci);

    smask = sigioblock ();

    if ((acb = findacblk (sd)) == NULL) {
	(void) sigiomask (smask);
	return acsaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
    }

    *pci = acb -> acb_id;
    
    (void) sigiomask (smask);

    return OK;
}
