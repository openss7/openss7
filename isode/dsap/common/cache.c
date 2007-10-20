/* cache.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/cache.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/cache.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: cache.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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
#include "quipu/dua.h"
#include "quipu/list.h"
#include "quipu/entry.h"
#include "quipu/turbo.h"

extern LLog * log_dsap;
extern Entry database_root;
extern int local_cache_size;
extern int entryrdn_cmp();

struct list_cache *list_top = NULLCACHE;
Entry  current_entry = NULLENTRY;
DN     current_dn = NULLDN;
extern time_t cache_timeout;

Entry local_find_entry_aux ();

struct subordinate * subord_cpy (x)
struct subordinate * x;
{
struct subordinate * sub;
struct subordinate * y;
struct subordinate * top;

	if (x == NULLSUBORD)
		return (x);
	
	top = (struct subordinate *) smalloc (sizeof(struct subordinate));
	top->sub_copy =  x->sub_copy;
	top->sub_rdn = rdn_cpy(x->sub_rdn);
	top->sub_aliasentry = x->sub_aliasentry;
	top->sub_next = NULLSUBORD;
	y = top;

	for (x=x->sub_next; x != NULLSUBORD; x=x->sub_next) {
		sub = (struct subordinate *) smalloc (sizeof(struct subordinate));
		sub->sub_copy =  x->sub_copy;
		sub->sub_rdn = rdn_cpy(x->sub_rdn);
		sub->sub_aliasentry = x->sub_aliasentry;
		sub->sub_next = NULLSUBORD;
		y->sub_next = sub;
		y = sub;
	}

	return (top);	
}

/* ARGSUSED */
cache_list (ptr, prob,dn,sizelimit)
struct subordinate *ptr;
int             prob;
DN 		dn;
int 		sizelimit;
{
	struct list_cache *cache;
	struct subordinate *sub;
	register int i;

	if ((cache = find_list_cache (dn,0)) == NULLCACHE) {
		cache = (struct list_cache *) smalloc (sizeof (struct list_cache));
		cache->list_dn = dn_cpy (dn);
		cache->list_subs = subord_cpy (ptr);
		cache->list_sub_top = cache->list_subs;
		cache->list_next = list_top;
		cache->list_problem = prob;
		list_top = cache;
	} else {
		subords_free (cache->list_sub_top);
		cache->list_subs = subord_cpy (ptr);
			cache->list_sub_top = cache->list_subs;
		cache->list_problem = prob;
	}

	for (i=0, sub=cache->list_subs; 
		sub != NULLSUBORD; 
		i++, sub = sub->sub_next);
	cache->list_count = i;

}

delete_list_cache (adn)
DN adn;
{
DN dntop, trail = NULLDN;
struct list_cache *ptr, *lt = NULLCACHE;

	if (adn == NULLDN)
		return;

	dntop = adn;

	for (; adn->dn_parent != NULLDN; adn=adn->dn_parent)
		trail = adn;

	if (trail == NULLDN)
		dntop = NULLDN;
	else
		trail->dn_parent = NULLDN;

        for (ptr = list_top; ptr != NULLCACHE; ptr = ptr->list_next) {
                if (dn_cmp (ptr->list_dn, dntop) == 0) {
			if (lt == NULLCACHE) 
				list_top = ptr->list_next;
			else 
				lt->list_next = ptr->list_next;
			subords_free(ptr->list_sub_top);
			free ((char *)ptr);
			if (trail != NULLDN)
				trail->dn_parent = adn;
			return;
		}
		lt = ptr;
	}
	if (trail != NULLDN)
		trail->dn_parent = adn;
}

struct list_cache *find_list_cache (dn,sizelimit)
DN dn;
int sizelimit;
{
	struct list_cache *ptr;
	int i;
	for (ptr = list_top; ptr != NULLCACHE; ptr = ptr->list_next)
		if (dn_cmp (ptr->list_dn, dn) == 0)
			if ((ptr->list_problem == LSR_NOLIMITPROBLEM) 
				|| ((ptr->list_count >= sizelimit) && (sizelimit != -1)))
				{
				ptr->list_subs = ptr->list_sub_top;
				if (sizelimit == -1) 	
					return (ptr);
				/* only want sizelimit of them */
				for (i=ptr->list_count - sizelimit; i>0; i--)
					ptr->list_subs = ptr->list_subs->sub_next;
				return (ptr);
				}

	return (NULLCACHE);
}


free_all_list_cache ()
{
	struct list_cache *ptr, *pn;
	for (ptr = list_top; ptr != NULLCACHE; ptr = pn)  {
	        pn = ptr->list_next;
		dn_free (ptr->list_dn);
		subords_free (ptr->list_sub_top);
		free ((char *)ptr);
	    }
}




