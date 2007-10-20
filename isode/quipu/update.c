/* update.c - write EDB back to disk after modify */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/update.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/update.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: update.c,v $
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


#include "quipu/util.h"
#include "quipu/entry.h"
#include "tailor.h"
#include "quipu/read.h"
#include "quipu/dua.h"
#include "quipu/connection.h"
#include "pepsy.h"
#include "quipu/Quipu-types.h"
#include "quipu/turbo.h"
#include "quipu/malloc.h"
extern int parent_link();

#ifndef NO_STATS
extern LLog * log_stat;
#endif
extern char *new_version ();

extern LLog * log_dsap;
extern char remote_lookup;
extern int local_slave_size;
extern int slave_edbs;
extern Entry database_root;
extern DN mydsadn;
extern Attr_Sequence dsa_real_attr;
extern Entry local_find_entry_aux();
extern time_t lastedb_update, timenow;
extern time_t	conn_timeout;
extern time_t	nsap_timeout;
extern struct oper_act * pending_ops;

struct oper_act *	oper_alloc();
struct oper_act *	make_get_edb_op();

static int split_size = 10;
static get_more_edb ();
static read_part_edb ();
static pull_up_result ();
static edb_start ();
static edb_continue ();

char * edbtmp_path = NULLCP;

#ifndef TURBO_DISK
/* routine name is historic - not significant */

journal (myentry)
Entry myentry;
{
char * filename, *dn2edbfile();
char savefile [LINESIZE], newfile[LINESIZE];
DN dn;
extern char * treedir;
extern char * parse_file;
Entry liststart;
extern int errno;

	if (myentry == NULLENTRY) {
		LLOG (log_dsap,LLOG_FATAL,("update edb problem"));
		return NOTOK;
	}

	if ((myentry->e_parent != NULLENTRY) && (myentry->e_parent->e_leaf)) {
		liststart = myentry;
		dn = get_copy_dn (liststart->e_parent);
		if ((filename = dn2edbfile (dn)) == NULLCP) {
			dn_free (dn);
			return NOTOK;
		}
		myentry->e_parent->e_leaf = FALSE;  /* not a leaf now !! */

		if ( write_edb(liststart,filename) != OK) {
			dn_free (dn);
			return NOTOK;
		}
	} else {
		if (myentry->e_parent == NULLENTRY) {
			liststart = myentry;
			dn = NULLDN;
		} else {
			liststart = (Entry) avl_getone(myentry->e_parent->e_children);
	 		dn = get_copy_dn (liststart->e_parent);
		}
		if ((filename = dn2edbfile (dn)) == NULLCP) {
			dn_free (dn);
			return NOTOK;
		}

		(void) strcpy (newfile,filename);
		(void) strcat (newfile,".new");

		if ( write_edb(liststart,newfile) != OK) {
		    (void) unlink (newfile);
		    dn_free (dn);
		    return NOTOK;
		}

		(void) strcpy (savefile,filename);
		(void) strcat (savefile,".bak");

		(void) unlink (savefile);
		if (link (filename, savefile) == NOTOK)
		    SLOG (log_dsap, LLOG_EXCEPTIONS, savefile,
			  ("unable to link %s to", filename));
#ifdef ISC  /* Interactive UNIX V R3 */
		else
			/* dangerous - but rename will fail without it */
			(void) unlink (filename);
#endif
		if (rename (newfile, filename) == NOTOK) {
		    SLOG (log_dsap, LLOG_EXCEPTIONS, filename,
			  ("unable to rename %s to", newfile));
		    if (link (savefile, filename) == NOTOK
			    && !fileexists (filename))
			LLOG (log_dsap, LLOG_EXCEPTIONS,
			      ("and couldn't get old file back - PANIC!!!")) 
		    dn_free (dn);
		    return NOTOK;
		}
	}

	dn_free (dn);

	return OK;
}
#endif TURBO_DISK

modify_attr (eptr,who)
Entry eptr;
DN who;
{
AttributeType at;
AttributeValue av;
AV_Sequence avs;
Attr_Sequence as, old, entry_find_type();
extern int	  no_last_mod;

	if (no_last_mod)
		return;

	at = AttrT_new (LAST_MOD_OID);

	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("UTCTime");
	av->av_struct = (caddr_t) new_version();

	avs = avs_comp_new(av);

	if ((old = as_find_type (eptr->e_attributes,at)) == NULLATTR) {
		as = as_comp_new (at,avs,NULLACL_INFO);
		eptr->e_attributes = as_merge (eptr->e_attributes,as);
	} else {
		avs_free (old->attr_value);
		old->attr_value = avs;
		AttrT_free (at);
	}

	at = AttrT_new (MOD_BY_OID);

	av = AttrV_alloc ();
	av->av_syntax = str2syntax ("DN");
	av->av_struct = (caddr_t) dn_cpy (who);

	avs = avs_comp_new(av);

	/* Is it inherited ? */
	set_inheritance (eptr);
	if ((old = entry_find_type (eptr,at)) != NULLATTR) 
		/* possibly... */
		if (as_find_type (eptr->e_attributes,at) == NULLATTR)
			/* yes ! */
			if (avs_cmp(avs, old->attr_value) == 0) {
				/* No need to change it ! */
				avs_free (avs);
				return;
			}

	if (old == NULLATTR) {
		as = as_comp_new (at,avs,NULLACL_INFO);
		eptr->e_attributes = as_merge (eptr->e_attributes,as);
	} else {
		avs_free (old->attr_value);
		old->attr_value = avs;
		AttrT_free (at);
	}
}

