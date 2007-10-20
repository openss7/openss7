/* ds_bind.c - BindArgument Checking and Authentication */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/ds_bind.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/ds_bind.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: ds_bind.c,v $
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
#include "quipu/commonarg.h"
#include "quipu/bind.h"
#include "quipu/compare.h"
#include "quipu/dua.h"
#include "quipu/connection.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"
#include <sys/stat.h>

#include "osisec-stub.h"

extern LLog * log_dsap;
extern DN	mydsadn;
struct oper_act	* oper_alloc();
extern char quipu_shutdown;

static check_prefix_list ();
static check_dn_length ();

#ifndef NO_STATS
extern LLog * log_stat;
#endif

int bind_window = 300; /* Tailorable timeout for credentials */
int auth_bind = 0;

int	  ds_bind_init (cn)
struct connection	* cn;
{
    struct ds_bind_arg	* arg = &(cn->cn_start.cs_ds.ds_bind_arg);
    struct ds_bind_arg	* result = &(cn->cn_start.cs_res);
    struct ds_bind_error * error = &(cn->cn_start.cs_err);
    Attr_Sequence		  as;
    Entry			  entryptr;
    extern AttributeType	  at_password;
    extern AttributeType	  at_p_password;
    struct di_block		* dsas = NULL_DI_BLOCK;
    struct di_block		* di_tmp;
    struct oper_act		* on;
    struct ds_compare_arg	* cma;
    struct DSError		  err;
    static struct common_args	  ca_def = default_common_args;
    int 			  res;
    int				  retval;
    struct protected_password   * pp;
#ifndef NO_STATS
    char buff[LINESIZE];
#endif
    extern struct SecurityServices *dsap_security;

    DLOG (log_dsap,LLOG_TRACE,("ds_bind_init"));


    if ( (arg->dba_version != DBA_VERSION_V1988) || quipu_shutdown)
    {
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SERVICE;
	error->dbe_value = DSE_SV_UNAVAILABLE;
	return(DS_ERROR_CONNECT);
    }

/* We don't support any bilaterally-defined authentication procedures.
 * Hence, if we get EXTERNAL credentials in the bind, reject them.
 */

    if (arg->dba_auth_type == DBA_AUTH_EXTERNAL)
    {
	DLOG(log_dsap, LLOG_EXCEPTIONS, ("EXTERNAL found in credentials"));
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SERVICE;
	error->dbe_value = DSE_SV_UNAVAILABLE;
	return (DS_ERROR_CONNECT);
    }

/* If password is present, but zero length, treat as though absent */
    if ((arg->dba_auth_type == DBA_AUTH_SIMPLE) && (arg->dba_passwd_len == 0))
	arg->dba_auth_type = DBA_AUTH_NONE;


    switch (arg->dba_auth_type) {
	case DBA_AUTH_NONE:
	    if (((arg->dba_dn == NULLDN) && auth_bind == 1) || 
		(auth_bind > 1)) {
out:;
#ifndef NO_STATS
		    if (arg->dba_dn == NULLDN) 
			    LLOG(log_stat, LLOG_TRACE, ("Bind (%d) (rejected)", cn->cn_ad));
		    else {
			    (void) sprintf (buff,"Bind (%d) (rejected)",cn->cn_ad);
			    pslog (log_stat,LLOG_TRACE,buff,(IFP)dn_print, 
				   (caddr_t)arg->dba_dn);
		    }
#endif
		    error->dbe_version = DBA_VERSION_V1988;
		    error->dbe_type = DBE_TYPE_SECURITY;
		    error->dbe_value = DSE_SC_AUTHENTICATION;
		    return (DS_ERROR_CONNECT);
	    }
	    break;
	case DBA_AUTH_SIMPLE:
	    if (auth_bind > 2) goto out;
	    break;
	case DBA_AUTH_PROTECTED:
	    if (auth_bind > 3) goto out;
	    break;
	case DBA_AUTH_STRONG:
	    break;
	case DBA_AUTH_EXTERNAL:
	    goto out;
    }

    if (arg->dba_dn == NULLDN)
    {
#ifndef NO_STATS
	LLOG(log_stat, LLOG_NOTICE, ("Bind (%d) (anonymous)", cn->cn_ad));
#endif
	cn->cn_authen = DBA_AUTH_NONE;
	make_dsa_bind_arg(result);
	return(DS_OK);
    }

/* Now we're sure dba_dn contains a valid pointer, can decode it */

