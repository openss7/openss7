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

/* people.c - search for a person */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/de/RCS/people.c,v 9.0 1992/06/16 12:45:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/people.c,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: people.c,v
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
#include "filter.h"
#include "demanifest.h"
#include "namelist.h"
#include "destrings.h"

extern struct ds_search_arg sarg;
extern struct ds_search_result sresult;
extern struct DSError serror;

extern int abandoned;
extern int accessrightproblem;
extern int highNumber;
extern int exactMatch;
extern char exactString[];

/* PRR stands fpr people, rooms and roles */

struct namelist *prratts;

struct ds_search_arg *fillMostPRRSearchArgs();

void makeExplicitPRRFilter();
void prrFilter1(), prrFilter2(), prrFilter3(), prrFilter4();

VFP explicitPRR[] = { makeExplicitPRRFilter, NULLVFP };
VFP normalPRR[] = { prrFilter1, prrFilter2, prrFilter3, prrFilter4, NULLVFP };

/*VFP normalPRR[] = {prrFilter1, prrFilter3, NULLVFP};*/

int
listPRRs(parentstr, thisstr, listp)
	char *parentstr, *thisstr;
	struct namelist **listp;
{
	clearProblemFlags();
	initAlarm();
	if (exactMatch == PERSON)
		return (listExactPRRs(exactString, listp));
	if (strcmp(thisstr, "*") == 0)
		return (listAllPRRs(parentstr, listp));
	else
		return (listMatchingPRRs(parentstr, thisstr, listp));
}

void
printListPRRs(str, listp, searchparent, pdet)
	char *str;
	struct namelist *listp;
	int searchparent, pdet;
{
	struct namelist *x;
	char *savestr;
	static char lastsavedcomp[LINESIZE];
	int i;

	if (listp == NULLLIST)
		if (strcmp(str, "*") == 0)
			pageprint("      No people, rooms or roles found\n");
		else
			pageprint("      No people, rooms or roles match entered string\n");
	else {
		for (i = 1, x = listp; x != NULLLIST; i++, x = x->next) {
			/* if searching subtree under org (as opposed to an org unit), print ou if
			   there is one */
			if (searchparent == ORG) {
				savestr = removeLastRDN(x->name);
				if (index(savestr, '@') != rindex(savestr, '@')) {
					if (strncmp((rindex(savestr, '@') + 1),
						    SHORT_OU, strlen(SHORT_OU)) == 0)
						if (strcmp(lastsavedcomp, savestr) != 0) {
							printLastComponent(INDENTON, savestr,
									   ORGUNIT, 0);
							(void) strcpy(lastsavedcomp, savestr);
						}
				}
				free(savestr);
			}
			if (pdet) {
				printLastComponent(INDENTON, x->name, PERSON, pdet ? 0 : i);
				printDetails(PERSON, x);
			} else
				printPersonOneLiner(x, i);
		}
		showAnyProblems(str);
	}
	lastsavedcomp[0] = '\0';
}

void
freePRRs(listpp)
	struct namelist **listpp;
{
	struct namelist *w, *x;

	w = *listpp;
	while (w != NULLLIST) {
		if (w->name != NULLCP)
			free(w->name);
		as_free(w->ats);
		x = w->next;
		free((char *) w);
		w = x;
	}
	*listpp = NULLLIST;
}

freePRRSearchArgs()
{
	dn_free(sarg.sra_baseobject);
	as_free(sarg.sra_eis.eis_select);
}

int
listAllPRRs(parentstr, listp)
	char *parentstr;
	struct namelist **listp;
{
	int ret;

	sarg = *fillMostPRRSearchArgs(parentstr, SRA_WHOLESUBTREE);
	makeAllPRRFilter(&sarg.sra_filter);
	ret = makeListPRRs(listp, parentstr);
	if (ret != OK)
		logListSuccess(LIST_ERROR, "prr", 0);
	else
		logListSuccess(LIST_OK, "prr", listlen(*listp));
	freePRRSearchArgs();
	alarmCleanUp();
	return ret;
}

int
listMatchingPRRs(parentstr, thisstr, listp)
	char *parentstr, *thisstr;
	struct namelist **listp;
{
	VFP *filtarray;
	VFP filterfunc;
	int filtnumber;

	if (index(thisstr, '*') != NULLCP) {	/* contains at least one asterisk */
		filtarray = explicitPRR;
		filtnumber = -1;
	} else {
		filtarray = normalPRR;
		filtnumber = 0;
	}
	sarg = *fillMostPRRSearchArgs(parentstr, SRA_WHOLESUBTREE);
	while ((filterfunc = *filtarray++) != NULLVFP) {
		filtnumber++;
		filterfunc(thisstr, &sarg.sra_filter);
		if (sarg.sra_filter == NULLFILTER)
			continue;
		if (makeListPRRs(listp, parentstr) != OK) {
			freePRRSearchArgs();
			logSearchSuccess(SEARCH_ERROR, "prr", thisstr, filtnumber, 0);
			alarmCleanUp();
			return NOTOK;
		}
		if (*listp != NULLLIST)
			break;
	}
	if (*listp != NULLLIST)
		logSearchSuccess(SEARCH_OK, "prr", thisstr, filtnumber, listlen(*listp));
	else
		logSearchSuccess(SEARCH_FAIL, "prr", thisstr, filtnumber, 0);
	freePRRSearchArgs();
	alarmCleanUp();
	return OK;
}

