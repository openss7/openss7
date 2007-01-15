/*****************************************************************************

 @(#) $RCSfile: slconfd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/01/15 11:33:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slconfd.c,v $
 Revision 0.9.2.1  2007/01/15 11:33:57  brian
 - added new and old signalling link utilities

 *****************************************************************************/

#ident "@(#) $RCSfile: slconfd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $"

static char const ident[] = "$RCSfile: slconfd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:57 $";

/*
 *  This is a configuration daemon for the SL-MUX multiplexing driver.  Its purpose is to open
 *  Signalling Link streams on various drivers, attach them to specific PPAs, and link the SL
 *  streams under the SL-MUX multiplexing driver, configure each SL lower multiplex Stream, and then
 *  wait in the background listening for management events on a management stream.  Also, the daemon
 *  watches for updates for the configuration file and applies the updates as they occur.
 *
 *  The daemon is intended to run at system startup from a System V init script.  Once the
 *  configuration of the SL-MUX multiplexing driver is complete, applications programs can open,
 *  attach, enable and use Signalling Link streams opened on the driver.
 *
 *  The purpose of the SL-MUX driver is to provide a unified interface for accessing signalling
 *  links of a number of forms and from a number of various drivers.  Signalling links could
 *  correspond to device drivers, or could be M2UA or M2PA signalling links, or other
 *  implementations.  M2PA signalling links require some additional configuration before they can be
 *  used and will be linked under the SL-MUX by a separate M2PA configuration daemon.
 *
 *  Another possibility is to implement plugable dlopened modules for various devices.  A plugable
 *  module would define its own syntax for the last fields in the configuration file entry (after
 *  the selector).  New selectors could then be defined.
 */

int
main(int argc, char **argv)
{
	int c;
	int val;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "qvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --versbose */
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
		case '?':
		default:
		      bad_option:
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
	exit(0);
}
