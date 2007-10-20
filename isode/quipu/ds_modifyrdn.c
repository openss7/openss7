/* ds_modifyrdn.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/ds_modifyrdn.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/ds_modifyrdn.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: ds_modifyrdn.c,v $
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
#include "quipu/entry.h"
#include "quipu/modifyrdn.h"
#include "quipu/malloc.h"
#include "quipu/turbo.h"
extern int entry_cmp(), entryrdn_cmp();
#include "pepsy.h"
#include "quipu/DAS-types.h"
#include "quipu/connection.h"

extern LLog * log_dsap;
extern DN mydsadn;

do_ds_modifyrdn (arg, error, binddn, target, di_p, dsp, authtype)
    register struct ds_modifyrdn_arg     *arg;
    struct DSError              *error;
    DN                          binddn;
    DN                          target;
    struct di_block		**di_p;
    char 			dsp;
    char			authtype;
{
Entry  entryptr;
register RDN rdn;
Attr_Sequence as;
AV_Sequence avs;
RDN modrdn;
char * new_version ();
int retval;
int authp, pauthp;
extern int read_only;

	DLOG (log_dsap,LLOG_TRACE,("ds_modifyrdn"));

	if (!dsp)
		target = arg->mra_object;

	/* stop aliases being dereferenced */
	arg->mra_common.ca_servicecontrol.svc_options |= SVC_OPT_DONTDEREFERENCEALIAS;

	if (target == NULLDN) {
		error->dse_type = DSE_NAMEERROR;
		error->ERR_NAME.DSE_na_problem = DSE_NA_NOSUCHOBJECT;
		error->ERR_NAME.DSE_na_matched = NULLDN;
		return (DS_ERROR_REMOTE);
	}

	switch(find_entry(target,&(arg->mra_common),binddn,NULLDNSEQ,TRUE,&(entryptr), error, di_p, OP_MODIFYRDN))
	{
	case DS_OK:
	    /* Filled out entryptr - carry on */
	    break;
	case DS_CONTINUE:
	    /* Filled out di_p - what do we do with it ?? */
	    return(DS_CONTINUE);

	case DS_X500_ERROR:
	    /* Filled out error - what do we do with it ?? */
	    return(DS_X500_ERROR);
	default:
	    /* SCREAM */
	    LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_modifyrdn() - find_entry failed"));
	    return(DS_ERROR_LOCAL);
	}

	if (read_only || entryptr->e_parent->e_lock) {
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNAVAILABLE;
		return (DS_ERROR_REMOTE);
	}

	if (dn_cmp(mydsadn,target) == 0) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("ModifyRDN not allowed on my DSA entry"));
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
		return (DS_ERROR_REMOTE);
	}

	/* Strong authentication  */
	if ((retval = check_security_parms((caddr_t) arg,
			_ZModifyRDNArgumentDataDAS,
			&_ZDAS_mod,
			arg->mra_common.ca_security,
			arg->mra_common.ca_sig, &binddn)) != 0)
	{
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = retval;
		return (DS_ERROR_REMOTE);
	}

	/* not prepared to accept operation over DSP */
	if (dsp) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_AUTHENTICATION;
		return (DS_ERROR_REMOTE);
	}

	if (!manager(binddn)) {
		pauthp = entryptr->e_parent->e_authp ?
		    entryptr->e_parent->e_authp->ap_modification :
		    AP_SIMPLE;
		authp = entryptr->e_authp ? entryptr->e_authp->ap_modification :
		    AP_SIMPLE;
	} else {
		/* manager -- fool us into accepting the name */
		pauthp = authp = AP_SIMPLE;
	}

	if ((check_acl ((authtype % 3) >= authp ? binddn : NULLDN, ACL_WRITE,
	    entryptr->e_acl->ac_entry, target) == NOTOK)
	    || ((entryptr->e_parent->e_data != E_TYPE_CONSTRUCTOR)
	    && (check_acl ((authtype % 3) >= pauthp ? binddn : NULLDN,
	    ACL_WRITE,entryptr->e_parent->e_acl->ac_child, target)
	    == NOTOK)) ) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_ACCESSRIGHTS;
		return (DS_ERROR_REMOTE);
	}
	if ( ! (isleaf(entryptr))) {
		error->dse_type = DSE_UPDATEERROR;
		error->ERR_UPDATE.DSE_up_problem = DSE_UP_NOTONNONLEAF;
		return (DS_ERROR_REMOTE);
	}

	/* make sure the new name doesn't already exist */
	if ( (Entry) avl_find( entryptr->e_parent->e_children,
	    (caddr_t) arg->mra_newrdn, entryrdn_cmp ) != NULLENTRY ) {
		error->dse_type = DSE_UPDATEERROR;
		error->ERR_UPDATE.DSE_up_problem = DSE_UP_ALREADYEXISTS;
		return( DS_ERROR_REMOTE );
	}

	/* first check that it is an allowed type */
	for (rdn=arg->mra_newrdn; rdn!=NULLRDN; rdn=rdn->rdn_next)
		if (check_schema_type (entryptr, rdn->rdn_at, error) == NOTOK)
			return (DS_ERROR_REMOTE);

	if (arg->deleterdn)
		for (rdn=entryptr->e_name; rdn!=NULLRDN; rdn=rdn->rdn_next)
			if (remove_attribute (entryptr, rdn->rdn_at, error, binddn, target, entryptr) != OK)
				return (DS_ERROR_REMOTE);


	/* must now add rdn as attribute */
	for (rdn=arg->mra_newrdn; rdn!=NULLRDN; rdn=rdn->rdn_next) {
		avs = avs_comp_new (AttrV_cpy(&rdn->rdn_av));
		as = as_comp_new (AttrT_cpy(rdn->rdn_at),avs, NULLACL_INFO);
		if (addrdn_attribute (entryptr,as,error,binddn,target) != OK)
			return (DS_ERROR_REMOTE);

	}