static allowed_to_send (a,b)
register DN  a,b;
{
	/* Return TRUE if the DNs are the same */
	/* Return TRUE if all components of 'a' match, but 'b' has one extra */
	/* False otherwise */

	if ((a == NULLDN) || (b == NULLDN))
		return FALSE;

	for (; a != NULLDN && b != NULLDN ; a = a->dn_parent, b = b->dn_parent)
		if ( dn_comp_cmp (a,b) == NOTOK)
			return FALSE;

	if (( b == NULLDN) || (b->dn_parent == NULLDN))
		return TRUE;
	else
		return FALSE;

}


do_get_edb (arg,error,result,binddn,fd)
struct getedb_arg *arg;
struct DSError	  *error;
struct getedb_result *result;
DN binddn;
int fd;
{
Entry eptr;
Entry my_entry;
AV_Sequence avs;
struct edb_info * dsainfo;
char proceed = FALSE;
struct dn_seq	* dnseq;
struct di_block	* di;
Entry	akid;

	DLOG (log_dsap,LLOG_DEBUG,("getedb '%s'",arg->ga_version));

	if (arg->ga_type == GA_CONTINUE) 
		return edb_continue (arg,error,result,binddn,fd);

	switch(really_find_entry (arg->ga_entry,FALSE,NULLDNSEQ,FALSE,&(eptr),error,&(di)))
	{
	case DS_OK:
	    /*
	    *  Entry has been found and returned via eptr.
	    *  Go through and process this entry.
	    */
	    break;

	case DS_CONTINUE:
	    /*
	    * Get edb operations should never generate referrals.
	    * Free the di_blocks generated and return an error.
	    */
	    error->dse_type = DSE_SERVICEERROR;
	    error->ERR_SERVICE.DSE_sv_problem = DSE_SV_CHAININGREQUIRED;
	    return (DS_X500_ERROR);

	case DS_X500_ERROR:
	    /* something wrong with the request - error should be filled out */
	    return(DS_X500_ERROR);

	default:
	    LLOG(log_dsap, LLOG_EXCEPTIONS, 
		 ("do_get_edb() - really_find_entry() failed"));
	    error->dse_type = DSE_SERVICEERROR;
	    error->ERR_SERVICE.DSE_sv_problem = DSE_SV_DITERROR;
	    return (DS_X500_ERROR);
	}

	if ((my_entry = local_find_entry_aux (mydsadn,TRUE)) == NULLENTRY)
		fatal (84,"my entry has gone - no getedb");

	/* Check we will send to this DSA */
	for (avs = my_entry->e_dsainfo->dsa_attr ; avs != NULLAV; 
	     				avs=avs->avseq_next) {
		if (avs->avseq_av.av_struct == NULL)
			continue;
		dsainfo = (struct edb_info *) avs->avseq_av.av_struct;
		if (dn_cmp(dsainfo->edb_name,arg->ga_entry) == 0) {
			for (dnseq=dsainfo->edb_allowed; dnseq!=NULLDNSEQ; 
			     			dnseq=dnseq->dns_next) {
				if (allowed_to_send(dnseq->dns_dn,binddn)) {
					proceed = TRUE;
					break;
				}
			}
		}
		if (proceed)
			break;
	}
					
	if (!proceed) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_ACCESSRIGHTS;
		return (DS_ERROR_REMOTE);
	}

	akid = (Entry) avl_getone(eptr->e_children);
	if (akid == NULLENTRY || (akid->e_data != E_DATA_MASTER
	    && akid->e_data != E_TYPE_SLAVE)) {
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_DITERROR;
		return (DS_X500_ERROR);
	}

	result->gr_nextEntryPos = 0;
	result->gr_edb = NULLAVL;
	result->gr_next = NULL_GETRESULT;

	if (eptr->e_edbversion != NULLCP) {
		DLOG(log_dsap, LLOG_DEBUG, ("edb_ver = %s", eptr->e_edbversion));

		if ( (arg->ga_type == GA_SENDIFMORERECENT) && 
		     (lexequ (arg->ga_version,eptr->e_edbversion) == 0)) {
			result->gr_version = strdup (eptr->e_edbversion);
			return (DS_OK);
		}
	} else 
		eptr->e_edbversion = new_version();

	result->gr_version = strdup(eptr->e_edbversion);

	if (arg->ga_type == GA_GETVERSION) 
		return (DS_OK);

	result->gr_edb = eptr->e_children;

	if (arg->ga_maxEntries != 0)
		return edb_start (arg,error,result,binddn,fd);

	return (DS_OK);
}

slave_update () 
{ 
	(void) update_aux (NULLDN, 0); 
	shadow_update ();
	lastedb_update = timenow;
}

