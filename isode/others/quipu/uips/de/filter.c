/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/filter.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/filter.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: filter.c,v $
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


#include "quipu/util.h"
#include "quipu/ds_search.h"
#include "util.h"
#include "filter.h"

struct ds_search_arg sarg;
struct ds_search_result sresult;
struct DSError serror;

struct s_filter *
andfilter()
{
struct s_filter * fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_AND;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
orfilter()
{
struct s_filter * fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_OR;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter * 
eqfilter(matchtype, type, value)
int matchtype;
char * type, * value;
{
struct s_filter * fp;
AttributeType at;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
        fp->FUITEM.fi_type = matchtype;
        at = fp->FUITEM.fi_un.fi_un_ava.ava_type =
                str2AttrT(type);
        fp->FUITEM.fi_un.fi_un_ava.ava_value =
                str2AttrV(value, at->oa_syntax);
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
subsfilter(substrtype, type, value)
int substrtype;
char * type, * value;
{
struct s_filter * fp;
AttributeType at;

	fp = filter_alloc();
        fp->flt_type = FILTER_ITEM;
        fp->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;
	at = fp->FUITEM.fi_un.fi_un_ava.ava_type =
		str2AttrT(type);
	switch (substrtype) {
		case LEADSUBSTR:
			fp->FUITEM.UNSUB.fi_sub_initial =
		                avs_comp_new(str2AttrV(value, at->oa_syntax));
		        fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		        fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
			break;
		case TRAILSUBSTR:
			fp->FUITEM.UNSUB.fi_sub_final =
		                avs_comp_new(str2AttrV(value, at->oa_syntax));
		        fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		        fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
			break;
		case ANYSUBSTR:
			fp->FUITEM.UNSUB.fi_sub_any =
		                avs_comp_new(str2AttrV(value, at->oa_syntax));
		        fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
		        fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
			break;
	}
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
presfilter(type)
char * type;
{
struct s_filter * fp;

        fp = filter_alloc();
        fp->flt_type = FILTER_ITEM;
        fp->FUITEM.fi_type = FILTERITEM_PRESENT;
        fp->FUITEM.UNTYPE = str2AttrT(type);
        fp->flt_next = NULLFILTER;
        return fp;
}
