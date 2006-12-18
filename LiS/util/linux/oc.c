/*****************************************************************************

 @(#) $RCSfile: oc.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2005/07/13 12:01:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: oc.c,v $
 Revision 1.1.1.1.12.5  2005/07/13 12:01:24  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 1.1.1.1.12.3  2005/05/14 08:35:16  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: oc.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $"

static char const ident[] =
    "$RCSfile: oc.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $";

/* 
 *  oc.c - open/<default>close testing of files.  Given a list of paths,
 *         this program simply opens them an optional number of times each,
 *         then waits an optional number of seconds before exiting.
 * 
 *         This program also supports receiving passed file descriptors.
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
#endif				/* _GNU_SOURCE */
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <errno.h>

int verbose = 1;

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2003-2005  OpenSS7 Corporation <http://www.openss7.com/>\n\
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

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
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
    %1$s [options] PATH ...\n\
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
    %1$s [options] PATH ...\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Arguments:\n\
    PATH ...\n\
        The path or paths to STREAMS devices open (and close).\n\
Options:\n\
    -n, --number NUMBER\n\
        set the number of iterations\n\
    -w, --wait WAIT\n\
        set the close wait in milliseconds [default: 0]\n\
    -r, --recvfd RECVFD\n\
        set the receive file descriptor\n\
    -W, --wronly\n\
        open write only\n\
    -R, --rdonly\n\
        open read only\n\
    -B, --nonblock\n\
        open for non-blocking operation\n\
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
	int fd, i, n = 1;
	struct stat stat;
	int wt = 0;
	int o_flags, o_wronly = 0, o_rdonly = 0, o_nonblock = 0;
	int is_stream = 0, recv_fd = 0;
	int p0;

	for (;;) {
		int c;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{ "number",	required_argument,  NULL, 'n' },
			{ "wait",	required_argument,  NULL, 'w' },
			{ "recvfd",	no_argument,	    NULL, 'r' },
			{ "wronly",	required_argument,  NULL, 'W' },
			{ "rdonly",	no_argument,	    NULL, 'R' },
			{ "nonblock",	no_argument,	    NULL, 'B' },
			{ "quiet",	no_argument,	    NULL, 'q' },
			{ "verbose",	optional_argument,  NULL, 'v' },
			{ "help",	no_argument,	    NULL, 'h' },
			{ "version",	no_argument,	    NULL, 'V' },
			{ "copying",	no_argument,	    NULL, 'C' },
			{ "?",		no_argument,	    NULL, 'h' },
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "n:w:rWRBqv::hVC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "n:w:rWRBqv::hVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'n':	/* -n, --number */
			n = strtol(optarg, NULL, 0);
			break;
		case 'w':	/* -w, --wait */
			wt = strtol(optarg, NULL, 0);
			break;
		case 'r':	/* -r, --recvfd */
			recv_fd = 1;
			break;
		case 'W':	/* -W, --wronly */
			o_wronly = 1;
			o_rdonly = 0;
			break;
		case 'R':	/* -R, --rdonly */
			o_rdonly = 1;
			o_wronly = 0;
			break;
		case 'B':	/* -B, --nonblock */
			o_nonblock = 1;
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
	o_flags =
	    (o_wronly ? O_WRONLY : (o_rdonly ? O_RDONLY : O_RDWR)) | (o_nonblock ? O_NONBLOCK : 0);

	if (n <= 1)
		n = 1;

	for (i = 0; i < n; i++) {
		p0 = optind;
		while (argc > p0) {
			strcpy(path, argv[p0++]);

			if ((fd = open(path, o_flags)) < 0) {
				fprintf(stderr, "open( \"%s\", 0%o ) failed: %s (%d)\n", path,
					o_flags, strerror(errno), errno);
				exit(1);
			}

			if (verbose) {
				fstat(fd, &stat);
				printf("%d: fd %d = open( \"%s\", 0%o ) "
				       "mode 0%o dev 0x%04x rdev 0x%04x", i, fd, path, o_flags,
				       (int) (stat.st_mode), (int) (stat.st_dev),
				       (int) (stat.st_rdev));
				is_stream = isastream(fd);
				printf("%s", (is_stream ? " [STREAM]" : ""));
				printf("\n");
				fflush(stdout);
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
				if (wt && !poll(&pfd, 1, wt * 1000)) {
					/* 
					 *  poll timed out
					 */
					if (verbose)
						fprintf(stderr,
							"poll(..., %dms) timed out waiting for RECVFD\n",
							wt * 1000);
				} else {
					if (ioctl(fd, I_RECVFD, &recv) < 0) {
						if (verbose)
							fprintf(stderr,
								"ioctl(%d, I_RECVFD,...) failed: %s\n",
								fd, strerror(errno));
					} else {
						if (verbose) {
							fstat(recv.fd, &stat);
							printf("%d: fd %d = received"
							       " (mode 0%o dev 0x%04x rdev 0x%04x)",
							       i, recv.fd, (int) (stat.st_mode),
							       (int) (stat.st_dev),
							       (int) (stat.st_rdev));
							printf("%s",
							       (isastream(recv.fd) ? " [STREAM]" :
								""));
							printf("\n");
							fflush(stdout);
						}
					}
				}
			}
		}
	}
	if (wt > 0) {
		while (wt--) {
			if (verbose)
				fprintf(stderr, ".");
			sleep(1);
		}
		if (verbose)
			fprintf(stderr, "!\n");
	}

	exit(0);
}