    if ( ! check_prefix_list (arg->dba_dn)) {
#ifndef NO_STATS
	    (void) sprintf (buff,"Bind (%d) (reject - prefix)",cn->cn_ad);
	    pslog (log_stat,LLOG_TRACE,buff,(IFP)dn_print,
		   (caddr_t)arg->dba_dn);
#endif
	    error->dbe_version = DBA_VERSION_V1988;
	    error->dbe_type = DBE_TYPE_SECURITY;
	    error->dbe_value = DSE_SC_ACCESSRIGHTS;
	    return (DS_ERROR_CONNECT);
    }

    if ((cn->cn_ctx == DS_CTX_X500_DAP) && !(check_dn_length(arg->dba_dn))) {
#ifndef NO_STATS
	    (void) sprintf (buff,"Bind (%d) (reject - DAP length)",cn->cn_ad);
	    pslog (log_stat,LLOG_TRACE,buff,(IFP)dn_print,
		   (caddr_t)arg->dba_dn);
#endif
	    error->dbe_version = DBA_VERSION_V1988;
	    error->dbe_type = DBE_TYPE_SECURITY;
	    error->dbe_value = DSE_SC_ACCESSRIGHTS;
	    return (DS_ERROR_CONNECT);
    }

    switch (arg->dba_auth_type)
    {
    case DBA_AUTH_NONE:
	/* partially check DN - i.e see if we can say if DEFINATELY does */
	/* not exist.  If it possibly exists - allow bind, checking it   */
	/* runs the risk of livelock */

	switch (res = really_find_entry(arg->dba_dn, TRUE, NULLDNSEQ, 
		FALSE, &(entryptr), &(err), &(dsas))) {
	    case DS_X500_ERROR:
		if ((err.dse_type == DSE_NAMEERROR) && 
		    (err.ERR_NAME.DSE_na_problem == DSE_NA_NOSUCHOBJECT)) {
			ds_error_free(&(err));
#ifndef NO_STATS
	(void) sprintf (buff,"Bind (%d) (no auth - rejected)",cn->cn_ad);
	pslog (log_stat,LLOG_TRACE,buff,(IFP)dn_print,(caddr_t)arg->dba_dn);
#endif
			error->dbe_version = DBA_VERSION_V1988;
			error->dbe_type = DBE_TYPE_SECURITY;
			error->dbe_value = DSE_SC_INVALIDCREDENTIALS;
			return (DS_ERROR_CONNECT);
		}
		/* fall */
	    default:
#ifndef NO_STATS
	(void) sprintf (buff,"Bind (%d) (no auth)",cn->cn_ad);
	pslog (log_stat,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)arg->dba_dn);
#endif
		if (dsas != NULL_DI_BLOCK)
			di_desist (dsas);
		cn->cn_authen = DBA_AUTH_NONE;
		make_dsa_bind_arg(result);
		return (DS_OK);
	}
		
    case DBA_AUTH_SIMPLE:
#ifndef NO_STATS
	(void) sprintf (buff,"Bind (%d) (simple)",cn->cn_ad);
	pslog (log_stat,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)arg->dba_dn);
#endif
	/* Can't check simple credentials from DSP (livelock risk).
	 * Hence treat DSP accesses as unauthenticated.
	 */
	if (cn->cn_ctx != DS_CTX_X500_DAP)
	{
	    cn->cn_authen = DBA_AUTH_NONE;
	    make_dsa_bind_arg(result);
	    return(DS_OK);
	}
	break;
    case DBA_AUTH_PROTECTED:
#ifndef NO_STATS
	(void) sprintf (buff,"Bind (%d) (protected)",cn->cn_ad);
	pslog (log_stat,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)arg->dba_dn);
