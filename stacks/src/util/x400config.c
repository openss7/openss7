/*****************************************************************************

 @(#) $RCSfile: x400config.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 $Log: x400config.c,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

#ident "@(#) $RCSfile: x400config.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: x400config.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";


/*
 * This is the main loop parser for the x400config program.
 */

#include <stropts.h>
#include <stdlib.h>
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
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>


static int output = 1;

static void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
General Public License (GPL) Version 3,  so long as the  software is distributed\n\
with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.\n\
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
");
}

static void
version(int argc, char *argv[])
{
	if (!output)
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
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-g, --get} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot]\n\
    %1$s [options] {-s, --set} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot] [set-options]\n\
    %1$s [options] {-f, --file} config_file\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] {-g, --get} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot]\n\
    %1$s [options] {-s, --set} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot] [set-options]\n\
    %1$s [options] {-f, --file} config_file\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet				(default: off)\n\
	suppress output\n\
    -v, --verbose			(default: off)\n\
	increase verbosity of output\n\
Command Options:\n\
    -g, --get\n\
	get (display) device configuration\n\
    -s, --set\n\
	set (configure) device configuration\n\
    -f, --file file			(default: %2$s)\n\
	read device configuration from file\n\
    -h, --help\n\
	prints this usage information and exits\n\
    -V, --version\n\
	prints the version and exits\n\
    -C, --copying\n\
	prints the copying permissions and exits\n\
Set/Get Options:\n\
    -d, --device devname		(default: %3$s)\n\
	device name\n\
    -c, --card card			(default: %4$d)\n\
	card in host (numbered from 0)\n\
    -p, --span span			(default: %5$d)\n\
	span on card (numbered from 0)\n\
    -t, --slot slot			(default: %6$d)\n\
	timeslot (numbered from 1, 0 for entire span)\n\
Set Options:\n\
    --clock				(default: %7$s)\n\
	span clock source: default int ext loop master slave dpll abr shaper tick\n\
    --rate				(default: %8$u)\n\
	channel interface rate: 56000 64000 1544000 2048000\n\
    --type				(default: %9$s)\n\
	channel interface type: default v35 ds0 ds0a e1 t1 j1 atm packet\n\
    --mode				(default: %10$s)\n\
	channel interface mode: default dsu csu dte dce client server peer echo rem_lb loc_lb lb_echo test\n\
    --grate				(default: %11$u)\n\
	span interface rate: 1544000 2048000 10000000 100000000\n\
    --gtype				(default: %12$s)\n\
	span interface type: default t1 e1 j1 atm eth ip udp tcp rtp sctp\n\
    --gmode				(default: %13$s)\n\
	span interface mode: default loc_lb rem_lb\n\
    --gcrc				(default: %14$s)\n\
	span CRC calulation: default crc4 crc5 crc6 crc6j\n\
    --coding				(default: %15$s)\n\
	span coding: default nrz nrzi ami b6zs b8zs aal1 aal2 aal5 hbd3\n\
    --framing				(default: %16$s)\n\
	span framing: ccs cas sf esf\n\
    --syncsrc sync1,sync2,sync3,sync4\n\
	span synchronization: 0 - self, 1 - span1 ...\n\
", argv[0], cfgfile, devname, card, span, slot, clock_names[config.ifclock].key, config.ifrate, type_names[config.iftype].key, mode_names[config.ifmode].key, config.ifgrate, gtype_names[config.ifgtype].key, gmode_names[config.ifgmode].key, gcrc_names[config.ifgcrc].key, coding_names[config.ifcoding].key, framing_names[config.ifframing].key);
}

