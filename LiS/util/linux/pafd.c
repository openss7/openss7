/*
 *  pafd.c - a test of FD passing to an attached-only path
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
#ident "@(#) LiS pafd.c 1.1 12/15/00"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <poll.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <limits.h>

static int verbose = 1;

static void usage( char *progname, char opt )
{
    if (opt != '?') {
	fprintf( stderr,
		 "%s: option '-%c' not recognized.\n", progname, opt );
    }
    fprintf( stderr,
	     "usage: %s [-n<n>][-i][-w<ms>][-q][-?] <fattach_path>\n",
	     progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[40];
    int fd[2], afd, rfd, sfd;
    struct stat stat;
    int c, wt = -1, not_same = 0, ignore_recv = 0;
    struct strrecvfd recv;

    while ((c = getopt( argc, argv, "w:niq?" )) > 0) {
	switch (c) {
	case 'w':
	    wt = atoi(optarg);
	    break;
	case 'n':
	    not_same = 1;
	    break;
	case 'i':
	    ignore_recv = 1;
	    break;
	case 'q':
	    verbose = 0;
	    break;
	case '?':
	default:
	    usage( argv[0], c );
	    exit(0);
	}
    }

    if (argc > optind)
	strcpy( path, argv[optind] );
    else {
	usage( argv[0], '?' );
	exit(1);
    }

    if (pipe(fd) < 0) {
	fprintf( stderr, "pipe() failed: %s\n", strerror(errno) );
	exit(1);
    }
    if (verbose) {
	fstat( fd[0], &stat );
	printf( "pipe() fd[0]=%d mode 0%o dev 0x%x rdev 0x%x",
		fd[0], (int)stat.st_mode,
		(int)stat.st_dev, (int)stat.st_rdev );
	printf( " [%s]\n", (isastream(fd[0]) ? "STREAMS" : "Linux") );
	fstat( fd[1], &stat );
	printf( "pipe() fd[1]=%d mode 0%o dev 0x%x rdev 0x%x",
		fd[1], (int)stat.st_mode,
		(int)stat.st_dev, (int)stat.st_rdev );
	printf( " [%s]\n", (isastream(fd[1]) ? "STREAMS" : "Linux") );
    }

    sfd = (not_same ? fd[0] : fd[1]);

    if (fattach( fd[1], path ) < 0) {
	fprintf( stderr, "fattach( %d, \"%s\" ) failed: %s\n",
		 fd[1], path, strerror(errno) );
	exit(1);
    } else {
	if (verbose)
	    printf( "fattach( %d, \"%s\" ) OK\n", fd[1], path );
    }

    if (ioctl( fd[0], I_SENDFD, sfd ) < 0) {
	fprintf( stderr, "I_SENDFD failed: %s\n", strerror(errno) );
	exit(1);
    } else {
	if (verbose)
	    printf( "ioctl( %d, I_SENDFD %d ) - OK\n", fd[0], sfd );
    }

    if (verbose)
	printf( "closing pipe end 0\n" );
    close(fd[0]);

    if (verbose)
	printf( "closing pipe end 1\n" );
    close(fd[1]);

    if ((afd = open( path, O_RDONLY|O_NONBLOCK )) < 0) {
	fprintf( stderr, "open failed for device %s: %s (%d)\n",
		 path, strerror(errno), errno );
    } else {
	if (verbose) {
	    fstat( afd, &stat );
	    printf( "open( \"%s\",... ): fd %d mode 0%o dev 0x%x rdev 0x%x",
		    path, afd,
		    (int)stat.st_mode, (int)stat.st_dev, (int)stat.st_rdev );
	    printf( " [%s]\n", (isastream(afd) ? "STREAMS" : "Linux") );
	}
    }

    if (fdetach( path ) < 0) {
	fprintf( stderr, "fdetach( \"%s\" ) failed: %s\n",
		 path, strerror(errno) );
	exit(1);
    } else {
	if (verbose)
	    printf( "fdetach( \"%s\" ) OK\n", path );
    }

    if (afd < 0)   exit(1);

    if (!ignore_recv) {
	if (ioctl( afd, I_RECVFD, &recv ) < 0) {
	    fprintf( stderr, "I_RECVFD failed: %s\n", strerror(errno) );
	    exit(1);
	} else {
	    rfd = recv.fd;
	    if (verbose) {
		fstat( rfd, &stat );
		printf( "I_RECVFD: fd %d mode 0%o dev 0x%x rdev 0x%x",
			rfd,
			(int)stat.st_mode,
			(int)stat.st_dev, (int)stat.st_rdev );
		printf( " [%s]\n", (isastream(rfd) ? "STREAMS" : "Linux") );
	    }
	}
	close(rfd);
    }

    exit(0);
}
