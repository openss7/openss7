/* aetseq.c - application entity titles -- sequential lookup */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/aetseq.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/aetseq.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: aetseq.c,v $
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

/*    DATA */

static char objent[BUFSIZ];
static struct isoentity ies;

/*  */

int	str2aet_seq (designator, qualifier, iep)
char   *designator,
       *qualifier;
struct isoentity *iep;
{
    int     hitdes,
	    hitqual;
    char    descriptor[BUFSIZ],
	    desdflt[BUFSIZ],
	    qualdflt[BUFSIZ];
    register struct isoentity  *ie;
    struct isoentity ids,
		     iqs;

    (void) sprintf (objent, "%s-%s", designator, qualifier);
    (void) sprintf (desdflt, "%s-%s", designator, "default");
    (void) sprintf (qualdflt, "%s-%s", "default", qualifier);
    hitdes = hitqual = 0;
    bzero ((char *) &ids, sizeof ids);
    bzero ((char *) &iqs, sizeof iqs);

    ie = NULL;

    if (!setisoentity (0))
	return NOTOK;
    while (_startisoentity (descriptor) == OK) {
	if (strcmp (descriptor, objent) == 0) {
	    if (_stopisoentity (descriptor, &ies) != OK)
		continue;

	    ie = &ies;
	    break;
	}

	if (!hitdes && strcmp (descriptor, desdflt) == 0) {
	    if (_stopisoentity (descriptor, &ies) != OK)
		continue;
	    ies.ie_descriptor = objent;

	    hitdes++;
	    ids = ies;		/* struct copy */
	    continue;
	}

	if (!hitqual && strcmp (descriptor, qualdflt) == 0) {
	    if (_stopisoentity (descriptor, &ies) != OK)
		continue;
	    ies.ie_descriptor = objent;

	    hitqual++;
	    iqs = ies;		/* struct copy */
	    continue;
	}
    }
    (void) endisoentity ();

    if (!ie && hitqual) {
	ie = &ies;
	*ie = iqs;		/* struct copy */

	if (hitdes) {
	    bcopy ((char *) ids.ie_addr.pa_addr.sa_addr.ta_addrs,
		   (char *) ie -> ie_addr.pa_addr.sa_addr.ta_addrs,
		   sizeof ie -> ie_addr.pa_addr.sa_addr.ta_addrs);
	    ie -> ie_addr.pa_addr.sa_addr.ta_naddr =
		ids.ie_addr.pa_addr.sa_addr.ta_naddr;
	}
    }

    if (ie) {
	*iep = *ie;	/* struct copy */
	return OK;
    }
    
    return NOTOK;
}
