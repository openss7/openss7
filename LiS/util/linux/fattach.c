/*
 *  fattach.c - try to fattach a list of paths to a path naming a STREAMS
 *  device, or to the ends of a pipe.
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

#ident "@(#) LiS fattach.c 1.2 12/15/00"

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
#include <sys/ioctl.h>

int verbose = 0;


/*
 *  a variant of atoi() that also does C-style octal and hex conversion
 */
static int a2i( char *s )
{
    char c;
    int val = 0, sign = 1, rad = 10;
    
    c = *(s++);
    if (c == '-') {
	sign = -1;  c = *s++;
    }
    if (c == '0') {
	rad = 8;  c = *s++;
    }
    if (c == 'x') {
	rad = 16;  c = *s++;
    }

    while (c) {
	if (c >= '0' && c <= '9' && c - '0' < rad) {
	    val = (val * rad) + (c - '0');  c = *s++;
	}
	else if (c >= 'a' && c <= 'f' && rad == 16) {
	    val = (val * rad) + (c - 'a' + 10);  c = *s++;
	}
	else if (c >= 'A' && c <= 'F' && rad == 16) {
	    val = (val * rad) + (c - 'A' + 10);  c = *s++;
	}
	else c = 0;
    }

    return (val * sign);
}


static void usage( char *progname, char opt )
{
    if (opt != '?') {
	fprintf( stderr,
		 "%s: option '-%c' not recognized.\n", progname, opt );
    }
    fprintf(
	stderr,
	"usage: %s [-u|-m|-M<mode>] [-v] [-?] \\\n"
	"\t\t[-p|-c|<stream_path>] <fattach_path> ...\n",
	progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[PATH_MAX];
    int c, fd[2];
    int use_pipe = 0, x, push_connld = 0;
    int use_mode = 0, use_new_mode = 0, um, new_mode[2];
    struct stat st;

    um = umask(0);  umask(um);

    while ((c = getopt( argc, argv, "muM:pcv?" )) > 0) {
	switch (c) {
	case 'm':
	    use_mode = 1;
	    break;
	case 'u':
	    use_new_mode = 1;
	    new_mode[0] = new_mode[1] = (0666 & ~um);
	    break;
	case 'M':
	    use_new_mode = 1;
	    new_mode[0] = new_mode[1] = a2i(optarg);
	    break;
	case 'p':
	    use_pipe = use_new_mode = 1;
	    new_mode[0] = new_mode[1] = (0666 & ~um);
	    break;
        case 'c':
	    push_connld = use_pipe = use_new_mode = 1;
	    new_mode[0] = new_mode[1] = (0666 & ~um);
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

    if (optind+1 >= argc) {
	usage( argv[0], '?' );  exit(1);
    }

    if (use_pipe) {
	if (pipe(fd) < 0)  {
	    fprintf( stderr,
		     "pipe() failed: %s\n", strerror(errno) );
	    exit(1);
	}
	if (use_mode && !use_new_mode) {
	    if (!(fstat( fd[0], &st ) < 0))
		new_mode[0] = st.st_mode;
	    if (!(fstat( fd[1], &st ) < 0))
		new_mode[1] = st.st_mode;
	}
	if (push_connld) {
	    if (ioctl( fd[0], I_PUSH, "connld" ) < 0) {
		fprintf( stderr,
			 "ioctl( %d, I_PUSH, connld ) failed: %s\n",
			 fd[0], strerror(errno) );
		exit(1);
	    }
	}
    } else {
	strcpy( path, argv[optind++] );
	
	if ((fd[0] = open( path, O_RDONLY|O_NONBLOCK )) < 0) {
	    fprintf( stderr, "open failed for device %s: %s (%d)\n",
		     path, strerror(errno), errno );
	    exit(1);
	}
	if (!isastream(fd[0])) {
	    fprintf( stderr, "\"%s\" is not a STREAMS device\n", path );
	    exit(1);
	}
	fd[1] = fd[0];
	if (use_mode && !use_new_mode) {
	    if (!(fstat( fd[0], &st ) < 0))
		new_mode[0] = new_mode[1] = st.st_mode;
	}
    }

    if (verbose) {
	if (use_pipe) {
	    fstat( fd[0], &st );
	    printf( "pipe() fd[0]=%d mode 0%o dev 0x%x rdev 0x%x",
		    fd[0], (int)st.st_mode,
		    (int)st.st_dev, (int)st.st_rdev );
	    printf( "%s", (push_connld ? " <connld>" : "") );
	    printf( " [%s]\n", (isastream(fd[0]) ? "STREAM" : "Linux") );
	    
	    fstat( fd[1], &st );
	    printf( "pipe() fd[1]=%d mode 0%o dev 0x%x rdev 0x%x",
		    fd[1], (int)st.st_mode,
		    (int)st.st_dev, (int)st.st_rdev );
	    printf( " [%s]\n", (isastream(fd[1]) ? "STREAM" : "Linux") );
	} else {
	    fstat( fd[0], &st );
	    printf( "\"%s\": mode 0%o dev 0x%x rdev 0x%x [STREAM]\n",
		    path, (int)st.st_mode,
		    (int)st.st_dev, (int)st.st_rdev );
	}
    }

    x = 0;
    while (argc > optind) {
	strcpy( path, argv[optind++] );

	if (use_mode && !use_new_mode)
	    if (!(stat( path, &st ) < 0))
		new_mode[x] = st.st_mode;

	if (fattach( fd[x], path ) < 0) {
	    fprintf( stderr, "fattach( %d, \"%s\" ) failed: %s\n",
		     fd[x], path, strerror(errno) );
	    exit(1);
	} else {
	    if (verbose)
		printf( "fattach( %d, \"%s\" ) OK\n", fd[x], path );
	}
	x ^= 1;
    }

    if (use_mode || use_new_mode)
	fchmod( fd[0], new_mode[0] );
    close(fd[0]);
    if (use_pipe)  {
	if (use_mode || use_new_mode)
	    fchmod( fd[1], new_mode[1] );
	close(fd[1]);
    }

    exit(0);
}
