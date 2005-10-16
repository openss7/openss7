/*****************************************************************************

 @(#) $RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/10/16 05:31:43 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/10/16 05:31:43 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/10/16 05:31:43 $"

static char const ident[] =
    "$RCSfile: strconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/10/16 05:31:43 $";

/* 
 * SVR 4.2 Utility: strconf - Queries stream configuration.
 */

#define _XOPEN_SOURCE 600

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

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-a|--all}]\n\
    %1$s [options] {-t|--topmost}\n\
    %1$s [options] {-m|--module MODULE}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-a|--all}]\n\
    %1$s [options] {-t|--topmost}\n\
    %1$s [options] {-m|--module MODULE}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Options:\n\
    [-a, --all]\n\
        print all module names and driver name, one per line\n\
    -t, --topmost\n\
        print only the topmost module (if one exists)\n\
    -m, --module MODULE\n\
        print 'yes' and return zero if MODULE exists on Stream\n\
        print 'no' and returns non-zero if MODULE does not exist on Stream\n\
    -q, --quiet\n\
        suppress normal output\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL (default: 0)\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL (default: 1)\n\
    -h, --help, -?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0]);
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>\n\
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
", ident);
}

enum { CMN_NONE, CMN_ALL, CMN_MODULE, CMN_TOPMOST };

char modname[FMNAMESZ + 1] = { '\0', };

int
main(int argc, char *argv[])
{
	int command = CMN_NONE;

	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
                        {"all",         no_argument,            NULL, 'a'},
			{"topmost",	no_argument,		NULL, 't'},
			{"module",	required_argument,	NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "atm:qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "atm:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'a':	/* -a, --all */
			if (command != CMN_NONE)
				goto bad_option;
			command = CMN_ALL;
			break;
		case 't':	/* -t, --topmost */
			if (command != CMN_NONE)
				goto bad_option;
			command = CMN_TOPMOST;
			break;
		case 'm':	/* -m, --module module */
			if (optarg == NULL)
				goto bad_option;
			if (command != CMN_NONE)
				goto bad_option;
			if (strnlen(optarg, FMNAMESZ + 1) > FMNAMESZ)
				goto bad_option;
			strncpy(modname, optarg, FMNAMESZ + 1);
			command = CMN_MODULE;
			break;
		case 'D':	/* -D, --debug [level] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'v':	/* -v, --verbose [level] */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	{
		int fd;
		int count;
		ssize_t size;
		struct str_list *slp;
		struct str_mlist *smlp;

		if (debug)
			fprintf(stderr, "%s: Finding file descriptor for standard input.\n", __FUNCTION__);
		if ((fd = fileno(stdin)) == -1) {
			perror(argv[0]);
			exit(1);
		}
		if (debug)
			fprintf(stderr, "%s: Determining number of modules on list.\n", __FUNCTION__);
		if ((count = ioctl(fd, I_LIST, NULL)) == -1) {
			if (errno == EINVAL)
				errno = ENOSTR;
			perror(argv[0]);
			exit(1);
		}
		size = sizeof(*slp) + count * sizeof(*smlp);
		if (debug)
			fprintf(stderr, "%s: Allocating memory for %d names.\n", __FUNCTION__, count);
		if (!(slp = malloc(size))) {
			perror(argv[0]);
			exit(1);
		}
		memset(slp, 0, size);
		smlp = (typeof(smlp)) (slp + 1);
		slp->sl_nmods = count;
		slp->sl_modlist = smlp;
		if (debug)
			fprintf(stderr, "%s: Listing %d names.\n", __FUNCTION__, count);
		if (ioctl(fd, I_LIST, slp) == -1) {
			perror(argv[0]);
			free(slp);
			exit(1);
		}
		if (debug)
			fprintf(stderr, "%s: Executing command.\n", __FUNCTION__);
		switch (command) {
		case CMN_NONE:
		case CMN_ALL:
		{
			int i;

			if (debug)
				fprintf(stderr, "%s: Listing all %d names.\n", __FUNCTION__, count);
			if (output)
				for (i = 0; i < count; i++)
					fprintf(stdout, "%s\n", smlp[i].l_name);
			free(slp);
			exit(0);
		}
		case CMN_TOPMOST:
		{
			if (count) {
				if (output)
					fprintf(stdout, "%s\n", smlp[0].l_name);
				free(slp);
				exit(0);
			} else {
				if (output)
					fprintf(stderr, "%s: no modules in stream\n", argv[0]);
				free(slp);
				exit(1);
			}
		}
		case CMN_MODULE:
		{
			int i;

			for (i = 0; i < count; i++) {
				if (strncmp(modname, smlp[i].l_name, FMNAMESZ) == 0) {
					if (output)
						fprintf(stdout, "yes\n");
					free(slp);
					exit(0);
				}
			}
			if (output)
				fprintf(stdout, "no\n");
			exit(1);
		}
		}
	}
	exit(0);
}
