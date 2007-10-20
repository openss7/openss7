/* ds_init.c - initialise the DSA */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/ds_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/ds_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: ds_init.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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


#include "quipu/config.h"
#include "quipu/util.h"
#include "quipu/read.h"
#include "quipu/dua.h"
#include "quipu/connection.h"
#include "quipu/entry.h"

AV_Sequence super_user;
Entry subtree_load ();
Entry make_path ();
extern LLog * log_dsap;
extern DN mydsadn;
extern Entry database_root;
static Entry load_dsa_cache_entry(), load_dsa_remote_entry();
extern Entry local_find_entry_aux();
extern Attr_Sequence entry_find_type();
extern Attr_Sequence dsa_real_attr;
extern char * mydsaname;
extern char * treedir;
extern char * get_entry_passwd();
extern char * new_version();
extern char * quipuversion, * TidyString();
extern int parse_status;
extern IFP unrav_fn;
extern IFP schema_fn;
extern IFP restart_fn;
extern AttributeType at_version;
extern SFD attempt_restart ();
time_t	timenow;

static set_context ();

dsa_init ()
{
Attr_Sequence as, get_cacheEDB();
AttributeType manager;
DN str2dn();
struct edb_info * dsainfo;
AV_Sequence avs;
Entry newentry;
Entry my_entry;
int real_unravel_attribute ();
int real_check_schema ();
char loadstate = TRUE;
struct DSError error;
Entry akid;

	check_dsa_known_oids ();

	unrav_fn = (IFP) real_unravel_attribute;
	schema_fn = (IFP) real_check_schema;
	restart_fn = (IFP) attempt_restart;

	if (( manager = AttrT_new (MANAGER_OID)) == NULLAttrT)
		fatal (-1,"Manager - unknown attribute - check oid tables");

	LLOG (log_dsap,LLOG_NOTICE,("dsa name %s",mydsaname));

	if ((mydsadn = str2dn (mydsaname)) == NULLDN) {
		fatal (-2,"Invalid dsa name");
	}

	if ((my_entry = load_dsa_cache_entry (mydsadn)) == NULLENTRY) {
		if (parse_status != 0)
		     fatal (-3,"Can't load EDB subtree holding my DSA entry");

		if (database_root != NULLENTRY) 
		     fatal (-4,"Found EDB - but my DSA entry not in it!");

		fatal (-4,"can't locate my DSA entry in local database!");

	} else if (my_entry->e_data == E_TYPE_CACHE_FROM_MASTER)
		shadow_myentry ();

	if (get_entry_passwd (my_entry->e_attributes) == NULLCP) 
		/* This is not a fatal error, but some remote operations may fail */
		LLOG(log_dsap,LLOG_EXCEPTIONS,("Can't find my own PASSWORD"));

	if (dsa_real_attr) {
		if (as_cmp (my_entry->e_attributes,dsa_real_attr) != 0) {
			LLOG (log_dsap,LLOG_EXCEPTIONS,
 ("DSA.real entry inconsistent with EDB -- problem should fix itself later!"));
			as_free (my_entry->e_attributes);
			my_entry->e_attributes = as_cpy (dsa_real_attr);
			if (unravel_attribute (my_entry,&error) != OK) 
				fatal (-82,"schema error in DSA entry");
		}
	} else
		dsa_real_attr = as_cpy (my_entry->e_attributes);

	if (my_entry->e_dsainfo != NULLDSA) {
		/* get manager attribute */
		if ((as = entry_find_type(my_entry,manager)) == NULLATTR )
			fatal (-5,"Manager attribute missing in my own entry");
		AttrT_free (manager);
		super_user = avs_cpy ((AV_Sequence)as->attr_value);

		if (quipu_ctx_supported(my_entry) < 5) {
			LLOG(log_dsap,LLOG_EXCEPTIONS,(
			"Adding QUIPU and/or Internet DSP to application context!!!"));
			set_context (my_entry);
		}

		my_entry->e_dsainfo->dsa_version = 
			TidyString (strdup (quipuversion));
		if (as = entry_find_type (my_entry,at_version)) 
			if ( strcmp (
			     (char *) as->attr_value->avseq_av.av_struct,
			     my_entry->e_dsainfo->dsa_version) != 0) {

			if (as->attr_value->avseq_av.av_struct)
				free (as->attr_value->avseq_av.av_struct);
			as->attr_value->avseq_av.av_struct = 
				(caddr_t) strdup (my_entry->e_dsainfo->dsa_version);

			if (as = as_find_type (dsa_real_attr,at_version)) {
				if (as->attr_value->avseq_av.av_struct)
					free (as->attr_value->avseq_av.av_struct);
				as->attr_value->avseq_av.av_struct = 
				    (caddr_t) strdup (my_entry->e_dsainfo->dsa_version);
			}

			if (parse_status == 0) 
			 if (my_entry->e_data == E_DATA_MASTER) {
			    if (my_entry->e_parent != NULLENTRY)
				my_entry->e_parent->e_edbversion = new_version();
			    LLOG (log_dsap,LLOG_NOTICE,("Updating version number"));
#ifdef TURBO_DISK
				if (turbo_write(my_entry) != OK)
					fatal (-33,"self rewrite failed - check database");
#else
			        akid = (Entry) avl_getone(my_entry->e_parent->e_children);
			        if (journal (akid) != OK)
					fatal (-33,"self rewrite failed - check database");
#endif
		         } else {
				write_dsa_entry(my_entry);
			 }
		}

	} else 
		fatal (-6,"No edbinfo attribute in my own entry");

	if (parse_status != 0)
		loadstate = FALSE;

	for (avs = my_entry->e_dsainfo->dsa_attr ; avs != NULLAV; avs=avs->avseq_next) {
		if (avs->avseq_av.av_struct == NULL)
			continue;
		dsainfo = (struct edb_info *) avs->avseq_av.av_struct;
		if ((newentry = make_path (dsainfo->edb_name)) == NULLENTRY)
			continue;

		(void) subtree_load (newentry,dsainfo->edb_name);
		if (parse_status != 0)
			loadstate = FALSE;
	}

	if (loadstate == FALSE)
		fatal (-7,"DSA Halted");

	if ((akid = (Entry) avl_getone(database_root->e_children))
	    != NULLENTRY )
		database_root->e_data = akid->e_data;

	/* Load cached EDB files - if any */
	if ((as = get_cacheEDB()) != NULLATTR) {
		(void) time (&timenow);

		for (avs = as -> attr_value; avs != NULLAV; avs = avs -> avseq_next) {
			if ((newentry = make_path ((DN)avs->avseq_av.av_struct)) == NULLENTRY)
				continue;
			newentry = subtree_load (newentry,(DN)avs->avseq_av.av_struct);
			/* Should timestamp using version number ! */
			if (newentry)
				newentry->e_age = timenow;
		}
	}

#ifndef TURBO_DISK
    	free_phylinebuf();	/* Large buffer used in loading text database */
#endif

	return (OK);

}

