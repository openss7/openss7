/* entry.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/entry.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/entry.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: entry.c,v $
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
#include "quipu/turbo.h"

AttributeType at_alias;
AttributeType at_objectclass;

extern int local_master_size;
extern int local_slave_size;
extern int local_cache_size;


directory_free (directory)
Entry directory;
{
extern Entry database_root;

	if (directory !=  NULLENTRY) {
		if (directory != database_root)
			turbo_index_delete(directory);

		if (directory->e_children != NULLAVL)
			(void) avl_free(directory->e_children, directory_free);

		switch (directory->e_data) {
		case E_TYPE_SLAVE:
			local_slave_size--; break;
		case E_DATA_MASTER:
			local_master_size--; break;
		case E_TYPE_CACHE_FROM_MASTER:
			local_cache_size--; break;
		}

		entry_free(directory);
	}
}

entry_free (entryptr)
Entry entryptr;
{
	if (entryptr->e_refcount-- > 0)
		return;

	rdn_free (entryptr->e_name);
	as_free (entryptr->e_attributes);

	if (entryptr->e_edbversion !=  NULLCP )
		free (entryptr->e_edbversion);

	if (entryptr->e_dsainfo != NULLDSA)
		free ((char *) entryptr->e_dsainfo);

	if (entryptr->e_inherit != NULLAV)
		avs_free (entryptr->e_inherit);

	free ((char *) entryptr);
}

/*
 * entry_replace - called from do_ds_modifyentry to update entry old with most
 * fields of entry new.
 */

entry_replace( old, new )
Entry   old;
Entry   new;
{
        as_free( old->e_attributes );
        old->e_attributes = as_cpy( new->e_attributes );
        old->e_leaf = new->e_leaf;
        old->e_complete = new->e_complete;
        old->e_data = new->e_data;
        old->e_allchildrenpresent = new->e_allchildrenpresent;
        /* parent, children, inherit, and index are the same */
        old->e_age = new->e_age;
        old->e_lock = new->e_lock;
        /* the rest must be set by calling unravel */
}

Entry entry_cpy (entryptr)
register Entry entryptr;
{
register Entry ptr;

	if (entryptr == NULLENTRY)
		return (NULLENTRY);

	ptr = get_default_entry (entryptr->e_parent);
	ptr->e_name = rdn_cpy (entryptr->e_name);
	ptr->e_attributes = as_cpy (entryptr->e_attributes);
	if (entryptr->e_edbversion != NULLCP)
		ptr->e_edbversion = strdup (entryptr->e_edbversion);
	else
		ptr->e_edbversion = NULLCP;
        ptr->e_children = entryptr->e_children;
	ptr->e_inherit = avs_cpy(entryptr->e_inherit);
	ptr->e_leaf  = entryptr->e_leaf;
	ptr->e_complete = entryptr->e_complete;
	ptr->e_data = entryptr->e_data;
	ptr->e_lock = entryptr->e_lock;
	ptr->e_allchildrenpresent = entryptr->e_allchildrenpresent;

	/* rest must be set by calling unravel_attributes */
	return (ptr);
}


Entry get_default_entry (parent)
Entry parent;
{
register Entry eptr;

	eptr = entry_alloc();
	eptr->e_leaf = TRUE;
	eptr->e_allchildrenpresent = 2;
	eptr->e_complete = TRUE;
	eptr->e_data = E_DATA_MASTER;
	eptr->e_parent  = parent;
	return (eptr);
}


check_known_oids ()
{
	at_objectclass = AttrT_new (OBJECTCLASS_OID);
	at_alias = AttrT_new (ALIAS_OID);
}


entryrdn_cmp( rdn, ent )
RDN     rdn;
Entry   ent;
{
        return( rdn_cmp_reverse( rdn, ent->e_name ) );
}

entry_cmp( e1, e2 )
Entry   e1;
Entry   e2;
{
        return( rdn_cmp_reverse( e1->e_name, e2->e_name ) );
}

