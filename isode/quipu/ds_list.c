/* ds_list.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/ds_list.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/ds_list.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: ds_list.c,v $
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
#include "quipu/connection.h"
#include "quipu/list.h"
#include "quipu/turbo.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

extern LLog * log_dsap;
extern Entry database_root;
static int build_result();

do_ds_list (arg, error, result, binddn, target, di_p, dsp, authtype)
    register struct ds_list_arg          *arg;
    register struct ds_list_result       *result;
    struct DSError                      *error;
    DN                                  binddn;
    DN                                  target;
    struct di_block			**di_p;
    char				dsp;
    char				authtype;
{
Entry  entryptr;
int retval;
DN realtarget;
int sizelimit;
int authp;

	DLOG (log_dsap,LLOG_TRACE,("ds_list"));

	if (!dsp)
		target = arg->lsa_object;

	switch(find_child_entry(target,&(arg->lsa_common),binddn,NULLDNSEQ,FALSE,&(entryptr),error,di_p))
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
	    LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_list() - find_child_entry failed"));
	    return(DS_ERROR_LOCAL);
	}

	/* Strong authentication  */
	if ((retval = check_security_parms((caddr_t) arg,
			_ZListArgumentDataDAS,
			&_ZDAS_mod,
			arg->lsa_common.ca_security,
			arg->lsa_common.ca_sig, &binddn)) != 0)
	{
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = retval;
		return (DS_ERROR_REMOTE);
	}

	realtarget = get_copy_dn(entryptr);

	if (isleaf(entryptr)) {

		dn_free (realtarget);

		result->lsr_subordinates = NULLSUBORD;
		result->lsr_age  =  (time_t) 0 ;
		result->lsr_common.cr_requestor = NULLDN;
		if ( error->dse_type == DSE_NOERROR ) {
			result->lsr_object = NULLDN;
			result->lsr_common.cr_aliasdereferenced = FALSE;
		} else {
			result->lsr_common.cr_aliasdereferenced = TRUE;
			result->lsr_object = get_copy_dn (entryptr->e_parent);
		}
		result->lsr_cr = NULLCONTINUATIONREF;
		result->lsr_limitproblem = LSR_NOLIMITPROBLEM;
		return (DS_OK);
	}

	/* do authentication policy stuff here */
	if (!manager(binddn))
		authp = entryptr->e_authp ?
		    entryptr->e_authp->ap_listandsearch : AP_SIMPLE;
	else
		authp = AP_SIMPLE;

	/* check parent will allow listing */
	sizelimit = SVC_NOSIZELIMIT;
	if ( entryptr->e_lacl != NULLAV ) {
		if ( check_lacl( (authtype % 3) >= authp ? binddn : NULLDN,
		    realtarget, entryptr->e_lacl, SACL_SINGLELEVEL,
		    &sizelimit ) == NOTOK ) {
			/* security error */
			error->dse_type = DSE_SECURITYERROR;
			error->ERR_SECURITY.DSE_sc_problem =
			    DSE_SC_ACCESSRIGHTS;
			dn_free( realtarget );
			return( DS_ERROR_REMOTE );
		}
	} 
	if (check_acl ((authtype % 3) >= authp ? binddn : NULLDN,
	    ACL_READ, entryptr->e_acl->ac_child, realtarget) != OK) {
		if (dsp && (check_acl (binddn,ACL_READ, entryptr->e_acl->ac_child, realtarget) == OK)) {
			error->dse_type = DSE_SECURITYERROR;
			error->ERR_SECURITY.DSE_sc_problem = DSE_SC_AUTHENTICATION;
			dn_free (realtarget);
			return (DS_ERROR_REMOTE);
		}
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_ACCESSRIGHTS;
		dn_free (realtarget);
		return (DS_ERROR_REMOTE);
	}

	if (entryptr->e_children == NULLAVL) {
		int res;
		if (try_cache (arg,result,realtarget) == OK) {
			dn_free (realtarget);
			return (DS_OK);
		}
		res = constructor_dsa_info(realtarget,NULLDNSEQ,FALSE,entryptr,error,di_p);
		dn_free (realtarget);
		return res;		
	}

	dn_free (realtarget);

	build_result (arg, entryptr, result, error,
	    (authtype % 3) >= authp ? binddn : NULLDN , dsp, sizelimit);
	return (DS_OK);
}

/*
 * these are globals changed by build_result and build_list.  they have
 * to be globals because build_list is called by avl_apply, and it only
 * allows one additional argument to be passed.  pretty gross, but that's
 * the way it is...
 */

static struct subordinate       *g_sub;
static struct subordinate       *g_trail = NULLSUBORD;
static int                      g_count;
static int			g_size;
static int			g_dsp;
static int			g_security;
static DN                       g_dn;
static DN                       g_dnend;

