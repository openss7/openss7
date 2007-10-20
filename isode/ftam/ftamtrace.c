/* ftamtrace.c - FPM: tracing */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ftam/RCS/ftamtrace.c,v 9.0 1992/06/16 12:14:55 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ftam/RCS/ftamtrace.c,v 9.0 1992/06/16 12:14:55 isode Rel $
 *
 *
 * $Log: ftamtrace.c,v $
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

/*    DATA */

LLog _ftam_log = {
    "ftam.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
    LLOG_NONE, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *ftam_log = &_ftam_log;

static char *ftam_appls[] = {
    "Abstract-Syntax-Name",
    "Access-Context",
    "Access-Passwords",
    "Access-Request",
    "Account",
    "Action-Result",
    "Activity-Identifier",
    "Application-Entity-Title",
    "Change-Attributes",
    "Charging",
    "Concurrency-Control",
    "Constraint-Set-Name",
    "Create-Attributes",
    "Diagnostic",
    "Document-Type-Name",
    "FADU-Identity",
    "FADU-Lock",
    "Password",
    "Read-Attributes",
    "Select-Attributes",
    "Shared-ASE-Information",
    "State-Result",
    "User-Identity"
};

static int ftam_nappl = sizeof ftam_appls / sizeof ftam_appls[0];

/*    set tracing */

int	FHookRequest (sd, tracing, fti)
int	sd;
IFP	tracing;
struct FTAMindication *fti;
{
    SBV	    smask;
    register struct ftamblk *fsb;

    missingP (fti);

    smask = sigioblock ();

    ftamPsig (fsb, sd);

    if (fsb -> fsb_trace = tracing)
	pe_applist = ftam_appls, pe_maxappl = ftam_nappl;

    (void) sigiomask (smask);

    return OK;
}

/*    user-defined tracing */

/* ARGSUSED */

int	FTraceHook (sd, event, fpdu, pe, rw)
int	sd;
char   *event,
       *fpdu;
PE	pe;
int	rw;
{
    if (event)
	LLOG (ftam_log, LLOG_ALL, ("%s %s", rw > 0 ? "event"
			     : rw == 0 ? "action" : "exception", event));

    if (pe)
	pvpdu (ftam_log, print_FTAM_PDU_P, pe, fpdu ? fpdu : "FPDU", rw);

    (void) ll_sync (ftam_log);
}
