/* str2spkt.c - read/write a SPDU thru a string */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/ssap/RCS/str2spkt.c,v 5.0 88/07/21 14:58:20 mrose Rel $";
#endif

/* 
 * $Header: /f/iso/ssap/RCS/str2spkt.c,v 5.0 88/07/21 14:58:20 mrose Rel $
 *
 *
 * $Log$
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
#include "spkt.h"
#include "tailor.h"

/*  */

char   *spkt2str (s)
struct ssapkt *s;
{
    int     i,
	    len;
    char   *base,
	   *dp;
    static char buffer[(CONNECT_MAX + BUFSIZ) * 2 + 1];

    if (spkt2tsdu (s, &base, &len) == NOTOK)
	return NULLCP;
    if (s -> s_udata)
	switch (s -> s_code) {
	    case SPDU_DT:
		if (s -> s_mask & SMASK_SPDU_GT)
		    break;	/* else fall */
	    case SPDU_EX:
	    case SPDU_TD:
		if ((dp = realloc (base, (unsigned) (i = len + s -> s_ulen)))
			== NULL) {
		    free (base);
		    return NULLCP;
		}
		bcopy (s -> s_udata, (base = dp) + len, s -> s_ulen);
		len = i;
		break;

	    default:
		break;
	}

    buffer[explode (buffer, (u_char *) base, len)] = NULL;
    if (len > 0)
	free (base);

#ifdef	DEBUG
    if (ssaplevel & ISODELOG_PDUS) {
	if (strcmp (ssapfile, "-")) {
	    char    file[BUFSIZ];
	    FILE   *fp;

	    (void) sprintf (file, ssapfile, getpid ());
	    if (fp = fopen (file, "a")) {
		fprintf (fp, "str = %d/\"%s\"\n", strlen (buffer), buffer);
		spkt2text (fp, s, 0);
		(void) fclose (fp);
	    }
	}
	else {
	    (void) fflush (stdout);
	    fprintf (stderr, "str = %d/\"%s\"\n", strlen (buffer), buffer);
	    spkt2text (stderr, s, 0);
	}
    }
#endif

    return buffer;
}

/*  */

struct ssapkt *str2spkt (buffer)
char  *buffer;
{
    int	    cc;
    char    packet[CONNECT_MAX + BUFSIZ];
    register struct ssapkt *s;
    struct qbuf qbs;
    register struct qbuf *qb = &qbs,
			 *qp;

    bzero ((char *) qb, sizeof *qb);
    qb -> qb_forw = qb -> qb_back = qb;

    cc = implode ((u_char *) packet, buffer, strlen (buffer));
    if ((qp = (struct qbuf *) malloc (sizeof *qp + (unsigned) cc)) == NULL)
	s = NULLSPKT;
    else {
	bcopy (packet, qp -> qb_data = qp -> qb_base, qp -> qb_len = cc);
	insque (qp, qb -> qb_back);
	s = tsdu2spkt (qb, cc, NULLIP);
	for (qp = qb -> qb_forw; qp != qb; qp = qp -> qb_forw) {/* never! */
	    remque (qp);

	    free ((char *) qp);
	}
    }

#ifdef	DEBUG
    if (ssaplevel & ISODELOG_PDUS) {
	if (strcmp (ssapfile, "-")) {
	    char    file[BUFSIZ];
	    FILE   *fp;

	    (void) sprintf (file, ssapfile, getpid ());
	    if (fp = fopen (file, "a")) {
		fprintf (fp, "str = %d/\"%s\"\n", strlen (buffer), buffer);
		if (s)
		    spkt2text (fp, s, 1);
		(void) fclose (fp);
	    }
	}
	else {
	    (void) fflush (stdout);
	    fprintf (stderr, "str = %d/\"%s\"\n", strlen (buffer), buffer);
	    if (s)
		spkt2text (stderr, s, 1);
	}
    }
#endif

    return s;
}
