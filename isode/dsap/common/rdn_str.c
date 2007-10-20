#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_str.c,v $
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
#include "quipu/name.h"
#include "quipu/malloc.h"

RDN str2rdn_aux (str)
char * str;
{
AttributeType at;
char * ptr;
char * save, val;
RDN rdn;
unsigned last_heap;

	/* look for "type = value" */

	if (str == NULLCP) {
		parse_error ("NULL rdn component",NULLCP);
		return (NULLRDN);
	}

	if ((ptr=index(str,'=')) == 0) {
		parse_error ("Equals missing in RDN '%s'",str);
		return (NULLRDN);
	}

	save = ptr++;
	save--;
	if (! isspace (*save))
		save++;
	val = *save;
	*save = 0;

	FAST_TIDY(str);

	if ((at = AttrT_new (str)) == NULLTABLE_ATTR) {
		parse_error ("Unknown attribute type in RDN '%s'",str);
		*save = val;
		return (NULLRDN);
	}

	if (*ptr == 0) {
		*save = val;
		parse_error ("Attribute Value missing in RDN '%s'",str);
		return (NULLRDN);
	}

	rdn = rdn_comp_alloc();
	rdn->rdn_next = NULLRDN;
	rdn->rdn_at = at;

	if ((last_heap = mem_heap) == 1)
		mem_heap = 2 + attr_index;

	if (str_at2AttrV_aux (ptr,rdn->rdn_at,&rdn->rdn_av) == NOTOK) {
		*save = val;
		mem_heap = last_heap;
		free ((char *) rdn);
		return (NULLRDN);
	}

	mem_heap = last_heap;
	*save = val;
	return (rdn);
}


RDN str2rdn (str)
char * str;
{
register char *ptr;
register char *save,val;
RDN rdn = NULLRDN, newrdn;

	/* look for "rdn % rdn % rdn" */

	if (str == NULLCP)
		return (NULLRDN);

	while ( (ptr = index (str,'%')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;
		if ((newrdn = str2rdn_aux (str)) == NULLRDN) {
			rdn_free (rdn);
			return (NULLRDN);
		}

		rdn = rdn_merge (rdn,newrdn);
		*save = val;
		str = ptr;
	}

	if ((newrdn = str2rdn_aux (str)) == NULLRDN) {
		rdn_free (rdn);
		return (NULLRDN);
	}

	return (rdn_merge (rdn,newrdn));
}
