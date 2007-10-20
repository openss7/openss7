/* shadow.c - spot shadowing of entries */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/shadow.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/shadow.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: shadow.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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
#include "quipu/read.h"
#include "quipu/dua.h"
#include "quipu/connection.h"
#include "quipu/malloc.h"

#ifndef NO_STATS
extern LLog * log_stat;
#endif
extern LLog * log_dsap;
static struct dn_seq * shades = NULLDNSEQ;
static struct dn_seq * dn_shades = NULLDNSEQ;
extern struct dn_seq * dn_seq_push();
extern struct di_block * di_alloc();
extern struct oper_act * oper_alloc();
extern DN mydsadn;
extern LLog * log_dsap;
extern Entry entry_cpy ();
extern Entry database_root;
extern char * new_version ();
extern Attr_Sequence as_comp_cpy ();
extern Attr_Sequence entry_find_type ();
extern struct access_point * ap_cpy();
extern time_t timenow;
extern time_t	conn_timeout;
extern time_t	nsap_timeout;
extern Entry make_path();
extern Entry local_find_entry_aux();
extern struct oper_act * pending_ops;

extern AttributeType at_subord;
extern AttributeType at_xref;
extern AttributeType at_nssr;
extern AttributeType at_objectclass;
extern short syntax_dn;

typedef struct _atlist {
	AttributeType  at;
	struct _atlist *next;
} *atlist;
#define NULLATL (atlist)NULL
atlist at_list = NULLATL;

shadow_entry (eptr)
Entry eptr;
{
DN dn, ndn;
Attr_Sequence as;
AV_Sequence avs;
atlist at;

	/* All MASTER entries get passed through here. */
	/* See if it need shadowing, if so, add to shades */

   if (eptr->e_data == E_DATA_MASTER) {	
	if ((eptr->e_dsainfo) &&
	    (quipu_ctx_supported (eptr) > 2) &&
	    (quipu_version_7 (eptr))) {
		/* Its a version 7 quipuDSA */

	    dn = get_copy_dn (eptr);

	    if (dn_cmp (dn, mydsadn) == 0) {
		    dn_free (dn);
		    return;  /* can't shadow myself ! */
	    }
	
	    if ( check_dnseq (shades, dn) == NOTOK)
		    shades = dn_seq_push (dn,shades);

	    dn_free (dn);

	} else if ((eptr->e_external) && (eptr->e_reftype != RT_NONSPECIFICSUBORDINATE)) {
		
	    dn = get_copy_dn (eptr);

	    if ( check_dnseq (shades, dn) == NOTOK)
		    shades = dn_seq_push (dn,shades);

	    dn_free (dn);
	} 
    }

    for (as = eptr->e_attributes; as != NULLATTR; as=as->attr_link)
       if (as->attr_type->oa_syntax == syntax_dn)
	  for (at = at_list; at != NULLATL; at=at->next)
	     if (as->attr_type == at->at) {
		for (avs = as->attr_value; avs != NULLAV; avs = avs->avseq_next) {
		   ndn = (DN)avs->avseq_av.av_struct;
		   if (check_dnseq (dn_shades, ndn) == NOTOK)
			dn_shades = dn_seq_push (ndn,dn_shades);
	        }
	     }
}

shadow_myentry ()
{
	    if ( check_dnseq (shades, mydsadn) == NOTOK)
		    shades = dn_seq_push (mydsadn,shades);
}

#ifdef DEBUG
free_shadow_lists()
{
    if (dn_shades)
	dn_seq_free (dn_shades);	
    if (shades)
	dn_seq_free (shades);	
}
#endif

shadow_attribute (s)
char * s;
{
AttributeType at;
atlist new_atl;

	if (( at = AttrT_new (s)) == NULLAttrT)
	      LLOG (log_dsap, LLOG_EXCEPTIONS, ("Unknown shadow attr %s",s));
	else {
	      new_atl = (atlist) smalloc (sizeof (*new_atl));
	      new_atl->at = at;
	      new_atl->next = at_list;
	      at_list = new_atl;
	}
}

