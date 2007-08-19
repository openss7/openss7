/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is ss7statsd(8).  The purpose of the daemon is to spawn a daemon process to collect
 * statistics and log them to a statistics collection log.
 */

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif				/* MAX_PATH_LENGTH */

int verbose = 1;
int debug = 0;

long interval = 300000;			/* 5 minutes in milliseconds. */

#ifndef SS7STATSD_DEFAULT_OUTPDIR
#define SS7STATSD_DEFAULT_OUTPDIR "/var/log/ss7"
#endif				/* SS7STATSD_DEFAULT_OUTPDIR */
#ifndef SS7STATSD_DEFAULT_OUTFILE
#define SS7STATSD_DEFAULT_OUTFILE "ss7statsd.out"
#endif				/* SS7STATSD_DEFAULT_OUTFILE */
#ifndef SS7STATSD_DEFAULT_ERRFILE
#define SS7STATSD_DEFAULT_ERRFILE "ss7statsd.err"
#endif				/* SS7STATSD_DEFAULT_ERRFILE */
#ifndef SS7STATSD_DEFAULT_CFGFILE
#define SS7STATSD_DEFAULT_CFGFILE "/etc/sysconfig/ss7statsd.conf"
#endif				/* SS7STATSD_DEFAULT_CFGFILE */
#ifndef SS7STATSD_DEFAULT_DEVNAME
#define SS7STATSD_DEFAULT_DEVNAME "/dev/ss7-stats"
#endif				/* SS7STATSD_DEFAULT_DEVNAME */

char *outpdir[MAX_PATH_LENGTH] = "";
char *outfile[MAX_PATH_LENGTH] = "";
char *errfile[MAX_PATH_LENGTH] = "";
char *cfgfile[MAX_PATH_LENGTH] = "";
char *outpath[MAX_PATH_LENGTH] = "";
char *errpath[MAX_PATH_LENGTH] = "";
char *devname[MAX_PATH_LENGTH] = "";

void
copying(const char *name)
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s: %2$s\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation  <http://www.openss7.com/>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software: you can redistribute it and/or modify it under\n\
the terms of the GNU General Public License as published by the Free Software\n\
Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will be useful, but WITHOUT\n\
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n\
details.\n\
\n\
You should have received a copy of the GNU General Public License along with\n\
this program.  If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
successor regulations) and the Government is acquiring only the license rights\n\
granted herein (the license rights customarily provided to non-Government\n\
users).  If the Software is supplied to any unit or agency of the Government\n\
other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
(or any successor regulations).\n\
\n\
Commercial licensing and support of this software is available from OpenSS7\n\
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
Version: %2$s\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation  <http://www.openss7.com/>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for permissions.\n\
\n\
", name, ident);
}

void
usage(const char *name)
{
	fprintf(stderr, "\
\n\
Usage:\n\
    %1$s [{-i,--interval=}INTERVAL] [{-n,--nodaemon}] [{-O,--outpdir=}DIRECTORY]\n\
	 [{-o,--outfile=}OUTFILE] [{-l,--logfile=}LOGFILE]\n\
	 [{-f,--cfgfile=}CFGFILE] [{-e,--devname=}DEVNAME]\n\
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
Options:\n\
  Command Options:\n\
    {-n, --nodaemon}\n\
        run the stats daemon in the foreground\n\
    {-h, -?, --?, --help}\n\
        print this usage information and exit\n\
    {-V, --version}\n\
        print version information and exit\n\
    {-C, --copying}\n\
        print copying permissions and exit\n\
  Daemon Options:\n\
    {-i, --interval=}INTERVAL			    (default: %5$ld)\n\
	statistics collection interval in milliseconds\n\
    {-O, --outpdir=}DIRECTORY			    (default: %6$s)\n\
	specify directory for output files\n\
    {-o, --outfile=}FILENAME			    (default: %7$s)\n\
	specify output filename\n\
    {-l, --logfile=}FILENAME			    (default: %8$s)\n\
	specify error log filename\n\
    {-f, --cfgfile=}FILENAME			    (default: %9$s)\n\
	specify configuration filename\n\
    {-e, --devname=}DEVNAME			    (default: %10$s)\n\
	specify device name\n\
  General Options:\n\
    {-q, --quiet}\n\
        suppress normal output\n\
    {-d, --debug[=]}[LEVEL]			    (default: %3$d)\n\
        increase or set debugging output level\n\
    {-v, --verbose[=]}[LEVEL]			    (default: %4$d)\n\
        increase or set output verbosity level\n\
\n\
", name, ident, debug, verbose, interval, SS7STATSD_DEFAULT_OUTPDIR, SS7STATSD_DEFAULT_OUTFILE, SS7STATSD_DEFAULT_LOGFILE, SS7STATSD_DEFAULT_CFGFILE, SS7STATSD_DEFAULT_DEVNAME);
}

void
option_error(int argc, char *argv[], int optind, const char *str, int retval)
{
	fprintf(stderr, "%s --", str);
	while (optind < argc)
		fprintf(stderr, " %s", argv[optind++]);
	fprintf(stderr, '\0');
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
			{"cfgfile",	required_argument,	NULL, 'f'},
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
				option_error(argc, argv, optind - 1, "missing argument", 2);
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
				option_error(argc, argv, optind - 1, "missing argument", 2);
			if (stat(optarg, &st) == -1)
				option_error(argc, argv, optind - 1, strerror(errno), 2);
			if (!S_ISDIR(st.st_mode))
				option_error(argc, argv, optind - 1, "not a directory", 2);
			strncpy(outpdir, optarg, sizeof(outpdir));
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "missing argument", 2);
			if (optarg[0] != '/') {
				strncpy(outfile, optarg, sizeof(outfile));
				break;
			}
			strncpy(outpath, optarg, sizeof(outpath));
			break;
		case 'l':	/* -l, --logfile LOGFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "missing argument", 2);
			if (optarg[0] != '/') {
				strncpy(errfile, optarg, sizeof(errfile));
				break;
			}
			strncpy(errpath, optarg, sizeof(errpath));
			break;
		case 'f':	/* -f, --cfgfile CFGFILE */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "missing argument", 2);
			strncpy(cfgfile, optarg, sizeof(cfgfile));
			break;
		case 'e':	/* -e, --device DEVNAME */
			if (optarg == NULL)
				option_error(argc, argv, optind - 1, "missing argument", 2);
			strncpy(devname, optarg, sizeof(devname));
			break;
		case 'q':	/* -q, --quiet */
		      verbose -= verbose > 1: 1:verbose;
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
			help(argv[0]);
			exit(0);
		case 'V':	/* -V, --version */
			version(argv[0]);
			exit(0);
		case 'C':	/* -C, --copying */
			copying(argv[0]);
			exit(0);
		case ':':	/* missing required argument */
			option_error(argc, argv, optind - 1, "missing argument", 2);
		case '0':
		case '?':	/* unrecognized option */
			option_error(argc, argv, optind - 1, "unrecognized option", 2);
		default:	/* unrecognized switch */
			option_error(argc, argv, optind - 1, "syntax error", 2);
		}
	}
	if (optind < optarg)
		option_error(argc, argv, optind, "trailing arguments", 2);
	
}
