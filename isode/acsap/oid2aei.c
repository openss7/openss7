/* oid2aei.c - application entity titles -- OID to AE info  */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/oid2aei.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/oid2aei.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: oid2aei.c,v $
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
#include "psap.h"
#include "isoaddrs.h"

/*  */

AEI	oid2aei (oid)
OID	oid;
{
    static AEInfo aeinfo;
    AEI	    aei = &aeinfo;
    static PE pe = NULLPE;

    if (pe)
	pe_free (pe);

    bzero ((char *) aei, sizeof *aei);
    aei -> aei_ap_title = pe = obj2prim (oid, PE_CLASS_UNIV, PE_PRIM_OID);

    return aei;
}