shadow_update ()
{
struct dn_seq * dnseq;
struct oper_act	* op;
static struct ds_read_arg sarg =
{
	default_common_args,
	NULLDN,
	{       /* entry info selection */
		TRUE,
		NULLATTR,
		EIS_ATTRIBUTESANDVALUES
	}
};
Entry eptr;
DN tdn;
struct access_point * aps;
struct DSError err;

	DLOG(log_dsap, LLOG_TRACE, ("shadow_update"));

	for (dnseq = dn_shades; dnseq != NULLDNSEQ; dnseq = dnseq -> dns_next) {
		if ((eptr = local_find_entry (dnseq -> dns_dn,FALSE)) == NULLENTRY) {
			/* aliases !!! */
			if ((eptr = local_find_entry (dnseq -> dns_dn,TRUE)) == NULLENTRY)
				if ((eptr = make_path (dnseq -> dns_dn)) == 
				    NULLENTRY) {
					pslog (log_dsap,LLOG_EXCEPTIONS,
			       "Shadowing entry which does not exist !!!",
					       (IFP)dn_print,
					       (caddr_t) dnseq -> dns_dn);
					continue;
				}

			else if ( eptr -> e_alias )
				if ((eptr = make_path (eptr -> e_alias)) == 
				    NULLENTRY) {
					pslog (log_dsap,LLOG_EXCEPTIONS,
				"Shadowing alias which does not exist !!!",
					       (IFP)dn_print,
					       (caddr_t) eptr -> e_alias);
					continue;
				}
		}

		else if ((eptr->e_data == E_TYPE_SLAVE) || 
			   (eptr->e_data == E_DATA_MASTER)) 
			continue;

		if ( check_dnseq (shades, dnseq -> dns_dn) == NOTOK)
			shades = dn_seq_push (dnseq -> dns_dn,shades);
	}

	dn_seq_free (dn_shades);	
	dn_shades = NULLDNSEQ;

	for (dnseq = shades; dnseq != NULLDNSEQ; dnseq = dnseq -> dns_next) {

		if((op = oper_alloc()) == NULLOPER)
			return;

		op -> on_type = ON_TYPE_SHADOW;
		op -> on_req.dca_dsarg.arg_type = OP_READ;

		sarg.rda_common.ca_servicecontrol.svc_options |= 
			SVC_OPT_DONTDEREFERENCEALIAS;
			/* would loose track on result - could do more tho! */

		op -> on_req.dca_dsarg.arg_rd = sarg;	  /* struct copy */

		op -> on_req.dca_dsarg.arg_rd.rda_object = 
			dn_cpy (dnseq -> dns_dn);

		op -> on_req.dca_charg.cha_originator = 
			dn_cpy (mydsadn);

		op -> on_req.dca_charg.cha_reftype = RT_SUBORDINATE;

		op -> on_req.dca_charg.cha_progress.op_resolution_phase = 
			OP_PHASE_PROCEEDING;

		op -> on_req.dca_charg.cha_progress.op_nextrdntoberesolved = -1;
		for (tdn = dnseq -> dns_dn ; tdn != NULLDN ; 
		     			     tdn = tdn -> dn_parent)
			op -> 
			on_req.dca_charg.cha_progress.op_nextrdntoberesolved++;

		op -> on_req.dca_charg.cha_trace = 
			(struct trace_info *) malloc (sizeof (struct trace_info));
		op -> on_req.dca_charg.cha_trace -> ti_dsa = 
			dn_cpy (mydsadn);
		op -> on_req.dca_charg.cha_trace -> ti_target = 
			dn_cpy (dnseq -> dns_dn);
		op -> on_req.dca_charg.cha_trace -> ti_progress = 
			op -> on_req.dca_charg.cha_progress;
		op -> on_req.dca_charg.cha_trace -> ti_next = NULLTRACEINFO;

		op -> on_dsas = NULL_DI_BLOCK;

		if ((eptr = local_find_entry_aux (dnseq -> dns_dn,FALSE)) == NULLENTRY) {
		    if ((eptr = local_find_entry_aux (dnseq -> dns_dn,TRUE)) == NULLENTRY) 
			if ((eptr = make_path (dnseq -> dns_dn)) == NULLENTRY) {
			  pslog (log_dsap,LLOG_EXCEPTIONS,
			       "local shadow entry failure",
			       (IFP)dn_print, (caddr_t) dnseq -> dns_dn);
			  oper_free (op);
			  continue;
		    }
		} else if ( eptr -> e_external ) {
			op -> on_dsas = di_alloc();
			op -> on_dsas -> di_type = DI_TASK;

			op -> on_dsas -> di_rdn_resolved = op ->
				on_req.dca_charg.cha_progress.op_nextrdntoberesolved;
			op -> on_dsas -> di_aliasedRDNs = CR_NOALIASEDRDNS;

			op -> on_dsas -> di_oper = op;
			op -> on_dsas -> di_type = DI_OPERATION;

			op -> on_dsas -> di_target = dn_cpy (dnseq -> dns_dn);
			op -> on_dsas -> di_reftype = eptr-> e_reftype;
			aps = ap_cpy ((struct access_point *) eptr ->
				       e_reference -> avseq_av.av_struct);
			op -> on_dsas -> di_dn = dn_cpy (aps->ap_name);
			op -> on_dsas -> di_accesspoints = aps;
			op -> on_dsas -> di_state = DI_ACCESSPOINT;

		} else if ((eptr->e_data == E_TYPE_SLAVE) || 
			   (eptr->e_data == E_DATA_MASTER)) {
			op -> on_dsas = di_alloc();
			op -> on_dsas -> di_type = DI_TASK;

			op -> on_dsas -> di_rdn_resolved = op ->
				on_req.dca_charg.cha_progress.op_nextrdntoberesolved;
			op -> on_dsas -> di_aliasedRDNs = CR_NOALIASEDRDNS;

			op -> on_dsas -> di_oper = op;
			op -> on_dsas -> di_type = DI_OPERATION;
			op -> on_dsas -> di_dn = dn_cpy (dnseq -> dns_dn);
			op -> on_dsas -> di_target = dn_cpy (dnseq -> dns_dn);
			op -> on_dsas -> di_reftype = RT_SUBORDINATE;
			op -> on_dsas -> di_entry = eptr;
			eptr -> e_refcount++;
			op -> on_dsas -> di_state = DI_COMPLETE;
		} else 
		        (void) constructor_dsa_info (dnseq -> dns_dn, NULLDNSEQ,
				    TRUE, eptr, &err, &(op -> on_dsas) );

		if ( op -> on_dsas )
			schedule_operation (op);
		else 
			oper_free (op);
	}
}


