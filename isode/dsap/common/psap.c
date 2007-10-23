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

/* psap.c - General PSAP utility routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/psap.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/psap.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: psap.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include "isoaddrs.h"
#include "../x500as/DO-types.h"

extern LLog *log_dsap;

psap_free(psap)
	struct PSAPaddr *psap;
{
	free((char *) psap);
}

struct PSAPaddr *
psap_cpy(a)
	struct PSAPaddr *a;
{
	struct PSAPaddr *r;

	r = (struct PSAPaddr *) smalloc(sizeof(struct PSAPaddr));
	bzero((char *) r, sizeof(struct PSAPaddr));

	*r = *a;		/* struct copy */

	return (r);
}

psap_dup(r, a)
	struct PSAPaddr *r, *a;
{
	*r = *a;		/* struct copy */
}

static
psap_cmp(r, a)
	struct PSAPaddr *r, *a;
{
	return (bcmp((char *) r, (char *) a, sizeof *a) ? (-1) : 0);
}

static PE
psap_enc(p)
	struct PSAPaddr *p;
{
	PE ret_pe;

	if (build_DSE_PSAPaddr(&ret_pe, 0, 0, NULLCP, p) == NOTOK) {
		ret_pe = NULLPE;
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Failed to encode PSAP"));
	}
	return (ret_pe);
}

static struct PSAPaddr *
psap_dec(pe)
	PE pe;
{
	struct PSAPaddr *psap;

	psap = (struct PSAPaddr *) smalloc(sizeof *psap);

	if (parse_DSE_PSAPaddr(pe, 1, NULLIP, NULLVP, psap) == NOTOK) {
		free((char *) psap);
		return (NULLPA);
	}

	return (psap);
}

static struct PSAPaddr *
psap_parse(s)
	char *s;
{
	struct PSAPaddr *pa;
	struct PSAPaddr *psap;

	psap = (struct PSAPaddr *) calloc(1, sizeof(struct PSAPaddr));
	if (pa = str2paddr(s)) {
		*psap = *pa;	/* struct copy */
		return (psap);
	} else {
		parse_error("invalid presentation address %s", s);
		free((char *) psap);
		return (NULLPA);
	}
}

static
psap_print(ps, p, format)
	PS ps;
	struct PSAPaddr *p;
	int format;
{
	if (format != READOUT)
		ps_printf(ps, "%s", _paddr2str(p, NULLNA, -1));
	else
		ps_printf(ps, "%s", paddr2str(p, NULLNA));

}

psap_syntax()
{
	(void) add_attribute_syntax("presentationAddress",
				    (IFP) psap_enc, (IFP) psap_dec,
				    (IFP) psap_parse, psap_print,
				    (IFP) psap_cpy, psap_cmp, psap_free, NULLCP, NULLIFP, TRUE);
}
