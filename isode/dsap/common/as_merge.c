#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_merge.c,v $
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

Attr_Sequence as_merge (a,b)
Attr_Sequence a,b;
{
register Attr_Sequence aptr, bptr, result, trail, tmp;

	if ( a == NULLATTR )
		return (b);
	if ( b == NULLATTR )
		return (a);

	/* start sequence off, make sure 'a' is the first */
	switch (AttrT_cmp (a->attr_type,b->attr_type)) {
		case 0: /* equal */
			result = a;
			a->attr_value = avs_merge (a->attr_value, b->attr_value);
			aptr = a->attr_link;
			bptr = b->attr_link;
			free ((char *) b);
			break;
		case -1:
			result = b;
			aptr = a;
			bptr = b->attr_link;
			break;
		case 1:
			result = a;
			aptr = a->attr_link;
			bptr = b;
			break;
		}

	trail = result;
	while (  (aptr != NULLATTR) && (bptr != NULLATTR) ) {

	   switch (AttrT_cmp (aptr->attr_type,bptr->attr_type)) {
		case 0: /* equal */
			aptr->attr_value = avs_merge (aptr->attr_value, bptr->attr_value);
			tmp = bptr->attr_link;
			free ((char *) bptr);
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			bptr = tmp;
			break;
		case -1:
			trail->attr_link = bptr;
			trail = bptr;
			bptr = bptr->attr_link;
			break;
		case 1:
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			break;
	    }
	}
	if (aptr == NULLATTR)
		trail->attr_link = bptr;
	else
		trail->attr_link = aptr;

	return (result);
}

Attr_Sequence as_merge_aux (a,b)
Attr_Sequence a,b;
{
register Attr_Sequence aptr, bptr, result, trail, tmp;

	if ( a == NULLATTR )
		return (b);
	if ( b == NULLATTR )
		return (a);

	/* start sequence off, make sure 'a' is the first */
	switch (AttrT_cmp (a->attr_type,b->attr_type)) {
		case 0: /* equal */
			result = a;
			avs_free (a->attr_value);
			a->attr_value = b->attr_value;
			aptr = a->attr_link;
			bptr = b->attr_link;
			free ((char *) b);
			break;
		case -1:
			result = b;
			aptr = a;
			bptr = b->attr_link;
			break;
		case 1:
			result = a;
			aptr = a->attr_link;
			bptr = b;
			break;
		}

	trail = result;
	while (  (aptr != NULLATTR) && (bptr != NULLATTR) ) {

	   switch (AttrT_cmp (aptr->attr_type,bptr->attr_type)) {
		case 0: /* equal */
			avs_free (aptr->attr_value);
			aptr->attr_value = bptr->attr_value;
			tmp = bptr->attr_link;
			free ((char *) bptr);
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			bptr = tmp;
			break;
		case -1:
			trail->attr_link = bptr;
			trail = bptr;
			bptr = bptr->attr_link;
			break;
		case 1:
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			break;
	    }
	}
	if (aptr == NULLATTR)
		trail->attr_link = bptr;
	else
		trail->attr_link = aptr;

	return (result);
}

Attr_Sequence as_fast_merge (a,b,c,d)
Attr_Sequence a,b,c,d;
{
register Attr_Sequence aptr, bptr, result, trail, tmp;
static AV_Sequence fast_avs = NULLAV;
static AV_Sequence fast_tail = NULLAV;
extern AV_Sequence avs_fast_merge ();

	if ( a == NULLATTR )
		return (b);
	if ( b == NULLATTR )
		return (a);

	if (quipu_faststart && (a == c) && d->attr_link == NULLATTR)
		if (AttrT_cmp (d->attr_type,b->attr_type) == 1) {
			d->attr_link = b;
			return a;
		}

	/* start sequence off, make sure 'a' is the first */
	switch (AttrT_cmp (a->attr_type,b->attr_type)) {
		case 0: /* equal */
			result = a;
			a->attr_value = avs_fast_merge (a->attr_value,
				 b->attr_value, fast_avs, fast_tail);
			fast_avs = a->attr_value;
			fast_tail = b->attr_value;
			aptr = a->attr_link;
			bptr = b->attr_link;
			free ((char *) b);
			break;
		case -1:
			result = b;
			aptr = a;
			bptr = b->attr_link;
			break;
		case 1:
			result = a;
			aptr = a->attr_link;
			bptr = b;
			break;
		}

	trail = result;
	while (  (aptr != NULLATTR) && (bptr != NULLATTR) ) {

	   switch (AttrT_cmp (aptr->attr_type,bptr->attr_type)) {
		case 0: /* equal */
			aptr->attr_value = avs_fast_merge (aptr->attr_value,
				 bptr->attr_value, fast_avs, fast_tail);
			fast_avs = aptr->attr_value;
			fast_tail = bptr->attr_value;
			tmp = bptr->attr_link;
			free ((char *) bptr);
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			bptr = tmp;
			break;
		case -1:
			trail->attr_link = bptr;
			trail = bptr;
			bptr = bptr->attr_link;
			break;
		case 1:
			trail->attr_link = aptr;
			trail = aptr;
			aptr = aptr->attr_link;
			break;
	    }
	}
	if (aptr == NULLATTR)
		trail->attr_link = bptr;
	else
		trail->attr_link = aptr;

	return (result);
}