update_aux (dn, isroot)
DN	dn;
int	isroot;
{
Entry my_entry, make_path();
Entry find_sibling();
extern DN mydsadn;
struct edb_info * dsainfo;
Entry eptr;
static char *version = NULLCP;
AV_Sequence avs;
AV_Sequence avs_head;
int		  success;

	DLOG (log_dsap,LLOG_TRACE,("slave update"));

	if ((my_entry = local_find_entry_aux (mydsadn,TRUE)) == NULLENTRY)
		fatal (82,"Can't update slaves - my entry has gone");

	avs_head = avs_cpy(my_entry->e_dsainfo->dsa_attr);

	for (avs = avs_head ; avs != NULLAV; avs=avs->avseq_next)
	{
		if (avs->avseq_av.av_struct == NULL)
			continue;
		dsainfo = (struct edb_info *) avs->avseq_av.av_struct;
		if (dsainfo->edb_getfrom == NULLDN)
			continue;  /* not an EDB to update */
		if ((dn || isroot) && dn_cmp (dn, dsainfo -> edb_name) != OK)
		    continue;	   /* not an EDB this time */

		if ((eptr = local_find_entry (dsainfo->edb_name,FALSE)) == NULLENTRY) {
			version = "0000000000Z";
			if ((eptr = make_path (dsainfo->edb_name)) == NULLENTRY) {
				pslog (log_dsap,LLOG_EXCEPTIONS,
				       "edbinfo references EDB that does not exist !!!",
				       (IFP)dn_print,(caddr_t) dsainfo->edb_name);
				continue;
			}
		} 
		else
		{
			if((version = eptr->e_edbversion) == NULLCP)
			{
			    LLOG(log_dsap, LLOG_TRACE, 
				 ("update_aux: edbversion was NULLCP"));
			    version = "0000000000Z";
			}
		}

		success = send_get_edb(version, dsainfo->edb_name, 
				       dsainfo->edb_getfrom);

		if(dn || isroot)
		    break;

		(void) dsa_wait (0);	/* accept any results of previous ops */

	}
	avs_free (avs_head);

	return((dn || isroot) ? success : OK);
}

int	 send_get_edb (version,dn,from)
char	* version;
DN dn,from;
{
struct di_block		* di;
struct DSError		  error;
struct oper_act		* on;
char    buffer[BUFSIZ];
PS	    ps;

	switch(get_dsa_info(from, NULLDNSEQ, &(error), &(di)))
	{
	case DS_OK:
	    /*
	    *  di is a completed dsa info block
	    *  Make a get_edb operation from it, attempt to send the operation
	    *  and link the operation onto the global list of get_edb
	    *  operations.
	    */
	    if (  (log_dsap -> ll_events & LLOG_NOTICE)
		&&(ps = ps_alloc (str_open))) {

		if (str_setup (ps, buffer, sizeof buffer, 1) != NOTOK) {
		    ps_printf (ps, "contact ");
		    dn_print (ps, from, EDBOUT);
		    ps_printf (ps, " for ");
		    if (dn)
			dn_print (ps, dn, EDBOUT);
		    *ps -> ps_ptr = NULL;

		    LLOG (log_dsap, LLOG_NOTICE, ("%s", buffer));
		}

		(void) ps_free (ps);
	    }

#ifdef DEBUG
	    DLOG(log_dsap, LLOG_DEBUG, ("send_get_edb - get_dsa_info OK:"));
	    di_list_log(di);
#endif
	    if((on = make_get_edb_op(dn, version, di)) == NULLOPER)
	    {
		/* Flake out screaming */
		LLOG(log_dsap, LLOG_EXCEPTIONS, 
		     ("make_get_edb_op failed for send_get_edb"));
		return(NOTOK);
	    }

	    schedule_operation (on);

	    return(OK);

	case DS_CONTINUE:
	    /*
	    *  di is a deferred dsa info block
	    *  make the operation and suspend waiting for the di_block to be 
	    *  woken up.
	    */
#ifdef DEBUG
	    DLOG(log_dsap, LLOG_DEBUG, ("send_get_edb - get_dsa_info CONT:"));
	    di_list_log(di);
#endif
	    if((on = make_get_edb_op(dn, version, di)) == NULLOPER)
	    {
		/* Flake out screaming */
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("make_get_edb_op failed for send_get_edb"));
		return(NOTOK);
	    }

	    on->on_state = ON_DEFERRED;

	    on->on_next_task = get_edb_ops;
	    get_edb_ops = on;

	    if (  (log_dsap -> ll_events & LLOG_NOTICE)
		&&(ps = ps_alloc (str_open))) {
		if (str_setup (ps, buffer, sizeof buffer, 1) != NOTOK) {
		    ps_printf (ps, "contact ");
		    dn_print (ps, from, EDBOUT);
		    ps_printf (ps, " for ");
		    if (dn)
			dn_print (ps, dn, EDBOUT);
		    *ps -> ps_ptr = NULL;

		    LLOG (log_dsap, LLOG_NOTICE, ("%s", buffer));
		}

		(void) ps_free (ps);
	    }

	    return(OK);

	case DS_X500_ERROR:
	    /* Error encountered generating di_block */
	    LLOG(log_dsap, LLOG_TRACE, ("send_get_edb - get_dsa_info returned X500 ERROR"));
	    log_ds_error (&error);
	    ds_error_free (&error);
	    return(NOTOK);

	default:
	    LLOG(log_dsap, LLOG_EXCEPTIONS, ("send_get_edb - get_dsa_info unexpected return"));
	    return(NOTOK);
	}
	/* NOTREACHED */
}


