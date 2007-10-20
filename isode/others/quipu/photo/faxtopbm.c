/* faxtopbm.c - FAX to pbm filter */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/faxtopbm.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/faxtopbm.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: faxtopbm.c,v $
 * Revision 9.0  1992/06/16  12:43:35  isode
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
#include "psap.h"
#include "sys.file.h"
#include "pbm.h"

/*  */

/* If your pbm_writepbminit() and pbm_writepbmrow() routines only have  */
/* 3 parameters (look in pbm/pbm.h!) you should remove the next line... */
#define PBM4PARMS

#define ALLOCATION_SIZE 16384L

static	bit	black, white;

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    char   *cp;
    char   *data;
    int     fd;
    char   *file;
    int     len;
    long    limit;
    char   *newData;
    long    size;

    /* process command options and parameters */

    black = PBM_BLACK;
    white = PBM_WHITE;

    file = NULLCP;
    fd = fileno (stdin);

    for (argv++; cp = *argv; argv++) {
        if (*cp == '-') {
	    if (cp[1] == NULL) {
		continue;
	    }
	    else if (strcmp (cp, "-reversebits") == 0) {
		black = PBM_WHITE;
		white = PBM_BLACK;
		continue;
	    }
	    else if (strcmp (cp, "-2d") == 0) {
		continue;
	    }
	    goto usage;
        }
	else if (file) {
usage: ;
	    (void) fprintf (stderr, "usage: faxtopbm [-2d] [file]\n");
	    exit (1);
	}
	else {
	    file = cp;
	}
    }

    if ( file == NULLCP ) {
	file = "<stdin>";
    }
    else {
	fd = open (file, O_RDONLY);
	if ( fd == -1 ) {
	    perror (file);
	    exit (1);
	}
    }

    /* read the entire source file into memory */

    data = (char *)malloc ((unsigned int)ALLOCATION_SIZE);
    if ( !data ) {
	(void) fputs ("faxtopbm: out of memory\n", stderr);
	exit (1);
    }
    limit = ALLOCATION_SIZE;
    size = 0L;

    for (;;) {
	if (size + ALLOCATION_SIZE > limit) {
	    newData = (char *)realloc (data, (unsigned int)(limit + ALLOCATION_SIZE));
	    if ( !newData ) {
		(void) fputs ("faxtopbm: out of memory\n", stderr);
		exit (1);
	    }
	    data = newData;
	    limit += ALLOCATION_SIZE;
	}
	len = read (fd, &data[size], (int)ALLOCATION_SIZE);
	if (len == -1) {
	    perror (file);
	    exit (1);
	}
	else if (len == 0)
	    break;
	size += len;
    }

    if (size < 1) {
	(void) fprintf (stderr, "%s: is not a fax image\n", file);
	exit (1);
    }

    if (decode_t4 (data, file, (int)size) == -1
	    || decode_t4 (data, file, (int)size) == -1) {
	(void) fprintf (stderr,"\n");
	exit (-1);
    }

    free (data);

    exit (0);
}

/*    ERRORS */

static ps_die (ps, s)
register PS	 ps;
register char   *s;
{
    (void) fprintf (stderr, "%s: %s\n", s, ps_error (ps -> ps_errno));
    exit (1);
}


static pe_die (pe, s)
register PE	 pe;
register char   *s;
{
    (void) fprintf (stderr, "%s: %s\n", s, pe_error (pe -> pe_errno));
    exit (1);
}

/*    PHOTO */

static	int	passno = 1;
static	int	x, y, maxx;

static	bit    *bitrow, *bP;


/* ARGSUSED */

photo_start(name)
char   *name;
{
    if (passno == 1)
	maxx = 0;
    x = y = 0;

    return OK;
}


/* ARGSUSED */

photo_end (name)
char   *name;
{
    if (passno == 1) {
	register int	i;

	passno = 2;
	x = maxx, y--;

#ifdef PBM4PARMS
	pbm_writepbminit (stdout, maxx, y, 0);
#else
	pbm_writepbminit (stdout, maxx, y);
#endif

	bitrow = pbm_allocrow (maxx);

	for (i = maxx, bP = bitrow; i-- > 0; )
	    *bP++ = white;
	bP = bitrow;
    }
    else
	pbm_freerow (bitrow);

    return OK;
}


photo_black (length)
int	length;
{
    if (passno == 2) {
	register int	i;

	for (i = length; i > 0; i--)
	    *bP++ = black;
    }

    x += length;

}


photo_white (length)
int	length;
{
    if (passno == 2)
	bP += length;

    x += length;

}


/* ARGSUSED */

photo_line_end (line)
caddr_t line;
{
    if (passno == 1) {
	if (x > maxx)
	    maxx = x;
    }
    else {
	register int	i;

#ifdef PBM4PARMS
	pbm_writepbmrow (stdout, bitrow, maxx, 0);
#else
	pbm_writepbmrow (stdout, bitrow, maxx);
#endif

	for (i = maxx, bP = bitrow; i-- > 0; )
	    *bP++ = white;
	bP = bitrow;
    }

    x = 0, y++;
}