shadow_fail_wakeup (on)
struct oper_act * on;
{
#ifdef notanymore
    struct oper_act	* on_tmp;
    struct oper_act	**on_p;
#endif

    DLOG (log_dsap, LLOG_TRACE, ("Shadow fail wakeup"));

    if (on -> on_resp.di_type == DI_ERROR) {

	    if (on->on_resp.di_error.de_err.dse_type == DSE_DSAREFERRAL)
		if (oper_rechain(on) == OK)
			return FALSE;

	    pslog (log_dsap,LLOG_EXCEPTIONS,"Remote shadow error",
		   (IFP)dn_print,
		   (caddr_t) on -> on_req.dca_dsarg.arg_rd.rda_object);
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
	     ("subtask_fail_wakeup - op escaped from get_edb_ops (the global list)"));
    }

	/* Arrange for connection to shut in 30 seconds unless
	   used by something else in that time
	   Better than leaving it for 5 minutes
        */

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

    return TRUE;
}

/* ARGSUSED */
inherit_link(e, parent)
Entry   e;
Entry   parent;
{
	set_inheritance (e);
        return(OK);
}

process_shadow (on)
struct oper_act * on;
{
Entry eptr, ne = NULLENTRY;
struct DSError err;
Attr_Sequence new_as, as, tas;
DN dn;

	DLOG (log_dsap, LLOG_TRACE, ("Process shadow"));

	dn = on -> on_resp.di_result.dr_res.dcr_dsres.res_rd.rdr_entry.ent_dn;
	
	if ((eptr = local_find_entry_aux (dn,FALSE)) == NULLENTRY) 
		/* aliases on route !!! */
		if ((eptr = local_find_entry_aux (dn,TRUE)) == NULLENTRY) {
			pslog (log_dsap,LLOG_EXCEPTIONS,"Shadow has gone",
			       (IFP)dn_print, (caddr_t) dn);
			goto out;
		}

	new_as = on -> 
		 on_resp.di_result.dr_res.dcr_dsres.res_rd.rdr_entry.ent_attr;

