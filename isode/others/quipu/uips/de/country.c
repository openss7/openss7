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

/* country.c - search for a country */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/de/RCS/country.c,v 9.0 1992/06/16 12:45:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/country.c,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: country.c,v
 * Revision 9.0  1992/06/16  12:45:59  isode
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

#include <signal.h>
#include "quipu/util.h"
#include "quipu/ds_search.h"
#include "quipu/dua.h"
#include "demanifest.h"
#include "namelist.h"
#include "filter.h"
#include "destrings.h"

struct ds_search_arg sarg;
struct ds_search_result sresult;
struct DSError serror;

extern int abandoned;
extern int highNumber;
extern int exactMatch;
extern char exactString[];

struct ds_search_arg *fillMostCountrySearchArgs();

void makeExplicitCoFilter();
void coFilter1(), coFilter2(), coFilter3(), coFilter4();

VFP explicitCo[] = { makeExplicitCoFilter, NULLVFP };
VFP normalCo[] = { coFilter1, coFilter2, coFilter3, coFilter4, NULLVFP };

int
makeRootCountry(clistp)
	struct namelist **clistp;
{
	*clistp = list_alloc();
	(*clistp)->name = copy_string("root");
	(*clistp)->ats = NULLATTR;
	(*clistp)->next = NULLLIST;
	return OK;
}

int
listCos(cstr, clistp)
	char *cstr;
	struct namelist **clistp;
{
	clearProblemFlags();
	initAlarm();
	if (exactMatch == COUNTRY)
		return (listExactCos(exactString, clistp));
	else if (strcmp(cstr, "*") == 0)
		return (listAllCos(clistp));
	else if (strcmp(cstr, "-") == 0)
		return (makeRootCountry(clistp));	/* make the root a "country" to enable
							   searching under the root of the tree */
	else
		return (listMatchingCos(cstr, clistp));
}

void
printListCos(cstr, clistp)
	char *cstr;
	struct namelist *clistp;
{
	struct namelist *x;
	int i;

	if (clistp == NULLLIST)
		pageprint("No countries match entered string\n");
	else {
		for (i = 1, x = clistp; x != NULLLIST; i++, x = x->next)
			printLastComponent(INDENTON, x->name, COUNTRY, i);
		showAnyProblems(cstr);
	}
}

void
freeCos(listpp)
	struct namelist **listpp;
{
	struct namelist *x, *y;

	x = *listpp;
	while (x != NULLLIST) {
		if (x->name != NULLCP)
			free(x->name);
		as_free(x->ats);
		y = x->next;
		free((char *) x);
		x = y;
	}
	*listpp = NULLLIST;
}

void
freeCoSearchArgs()
{
	dn_free(sarg.sra_baseobject);
}

listAllCos(clistp)
	struct namelist **clistp;
{
	int ret;

	sarg = *fillMostCountrySearchArgs(NULLCP, SRA_ONELEVEL);
	makeAllCoFilter(&sarg.sra_filter);
	ret = makeListCountries(clistp);
	if (ret != OK)
		logListSuccess(LIST_ERROR, "co", 0);
	else
		logListSuccess(LIST_OK, "co", listlen(*clistp));

	freeCoSearchArgs();
	alarmCleanUp();
	return ret;
}

listMatchingCos(cstr, clistp)
	char *cstr;
	struct namelist **clistp;
{
	VFP *filtarray;
	VFP filterfunc;
	int filtnumber;

	if (index(cstr, '*') != NULLCP) {	/* contains at least one asterisk */
		filtarray = explicitCo;
		filtnumber = -1;
	} else {
		filtarray = normalCo;
		filtnumber = 0;
	}
	sarg = *fillMostCountrySearchArgs(NULLCP, SRA_ONELEVEL);
	while ((filterfunc = *filtarray++) != NULLVFP) {
		filtnumber++;
		filterfunc(cstr, &sarg.sra_filter);
		if (makeListCountries(clistp) != OK) {
			freeCoSearchArgs();
			logSearchSuccess(SEARCH_ERROR, "co", cstr, filtnumber, 0);
			alarmCleanUp();
			return NOTOK;
		}
		if (*clistp != NULLLIST)
			break;
	}
	if (*clistp != NULLLIST)
		logSearchSuccess(SEARCH_OK, "co", cstr, filtnumber, listlen(*clistp));
	else
		logSearchSuccess(SEARCH_FAIL, "co", cstr, filtnumber, 0);
	freeCoSearchArgs();
	alarmCleanUp();
	return OK;
}

listExactCos(objectstr, clistp)
	char *objectstr;
	struct namelist **clistp;
{
	int ret;

	sarg = *fillMostCountrySearchArgs(objectstr, SRA_BASEOBJECT);
	makeAllCoFilter(&sarg.sra_filter);
	ret = makeListCountries(clistp);
	freeCoSearchArgs();
	alarmCleanUp();
	return ret;
}