static Entry	g_parent;
static int 	g_entry_cnt;

/*
 * unravel_edb - called from avl_apply on a new slave edb to unravel the
 * new entries and link them to their new parent node, which should be
 * set in g_parent before the call.
 */

static unravel_edb(e, error)
Entry           e;
struct DSError  *error;
{
        e->e_parent = g_parent;
        if (unravel_attribute(e, error) != OK)
                return(NOTOK);

	shadow_entry (e);
        turbo_add2index(e);
	return OK;
}

static quick_unrav (e, error)
Entry           e;
struct DSError  *error;
{
	if (e->e_data == E_TYPE_CONSTRUCTOR)
		return(OK);

        if (unravel_attribute(e, error) != OK)
                return(NOTOK);

	return OK;
}

/*
 * make_parent - called from link_child to set the parent of a node.
 * it has to happen in this grossly inefficient way because make_parent
 * is called from avl_apply.
 */

static link_child(e, oldkids)
Entry   e;
Avlnode *oldkids;
{
struct DSError  error;

        Entry   old_entry;
        int     entryrdn_cmp();

        g_entry_cnt++;

        /* find the old entry the new one is replacing */
        old_entry = (Entry) avl_find(oldkids, (caddr_t) e->e_name, entryrdn_cmp);
        if (old_entry == NULLENTRY)
                return(OK);

        e->e_leaf = old_entry->e_leaf;
        e->e_allchildrenpresent = old_entry->e_allchildrenpresent;
        e->e_children = old_entry->e_children;

        turbo_index_delete(old_entry);
        turbo_add2index(e);

        /* link children to their new parent */
        (void) avl_apply(e->e_children, parent_link, (caddr_t) e, 
			 NOTOK, AVL_PREORDER);
	/* And unravel them to set new ACL pointers */
	/* MAY need to make this recursive */
        if (avl_apply(e->e_children, quick_unrav, (caddr_t) &error, 
		      NOTOK, AVL_PREORDER) == NOTOK) {
		log_ds_error (&error);
		return NOTOK;
	}

        if (old_entry->e_edbversion != NULLCP)
                e->e_edbversion = strdup(old_entry->e_edbversion);

        return(OK);
}


process_edb(on,newop)
struct oper_act	* on;
struct oper_act	** newop;
{
extern DN mydsadn;
Entry find_sibling();
Entry eptr;
Avlnode	*newkids;
int	entry_free();
struct DSError  error;
struct getedb_result	* result = &(on->on_resp.di_result.dr_res.dcr_dsres.res_ge);
struct getedb_arg	* arg = &(on->on_req.dca_dsarg.arg_ge);
char got_subtree = TRUE;

	if (result->gr_nextEntryPos != 0) {
		/* more to come */
		get_more_edb (on,newop);
		return;
	}  

	if (result->gr_pe == NULLPE) {
		DLOG (log_dsap, LLOG_TRACE,
		      ("  EDBs are the same (%d): %s",
		       on->on_id,on->on_getedb_ver));
		goto out;
	}

	if (!pull_up_result (arg,result))
		goto out;
	
	newkids = result->gr_edb;

	if ((eptr = local_find_entry (arg->ga_entry,FALSE)) == NULLENTRY) {
		pslog (log_dsap,LLOG_EXCEPTIONS,
		       "Updating something which does not exist !!!",
			(IFP)dn_print,(caddr_t) arg->ga_entry);
		goto out;
	}

	DLOG (log_dsap, LLOG_NOTICE,("  EDB updated from (%d): %s to: %s", 
	      on->on_id, on->on_getedb_ver, result->gr_version));

#ifndef NO_STATS
	{
	DN tmp_dn;
	tmp_dn = get_copy_dn (eptr);
	pslog (log_stat,LLOG_NOTICE,"Slave update",(IFP)dn_print,(caddr_t)tmp_dn);
	dn_free (tmp_dn);
	}
#endif

	if (eptr->e_edbversion) 
		free (eptr->e_edbversion);
	eptr->e_edbversion = NULLCP;

