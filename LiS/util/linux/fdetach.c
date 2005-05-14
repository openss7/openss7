/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";


/*
 *  fdetach.c - try to fdetach a list of paths.
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
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

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int             output = 1;

void copying(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2003-2004  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software;  you can  redistribute  it and/or modify it under\n\
the terms of the GNU General  Public License as  published by the  Free Software\n\
Foundation; either  version 2 of  the  License, or  (at  your option) any  later\n\
version.\n\
\n\
This program is distributed in the hope that it will be  useful, but WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received  a copy of the GNU  General  Public License along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
", argv[0], ident);
}

void version(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    included here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stderr, "\
Usage:\n\
    %1$s [options] -a\n\
    %1$s [options] PATH ...\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] -a\n\
    %1$s [options] PATH ...\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Arguments:\n\
    PATH ...\n\
        the path or paths in the directory to detach\n\
Options:\n\
    -a, --all\n\
        detach all paths\n\
    -v, --verbose\n\
        verbose output\n\
    -q, --quiet\n\
        suppress normal output\n\
    -h, --help, -?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0]);
}

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int main(int argc, char *argv[])
{
    char            path[PATH_MAX];
    for (;;) {
	int             c;
#ifdef _GNU_SOURCE
	int             option_index = 0;
	/* *INDENT-OFF* */
	static struct option long_options[] = {
	    { "all",	 no_argument, NULL, 'a' },
	    { "verbose", no_argument, NULL, 'v' },
	    { "quiet",	 no_argument, NULL, 'q' },
	    { "version", no_argument, NULL, 'V' },
	    { "copying", no_argument, NULL, 'C' },
	    { "help",	 no_argument, NULL, 'h' },
	    { "?",	 no_argument, NULL, 'h' },
	    { 0, }
	};
	/* *INDENT-ON* */
	c = getopt_long_only(argc, argv, "avqVCh?", long_options, &option_index);
#else				/* _GNU_SOURCE */
	c = getopt(argc, argv, "avqVCh?");
#endif				/* _GNU_SOURCE */

	if (c == -1)
	    break;
	switch (c) {
	case 'a':		/* -a, --all */
	    strcpy(path, "*");
	    break;
	case 'v':		/* -v, --verbose */
	    output += 1;
	    break;
	case 'q':		/* -q, --quiet */
	    output = 0;
	    break;
	case 'V':		/* -V, --version */
	    version(argc, argv);
	    exit(0);
	case 'C':		/* -C, --copying */
	    copying(argc, argv);
	    exit(0);
	case 'h':		/* -h, --help */
	    help(argc, argv);
	    exit(0);
	case '?':
	default:
	    optind--;
	  bad_nonopt:
	    if (optind < argc && output) {
		fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
		for (; optind < argc; optind++)
		    fprintf(stderr, "%s ", argv[optind]);
		fprintf(stderr, "\n");
	    }
	  bad_usage:
	    usage(argc, argv);
	    exit(2);
	}
    }

    if (strcmp(path, "*") == 0) {
	if (optind < argc)
	    goto bad_nonopt;
	if (fdetach("*") < 0) {
	    if (output)
		fprintf(stderr, "%s: -a failed: %s\n", argv[0],
			strerror(errno));
	    exit(1);
	} else {
	    if (output > 1)
		printf("%s: -a OK\n", argv[0]);
	}
    } else {
	if (optind == argc) {
	    if (output)
		fprintf(stderr, "%s: missing path\n", argv[0]);
	    goto bad_usage;
	}
	while (optind < argc) {
	    strcpy(path, argv[optind++]);

	    if (fdetach(path) < 0) {
		if (output)
		    fprintf(stderr, "fdetach( \"%s\" ) failed: %s\n",
			    path, strerror(errno));
		exit(1);
	    } else {
		if (output > 1)
		    printf("fdetach( \"%s\" ) OK\n", path);
	    }
	}
    }

    exit(0);
}
