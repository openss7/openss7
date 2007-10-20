#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_str.c,v $
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


#include "quipu/util.h"
#include "quipu/attrvalue.h"

short oc_sntx = -1;
IFP oc_hier = NULLIFP;
short acl_sntx = -1;
IFP merge_acl = NULLIFP;

AV_Sequence str2avs (str,at)
register char * str;
AttributeType at;
{
register char * ptr;
char * save,val;
AV_Sequence avs = NULLAV;
AV_Sequence newavs;

	if (str == NULLCP)
		return (NULLAV);

	if ((at) && (at->oa_syntax == oc_sntx))
		return ((AV_Sequence)(*oc_hier)(str));

	while ((ptr = index (str,'&')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;

		if ((avs) && (at->oa_syntax == acl_sntx)) {
			(*merge_acl)(avs,SkipSpace(str));
			*save = val;
			str = ptr;     
			continue;
		}

		newavs = avs_comp_alloc();
		newavs->avseq_next = NULLAV;

		if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
			return (NULLAV);

		*save = val;
		str = ptr;
		avs = avs_merge (avs,newavs);
	}

	if ((avs) && (at->oa_syntax == acl_sntx)) {
		(*merge_acl)(avs,SkipSpace(str));
		return (avs);
	}

	newavs = avs_comp_alloc();
	newavs->avseq_next = NULLAV;

	if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
		return (NULLAV);

	return (avs_merge (avs,newavs));

}


AV_Sequence fast_str2avs (str,at)
register char * str;
AttributeType at;
{
register char * ptr;
char * save,val;
AV_Sequence avs = NULLAV;
AV_Sequence newavs;
AV_Sequence fast_avs = NULLAV;
AV_Sequence fast_tail = NULLAV;
extern AV_Sequence avs_fast_merge ();

	if (str == NULLCP)
		return (NULLAV);

	if ((at) && (at->oa_syntax == oc_sntx))
		return ((AV_Sequence)(*oc_hier)(str));

	while ((ptr = index (str,'&')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;

		if ((avs) && (at->oa_syntax == acl_sntx)) {
			(*merge_acl)(avs,SkipSpace(str));
			*save = val;
			str = ptr;     
			continue;
		}

		newavs = avs_comp_alloc();
		newavs->avseq_next = NULLAV;

		if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
			return (NULLAV);

		*save = val;
		str = ptr;
		avs = avs_fast_merge (avs,newavs,fast_avs, fast_tail);
		fast_avs = avs;
		fast_tail = newavs;
	}

	if ((avs) && (at->oa_syntax == acl_sntx)) {
		(*merge_acl)(avs,SkipSpace(str));
		return (avs);
	}

	newavs = avs_comp_alloc();
	newavs->avseq_next = NULLAV;

	if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
		return (NULLAV);

	avs = avs_fast_merge (avs,newavs,fast_avs, fast_tail);

	return (avs);

}
