/*****************************************************************************

 @(#) $RCSfile: strchg.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $

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

 Last Modified $Date: 2004/03/04 23:15:47 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strchg.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $"

static char const ident[] = "$RCSfile: strchg.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $";

/* 
 * SVR 4.2 utility: strchg - Changes stream configuration.
 */

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

int verbose = 1;

void
version(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    included here by reference.\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-h|--push} MODULE_LIST\n\
    %1$s [options] (-p|--pop} [{-a|--all}|{-u|--upto} MODULE]\n\
    %1$s [options] (-f|--file} FILE\n\
    %1$s {-H, --help}\n\
    %1$s {-V, --version\n\
    %1$s {-C, --copying\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-h|--push} MODULE[,MODULE]\n\
    %1$s [options] (-p|--pop} [{-a|--all}|{-u|--upto} MODULE]\n\
    %1$s [options] (-f|--file} FILE\n\
    %1$s {-H, --help}\n\
    %1$s {-V, --version\n\
    %1$s {-C, --copying\n\
Options:\n\
    -h, --push MODULE[,MODULE]\n\
        specifies a module (or comma separated list of modules) to push\n\
	onto the stream\n\
    -p, --pop\n\
	requests that the topmost module be popped from the stream\n\
    -a, --all\n\
        specifies that all modules, not just the topmost module, are\n\
	to be popped from the stream\n\
    -u, --upto MODULE\n\
        specifies that modules up to, but not including, the specified\n\
	module are to be popped from the stream, instead of just the\n\
	topmost module\n\
    -f, --file FILE\n\
	specifies that the module stack is to conform to the module stack\n\
	specification contained in the specified file\n\
    -v, --verbose\n\
	verbose output\n\
    -q, --quiet\n\
	suppress normal output\n\
    -H, --help, -?\n\
	print this usage message and exits\n\
    -V, --version\n\
	print the version and exits\n\
    -C, --copying\n\
	print the copyright and exits\n\
", argv[0]);
}

void
copying(int argc, char *argv[])
{
	if (verbose < 0)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; either  version  2  of  the  License, or (at  your option) any later\n\
version.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received a copy of the GNU  General  Public License  along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
");
}

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#define OPTS_MAX 256

enum { CMN_NONE, CMN_PUSH, CMN_POP, CMN_POPUPTO, CMN_POPALL, CMN_FILE };

char fbuf[PATH_MAX] = { '\0', };
char mbuf[OPTS_MAX] = { '\0', };

int
main(int argc, char *argv[])
{
	char *mptr;
	int i, fd, flen = 0, mlen = 0, mnum = 1, command = CMN_NONE;
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"push",	required_argument,	NULL, 'h'},
			{"pop",		no_argument,		NULL, 'p'},
			{"upto",	required_argument,	NULL, 'u'},
			{"all",		no_argument,		NULL, 'a'},
			{"file",	required_argument,	NULL, 'f'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'H'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "h:pu:af:qvHVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "h:pu:af:qvHVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'h':	/* -h, --push modulelist */
			if (command != CMN_NONE && command != CMN_FILE)
				goto bad_option;
			command = CMN_PUSH;
			mlen = strnlen(optarg, OPTS_MAX);
			memset(mbuf, 0, OPTS_MAX);
			strncpy(mbuf, optarg, OPTS_MAX);
			/* count number of modules in list */
			for (i = 0, mnum = 1; i < mlen; i++) {
				if (mbuf[i] == ',') {
					mbuf[i] = '\0';
					mnum++;
				}
			}
			break;
		case 'p':	/* -p, --pop */
			if (command != CMN_NONE && command != CMN_FILE)
				goto bad_option;
			command = CMN_POP;
			break;
		case 'u':	/* -u, --upto module */
			if (command != CMN_POP)
				goto bad_option;
			mlen = strnlen(optarg, OPTS_MAX);
			strncpy(mbuf, optarg, OPTS_MAX);
			command = CMN_POPUPTO;
			break;
		case 'a':	/* -a, --all */
			if (command != CMN_POP)
				goto bad_option;
			command = CMN_POPALL;
			break;
		case 'f':	/* -f, --file filename */
			if (command != CMN_NONE)
				goto bad_option;
			command = CMN_FILE;
			flen = strnlen(optarg, PATH_MAX);
			strncpy(fbuf, optarg, PATH_MAX);
			break;
		case 'q':	/* -q, --quiet */
			verbose = -1;
			break;
		case 'v':	/* -v, --verbose [level] */
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'H':	/* -H, --help */
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
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					fprintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	if ((fd = fileno(stdin)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	switch (command) {
	case CMN_NONE:
	default:
		goto bad_nonopt;
	case CMN_PUSH:		/* -h, --push MODULE */
		for (i = 0, mptr = mbuf; i < mnum; i++, mptr += strlen(mbuf) + 1)
			if (ioctl(fd, I_PUSH, mbuf) < 0) {
				fprintf(stderr, "%s: %s: %s\n", argv[0], mptr, strerror(errno));
				goto recover;
			}
		break;
	      recover:
		while (i-- > 0)
			ioctl(fd, I_POP, NULL);
		exit(1);
	case CMN_POP:		/* -p, --pop */
		if (ioctl(fd, I_POP, NULL) < 0) {
			perror(argv[0]);
			exit(1);
		}
		break;
	case CMN_POPUPTO:	/* -p, --pop -u, --upto MODULE */
		switch (ioctl(fd, I_FIND, mbuf)) {
		case 1:
		{
		      struct str_mlist mlist = { l_name:{0,}, };
		      struct str_list list = { sl_nmods: 1, sl_modlist:&mlist, };
		      while (ioctl(fd, I_LIST, NULL) > 1) {
				list.sl_nmods = 1;
				if (ioctl(fd, I_LIST, &list) < 0) {
					perror(argv[0]);
					exit(1);
				}
				if (strncmp(mlist.l_name, mbuf, FMNAMESZ) == 0)
					break;
				if (ioctl(fd, I_POP, NULL) < 0) {
					perror(argv[0]);
					exit(1);
				}
			}
			break;
		}
		case 0:
			fprintf(stderr, "%s: %s: %s\n", argv[0], mbuf, strerror(EINVAL));
			exit(1);
		default:
			fprintf(stderr, "%s: %s: %s\n", argv[0], mbuf, strerror(errno));
			exit(1);
		}
		break;
	case CMN_POPALL:	/* -p, --pop, -a, --all */
		while (ioctl(fd, I_LIST, NULL) > 1) {
			if (ioctl(fd, I_POP, NULL) < 0) {
				perror(argv[0]);
				exit(1);
			}
		}
		break;
	case CMN_FILE:		/* -f, --file FILE */
	{
		break;
	}
	}
	exit(0);
}