int
listExactPRRs(objectstr, listp)
	char *objectstr;
	struct namelist **listp;
{
	int ret;

	sarg = *fillMostPRRSearchArgs(objectstr, SRA_BASEOBJECT);
	makeExactPRRFilter(&sarg.sra_filter);
	ret = makeListPRRs(listp, objectstr);
	freePRRSearchArgs();
	alarmCleanUp();
	return ret;
}

int
makeListPRRs(listp, parentstr)
	struct namelist **listp;
	char *parentstr;
{
	entrystruct *x;
	int retval;
	void onalarm();
	char *cp, *cp2;

	if (rebind() != OK)
		return NOTOK;
	retval = ds_search(&sarg, &serror, &sresult);
	if (retval == DSE_INTR_ABANDONED)
		if (serror.dse_type == DSE_ABANDONED)
			abandoned = TRUE;
		else if (serror.dse_type == DSE_SECURITYERROR)
			accessrightproblem = TRUE;
	if (retval != OK)
		return NOTOK;
	correlate_search_results(&sresult);

	setProblemFlags(sresult);

	highNumber = 0;
	if (strncmp(lastRDN(parentstr), SHORT_OU, strlen(SHORT_OU)) != 0) {
		/* we want to build the list so that any people with no ou come first */
		for (x = sresult.CSR_entries; x != NULLENTRYINFO; x = x->ent_next) {
			cp = dn2pstr(x->ent_dn);
			cp2 = removeLastRDN(cp);
			if (strncmp(lastRDN(cp2), SHORT_OU, strlen(SHORT_OU)) != 0) {
				*listp = list_alloc();
				(*listp)->name = cp;
				(*listp)->ats = as_cpy(x->ent_attr);
				listp = &(*listp)->next;
				highNumber++;
			} else
				free(cp);
			free(cp2);
		}
		for (x = sresult.CSR_entries; x != NULLENTRYINFO; x = x->ent_next) {
			cp = dn2pstr(x->ent_dn);
			cp2 = removeLastRDN(cp);
			if (strncmp(lastRDN(cp2), SHORT_OU, strlen(SHORT_OU)) == 0) {
				*listp = list_alloc();
				(*listp)->name = cp;
				(*listp)->ats = as_cpy(x->ent_attr);
				listp = &(*listp)->next;
				highNumber++;
			} else
				free(cp);
			free(cp2);
		}
	} else {
		for (x = sresult.CSR_entries; x != NULLENTRYINFO; x = x->ent_next) {
			*listp = list_alloc();
			(*listp)->name = dn2pstr(x->ent_dn);
			(*listp)->ats = as_cpy(x->ent_attr);
			listp = &(*listp)->next;
			highNumber++;
		}
	}
	*listp = NULLLIST;
	entryinfo_free(sresult.CSR_entries, 0);
	dn_free(sresult.CSR_object);
	crefs_free(sresult.CSR_cr);
	filter_free(sarg.sra_filter);
	return OK;
}

struct ds_search_arg *
fillMostPRRSearchArgs(parentstr, searchdepth)
	char *parentstr;
	int searchdepth;
{
	static struct ds_search_arg arg;
	Attr_Sequence *atl;
	AttributeType at;
	struct namelist *x;
	static CommonArgs sca = default_common_args;

	arg.sra_common = sca;	/* struct copy */
	arg.sra_common.ca_servicecontrol.svc_timelimit = SVC_NOTIMELIMIT;
	arg.sra_common.ca_servicecontrol.svc_sizelimit = SVC_NOSIZELIMIT;

	arg.sra_subset = searchdepth;
	arg.sra_baseobject = str2dn(parentstr);
	arg.sra_searchaliases = TRUE;
	/* specify attributes of interest */
	arg.sra_eis.eis_allattributes = FALSE;
	atl = &(arg.sra_eis.eis_select);
	for (x = prratts; x != NULLLIST; x = x->next) {
		if ((at = str2AttrT(x->name)) == NULLAttrT)
			continue;
		*atl = as_comp_alloc();
		(*atl)->attr_type = at;
		(*atl)->attr_value = NULLAV;
		atl = &(*atl)->attr_link;
	}
	*atl = NULLATTR;
	arg.sra_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
	return (&arg);
}

makeAllPRRFilter(fpp)
	struct s_filter **fpp;
{
	struct s_filter *fp;

	*fpp = orfilter();
	fp = (*fpp)->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}

makeExactPRRFilter(fpp)
	struct s_filter **fpp;
{
	struct s_filter *fp;

	*fpp = orfilter();
	fp = (*fpp)->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}

