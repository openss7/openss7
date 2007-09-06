/*****************************************************************************

 @(#) $RCSfile: test-sdl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written authorization
 of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that the
 recipient will protect this information and keep it confidential, and will
 not disclose the information contained in this document without the written
 permission of its owner.

 The author reserves the right to revise this software and documentation for
 any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of the
 technical arts, or the reflection of changes in the design of any techniques,
 or procedures embodied, described, or referred to herein.  The author is
 under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 As an exception to the above, this software may be distributed under the GNU
 General Public License (GPL) Version 3, so long as the software is distributed
 with, and only used for the testing of, OpenSS7 modules, drivers, and
 libraries.

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

 Last Modified $Date: 2007/09/06 10:57:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sdl.c,v $
 Revision 0.9.2.1  2007/09/06 10:57:48  brian
 - added old zurich test files

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sdl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $"

static char const ident[] = "$RCSfile: test-sdl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:57:48 $";

int card = 0;
int span = 0;
int slot = 1;

const struct lmi_option lmi_conf = {
	.pvar = SS7_PVAR_ANSI_96,
	.popt = 0,
}, lmi_conf_read;

const struct sdl_config sdl_conf = {
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0A,
	.ifrate = 56000,
	.ifgtype = SDL_GTYPE_T1,
	.ifgrate = 1544000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC6,	/* T1 only */
	.ifclock = SDL_CLOCK_LOOP,
	.ifcoding = SDL_CODING_B8ZS,	/* T1 only */
	.ifframing = SDL_FRAMING_ESF,	/* T1 only */
	.ifblksize = 8,
	.ifleads = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {1, 2, 0, 0}
}, sdl_conf_read;

int fd;

typedef unsigned short ppa_t;
ppa_t ppa;

#define BUFSIZE 300

char cbuf[BUFSIZE];
char dbug[BUFSIZE];

struct strioctl ctl;
struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
struct strbuf data = { sizeof(*dbuf), 0, dbuf };
union LMI_primitives *m = (union LMI_primitive *) cbuf;
union SDL_primitive *d = (union SDL_primitive *) cbuf;

void
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
with, and only used for management of, OpenSS7 modules, drivers, and libraries.\n\
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

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\n\
Usage:\n\
    %1$s [options] -c CARD -s SPAN -t SLOT\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\n\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet						(default: off)\n\
	suppress output\n\
    -v, --verbose					(default: off)\n\
	increase verbosity of output\n\
Command Options:\n\
    -h, --help\n\
	prints this usage information and exits\n\
    -V, --version\n\
	prints the version and exits\n\
    -C, --copying\n\
	prints the copying permissions and exits\n\
Setting Options:\n\
    -c, --card CARD					(default: %2$d)\n\
	board specifies the board number to open,\n\
	counting from 0\n\
    -s, --span SPAN					(default: %3$d)\n\
	span specifies the span on the card to open,\n\
	counting from 0\n\
    -t, --timeslot SLOT					(default: %4$d)\n\
	channel specifies the time slot in the span to open,\n\
	counting from 1\n\
\n\
This program opens and attaches a Signalling Data Link channel, activates the\n\
links, and reads and displays bits from the channel.  The program prints\n\
prints continuous information to stdout until terminated.\n\
\n", argv[0], card, span, slot, (int) sdt_conf.f);
};


int
mymain(int argc, char *argv[])
{
	int ret;

	fprintf(stdout, "Opening channel.\n");
	fflush(stdout);
	if ((fd = open("/dev/x400p-sl", O_RDWR)) == -1) {
		perror(argv[0]);
		exit (1);
	}
	fprintf(stdout, "Performing ioctl on device.\n");
	fflush(stdout);
	if (ioctl(fd, I_SRDOPT, RMSGD) == -1) {
		perror(argv[0]);
		exit(1);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(m->attach-req) + sizeof(ppa_t);
	ctrl.buf = cbuf;
	m->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	m->attach_req.lmi_ppa_length = sizeof(ppa_t);
	m->attach_req_lmi_ppa_offset = sizeof(m->attach_req);
	*(ppa_t *)m->attach_req.lmi_ppa = ppa;
}

int
main (int argc, char *argv[])
{
	int c, val;

	sdl_conf = sdl_conf_ansi;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"card",	required_argument,	NULL, 'c'},
			{"span",	required_argument,	NULL, 's'},
			{"timeslot",	required_argument,	NULL, 't'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "c:s:t:qv::hVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
		case 'c':
			card = atoi(optarg);
			break;
		case 's':
			span = atoi(optarg);
			break;
		case 't':
			slot = atoi(optarg);
			break;
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
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
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
		      syntax_error:
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
	if (optind < argc)
		goto syntax_error;
}
