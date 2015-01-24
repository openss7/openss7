/*****************************************************************************

 @(#) File: src/util/ss7capsetdir.c

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

static char const ident[] = "File: " "src/util/ss7capsetdir.c" "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

#include <stropts.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include <pcap/pcap.h>

/*
 * Simple program to set the direction to 2 on monitored links or to
 * set the direction to 0 or 1 and merge spans in the ppa.
 */

int output = 1;
int debug = 0;
int lasterr = 0;

int infiles = 1;

char outfile[256] = "/dev/stdout";
char errfile[256] = "/dev/stderr";
char inpfile[256] = "/dev/stdin";

char errbuf[PCAP_ERRBUF_SIZE] = "";

struct message {
	int eof;
	int valid;
	struct pcap_pkthdr *hdr;
	const u_char *dat;
} msgs;

int settotwo = 3; // default
int settodir = 0;

#define MTP2_HDR_LEN			4	/* length of the pseudo-header */

#define MTP2_SENT_OFFSET		0	/* 1 byte */
#define MTP2_ANNEX_A_USED_OFFSET	1	/* 1 byte */
#define MTP2_LINK_NUMBER_OFFSET		2	/* 2 bytes */

#define MTP2_ANNEX_A_NOT_USED		0
#define MTP2_ANNEX_A_USED		1
#define MTP2_ANNEX_A_USED_UNKNOWN	2

static void
ss7capsetdir(void)
{
	pcap_t *p;
	pcap_dumper_t *pd;
	FILE *f;
	pcap_t *pi;

	// open input file
	if (debug)
		fprintf(stderr, "opening input file %s\n", inpfile);
	if ((f = fopen(inpfile, "r")) == NULL) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "opening pcap file %s\n", inpfile);
	if ((pi = pcap_fopen_offline(f, errbuf)) == NULL) {
		fprintf(stderr, "error: %s: %s\n", __FUNCTION__, errbuf);
		exit(1);
	}
	msgs.eof = 0;
	msgs.valid = 0;
	msgs.hdr = NULL;
	msgs.dat = NULL;
	if (debug)
		fprintf(stderr, "opening output file %s\n", outfile);
	if (strncmp(outfile, "/dev/stdout", sizeof(outfile) - 1) != 0) {
		// redirect standard output
		if (freopen(outfile, "w", stdout) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	if (debug)
		fprintf(stderr, "opening error file %s\n", errfile);
	if (strncmp(errfile, "/dev/stderr", sizeof(errfile) - 1) != 0) {
		// redirect standard error
		if (freopen(errfile, "w", stderr) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	if (debug)
		fprintf(stderr, "creating pcap handle\n");
	if ((p = pcap_open_dead(DLT_MTP2_WITH_PHDR, 0)) == NULL) {
		pcap_perror(p, (char *) __FUNCTION__);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "opening pcap dump file %s\n", outfile);
	if ((pd = pcap_dump_fopen(p, stdout)) == NULL) {
		pcap_perror(p, (char *) __FUNCTION__);
		exit(1);
	}
	for (;;) {
		int rtn;
		u_char *psu;

		if (debug > 3)
			fprintf(stderr, "reading messages from input file\n");
		rtn = pcap_next_ex(pi, &msgs.hdr, &msgs.dat);
		msgs.eof = (rtn == -2);
		if (msgs.eof) {
			if (debug)
				fprintf(stderr, "hit end of input file\n");
			break;
		}
		msgs.valid = (rtn == 1);
		if (!msgs.valid) {
			if (debug > 3)
				fprintf(stderr, "error reading message from input file\n");
			break;
		}
		psu = (u_char *) msgs.dat;
		if (settotwo) {
			int dir = settotwo;

			psu[MTP2_SENT_OFFSET] = dir;
		} else if (settodir) {
			int card, span, slot, dir;

			// our link numbers are 2 bits for card number, 2 bits for span on card, and 5 bits
			// for slot in span Note that a slot number of zero means entire span (Annex A).
			card = ((psu[MTP2_LINK_NUMBER_OFFSET  ] & 0x01) << 1) |
			       ((psu[MTP2_LINK_NUMBER_OFFSET+1] & 0x80) >> 7);
			span = ((psu[MTP2_LINK_NUMBER_OFFSET+1] & 0x60) >> 5);
			slot = ((psu[MTP2_LINK_NUMBER_OFFSET+1] & 0x1f) >> 0);

			dir = span & 0x1;
			span &= ~0x1;

			psu[MTP2_SENT_OFFSET] = dir;
			psu[MTP2_LINK_NUMBER_OFFSET] = (card >> 1) & 0x1;
			psu[MTP2_LINK_NUMBER_OFFSET + 1] = ((card & 0x1) << 7) | ((span & 0x3) << 5) | (slot & 0x1f);
		}
		// write it out
		pcap_dump((u_char *) pd, msgs.hdr, msgs.dat);
		pcap_dump_flush(pd);
	}
	if (debug) {
		fprintf(stderr, "closing pcap dump file\n");
		fflush(stderr);
	}
	pcap_close(p);
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
Copyright (c) 2008, 2009, 2010, 2011, 2012, 2015  Monavacon Limited.\n\
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
    %1$s [options] [{-o|--outfile} outfile] [{-e|--errfile} errfile] [infile]\n\
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
    %1$s [options] [{-o|--outfile} outfile] [{-e|--errfile} errfile] [infile]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    infile                              (default: %2$s)\n\
        input file to read pcap formatted data\n\
Options:\n\
  Direction Options:\n\
    -2, --settotwo                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 2\n\
    -3, --settothr                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 3\n\
    -4, --settofor                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 4\n\
    -d, --settodir                      (default: %8$d)\n\
        set direction in MTP2 pseudo-header to span\n\
        and merge spans\n\
  File Options:\n\
    -o, --outfile outfile               (default: %3$s)\n\
        output file to write pcap formatted data\n\
    -e, --errfile errfile               (default: %4$s)\n\
        error log file to which to write errors\n\
  General Options:\n\
    -q, --quiet                         (default: off)\n\
        suppress normal output\n\
    -D, --debug [LEVEL]                 (default: %5$d)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]               (default: %6$d)\n\
        increment or set verbosity LEVEL\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], inpfile, outfile, errfile, debug, output, settotwo, settodir);
}

