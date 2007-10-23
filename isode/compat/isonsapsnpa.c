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

/* isonsapsnpa.c - look-up ISODE NSAP->SNP binding */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/isonsapsnpa.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/isonsapsnpa.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: isonsapsnpa.c,v
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
#include "isoaddrs.h"
#include "tailor.h"
#include "cmd_srch.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */

/* DATA */

static char *isonsapsnpa = "isonsapsnpa";

static FILE *servf = NULL;
static int stayopen = 0;

static CMD_TABLE tbl_communities[] = {
	{"cons", NAS_CONS},
	{"clns", NAS_CLNS},
	{NULLCP, NOTOK}
};

int
setisonsapsnpa(f)
	int f;
{
	if (servf == NULL)
		servf = fopen(isodefile(isonsapsnpa, 0), "r");
	else
		rewind(servf);
	stayopen |= f;

	return (servf != NULL);
}

int
endisonsapsnpa()
{
	if (servf && !stayopen) {
		(void) fclose(servf);
		servf = NULL;
	}

	return 1;
}

struct NSAPinfo *
getisonsapsnpa()
{
	int mask, vecp;
	register char *cp;
	static struct NSAPinfo iss;
	register struct NSAPinfo *is = &iss;
	static char buffer[BUFSIZ + 1];
	static char *vec[NVEC + NSLACK + 1];
	struct TSAPaddr *ta;

	if (servf == NULL && (servf = fopen(isodefile(isonsapsnpa, 0), "r")) == NULL)
		return NULL;

	bzero((char *) is, sizeof *is);

	while (fgets(buffer, sizeof buffer, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if ((cp = index(buffer, '\n')))
			*cp = '\0';
		if ((vecp = str2vecX(buffer, vec, 1 + 1, &mask, NULL, 1)) < 1)
			continue;

		ta = str2taddr(vec[0]);
		if (ta == NULLTA)
			continue;
		if (ta->ta_naddr != 1 || ta->ta_addrs[0].na_stack != NA_NSAP)
			continue;
		bcopy(ta->ta_addrs[0].na_address, is->is_prefix,
		      is->is_plen = ta->ta_addrs[0].na_addrlen);
		if (strcmp(vec[1], "-") == 0)
			is->is_nsap.na_stack = NOTOK;
		else {
			if ((ta = str2taddr(vec[1])) == NULLTA || ta->ta_naddr < 1)
				continue;
			is->is_nsap = ta->ta_addrs[0];	/* struct copy */
		}

		if (vecp > 1)
			switch (mask = cmd_srch(vec[2], tbl_communities)) {
			case NAS_CONS:
			case NAS_CLNS:
				is->is_stack = mask;
				break;
			default:
				is->is_stack = nsap_default_stack;
				break;
		} else
			is->is_stack = nsap_default_stack;

		if (vecp > 2 && strcmp(vec[2], "-") != 0)
			(void) strncpy(is->is_class, vec[2], sizeof(is->is_class));
		else
			is->is_class[0] = '\0';

		return is;
	}

	return NULL;
}

struct NSAPaddr *
getisosnpa(nsap)
	struct NSAPaddr *nsap;
{
	struct NSAPinfo *is;
	int best = -1;
	static struct NSAPinfo nss;
	struct NSAPinfo *ns = &nss;

	if (nsap->na_stack != NA_NSAP)
		return NULLNA;

	(void) setisonsapsnpa(0);

	while ((is = getisonsapsnpa())) {
		if (is->is_nsap.na_stack == NOTOK || is->is_plen < best)
			continue;
		if (bcmp(nsap->na_address, is->is_prefix, is->is_plen) == 0) {
			best = is->is_plen;
			*ns = *is;	/* struct copy */
		}
	}
	(void) endisonsapsnpa();

	if (best >= 0)
		return &ns->is_nsap;
	return NULLNA;
}

struct NSAPinfo *
getnsapinfo(nsap)
	struct NSAPaddr *nsap;
{
	struct NSAPinfo *is;
	int best = -1;
	static struct NSAPinfo nss;
	struct NSAPinfo *ns = &nss;

	if (nsap->na_stack != NA_NSAP)
		return NULLNI;

	(void) setisonsapsnpa(0);

	while ((is = getisonsapsnpa())) {
		if (is->is_plen < best)
			continue;
		if (bcmp(nsap->na_address, is->is_prefix, is->is_plen) == 0) {
			best = is->is_plen;

			*ns = *is;	/* struct copy */
		}
	}
	(void) endisonsapsnpa();

	if (best >= 0)
		return ns;
	return NULLNI;
}

static inline void
dummy(void)
{
	(void) rcsid;
}
