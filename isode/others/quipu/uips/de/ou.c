/* ou.c - search for an org unit */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/ou.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/ou.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: ou.c,v $
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

extern struct ds_search_arg sarg;
extern struct ds_search_result sresult;
extern struct DSError serror;

extern int abandoned;
extern int highNumber;
extern int exactMatch;
extern char exactString[];

struct namelist * ouatts;

struct ds_search_arg *fillMostOUSearchArgs();

void makeExplicitOUFilter();
void ouFilter1(), ouFilter2(), ouFilter3(), ouFilter4();

VFP explicitOU[] = {makeExplicitOUFilter, NULLVFP};
VFP normalOU[] = {ouFilter1, ouFilter2, ouFilter3, ouFilter4, NULLVFP};

int
listOUs(parentstr, thisstr, listp)
char * parentstr, * thisstr;
struct namelist ** listp;
{
	clearProblemFlags();
	initAlarm();
	if (exactMatch == ORGUNIT)
	  return (listExactOUs(exactString, listp));
        if (strcmp(thisstr, "*") == 0)
          return (listAllOUs(parentstr, listp));
        else
          return (listMatchingOUs(parentstr, thisstr, listp));
}

void
printListOUs(str, listp)
char * str;
struct namelist * listp;
{
struct namelist * x;
int i;
	if (listp == NULLLIST)
		if (strcmp(str, "*") == 0)
			pageprint("    No organisational units found\n");
		else
			pageprint("    No organisational units match entered string\n");
	else
	{
		for (i =1, x = listp; x != NULLLIST; i++, x = x->next)
			printLastComponent(INDENTON, x->name, ORGUNIT, i);
		showAnyProblems(str);
	}
}

void
freeOUs(listpp)
struct namelist ** listpp;
{
struct namelist * x, * y;

	x = *listpp;
	while (x != NULLLIST)
	{
		if (x->name != NULLCP)
			free(x->name);
		as_free(x->ats);
		y = x->next;
		free((char *)x);
		x = y;
	}
	*listpp = NULLLIST;
}

void
freeOUSearchArgs()
{

	dn_free(sarg.sra_baseobject);
	as_free(sarg.sra_eis.eis_select);
}

int
listAllOUs(parentstr, listp)
char * parentstr;
struct namelist ** listp;
{
int ret;
	
        sarg = * fillMostOUSearchArgs(parentstr, SRA_ONELEVEL);
        makeAllOUFilter(&sarg.sra_filter);
        ret = makeListOUs(listp);
        if (ret != OK)
		logListSuccess(LIST_ERROR, "ou", 0);
        else
		logListSuccess(LIST_OK, "ou", listlen(*listp));
	freeOUSearchArgs();
	alarmCleanUp();
	return ret;
}

int
listMatchingOUs(parentstr, thisstr, listp)
char * parentstr, * thisstr;
struct namelist ** listp;
{
VFP * filtarray;
VFP filterfunc;
int filtnumber;

        if (index(thisstr, '*') != NULLCP) /* contains at least one asterisk */
	{
                filtarray = explicitOU;
		filtnumber = -1;
	}
        else
	{
                filtarray = normalOU;
		filtnumber = 0;
	}
	sarg = * fillMostOUSearchArgs(parentstr, SRA_ONELEVEL);
        while ((filterfunc = *filtarray++) != NULLVFP)
	{
		filtnumber++;
                filterfunc(thisstr, &sarg.sra_filter);
                if (makeListOUs(listp) != OK)
		{
			freeOUSearchArgs();
			logSearchSuccess(SEARCH_ERROR, "ou", thisstr, filtnumber, 0);
			alarmCleanUp();
			return NOTOK;
		}
                if (*listp != NULLLIST)
		{
			freeOUSearchArgs();
			logSearchSuccess(SEARCH_OK, "ou", thisstr, filtnumber, listlen(*listp));
			alarmCleanUp();
                        return OK;
		}
	}
	logSearchSuccess(SEARCH_FAIL, "ou", thisstr, filtnumber, 0);

	/* nothing found by single level searches - let's try subtree searching */
        if (index(thisstr, '*') != NULLCP) /* contains at least one asterisk */
	{
                filtarray = explicitOU;
		filtnumber = -1;
	}
        else
	{
                filtarray = normalOU;
		filtnumber = 0;
	}
	sarg = * fillMostOUSearchArgs(parentstr, SRA_WHOLESUBTREE);
        while ((filterfunc = *filtarray++) != NULLVFP)
	{
		filtnumber++;
                filterfunc(thisstr, &sarg.sra_filter);
                if (makeListOUs(listp) != OK)
		{
			freeOUSearchArgs();
			alarmCleanUp();
			return NOTOK;
		}
                if (*listp != NULLLIST)
		{
			freeOUSearchArgs();
			logSearchSuccess(SEARCH_OK, "ou", thisstr, filtnumber, listlen(*listp));
			alarmCleanUp();
                        return OK;
		}
	}
	logSearchSuccess(SEARCH_FAIL, "ou", thisstr, filtnumber, 0);
	freeOUSearchArgs();
	alarmCleanUp();
	return OK;
}

