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

/* ftamdocument.c - FTAM document database */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamdocument.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamdocument.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamdocument.c,v
 * Revision 9.0  1992/06/16  12:14:55  isode
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

#include <stdio.h>
#include "ftam.h"
#include "tailor.h"

/*    DATA */

static char *isodocuments = "isodocuments";

static FILE *servf = NULL;
static int stayopen = 0;

static struct isodocument ids;

/*  */

int
setisodocument(f)
	int f;
{
	if (servf == NULL)
		servf = fopen(isodefile(isodocuments, 0), "r");
	else
		rewind(servf);
	stayopen |= f;

	return (servf != NULL);
}

int
endisodocument()
{
	if (servf && !stayopen) {
		(void) fclose(servf);
		servf = NULL;
	}

	return 1;
}

/*  */

struct isodocument *
getisodocument()
{
	register char *cp;
	register struct isodocument *id = &ids;
	static char buffer[BUFSIZ + 1];
	static char *vec[NVEC + NSLACK + 1];

	if (servf == NULL && (servf = fopen(isodefile(isodocuments, 0), "r")) == NULL)
		return NULL;

	if (id->id_type)
		oid_free(id->id_type);
	if (id->id_abstract)
		oid_free(id->id_abstract);
	if (id->id_transfer)
		oid_free(id->id_transfer);
	if (id->id_model)
		oid_free(id->id_model);
	if (id->id_constraint)
		oid_free(id->id_constraint);

	bzero((char *) id, sizeof *id);

	while (fgets(buffer, sizeof buffer, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if (cp = index(buffer, '\n'))
			*cp = NULL;
		if (str2vec(buffer, vec) < 6)
			continue;

		id->id_entry = vec[0];

		if ((id->id_type = str2oid(vec[1])) == NULLOID
		    || (id->id_type = oid_cpy(id->id_type)) == NULLOID)
			continue;

		if ((id->id_abstract = str2oid(vec[2])) == NULLOID
		    || (id->id_abstract = oid_cpy(id->id_abstract))
		    == NULLOID)
			goto free1;

		if ((id->id_transfer = str2oid(vec[3])) == NULLOID
		    || (id->id_transfer = oid_cpy(id->id_transfer))
		    == NULLOID)
			goto free2;

		if ((id->id_model = str2oid(vec[4])) == NULLOID
		    || (id->id_model = oid_cpy(id->id_model)) == NULLOID)
			goto free3;

		if ((id->id_constraint = str2oid(vec[5])) == NULLOID
		    || (id->id_constraint = oid_cpy(id->id_constraint))
		    == NULLOID) {
			oid_free(id->id_model);
			id->id_model = NULLOID;

		      free3:;
			oid_free(id->id_transfer);
			id->id_transfer = NULLOID;

		      free2:;
			oid_free(id->id_abstract);
			id->id_abstract = NULLOID;

		      free1:;
			oid_free(id->id_type);
			id->id_type = NULLOID;

			continue;
		}

		return id;
	}

	return NULL;
}

/*  */

struct isodocument *
getisodocumentbyentry(entry)
	char *entry;
{
	register struct isodocument *id;

	(void) setisodocument(0);
	while (id = getisodocument())
		if (strcmp(id->id_entry, entry) == 0)
			break;
	(void) endisodocument();

	return id;
}

/*  */

struct isodocument *
getisodocumentbytype(type)
	OID type;
{
	register struct isodocument *id;

	(void) setisodocument(0);
	while (id = getisodocument())
		if (oid_cmp(id->id_type, type) == 0)
			break;
	(void) endisodocument();

	return id;
}
