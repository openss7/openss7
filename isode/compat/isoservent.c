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

/* isoservent.c - look-up ISODE services */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/isoservent.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/isoservent.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: isoservent.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "isoservent.h"
#include "tailor.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */

/* DATA */

static char *isoservices = "isoservices";

static FILE *servf = NULL;
static int stayopen = 0;

static struct isoservent iss;

int
setisoservent(f)
	int f;
{
	(void) rcsid;
	if (servf == NULL)
		servf = fopen(isodefile(isoservices, 0), "r");
	else
		rewind(servf);
	stayopen |= f;

	return (servf != NULL);
}

int
endisoservent()
{
	if (servf && !stayopen) {
		(void) fclose(servf);
		servf = NULL;
	}

	return 1;
}

struct isoservent *
getisoservent()
{
	int mask, vecp;
	register char *cp;
	register struct isoservent *is = &iss;
	static char buffer[BUFSIZ + 1], file[BUFSIZ];
	static char *vec[NVEC + NSLACK + 1];

	if (servf == NULL && (servf = fopen(isodefile(isoservices, 0), "r")) == NULL)
		return NULL;

	bzero((char *) is, sizeof *is);

	while (fgets(buffer, sizeof buffer, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if ((cp = index(buffer, '\n')))
			*cp = '\0';
		if ((vecp = str2vecX(buffer, vec, 1 + 1, &mask, NULL, 1)) < 3)
			continue;

		if ((cp = index(vec[0], '/')) == NULL)
			continue;
		*cp++ = '\0';

		is->is_provider = vec[0];
		is->is_entity = cp;
		is->is_selectlen = str2sel(vec[1], (mask & (1 << 1)) ? 1 : 0,
					   is->is_selector, ISSIZE);

		is->is_vec = vec + 2;
		is->is_tail = vec + vecp;

		if (strcmp(cp = is->is_vec[0], "tsapd-bootstrap"))
			(void) strcpy(is->is_vec[0] = file, isodefile(cp, 1));

		return is;
	}

	return NULL;
}

#ifdef	DEBUG
_printsrv(is)
	register struct isoservent *is;
{
	register int n = is->is_tail - is->is_vec - 1;
	register char **ap = is->is_vec;

	LLOG(addr_log, LLOG_DEBUG,
	     ("\tENT: \"%s\" PRV: \"%s\" SEL: %s",
	      is->is_entity, is->is_provider, sel2str(is->is_selector, is->is_selectlen, 1)));

	for (; n >= 0; ap++, n--)
		LLOG(addr_log, LLOG_DEBUG, ("\t\t%d: \"%s\"\n", ap - is->is_vec, *ap));
}
#endif
