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

/* isoalias.c - application entity info --  directory service utilities */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/isoalias.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/isoalias.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: isoalias.c,v
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*    DATA */

static char *isoaliases = "isoaliases";

#define	PBUCKETS	128
#define	PHASH(nm) \
    (((nm)[1]) ? (((chrcnv[((nm)[0])] - chrcnv[((nm)[1])]) & 0x1f) \
		  	+ ((chrcnv[(nm)[2]]) & 0x5f)) \
	       : (chrcnv[(nm)[0]]) & 0x7f)

struct pair {
	char *p_name;
	char *p_value;

	struct pair *p_chain;
};

static int inited = 0;
static struct pair *Pbuckets[PBUCKETS];

static int read_aliases();
static int read_file();

/*  */

char *
alias2name(name)
	char *name;
{
	register struct pair *p;

	if (!inited)
		read_aliases();

	for (p = Pbuckets[PHASH(name)]; p && lexequ(p->p_name, name); p = p->p_chain)
		continue;

	return (p ? p->p_value : NULL);
}

/*  */

static int
read_aliases()
{
	register char *hp;
	char buffer[BUFSIZ];

	if (inited)
		return;
	inited = 1;

	bzero((char *) Pbuckets, sizeof Pbuckets);

	read_file(isodefile(isoaliases, 0));

	if ((hp = getenv("HOME")) == NULL)
		hp = ".";
	(void) sprintf(buffer, "%s/.isode_aliases", hp);
	read_file(buffer);
}

/*  */

static int
read_file(file)
	char *file;
{
	register char *cp;
	char buffer[BUFSIZ + 1], *vec[NVEC + NSLACK + 1];
	register FILE *fp;

	if ((fp = fopen(file, "r")) == NULL)
		return;

	while (fgets(buffer, sizeof buffer, fp)) {
		if (*buffer == '#')
			continue;
		if (cp = index(buffer, '\n'))
			*cp = NULL;
		if (str2vec(buffer, vec) < 2)
			continue;

		if (add_alias(vec[0], vec[1]) == NOTOK)
			break;
	}

	(void) fclose(fp);
}

/*  */

int
add_alias(name, value)
	char *name, *value;
{
	int i;
	register struct pair *p;

	if (!inited)
		read_aliases();

	if ((p = (struct pair *) calloc(1, sizeof *p)) == NULL) {
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("calloc of alias structure failed"));
		return NOTOK;
	}

	if ((p->p_name = malloc((unsigned) (strlen(name) + 1))) == NULL
	    || (p->p_value = malloc((unsigned) (strlen(value) + 1)))
	    == NULL) {
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("malloc of alias structure failed"));
		if (p->p_name)
			free(p->p_name);
		free((char *) p);
		return NOTOK;
	}
	(void) strcpy(p->p_name, name);
	(void) strcpy(p->p_value, value);

	p->p_chain = Pbuckets[i = PHASH(p->p_name)];
	Pbuckets[i] = p;

	return OK;
}

#ifdef DEBUG
free_isode_alias()
{
	int i;
	struct pair *p, *np;

	for (i = 0; i < PBUCKETS; i++)
		for (p = Pbuckets[i]; p != (struct pair *) NULL; p = np) {
			np = p->p_chain;
			free(p->p_name);
			free(p->p_value);
			free((char *) p);
		}

}
#endif
