#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_str.c,v $
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
#include "quipu/entry.h"
#include "quipu/malloc.h"

extern short acl_sntx;
extern IFP merge_acl;
extern IFP acl_fn;
extern char dsa_mode;

Attr_Sequence str2as (str)
register char * str;
{
register char * ptr;
char * save, val;
AttributeType at;
Attr_Sequence as;

	if (str == NULLCP)
		return (NULLATTR);

	if ((ptr = index (str,'=')) == 0) {
		parse_error ("equals missing in '%s'",str);
		return (NULLATTR);
	}

	save = ptr++;
	if (! isspace (*--save))
		save++;

	ptr = SkipSpace (ptr);
	if (*ptr == 0)
		return (NULLATTR);

	val = *save;
	*save = 0;
	
	if ((at = AttrT_new (str)) == NULLAttrT) {
		parse_error ("unknown attribute type '%s'",str);
		*ptr = '=';
		return (NULLATTR);
	}

	*save = val;

	as = as_comp_alloc ();
	as->attr_acl  = NULLACL_INFO;
	as->attr_type = at;
	as->attr_link = NULLATTR;

	ATTRIBUTE_HEAP;

	if ((as->attr_value = str2avs (ptr,as->attr_type)) == NULLAV) {
		RESTORE_HEAP;
		as_free (as);
		return (NULLATTR);
	}

	RESTORE_HEAP;

	return (as);

}

Attr_Sequence as_combine (as,str,allownull)
Attr_Sequence as;
register char * str;
char allownull;
{
register char * ptr = str;
char * save, val;
AV_Sequence avs;
Attr_Sequence as2,nas;
AttributeType at;
int i;
static Attr_Sequence fast_as = NULLATTR;
static Attr_Sequence fast_tail = NULLATTR;
extern Attr_Sequence as_fast_merge();
extern AV_Sequence avs_fast_merge ();
extern AV_Sequence fast_str2avs ();
AV_Sequence fast_avs = NULLAV;
AV_Sequence fast_avstail = NULLAV;

	if (str == NULLCP)
		return (as);

	while (*ptr != 0)
		if (*ptr == '=')
			break;
		else
			ptr++;

	if (*ptr == 0) {
		if (allownull) {
			nas = as_comp_alloc ();
			nas->attr_acl  = NULLACL_INFO;
			if ((nas->attr_type = AttrT_new (str)) == NULLAttrT) {
				parse_error ("unknown attribute type '%s'",str);
				return as;
			}
			nas->attr_link = NULLATTR;
			nas->attr_value = NULLAV;
			as = as_fast_merge (as,nas,fast_as,fast_tail);
			fast_as = as, fast_tail= nas;
			return as;
		}
		parse_error ("equals missing in '%s'",str);
		return (as);
	}

	save = ptr++;
	if (! isspace (*--save))
		save++;

	val = *save;
	*save = 0;

	ptr = SkipSpace (ptr);

	if ((at = AttrT_new (str)) == NULLAttrT) {
		parse_error ("unknown attribute type '%s'",str);
		*ptr = '=';
		return (as);
	}

	if (*ptr == 0) {
		if ((at->oa_syntax == acl_sntx) && dsa_mode) {
			/* Add default ACL */	
			struct acl * acl = (struct acl *) NULL;

			acl = acl_alloc();
			acl->ac_child = (struct acl_info *)(*acl_fn)();
			acl->ac_entry = (struct acl_info *)(*acl_fn)();
			acl->ac_default = (struct acl_info *)(*acl_fn)();
			acl->ac_attributes = NULLACL_ATTR;

			nas = as_comp_alloc ();
			nas->attr_acl  = NULLACL_INFO;
			nas->attr_link = NULLATTR;
			nas->attr_value = avs_comp_alloc();
			nas->attr_value->avseq_next = NULLAV;
			nas->attr_value->avseq_av.av_syntax = acl_sntx;
			nas->attr_value->avseq_av.av_struct = (caddr_t) acl;
			nas->attr_type = at;
			as = as_fast_merge (as,nas,fast_as,fast_tail);
			fast_as = as, fast_tail= nas;
		} else if ((allownull) ||
		   	   ((at->oa_syntax == acl_sntx) && !dsa_mode)) {
			nas = as_comp_alloc ();
			nas->attr_acl  = NULLACL_INFO;
			nas->attr_link = NULLATTR;
			nas->attr_value = NULLAV;
			nas->attr_type = at;
			as = as_fast_merge (as,nas,fast_as,fast_tail);
			fast_as = as, fast_tail= nas;
		}
		return (as);
	}

	for (as2=as; as2 != NULLATTR; as2=as2->attr_link) {
		if ((i = AttrT_cmp (at, as2->attr_type)) == 0) {
			*save = val;

			ATTRIBUTE_HEAP;

			if (at->oa_syntax == acl_sntx) {
				(*merge_acl)(as2->attr_value,SkipSpace(ptr));
				RESTORE_HEAP;
				return (as);
			}

			if ((avs = fast_str2avs (ptr,as2->attr_type)) == NULLAV) {
				RESTORE_HEAP;
				return (as);
			}

			as2->attr_value = avs_fast_merge (as2->attr_value,avs,
				   fast_avs, fast_avstail);
			fast_avs = as2->attr_value;
			fast_avstail = avs;

			RESTORE_HEAP;

			return (as);	
		} 
		else 
			if ( i > 0 )
				break;
	}

	*save = val;

	nas = as_comp_alloc ();
	nas->attr_acl  = NULLACL_INFO;
	nas->attr_type = at;
	nas->attr_link = NULLATTR;

	ATTRIBUTE_HEAP;

	if ((nas->attr_value = fast_str2avs (ptr,nas->attr_type)) == NULLAV) {
		RESTORE_HEAP;
		as_free (nas);
		return (as);
	}

	RESTORE_HEAP;

	as = as_fast_merge (as,nas,fast_as,fast_tail);
	fast_as = as, fast_tail= nas;
	return as;

}
