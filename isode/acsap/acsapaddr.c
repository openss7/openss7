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

/* acsapaddr.c - application entity information -- lookup */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapaddr.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapaddr.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapaddr.c,v
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
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"

#ifndef	NOSTUB
AEI str2aei_stub();
struct PSAPaddr *aei2addr_stub();
#endif

AEI str2aei_dse();
struct PSAPaddr *aei2addr_dse();

/*    DATA */

#ifndef	NOSTUB
static char fallback1[BUFSIZ], fallback2[BUFSIZ];
#endif

static struct PSAPaddr *(*lookup) () = NULL;

/*  */

/* backwards compatibility... */

static struct mapping {
	char *m_key;
	char *m_value;
} sac2cn[] = {
"iso ftam", "filestore",
	    "iso vt", "terminal",
	    "iso cmip", "mib",
	    "isode passwd lookup demo",
	    "passwdstore",
	    "isode shell", "shell", "IRP Z39.50", "Z39.50", "pp qmgr interface", "pp qmgr", NULL};

/*  */

AEI
_str2aei(designator, qualifier, context, interactive, userdn, passwd)
	char *designator, *qualifier, *context, *userdn, *passwd;
	int interactive;
{
	AEI aei;
	register struct mapping *m;

	if (qualifier == NULLCP) {
		if (context)
			for (m = sac2cn; m->m_key; m++)
				if (strcmp(m->m_key, context) == 0) {
					qualifier = m->m_value;
					break;
				}

		if (qualifier == NULLCP)
			qualifier = context ? context : "default";
	}

	if (context == NULLCP) {
		for (m = sac2cn; m->m_key; m++)
			if (strcmp(m->m_value, qualifier) == 0) {
				context = m->m_key;
				break;
			}

		if (context == NULLCP)
			context = qualifier;
	}

	isodetailor(NULLCP, 0);
	LLOG(addr_log, LLOG_TRACE,
	     ("str2aei \"%s\" \"%s\" \"%s\" %d", designator, qualifier, context, interactive));

	aei = NULL, lookup = NULL;
	PY_pepy[0] = NULL;

	if (ns_enabled) {
		if (aei = str2aei_dse(designator, context, interactive, userdn, passwd)) {
			lookup = aei2addr_dse;
#ifndef	NOSTUB
			(void) strcpy(fallback1, designator);
			(void) strcpy(fallback2, qualifier);
			goto out;
#endif
		} else
			SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
			     ("DSE lookup of service \"%s\" \"%s\" \"%s\" failed",
			      designator, context, qualifier));
	}
#ifndef	NOSTUB
	if (aei = str2aei_stub(designator, qualifier))
		lookup = aei2addr_stub;
	else
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ("stub DSE lookup of service \"%s\" \"%s\" \"%s\" failed",
		      designator, context, qualifier));
#endif

#ifndef	NOSTUB
      out:;
#endif
	SLOG(addr_log, LLOG_TRACE, NULLCP,
	     ("str2aei returns %s", aei ? sprintaei(aei) : "NULLAEI"));

	return aei;
}

/*  */

struct PSAPaddr *
aei2addr(aei)
	AEI aei;
{
	struct PSAPaddr *pa;

	isodetailor(NULLCP, 0);
	SLOG(addr_log, LLOG_TRACE, NULLCP, ("aei2addr %s", sprintaei(aei)));

	PY_pepy[0] = NULL;

	if (lookup) {
		pa = (*lookup) (aei);
#ifndef	NOSTUB
		if (pa == NULLPA
		    && lookup == aei2addr_dse && (aei = str2aei_stub(fallback1, fallback2))
		    && (pa = aei2addr_stub(aei))) {
			SLOG(addr_log, LLOG_NOTICE, NULLCP, ("fallback use of stub DSE succeeded"));
		}
#endif

		lookup = NULL;
	} else
		pa = NULLPA;

	SLOG(addr_log, LLOG_TRACE, NULLCP, ("aei2addr returns %s", paddr2str(pa, NULLNA)));

	return pa;
}