#endif
	if (cn->cn_ctx != DS_CTX_X500_DAP)
	{
	    cn->cn_authen = DBA_AUTH_NONE;
	    make_dsa_bind_arg(result);
	    return(DS_OK);
	}
	else
	{
	    UTC ut;
	    long c_time, s_time, delta;

	    (void) time(&s_time);
	    ut = str2utct(arg->dba_time1, strlen(arg->dba_time1));
	    if (ut == NULLUTC)
		c_time = 0L; /* 1970 is a convenient out-of-date timestamp */
	    else
		c_time = gtime(ut2tm(ut));
	    delta = s_time - c_time;
	    if ((delta < 0) || (delta > bind_window))
	    {
		DLOG(log_dsap, LLOG_EXCEPTIONS, 
			("Time = %s, Delay = %D s : Association rejected", 
			arg->dba_time1, delta));
		error->dbe_version = DBA_VERSION_V1988;
		error->dbe_type = DBE_TYPE_SECURITY;
		error->dbe_value = DSE_SC_INVALIDCREDENTIALS;
		return (DS_ERROR_CONNECT);
	    }
	    pp = (struct protected_password *) calloc(1, sizeof(*pp));
	    /* Ought to check for null pointer ... */
	    pp->passwd = malloc((unsigned)arg->dba_passwd_len);
	    bcopy(arg->dba_passwd, pp->passwd, arg->dba_passwd_len);
	    pp->n_octets = arg->dba_passwd_len;
	    pp->time1 = strdup(arg->dba_time1);
	    pp->is_protected[0] = (char) 1;
	}
	break;
    case DBA_AUTH_STRONG:
#ifndef NO_STATS
	(void) sprintf (buff,"Bind (%d) (strong)",cn->cn_ad);
	pslog (log_stat,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)arg->dba_dn);
#endif
	/* Strong authentication is not yet supported.
	 * It will eventually be possible to check strong credentials over DSP.
	 * For the moment, accept them and treat as NONE over DSP, but reject
	 * over DAP.
	 */
	if (dsap_security && dsap_security->serv_ckpath && 
		dsap_security->serv_cknonce)
	{
	int rc;
	DN real_name;
	struct Nonce nonce;
	
	    nonce.non_time1 = arg->dba_time1;
	    nonce.non_time2 = arg->dba_time2;
	    nonce.non_r1.n_bits = arg->dba_r1.n_bits;
	    nonce.non_r1.value = arg->dba_r1.value;
	    nonce.non_r2.n_bits = arg->dba_r2.n_bits;
	    nonce.non_r2.value = arg->dba_r2.value;

	    rc = (dsap_security->serv_cknonce)(&nonce);
	    if (rc != OK)
            {
                error->dbe_version = DBA_VERSION_V1988;
                error->dbe_type = DBE_TYPE_SECURITY;
                error->dbe_value = rc;
                return (DS_ERROR_CONNECT);
            }
	    rc = (dsap_security->serv_ckpath)((caddr_t) arg, 
		_ZTokenToSignDAS, &_ZDAS_mod,
		arg->dba_cpath, arg->dba_sig, &real_name);
	    if (rc != OK)
	    {
		error->dbe_version = DBA_VERSION_V1988;
		error->dbe_type = DBE_TYPE_SECURITY;
		error->dbe_value = rc;
		return (DS_ERROR_CONNECT);
	    }
	    else {
			if(dn_cmp(real_name, arg->dba_dn) == OK)
	    		{
				make_dsa_bind_arg(result);
				return (DS_OK);
	    		}
			else {
				(void) sprintf (buff,"User != Authenticated User, ie %s != ", dn2str(arg->dba_dn));
				pslog (log_dsap,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)real_name);
				error->dbe_version = DBA_VERSION_V1988;
				error->dbe_type = DBE_TYPE_SECURITY;
				error->dbe_value = DSE_SC_AUTHENTICATION;
				return (DS_ERROR_CONNECT);
	    		}
	   }
	}
	else
	{
		if (cn->cn_ctx != DS_CTX_X500_DAP)
		{
	    	    cn->cn_authen = DBA_AUTH_NONE;
	    	    make_dsa_bind_arg(result);
	    	    return (DS_OK);
		}
		else
		{
		    error->dbe_version = DBA_VERSION_V1988;
		    error->dbe_type = DBE_TYPE_SERVICE;
		    error->dbe_value = DSE_SV_UNAVAILABLE;
		    return (DS_ERROR_CONNECT);
		}
	}
    }