#ifdef TURBO_INDEX
	/* delete the old one from the index */
	turbo_index_delete(entryptr);
#endif

	/* delete the old one from core */
        if ((entryptr = (Entry) avl_delete( &entryptr->e_parent->e_children,
            (caddr_t) entryptr->e_name, entryrdn_cmp )) == NULLENTRY ) {
                LLOG(log_dsap, LLOG_EXCEPTIONS, ("modrdn: entry has disappeared!"));
                return( DS_ERROR_REMOTE );
        }

#ifdef TURBO_DISK
	/* delete the old one from disk */
	if (turbo_delete(entryptr) != OK)
		fatal (-34,"mod rdn delete failed - check database");
#endif

	modrdn = entryptr->e_name;
	DATABASE_HEAP;
	entryptr->e_name = rdn_cpy(arg->mra_newrdn);

	modify_attr (entryptr,binddn);
	if (unravel_attribute (entryptr,error) != OK) {
		GENERAL_HEAP;
		LLOG (log_dsap,LLOG_EXCEPTIONS,("modify rdn protocol error"));
		rdn_free (modrdn);
		return (DS_ERROR_REMOTE);
	} else {
		GENERAL_HEAP;
		if (entryptr->e_parent != NULLENTRY) {
			if (entryptr->e_parent->e_edbversion)
				free (entryptr->e_parent->e_edbversion);
			entryptr->e_parent->e_edbversion = new_version();
		}

		/* add the new one to core */
                if (avl_insert(&entryptr->e_parent->e_children, (caddr_t) entryptr,
                    entry_cmp, avl_dup_error) != OK) {
                        LLOG(log_dsap, LLOG_EXCEPTIONS, ("modrdn: can't add new entry!"));
                        return(DS_ERROR_REMOTE);
                }

#ifdef TURBO_INDEX
                turbo_add2index(entryptr);
#endif


#ifdef TURBO_DISK
		/* add the new one to disk */
		if (turbo_write(entryptr) != OK)
			fatal (-34,"mod rdn failed - check database");
#else
		if ((journal (entryptr)) != OK)
			fatal (-34,"mod rdn failed - check database");
#endif

		rdn_free (modrdn);
		return (DS_OK);
	}

}


addrdn_attribute (eptr,newas,error,requestor,dn)
Entry eptr;
Attr_Sequence newas;
struct DSError *error;
DN requestor,dn;
{
register Attr_Sequence as;
struct acl_info * acl;

	DLOG (log_dsap,LLOG_DEBUG,("add attribute"));

	if ( (as = as_find_type (eptr->e_attributes,newas->attr_type)) == NULLATTR)
		acl = eptr->e_acl->ac_default;
	else
		acl = as->attr_acl;

	if (check_acl(requestor,ACL_WRITE,acl,dn) == NOTOK) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_ACCESSRIGHTS;
		DLOG (log_dsap,LLOG_DEBUG,("add acl failed"));
		return (NOTOK);
	}

	eptr->e_attributes = as_merge (newas,eptr->e_attributes);
	return (OK);
}
