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

/* ufn_aei.c - user-friendly aei lookup */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/ufn_aet.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/ufn_aet.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: ufn_aet.c,v
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

#include "quipu/ufn.h"
#include "quipu/list.h"
#include "quipu/ds_search.h"
#include "quipu/connection.h"	/* ds_search uses di_block - include this for lint !!! */
#include "quipu/dua.h"

extern LLog *log_dsap;
extern LLog *addr_log;

extern Filter ocfilter();
extern Filter joinfilter();

extern char PY_pepy[];
void PY_advise();

static Filter
aet_filter(context)
	char *context;
{
	Filter a, b;

	if ((a = ocfilter("ApplicationEntity")) == NULLFILTER)
		return NULLFILTER;

	b = filter_alloc();
	b->flt_next = a;
	b->flt_type = FILTER_ITEM;
	b->FUITEM.fi_type = FILTERITEM_EQUALITY;
	if ((b->FUITEM.UNAVA.ava_type = AttrT_new(APPLCTX_OID)) == NULLAttrT) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("supported application context attribute unknown"))
		    return NULLFILTER;
	}
	b->FUITEM.UNAVA.ava_value = str2AttrV(context, b->FUITEM.UNAVA.ava_type->oa_syntax);
	if (b->FUITEM.UNAVA.ava_value == NULLAttrV) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("'%s' unknown OID", context));
		return NULLFILTER;
	}

	return joinfilter(b, FILTER_AND);
}

static
aet_search(base, subtree, filt, res)
	DN base;
	char subtree;
	Filter filt;
	DNS *res;
{
	struct ds_search_arg search_arg;
	static struct ds_search_result result;
	struct DSError err;
	static CommonArgs ca = default_common_args;
	EntryInfo *ptr;
	DNS newdns, r = NULLDNS;

	search_arg.sra_baseobject = base;
	search_arg.sra_filter = filt;
	if (subtree)
		search_arg.sra_subset = SRA_WHOLESUBTREE;
	else
		search_arg.sra_subset = SRA_ONELEVEL;
	search_arg.sra_searchaliases = TRUE;
	search_arg.sra_common = ca;	/* struct copy */
	search_arg.sra_eis.eis_infotypes = TRUE;
	search_arg.sra_eis.eis_allattributes = TRUE;
	search_arg.sra_eis.eis_select = NULLATTR;

	if (ds_search(&search_arg, &err, &result) != DS_OK) {
		log_ds_error(&err);
		ds_error_free(&err);
		return FALSE;
	}

	correlate_search_results(&result);

	dn_free(result.CSR_object);

	if ((result.CSR_limitproblem != LSR_NOLIMITPROBLEM)
	    || (result.CSR_cr != NULLCONTINUATIONREF)) {
		crefs_free(result.CSR_cr);

		if (!result.CSR_entries)
			return FALSE;
	}

	for (ptr = result.CSR_entries; ptr != NULLENTRYINFO; ptr = ptr->ent_next) {
		cache_entry(ptr, FALSE, TRUE);
		newdns = dn_seq_alloc();
		newdns->dns_next = r;
		newdns->dns_dn = dn_cpy(ptr->ent_dn);
		r = newdns;
	}
	entryinfo_free(result.CSR_entries, 0);

	*res = r;

	return TRUE;
}

aet_match(c, v, interact, result, el, context)
	int c;
	char **v;
	DNS *result;

DNS(*interact) ();
	envlist el;
	char *context;
{
	DNS ufnr = NULLDNS;
	DNS newap = NULLDNS;
	DNS apps = NULLDNS;
	DNS dns, DNS_append();
	Filter filt;
	int ok = TRUE;

	if (!ufn_match(c, v, interact, &ufnr, el)) {
		if (PY_pepy[0]) {
			char buffer[BUFSIZ];

			(void) sprintf(buffer, "ufn_match failed: %s", PY_pepy);
			(void) strcpy(PY_pepy, buffer);
			SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("%s", PY_pepy));
		}
		*result = NULLDNS;
		return FALSE;
	}

	filt = aet_filter(context);

	for (dns = ufnr; dns != NULLDNS; dns = dns->dns_next) {
		newap = NULLDNS;
		if (aet_search(dns->dns_dn, FALSE, filt, &newap))
			apps = DNS_append(apps, newap);
		else
			ok = FALSE;
	}

	if ((apps == NULLDNS) && ok) {
		/* go deeper */
		for (dns = ufnr; dns != NULLDNS; dns = dns->dns_next) {
			if (dns->dns_dn->dn_parent == NULLDN)
				/* too high for subtree search */
				continue;
			newap = NULLDNS;
			if (aet_search(dns->dns_dn, TRUE, filt, &newap))
				apps = DNS_append(apps, newap);
			else
				ok = FALSE;
		}
	}

	if (!ok && !apps) {
		PY_advise(NULLCP, "search for applicationEntity supporting \"%s\" failed", context);
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("%s", PY_pepy));
	}

	filter_free(filt);

	dn_seq_free(ufnr);

	*result = apps;

	return ok;
}
