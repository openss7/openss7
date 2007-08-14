/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 $Log: pafd.c,v $
 Revision 1.1.1.1.12.5  2005/07/13 12:01:24  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 1.1.1.1.12.3  2005/05/14 08:35:16  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: pafd.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $"

static char const ident[] =
    "$RCSfile: pafd.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $";

/* 
 *  pafd.c - a test of FD passing to an attached-only path
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/poll.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <limits.h>

static int verbose = 1;

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2003-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software;  you can  redistribute  it and/or modify it under\n\
the terms of the GNU General  Public License as  published by the  Free Software\n\
Foundation; version 3 of the License.\n\
\n\
This program is distributed in the hope that it will be  useful, but WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received  a copy of the GNU  General  Public License along with\n\
this program.  If not, see <http://www.gnu.org/licenses/>,  or write to the Free\n\
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
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

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 3,\n\
    included here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] PATH\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] PATH\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Arguments:\n\
    PATH\n\
        The path to which to fattach.\n\
Options:\n\
    -w, --wait WAIT\n\
        set the wait in milliseconds [default: 0]\n\
    -n, --compare\n\
        compare sent and received file descriptor\n\
    -i, --ignore\n\
        ignore passing file descriptors\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0]);
}

int
main(int argc, char *argv[])
{
	char path[40];
	int fd[2], afd, rfd, sfd;
	struct stat stat;
	int wt = -1, not_same = 0, ignore_recv = 0;
	struct strrecvfd recv;

	for (;;) {
		int c;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{ "wait",    required_argument,	NULL, 'w' },
			{ "compare", no_argument,	NULL, 'n' },
			{ "ignore",  no_argument,	NULL, 'i' },
			{ "quiet",   no_argument,	NULL, 'q' },
			{ "verbose", optional_argument,	NULL, 'v' },
			{ "help",    no_argument,	NULL, 'h' },
			{ "version", no_argument,	NULL, 'V' },
			{ "copying", no_argument,	NULL, 'C' },
			{ "?",	     no_argument,	NULL, 'h' },
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "w:niqvhVC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "w:niqvhVC?");
#endif				/* _GNU_SOURCE */
		switch (c) {
		case 'w':	/* -w, --wait WAIT */
			wt = strtol(optarg, NULL, 0);
			break;
		case 'n':	/* -n, --compare */
			not_same = 1;
			break;
		case 'i':	/* -i, --ignore */
			ignore_recv = 1;
			break;
		case 'q':	/* -q, --quiet */
			verbose = 0;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (!optarg)
				verbose++;
			else
				verbose = strtol(optarg, NULL, 0);
			break;
		case 'h':	/* -h, --help, -?, --? */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			copying(argc, argv);
			exit(0);
		case '?':
		default:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}

	if (optind == argc)
		goto bad_nonopt;

	strcpy(path, argv[optind]);

	if (pipe(fd) < 0) {
		if (verbose)
			fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
		exit(1);
	}
	if (verbose) {
		fstat(fd[0], &stat);
		printf("pipe() fd[0]=%d mode 0%o dev 0x%x rdev 0x%x", fd[0], (int) stat.st_mode,
		       (int) stat.st_dev, (int) stat.st_rdev);
		printf(" [%s]\n", (isastream(fd[0]) ? "STREAMS" : "Linux"));
		fstat(fd[1], &stat);
		printf("pipe() fd[1]=%d mode 0%o dev 0x%x rdev 0x%x", fd[1], (int) stat.st_mode,
		       (int) stat.st_dev, (int) stat.st_rdev);
		printf(" [%s]\n", (isastream(fd[1]) ? "STREAMS" : "Linux"));
	}

	sfd = (not_same ? fd[0] : fd[1]);

	if (fattach(fd[1], path) < 0) {
		if (verbose)
			fprintf(stderr, "fattach( %d, \"%s\" ) failed: %s\n", fd[1], path,
				strerror(errno));
		exit(1);
	} else {
		if (verbose)
			printf("fattach( %d, \"%s\" ) OK\n", fd[1], path);
	}

	if (ioctl(fd[0], I_SENDFD, sfd) < 0) {
		if (verbose)
			fprintf(stderr, "I_SENDFD failed: %s\n", strerror(errno));
		exit(1);
	} else {
		if (verbose)
			printf("ioctl( %d, I_SENDFD %d ) - OK\n", fd[0], sfd);
	}

	if (verbose)
		printf("closing pipe end 0\n");
	close(fd[0]);

	if (verbose)
		printf("closing pipe end 1\n");
	close(fd[1]);

	if ((afd = open(path, O_RDONLY | O_NONBLOCK)) < 0) {
		if (verbose)
			fprintf(stderr, "open failed for device %s: %s (%d)\n", path,
				strerror(errno), errno);
	} else {
		if (verbose) {
			fstat(afd, &stat);
			printf("open( \"%s\",... ): fd %d mode 0%o dev 0x%x rdev 0x%x", path, afd,
			       (int) stat.st_mode, (int) stat.st_dev, (int) stat.st_rdev);
			printf(" [%s]\n", (isastream(afd) ? "STREAMS" : "Linux"));
		}
	}

	if (fdetach(path) < 0) {
		if (verbose)
			fprintf(stderr, "fdetach( \"%s\" ) failed: %s\n", path, strerror(errno));
		exit(1);
	} else {
		if (verbose)
			printf("fdetach( \"%s\" ) OK\n", path);
	}

	if (afd < 0)
		exit(1);

	if (!ignore_recv) {
		if (ioctl(afd, I_RECVFD, &recv) < 0) {
			if (verbose)
				fprintf(stderr, "I_RECVFD failed: %s\n", strerror(errno));
			exit(1);
		} else {
			rfd = recv.fd;
			if (verbose) {
				fstat(rfd, &stat);
				printf("I_RECVFD: fd %d mode 0%o dev 0x%x rdev 0x%x", rfd,
				       (int) stat.st_mode, (int) stat.st_dev, (int) stat.st_rdev);
				printf(" [%s]\n", (isastream(rfd) ? "STREAMS" : "Linux"));
			}
		}
		close(rfd);
	}

	exit(0);
}