static int build_list(e, dn)
Entry   e;
DN      dn;
{
        struct subordinate      *sub;

        if (g_size != SVC_NOSIZELIMIT && g_count >= g_size)
                return(NOTOK);

        g_dnend->dn_rdn = e->e_name;

	if (g_dsp) {
	    /* Only send publicly read data over DSP */

	    if ( e->e_lacl != NULLAV ) {
		    int	dummy;

		    if ( check_lacl( NULLDN, g_dn, e->e_lacl, 
				    SACL_BASEOBJECT, &dummy ) != OK ) {

		         if ( check_lacl( dn, g_dn, e->e_lacl, 
				    SACL_BASEOBJECT, &dummy ) == OK ) 
			     /* Would get more over DAP */
			     g_security = 1;

			 return( 0 );
		    }
	    } else {
		if (check_acl(NULLDN, ACL_READ, e->e_acl->ac_entry, 
			      g_dn) != OK) {
		    if (check_acl(dn, ACL_READ, e->e_acl->ac_entry, 
				  g_dn) != OK)
			/* Would get more over DAP */
			g_security = 1;
		    return(0);
		    }
	    }

	} else {

	    if ( e->e_lacl != NULLAV ) {
		    int	dummy;
		    if ( check_lacl( dn, g_dn, e->e_lacl, SACL_BASEOBJECT,
			&dummy ) != OK ) 
			    return( 0 );

	    } else if (check_acl(dn, ACL_READ, e->e_acl->ac_entry, g_dn) != OK)
		    return(0);

	}
        sub = (struct subordinate *) smalloc(sizeof(struct subordinate));
        sub->sub_copy = e->e_data;
        sub->sub_rdn = rdn_cpy(e->e_name);
        sub->sub_aliasentry = (e->e_alias == NULLDN ? FALSE : TRUE);
        sub->sub_next = NULLSUBORD;

        if (g_sub == NULLSUBORD) {
                g_sub = sub;
                g_trail = sub;
        } else {
                g_trail->sub_next = sub;
                g_trail = sub;
        }

        g_count++;
        return(0);
}

static int build_result( arg, ptr, result, error, binddn, dsp, laclsizelimit )
register Entry ptr;
struct ds_list_arg    *arg;
struct ds_list_result *result;
struct DSError * error;
DN binddn;
char dsp;
int laclsizelimit;
{
DN dn;
DN dnend;
int size;
RDN dnrdn;
extern int admin_size;
char adminlimit = FALSE;
int rc;
Entry akid;

	DLOG (log_dsap,LLOG_DEBUG,("building list results"));

	result->lsr_subordinates = NULLSUBORD;
	if (!dsp && manager (binddn))
		size = arg->lsa_common.ca_servicecontrol.svc_sizelimit;
	else {
		if ( laclsizelimit == SVC_NOSIZELIMIT )
			laclsizelimit = admin_size;

		if ((size = MIN( laclsizelimit, MIN( admin_size,
		    arg->lsa_common.ca_servicecontrol.svc_sizelimit)))
		    == SVC_NOSIZELIMIT) {
			size = MIN( laclsizelimit, admin_size );
			adminlimit = TRUE;
		}
	}

	result->lsr_age  =  (time_t) 0 ;
	result->lsr_common.cr_requestor = NULLDN;
	/* if no error and NOT SVC_OPT_DONTDEREFERENCEALIASES then */
	/* the alias will have been derefeferenced -signified by   */
	/* NO_ERROR !!! */
	if ( error->dse_type == DSE_NOERROR ) {
		result->lsr_object = NULLDN;
		result->lsr_common.cr_aliasdereferenced = FALSE;
	} else {
		result->lsr_common.cr_aliasdereferenced = TRUE;
		result->lsr_object = get_copy_dn (ptr);
	}
	result->lsr_cr = NULLCONTINUATIONREF;

	/* we already checked for null kids ... */
	akid = (Entry) avl_getone(ptr->e_children);
	dn = get_copy_dn(akid);
	for (dnend = dn; dnend->dn_parent != NULLDN; dnend=dnend->dn_parent)
		;  /* NO-OP */
	dnrdn = dnend->dn_rdn;

	g_dn = dn;
	g_dsp = dsp;
	g_dnend = dnend;
	g_sub = NULLSUBORD;
	g_size = size;
	g_count = 0;
	g_security = 0;

        /*
         * preorder would be a little faster in case of small size limit,
         * but inorder is more user-predictable, which is nice, though not
         * required...
         */

	rc = avl_apply(ptr->e_children, build_list, (caddr_t) binddn, NOTOK,
	    AVL_INORDER);

        /*
         * build_list has updated g_count and g_sub to contain a count of
         * the number of entries in the list and the list itself,
         * respectively.  if avl_apply was not cut short because the size
         * limit was reached (i.e. instead it ran out of nodes), rc will
         * be NOTOK.
         */

	size = g_size;
	result->lsr_subordinates = g_sub;


	if ( rc != AVL_NOMORE )
		/* stopped look up due to size limit */
		/* need to send continuation reference */
		result->lsr_limitproblem = adminlimit ? 
			LSR_ADMINSIZEEXCEEDED : LSR_SIZELIMITEXCEEDED;
	else
		result->lsr_limitproblem = LSR_NOLIMITPROBLEM;

	if (g_security)
	        /* Could force a DAP operation - this is easier ! */
		result->lsr_limitproblem = LSR_ADMINSIZEEXCEEDED;

	dnend->dn_rdn = NULLRDN;
	dn_free (dn);
	rdn_free (dnrdn);
}


try_cache (arg,result,target)
    register struct ds_list_arg          *arg;
    register struct ds_list_result       *result;
    DN 					 target;
{
struct list_cache *ptr;
struct subordinate * subord_cpy();

	if ((arg->lsa_common.ca_servicecontrol.svc_options & SVC_OPT_DONTUSECOPY) == 0) {
		if ((ptr = find_list_cache (target,arg->lsa_common.ca_servicecontrol.svc_sizelimit)) != NULLCACHE) {
			DLOG (log_dsap,LLOG_DEBUG,("building list results using cache"));
			result->lsr_subordinates = subord_cpy(ptr->list_subs);
			result->lsr_age  =  (time_t) 0 ;
			result->lsr_common.cr_aliasdereferenced = FALSE;
			result->lsr_common.cr_requestor = NULLDN;
			result->lsr_object = NULLDN;
			result->lsr_cr = NULLCONTINUATIONREF;
			result->lsr_limitproblem = ptr->list_problem;
			return (OK);
		}
	}

	return (NOTOK);
}

