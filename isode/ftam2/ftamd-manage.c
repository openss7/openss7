/* ftamd-manage.c - FTAM responder -- management */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ftam2/RCS/ftamd-manage.c,v 9.0 1992/06/16 12:15:43 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ftam2/RCS/ftamd-manage.c,v 9.0 1992/06/16 12:15:43 isode Rel $
 *
 *
 * $Log: ftamd-manage.c,v $
 * Revision 9.0  1992/06/16  12:15:43  isode
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

#include <stdio.h>
#include "ftamsystem.h"

/*    MANAGEMENT */

int	ftam_managementindication (ftg)
struct FTAMgroup *ftg;
{
    struct FTAMgroup    ftms;
    struct FTAMgroup   *ftm = &ftms;
    struct FTAMindication   ftis;
    register struct FTAMindication *fti = &ftis;

    ftam_selection (ftg, ftm);

    if (myfd != NOTOK) {
#ifndef	BRIDGE
	unlock ();
	(void) close (myfd);
#else
	(void) close (myfd);
	(void) ftp_reply ();
#endif
	myfd = NOTOK;
    }

    if (FManageResponse (ftamfd, ftm, fti) == NOTOK)
	ftam_adios (&fti -> fti_abort, "F-MANAGE.RESPONSE");

    FTGFREE (ftg);
}
