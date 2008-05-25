/*****************************************************************************

 @(#) $RCSfile: dlinfo.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-05-25 12:46:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlinfo.c,v $
 Revision 0.9.2.1  2008-05-25 12:46:57  brian
 - added manual pages, libraries, utilities and drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: dlinfo.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:57 $"

static char const ident[] = "$RCSfile: dlinfo.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:57 $";

/*
 * NAME
 *  dlinfo - print information about the datalink provider
 * SYNOPSIS:
 *	dlinfo device ppa sap
 *
 * Open datalink provider.
 * Attach to PPA.
 * Bind to sap
 * Get DL_INFO_ACK.
 * Print DL_INFO_ACK information.
 */

#include	<sys/types.h>
#include	<sys/stropts.h>
#include	<sys/dlpi.h>
#include	<sys/signal.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	"dltest.h"

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

static int debug = 0;
static int output = 1;

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2008  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under AGPL Version 3, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] DEVICE PPA SAP\n\
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
    %1$s [options] DEVICE PPA SAP\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    DEVICE\n\
        path to the device name to open\n\
    PPA\n\
        value of the Physical Point of Attachment to attach\n\
    SAP\n\
        value of the SAP to bind\n\
Options:\n\
    -q, --quiet\n\
        suppress output\n\
    -D, --debug [LEVEL]\n\
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
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms fo the  GNU  Affero  General  Public  License as published by the Free\n\
Software Foundation; Version 3 of the License.\n\
\n\
This program is distributed in the hope that it will be useful, but  WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of  MECHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.  If not, see <http://www.gnu.org/licenses/>,  or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
as  \"Commercial  Computer  Software\"  under  paragraph  252.277-7014  of the DoD\n\
Supplement to the  Federal Acquisition Regulations  (\"DFARS\")  (or any successor\n\
regulations)  and the  Government  is acquiring only  the license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is classified as \"Restricted  Computer  Software\" and the Government's rights in\n\
the  Software  are defined in paragraph  52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86  of the  NASA Supplement  to the  FAR  (or any successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support  of this software  is available from OpenSS7\n\
Corporation at a fee.  See <http://www.openss7.com/>.\n\
--------------------------------------------------------------------------------\n\
", ident);
}

int
main(int argc, char *argv[])
{
	int options;
	long buf[MAXDLBUF];		/* aligned on long */
	union DL_primitives *dlp;
	char *device;
	int ppa;
	int fd;
	int sap;

	dlp = (union DL_primitives *) buf;

	for (options = 0;; options++) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
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

		c = getopt_long_only(argc, argv, "qD::v::hVC?", long_options, &option_index);
#else
		c = getopt(argc, argv, "qD::v::hVC?");
#endif
		if (c == -1) {
			if (debug)
				(void) fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 'q':
			if (debug)
				(void) fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			continue;
		case 'D':
			if (debug)
				(void) fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
				continue;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			continue;
		case 'v':
			if (debug)
				(void) fprintf(stderr, "%s: inccreasing output verbosity\n",
					       argv[0]);
			if (optarg == NULL) {
				output++;
				continue;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			continue;
		case 'h':
		case 'H':
			if (debug)
				(void) fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':
			if (debug)
				(void) fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':
			if (debug)
				(void) fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copyring(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output > 0 || debug > 0) {
				if (optind < argc) {
					const char *delim = "";

					(void) fprintf(stderr, "%s: syntax error near '", argv[0]);

					while (optind < argc) {
						(void) fprintf(stderr, "%s%s", delim,
							       argv[optind++]);
						delim = " ";
					}
					(void) fprintf(stderr, "'\n");
				} else {
				      missing_arg:
					(void) fprintf(stderr, "%s: missing option or argument",
						       argv[0]);
					(void) fprintf(stderr, "\n");
				}
				fflush(stderr);
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (argc - optind < 3)
		goto missing_arg;

	device = argv[++optind];
	ppa = atoi(argv[++optind]);
	sap = atoi(argv[++optind]);

	/* 
	 * Open the device.
	 */
	if (debug > 0)
		(void) fprintf(stderr, "%s: opening device %s\n", argv[0], device);
	if ((fd = open(device, O_RDWR)) < 0) {
		(void) fprintf(stderr, "%s: %s: %s\n", argv[0], device, strerror(errno));
		exit(1);
	}

	/* 
	 * Attach.
	 */
	if (debug > 0)
		fprintf(stderr, "%s: attaching PPA %X\n", argv[0], (dl_ulong) ppa);
	dlattachreq(fd, ppa);
	if (debug > 1)
		fprintf(stderr, "%s: expecting DL_OK_ACK\n", argv[0]);
	dlokack(fd, buf);

	/* 
	 * Bind.
	 */
	if (debug > 0)
		fprintf(stderr, "%s: bind SAP %X\n", argv[0], (dl_ulong) localsap);
	dlbindreq(fd, sap, 0, DL_CLDLS, 0, 0);
	if (debug > 1)
		fprintf(stderr, "%s: expecting DL_BIND_ACK\n", argv[0]);
	dlbindack(fd, buf);

	/* 
	 * Issue DL_INFO_REQ.
	 */
	if (debug > 0)
		fprintf(stderr, "%s: getting information\n", argv[0]);
	dlinforeq(fd);
	if (debug > 1)
		fprintf(stderr, "%s: expecting DL_INFO_ACK\n", argv[0]);
	dlinfoack(fd, buf);

	/* 
	 * Print DL_INFO_ACK values.
	 */
	printdlprim(dlp);

	return (0);
}
