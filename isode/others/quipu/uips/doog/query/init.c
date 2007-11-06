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

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/init.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/init.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*
 * Log: init.c,v
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  init.c - Initialize query engine

*****************************************************************************/

#include "types.h"
#include "request.h"
#include "ds_tasks.h"
#include "util.h"
#include "read.h"

#include "quipu/util.h"
#include "quipu/photo.h"
#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/dua.h"
#include "tailor.h"

extern int print_parse_errors;		/* Quipu variable to disable printing of errors */
extern char *oidtable,			/* Name of oidtable to use */
*tailfile,				/* Name of dsaptailor file to use */
*myname,				/* Name of dsa to bind to */
*dsa_address;

char username[LINESIZE];
char userpassword[LINESIZE];

QBool want_photo_as_bytes = FALSE;

int quipu_pe_cmp();

static void read_args(), read_quipurc();

/*
 * - init_query_engine() -
 *
 */
void
init_query_engine(argc, argv)
	int argc;
	char *argv[];
{
	FILE *config_file;
	char linebuf[LINESIZE];

	int test_ufn_paths();

	username[0] = userpassword[0] = '\0';

	print_parse_errors = FALSE;	/* Stop auto printing of errors */
	quipu_syntaxes();

#ifdef USE_PP
	pp_quipu_init("doog");
#endif

	dsap_init((int *) NULL, (char ***) NULL);

#ifdef USE_PP
	pp_quipu_run();
#endif

	read_args(argc, argv);

	/* 
	 *    Read `dsaptailor' file
	 */

	if ((config_file = fopen(isodefile(tailfile, 0), "r")) == (FILE *) NULL) {
		(void) fprintf(stderr, "Cannot open `dsaptailor' file. Attempting to continue.\n");
	} else {
		while (fgets(linebuf, sizeof(linebuf), config_file) != NULLCP)
			if ((*linebuf != '#') && (*linebuf != '\n'))
				(void) tai_string(linebuf);

		(void) fclose(config_file);
	}

	/* 
	 *    Read users' .quipurc
	 */

	read_quipurc();

	if (dsa_address == NULLCP)
		dsa_address = myname;

	if (want_photo_as_bytes)
		set_attribute_syntax(str2syntax("photo"),
				     (IFP) pe_cpy,
				     NULLIFP,
				     NULLIFP,
				     photo2xbm,
				     (IFP) pe_cpy, quipu_pe_cmp, pe_free, NULLCP, NULLIFP, TRUE);
}				/* init_query_engine */

/*
 * - read_quipurc() -
 *
 *
 */
static void
read_quipurc()
{
	extern char *SkipSpace(), *TidyString(), *getenv();
	char quipurc_path[LINESIZE], line[LINESIZE];
	char *quipurc_name = "/.quipurc";
	char *p, *part1, *part2;
	FILE *config_file;

	(void) strcpy(quipurc_path, getenv("HOME"));
	(void) strcat(quipurc_path, quipurc_name);

	if ((config_file = fopen(quipurc_path, "r")) == 0)
		return;

	while (fgets(line, LINESIZE, config_file) != 0) {
		p = SkipSpace(line);

		/* Ignore comments and blanks */

		if (*p == '#' || *p == '\0')
			continue;

		part1 = p;
		if ((part2 = index(p, ':')) == NULLCP)
			continue;	/* ignore it */

		*part2++ = '\0';
		part2 = TidyString(part2);

		/* 
		 *        Read users DN
		 */
		if ((lexequ(part1, "username") == 0) && *username == '\0')
			(void) strcpy(username, part2);

		/* 
		 *        Read password
		 */
		else if ((lexequ(part1, "password") == 0) && *userpassword == '\0')
			(void) strcpy(userpassword, part2);

		/* 
		 *        Dsap options
		 */
		else if (lexequ(part1, "dsap") == 0)
			(void) tai_string(part2);

		/* 
		 *        Isode options
		 */
		else if (lexequ(part1, "isode") == 0) {
			char *split;

			if ((split = index(part2, ' ')) != NULLCP) {
				*split++ = 0;
				(void) isodesetvar(part2, split, 0);
			}
		}

		/* 
		 *        Service options
		 */
		else if (lexequ(part1, "service") == 0)
			new_service(part2);
	}

	isodexport(NULLCP);
	(void) fclose(config_file);
}				/* read_quipurc */

/*
 * - read_args() -
 * Parse command line args.
 *
 */
static void
read_args(argc, argv)
	int argc;
	char *argv[];
{
	register char *argstr;

	if (argc <= 1)
		return;

	argv++;
	while (argc != 0 && (argstr = *argv)) {
		if (*argstr == '-') {
			switch (*++argstr) {
			case 'T':

				if (*++argv != NULLCP && *++argstr == '\0')
					load_oid_table(*argv);

				break;

			case 'c':

				if (*++argv != NULLCP && *++argstr == '\0')
					myname = *argv;
				dsa_address = NULLCP;

				break;

			case 't':

				if (*++argv != NULLCP && *++argstr == '\0')
					tailfile = *argv;

				break;

			default:
				argv++;
			}
		}

		argv++;
		argc--;
	}
}				/* read_args */

/*
 * - have_iso8859_font() -
 * Set up use of an 1so8859-1 font, to allow display of T.61 strings.
 *
 */
void
have_iso8859_font()
{
	extern int ch_set;

	ch_set = 1;
}
