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

/* d_main.c - make the decode routine into a stand alone program */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/d_main.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/d_main.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: d_main.c,v
 * Revision 9.0  1992/06/16  12:43:35  isode
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
#include "psap.h"
#include "sys.file.h"
#include "quipu/photo.h"

#define ALLOCATION_SIZE 16384L

int two_passes = 0;

char *getenv();

main(argc, argv)
	int argc;
	char **argv;
{
	char *data;
	int fd;
	char *file = "<stdin>";
	int len;
	long limit;
	char *name;
	char *newData;
	long size;

	if ((name = getenv("RDN")) == NULL)
		name = "unknown";

	/* process commmand line options and parameters */

	if (argc > 1)
		name = *(++argv);

	fd = fileno(stdin);

	/* read the entire source file into memory */

	data = (char *) malloc((unsigned int) ALLOCATION_SIZE);
	if (!data) {
		(void) fputs("decode_fax: out of memory\n", stderr);
		exit(1);
	}
	limit = ALLOCATION_SIZE;
	size = 0L;

	for (;;) {
		if (size + ALLOCATION_SIZE > limit) {
			newData = (char *) realloc(data, (unsigned int) (limit + ALLOCATION_SIZE));
			if (!newData) {
				(void) fputs("decode_fax: out of memory\n", stderr);
				exit(1);
			}
			data = newData;
			limit += ALLOCATION_SIZE;
		}
		len = read(fd, &data[size], (int) ALLOCATION_SIZE);
		if (len == -1) {
			perror(file);
			exit(1);
		} else if (len == 0)
			break;
		size += len;
	}

	if (size < 1) {
		(void) fprintf(stderr, "%s: is not a fax image\n", file);
		exit(1);
	}
	if (decode_t4(data, name, (int) size) == -1) {
		(void) fprintf(stderr, "\n");
		exit(-1);
	}
	if (two_passes && decode_t4(data, name, (int) size) == -1) {
		(void) fprintf(stderr, "\n");
		exit(-1);
	}
	return (0);
}

/*    ERRORS */

static
ps_die(ps, s)
	register PS ps;
	register char *s;
{
	(void) fprintf(stderr, "%s: %s\n", s, ps_error(ps->ps_errno));
	exit(1);
}

static
pe_die(pe, s)
	register PE pe;
	register char *s;
{
	(void) fprintf(stderr, "%s: %s\n", s, pe_error(pe->pe_errno));
	exit(1);
}
