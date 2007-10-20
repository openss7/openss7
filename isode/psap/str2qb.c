/* str2qb.c - string to qbuf */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/str2qb.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/str2qb.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: str2qb.c,v $
 * Revision 9.0  1992/06/16  12:25:44  isode
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

/*  */

struct qbuf *str2qb (s, len, head)
char   *s;
int	len,
	head;
{
    register struct qbuf *qb,
			 *pb;

    if ((pb = (struct qbuf *) malloc ((unsigned) (sizeof *pb + len))) == NULL)
	return NULL;

    if (head) {
	if ((qb = (struct qbuf *) malloc (sizeof *qb)) == NULL) {
	    free ((char *) pb);
	    return NULL;
	}
	qb -> qb_forw = qb -> qb_back = qb;
	qb -> qb_data = NULL, qb -> qb_len = len;
	insque (pb, qb);
    }
    else {
	pb -> qb_forw = pb -> qb_back = pb;
	qb = pb;
    }

    pb -> qb_data = pb -> qb_base;
    if ((pb -> qb_len = len) > 0 && s)
	bcopy (s, pb -> qb_data, len);

    return qb;
}