	if (result->gr_version == NULLCP) {
		eptr->e_edbversion = strdup ("Unknown");
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("EDBRES: NULL version"));
	} else
		eptr->e_edbversion = strdup (result->gr_version);

	/*
         * now unravel the attributes, linking all nodes to eptr, their
         * parent node.
         */

	g_parent = eptr;
	if (avl_apply(newkids, unravel_edb, (caddr_t) &error, NOTOK, 
		      AVL_INORDER)
	    == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, 
		     ("Error in new EDB - continuing with old"));
		log_ds_error(&error);
			/* ??? should we free newkids here ??? */
		goto out;
	}

	/*
	 * the new edb is now unravelled and linked to its parent.  now go
	 * through and link children of entries in the old edb that also 
	 * exist in the new edb to their new parent.  link_child updates
	 * g_entry_cnt with the number of entries in the new edb.
	 */

	g_entry_cnt = 0;
	if (avl_apply(newkids, link_child, (caddr_t) eptr->e_children, NOTOK,
	    AVL_INORDER) == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, 
		     ("Child problem in new EDB - continuing with old"));
			/* ??? should we free newkids here ??? */
		goto out;
	}


	(void) dsa_wait (0);	/* progress any other connections before writing EDB */

	if (eptr->e_children == NULLAVL)
		slave_edbs++;

	/*
	 * now free up entries from the old edb and update the size of
	 * our cache. avl_free returns the number of nodes freed.
	 */

	local_slave_size -= avl_free(eptr->e_children, entry_free);

	eptr->e_children = newkids;
	local_slave_size += g_entry_cnt;
	eptr->e_leaf = FALSE;
	eptr->e_allchildrenpresent = (got_subtree ? 2 : 1);

#ifdef TURBO_DISK
	if (turbo_writeall(eptr) != OK)
		fatal (-79,"Lost old EDB, can't write new one!!!");
#else
	if (journal ((Entry)avl_getone(eptr->e_children)) != OK)
		fatal (-79,"Lost old EDB, can't write new one !!!");
#endif

	if ((eptr = local_find_entry_aux (mydsadn,TRUE)) == NULLENTRY)
		fatal (-80,"My entry has disappeared from the DIT !!!");
	else if (as_cmp (eptr->e_attributes, dsa_real_attr) != 0) {
		LLOG (log_dsap, LLOG_TRACE, 
		      ("Slave Copy out of date with my entry - re-applying modify"));
		as_free (eptr->e_attributes);
		eptr->e_attributes = as_cpy (dsa_real_attr);
		if (unravel_attribute (eptr,&error) != OK) 
			fatal (-81,"real DSA entry bad error");
	}

	/* Arrange for connection to shut in 30 seconds unless
	   used by something else in that time
	   Better than leaving it for 5 minutes
        */

out:;

	if (on->on_conn) {
	    (void) time (&timenow);
	    on->on_conn->cn_last_used = timenow - conn_timeout + nsap_timeout;
	}

}

/*
*  get_edb_fail_wakeup suffices for both fail and error conditions
*  arising on a get edb operation.
*/
get_edb_fail_wakeup(on)
struct oper_act	* on;
{
#ifdef notanymore
    struct oper_act	* on_tmp;
    struct oper_act	**on_p;
#endif

    DLOG(log_dsap, LLOG_TRACE, ("get_edb_fail_wakeup"));

    if (on -> on_resp.di_type == DI_ERROR) {
	    pslog (log_dsap,LLOG_EXCEPTIONS,"Remote getEDB error",(IFP)dn_print,
		    (caddr_t) on->on_req.dca_dsarg.arg_ge.ga_entry);
	    log_ds_error (& on -> on_resp.di_error.de_err);

	    if (on->on_conn) {
		(void) time (&timenow);
		on->on_conn->cn_last_used = 
		    timenow - conn_timeout + nsap_timeout;
	    }
    }

#ifdef notanymore
    on_p = &(get_edb_ops);
    for(on_tmp = get_edb_ops; on_tmp != NULLOPER; on_tmp = on_tmp->on_next_task)
    {
	if(on_tmp == on)
	    break;

	on_p = &(on_tmp->on_next_task);
    }

    if(on_tmp != NULLOPER)
    {
	(*on_p) = on_tmp->on_next_task;
    }
    else
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, 
	("get_edb_fail_wakeup - op escaped from get_edb_ops (the global list)"));
    }

#else

    if (get_edb_ops == on) {
	pending_ops = get_edb_ops -> on_next_task;
	get_edb_ops -> on_next_task = NULLOPER;
	get_edb_ops = NULLOPER;
    } else 
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("Failure is not current opertion"));

#endif

    oper_conn_extract(on);
    oper_free(on);
}

struct oper_act	* make_get_edb_op(dn, version, di)
DN		  dn;
char		* version;
struct di_block	* di;
{
struct di_block	* di_tmp;
struct oper_act	* on_tmp;
struct getedb_arg	* arg;
int edb_size;

	DLOG(log_dsap, LLOG_TRACE, ("make_get_edb_op"));

	if((on_tmp = oper_alloc()) == NULLOPER)
	{
		LLOG(log_dsap, LLOG_EXCEPTIONS, 
		     ("make_get_edb_op - out of memory"));
		return(NULLOPER);
	}

	on_tmp->on_type = ON_TYPE_GET_EDB;
	on_tmp->on_req.dca_dsarg.arg_type = OP_GETEDB;
	on_tmp->on_getedb_ver = version;

	arg = &(on_tmp->on_req.dca_dsarg.arg_ge);

	arg->ga_entry = dn_cpy(dn);
	arg->ga_version = strdup(version);
	arg->ga_nextEntryPos = 0; 
	arg->ga_type = GA_SENDIFMORERECENT;    
	DLOG(log_dsap, LLOG_TRACE, ("EDBARG: ver = %s", arg->ga_version));

