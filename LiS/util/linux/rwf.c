/*****************************************************************************

 @(#) $RCSfile: rwf.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $

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

 $Log: rwf.c,v $
 Revision 1.1.1.1.12.5  2005/07/13 12:01:24  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 1.1.1.1.12.3  2005/05/14 08:35:16  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: rwf.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $"

static char const ident[] =
    "$RCSfile: rwf.c,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2005/07/13 12:01:24 $";

/* 
 *  rwf.c - simple reading/writing test using poll() to get FIFO input.
 *
 *  This program could be changed to use getmsg()/putmsg() instead of
 *  read()/write(), hopefully in several obvious ways.  It has been
 *  written for read()/write() in order to be useful with non-STREAMS
 *  fifos.
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif
#include <stropts.h>
#include <sys/poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>

int verbose = 1;

static inline int
_getmsg(int fd, struct strbuf *sb)
{
	int len;

	if ((len = read(fd, sb->buf, sb->maxlen)) >= 0)
		sb->len = len;

	return (len < 0 ? len : 0);
}

static inline int
_putmsg(int fd, struct strbuf *sb)
{
	int len;

	if ((len = write(fd, sb->buf, sb->len)) >= 0)
		sb->len = len;

	return (len < 0 ? len : 0);
}

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
        The path to file.\n\
Options:\n\
    -m, --number NUMBER\n\
        number of iterations\n\
    -l, --length LENGTH\n\
        length of the message to send.\n\
    -w, --wait WAIT\n\
        set the wait in milliseconds [default: 0]\n\
    -r, --relay\n\
        push the relay module\n\
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
	int fd, j, m = 0, l = 1;
	struct stat st;
	int wt = 0;
	int is_stream = 0, push_relay = 0;

	for (;;) {
		int c;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{ "number",	required_argument,  NULL, 'm' },
			{ "length",	required_argument,  NULL, 'l' },
			{ "wait",	required_argument,  NULL, 'w' },
			{ "relay",	no_argument,	    NULL, 'r' },
			{ "quiet",	no_argument,	    NULL, 'q' },
			{ "verbose",	optional_argument,  NULL, 'v' },
			{ "help",	no_argument,	    NULL, 'h' },
			{ "version",	no_argument,	    NULL, 'V' },
			{ "copying",	no_argument,	    NULL, 'C' },
			{ "?",		no_argument,	    NULL, 'h' },
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "m:l:w:rqv::hVC?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "m:l:w:rqv::hVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'm':	/* -m, --number */
			m = strtol(optarg, NULL, 0);
			break;
		case 'l':
			l = strtol(optarg, NULL, 0);
			break;
		case 'w':	/* -w, --wait WAIT(ms) */
			wt = strtol(optarg, NULL, 0);
			break;
		case 'r':	/* -r, --relay */
			push_relay = 1;
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
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}

	if (argc == optind)
		goto bad_usage;

	strcpy(path, argv[optind]);

	if ((fd = open(path, O_RDWR | O_NONBLOCK)) < 0) {
		if (verbose)
			fprintf(stderr, "open(\"%s\", O_RDWR|O_NONBLOCK) failed: %s\n", path,
				strerror(errno));
		exit(1);
	}

	if (verbose > 1) {
		fstat(fd, &st);
		printf("%d: open \"%s\" (mode 0%o dev 0x%04x rdev 0x%04x)", fd, path,
		       (int) (st.st_mode), (int) (st.st_dev), (int) (st.st_rdev));
		is_stream = isastream(fd);
		printf("%s\n", (is_stream ? " [STREAM]" : ""));
	}
	if (is_stream) {
		if (push_relay) {
			if (ioctl(fd, I_PUSH, "relay") < 0) {
				if (verbose)
					fprintf(stderr, "ioctl(%d, I_PUSH, relay) failed: %s\n", fd,
						strerror(errno));
				exit(1);
			}
			if (verbose > 1)
				printf("module \"relay\" pushed onto %d\n", fd);
		}
	}

	if (m > 0) {
		struct pollfd pfd;
		struct strbuf sbo, sbi;

		pfd.fd = fd;
		pfd.events = POLLIN | POLLHUP;

		sbo.len = l;
		sbo.buf = malloc(l);
		memset(sbo.buf, 'x', l);

		sbi.maxlen = l;
		sbi.buf = malloc(l);

		for (j = 0; j < m; j++) {
			_putmsg(fd, &sbo);
			poll(&pfd, 1, wt);
			if (verbose > 1)
				printf("poll(%d,...) -> 0x%x\n", fd, pfd.revents);
			if (pfd.revents & POLLHUP) {
				if (verbose)
					fprintf(stderr, "POLLHUP received - exiting\n");
				break;
			} else if (!pfd.revents) {
				if (verbose)
					fprintf(stderr, "poll(%d,...) timed out - exiting\n",
						pfd.fd);
				break;
			}
			sbi.len = 0;
			_getmsg(fd, &sbi);
			if (verbose > 1)
				printf("read(%d,...) -> len %d\n", fd, sbi.len);
		}
	}

	exit(0);
}