static Entry load_dsa_cache_entry(dn)
DN dn;
{
DN ptr,trail = NULLDN;
Entry newentry, res;
DN tmp;
DN tmp2;
int fail = FALSE;

	tmp = dn_cpy (dn);

	if (tmp->dn_parent == NULLDN) {
		database_root = subtree_load (NULLENTRY,NULLDN);
		dn_free (tmp);
		if (parse_status != 0)
			return NULLENTRY;
		if ((res = local_find_entry_aux (dn,TRUE)) != NULLENTRY) 
			load_pseudo_attrs (res->e_data);
		return res;
	}

	database_root = subtree_load (NULLENTRY,NULLDN);
	if (parse_status != 0)
		fail = TRUE;

	for (ptr=tmp; ptr->dn_parent != NULLDN; ptr=ptr->dn_parent) {
		trail = ptr;
		tmp2 = trail->dn_parent;
		trail->dn_parent = NULLDN;
		if ((newentry = make_path (tmp)) == NULLENTRY)
			parse_error ("Make_path failed",NULLCP);
		else
			(void) subtree_load (newentry,tmp);

		trail->dn_parent = tmp2;
	
		if (parse_status != 0) 
			fail = TRUE;
		
	}

	dn_free (tmp);

	if ((res = local_find_entry_aux (dn,TRUE)) != NULLENTRY)
		load_pseudo_attrs (res->e_data);

	if (fail)
		return NULLENTRY;

	return res;
}

static set_context (eptr)
Entry eptr;
{
AttributeType at;
AttributeValue av;
AV_Sequence avs;
Attr_Sequence as, entry_find_type();
extern int	  no_last_mod;

	/* DAP */
	at = AttrT_new (APPLCTX_OID);
	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("OID");
	av->av_struct = (caddr_t) oid_cpy (DIR_ACCESS_AC);
	avs = avs_comp_new(av);
	as = as_comp_new (at,avs,NULLACL_INFO);

	/* DSP */
	at = AttrT_new (APPLCTX_OID);
	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("OID");
	av->av_struct = (caddr_t) oid_cpy (DIR_SYSTEM_AC);
	avs = avs_comp_new(av);
	as = as_merge (as,as_comp_new (at,avs,NULLACL_INFO));

	/* QSP */
	at = AttrT_new (APPLCTX_OID);
	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("OID");
	av->av_struct = (caddr_t) oid_cpy (DIR_QUIPU_AC);
	avs = avs_comp_new(av);
	as = as_merge (as,as_comp_new (at,avs,NULLACL_INFO));

	/* ISP */
	at = AttrT_new (APPLCTX_OID);
	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("OID");
	av->av_struct = (caddr_t) oid_cpy (DIR_INTERNET_AC);
	avs = avs_comp_new(av);
	as = as_merge (as,as_comp_new (at,avs,NULLACL_INFO));

	eptr->e_attributes = as_merge (eptr->e_attributes,as);

	if (quipu_ctx_supported(eptr) != 5) 
		fatal (-1, "Setting application context botch");
}
