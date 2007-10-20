/* acserver1.c - generic server dispatch */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/acserver1.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/acserver1.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: acserver1.c,v $
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

#include <signal.h>
#include "psap.h"
#include "tsap.h"
#include "sys.file.h"
#include "tailor.h"

/*  */

int	isodeserver (argc, argv, aei, initfnx, workfnx, losefnx, td)
int	argc;
char  **argv;
AEI	aei;
IFP	initfnx,
	workfnx,
	losefnx;
struct TSAPdisconnect *td;
{
    if (iserver_init (argc, argv, aei, initfnx, td) == NOTOK)
	return NOTOK;

    for (;;) {
	int     result;

	if ((result = iserver_wait (initfnx, workfnx, losefnx, 0, NULLFD,
				    NULLFD, NULLFD, NOTOK, td)) != OK)
	    return (result == DONE ? OK : result);
    }
}
