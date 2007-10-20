/* d_main.c - make the decode routine into a stand alone program */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/d_main.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/d_main.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: d_main.c,v $
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
#include "quipu/photo.h"

#define ALLOCATION_SIZE 16384L

int	two_passes = 0;

char   *getenv ();

main (argc, argv)
int     argc;
char ** argv;
{
    char *data;
    int   fd;
    char *file = "<stdin>";
    int   len;
    long  limit;
    char *name;
    char *newData;
    long  size;

    if ((name = getenv ("RDN")) == NULL)
        name = "unknown";

    /* process commmand line options and parameters */

    if (argc > 1)
	    name = *(++argv);

    fd = fileno (stdin);

    /* read the entire source file into memory */

    data = (char *)malloc ((unsigned int)ALLOCATION_SIZE);
    if ( !data ) {
	(void) fputs ("decode_fax: out of memory\n", stderr);
	exit (1);
    }
    limit = ALLOCATION_SIZE;
    size = 0L;

    for (;;) {
	if (size + ALLOCATION_SIZE > limit) {
	    newData = (char *)realloc (data, (unsigned int) (limit + ALLOCATION_SIZE));
	    if ( !newData ) {
		(void) fputs ("decode_fax: out of memory\n", stderr);
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
   if (decode_t4 (data, name, (int)size) == -1) {
	(void) fprintf (stderr,"\n");
	exit (-1);
   }
   if (two_passes && decode_t4 (data, name, (int)size) == -1) {
	(void) fprintf (stderr,"\n");
	exit (-1);
   }
   return (0);
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