	edb_size = split_size;

	on_tmp->on_dsas = di;
	for(di_tmp=di; di_tmp!=NULL_DI_BLOCK; di_tmp=di_tmp->di_next)
	{
#ifdef DEBUG
	    DLOG(log_dsap, LLOG_DEBUG, ("Linking a di_block to this op"));
	    di_log(di_tmp);
#endif
	    if (di_tmp -> di_state != DI_COMPLETE)
		    edb_size = 0;	
	    		/* Can't tell if new protocol supported */
	    		/* If ISP this is reset later */
	    else {
		    int res = quipu_ctx_supported (di_tmp -> di_entry);
		    if ( ! (res == 5) || (res == 2))
			    edb_size = 0;	/* Old QSP */
	    }
	    di_tmp->di_type = DI_OPERATION;
	    di_tmp->di_oper = on_tmp;
	}

	arg->ga_maxEntries = edb_size;

	return(on_tmp);
}

struct edbops_list {
	DN edb_name;
	DN bind_name;	
	char * edb_fname;
	char * edb_version;
	PS   edb_ps;
	int  edb_next;
	int  edb_fd;
	struct edbops_list * next;
};

#define NULLEDBOP (struct edbops_list *)NULL

static struct edbops_list * edbops = NULLEDBOP;

static edb_start (arg,error,result,binddn,fd)
struct getedb_arg *arg;
struct DSError	  *error;
struct getedb_result *result;
DN binddn;
int fd;
{
PE spe, lpe, pe = NULLPE;
char buffer [LINESIZE];
char *fname = NULLCP;
int um;
FILE * fptr = (FILE *) NULL;
PS fps;
struct edbops_list * nextop;

	/* send first part of an EDB file */
	if (EDB_encode (result, &pe) == NOTOK) {
out:;
    		if (fname)
			free (fname);
    		if (pe)
			pe_free (pe);
    		if (fptr)
			(void) fclose (fptr);
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNAVAILABLE;
		return (DS_X500_ERROR);
	}

	/* This is a slow process, take a look at the network... */
	(void) dsa_wait (0);

	/* write PE to file */

	/* Make file name - this is where we could be clever an pick up
	   old files etc.  Worry about optimization later.
	*/

	(void) sprintf (buffer,"%s/%s.XXXXXX",edbtmp_path,result->gr_version);
	if ((fname = mktemp (strdup(buffer))) == NULLCP) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,
		      ("Too many getedbs at once '%s'",fname));
		goto out;
	}

        um = umask (0177);
	if ((fptr = fopen (fname,"w")) != NULL) {
		(void) umask (um);
		if ((fps = ps_alloc (std_open)) == NULLPS) {
			LLOG (log_dsap,LLOG_EXCEPTIONS,
			      ("Could not alloc PS file '%s'",fname));
			goto out;
		}				
		if ((std_setup (fps,fptr)) == NOTOK) {
			ps_free (fps);
			LLOG (log_dsap,LLOG_EXCEPTIONS,
			      ("Could not open PS file '%s'",fname));
			goto out;
		}
	} else {
		(void) umask (um);
		LLOG ( log_dsap,LLOG_EXCEPTIONS,
		      ("Could not open EDB/PE file '%s'",fname));
		goto out;
	}

	/* loop, writing each part of the set to file... */
	if ((spe = prim2seq (pe)) == NULLPE) {
	    ps_free (fps);
	    LLOG ( log_dsap,LLOG_EXCEPTIONS,
		      ("How comes its not a seq !?!"));
		goto out;
	}


	for (lpe = first_member (spe); lpe; lpe = next_member (spe, lpe)) {
	       if (pe2ps(fps, lpe) != OK) {
		       ps_free (fps);
		       LLOG ( log_dsap,LLOG_EXCEPTIONS,
			     ("Could not write EDB/PE file '%s'",fname));
		       goto out;
	       }
	}

	if (fclose (fptr) != 0) {
	        ps_free (fps);
		LLOG ( log_dsap,LLOG_EXCEPTIONS,
		      ("Could not close EDB/PE file '%s'",fname));
		fptr = (FILE *) NULL;
		goto out;
	}
	ps_free (fps);

	/* Now open file up for reading */
	if ((fptr = fopen (fname,"r")) != NULL) {
		if ((fps = ps_alloc (std_open)) == NULLPS) {
			LLOG (log_dsap,LLOG_EXCEPTIONS,
			      ("Could not alloc PS file (R)'%s'",fname));
			goto out;
		}				
		if ((std_setup (fps,fptr)) == NOTOK) {
			ps_free (fps);
			LLOG (log_dsap,LLOG_EXCEPTIONS,
			      ("Could not open PS file (R)'%s'",fname));
			goto out;
		}
	} else {
		LLOG ( log_dsap,LLOG_EXCEPTIONS,
		      ("Could not open EDB/PE file (R)'%s'",fname));
		goto out;
	}

	nextop = (struct edbops_list *) smalloc (sizeof (struct edbops_list));
	nextop -> edb_name = dn_cpy (arg->ga_entry);
	nextop -> bind_name = dn_cpy (binddn);
	nextop -> edb_fname = fname;
	nextop -> edb_version = strdup (result->gr_version);
	nextop -> edb_ps = fps;
	nextop -> edb_next = 0;
	nextop -> edb_fd = fd;
	nextop -> next = edbops;
	edbops = nextop;

	/* This is a slow process, take a look at the network... */
	(void) dsa_wait (0);

	pe_free (pe);

	/* Now behave as if it's the first continue call */
	/* Extra checks are made, but thats OK ! */

	return edb_continue (arg,error,result,binddn,fd);

}