int
main(int argc, char **argv)
{
	int c;
	int val;
	enum {
		CMD_NONE = 0,
		CMD_GET,		/* get device */
		CMD_SET,		/* set device */
		CMD_CFG,		/* configure from file */
	} cmd = CMD_NONE;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"get",		no_argument,		NULL, 'g'},
			{"set",		no_argument,		NULL, 's'},
			{"file",	required_argument,	NULL, 'f'},
			{"device",	required_argument,	NULL, 'd'},
			{"card",	required_argument,	NULL, 'c'},
			{"span",	required_argument,	NULL, 'p'},
			{"slot",	required_argument,	NULL, 't'},
			{"type",	required_argument,	NULL, CFG_TYPE + 1},
			{"gtype",	required_argument,	NULL, CFG_GTYPE + 1},
			{"mode",	required_argument,	NULL, CFG_MODE + 1},
			{"gmode",	required_argument,	NULL, CFG_GMODE + 1},
			{"gcrc",	required_argument,	NULL, CFG_GCRC + 1},
			{"clock",	required_argument,	NULL, CFG_CLOCK + 1},
			{"coding",	required_argument,	NULL, CFG_CODING + 1},
			{"framing",	required_argument,	NULL, CFG_FRAMING + 1},
			{"rate",	required_argument,	NULL, CFG_RATE + 1},
			{"grate",	required_argument,	NULL, CFG_GRATE + 1},
			{"txlevel",	required_argument,	NULL, CFG_TXLEVEL + 1},
			{"syncsrc",	required_argument,	NULL, CFG_SYNCSRC + 1},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "gsf:c:p:t:qvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'g':	/* -g, --get */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_GET;
			break;
		case 's':	/* -s, --set */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_SET;
			break;
		case 'f':	/* -f, --file file */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_CFG;
			strncpy(cfgfile, optarg, sizeof(cfgfile) - 1);
			break;
		case 'd':	/* -d, --device */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			strncpy(devname, optarg, sizeof(devname) - 1);
			break;
		case 'c':	/* -c, --card */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			card = atoi(optarg);
			break;
		case 'p':	/* -p, --span */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			span = atoi(optarg);
			break;
		case 't':	/* -t, --slot */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			slot = atoi(optarg);
			break;
		case CFG_TYPE + 1:	/* --type */
		case CFG_GTYPE + 1:	/* --gtype */
		case CFG_MODE + 1:	/* --mode */
		case CFG_GMODE + 1:	/* --gmode */
		case CFG_GCRC + 1:	/* --gcrc */
		case CFG_CLOCK + 1:	/* --clock */
		case CFG_CODING + 1:	/* --coding */
		case CFG_FRAMING + 1:	/* --framing */
		case CFG_RATE + 1:	/* --rate */
		case CFG_GRATE + 1:	/* --grate */
		case CFG_TXLEVEL + 1:	/* --txlevel */
		{
			int i;

			if (cmd != CMD_SET)
				goto bad_option;
			if ('0' <= optarg[0] && optarg[0] <= '9') {
				*(optnames[c - 1].valp) = atoi(optarg);
				break;
			}
			for (i = 0; i < optnames[c - 1].size; i++)
				if (!strcmp(optarg, optnames[c - 1].name[i].key))
					break;
			if (i < optnames[c - 1].size) {
				*(optnames[c - 1].valp) = optnames[c - 1].name[i].val;
				break;
			}
			fprintf(stderr, "%s: illegal parm -- %s\n", argv[0], argv[optind - 1]);
			fprintf(stderr, "%s: valid labels --", argv[0]);
			for (i = 0; i < optnames[c - 1].size; i++)
				fprintf(stderr, " %s", optnames[c - 1].name[i].key);
			fprintf(stderr, "\n");
			usage(argc, argv);
			exit(2);
		}
		case CFG_SYNCSRC + 1:	/* --syncsrc */
		{
			ulong syncs[4] = { 0, 0, 0, 0 };
			int i, j, p, len;

			if (cmd != CMD_SET)
				goto bad_option;
			len = strlen(optarg);
			for (p = 0, j = 0, i = 0; i < 4; i++) {
				for (; j <= len; j++)
					if (optarg[j] == ',' || optarg[j] == '\0')
						goto gotone;
				if (j > p) {
				      gotone:
					syncs[i] = atoi(&optarg[p]);
					if (output > 3)
						fprintf(stderr, "got one: %d : %lu\n", i, syncs[i]);
					j++;
					p = j;
					if (j >= len)
						break;
				}
			}
			if (i == 0)
				goto bad_option;
			for (j = 0; j <= i; j++)
				config.ifsyncsrc[j] = syncs[j];
			break;
		}
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --verbose */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
			if (cmd != CMD_NONE)
				goto bad_option;
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (cmd != CMD_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case 'C':
			if (cmd != CMD_NONE)
				goto bad_option;
			copying(argc, argv);
			exit(0);
		case ':':
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind - 1]);
			usage(argc, argv);
			exit(2);
		case '?':
		default:
		      bad_option:
			optind--;
			// syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	switch (cmd) {
	default:
	case CMD_NONE:
		help(argc, argv);
		exit(0);
	case CMD_GET:
		slconf_get();
		exit(0);
	case CMD_SET:
		slconf_set();
		exit(0);
	case CMD_CFG:
		slconf_cfg();
		exit(0);
	}
	exit(4);
}