makeListCountries(clistp)
	struct namelist **clistp;
{
	entrystruct *x;
	int retval;

	if (rebind() != OK)
		return NOTOK;
	retval = ds_search(&sarg, &serror, &sresult);
	if ((retval == DSE_INTR_ABANDONED) && (serror.dse_type == DSE_ABANDONED))
		abandoned = TRUE;
	correlate_search_results(&sresult);

	setProblemFlags(sresult);

	highNumber = 0;
	for (x = sresult.CSR_entries; x != NULLENTRYINFO; x = x->ent_next) {
		*clistp = list_alloc();
		(*clistp)->name = dn2pstr(x->ent_dn);
		(*clistp)->ats = NULLATTR;
		clistp = &(*clistp)->next;
		highNumber++;
	}
	*clistp = NULLLIST;
	entryinfo_free(sresult.CSR_entries, 0);
	dn_free(sresult.CSR_object);
	crefs_free(sresult.CSR_cr);
	filter_free(sarg.sra_filter);
	return OK;
}

struct ds_search_arg *
fillMostCountrySearchArgs(objectstr, searchdepth)
	char *objectstr;
	int searchdepth;
{
	static struct ds_search_arg arg;
	static CommonArgs sca = default_common_args;

	arg.sra_common = sca;	/* struct copy */
	arg.sra_common.ca_servicecontrol.svc_timelimit = SVC_NOTIMELIMIT;
	arg.sra_common.ca_servicecontrol.svc_sizelimit = SVC_NOSIZELIMIT;

	arg.sra_subset = searchdepth;
	arg.sra_baseobject = str2dn(objectstr);
	/* specify attributes of interest */
	arg.sra_eis.eis_allattributes = FALSE;
	arg.sra_eis.eis_select = NULLATTR;
	arg.sra_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
	return (&arg);
}

makeAllCoFilter(fpp)
	struct s_filter **fpp;
{
	struct s_filter *fp;

	*fpp = orfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
}

void
makeExplicitCoFilter(cstr, fpp)
	char *cstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp2;
	int wildcardtype;
	char *ostr1, *ostr2;

	wildcardtype = starstring(cstr, &ostr1, &ostr2);
	*fpp = orfilter();
	fp = fp2 = (*fpp)->FUFILT = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	switch (wildcardtype) {
	case LEADSUBSTR:	/* fall through */
	case TRAILSUBSTR:	/* fall through */
	case ANYSUBSTR:
		fp = fp->flt_next = subsfilter(wildcardtype, DE_FRIENDLY_COUNTRY, ostr1);
		break;
	case LEADANDTRAIL:
		fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_FRIENDLY_COUNTRY, ostr1);
		fp = fp->flt_next = subsfilter(TRAILSUBSTR, DE_FRIENDLY_COUNTRY, ostr2);
		break;
	}
	fp = fp2->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
	switch (wildcardtype) {
	case LEADSUBSTR:	/* fall through */
	case TRAILSUBSTR:	/* fall through */
	case ANYSUBSTR:
		fp = fp->flt_next = subsfilter(wildcardtype, DE_LOCALITY_NAME, ostr1);
		break;
	case LEADANDTRAIL:
		fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_LOCALITY_NAME, ostr1);
		fp = fp->flt_next = subsfilter(TRAILSUBSTR, DE_LOCALITY_NAME, ostr2);
		break;
	}

}

void
coFilter1(cstr, fpp)
	char *cstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp2;

	*fpp = orfilter();
	fp = fp2 = (*fpp)->FUFILT = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	if ((strlen(cstr) == 2) && (check_3166(cstr)))
		fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_COUNTRY_NAME, cstr);
	else
		fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_FRIENDLY_COUNTRY, cstr);
	fp = fp2->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_LOCALITY_NAME, cstr);
}

void
coFilter2(cstr, fpp)
	char *cstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp2;

	*fpp = orfilter();
	fp = fp2 = (*fpp)->FUFILT = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_FRIENDLY_COUNTRY, cstr);
	fp = fp2->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
	fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_LOCALITY_NAME, cstr);
}

void
coFilter3(cstr, fpp)
	char *cstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp2;

	*fpp = orfilter();
	fp = fp2 = (*fpp)->FUFILT = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	fp = fp->flt_next = subsfilter(ANYSUBSTR, DE_FRIENDLY_COUNTRY, cstr);
	fp = fp2->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
	fp = fp->flt_next = subsfilter(ANYSUBSTR, DE_LOCALITY_NAME, cstr);
}

void
coFilter4(cstr, fpp)
	char *cstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp2;

	*fpp = orfilter();
	fp = fp2 = (*fpp)->FUFILT = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_COUNTRY);
	fp = fp->flt_next = eqfilter(FILTERITEM_APPROX, DE_FRIENDLY_COUNTRY, cstr);
	fp = fp2->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_LOCALITY);
	fp = fp->flt_next = eqfilter(FILTERITEM_APPROX, DE_LOCALITY_NAME, cstr);
}
