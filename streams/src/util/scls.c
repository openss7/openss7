/*****************************************************************************

 @(#) $RCSfile: scls.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2006/03/03 10:57:17 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/03/03 10:57:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: scls.c,v $
 Revision 0.9.2.20  2006/03/03 10:57:17  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.19  2006/02/20 10:59:30  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ident "@(#) $RCSfile: scls.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2006/03/03 10:57:17 $"

static char const ident[] =
    "$RCSfile: scls.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2006/03/03 10:57:17 $";

/* 
 *  AIX Utility: scls - Produces a list of module and driver names.
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <stropts.h>
#include <sys/sc.h>

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    %1$s [options] [{-c|--count}|{-l|--long}] [MODULE ...]\n\
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
    %1$s [options] [{-c|--count}|{-l|--long}] [MODULE ...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    MODULE ...\n\
	specific drivers and modules to list instead of all drivers and\n\
	modules\n\
Options:\n\
    -c, --count\n\
        print module_stats only\n\
    -l, --long\n\
        print module_info and module_stats\n\
    -q, --quiet\n\
        suppress output\n\
    -d, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
    -C, --copying\n\
        prints copying permissions and exits\n\
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
Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; version  2  of  the  License.\n\
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

enum { CMN_NONE, CMN_NAMES, CMN_LONG, CMN_COUNT, } command = CMN_NONE;

void
printit(struct sc_mlist *l, int cmd)
{
	int i;

	if (output <= 0 || l->major == -1)
		return;
	for (i = 0; i < 4; i++) {
		if (l->mi[i].mi_idnum == 0)
			continue;
		if (i > 0 && cmd != CMN_LONG && cmd != CMN_COUNT)
			continue;
		fprintf(stdout, "%-10s", l->name);
		if (cmd == CMN_LONG || cmd == CMN_COUNT) {
			switch (i) {
			case 0:
				fprintf(stdout, "   rd:");
				break;
			case 1:
				fprintf(stdout, "   wr:");
				break;
			case 2:
				fprintf(stdout, " muxr:");
				break;
			case 3:
				fprintf(stdout, " muxw:");
				break;
			}
		}
		switch (cmd) {
		case CMN_LONG:
			if (l->major != 0) {
				fprintf(stdout, "  device");
				fprintf(stdout, " %-3ld", (long) l->major);
			} else {
				fprintf(stdout, "  module");
				fprintf(stdout, "   -");
			}
			fprintf(stdout, "  %-5u", l->mi[i].mi_idnum);
			fprintf(stdout, "  %-6ld", (long) l->mi[i].mi_minpsz);
			fprintf(stdout, "  %-6ld", (long) l->mi[i].mi_maxpsz);
			fprintf(stdout, "  %-6ld", (long) l->mi[i].mi_hiwat);
			fprintf(stdout, "  %-6ld", (long) l->mi[i].mi_lowat);
		case CMN_COUNT:
			fprintf(stdout, "  %-8ld", (long) l->ms[i].ms_pcnt);
			fprintf(stdout, "  %-8ld", (long) l->ms[i].ms_scnt);
			fprintf(stdout, "  %-8ld", (long) l->ms[i].ms_ocnt);
			fprintf(stdout, "  %-8ld", (long) l->ms[i].ms_ccnt);
			fprintf(stdout, "  %-8ld", (long) l->ms[i].ms_acnt);
			fprintf(stdout, "  %08x", l->ms[i].ms_flags);
		}
		fprintf(stdout, "\n");
	}
};

int
main(int argc, char *argv[])
{
	int i, fd, count;
	struct sc_list *list;

	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"long",	no_argument,		NULL, 'l'},
			{"count",	no_argument,		NULL, 'c'},
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

		c = getopt_long_only(argc, argv, "lcqD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "lcqDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'l':
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting long command\n", argv[0]);
			command = CMN_LONG;
			break;
		case 'c':
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting count command\n", argv[0]);
			command = CMN_COUNT;
			break;
		case 'D':	/* -D, --debug */
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
			goto bad_nonopt;
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
	if (debug)
		fprintf(stderr, "%s: opening /dev/nuls\n", argv[0]);
	if ((fd = open("/dev/nuls", O_RDWR)) < 0) {
		if (debug)
			fprintf(stderr, "%s: could not open /dev/nuls\n", argv[0]);
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: pushing sc module\n", argv[0]);
	if (ioctl(fd, I_PUSH, "sc") < 0) {
		if (debug)
			fprintf(stderr, "%s: could not push sc module\n", argv[0]);
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: getting size of list\n", argv[0]);
	/* go out and get all the names anyway */
	if ((count = ioctl(fd, SC_IOC_LIST, NULL)) < 0) {
		if (debug)
			fprintf(stderr, "%s: could not perform SC_IOC_LIST command\n", argv[0]);
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: size of list = %d\n", argv[0], count);
	/* get entire list */
	if ((list = malloc(sizeof(struct sc_list) + count * sizeof(struct sc_mlist))) == NULL) {
		if (debug)
			fprintf(stderr, "%s: could not allocate memory\n", argv[0]);
		fprintf(stderr, "%s: %s\n", argv[0], strerror(ENOMEM));
		fflush(stderr);
		exit(1);
	}
	list->sc_nmods = count;
	list->sc_mlist = (struct sc_mlist *) (list + 1);
	if (ioctl(fd, SC_IOC_LIST, list) < 0) {
		if (debug)
			fprintf(stderr, "%s: could not perform second SC_IOC_LIST command\n", argv[0]);
		perror(argv[0]);
		exit(1);
	}
	if (optind < argc) {
		int ret = 0;

		/* have module name arguments - iterate through them */
		for (; optind < argc; optind++) {
			for (i = 0; i < count; i++)
				if (strncmp(argv[optind], list->sc_mlist[i].name, FMNAMESZ)
				    == 0)
					break;
			if (i < count)
				printit(&list->sc_mlist[i], command);
			else {
				fprintf(stderr, "%s: %s: %s\n", argv[0], argv[optind],
					strerror(ENXIO));
				fflush(stderr);
				ret = 1;
			}
		}
		exit(ret);
	} else {
		/* have no module name arguments - operate on all */
		for (i = 0; i < count; i++)
			printit(&list->sc_mlist[i], command);
	}
	if (ioctl(fd, I_POP, NULL) < 0) {
		perror(argv[0]);
	}
	if (close(fd) < 0) {
		perror(argv[0]);
	}
	exit(0);
}
