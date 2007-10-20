#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/turbo/RCS/tdirname.c,v 9.0 1992/06/16 12:36:15 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/turbo/RCS/tdirname.c,v 9.0 1992/06/16 12:36:15 isode Rel $
 *
 *
 * $Log: tdirname.c,v $
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

#include "general.h"

main( argc, argv )
int	argc;
char	**argv;
{
	char	*p;

	if ( argc < 2 || ((p = rindex( argv[ 1 ], '/' )) == 0) ) {
		(void) write( 1, ".\n", sizeof( ".\n" ) );
		return( 0 );
	}

	*p = '\0';
	(void) write( 1, argv[ 1 ], strlen( argv[ 1 ] ) );
	(void) write( 1, "\n", 1 );

	return( 0 );
}
