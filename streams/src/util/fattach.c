/*****************************************************************************

 @(#) $RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/04/28 01:30:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/28 01:30:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/04/28 01:30:34 $"

static char const ident[] =
    "$RCSfile: fattach.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/04/28 01:30:34 $";

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
#include <sys/ioctl.h>

int output = 1;

void version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Distributed under GPL Version 2, included here by reference.\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] -p PATH ...\n\
    %1$s [options] -c PATH ...\n\
    %1$s [options] DEVICE PATH ...\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] -p PATH ...\n\
    %1$s [options] -c PATH ...\n\
    %1$s [options] DEVICE PATH ...\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version\n\
Arguments:\n\
    DEVICE\n\
	the STREAMS device to open and attach\n\
    PATH ...\n\
	the path or paths in the directory to which to attach the pipe end or\n\
	device.\n\
Options:\n\
    -p, --pipe\n\
	create a pipe to attach to PATH\n\
    -c, --connld\n\
	push connld on the first side of a the pipe (implies -p)\n\
    -m, --mode\n\
	apply the mode of the old file after attaching to PATH\n\
    -u, --umask\n\
	apply the umask after attaching to PATH\n\
    -M, --mode MODE\n\
	apply mode MODE after attaching to PATH\n\
    -v, --verbose\n\
	verbose output\n\
    -q, --quiet\n\
	suppress normal output\n\
    -h, --help, -?\n\
	print this usage message and exits\n\
    -V, --version\n\
	print the version and exits\n\
", argv[0]);
}

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int main(int argc, char *argv[])
{
	char path[PATH_MAX];
	int fd[2];
	int use_pipe = 0, x, push_connld = 0;
	int use_mode = 0, use_new_mode = 0, um, new_mode[2];
	struct stat st;
	um = umask(0);
	umask(um);
	for (;;) {
		int c;
#ifdef _GNU_SOURCE
		int option_index = 0;
		static struct option long_options[] = {
			/* *INDENT-OFF* */
			{"umask",	no_argument,		NULL, 'u'},
			{"mode",	optional_argument,	NULL, 'M'},
			{"pipe",	no_argument,		NULL, 'p'},
			{"connld",	no_argument,		NULL, 'c'},
			{"verbose",	no_argument,		NULL, 'v'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"version",	no_argument,		NULL, 'V'},
			{"help",	no_argument,		NULL, 'h'},
			{"?",		no_argument,		NULL, 'h'},
			/* *INDENT-ON* */
		};
		c = getopt_long_only(argc, argv, "muM:pcvqVh?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "muM:pcvqVh?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'm':	/* -m */
			use_mode = 1;
			break;
		case 'u':	/* -u, --umask */
			use_new_mode = 1;
			new_mode[0] = new_mode[1] = (0666 & ~um);
			break;
		case 'M':	/* -M, --mode [MODE] */
			if (optarg == NULL) {
				use_mode = 1;	/* --mode */
			} else {
				use_new_mode = 1;	/* -M, --mode MODE */
				new_mode[0] = new_mode[1] = strtoul(optarg, NULL, 0);
			}
			break;
		case 'p':	/* -p, --pipe */
			use_pipe = use_new_mode = 1;
			new_mode[0] = new_mode[1] = (0666 & ~um);
			break;
		case 'c':	/* -c, --connld */
			push_connld = use_pipe = use_new_mode = 1;
			new_mode[0] = new_mode[1] = (0666 & ~um);
			break;
		case 'v':	/* -v, --verbose */
			output += 1;
			break;
		case 'q':	/* -q, --quiet */
			output = 0;
			break;
		case 'h':	/* -h, --help, -?, --? */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		default:
		case '?':
			optind--;
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
	if (argc - optind < 1) {
		if (output)
			fprintf(stderr, "%s: missing path", argv[0]);
		goto bad_usage;
	}
	if (argc - optind < 2 && !use_pipe) {
		if (output)
			fprintf(stderr, "%s: missing streams device name", argv[0]);
		goto bad_usage;
	}
	if (use_pipe) {
		if (pipe(fd) < 0) {
			if (output)
				fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
			exit(1);
		}
		if (use_mode && !use_new_mode) {
			if (!(fstat(fd[0], &st) < 0))
				new_mode[0] = st.st_mode;
			if (!(fstat(fd[1], &st) < 0))
				new_mode[1] = st.st_mode;
		}
		if (push_connld) {
			if (ioctl(fd[0], I_PUSH, "connld") < 0) {
				if (output)
					fprintf(stderr, "ioctl( %d, I_PUSH, connld ) failed: %s\n",
						fd[0], strerror(errno));
				exit(1);
			}
		}
	} else {
		strcpy(path, argv[optind++]);
		if ((fd[0] = open(path, O_RDONLY | O_NONBLOCK)) < 0) {
			if (output)
				fprintf(stderr, "open failed for device %s: %s (%d)\n", path,
					strerror(errno), errno);
			exit(1);
		}
		if (!isastream(fd[0])) {
			if (output)
				fprintf(stderr, "\"%s\" is not a STREAMS device\n", path);
			exit(1);
		}
		fd[1] = fd[0];
		if (use_mode && !use_new_mode) {
			if (!(fstat(fd[0], &st) < 0))
				new_mode[0] = new_mode[1] = st.st_mode;
		}
	}
	if (output > 1) {
		if (use_pipe) {
			fstat(fd[0], &st);
			printf("pipe() fd[0]=%d mode 0%o dev 0x%x rdev 0x%x", fd[0],
			       (int) st.st_mode, (int) st.st_dev, (int) st.st_rdev);
			printf("%s", (push_connld ? " <connld>" : ""));
			printf(" [%s]\n", (isastream(fd[0]) ? "STREAM" : "Linux"));
			fstat(fd[1], &st);
			printf("pipe() fd[1]=%d mode 0%o dev 0x%x rdev 0x%x", fd[1],
			       (int) st.st_mode, (int) st.st_dev, (int) st.st_rdev);
			printf(" [%s]\n", (isastream(fd[1]) ? "STREAM" : "Linux"));
		} else {
			fstat(fd[0], &st);
			printf("\"%s\": mode 0%o dev 0x%x rdev 0x%x [STREAM]\n", path,
			       (int) st.st_mode, (int) st.st_dev, (int) st.st_rdev);
		}
	}
	x = 0;
	while (argc > optind) {
		strcpy(path, argv[optind++]);
		if (use_mode && !use_new_mode)
			if (!(stat(path, &st) < 0))
				new_mode[x] = st.st_mode;
		if (fattach(fd[x], path) < 0) {
			if (output)
				fprintf(stderr, "fattach( %d, \"%s\" ) failed: %s\n", fd[x], path,
					strerror(errno));
			exit(1);
		} else {
			if (output > 1)
				printf("fattach( %d, \"%s\" ) OK\n", fd[x], path);
		}
		x ^= 1;
	}
	if (use_mode || use_new_mode)
		fchmod(fd[0], new_mode[0]);
	close(fd[0]);
	if (use_pipe) {
		if (use_mode || use_new_mode)
			fchmod(fd[1], new_mode[1]);
		close(fd[1]);
	}
	exit(0);
}