int
listExactOUs(objectstr, listp)
char * objectstr;
struct namelist ** listp;
{
int ret;

        sarg = * fillMostOUSearchArgs(objectstr, SRA_BASEOBJECT);
        makeAllOUFilter(&sarg.sra_filter);
        ret = makeListOUs(listp);
	freeOUSearchArgs();
	alarmCleanUp();
	return ret;
}

int
makeListOUs(listp)
struct namelist ** listp;
{
entrystruct * x;
int retval;

	if (rebind() != OK)
		return NOTOK;
	retval = ds_search(&sarg, &serror, &sresult);
        if ((retval == DSE_INTR_ABANDONED) &&
	    (serror.dse_type == DSE_ABANDONED))
		abandoned = TRUE;
	if (retval != OK)
		return NOTOK;
	correlate_search_results (&sresult);

	setProblemFlags(sresult);

	highNumber = 0;
	for (x = sresult.CSR_entries; x != NULLENTRYINFO; x = x->ent_next) {
		*listp = list_alloc();
		(*listp)->name = dn2pstr(x->ent_dn);
                (*listp)->ats = as_cpy(x->ent_attr);
		listp = &(*listp)->next;
		highNumber++;
	}
	*listp = NULLLIST;
	entryinfo_free(sresult.CSR_entries,0);
	dn_free (sresult.CSR_object);
	crefs_free (sresult.CSR_cr);
	filter_free(sarg.sra_filter);
	return OK;
}

struct ds_search_arg *
fillMostOUSearchArgs(parentstr, searchdepth)
char * parentstr;
int searchdepth;
{
static struct ds_search_arg arg;
Attr_Sequence * atl;
AttributeType at;
struct namelist * x;
static CommonArgs sca = default_common_args;

	arg.sra_common = sca; /* struct copy */
        arg.sra_common.ca_servicecontrol.svc_timelimit = SVC_NOTIMELIMIT;
        arg.sra_common.ca_servicecontrol.svc_sizelimit= SVC_NOSIZELIMIT;

	arg.sra_subset = searchdepth;
	arg.sra_baseobject = str2dn(parentstr);
	arg.sra_searchaliases = TRUE;
	/* specify attributes of interest */
	arg.sra_eis.eis_allattributes = FALSE;
	atl = &(arg.sra_eis.eis_select);
        for (x = ouatts; x != NULLLIST; x = x->next)
        {
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

makeAllOUFilter(fpp)
struct s_filter ** fpp;
{
	*fpp = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
}

void
makeExplicitOUFilter(oustr, fpp)
char * oustr;
struct s_filter ** fpp;
{
struct s_filter * fp;
int wildcardtype;
char * ostr1, * ostr2;

	wildcardtype = starstring(oustr, &ostr1, &ostr2);
	*fpp = andfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
	switch (wildcardtype) {
		case LEADSUBSTR: /* fall through */
		case TRAILSUBSTR: /* fall through */
		case ANYSUBSTR:
			fp = fp->flt_next = subsfilter(wildcardtype, 
					DE_ORGANISATIONAL_UNIT_NAME, ostr1);
			break;
		case LEADANDTRAIL:
			fp = fp->flt_next = subsfilter(LEADSUBSTR, 
					DE_ORGANISATIONAL_UNIT_NAME, ostr1);
			fp = fp->flt_next = subsfilter(TRAILSUBSTR,
					DE_ORGANISATIONAL_UNIT_NAME, ostr2);
                        break;
	}
	fp->flt_next = NULLFILTER;
}

void
ouFilter1(oustr, fpp)
char * oustr;
struct s_filter ** fpp;
{
struct s_filter * fp;

	*fpp = andfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
	fp = fp->flt_next = eqfilter(FILTERITEM_EQUALITY, DE_ORGANISATIONAL_UNIT_NAME, oustr);
	fp->flt_next = NULLFILTER;
}

void
ouFilter2(oustr, fpp)
char * oustr;
struct s_filter ** fpp;
{
struct s_filter * fp;

	*fpp = andfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
	fp = fp->flt_next = subsfilter(LEADSUBSTR, DE_ORGANISATIONAL_UNIT_NAME, oustr);
	fp->flt_next = NULLFILTER;
}

void
ouFilter3(oustr, fpp)
char * oustr;
struct s_filter ** fpp;
{
struct s_filter * fp;

	*fpp = andfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
	fp = fp->flt_next = subsfilter(ANYSUBSTR, DE_ORGANISATIONAL_UNIT_NAME, oustr);
	fp->flt_next = NULLFILTER;
}

void
ouFilter4(oustr, fpp)
char * oustr;
struct s_filter ** fpp;
{
struct s_filter * fp;

	*fpp = andfilter();
	fp = (*fpp)->FUFILT = eqfilter(FILTERITEM_EQUALITY, DE_OBJECT_CLASS, DE_ORGANISATIONAL_UNIT);
	fp = fp->flt_next = eqfilter(FILTERITEM_APPROX, DE_ORGANISATIONAL_UNIT_NAME, oustr);
	fp->flt_next = NULLFILTER;
}

