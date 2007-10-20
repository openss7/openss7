/* tsaputils.c - common service routines to tsap unit data */


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
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "tailor.h"


#define	selmask(fd,m,n) \
{ \
    FD_SET (fd, &(m)); \
    if ((fd) >= (n)) \
	(n) = (fd) + 1; \
}

/*    DATA */

#ifdef HULA

static int tu_once_only = 0;
static struct tsapblk tusapque;
static struct tsapblk *TuHead = &tusapque;


#ifndef	SIGPOLL
#ifndef TPid
static int TPid = NOTOK;
#endif
#endif


/*    INTERNAL */

struct tsapblk   *newtublk () {
    register struct tsapblk *tb;

    tb = (struct tsapblk   *) calloc (1, sizeof *tb);
    if (tb == NULL)
	return NULL;

    tb -> tb_fd = NOTOK;

    tb -> tb_qbuf.qb_forw = tb -> tb_qbuf.qb_back = &tb -> tb_qbuf;

    if (tu_once_only == 0) {
	TuHead -> tb_forw = TuHead -> tb_back = TuHead;
	tu_once_only++;
    }

    insque (tb, TuHead -> tb_back);

    return tb;
}


freetublk (tb)
register struct tsapblk *tb;
{
    SBV     smask;
    register struct qbuf *qb,
			 *qp;
#ifndef	SIGPOLL
    struct TSAPdisconnect   tds;
#endif

    if (tb == NULL)
	return;

    smask = sigioblock ();

#ifdef HULA
    if (tb -> tb_flags & TB_CLNS)
        {
	if (*tb -> tb_UnitDataClose)
	    (*tb -> tb_UnitDataClose) (tb -> tb_fd);
	tb -> tb_fd = NOTOK;
	}
#endif 

    if (tb -> tb_fd != NOTOK)
	(void) (*tb -> tb_closefnx) (tb -> tb_fd);

    if (tb -> tb_retry)
	freetpkt (tb -> tb_retry);

#ifndef	SIGPOLL
    if ((tb -> tb_flags & TB_ASYN) && TPid > OK) {
	(void) kill (TPid, SIGTERM);
	TPid = NOTOK;
    }
#endif

    for (qb = tb -> tb_qbuf.qb_forw; qb != &tb -> tb_qbuf; qb = qp) {
	qp = qb -> qb_forw;
	remque (qb);

	free ((char *) qb);
    }

    remque (tb);

    free ((char *) tb);

#ifndef	SIGPOLL
    for (tb = TuHead -> tb_forw; tb != TuHead; tb = tb -> tb_forw)
	if (tb -> tb_fd != NOTOK && (tb -> tb_flags & TB_ASYN)) {
	    if (tb -> tb_flags & TB_CLNS)
	        /* (void) TUnitDataWakeUp (tb); */
		;
	    break;
	}
#endif

    (void) sigiomask (smask);
}

/*  */

struct tsapblk   *findtublk (sd)
register int sd;
{
    register struct tsapblk *tb;

    if (tu_once_only == 0)
	return NULL;

    for (tb = TuHead -> tb_forw; tb != TuHead; tb = tb -> tb_forw)
	if (tb -> tb_fd == sd)
	    return tb;

    return NULL;
}



#endif
