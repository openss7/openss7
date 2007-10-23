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

/* isobject.c - lookup Object IDentifiers/DEscriptors */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/isobject.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/isobject.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: isobject.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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

/* LINTLIBRARY */

#include <ctype.h>
#include <stdio.h>
#include "psap.h"
#include "tailor.h"

/*    DATA */

static char *isobjects = "isobjects";

static FILE *servf = NULL;
static int stayopen = 0;

static struct isobject ios;

/*  */

int
setisobject(f)
	int f;
{
	if (servf == NULL)
		servf = fopen(isodefile(isobjects, 0), "r");
	else
		rewind(servf);
	stayopen |= f;

	return (servf != NULL);
}

int
endisobject()
{
	if (servf && !stayopen) {
		(void) fclose(servf);
		servf = NULL;
	}

	return 1;
}

/*  */

struct isobject *
getisobject()
{
	register int i;
	register struct isobject *io = &ios;
	register char *cp;
	static char buffer[BUFSIZ + 1];
	static char *vec[NVEC + NSLACK + 1];
	static unsigned int elements[NELEM + 1];

	if (servf == NULL && (servf = fopen(isodefile(isobjects, 0), "r")) == NULL)
		return NULL;

	while (fgets(buffer, sizeof buffer, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if (cp = index(buffer, '\n'))
			*cp = NULL;
		if (str2vec(buffer, vec) < 2)
			continue;

		if ((i = str2elem(vec[1], elements)) <= 1)
			continue;

		io->io_descriptor = vec[0];
		io->io_identity.oid_elements = elements;
		io->io_identity.oid_nelem = i;

		return io;
	}
	return NULL;
}
