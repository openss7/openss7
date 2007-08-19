/*****************************************************************************

 @(#) $RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $

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

 Last Modified $Date: 2007/08/19 05:19:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ss7statsd.c,v $
 Revision 0.9.2.1  2007/08/19 05:19:33  brian
 - added more daemon files

 *****************************************************************************/

#ident "@(#) $RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $"

static char const ident[] =
    "$RCSfile: ss7statsd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/19 05:19:33 $";

/*
 * This is ss7statsd(8).  The purpose of the daemon is to spawn a daemon process to collect
 * statistics and log them to a statistics collection log.
 */

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

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif				/* MAX_PATH_LENGTH */

int verbose = 1;
int debug = 0;
int nomead = 1;
int useconfig = 0;

long interval = 300000;			/* 5 minutes in milliseconds. */

#ifndef SS7STATSD_DEFAULT_OUTPDIR
#define SS7STATSD_DEFAULT_OUTPDIR "/var/log" "/" PACKAGE
#endif				/* SS7STATSD_DEFAULT_OUTPDIR */
#ifndef SS7STATSD_DEFAULT_OUTFILE
#define SS7STATSD_DEFAULT_OUTFILE NAME ".out"
#endif				/* SS7STATSD_DEFAULT_OUTFILE */
#ifndef SS7STATSD_DEFAULT_ERRFILE
#define SS7STATSD_DEFAULT_ERRFILE NAME ".err"
#endif				/* SS7STATSD_DEFAULT_ERRFILE */
#ifndef SS7STATSD_DEFAULT_CFGFILE
#define SS7STATSD_DEFAULT_CFGFILE PACKAGE_CONFIGDIR "/" NAME ".conf"
#endif				/* SS7STATSD_DEFAULT_CFGFILE */
#ifndef SS7STATSD_DEFAULT_DEVNAME
#define SS7STATSD_DEFAULT_DEVNAME "/dev/ss7-stats"
#endif				/* SS7STATSD_DEFAULT_DEVNAME */

char outpdir[MAX_PATH_LENGTH] = "";
char outfile[MAX_PATH_LENGTH] = "";
char errfile[MAX_PATH_LENGTH] = "";
char cfgfile[MAX_PATH_LENGTH] = "";
char outpath[MAX_PATH_LENGTH] = "";
char errpath[MAX_PATH_LENGTH] = "";
char devname[MAX_PATH_LENGTH] = "";

void
copying(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
This program is  free  software:  you can redistribute it and/or modify it under\n\
the terms of the  GNU General Public License Version 3  as published by the Free\n\
Software Foundation, found in the distributed information file  \"COPYING\",  with\n\
the Section 7 conditions found in the file \"CONDITIONS\".\n\
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
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.1 $ $Date: 2007/08/19 05:19:33 $");
}

void
version(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permission.\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.1 $ $Date: 2007/08/19 05:19:33 $");
}

