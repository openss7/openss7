/*****************************************************************************

 @(#) File: src/util/strxt.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

static char const ident[] = "src/util/strxt.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

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

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

enum tclass {
	TC_4_CLNS,
	TC_0_CONS,
	TC_2_CONS,
	TC_4_CONS,
};

struct testconfig {
	enum tclass tclass;
	int bytes;
	int connections;
	char data[BUFSIZ+1];
	char nsap[40+1];
	char responder[4+1];
	int seconds;
	int vary;
};

static struct testconfig config = {
	.tclass = TC_4_CLNS,
	.bytes = 16256,
	.connections = 1,
	.data = "00",
	.nsap = "",
	.responder = "",
	.seconds = 1,
	.vary = 0,
};

static void
do_nsap(int argc, char *argv[], int start)
{
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>\n\
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

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2015  Monavacon Limited.\n\
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

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
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
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    NSAP\n\
        NSAP address of the remote user\n\
Options:\n\
  Command: (-l assumed if none given)\n\
    -l, --clns\n\
        specifies TP Class 4 and CLNS\n\
    -0, --class0\n\
        specifies TP Class 0 and CONS\n\
    -2, --class2\n\
        specifies TP Class 2 and CONS\n\
    -4, --class4\n\
        specifies TP Class 4 and CONS\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
  Common:\n\
    -b, --bytes BYTES\n\
        size of TPDU [default: %2$d]\n\
    -c, --connections VALUE\n\
        maximum number of connections [default: %3$d]\n\
    -d, --data STRING\n\
        data payload for packet [default: %4$s]\n\
    -n, --nsap ADDRESS\n\
        remote NSAP or DTE address [default: %5$s]\n\
    -r, --responder SELECTOR\n\
        responder TSAP selector [default: %6$s]\n\
    -s, --seconds SECONDS\n\
        seconds between reports [default: %7$d]\n\
    -v, --vary SIZE\n\
        vary the TIDU size [default: %8$d]\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -x, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
", argv[0], config.bytes, config.connections, config.data, config.nsap, config.responder, config.seconds, config.vary);
}

#define COMMAND_DFLT  0
#define COMMAND_TEST  1
#define COMMAND_HELP  2
#define COMMAND_VERS  3
#define COMMAND_COPY  4

int
main(int argc, char *argv[])
{
	int command = COMMAND_DFLT;
	int c, val, len, bad;
	int start;

	for (;;) {
#if defined _GNU_SOURCE
		int option_index = 0;
                /* *INDENT-OFF* */
                static struct option long_options[] = {
			{"clns",	no_argument,		NULL, 'l'},
			{"class0",	no_argument,		NULL, '0'},
			{"class2",	no_argument,		NULL, '2'},
			{"class4",	no_argument,		NULL, '4'},
			{"bytes",	required_argument,	NULL, 'b'},
			{"connections",	required_argument,	NULL, 'c'},
			{"data",	required_argument,	NULL, 'd'},
			{"nsap",	required_argument,	NULL, 'n'},
			{"responder",	required_argument,	NULL, 'r'},
			{"seconds",	required_argument,	NULL, 's'},
			{"vary",	required_argument,	NULL, 'v'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'x'},
			{"verbose",	optional_argument,	NULL, 'o'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
                };
                /* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "qx::o::hVC?W:", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "qx:ohVC?");
#endif				/* _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'l':	/* -l, --clns */
			if (command != COMMAND_DFLT)
				goto bad_option;
			command = COMMAND_TEST;
			config.tclass = TC_4_CLNS;
			break;
		case '0':	/* -0, --class0 */
			if (command != COMMAND_DFLT)
				goto bad_option;
			command = COMMAND_TEST;
			config.tclass = TC_0_CONS;
			break;
		case '2':	/* -2, --class2 */
			if (command != COMMAND_DFLT)
				goto bad_option;
			command = COMMAND_TEST;
			config.tclass = TC_2_CONS;
			break;
		case '4':	/* -4, --class4 */
			if (command != COMMAND_DFLT)
				goto bad_option;
			command = COMMAND_TEST;
			config.tclass = TC_4_CONS;
			break;
		case 'b':	/* -b, --bytes BYTES */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.bytes = val;
			break;
		case 'c':	/* -c, --connections NUMBER */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((val = strtoul(optarg, NULL, 0)) <= 0)
				goto bad_option;
			config.connections = val;
			break;
		case 'd':	/* -d, --data STRING */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((len = strlen(optarg)) < 2 || len > BUFSIZ || (len & 0x1))
				goto bad_option;
			/* check for hexadecimal digits only */
			if ((bad = strspn(optarg, "0123456789abcdefABCDEF")) < len) {
				if (output || debug)
					fprintf(stderr, "%s: invalid hexadecimal character '%c' in data", argv[0], optarg[bad]);
				goto bad_option;
			}
			strncpy(config.data, optarg, sizeof(config.data));
			break;
		case 'n':	/* -n, --nsap NSAP */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			len = strnlen(optarg, BUFSIZ + 1);
			if (len < 1 || len > 40) {
				if (output || debug)
					fprintf(stderr, "%s: invalid NSAP/DTE length %d\n", argv[0], len);
				goto bad_option;
			}
			if ((optarg[len-2] == 'f' || optarg[len-2] == 'F') &&
			    (optarg[len-1] == 'e' || optarg[len-1] == 'E')) {
				if (len != 14) {
					if (output || debug)
						fprintf(stderr, "%s: invalid NSAP length %d\n", argv[0], len);
					goto bad_option;
				}
				/* check for hexadecimal digits only */
				if ((bad = strspn(optarg, "0123456789abcdefABCDEF")) < len) {
					if (output || debug)
						fprintf(stderr, "%s: invalid hexadecimal character '%c' in data", argv[0], optarg[bad]);
					goto bad_option;
				}
			} else {
				/* check for decimal digits only */
				if ((bad = strspn(optarg, "0123456789")) < len) {
					if (output || debug)
						fprintf(stderr, "%s: invalid decimal character '%c' in data", argv[0], optarg[bad]);
					goto bad_option;
				}
			}
			strncpy(config.nsap, optarg, sizeof(config.nsap));
			break;
		case 'r':	/* -r, --responder TSEL */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((len = strlen(optarg)) != 4) {
				if (output || debug)
					fprintf(stderr, "%s: invalid TSEL length %d\n", argv[0], len);
				goto bad_option;
			}
			strncpy(config.responder, optarg, 4);
			break;
		case 's':	/* -s, --seconds SECONDS */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((val = strtoul(optarg, NULL, 0)) <= 0)
				goto bad_option;
			config.seconds = val;
			break;
		case 'v':	/* -v, --vary OCTETS */
			if (command == COMMAND_DFLT)
				command = COMMAND_TEST;
			if ((val = strtoul(optarg, NULL, 0)) < 0)
				goto bad_option;
			config.vary = val;
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'x':	/* -x, --debug [level] */
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
		case 'o':	/* -o, --verbose [level] */
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
			command = COMMAND_HELP;
			break;
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			command = COMMAND_VERS;
			break;
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			command = COMMAND_COPY;
			break;
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
	if (optind < argc) {
		if (debug)
			fprintf(stderr, "%s: excess non-option arguments\n", argv[0]);
		goto bad_nonopt;
	}
	switch (command) {
	case COMMAND_DFLT:
	case COMMAND_TEST:
		do_nsap(argc, argv, start);
		break;
	case COMMAND_HELP:
		help(argc, argv);
		break;
	case COMMAND_VERS:
		version(argc, argv);
		break;
	case COMMAND_COPY:
		copying(argc, argv);
		break;
	}
	exit(0);
}