int
main(int argc, char **argv)
{
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"settotwo",	no_argument,		NULL,	'2'},
			{"settothr",	no_argument,		NULL,	'3'},
			{"settofor",	no_argument,		NULL,	'4'},
			{"settodir",	no_argument,		NULL,	'd'},
			{"outfile",	required_argument,	NULL,	'o'},
			{"errfile",	required_argument,	NULL,	'e'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"debug",	optional_argument,	NULL,	'D'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'H'},
			{NULL,		0,			NULL,	 0 }

		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "234do:e:qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "234do:e:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			break;
		}
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case '2': /* -2, --settotwo */
			settotwo = 2;
			settodir = 0;
		case '3': /* -3, --settothr */
			settotwo = 3;
			settodir = 0;
		case '4': /* -4, --settofor */
			settotwo = 4;
			settodir = 0;
			break;
		case 'd': /* -d, --settodir */
			settodir = 1;
			settotwo = 0;
			break;
		case 'o':	/* -o, --outfile outfile */
			if (debug)
				fprintf(stderr, "%s: setting outfile to %s\n", argv[0], optarg);
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'e':	/* -e, --errfile errfile */
			if (debug)
				fprintf(stderr, "%s: setting errfile to %s\n", argv[0], optarg);
			strncpy(errfile, optarg, sizeof(errfile) - 1);
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'D':	/* -D, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
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
					fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
				goto bad_usage;
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (optind < argc) {
		infiles = 0;
		for (;optind < argc && infiles < 1; optind++, infiles++) {
			if (debug)
				fprintf(stderr, "%s: assigning input file to %s\n", argv[0], argv[optind]);
			strncpy(inpfile, argv[optind], sizeof(inpfile) - 1);
		}
		if (debug) {
			int i;

			for (i = 0; i < infiles; i++)
				fprintf(stderr, "%s: input file was assigned %s\n", argv[0], inpfile);
		}
	}
	ss7capsetdir();
	exit(4);
}