cache_entry (ptr, complete, vals)
EntryInfo      *ptr;
char            complete;
char            vals;
{
	Entry           make_path ();
	DN              dnptr;
	extern 	AttributeType at_alias;
	Attr_Sequence	as, as_merge_aux();

	/* use e_lock to indicate if values are present */

	if (ptr->ent_dn == NULLDN)
		return;

	dn_free (current_dn);

	current_dn = dn_cpy (ptr->ent_dn);

	for (dnptr = current_dn; dnptr->dn_parent != NULLDN; dnptr = dnptr->dn_parent)
		;

	if ((current_entry = local_find_entry_aux (current_dn, FALSE)) != NULLENTRY) {
			
		current_entry->e_age = time((time_t *)0);
		if ((vals && complete) || 
		   ((current_entry->e_data == E_TYPE_CACHE_FROM_MASTER) 
		     && (time((time_t *)0) - current_entry->e_age > cache_timeout))) {
			
			as_free (current_entry->e_attributes);
			current_entry->e_attributes = as_cpy(ptr->ent_attr);
			current_entry->e_lock = vals;
			current_entry->e_complete = complete;
		} else if (!current_entry->e_complete) {
			current_entry->e_complete = complete;
			current_entry->e_attributes = as_merge_aux (current_entry->e_attributes, as_cpy(ptr->ent_attr));
			if (vals != current_entry->e_lock)
				current_entry->e_lock = FALSE;
		} else { 
			current_entry->e_attributes = as_merge_aux (current_entry->e_attributes, as_cpy(ptr->ent_attr));
		}
	} else {
		if ((current_entry = make_path (current_dn)) == NULLENTRY)
			return;
/*
		current_entry->e_name = rdn_cpy (dnptr->dn_rdn);
*/
		current_entry->e_complete = complete;
		current_entry->e_data = E_TYPE_CACHE_FROM_MASTER;
		current_entry->e_age = time((time_t *)0);
		current_entry->e_lock = vals;
		current_entry->e_attributes = as_cpy(ptr->ent_attr);
		local_cache_size++;
	}

	/* insert alias pointers */
	for ( as = current_entry->e_attributes; as != NULLATTR; as = as->attr_link) {
		if (as->attr_type == at_alias)
			if (as->attr_value)
				current_entry->e_alias = (DN) as->attr_value->avseq_av.av_struct;
	}
}


delete_cache (adn)
DN              adn;
{
	Entry           ptr;

	delete_list_cache (adn);

	if ((ptr = local_find_entry_aux (adn, FALSE)) != NULLENTRY) {
		if (ptr->e_data == E_TYPE_CACHE_FROM_MASTER) {
			local_cache_size--;
			if (ptr->e_children != NULLAVL) {
				ptr->e_data = E_TYPE_CONSTRUCTOR;
				ptr->e_complete = FALSE;	
				as_free (ptr->e_attributes);
				ptr->e_attributes = NULLATTR;
			} else {
                                ( void ) avl_delete( &ptr->e_parent->e_children,
                                    (caddr_t) ptr->e_name, entryrdn_cmp );
			}
		}
	}
}


Entry local_find_entry (object,deref)
DN                      object;
char deref;
{
Entry the_entry;

	if ((the_entry = local_find_entry_aux (object,deref)) == NULLENTRY)
		return NULLENTRY;

	if ((the_entry->e_data == E_TYPE_CACHE_FROM_MASTER) 
		&& (time((time_t *)0) - the_entry->e_age > cache_timeout)) {
		DLOG (log_dsap,LLOG_TRACE,("local find entry - timeout"));
		return (NULLENTRY);
	} else
		return (the_entry); 
}

Entry local_find_entry_aux (object,deref)
DN                      object;
char deref;
{
Entry  the_entry;
register RDN    b_rdn;
DN     dn;
Avlnode	*kids;

	if (database_root == NULLENTRY) 
		return (NULLENTRY);

	if ((dn = object) == NULLDN)
		return (database_root);

	b_rdn = dn->dn_rdn;
	if ((kids = database_root->e_children) == NULLAVL)
		return (NULLENTRY);

	b_rdn = dn->dn_rdn;

	for(;;) { /* break or return out */
		if ((the_entry = (Entry) avl_find(kids, (caddr_t) b_rdn, 
					 entryrdn_cmp)) == NULLENTRY)
			return(NULLENTRY);

		if ( the_entry->e_alias != NULLDN )
			/* got an alias entry */
			if (deref) {
				Entry new_entry;
				if (dn_cmp (the_entry->e_alias,object) == 0) 
					return (NULLENTRY);
				new_entry = local_find_entry (the_entry->e_alias,deref);
				if (new_entry == NULLENTRY ) 
					return (NULLENTRY);
				the_entry = new_entry;
			} else if ( dn->dn_parent == NULLDN) 
				return (the_entry);
			else 
				return (NULLENTRY);

		if ( dn->dn_parent == NULLDN) 
			return (the_entry); 

		dn = dn->dn_parent;
		b_rdn = dn->dn_rdn;

		if (the_entry->e_children == NULLAVL)
			return (NULLENTRY);

		kids = the_entry->e_children;
	}
	/* NOTREACHED */
}


DN get_copy_dn (entryptr)
Entry entryptr;
{
register DN dn;
register DN dnptr;
register Entry ptr;

	if ((entryptr == NULLENTRY) || (entryptr->e_parent == NULLENTRY))
		return NULLDN;

	dn = dn_comp_new (rdn_cpy (entryptr->e_name));
	for (ptr = entryptr->e_parent; ptr->e_parent != NULLENTRY; ptr = ptr->e_parent) {
		dnptr = dn_comp_new (rdn_cpy (ptr->e_name));
		dnptr->dn_parent = dn;
		dn = dnptr;
	}

	return (dn);
}


IFP unrav_fn = NULLIFP;
IFP schema_fn = NULLIFP;

unravel_attribute (eptr,error)
Entry eptr;
struct DSError * error;
{
	if (unrav_fn == NULLIFP)
		return (OK);
	else
		return ((*unrav_fn)(eptr,error));
}

check_schema (eptr,as,error)
Entry eptr;
Attr_Sequence as;
struct DSError * error;
{
	if (schema_fn == NULLIFP)
		return (OK);
	else 
		return ((*schema_fn)(eptr,as,error));
}


char * new_version ()
{
time_t clock;
struct UTCtime ut;

	(void) time (&clock);
	tm2ut (gmtime (&clock),&ut);
	return (strdup(utct2str(&ut)));
}