check_getedb_ops(fd)
int fd;
{
struct edbops_list * nextop, *loop, *trail = NULLEDBOP;

	/* step through the list to find oper... */
	for ( nextop = edbops; nextop != NULLEDBOP; nextop = loop ) {

	        loop = nextop->next;

		if ( nextop->edb_fd != fd ) {
		        trail = nextop;
			continue;
		}

		(void) fclose ((FILE *)nextop->edb_ps->ps_addr);
		ps_free (nextop -> edb_ps);
		dn_free (nextop -> edb_name);
		dn_free (nextop -> bind_name);

		(void) unlink (nextop -> edb_fname);
		free (nextop -> edb_fname);
		free (nextop -> edb_version);

		if (trail == NULLEDBOP)
		    edbops = loop;
		else 
		    trail -> next = loop;

		free ((char *) nextop);
	}
}

static edb_continue (arg,error,result,binddn,fd)
struct getedb_arg *arg;
struct DSError	  *error;
struct getedb_result *result;
DN binddn;
int fd;
{
struct edbops_list * nextop, *trail = NULLEDBOP;
PE pe = NULLPE;

	/* step through the list to find oper... */
	for ( nextop = edbops; nextop != NULLEDBOP; 
	     		trail = nextop, nextop = nextop->next ) {

	        if ( nextop->edb_fd != fd )
		        continue;
		if ( nextop->edb_next != arg->ga_nextEntryPos )
			continue;
		if ( dn_cmp (nextop->bind_name, binddn) != 0)
			continue;
		if ( dn_cmp (nextop->edb_name, arg->ga_entry) != 0)
			continue;

/* Should be different first, time, but what about second time !?! 
		if ( lexequ (nextop->edb_version, arg->ga_version) != 0)
			continue;
*/

		break;
	}

	if ( nextop == NULLEDBOP ) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("Lost EDB operation"));
out:;
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNAVAILABLE;
		return (DS_X500_ERROR);
	}

	switch (read_part_edb (nextop->edb_ps,&pe,arg->ga_maxEntries)) {
	    case NOTOK:
		if (pe)
			pe_free (pe);
		goto out;
	    case DONE:
		break;
	    case OK:
		result->gr_version = strdup(nextop->edb_version);
		result->gr_encoded = TRUE;     
		result->gr_pe = pe;
		result->gr_nextEntryPos = arg->ga_nextEntryPos + 
					  arg->ga_maxEntries;
		nextop->edb_next = result->gr_nextEntryPos;
		return DS_OK;
	}
					
	/* last one, send result and clean up */

	result->gr_version = strdup(nextop->edb_version);
	result->gr_encoded = TRUE;     
	result->gr_pe = pe;
	result->gr_nextEntryPos = 0;

	(void) fclose ((FILE *)nextop->edb_ps->ps_addr);
	ps_free (nextop -> edb_ps);
	dn_free (nextop -> edb_name);
	dn_free (nextop -> bind_name);

	(void) unlink (nextop -> edb_fname);
	free (nextop -> edb_fname);
	free (nextop -> edb_version);

	if (trail == NULLEDBOP)
		edbops = nextop -> next;
	else 
		trail -> next = nextop -> next;

	free ((char *) nextop);

	return DS_OK;
	/* send next part of an EDB file */
}

static get_more_edb (oper,newop)
struct oper_act	* oper;
struct oper_act	** newop;
{
struct getedb_result	* result = &(oper->on_resp.di_result.dr_res.dcr_dsres.res_ge);
struct getedb_arg	* arg = &(oper->on_req.dca_dsarg.arg_ge);
struct getedb_arg	* narg;
struct oper_act	* on_tmp;
struct di_block * di_alloc ();

	/* schedule getting the next part of the EDB */

	if((on_tmp = oper_alloc()) == NULLOPER)
	{
		LLOG(log_dsap, LLOG_EXCEPTIONS, 
		     ("make_get_edb_op - out of memory"));
		return;
	}

	on_tmp->on_type = ON_TYPE_GET_EDB;
	on_tmp->on_req.dca_dsarg.arg_type = OP_GETEDB;
	on_tmp->on_getedb_ver = arg->ga_version;

	narg = &(on_tmp->on_req.dca_dsarg.arg_ge);

	narg->ga_entry = dn_cpy(arg->ga_entry);
	narg->ga_version = strdup(arg->ga_version);
	narg->ga_nextEntryPos = result->gr_nextEntryPos;
	narg->ga_type = GA_CONTINUE;

	on_tmp->on_conn = oper->on_conn;

