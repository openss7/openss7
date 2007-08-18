/*****************************************************************************

 @(#) $RCSfile: slconf.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 Last Modified $Date: 2007/08/18 03:53:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slconf.c,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

#ident "@(#) $RCSfile: slconf.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: slconf.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";

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
copying(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s: %2$s\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
This program is  free  software:  you can redistribute it and/or modify it under\n\
the terms of the  GNU General Public License  as published by the  Free Software\n\
Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will be useful, but  WITHOUT ANY\n\
WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You  should  have received a copy of the  GNU General Public License  along with\n\
this program.   If not, see <http://www.gnu.org/licenses/>, or write to the Free\n\
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
Commercial licensing and  support  of  this  software is available from  OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
\n\
", name, ident);
}

void
version(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2003-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 3,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permission.\n\
\n\
", name, ident);
}

void
usage(const char *name)
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-i, --interactive}\n\
    %1$s [options] {-f, --file}[=FILENAME]\n\
    %1$s [options] COMMAND\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", name);
}

void
help(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] [{-c, --command}] -- COMMAND[; COMMAND]*\n\
    %1$s [options] {-f, --file}[=FILENAME]\n\
    %1$s [options] {-i, --interactive}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    COMMAND\n\
General Options:\n\
    -q, --quiet                         (default: off)\n\
        suppress output, for shell scripts\n\
    -v, --verbose                       (default: off)\n\
        increase verbosity of output\n\
Command Options:\n\
    -c, --command\n\
        process remainder of arguments as command\n\
    -f, --file[=FILENAME]               (default: %2$s)\n\
        read configuration commands from file\n\
    -i, --interactive\n\
        enter interactive command shell\n\
    -h, --help\n\
        print this usage information and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
\n\
", name, filename);
}

extern int yyparse(void);
extern void yyinit(const char *, FILE *, FILE *);
extern void optlex_init(int, char *[], int);
extern void optlex_term(void);

void
slconf_cmdline(const char *name, int argc, char *argv[], int optind)
{
	interactive = 0;
	optlex_init(argc, argv, optind);
	yyparse();
	optlex_term();
}

void
slconf_stdin(const char *name)
{
	yyinit(name, stdin, stdout);
	while (yyparse() == 0) ;
	return;
}
void
slconf_file(const char *name, const char *filename)
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
				strncpy(filename, optarg, sizeof(filename));
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
			help(argv[0]);
			exit(0);
		case 'V':	/* -V, --version */
			if (source != SOURCE_NONE)
				goto bad_option;
			version(argv[0]);
			exit(0);
		case 'C':	/* -C, --copying */
			if (source != SOURCE_NONE)
				goto bad_option;
			copying(argv[0]);
			exit(0);
		case ':':	/* missing mandatory parameter */
			optind--;
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind]);
			usage(argv[0]);
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
			usage(argv[0]);
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
	case SOURCE_CMDLINE:
		slconf_cmdline(argv[0], argc, argv, optind);
		break;
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