/* If we fall through to here, credentials are simple or protected simple */

    if ((res = really_find_entry(arg->dba_dn, TRUE, NULLDNSEQ, FALSE, &(entryptr), &(err), &(dsas))) == DS_OK) {
	/* is it really OK ??? */
	if ((entryptr->e_data == E_TYPE_CONSTRUCTOR) 
		|| (entryptr->e_data == E_TYPE_CACHE_FROM_MASTER)) {
		DN dn_found;
		DLOG(log_dsap, LLOG_TRACE, ("rfe (bind) returned a constructor"));
		dn_found = get_copy_dn(entryptr);
		res = constructor_dsa_info(dn_found,NULLDNSEQ,FALSE,entryptr,&err,&dsas);
		dn_free (dn_found);
	}
    }
    switch(res)
    {
    case DS_OK:
	/* entryptr filled out - break through to deal with it */
	break;

    case DS_CONTINUE:
	/*
	*  At this point a remote operation is required to compare
	*  the password given with the password of the entry, so
	*  fire up the remote operation and return without completing.
	*  Mark the operation as a BIND_COMPARE_OP and set the connection
	*  which will need to be restarted.
	*  Generate a compare argument.
	*  Chain the compare operation using the di_blocks.
	*/
	cn->cn_start.cs_bind_compare = on = oper_alloc();/* cn knows about on */
	on->on_type = ON_TYPE_BIND_COMPARE;
	on->on_bind_compare = cn;			/* on knows about cn */

	set_my_chain_args(&(on->on_req.dca_charg), arg->dba_dn);
	on->on_req.dca_dsarg.arg_type = OP_COMPARE;
	cma = &(on->on_req.dca_dsarg.arg_cm);

	cma->cma_common = ca_def;	/* struct copy */

		/* Set originator/requestor */
	if (on->on_req.dca_charg.cha_originator)  /* set my set_my_chain_arg */
		dn_free(on->on_req.dca_charg.cha_originator);
	on->on_req.dca_charg.cha_originator = dn_cpy(arg->dba_dn);
	cma->cma_common.ca_requestor = dn_cpy(arg->dba_dn);
 	cma->cma_common.ca_servicecontrol.svc_prio = SVC_PRIO_HIGH;

	cma->cma_object = dn_cpy(arg->dba_dn);

	if (arg->dba_auth_type == DBA_AUTH_SIMPLE)
	{	
		cma->cma_purported.ava_type = AttrT_cpy (at_password);
		cma->cma_purported.ava_value = 
			str2AttrV (arg->dba_passwd,str2syntax("octetstring"));
	}
	else
	{
		cma->cma_purported.ava_type = AttrT_cpy (at_p_password);
		cma->cma_purported.ava_value = 
			(AttributeValue) calloc(1, sizeof(attrVal));
		cma->cma_purported.ava_value->av_syntax = 
				str2syntax("protectedPassword");
		cma->cma_purported.ava_value->av_struct = (caddr_t) pp;
	}


	on->on_dsas = dsas;
	for(di_tmp=on->on_dsas; di_tmp!=NULL_DI_BLOCK; di_tmp=di_tmp->di_next)
	{
	    di_tmp->di_type = DI_OPERATION;
	    di_tmp->di_oper = on;
	}

	if(oper_chain(on) == OK)
	    return(DS_CONTINUE);

	oper_extract(on);
	cn->cn_start.cs_bind_compare = NULLOPER;

	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SERVICE;
	error->dbe_value = DSE_SV_UNAVAILABLE;

	return(DS_ERROR_CONNECT);

    case DS_X500_ERROR:
	/* User's entry doesn't exist, for example */
	LLOG(log_dsap, LLOG_TRACE, ("ds_bind - really_find_entry erred:"));
	log_ds_error(&(err));
	ds_error_free(&(err));
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SECURITY;
	error->dbe_value = DSE_SC_INVALIDCREDENTIALS;
	return(DS_ERROR_CONNECT);

    default:
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SERVICE;
	error->dbe_value = DSE_SV_DITERROR;
	return(DS_ERROR_CONNECT);
    }

    if ((as = as_find_type (entryptr->e_attributes,
		(arg->dba_auth_type == DBA_AUTH_SIMPLE) ? 
			at_password : at_p_password)) == NULLATTR)
    {
	/* No password in entry.
	 * Simple authentication is not possible for entities without passwords.
	 * Hence, give the `inappropriate authentication' message.
	 */
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SECURITY;
	error->dbe_value = DSE_SC_AUTHENTICATION;
	return (DS_ERROR_CONNECT);
    }

    if (arg->dba_auth_type == DBA_AUTH_SIMPLE) {
	if (strlen ((char *)as->attr_value->avseq_av.av_struct) != arg->dba_passwd_len)
		retval = -1;
	else
		retval = strncmp ((char *)as->attr_value->avseq_av.av_struct, 
					arg->dba_passwd, arg->dba_passwd_len);
    } else
	retval = check_guard(
		((struct protected_password *)
			as->attr_value->avseq_av.av_struct)->passwd, 
		((struct protected_password *)
			as->attr_value->avseq_av.av_struct)->n_octets, 
		arg->dba_time1, 
		arg->dba_passwd,
		arg->dba_passwd_len);

    if (retval == 0)
	{
	    /* Password OK! */
	    cn->cn_authen = arg->dba_auth_type;
	    make_dsa_bind_arg(result);
	    return (DS_OK);
	}
     else
	{
	/* password wrong ! */
	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type  = DBE_TYPE_SECURITY;
	error->dbe_value = DSE_SC_INVALIDCREDENTIALS;
	return (DS_ERROR_CONNECT);
	}
}

