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

/* ap.c - Quipu access point syntax  */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/ap.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/ap.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: ap.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include "isoaddrs.h"
#include "../x500as/DO-types.h"

extern LLog *log_dsap;
extern struct PSAPaddr *psap_cpy();
extern aps_free();

struct access_point *
qap_cpy(a)
	struct access_point *a;
{
	struct access_point *r;

	r = (struct access_point *) smalloc(sizeof(struct access_point));
	bzero((char *) r, sizeof(struct access_point));

	r->ap_name = dn_cpy(a->ap_name);
	if (a->ap_address)
		r->ap_address = psap_cpy(a->ap_address);

	return (r);
}

static
qap_cmp(r, a)
	struct access_point *r, *a;
{
	int res;

	if ((res = dn_cmp(r->ap_name, a->ap_name)) == 0)
		if (r->ap_address && a->ap_address)
			return (bcmp((char *) r->ap_address,
				     (char *) a->ap_address, sizeof *a->ap_address) ? (-1) : 0);
		else
			return (r->ap_address == a->ap_address ? 0 :
				r->ap_address > a->ap_address ? 1 : (-1));

	return res;
}

static PE
qap_enc(p)
	struct access_point *p;
{
	PE ret_pe;

	if (encode_DO_QAccessPoint(&ret_pe, 0, 0, NULLCP, p) == NOTOK)
		return NULLPE;

	return (ret_pe);
}

static struct access_point *
qap_dec(pe)
	PE pe;
{
	struct access_point *qap;

	if (decode_DO_QAccessPoint(pe, 1, NULLIP, NULLVP, &qap) == NOTOK) {
		return (NULLACCESSPOINT);
	}

	return (qap);
}

static struct access_point *
qap_parse(s)
	char *s;
{
	struct PSAPaddr *pa;
	struct access_point *qap;
	char *p;

	qap = (struct access_point *) calloc(1, sizeof(struct access_point));

	if ((p = index(s, '#')) != NULLCP) {
		*p++ = 0;
		if (pa = str2paddr(SkipSpace(p))) {
			qap->ap_address = (struct PSAPaddr *) calloc(1, sizeof(struct PSAPaddr));
			*qap->ap_address = *pa;	/* struct copy */
		} else {
			parse_error("invalid presentation address in access point %s", p);
			free((char *) qap);
			*(--p) = '#';
			return (NULLACCESSPOINT);
		}

	}

	if ((qap->ap_name = str2dn(s)) == NULLDN) {
		if (qap->ap_address)
			free((char *) qap->ap_address);
		free((char *) qap);
		return NULLACCESSPOINT;
	}

	if (p)
		*--p = '#';

	return qap;
}

static
qap_print(ps, p, format)
	PS ps;
	struct access_point *p;
	int format;
{

	dn_print(ps, p->ap_name, format);

	if (p->ap_address)
		if (format != READOUT)
			ps_printf(ps, " # %s", _paddr2str(p->ap_address, NULLNA, -1));
		else
			ps_printf(ps, " # %s", paddr2str(p->ap_address, NULLNA));

}

ap_syntax()
{
	(void) add_attribute_syntax("AccessPoint",
				    (IFP) qap_enc, (IFP) qap_dec,
				    (IFP) qap_parse, qap_print,
				    (IFP) qap_cpy, qap_cmp, aps_free, NULLCP, NULLIFP, TRUE);
}
