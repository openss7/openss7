/* ssaputils.c - comon session utilities */


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
#include "spkt.h"
#include "tailor.h"

/*    DATA */

#ifdef HULA


static int  su_once_only = 0;
static struct ssapblk susapque;
static struct ssapblk *SuHead = &susapque;



/*    INTERNAL */

struct ssapblk  *newsublk () {
    register struct ssapblk *sb;

    sb = (struct ssapblk   *) calloc (1, sizeof *sb);
    if (sb == NULL)
	return NULL;

    sb -> sb_fd = NOTOK;
    sb -> sb_qbuf.qb_forw = sb -> sb_qbuf.qb_back = &sb -> sb_qbuf;
    sb -> sb_pr = SPDU_PR;

    if (su_once_only == 0) {
	SuHead -> sb_forw = SuHead -> sb_back = SuHead;
	su_once_only++;
    }

    insque (sb, SuHead -> sb_back);

    return sb;
}


int	freesublk (sb)
register struct ssapblk *sb;
{
    register struct qbuf *qb,
			 *qp;

    if (sb == NULL)
	return;

    if (sb -> sb_retry) {
	sb -> sb_retry -> s_mask &= ~SMASK_UDATA_PGI;
	sb -> sb_retry -> s_udata = NULL, sb -> sb_retry -> s_ulen = 0;
	freespkt (sb -> sb_retry);
    }

    if (sb -> sb_xspdu)
	freespkt (sb -> sb_xspdu);
    if (sb -> sb_spdu)
	freespkt (sb -> sb_spdu);
    for (qb = sb -> sb_qbuf.qb_forw; qb != &sb -> sb_qbuf; qb = qp) {
	qp = qb -> qb_forw;
	remque (qb);

	free ((char *) qb);
    }

    remque (sb);

    free ((char *) sb);
}

/*  */

struct ssapblk   *findsublk (sd)
register int sd;
{
    register struct ssapblk *sb;

    if (su_once_only == 0)
	return NULL;

    for (sb = SuHead -> sb_forw; sb != SuHead; sb = sb -> sb_forw)
	if (sb -> sb_fd == sd)
	    return sb;

    return NULL;
}

#endif