void
makeExplicitPRRFilter(prrstr, fpp)
	char *prrstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fpsav, *fpsav2;
	int wildcardtype;
	char *ostr1, *ostr2;

	wildcardtype = starstring(prrstr, &ostr1, &ostr2);
	*fpp = andfilter();
	fpsav = fp = (*fpp)->FUFILT = orfilter();
	switch (wildcardtype) {
	case LEADSUBSTR:	/* fall through */
	case TRAILSUBSTR:	/* fall through */
	case ANYSUBSTR:
		fp = fp->FUFILT = subsfilter(wildcardtype, DE_COMMON_NAME, ostr1);
		fp->flt_next = subsfilter(wildcardtype, DE_SURNAME, ostr1);
		break;
	case LEADANDTRAIL:
		fpsav2 = fp = fp->FUFILT = andfilter();
		fp = fp->FUFILT = subsfilter(LEADSUBSTR, DE_COMMON_NAME, ostr1);
		fp = fp->flt_next = subsfilter(TRAILSUBSTR, DE_COMMON_NAME, ostr2);
		fp = fpsav2->flt_next = andfilter();
		fp = fp->FUFILT = subsfilter(LEADSUBSTR, DE_SURNAME, ostr1);
		fp = fp->flt_next = subsfilter(TRAILSUBSTR, DE_SURNAME, ostr2);
		break;
	}
	fp = fpsav->flt_next = orfilter();
	fp = fp->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}

void
prrFilter1(prrstr, fpp)
	char *prrstr;
	struct s_filter **fpp;
{
	struct s_filter *fp, *fp1;
	char firststring[LINESIZE];
	char *fsp, *lsp, *laststring;

	/* if the string entered contains any spaces, form two substrings firstnamepart = all up to 
	   the first space lastnamepart = all after last space */
	if (index(prrstr, ' ') != NULLCP) {
		(void) strcpy(firststring, prrstr);
		fsp = index(firststring, ' ');
		*fsp = '\0';
		lsp = rindex(fsp + 1, ' ');
		if (lsp != NULLCP)
			laststring = lsp++;
		else
			laststring = fsp + 1;

		*fpp = orfilter();
		fp1 = fp = (*fpp)->FUFILT = andfilter();
		fp = fp->FUFILT = subsfilter(LEADSUBSTR, DE_COMMON_NAME, firststring);
		fp = fp->flt_next = subsfilter(TRAILSUBSTR, DE_COMMON_NAME, laststring);
		fp->flt_next = presfilter(DE_SURNAME);
		fp = fp1->flt_next = andfilter();
	} else {
		*fpp = orfilter();
		fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_SURNAME, prrstr);
		fp = fp->flt_next = andfilter();
	}
	fp = fp->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_COMMON_NAME, prrstr);
	fp = fp->flt_next = orfilter();
	fp = fp->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}

void
prrFilter2(prrstr, fpp)
	char *prrstr;
	struct s_filter **fpp;
{
	struct s_filter *fp;
	char firststring[LINESIZE];
	char *fsp, *lsp, *laststring;

	/* if the string entered contains any spaces, and the string up to the first space is more
	   than 1 character long, form two substrings

	   firstnamepart = first initial (match against beginning of cn) lastnamepart = all after
	   last space (match against beg. of sn)

	   this means that "paul barker" will match entries of "p barker" */
	*fpp = NULLFILTER;
	if (index(prrstr, ' ') != NULLCP) {
		(void) strcpy(firststring, prrstr);
		if (strlen(firststring) == 1)
			return;
		fsp = index(firststring, ' ');
		firststring[1] = '\0';
		lsp = rindex(fsp + 1, ' ');
		if (lsp != NULLCP)
			laststring = lsp++;
		else
			laststring = fsp + 1;

		*fpp = andfilter();
		fp = (*fpp)->FUFILT = subsfilter(LEADSUBSTR, DE_COMMON_NAME, firststring);
		fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_SURNAME, laststring);
		fp->flt_next = presfilter(DE_SURNAME);
	}
}

void
prrFilter3(prrstr, fpp)
	char *prrstr;
	struct s_filter **fpp;
{
	struct s_filter *fp;

	*fpp = orfilter();
	fp = (*fpp)->FUFILT = subsfilter(ANYSUBSTR, DE_SURNAME, prrstr);
	fp = fp->flt_next = andfilter();
	fp = fp->FUFILT = subsfilter(ANYSUBSTR, DE_COMMON_NAME, prrstr);
	fp = fp->flt_next = orfilter();
	fp = fp->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}

void
prrFilter4(prrstr, fpp)
	char *prrstr;
	struct s_filter **fpp;
{
	struct s_filter *fp;

	*fpp = orfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_APPROX, DE_SURNAME, prrstr);
	fp = fp->flt_next = andfilter();
	fp = fp->FUFILT = eqfilter(FILTERITEM_APPROX, DE_COMMON_NAME, prrstr);
	fp = fp->flt_next = orfilter();
	fp = fp->FUFILT = presfilter(DE_SURNAME);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROLE);
	fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ROOM);
}
