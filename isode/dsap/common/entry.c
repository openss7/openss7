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

/* entry.c - */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/entry.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/entry.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: entry.c,v
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

directory_free(directory)
	Entry directory;
{
	extern Entry database_root;

	if (directory != NULLENTRY) {
		if (directory != database_root)
			turbo_index_delete(directory);

		if (directory->e_children != NULLAVL)
			(void) avl_free(directory->e_children, directory_free);

		switch (directory->e_data) {
		case E_TYPE_SLAVE:
			local_slave_size--;
			break;
		case E_DATA_MASTER:
			local_master_size--;
			break;
		case E_TYPE_CACHE_FROM_MASTER:
			local_cache_size--;
			break;
		}

		entry_free(directory);
	}
}

entry_free(entryptr)
	Entry entryptr;
{
	if (entryptr->e_refcount-- > 0)
		return;

	rdn_free(entryptr->e_name);
	as_free(entryptr->e_attributes);

	if (entryptr->e_edbversion != NULLCP)
		free(entryptr->e_edbversion);

	if (entryptr->e_dsainfo != NULLDSA)
		free((char *) entryptr->e_dsainfo);

	if (entryptr->e_inherit != NULLAV)
		avs_free(entryptr->e_inherit);

	free((char *) entryptr);
}

/*
 * entry_replace - called from do_ds_modifyentry to update entry old with most
 * fields of entry new.
 */

entry_replace(old, new)
	Entry old;
	Entry new;
{
	as_free(old->e_attributes);
	old->e_attributes = as_cpy(new->e_attributes);
	old->e_leaf = new->e_leaf;
	old->e_complete = new->e_complete;
	old->e_data = new->e_data;
	old->e_allchildrenpresent = new->e_allchildrenpresent;
	/* parent, children, inherit, and index are the same */
	old->e_age = new->e_age;
	old->e_lock = new->e_lock;
	/* the rest must be set by calling unravel */
}

Entry
entry_cpy(entryptr)
	register Entry entryptr;
{
	register Entry ptr;

	if (entryptr == NULLENTRY)
		return (NULLENTRY);

	ptr = get_default_entry(entryptr->e_parent);
	ptr->e_name = rdn_cpy(entryptr->e_name);
	ptr->e_attributes = as_cpy(entryptr->e_attributes);
	if (entryptr->e_edbversion != NULLCP)
		ptr->e_edbversion = strdup(entryptr->e_edbversion);
	else
		ptr->e_edbversion = NULLCP;
	ptr->e_children = entryptr->e_children;
	ptr->e_inherit = avs_cpy(entryptr->e_inherit);
	ptr->e_leaf = entryptr->e_leaf;
	ptr->e_complete = entryptr->e_complete;
	ptr->e_data = entryptr->e_data;
	ptr->e_lock = entryptr->e_lock;
	ptr->e_allchildrenpresent = entryptr->e_allchildrenpresent;

	/* rest must be set by calling unravel_attributes */
	return (ptr);
}

Entry
get_default_entry(parent)
	Entry parent;
{
	register Entry eptr;

	eptr = entry_alloc();
	eptr->e_leaf = TRUE;
	eptr->e_allchildrenpresent = 2;
	eptr->e_complete = TRUE;
	eptr->e_data = E_DATA_MASTER;
	eptr->e_parent = parent;
	return (eptr);
}

check_known_oids()
{
	at_objectclass = AttrT_new(OBJECTCLASS_OID);
	at_alias = AttrT_new(ALIAS_OID);
}

entryrdn_cmp(rdn, ent)
	RDN rdn;
	Entry ent;
{
	return (rdn_cmp_reverse(rdn, ent->e_name));
}

entry_cmp(e1, e2)
	Entry e1;
	Entry e2;
{
	return (rdn_cmp_reverse(e1->e_name, e2->e_name));
}
