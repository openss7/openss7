#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/turbo/RCS/edbcat.c,v 9.0 1992/06/16 12:36:15 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/turbo/RCS/edbcat.c,v 9.0 1992/06/16 12:36:15 isode Rel $
 *
 *
 * $Log: edbcat.c,v $
 * Revision 9.0  1992/06/16  12:36:15  isode
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
#include <gdbm.h>
#include "sys.file.h"
#include <ctype.h>

extern int gdbm_errno;

main (argc, argv)
int	argc;
char	**argv;
{
	GDBM_FILE	db;
 	datum		prv, key, content;
 	char		*p, gfname[1024];

	if ( argc != 2 ) {
		(void) printf("usage: %s edbdbmfile\n", argv[0]);
		return(1);
	}

	(void) strcpy(gfname, argv[1]);
	if ( (p = rindex(argv[1], '.')) == NULL 
	    || strcmp(p, ".gdbm") != 0 )
		(void) strcat(gfname, ".gdbm");

	if ( (db = gdbm_open(gfname, 0, GDBM_READER, 0, 0)) == NULL ) {
		(void) fprintf( stderr, "Can't open (%s)\ndbm_error is (%d)\n",
		    gfname, gdbm_errno );
		return(1);
	}

	key.dptr = "HEADER";
	key.dsize = sizeof("HEADER");
	content = gdbm_fetch(db, key);
	if ( content.dptr == NULL ) 
		(void) printf("No header!  Continuing...\n");
	else
		(void) printf("%s\n", content.dptr);

	prv.dptr = NULL;
	for ( key = gdbm_firstkey(db); key.dptr; key = gdbm_nextkey(db, prv) ) {
		if ( prv.dptr != NULL )
			free( prv.dptr );
		if ( strcmp(key.dptr, "HEADER") == 0 ) {
			prv = key;
			continue;
		}

		content = gdbm_fetch(db, key);
		(void) printf("%s", content.dptr);
		free(content.dptr);
		content.dptr = NULL;
		prv = key;
	}

	(void) gdbm_close(db);
	return(0);
}
