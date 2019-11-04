/*****************************************************************************

 @(#) File: src/util/slconf.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 *****************************************************************************/

static char const ident[] = "src/util/slconf.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

static int verbose = 1;
static int debug = 0;

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif

#ifndef SLCONF_CONF_FILENAME
#define SLCONF_CONF_FILENAME "/etc/sysconfig/slconf.conf"
#endif

char filename[MAX_PATH_LENGTH] = SLCONF_CONF_FILENAME;

enum {
	SOURCE_NONE = 0,
	SOURCE_STDIN,
	SOURCE_FILE,
	SOURCE_CMDLINE
} source = SOURCE_NONE;

int interactive = 0;

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
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

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011, 2015, 2016, 2018, 2019  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, PACKAGE_ENVR " " PACKAGE_DATE);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-i|--interactive}\n\
    %1$s [options] {-f|--file}[=FILENAME]\n\
    %1$s [options] COMMAND\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-c|--command}] -- COMMAND[; COMMAND]*\n\
    %1$s [options] {-f|--file}[=FILENAME]\n\
    %1$s [options] {-i|--interactive}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    COMMAND\n\
General Options:\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -d, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
Command Options:\n\
    -c, --command\n\
        process remainder of arguments as command\n\
    -f, --file[=FILENAME]               (default: %2$s)\n\
        read configuration commands from file\n\
    -i, --interactive\n\
        enter interactive command shell\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], filename);
}

extern void yyinit(char *name, FILE *input, FILE *output);
extern int yyparse(void);

void
slconf_stdin(char *name)
{
    yyinit(name, stdin, stdout);
    while (yyparse() == 0) ;
    return;
}

void
slconf_file(char *name, const char *filename)
{
	FILE *f;

	if ((f = fopen(filename, "r")) == NULL) {
		perror(name);
		exit(1);
	}
	yyinit(name, f, stdout);
	while (yyparse() == 0) ;
	fclose(f);
	return;
}

int
main(int argc, char *argv[])
{
	int c;
	int val;
	char *optstr;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"command",	no_argument,		NULL,	'c'},
			{"file",	optional_argument,	NULL,	'f'},
			{"interactive",	no_argument,		NULL,	'i'},
			{"debug",	optional_argument,	NULL,	'd'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'h'},
			{NULL,		0,			NULL,	 0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, ":W:cf::id::v::qhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'c':
			if (source != SOURCE_NONE)
				goto bad_option;
			source = SOURCE_CMDLINE;
			break;
		case 'f':	/* -f, --file [FILENAME] */
			if (source != SOURCE_NONE)
				goto bad_option;
			if (optarg != NULL)
				strncpy(filename, optarg, sizeof(filename) - 1);
			source = SOURCE_FILE;
			break;
		case 'i':	/* -i, --interactive */
			if (source != SOURCE_NONE)
				goto bad_option;
			source = SOURCE_STDIN;
			break;
		case 'd':	/* -d, --debug [LEVEL] */
			if ((optstr = optarg) == NULL) {
				debug++;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if ((optstr = optarg) == NULL) {
				verbose++;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'q':	/* -q, --quiet */
			verbose -= verbose > 1 ? 1 : verbose;
			break;
		case 'h':	/* -h, --help [WHAT] */
			if (source != SOURCE_NONE)
				goto bad_option;
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (source != SOURCE_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (source != SOURCE_NONE)
				goto bad_option;
			copying(argc, argv);
			exit(0);
		case ':':	/* missing mandatory parameter */
			optind--;
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind]);
			usage(argc, argv);
			exit(2);
		case '?':	/* illegal option */
		default:
		      bad_option:
			optind--;
		      syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax --", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, " %s", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
		if (source == SOURCE_CMDLINE)
			break;
	}
      reevaluate:
	switch (source) {
	case SOURCE_NONE:
		if (optind < argc) {
			source = SOURCE_CMDLINE;
			goto reevaluate;
		}
		source = SOURCE_STDIN;
		goto reevaluate;
#if 0
	case SOURCE_CMDLINE:
		slconf_cmdline(argv[0], argc, argv, optind);
		break;
#endif
	case SOURCE_STDIN:
		if (optind < argc)
			goto syntax_error;
		interactive = 1;
		slconf_stdin(argv[0]);
		break;
	case SOURCE_FILE:
		if (optind < argc)
			goto syntax_error;
		slconf_file(argv[0], filename);
		break;
	default:
		exit(4);
		break;
	}
	exit(0);
}