bind_compare_result_wakeup(on)
struct oper_act	* on;
{

    DLOG(log_dsap, LLOG_TRACE, ("bind_compare_result_wakeup()"));
    if(on->on_bind_compare == NULLCONN)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("bind_compare_result_wakeup - connection initiating compare already failed"));
    }
    else
    {
	if(on->on_resp.di_result.dr_res.dcr_dsres.res_cm.cmr_matched)
	{
	    DLOG(log_dsap, LLOG_DEBUG, ("bind_compare - user authenticated"));
	    on->on_bind_compare->cn_authen = on->on_bind_compare->cn_start.cs_ds.ds_bind_arg.dba_auth_type;
	    conn_init_res(on->on_bind_compare);
	}
	else
	{
	    struct ds_bind_error * error = &(on->on_bind_compare->cn_start.cs_err);
	    DLOG(log_dsap, LLOG_DEBUG, ("bind_compare - user NOT authenticated"));
	    error->dbe_version = DBA_VERSION_V1988;
	    error->dbe_type = DBE_TYPE_SECURITY;
	    /* Password match failed, therefore credentials are wrong */
	    error->dbe_value = DSE_SC_INVALIDCREDENTIALS;
	    conn_init_err(on->on_bind_compare);
	}
    }
    oper_conn_extract(on);
    oper_free(on);
}

bind_compare_error_wakeup(on)
struct oper_act	* on;
{
int errmsg = DSE_SV_DITERROR;
int errtype = DBE_TYPE_SERVICE;

    DLOG(log_dsap, LLOG_TRACE, ("bind_compare_error_wakeup()"));

    /*
    *  Check for referral and rechain if appropriate;
    *  Otherwise check if error requires propagation
    *  or another of the original di_blocks to be chained to.
    */

    if(on->on_bind_compare == NULLCONN)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("bind_compare_error_wakeup - connection initiating compare already failed"));
    }
    else
    {
	struct ds_bind_error * error = &(on->on_bind_compare->cn_start.cs_err);
	switch(on->on_resp.di_error.de_err.dse_type)
	{
	case DSE_NOERROR:
	    LLOG(log_dsap, LLOG_EXCEPTIONS, ("bind_compare_error_wakeup() - no error!"));
	break;
	case DSE_REFERRAL:
	    LLOG(log_dsap, LLOG_EXCEPTIONS, ("bind_compare_error_wakeup() - DAP referral received!"));
	case DSE_DSAREFERRAL:
	    /* Follow referral */
	    if(oper_rechain(on) == OK)
		return;
	break;
	case DSE_NAMEERROR:
	case DSE_SECURITYERROR:
	case DSE_ATTRIBUTEERROR:
	    errtype = DBE_TYPE_SECURITY;
	    errmsg = DSE_SC_INVALIDCREDENTIALS;
	    break;
	case DSE_SERVICEERROR:
	    errmsg = on->on_resp.di_error.de_err.ERR_SERVICE.DSE_sv_problem;
	    break;
 	default:
	    log_ds_error(&on->on_resp.di_error.de_err);
	    DLOG(log_dsap, LLOG_DEBUG, ("bind_compare_error_wakeup() - assuming all errors finish operation!"));
	break;
	}

	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = errtype;
	error->dbe_value = errmsg;
	conn_init_err(on->on_bind_compare);
    }
    oper_conn_extract(on);
    oper_free(on);
}

