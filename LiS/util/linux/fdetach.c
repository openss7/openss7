/*
 *  fdetach.c - try to fdetach a list of paths.
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 * 
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 *  MA 02139, USA.
 */

#ident "@(#) LiS fdetach.c 1.2 12/15/00"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int verbose = 0;


static void usage( char *progname, char opt )
{
    if (opt != '?') {
	fprintf( stderr,
		 "%s: option '-%c' not recognized.\n", progname, opt );
    }
    fprintf(
	stderr,	"usage: %s [-v] [-?] [-a|<fdetach_path> ...]\n",
	progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[PATH_MAX];
    int c;

    while ((c = getopt( argc, argv, "av?" )) > 0) {
	switch (c) {
        case 'a':
	    strcpy( path, "*" );
	    break;
	case 'v':
	    verbose = 1;
	    break;
	case '?':
	default:
	    usage( argv[0], c );
	    exit(0);
	}
    }

    if (strcmp( path, "*" ) == 0) {
	if (fdetach("*") < 0) {
	    fprintf( stderr, "fdetach -a failed: %s\n",
		     strerror(errno) );
	    exit(1);
	} else {
	    if (verbose)
		printf( "fdetach -a OK\n" );
	}
    } else {
	while (argc > optind) {
	    strcpy( path, argv[optind++] );
	    
	    if (fdetach(path) < 0) {
		fprintf( stderr, "fdetach( \"%s\" ) failed: %s\n",
			 path, strerror(errno) );
		exit(1);
	    } else {
		if (verbose)
		    printf( "fdetach( \"%s\" ) OK\n", path );
	    }
	}
    }

    exit(0);
}
