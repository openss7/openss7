/*
 *  rwf.c - simple reading/writing test using poll() to get FIFO input.
 *
 *  This program could be changed to use getmsg()/putmsg() instead of
 *  read()/write(), hopefully in several obvious ways.  It has been
 *  written for read()/write() in order to be useful with non-STREAMS
 *  fifos.
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
#ident "@(#) LiS rwf.c 1.1 12/15/00"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <stropts.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>

int verbose = 0;

static inline int _getmsg( int fd, struct strbuf *sb )
{
    int len;

    if ((len = read( fd, sb->buf, sb->maxlen )) >= 0)  sb->len = len;

    return (len < 0 ? len : 0);
}


static inline int _putmsg( int fd, struct strbuf *sb )
{
    int len;

    if ((len = write( fd, sb->buf, sb->len )) >= 0)  sb->len = len;

    return (len < 0 ? len : 0);
}


static void usage( char *progname, char opt )
{
    if (opt != '?') {
	fprintf( stderr,
		 "%s: option '-%c' not recognized.\n", progname, opt );
    }
    fprintf( stderr,
	     "usage: %s [-n<num>] [-m[-w<ms>][-x][-?]\n",
	progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[40];
    int fd, c, j, m = 0, l = 1;
    struct stat st;
    int wt = 0;
    int is_stream = 0, push_relay = 0;

    while ((c = getopt( argc, argv, "m:l:w:rv?" )) > 0) {
	switch (c) {
	case 'm':
	    m = atoi(optarg);
	    break;
	case 'l':
	    l = atoi(optarg);
	    break;
	case 'w':
	    wt = atoi(optarg);
	    break;
        case 'r':
	    push_relay = 1;
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

    if (argc > optind)
	strcpy( path, argv[optind] );
    else {
	usage( argv[0], '?' );
	exit(1);
    }

    if ((fd = open( path, O_RDWR|O_NONBLOCK )) < 0) {
	fprintf( stderr, "open(\"%s\", O_RDWR|O_NONBLOCK) failed: %s\n",
		 path, strerror(errno) );
	exit(1);
    }
    
    if (verbose) {
	fstat( fd, &st );
	printf( "%d: open \"%s\" (mode 0%o dev 0x%04x rdev 0x%04x)",
		fd, path, (int)(st.st_mode),
		(int)(st.st_dev), (int)(st.st_rdev) );
	is_stream = isastream(fd);
	printf("%s\n", (is_stream?" [STREAM]":"") );
    }
    if (is_stream) {
	if (push_relay) {
	    if (ioctl( fd, I_PUSH, "relay" ) < 0) {
		fprintf( stderr,
			 "ioctl(%d, I_PUSH, relay) failed: %s\n",
			 fd, strerror(errno) );
		exit(1);
	    }
	    if (verbose)
		printf( "module \"relay\" pushed onto %d\n", fd );
	}
    }
    
    if (m > 0) {
	struct pollfd pfd;
	struct strbuf sbo, sbi;
	
	pfd.fd = fd;
	pfd.events = POLLIN|POLLHUP;
	
	sbo.len = l;
	sbo.buf = malloc(l);
	memset( sbo.buf, 'x', l );
	
	sbi.maxlen = l;
	sbi.buf = malloc(l);
	
	for (j = 0;  j < m;  j++) {
	    _putmsg( fd, &sbo );
	    poll( &pfd, 1, wt );
	    if (verbose)
		printf( "poll(%d,...) -> 0x%x\n", fd, pfd.revents );
	    if (pfd.revents & POLLHUP) {
		if (verbose)
		    fprintf( stderr, "POLLHUP received - exiting\n" );
		break;
	    }
	    else if (!pfd.revents) {
		if (verbose)
		    fprintf( stderr, "poll(%d,...) timed out - exiting\n",
			     pfd.fd );
		break;
	    }
	    sbi.len = 0;
	    _getmsg( fd, &sbi );
	    if (verbose)
		printf( "read(%d,...) -> len %d\n", fd, sbi.len );
	}
    }

    exit(0);
}