void
usage(const char *name)
{
	fprintf(stderr, "\
\n\
Usage:\n\
    %1$s [{-i,--interval=}INTERVAL] [{-n,--nodaemon}] [{-O,--outpdir=}DIRECTORY]\n\
         [{-o,--outfile=}OUTFILE] [{-l,--logfile=}LOGFILE]\n\
         [{-f,--cfgfile[=]}[CFGFILE]] [{-e,--devname=}DEVNAME]\n\
         [{-q,--quiet}] [{-d,--debug[=]}[LEVEL]] [{-v,--verbose[=]}[LEVEL]]\n\
    %1$s {-h, -?, --?, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
\n\
    See `%1$s --help' for detailed information.\n\
\n\
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
    %1$s: [options]\n\
    %1$s: {-h, -?, --?, --help}\n\
    %1$s: {-V, --version}\n\
    %1$s: {-C, --copying}\n\
\n\
Arguments (non-option):\n\
    (none)\n\
\n\
Options:\n\
\n\
  Command Options:\n\
    {-n, --nodaemon}\n\
        run the stats daemon in the foreground\n\
    {-h, -?, --?, --help}\n\
        print this usage information and exit\n\
    {-V, --version}\n\
        print version information and exit\n\
    {-C, --copying}\n\
        print copying permissions and exit\n\
\n\
  Daemon Options:\n\
    {-i, --interval=}INTERVAL                       (default: %4$ld)\n\
        statistics collection interval milliseconds\n\
    {-O, --outpdir=}DIRECTORY                       (default: %5$s)\n\
        specify directory for output files\n\
    {-o, --outfile=}FILENAME                        (default: %6$s)\n\
        specify output filename\n\
    {-l, --logfile=}FILENAME                        (default: %7$s)\n\
        specify error log filename\n\
    {-f, --cfgfile[=]}[FILENAME]                    (default: %8$s)\n\
        specify configuration file and name\n\
    {-e, --devname=}DEVNAME                         (default: %9$s)\n\
        specify device name\n\
\n\
  General Options:\n\
    {-q, --quiet}\n\
        suppress normal output\n\
    {-d, --debug[=]}[LEVEL]                         (default: %2$d)\n\
        increase or set debugging output level\n\
    {-v, --verbose[=]}[LEVEL]                       (default: %3$d)\n\
        increase or set output verbosity level\n\
\n\
", NAME, debug, verbose, interval, SS7STATSD_DEFAULT_OUTPDIR, SS7STATSD_DEFAULT_OUTFILE, SS7STATSD_DEFAULT_ERRFILE, SS7STATSD_DEFAULT_CFGFILE, SS7STATSD_DEFAULT_DEVNAME);
}

void
option_error(int argc, char *argv[], int optind, const char *str, int retval)
{
	fprintf(stderr, "ERROR: %s: %s --", str, basename(argv[0]));
	while (optind < argc)
		fprintf(stderr, " %s", argv[optind++]);
	fprintf(stderr, "\n");
	usage(basename(argv[0]));
	exit(retval);
}

int
main(int argc, char *argv[])
{
	int c;
	long val;
	char *optstr;
	struct stat st;

	for (;;) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"interval",	required_argument,	NULL, 'i'},
			{"nodaemon",	no_argument,		NULL, 'n'},
			{"outpdir",	required_argument,	NULL, 'O'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"logfile",	required_argument,	NULL, 'l'},
			{"cfgfile",	optional_argument,	NULL, 'f'},
			{"device",	required_argument,	NULL, 'e'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"?",		no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 },
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, ":w:i:nO:o:l:f:e:qd::v::hVC?", long_options,
				&option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'i':	/* -i, --interval INTERVAL */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || optstr[0] != '\0' || val < 1000)
				option_error(argc, argv, optind - 1, "illegal argument", 2);
			interval = val;
			break;
		case 'n':	/* -n, --nodaemon */
			nomead = 0;
			break;
		case 'O':	/* -O, --outpdir DIRECTORY */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (stat(optarg, &st) == -1)
				option_error(argc, argv, optind - 1, strerror(errno), 2);
			if (!S_ISDIR(st.st_mode))
				option_error(argc, argv, optind - 1, "not a directory", 2);
			strncpy(outpdir, optarg, sizeof(outpdir));
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (optarg[0] != '/') {
				strncpy(outfile, optarg, sizeof(outfile));
				break;
			}
			strncpy(outpath, optarg, sizeof(outpath));
			break;
		case 'l':	/* -l, --logfile LOGFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			if (optarg[0] != '/') {
				strncpy(errfile, optarg, sizeof(errfile));
				break;
			}
			strncpy(errpath, optarg, sizeof(errpath));
			break;
		case 'f':	/* -f, --cfgfile [CFGFILE] */
			if (optarg != NULL)
				strncpy(cfgfile, optarg, sizeof(cfgfile));
			useconfig = 1;
			break;
		case 'e':	/* -e, --device DEVNAME */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "required argument", 2);
			strncpy(devname, optarg, sizeof(devname));
			break;
		case 'q':	/* -q, --quiet */
			verbose -= verbose > 1 ? 1 : verbose;
			break;
		case 'd':	/* -d, --debug [LEVEL] */
			if (optarg != NULL) {
				val = strtol(optarg, &optstr, 0);
				if (optstr == optarg || optstr[0] != '\0' || val < 0)
					option_error(argc, argv, optind - 1, "illegal argument", 2);
				debug = val;
			}
			debug++;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg != NULL) {
				val = strtol(optarg, &optstr, 0);
				if (optstr == optarg || optstr[0] != '\0' || val < 0)
					option_error(argc, argv, optind - 1, "illegal argument", 2);
				verbose = val;
			}
			verbose++;
			break;
		case 'h':	/* -h, --help */
			help(basename(argv[0]));
			exit(0);
		case 'V':	/* -V, --version */
			version(basename(argv[0]));
			exit(0);
		case 'C':	/* -C, --copying */
			copying(basename(argv[0]));
			exit(0);
		case ':':	/* required argument */
			option_error(argc, argv, optind - 1, "required argument", 2);
		case '0':
		case '?':	/* unrecognized option */
			option_error(argc, argv, optind - 1, "unrecognized option", 2);
		default:	/* unrecognized switch */
			option_error(argc, argv, optind - 1, "syntax error", 2);
		}
	}
	if (optind < argc)
		option_error(argc, argv, optind, "extra arguments", 2);
	/* organize path names */
	if (outpath[0] == '\0') {
		if (outpdir[0] == '\0')
			strncpy(outpdir, SS7STATSD_DEFAULT_OUTPDIR, sizeof(outpdir));
		if (outfile[0] == '\0')
			strncpy(outfile, SS7STATSD_DEFAULT_OUTFILE, sizeof(outfile));
		snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
	}
	if (errpath[0] == '\0') {
		if (outpdir[0] == '\0')
			strncpy(outpdir, SS7STATSD_DEFAULT_OUTPDIR, sizeof(outpdir));
		if (errfile[0] == '\0')
			strncpy(errfile, SS7STATSD_DEFAULT_ERRFILE, sizeof(errfile));
		snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
	}
	if (cfgfile[0] == '\0') {
		strncpy(cfgfile, SS7STATSD_DEFAULT_CFGFILE, sizeof(cfgfile));
	}
	exit(0);
}
