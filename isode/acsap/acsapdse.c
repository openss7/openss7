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

/* acsapdse.c - application entity info -- directory service interface */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapdse.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapdse.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapdse.c,v
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
#include "DSE-types.h"
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"

/*    DATA */

static AEInfo aeis;
static struct PSAPaddr pas;

extern PE name2value_dase();

PE(*acsap_lookup) () = name2value_dase;

/*  */

AEI
str2aei_dse(string, context, ontty, userdn, passwd)
	char *string, *context, *userdn, *passwd;
	int ontty;
{
	char *alias, name[BUFSIZ];
	PE pe;
	register AEI aei = &aeis;
	register struct PSAPaddr *pa = &pas;
	static int first_time = 1;

	if (first_time)
		first_time = 0;
	else {
		AEIFREE(aei);
	}
	bzero((char *) aei, sizeof *aei);

	if ((alias = alias2name(string)) == NULL)
		alias = string;
	(void) strcpy(name, alias);

	if (acsap_lookup == NULL) {
		PY_advise(NULLCP, "str2aei_dse: acsap_lookup function not set");
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("%s", PY_pepy));
		return NULLAEI;
	}

	bzero((char *) pa, sizeof *pa);
	if (pe = (*acsap_lookup) (name, context, ontty, userdn, passwd, &aei->aei_ap_title)) {
		if (parse_DSE_PSAPaddr(pe, 1, NULLIP, NULLVP, (char *) pa) == NOTOK) {
			SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
			     ("parse of presentationAddress failed: %s", PY_pepy));
		} else
			PLOG(addr_log, print_DSE_PSAPaddr, pe, "address", 1);

		pe_free(pe);
	} else
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("directory returns no value"));

	return (aei->aei_ap_title ? aei : NULLAEI);
}

/*  */

struct PSAPaddr *
aei2addr_dse(aei)
	AEI aei;
{
	register struct PSAPaddr *pa;

	if (aei != &aeis) {
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ("aei2addr_dse cache miss on %s", sprintaei(aei)));

		return NULLPA;
	}

	return ((pa = &pas)->pa_addr.sa_addr.ta_naddr > 0 ? pa : NULLPA);
}
