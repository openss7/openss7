/* entryinfo.c - Entry Information routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/entryinfo.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/entryinfo.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: entryinfo.c,v $
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


/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/commonarg.h"

entryinfo_comp_free (a,state)
register EntryInfo *a;
int state;
{
register EntryInfo * einfo, *e2;
register Attr_Sequence as;

	if (a == NULLENTRYINFO)
		return;

	dn_free (a->ent_dn);
	if (state == 1) {
		Attr_Sequence as_tmp = NULLATTR;
		
		for ( as=a->ent_attr; as!= NULLATTR; as=as_tmp) {
			as_tmp = as -> attr_link;
			free ((char *) as);
		}
	} else
		as_free (a->ent_attr);

	for (einfo=a->ent_next; einfo!=NULLENTRYINFO; einfo=e2) {
		dn_free (einfo->ent_dn);
		if (state == 1) {
			Attr_Sequence as_tmp = NULLATTR;
			for ( as=einfo->ent_attr; as!= NULLATTR; as=as_tmp) {
				as_tmp = as -> attr_link;
				free ((char *) as);
			}
		} else
			as_free (einfo->ent_attr);
		e2 = einfo -> ent_next;
		free ((char *) einfo);
	}
}

entryinfo_free (a,state)
register EntryInfo * a;
register int state;
{
	if (a == NULLENTRYINFO)
		return;
	entryinfo_comp_free (a,state);
	free ((char *) a);
}

entryinfo_cpy (a,b)
register EntryInfo *a;
register EntryInfo *b;
{
	a->ent_dn        = dn_cpy (b->ent_dn);
	a->ent_attr      = as_cpy (b->ent_attr);
	a->ent_iscopy    = b->ent_iscopy;
	a->ent_age       = b->ent_age;
	a->ent_next      = b->ent_next;
}

entryinfo_append (a,b)
register EntryInfo *a,*b;
{
register EntryInfo *ptr;

	if ( a  == NULLENTRYINFO )
		return;

	for (ptr=a; ptr->ent_next != NULLENTRYINFO; ptr=ptr->ent_next)
		;  /* noop */

	ptr->ent_next = b;
}

entryinfo_merge (a,b,fast)
register EntryInfo *a,*b;
int fast;
{
register EntryInfo *ptr;
EntryInfo *tmp, *prev, *trail;

	if (( a == NULLENTRYINFO )
	   || (b == NULLENTRYINFO ))
		return;

	/* Use fast mode if you know there are no duplicates */

	if (fast) {
		for (ptr=a; ptr->ent_next != NULLENTRYINFO; ptr=ptr->ent_next) 
		    	; 
		ptr->ent_next = b;
		return;
	}

	for (ptr=a; ptr != NULLENTRYINFO; ptr=ptr->ent_next) {
		prev = NULLENTRYINFO;
		for (tmp=b; tmp != NULLENTRYINFO; tmp=tmp->ent_next) {
		 	if (dn_cmp (ptr->ent_dn, tmp->ent_dn) == OK) {
				/* already got it - throw it away */
				if (prev == NULLENTRYINFO)
					b = tmp->ent_next;
				else
					prev->ent_next = tmp->ent_next;
				as_free (tmp->ent_attr);
				dn_free (tmp->ent_dn);
				free ((char *)tmp);
				break;	
			}
			prev = tmp;
		}
		trail = ptr;
	}

	trail->ent_next = b;
}

entryinfo_print (ps,entryinfo,format)
PS  ps;
EntryInfo *entryinfo;
int format;
{
register EntryInfo *einfo;

	for (einfo= entryinfo; einfo!=NULLENTRYINFO; einfo=einfo->ent_next) {
		dn_print(ps,einfo->ent_dn,EDBOUT);
		ps_print (ps,"\n");
		if (einfo->ent_attr)
			as_print (ps,einfo->ent_attr,format);
		ps_print (ps,"\n");
	}
}