	if (eptr -> e_external) {
		/* Add in Quipu attributes */
		
		if (  ((as = entry_find_type (eptr, at_subord)) == NULLATTR) 
	           && ((as = entry_find_type (eptr, at_xref)) != NULLATTR)
	           && ((as = entry_find_type (eptr, at_nssr)) != NULLATTR)) {
			LLOG (log_dsap, LLOG_EXCEPTIONS, (
				        "external reference missing"));
			goto out;
		}
		new_as = as_merge (new_as, as_comp_cpy (as));

		if ((as = as_find_type (new_as, at_objectclass)) == NULLATTR) {
			LLOG (log_dsap, LLOG_EXCEPTIONS, (
				        "no objectclass in shadow entry"));
			on -> on_resp.di_result.dr_res.
				dcr_dsres.res_rd.rdr_entry.ent_attr = new_as;
			goto out;
		}

		tas = as_comp_new (AttrT_cpy(at_objectclass),
			  str2avs(EXTERNOBJECT,at_objectclass),NULLACL_INFO);
		new_as = as_merge (new_as,tas);

		on -> on_resp.di_result.dr_res.
			dcr_dsres.res_rd.rdr_entry.ent_attr = new_as;
		
	}

	if (as_cmp (eptr->e_attributes, new_as)	== 0) {
		DLOG (log_dsap, LLOG_TRACE, ("Shadow: no change"));
		eptr->e_age = timenow;
		goto out;
	}


	DATABASE_HEAP;
	ne = entry_cpy (eptr);
	GENERAL_HEAP;

	as_free ( ne -> e_attributes );
	ne -> e_attributes = as_cpy ( new_as );

	if (ne -> e_data == E_TYPE_CONSTRUCTOR) {
		ne -> e_data = E_TYPE_CACHE_FROM_MASTER;
		new_cacheEDB (dn);
	}


                if (unravel_attribute(ne, &err) != OK) {
			pslog (log_dsap,LLOG_EXCEPTIONS,"shadow: unravel failure",
			       (IFP)dn_print, (caddr_t) dn);
			log_ds_error (&err);
			ds_error_free (&err);
			entry_free (ne);	
			ne = NULLENTRY;
			goto out;
		} else 	if ( ! check_oc_hierarchy(ne->e_oc)) {
			pslog (log_dsap,LLOG_EXCEPTIONS,"shadow: objectclass failure",
			       (IFP)dn_print, (caddr_t) dn);
			entry_free (ne);
			ne = NULLENTRY;
			goto out;

		} else if (check_schema (ne,NULLATTR,&err) != OK) {
			pslog (log_dsap,LLOG_EXCEPTIONS,"shadow: schema failure",
			       (IFP)dn_print, (caddr_t) dn);
			log_ds_error (&err);
			ds_error_free (&err);
			entry_free (ne);
			ne = NULLENTRY;
			goto out;
		}

#ifdef TURBO_INDEX
                turbo_index_delete(eptr);
#endif

                if (ne->e_parent == NULLENTRY) {
                        entry_replace(database_root, ne);
                } else {
                        entry_replace(eptr, ne);
                }

		entry_free (ne);
		ne = eptr;

                if (unravel_attribute(eptr, &err) != OK) {
			pslog (log_dsap,LLOG_EXCEPTIONS,"shadow: 2nd unravel failure",
			       (IFP)dn_print, (caddr_t) dn);
			log_ds_error (&err);
			ds_error_free (&err);
			goto out;
		}
		(void) avl_apply(eptr->e_children, inherit_link, 
				 (caddr_t) eptr, NOTOK, AVL_PREORDER);

		if (eptr->e_parent->e_edbversion)
			free (eptr->e_parent->e_edbversion);
		eptr->e_parent->e_edbversion = new_version();

#ifdef TURBO_INDEX
		/* add the new modified entry to the index */
		turbo_add2index(eptr);
#endif

#ifdef TURBO_DISK
	if (turbo_write(ne) != OK)
		fatal (-33,"shadow rewrite failed - check database");
#else
	if (journal (ne) != OK)
		fatal (-33,"shadow rewrite failed - check database");
#endif
#ifndef NO_STATS
	pslog (log_stat,LLOG_TRACE,"Shadow update",(IFP)dn_print, (caddr_t) on ->
	       on_resp.di_result.dr_res.dcr_dsres.res_rd.rdr_entry.ent_dn);
#endif

out:;

	(void) time (&timenow);
	if (ne)
		ne->e_age = timenow;

	if (on->on_conn) 
	    on->on_conn->cn_last_used = timenow - conn_timeout + nsap_timeout;

}


