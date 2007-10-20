/* tsapstate.c - TPM: hack state */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/tsapstate.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/tsap/RCS/tsapstate.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 *
 * $Log: tsapstate.c,v $
 * Revision 9.0  1992/06/16  12:40:39  isode
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
#include "tpkt.h"

/*  */

int	TSaveState (sd, vec, td)
int	sd;
char  **vec;
register struct TSAPdisconnect *td;
{
    SBV     smask;
    register struct tsapblk *tb;
    static char buffer[sizeof *tb * 2 + 1];

    missingP (vec);

    smask = sigioblock ();

    tsapPsig (tb, sd);

    if (tb -> tb_len > 0) {
	(void) sigiomask (smask);

	return tsaplose (td, DR_WAITING, NULLCP, NULLCP);
    }

    buffer[explode (buffer, (u_char *) tb, sizeof *tb)] = NULL;
    *vec++ = buffer;
    *vec = NULL;

    tb -> tb_fd = NOTOK;

    freetblk (tb);

    (void) sigiomask (smask);

    return OK;
}

/*  */

int	TRestoreState (buffer, ts, td)
char   *buffer;
register struct TSAPstart *ts;
register struct TSAPdisconnect *td;
{
    struct tsapblk  tbs;
    register struct tsapblk *tb;

    missingP (buffer);
    missingP (ts);

    if ((tb = newtblk ()) == NULL)
	return tsaplose (td, DR_CONGEST, NULLCP, "out of memory");

    if (implode ((u_char *) &tbs, buffer, strlen (buffer)) != sizeof tbs) {
	(void) tsaplose (td, DR_PARAMETER, NULLCP, "bad state vector");
	goto out1;
    }

    if (findtblk (tbs.tb_fd)) {
	(void) tsaplose (td, DR_PARAMETER, NULLCP, "transport descriptor active");
	goto out1;
    }
    tb -> tb_fd = tbs.tb_fd;
    tb -> tb_flags = tbs.tb_flags & (TB_CONN | TB_EXPD | TB_TP0 | TB_TP4);
    tb -> tb_srcref = tbs.tb_srcref;
    tb -> tb_dstref = tbs.tb_dstref;
    tb -> tb_initiating = tbs.tb_initiating;	/* struct copy */
    tb -> tb_responding = tbs.tb_responding;	/* struct copy */

    switch (tb -> tb_flags & (TB_TP0 | TB_TP4)) {
#ifdef	TCP
	case TB_TCP: 
	    (void) TTService (tb);
	    break;
#endif

#ifdef	X25
	case TB_X25: 
	    (void) XTService (tb);
	    break;
#endif

#ifdef	TP4
	case TB_TP4: 
	    (void) tp4init (tb);
	    break;
#endif

	default: 
	    (void) tsaplose (td, DR_PARAMETER, NULLCP, "network type not set");
	    tb -> tb_fd = NOTOK;
	    goto out1;
    }

    tb -> tb_tsdusize = tbs.tb_tsdusize;

    bzero ((char *) ts, sizeof *ts);
    ts -> ts_sd = tb -> tb_fd;
    copyTSAPaddrX (&tb -> tb_initiating, &ts -> ts_calling);
    copyTSAPaddrX (&tb -> tb_responding, &ts -> ts_called);
    if (tb -> tb_flags & TB_EXPD)
	ts -> ts_expedited = 1;
    ts -> ts_tsdusize = tb -> tb_tsdusize;
    ts -> ts_qos = tb -> tb_qos;	/* struct copy */

    return OK;

out1: ;
    freetblk (tb);

    return NOTOK;
}
