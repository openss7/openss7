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

/* isoentity.c - application entity titles -- sequential lookup utilities  */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/isoentity.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/isoentity.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: isoentity.c,v
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

#include <ctype.h>
#include <stdio.h>
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"

/*    DATA */

static char *isoentities = "isoentities";

static FILE *servf = NULL;
static int stayopen = 0;

static int mask;
static int vecp;
static char buffer[BUFSIZ + 1];
static char *vec[NVEC + NSLACK + 1];

static struct isoentity ies;

/*  */

int
setisoentity(f)
	int f;
{
	if (servf == NULL)
		servf = fopen(isodefile(isoentities, 0), "r");
	else
		rewind(servf);
	stayopen |= f;

	return (servf != NULL);
}

int
endisoentity()
{
	if (servf && !stayopen) {
		(void) fclose(servf);
		servf = NULL;
	}

	return 1;
}

/*  */

struct isoentity *
getisoentity()
{
	static char descriptor[BUFSIZ];

	if (servf == NULL && (servf = fopen(isodefile(isoentities, 0), "r")) == NULL)
		return NULL;

	while (_startisoentity(descriptor) == OK)
		if (_stopisoentity(descriptor, (struct isoentity *) NULL) == OK)
			return (&ies);

	return NULL;
}

/*  */

int
_startisoentity(descriptor)
	char *descriptor;
{
	register char *cp, *dp;
	char *ep;

	ep = (dp = buffer) + sizeof buffer;
	while (fgets(dp, ep - dp, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if (*dp == '\n' && dp == buffer)
			continue;
		if (cp = index(buffer, '\n')) {
			*cp-- = NULL;
			if (*cp == '\\') {
				if ((dp = cp) + 1 >= ep)
					dp = buffer;
				continue;
			}
		}

		switch (vecp = str2vecX(buffer, vec, 5 + 1, &mask, NULL, 1)) {
		case 3:	/* no address */
		case 4:	/* new-style */
			break;

		default:
			continue;
		}

		(void) sprintf(descriptor, "%s-%s", vec[0], vec[1]);

		return OK;
	}

	return DONE;
}

/*  */

int
_stopisoentity(descriptor, iep)
	char *descriptor;
	struct isoentity *iep;
{
	register int i;
	register struct isoentity *ie = &ies;
	OID oid = &ie->ie_identifier;
	struct PSAPaddr *pa = &ie->ie_addr;
	struct PSAPaddr *pz;
	static unsigned int elements[NELEM + 1];

	bzero((char *) ie, sizeof *ie);

	if (strcmp(vec[2], "NULL") == 0)
		elements[i = 0] = 0;
	else if ((i = str2elem(vec[2], elements)) <= 1)
		return NOTOK;
	oid->oid_elements = elements;
	oid->oid_nelem = i;
	ie->ie_descriptor = descriptor;

	switch (vecp) {
	case 3:		/* no address */
		break;

	case 4:		/* new-style */
		if (pz = str2paddr(vec[3]))
			*pa = *pz;	/* struct copy */
		break;
	}

	if (iep)
		*iep = *ie;	/* struct copy */

	return OK;
}

/*  */

_printent(ie)
	register struct isoentity *ie;
{
	LLOG(addr_log, LLOG_DEBUG,
	     ("Entity:  %s (%s)", ie->ie_descriptor, oid2ode(&ie->ie_identifier)));

	(void) ll_printf(addr_log, "Address: %s\n", paddr2str(&ie->ie_addr, NULLNA));

	(void) ll_printf(addr_log, "///////\n");

	(void) ll_sync(addr_log);
}