	on_tmp->on_next_conn = oper->on_conn->cn_operlist;	
        oper->on_conn -> cn_operlist = on_tmp;
	on_tmp->on_relay = FALSE;

	narg->ga_maxEntries = split_size;

	/* save previous results */
	
	narg->ga_previous = (struct getedb_result *) smalloc 
					(sizeof (struct getedb_result));

	narg->ga_previous->gr_encoded = TRUE;
	narg->ga_previous->gr_pe = result->gr_pe;
	narg->ga_previous->gr_next = arg->ga_previous;

	*newop = on_tmp;
}

static pull_up_result (arg,result)
struct getedb_arg * arg;
struct getedb_result *result;
{
struct getedb_result *loop;
struct getedb_result *ln;
PE pe, npe, spe, lpe, zpe;	/* what useful names ! */

	/* TODO - freeing pe's on error conditions */
	
	pe = result -> gr_pe;

	if (loop = arg->ga_previous) /* ASSIGN */ {

	    if ((spe = prim2seq (pe)) == NULLPE) {
		    LLOG ( log_dsap,LLOG_EXCEPTIONS,
			  ("First EDB bit is not a seq !?!"));
		    pe_free(pe);
		    return FALSE;
	    }

	    if ((npe = first_member (spe)) == NULLPE) {
		    /* last part of EDB is empty - start with next one */
		    pe_free (pe);
		    pe = result -> gr_pe = loop -> gr_pe;

		    loop = loop -> gr_next;

		    if (loop) {
			if (loop -> gr_version)
			    free (loop -> gr_version);
			loop -> gr_version = NULLCP;
		    }

		    if ((spe = prim2seq (pe)) == NULLPE) {
			LLOG ( log_dsap,LLOG_EXCEPTIONS,
			      ("first PE EDB bit is not a seq !?!"));
			pe_free(pe);
			return FALSE;
		    }
		    if ((npe = first_member (spe)) == NULLPE) {
			    /* can this happen - what a mess ! */
			LLOG ( log_dsap,LLOG_EXCEPTIONS,
			      ("first PE EDB bit is also null !?!"));
			pe_free(pe);
			return FALSE;
		    }
	    }

	    for ( ; npe ; npe = next_member (spe, npe) ) 
		    lpe = npe;

	    for ( ; loop != NULL_GETRESULT; loop = ln ) {

		ln = loop -> gr_next;

		if ((spe = prim2seq (loop->gr_pe)) == NULLPE) {
			LLOG ( log_dsap,LLOG_EXCEPTIONS,
			      ("next PE EDB bit is not a seq !?!"));
			pe_free(pe);
			return FALSE;
		}
		loop->gr_pe = NULLPE;

		/* loop round this PE, adding each element to result PE */
		for (npe = first_member (spe); npe; npe = zpe) {
			zpe = next_member (spe, npe);
			npe -> pe_next = NULLPE;

			if (seq_addon (pe, lpe, npe) == NOTOK) {
				LLOG ( log_dsap,LLOG_EXCEPTIONS, 
				      ("EDB/PE seq_addon failed"));
				pe_free(pe);
				return FALSE;
			}

			lpe = npe;
		}

		spe->pe_cons = NULLPE;
		spe->pe_next = NULLPE;
		pe_free (spe);
		free ((char *) loop);
	    }
	}

	DATABASE_HEAP;
	
	if (EDB_decode_force (&result,pe) == NOTOK) {
		GENERAL_HEAP;
		pe_free(pe);
		return FALSE;
	}
	GENERAL_HEAP;

	pe_free(pe);

	(void) dsa_wait (0);	/* accept any results of previous ops */

	return TRUE;
}

static read_part_edb (ps,pep,n)
PS ps;
PE * pep;
int n;
{
int i;
PE pe;
PE lpe = NULLPE;

	/* return OK    == got n */
	/* return DONE  == got <= n, and EOF */
	/* return NOTOK == failed in some way */

	if (((*pep) = pe_alloc (PE_CLASS_UNIV, PE_FORM_CONS, PE_CONS_SEQ)) == NULLPE) {
	    LLOG ( log_dsap,LLOG_EXCEPTIONS, ("EDB/PE alloc failed"));
	    return NOTOK;
	}

	for (i=0 ; i<n ; i++) {
		if ((pe = ps2pe (ps)) == NULLPE) {
			if ((ps -> ps_errno == PS_ERR_EOF) ||
			    (ps -> ps_errno == 0))
				return DONE;
			
			pe_free (*pep);
			LLOG ( log_dsap,LLOG_EXCEPTIONS, 
			      ("EDB/PE read failed (%s)",
			       ps_error(ps->ps_errno)));
			return NOTOK;
		
		}
		if (seq_addon (*pep, lpe, pe) == NOTOK) {
			LLOG ( log_dsap,LLOG_EXCEPTIONS, ("EDB/PE seq_add failed"));
			pe_free (*pep);
			return NOTOK;
		}
		lpe = pe;
	}

	return OK;
}


set_edb_limit (oper)
struct oper_act	* oper;
{
	oper->on_req.dca_dsarg.arg_ge.ga_maxEntries = split_size;
}

getedb_size (x)
int x;
{
	split_size = x;
}
