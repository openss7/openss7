/*
 *  oc.c - open/<default>close testing of files.  Given a list of paths,
 *         this program simply opens them an optional number of times each,
 *         then waits an optional number of seconds before exiting.
 * 
 *         This program also supports receiving passed file descriptors.
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
#ident "@(#) LiS oc.c 1.1 12/15/00"
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

int verbose = 1;


static void usage( char *progname, char opt )
{
    if (opt != '?') {
	fprintf( stderr,
		 "%s: option '-%c' not recognized.\n", progname, opt );
    }
    fprintf(
	stderr,	"usage: %s [-n<n>][-w<ms>][-r][-q][-?] <path> ...\n",
	progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[40];
    int fd, c, i, n = 1;
    struct stat stat;
    int wt = 0;
    int o_flags, o_wronly = 0, o_rdonly = 0, o_nonblock = 0;
    int is_stream = 0, recv_fd = 0;
    int p0;

    while ((c = getopt( argc, argv, "n:w:rWRBq?" )) > 0) {
	switch (c) {
	case 'n':
	    n = atoi(optarg);
	    break;
	case 'w':
	    wt = atoi(optarg);
	    break;
        case 'r':
	    recv_fd = 1;
	    break;
	case 'W':
	    o_wronly = 1;  o_rdonly = 0;
	    break;
	case 'R':
	    o_rdonly = 1;  o_wronly = 0;
	    break;
	case 'B':
	    o_nonblock = 1;
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
    o_flags =
	(o_wronly ? O_WRONLY : (o_rdonly ? O_RDONLY : O_RDWR)) |
	(o_nonblock ? O_NONBLOCK : 0);

    if (n <= 1)  n = 1;

    for (i = 0;  i < n;  i++) {
	p0 = optind;
	while (argc > p0) {
	    strcpy( path, argv[p0++] );

	    if ((fd = open( path, o_flags )) < 0) {
		fprintf( stderr, "open( \"%s\", 0%o ) failed: %s (%d)\n",
			 path, o_flags, strerror(errno), errno );
		exit(1);
	    }

	    if (verbose) {
		fstat( fd, &stat );
		printf( "%d: fd %d = open( \"%s\", 0%o ) "
			"mode 0%o dev 0x%04x rdev 0x%04x",
		    i, fd, path, o_flags, (int)(stat.st_mode),
		    (int)(stat.st_dev), (int)(stat.st_rdev) );
		is_stream = isastream(fd);
		printf("%s", (is_stream?" [STREAM]":"") );
		printf("\n");  fflush(stdout);
	    }
		
	    if (is_stream && recv_fd) {
		struct pollfd pfd;
		struct strrecvfd recv;
		
		/*
		 *  FIXME: poll wt*1000 for input, and if OK try to
		 *  ioctl( I_RECVFD ).  If OK, report the received
		 *  file via stat.
		 */
		pfd.fd = fd;
		pfd.events = POLLIN;
		if (wt && !poll( &pfd, 1, wt*1000 )) {
		    /*
		     *  poll timed out
		     */
		    fprintf(
			stderr,
			"poll(..., %dms) timed out waiting for RECVFD\n",
			wt*1000 );
		} else {
		    if (ioctl( fd, I_RECVFD, &recv ) < 0) {
			fprintf(
			    stderr,
			    "ioctl(%d, I_RECVFD,...) failed: %s\n",
			    fd, strerror(errno) );
		    } else {
			if (verbose) {
			    fstat( recv.fd, &stat );
			    printf( "%d: fd %d = received"
				    " (mode 0%o dev 0x%04x rdev 0x%04x)",
				    i, recv.fd, (int)(stat.st_mode),
				    (int)(stat.st_dev), (int)(stat.st_rdev) );
			    printf("%s", (isastream(recv.fd)?" [STREAM]":"") );
			    printf("\n");  fflush(stdout);
			}
		    }
		}
	    }
	}
    }
    if (wt > 0) {
	while (wt--)  {
	    fprintf( stderr, "." );  sleep(1);
	}
	fprintf( stderr, "!\n" );
    }
    
    exit(0);
}
