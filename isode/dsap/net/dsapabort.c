/* dsapabort.c - DSAP: Lose the binding identified by the descriptor */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/net/RCS/dsapabort.c,v 9.0 1992/06/16 12:14:05 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/net/RCS/dsapabort.c,v 9.0 1992/06/16 12:14:05 isode Rel $
 *
 *
 * $Log: dsapabort.c,v $
 * Revision 9.0  1992/06/16  12:14:05  isode
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

#include "logger.h"
#include "quipu/dsap.h"

extern LLog	* log_dsap;

/*    RO-ABORT.REQUEST */

/* ARGSUSED */


int	  DUAbortRequest (sd, di)
int			  sd;
struct DSAPindication	* di;
{
	int			  result;
        struct RoNOTindication    rni_s;
        struct RoNOTindication  * rni = &(rni_s);

	LLOG (log_dsap, LLOG_NOTICE, 
	     ("RO-ABORT-BIND.REQUEST called on %d", sd));

	watch_dog ("RoBindUAbort");
	result = RoBindUAbort (sd, rni);
	watch_dog_reset();

	if (result != OK)
		return (ronot2dsaplose (di, "D-ABORT-BIND.REQUEST", rni));

	return (OK);
}

