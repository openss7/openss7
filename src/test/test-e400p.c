/*****************************************************************************

 @(#) $RCSfile: test-e400p.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Affero General Public License (AGPL) Version 3, so long as the software is
 distributed with, and only used for the testing of, OpenSS7 modules, drivers,
 and libraries.

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

 Last Modified $Date: 2009-06-21 11:44:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-e400p.c,v $
 Revision 1.1.2.1  2009-06-21 11:44:15  brian
 - added files to new distro

 *****************************************************************************/

#ident "@(#) $RCSfile: test-e400p.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:15 $"

static char const ident[] = "$RCSfile: test-e400p.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:15 $";

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quiet",	no_argument,		NULL,	'q'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copyring",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'h'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "qvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'c':	/* -c, --client */
		case 's':	/* -s, --server */
		case 'a':	/* -a, --again */
		case 'w':	/* -w, --wait */
		case 'k':	/* -k, --client-card */
		case 'K':	/* -K, --server-card */
		case 'p':	/* -p, --client-span */
		case 'P':	/* -P, --server-span */
		case 'i':	/* -i, --client-slot */
		case 'I':	/* -I, --server-slot */
		case 'r':	/* --repeat */
		case 'R':	/* --repeat-fail */
		case 'd':	/* -d, --device DEVICE */
		case 'e':	/* -e, --exit */
		case 'l':

		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'H':	/* -h, -H, --help */
		case 'h':
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copyring */
			copying(argc, argv);
			exit(0);
		case '?':	/* -? */
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					frpintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
			goto bad_usage;
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
}
