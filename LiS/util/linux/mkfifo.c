/*
 *  mkfifo.c - a mkfifo for LiS FIFOs
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
#ident "@(#) LiS mkfifo.c 1.1 12/15/00"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <errno.h>

int verbose = 1;

#define  DEV_CLONE_FIFO_PATH  "/dev/fifo"
#define  DEV_FIFO_0_PATH      "/dev/fifo.0"


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
    fprintf( stderr, "usage: %s [-?][-m<mode>] <path> ...\n", progname );
	
    exit(1);
}


int main( int argc, char *argv[] )
{
    char path[40];
    int c;
    mode_t mode, um;
    struct stat st;
    dev_t fifo_dev;

    /*
     *  get the current umask - since umask() returns it but also sets it,
     *  we get it then restore it.
     *
     *  we then set a default mode which just reflects the umask.
     */
    um = umask(0);  umask(um);
    mode = 0666 & ~um;

    while ((c = getopt( argc, argv, "m:?" )) > 0) {
	switch (c) {
	case 'm':
	    mode = a2i(optarg) & 0666 & ~um;
	    break;
	case '?':
	default:
	    usage( argv[0], c );
	    exit(0);
	}
    }

    stat( DEV_FIFO_0_PATH, &st );
    fifo_dev = st.st_rdev;

    while (argc > optind) {
	strcpy( path, argv[optind++] );

	if (mknod( path, (mode | S_IFCHR), fifo_dev ) < 0) {
	    fprintf( stderr,
		     "mknod(\"%s\", ...) failed: %s (%d)\n",
		     path, strerror(errno), errno );
	}
    }

    exit(0);
}
