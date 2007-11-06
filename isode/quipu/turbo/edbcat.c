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

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/turbo/RCS/edbcat.c,v 9.0 1992/06/16 12:36:15 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/turbo/RCS/edbcat.c,v 9.0 1992/06/16 12:36:15 isode Rel
 *
 *
 * Log: edbcat.c,v
 * Revision 9.0  1992/06/16  12:36:15  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <gdbm.h>
#include "sys.file.h"
#include <ctype.h>

extern int gdbm_errno;

main(argc, argv)
	int argc;
	char **argv;
{
	GDBM_FILE db;
	datum prv, key, content;
	char *p, gfname[1024];

	if (argc != 2) {
		(void) printf("usage: %s edbdbmfile\n", argv[0]);
		return (1);
	}

	(void) strcpy(gfname, argv[1]);
	if ((p = rindex(argv[1], '.')) == NULL || strcmp(p, ".gdbm") != 0)
		(void) strcat(gfname, ".gdbm");

	if ((db = gdbm_open(gfname, 0, GDBM_READER, 0, 0)) == NULL) {
		(void) fprintf(stderr, "Can't open (%s)\ndbm_error is (%d)\n", gfname, gdbm_errno);
		return (1);
	}

	key.dptr = "HEADER";
	key.dsize = sizeof("HEADER");
	content = gdbm_fetch(db, key);
	if (content.dptr == NULL)
		(void) printf("No header!  Continuing...\n");
	else
		(void) printf("%s\n", content.dptr);

	prv.dptr = NULL;
	for (key = gdbm_firstkey(db); key.dptr; key = gdbm_nextkey(db, prv)) {
		if (prv.dptr != NULL)
			free(prv.dptr);
		if (strcmp(key.dptr, "HEADER") == 0) {
			prv = key;
			continue;
		}

		content = gdbm_fetch(db, key);
		(void) printf("%s", content.dptr);
		free(content.dptr);
		content.dptr = NULL;
		prv = key;
	}

	(void) gdbm_close(db);
	return (0);
}