bind_compare_fail_wakeup(on)
struct oper_act	* on;
{
    DLOG(log_dsap, LLOG_TRACE, ("bind_compare_fail_wakeup()"));

    /*
    *  If there are any more "di_block"s to attempt it must be
    *  worth a go (perhaps this depends on the failure which
    *  has occurrred).
    */
    if(on->on_bind_compare == NULLCONN)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("bind_compare_fail_wakeup - connection initiating compare already failed"));
    }
    else
    {
	struct ds_bind_error * error = &(on->on_bind_compare->cn_start.cs_err);

	if(on->on_dsas)
	{
	    if(oper_chain(on) == OK)
		return;
	}

	if(on->on_dsas)
	{
	    /* oper_chain must be awaiting deferred di_blocks */
	    return;
	}

	error->dbe_version = DBA_VERSION_V1988;
	error->dbe_type = DBE_TYPE_SERVICE;
	error->dbe_value = DSE_SV_UNAVAILABLE;
	conn_init_err(on->on_bind_compare);
    }
    oper_conn_extract(on);
    oper_free(on);
}

do_ds_unbind (conn)
register        struct connection       * conn;
{
struct stat st;
#ifndef NO_STATS
char buff[LINESIZE];

	if(conn->cn_initiator)
	{
	    (void) sprintf (buff,"Unbind (%d) (initiator)",conn->cn_ad);
	}
	else
	{
	    (void) sprintf (buff,"Unbind (%d) (responder)",conn->cn_ad);
	}
	pslog (log_stat,LLOG_NOTICE,buff,(IFP)dn_print,(caddr_t)conn->cn_dn);
#endif

	if (fstat (conn->cn_ad, &st) != NOTOK) {
	    if (conn->cn_ad != log_dsap -> ll_fd)
#ifndef NO_STATS
	      if (conn->cn_ad != log_stat -> ll_fd)
#endif
	    LLOG (log_dsap, LLOG_EXCEPTIONS,
		  ("*** fd %d: fmt=0%o", conn->cn_ad, st.st_mode & S_IFMT));
        } else if (errno != EBADF) 
	    LLOG (log_dsap, LLOG_EXCEPTIONS,
		  ("*** fd %d: errno=%d ***", conn->cn_ad, errno));


	DLOG (log_dsap,LLOG_TRACE,("ds_un_bind"));
}

static struct dn_seq * reject_prefix_list = NULLDNSEQ;
static struct dn_seq * accept_prefix_list = NULLDNSEQ;
static int reject_len;

reject_length (str)
char * str;
{
	reject_len = atoi (str);
}

reject_prefix (str)
char * str;
{
struct dn_seq * dsa, *loop;

	if (( dsa=str2dnseq(str)) == NULLDNSEQ) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("Invalid reject name %s",str));	
		return;
	}
	
	if (reject_prefix_list == NULLDNSEQ) 
		reject_prefix_list = dsa;
	else {
		for (loop = reject_prefix_list; loop->dns_next != NULLDNSEQ; loop=loop->dns_next)
			;
		loop->dns_next = dsa;
	}
}


accept_prefix (str)
char * str;
{
struct dn_seq * dsa, *loop;

	if (( dsa=str2dnseq(str)) == NULLDNSEQ) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("Invalid accept name %s",str));	
		return;
	}
	
	if (accept_prefix_list == NULLDNSEQ) 
		accept_prefix_list = dsa;
	else {
		for (loop = accept_prefix_list; loop->dns_next != NULLDNSEQ; loop=loop->dns_next)
			;
		loop->dns_next = dsa;
	}
}

static check_prefix_list (dn)
DN dn;
{
	if (accept_prefix_list) {

		if (check_dnseq_prefix (accept_prefix_list,dn) == OK) {
			if (check_dnseq_prefix (reject_prefix_list,dn) == OK)
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	if (check_dnseq_prefix (reject_prefix_list,dn) == OK)
		return FALSE;

	return TRUE;
}

static check_dn_length (dn)
DN dn;
{
register int i = 0;
register DN tmp;

	for (tmp = dn; tmp != NULLDN ; tmp=tmp->dn_parent)
		i++;

	if ( i <= reject_len )
		return FALSE;

	return TRUE;
}


