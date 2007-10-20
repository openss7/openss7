/* ds_search.c - DSA search of the directory */

#ifndef lint
static char    *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/ds_search.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/ds_search.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: ds_search.c,v $
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
#include "quipu/list.h"		/* to get LSR # defs */
#include "quipu/ds_search.h"
#include "config.h"
#include "quipu/turbo.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

extern LLog    *log_dsap;

#ifndef NO_STATS
extern LLog    *log_stat;
static PS       filter_ps;
#endif

EntryInfo      *filterentry();
static EntryInfo *filterchildren();
static          test_avs();
static          apply_search();
static          substr_search();
static          aux_substr_search();
static          check_filteritem_presrch();
static          check_filter_presrch();
static          check_filterop_presrch();
static          check_filteritem();
static          check_filter();
static          check_filterop();
struct ds_search_task *st_done();
static          do_base();

extern Entry    database_root;
int             size;
char            qctx;
extern int      search_level;
IFP             approxfn();
IFP             av_cmp_fn();
#ifdef TURBO_INDEX
extern int      optimized_only;
#endif
static Ftypelist	ftlist;

#ifndef	NBBY
#define	NBBY	8
#endif
static int      big_size = 0;
static int      timelimit;
static int      phoneflag;
extern time_t   timenow;
extern int      admin_size;

Attr_Sequence   eis_select();
extern Attr_Sequence entry_find_type();

do_ds_search(arg, error, result, dnbind, target, local, refer, di_p,
	     dsp, quipu_ctx, tktime, entryonly, authtype)
register struct ds_search_arg *arg;
struct ds_search_result *result;
struct DSError *error;
DN              dnbind;
DN              target;
struct ds_search_task **local, **refer;
struct di_block **di_p;
char            dsp;
char            quipu_ctx;
time_t          tktime;
char            entryonly;
char            authtype;
{
	extern time_t   admin_time;
	int             ismanager = FALSE;
	int             retval;

	qctx = quipu_ctx;

	if ((timelimit = tktime) == (time_t) 0) {
		register int    i;

		for (i = NBBY * sizeof timelimit - 1; i > 0; i--)
			timelimit <<= 1, timelimit |= 1;
	}
	if (!dsp)
		ismanager = manager(dnbind);

	if (ismanager && big_size == 0) {
		register int    i;

		for (i = NBBY * sizeof big_size - 1; i > 0; i--)
			big_size <<= 1, big_size |= 1;
	}
	if (*local == NULL_ST) {
		DLOG(log_dsap, LLOG_TRACE, ("ds_search"));

		if (!dsp)
			target = arg->sra_baseobject;

		/* Put local stuff straight into result structure (dangerous) */
		result->srr_correlated = TRUE;
		result->srr_un.srr_unit = (struct ds_search_unit *) calloc(1, sizeof(struct ds_search_unit));
		result->CSR_cr = NULLCONTINUATIONREF;

		*local = st_alloc();
		(*local)->st_baseobject = dn_cpy(target);
		(*local)->st_originalbase = dn_cpy(target);
		if ((*local)->st_entryonly = entryonly)	/* assign */
			(*local)->st_subset = SRA_BASEOBJECT;
		else
			(*local)->st_subset = arg->sra_subset;
		(*local)->st_alias = NULLDN;
		(*local)->st_bind = NULLDN;
		(*local)->st_save = NULL_ST;

		/* search acl thangs */
		(*local)->st_saclheader = (struct thing_header *)
		    smalloc(sizeof(struct thing_header));
		(*local)->st_sacls = (caddr_t) NULLAVL;
		(*local)->st_saclrefcount = 1;
		(*local)->st_ftypeheader = (struct thing_header *)
		    smalloc(sizeof(struct thing_header));
		(*local)->st_ftypes = (caddr_t) NULLFTL;
		(*local)->st_ftyperefcount = 1;
		(*local)->st_baseptr = NULLENTRY;

		if (ismanager) {
			if (((*local)->st_size = arg->sra_common.ca_servicecontrol.svc_sizelimit) == SVC_NOSIZELIMIT)
				(*local)->st_size = big_size;
		} else if (((*local)->st_size = MIN(admin_size, arg->sra_common.ca_servicecontrol.svc_sizelimit)) == SVC_NOSIZELIMIT)
			(*local)->st_size = admin_size;

		(*local)->st_next = NULL_ST;

		result->CSR_entries = NULLENTRYINFO;

#ifndef NO_STATS
		if ((filter_ps = ps_alloc(str_open)) == NULLPS) {
			st_comp_free(*local);
			*local = NULL_ST;
			return (DS_ERROR_LOCAL);
		} if (str_setup(filter_ps, NULLCP, BUFSIZ, 0) == NOTOK) {
			st_comp_free(*local);
			*local = NULL_ST;
			return (DS_ERROR_LOCAL);
		}
#endif

		if (arg->sra_filter == NULLFILTER) {
			/* set the default */
			arg->sra_filter = filter_alloc();
			arg->sra_filter->flt_next = NULLFILTER;
			arg->sra_filter->flt_type = FILTER_AND;
			arg->sra_filter->FUFILT = NULLFILTER;
		}
		ftlist = NULLFTL;
		if (check_filter_presrch(arg->sra_filter, error, target) != OK) {
#ifndef NO_STATS
			ps_free(filter_ps);
#endif
			st_comp_free(*local);
			*local = NULL_ST;
			return (DS_ERROR_REMOTE);
		} else {
			Entry           entryptr;
#ifdef TURBO_INDEX
			(*local)->st_optimized =
			    optimized_filter( arg->sra_filter );
#endif

#ifndef NO_STATS
			*filter_ps->ps_ptr = 0;
			switch ((*local)->st_subset) {
			case SRA_ONELEVEL:
				LLOG(log_stat, LLOG_TRACE, ("Search onelevel %s", filter_ps->ps_base));
				break;
			case SRA_WHOLESUBTREE:
				LLOG(log_stat, LLOG_TRACE, ("Search subtree %s", filter_ps->ps_base));
				break;
			default:
				LLOG(log_stat, LLOG_TRACE, ("Search base %s", filter_ps->ps_base));
				break;
			}
			ps_free(filter_ps);
#endif

#ifdef TURBO_INDEX
			/*
			 * if optimized_only is turned on, refuse if:
			 *   1) filter is not optimized and
			 *   2) it's not the manager asking and
			 *   3) it's a subtree search (hack)
			 */

			if ( !((*local)->st_optimized) && optimized_only
			    && !ismanager
			    && arg->sra_subset == SRA_WHOLESUBTREE ) {
				LLOG(log_dsap, LLOG_EXCEPTIONS, ("Non-optimized filter not allowed"));
				error->dse_type = DSE_SERVICEERROR;
				error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
				*local = NULL_ST;
				return (DS_ERROR_REMOTE);
			}
#endif
			(*local)->st_ftypes = (caddr_t) ftlist;

			if ((arg->sra_subset == SRA_ONELEVEL) ||
			    (arg->sra_subset == SRA_WHOLESUBTREE)) {
				switch (find_child_entry((*local)->st_baseobject, &(arg->sra_common), dnbind, NULLDNSEQ, FALSE, &(entryptr), error, di_p)) {
				case DS_OK:
					/* Filled out entryptr - carry on */
					break;
				case DS_CONTINUE:

					/*
					 * Filled out di_p - what do we do
					 * with it ??
					 */
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_CONTINUE);

				case DS_X500_ERROR:

					/*
					 * Filled out error - what do we do
					 * with it ??
					 */
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_X500_ERROR);
				default:
					/* SCREAM */
					LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_search() - find_child_entry failed 1"));
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_ERROR_LOCAL);
				}
			} else {
				if ((*local)->st_baseobject == NULLDN) {
					error->dse_type = DSE_NAMEERROR;
					error->ERR_NAME.DSE_na_problem = DSE_NA_NOSUCHOBJECT;
					error->ERR_NAME.DSE_na_matched = NULLDN;
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_ERROR_REMOTE);
				}
				switch (find_entry((*local)->st_baseobject, &(arg->sra_common), dnbind, NULLDNSEQ, FALSE, &(entryptr), error, di_p, OP_SEARCH)) {
				case DS_OK:
					/* Filled out entryptr - carry on */
					break;
				case DS_CONTINUE:

					/*
					 * Filled out di_p - what do we do
					 * with it ??
					 */
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_CONTINUE);

				case DS_X500_ERROR:

					/*
					 * Filled out error - what do we do
					 * with it ??
					 */
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_X500_ERROR);
				default:
					/* SCREAM */
					LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_search() - find_entry failed 1"));
					st_comp_free(*local);
					*local = NULL_ST;
					return (DS_ERROR_LOCAL);
				}
			}

			/*
			 * if no error and NOT SVC_OPT_DONTDEREFERENCEALIASES
			 * then the alias will have been derefeferenced
			 * signified by NO_ERROR !!!
			 */
			if (error->dse_type == DSE_NOERROR) {
				result->CSR_object = NULLDN;
				result->CSR_common.cr_aliasdereferenced = FALSE;
			} else {
				result->CSR_common.cr_aliasdereferenced = TRUE;
				result->CSR_object = get_copy_dn(entryptr);
			}

			/* Strong authentication  */
			if ((retval = check_security_parms((caddr_t) arg,
						    _ZSearchArgumentDataDAS,
							   &_ZDAS_mod,
						arg->sra_common.ca_security,
				   arg->sra_common.ca_sig, &dnbind)) != 0) {
				error->dse_type = DSE_SECURITYERROR;
				error->ERR_SECURITY.DSE_sc_problem = retval;
				st_comp_free(*local);
				*local = NULL_ST;
				return (DS_ERROR_REMOTE);
			}
			/* Do we have the entire subtree, if so allow */
			/* authenticated search iff dap		      */

			if (!dsp && (entryptr->e_allchildrenpresent == 2))
				(*local)->st_bind = dnbind;

			/*
			 * one final check - will we allow such searched in
			 * this DSA ?
			 */
			if (arg->sra_subset == SRA_WHOLESUBTREE) {
				DN              dn;
				int             x = 0;
				for (dn = (*local)->st_baseobject; dn != NULLDN; dn = dn->dn_parent, x++);
				if (x < search_level) {
					if (!ismanager) {
						/* Too high */
						error->dse_type = DSE_SERVICEERROR;
						error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
						st_comp_free(*local);
						*local = NULL_ST;
						return (DS_ERROR_REMOTE);
					}
				}
				if (entryptr->e_data != E_TYPE_CONSTRUCTOR) {
					int             saclerror = 0;

					(*local)->st_baseptr = entryptr;
					if ((*local)->st_baseobject != NULLDN) {
						if ((result->CSR_entries = filterentry(arg, entryptr, (*local)->st_bind, authtype, &saclerror, (*local), 1)) != NULLENTRYINFO)
							(*local)->st_size--;
						else if (saclerror < 0) {
							error->dse_type = DSE_SECURITYERROR;
							error->ERR_SECURITY.DSE_sc_problem =
							    DSE_SC_ACCESSRIGHTS;
							return (DS_X500_ERROR);
						} else if (saclerror > 0) {
							result->CSR_limitproblem = LSR_ADMINSIZEEXCEEDED;
							return(DS_OK);
						}
					}
				} else {
					do_base(entryptr, local);
				}
			}
			result->CSR_limitproblem = LSR_NOLIMITPROBLEM;
			return (DS_SUSPEND);	/* yup - we will take the
						 * search on */
		}
	} else {
		int             saclerror = 0;

		DLOG(log_dsap, LLOG_TRACE, ("ds_search continuing"));

		size = (*local)->st_size;

		if (apply_search(arg, error, result, local, refer, ismanager,
				 authtype, &saclerror) == NOTOK) {
			if (saclerror < 0) {
				error->dse_type = DSE_SECURITYERROR;
				error->ERR_SECURITY.DSE_sc_problem =
				    DSE_SC_ACCESSRIGHTS;
				st_free(local);
				st_free_dis(refer,0);
				st_free(refer);
				return (DS_X500_ERROR);
			} else if (saclerror > 0) {
				result->CSR_limitproblem = LSR_ADMINSIZEEXCEEDED;
			} else {
				st_free(local);
				st_free_dis(refer,0);
				st_free(refer);
				return (DS_ERROR_REMOTE);
			}
		}
		if (size < 0) {
			st_free(local);
			st_free_dis(refer,0);
			st_free(refer);
			result->CSR_limitproblem =
			    arg->sra_common.ca_servicecontrol.svc_sizelimit
			    == SVC_NOSIZELIMIT
			    || arg->sra_common.ca_servicecontrol.svc_sizelimit
			    > admin_size
			    ? LSR_ADMINSIZEEXCEEDED
			    : LSR_SIZELIMITEXCEEDED;
			/* should fill out a POQ */
			return (DS_OK);
		}
		if (timelimit <= timenow) {
			st_free(local);
			st_free_dis(refer,0);
			st_free(refer);
			result->CSR_limitproblem =
			    arg->sra_common.ca_servicecontrol.svc_timelimit
			    == SVC_NOTIMELIMIT
			    || arg->sra_common.ca_servicecontrol.svc_timelimit
			    > admin_time
			    ? LSR_ADMINSIZEEXCEEDED
			    : LSR_TIMELIMITEXCEEDED;
			/* should fill out a POQ */
			return (DS_OK);
		}
		if ((*local)->st_next == NULL_ST) {
			st_free(local);
			if (saclerror == 0)
				result->CSR_limitproblem = LSR_NOLIMITPROBLEM;
			(void) dsa_search_control(arg, result);
			return (DS_OK);
		}
		(*local) = st_done(local);
		(*local)->st_size = size;
		return (DS_SUSPEND);
	}

}

/*
 * SEARCH TASK HANDLING
 */

st_comp_free(st)
struct ds_search_task *st;
{
	extern int rc_free();

	dn_free(st->st_baseobject);
	dn_free(st->st_originalbase);
	dn_free(st->st_alias);
	if (st->st_save != NULL_ST)
		st_free(&st->st_save);
	if (--st->st_saclrefcount <= 0) {
		(void) avl_free((Avlnode *) st->st_sacls, rc_free);
		free((char *) st->st_saclheader);
	}
	if (--st->st_ftyperefcount <= 0) {
		ftype_free((Ftypelist) st->st_ftypes);
		free((char *) st->st_ftypeheader);
	}
	free((char *) st);

}

st_free(st)
struct ds_search_task **st;
{
	struct ds_search_task *next;

	for (; (*st) != NULL_ST; (*st) = next) {
		next = (*st)->st_next;
		st_comp_free(*st);
	}
}

st_free_dis(st,internals)
struct ds_search_task **st;
int internals;
{
	struct ds_search_task *next;

	for (; (*st) != NULL_ST; (*st) = next) {
		next = (*st)->st_next;
		if ((*st)->st_di) {
			di_desist((*st)->st_di);
			if (internals)
				st_comp_free(*st);
		    }
	}
}

struct ds_search_task *st_done(st)
struct ds_search_task **st;
{
	struct ds_search_task *next;

	if ((next = (*st)->st_next) == NULL_ST)
		return NULL_ST;
	next->st_save = (*st);
	(*st)->st_next = (*st)->st_save;
	(*st)->st_save = NULL_ST;
	return (next);
}



/*
 * CHECK FILTER BEFORE SEARCHING
 */


static          check_filter_presrch(fltr, error, dn)
register Filter fltr;
struct DSError *error;
DN              dn;
{
	DLOG(log_dsap, LLOG_DEBUG, ("in check filter aux"));

	switch (fltr->flt_type) {
	case FILTER_ITEM:
		return (check_filteritem_presrch(&fltr->FUITEM, error, dn));
	case FILTER_AND:
#ifndef NO_STATS
		ps_print(filter_ps, "& ");
#endif
		return (check_filterop_presrch(fltr->FUFILT, error, dn));
	case FILTER_OR:
#ifndef NO_STATS
		ps_print(filter_ps, "| ");
#endif
		return (check_filterop_presrch(fltr->FUFILT, error, dn));
	case FILTER_NOT:
#ifndef NO_STATS
		ps_print(filter_ps, "! ");
#endif
		return (check_filter_presrch(fltr->FUFILT, error, dn));
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("check_filter protocol error"));
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
		return (NOTOK);
	}
	/* NOTREACHED */
}

static          check_filterop_presrch(fltr, error, dn)
register Filter fltr;
struct DSError *error;
DN              dn;
{
	register Filter ptr;
	int             i;

#ifndef NO_STATS
	ps_print(filter_ps, "(");
#endif
	DLOG(log_dsap, LLOG_DEBUG, ("in filter op aux"));
	for (ptr = fltr; ptr != NULLFILTER; ptr = ptr->flt_next) {
		i = check_filter_presrch(ptr, error, dn);
		if (i != OK)
			return (NOTOK);
	}
#ifndef NO_STATS
	ps_print(filter_ps, ")");
#endif
	return (OK);

}

static          prepare_string(c)
caddr_t         c;
{
	register char  *p;

	for (p = (char *) c; *p; p++)
		*p = chrcnv[*p];
}

static          check_filteritem_presrch(fitem, error, dn)
register struct filter_item *fitem;
struct DSError *error;
DN              dn;
{
	int             av_acl, av_update, av_schema, av_syntax;
	int		maxlen = -1;
	extern char     chrcnv[];
	extern char     nochrcnv[];

	DLOG(log_dsap, LLOG_DEBUG, ("search: check filter item aux"));
	if (fitem == NULLFITEM) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("check_filter_item protocol error (1)"));
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
		return (NOTOK);
	}
	switch (fitem->fi_type) {
	case FILTERITEM_APPROX:
		if (fitem->UNAVA.ava_type == NULLTABLE_ATTR)
			return (invalid_matching(fitem->UNAVA.ava_type, error, dn));

		if ((fitem->fi_ifp = approxfn(fitem->UNAVA.ava_type->oa_syntax)) == NULLIFP)
			/* approx not suported for this type */
			/* so set it to equality */
			fitem->fi_type = FILTERITEM_EQUALITY;
		/* NO break - check equality is OK */
		else
			prepare_string(fitem->UNAVA.ava_value->av_struct);
		if (sub_string(fitem->UNAVA.ava_type->oa_syntax))
			maxlen = strlen( fitem->UNAVA.ava_value->av_struct );

	case FILTERITEM_GREATEROREQUAL:
	case FILTERITEM_LESSOREQUAL:
	case FILTERITEM_EQUALITY:
		if (fitem->UNAVA.ava_type == NULLTABLE_ATTR)
			return (invalid_matching(fitem->UNAVA.ava_type, error, dn));

		if (fitem->fi_type != FILTERITEM_APPROX)
			if ((fitem->fi_ifp = av_cmp_fn(fitem->UNAVA.ava_type->oa_syntax)) == NULLIFP)
				return (invalid_matching(fitem->UNAVA.ava_type, error, dn));

		av_acl = str2syntax("acl");
		av_schema = str2syntax("schema");
		av_update = str2syntax("edbinfo");
		av_syntax = fitem->UNAVA.ava_type->oa_syntax;

		if ((av_syntax == av_acl)
		    || (av_syntax == av_schema)
		    || (av_syntax == av_update))
			return (invalid_matching(fitem->UNAVA.ava_type, error, dn));

		if ((fitem->fi_type == FILTERITEM_GREATEROREQUAL
		    || fitem->fi_type == FILTERITEM_LESSOREQUAL)
		    && sub_string(fitem->UNAVA.ava_type->oa_syntax)) {
			ftype_add( &ftlist, fitem->UNAVA.ava_type, maxlen,
			    (char *) fitem->UNAVA.ava_value->av_struct );
		} else {
			ftype_add( &ftlist, fitem->UNAVA.ava_type, maxlen,
			    (char *) NULL );
		}
		break;
	case FILTERITEM_SUBSTRINGS:
		if (fitem->UNSUB.fi_sub_type == NULLTABLE_ATTR)
			return (invalid_matching(fitem->UNSUB.fi_sub_type, error, dn));

		av_syntax = fitem->UNSUB.fi_sub_type->oa_syntax;

		if (!sub_string(av_syntax))
			return (invalid_matching(fitem->UNSUB.fi_sub_type, error, dn));

		if (case_exact_match(av_syntax))
			fitem->UNSUB.fi_sub_match = &nochrcnv[0];
		else {
			AV_Sequence     loopavs;
			fitem->UNSUB.fi_sub_match = &chrcnv[0];
			if (fitem->UNSUB.fi_sub_initial != NULLAV)
				prepare_string(fitem->UNSUB.fi_sub_initial->avseq_av.av_struct);
			for (loopavs = fitem->UNSUB.fi_sub_any; loopavs != NULLAV; loopavs = loopavs->avseq_next)
				prepare_string(loopavs->avseq_av.av_struct);
			if (fitem->UNSUB.fi_sub_final != NULLAV)
				prepare_string(fitem->UNSUB.fi_sub_final->avseq_av.av_struct);
		}
#ifdef TURBO_INDEX
		/* find the longest substring length in the filter */
		{
			AV_Sequence     loopavs;
			int		tmplen;

			if ( fitem->UNSUB.fi_sub_initial != NULLAV )
				maxlen = strlen( fitem->UNSUB.fi_sub_initial->avseq_av.av_struct );
			for ( loopavs = fitem->UNSUB.fi_sub_any;
			    loopavs != NULLAV; loopavs = loopavs->avseq_next ) {
				tmplen = strlen( loopavs->avseq_av.av_struct );
				if ( tmplen > maxlen )
					maxlen = tmplen;
			}
			if ( fitem->UNSUB.fi_sub_final != NULLAV ) {
				tmplen = strlen( fitem->UNSUB.fi_sub_final->avseq_av.av_struct );
				if ( tmplen > maxlen )
					maxlen = tmplen;
			}
		}
		ftype_add( &ftlist, fitem->UNAVA.ava_type, maxlen,
		    (char *) NULL );
		break;
#endif
	case FILTERITEM_PRESENT:
		ftype_add( &ftlist, fitem->UNAVA.ava_type, maxlen, 
		    (char *) NULL );
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("check_filter_item protocol error (2)"));
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
		return (NOTOK);
	}

#ifndef NO_STATS
	ps_print(filter_ps, "(");
	switch (fitem->fi_type) {
	case FILTERITEM_APPROX:
		AttrT_print(filter_ps, fitem->UNAVA.ava_type, EDBOUT);
		ps_print(filter_ps, "~=");
		AttrV_print(filter_ps, fitem->UNAVA.ava_value, EDBOUT);
		break;
	case FILTERITEM_EQUALITY:
		AttrT_print(filter_ps, fitem->UNAVA.ava_type, EDBOUT);
		ps_print(filter_ps, "=");
		AttrV_print(filter_ps, fitem->UNAVA.ava_value, EDBOUT);
		break;
	case FILTERITEM_GREATEROREQUAL:
		AttrT_print(filter_ps, fitem->UNAVA.ava_type, EDBOUT);
		ps_print(filter_ps, ">=");
		AttrV_print(filter_ps, fitem->UNAVA.ava_value, EDBOUT);
		break;
	case FILTERITEM_LESSOREQUAL:
		AttrT_print(filter_ps, fitem->UNAVA.ava_type, EDBOUT);
		ps_print(filter_ps, "<=");
		AttrV_print(filter_ps, fitem->UNAVA.ava_value, EDBOUT);
		break;
	case FILTERITEM_SUBSTRINGS:
		AttrT_print(filter_ps, fitem->UNSUB.fi_sub_type, EDBOUT);
		ps_print(filter_ps, "=");
		avs_print_aux(filter_ps, fitem->UNSUB.fi_sub_initial, EDBOUT, "*");
		ps_print(filter_ps, "*");
		avs_print_aux(filter_ps, fitem->UNSUB.fi_sub_any, EDBOUT, "*");
		ps_print(filter_ps, "*");
		avs_print_aux(filter_ps, fitem->UNSUB.fi_sub_final, EDBOUT, "*");
		break;
	case FILTERITEM_PRESENT:
		AttrT_print(filter_ps, fitem->UNTYPE, EDBOUT);
		ps_print(filter_ps, "=*");
		break;
	}
	ps_print(filter_ps, ")");
#endif
	return (OK);
}

/* APPLY SEARCH TO ONE LEVEL */

static          apply_search(arg, error, result, local, refer, ismanager,
			authtype, saclerror)
struct ds_search_arg *arg;
struct DSError *error;
struct ds_search_result *result;
struct ds_search_task **local, **refer;
int             ismanager;
int             authtype;
int		*saclerror;
{
	Entry           entryptr;
	EntryInfo      *einfo = NULLENTRYINFO;
	struct di_block *di_tmp;

	if ((*local)->st_subset == SRA_BASEOBJECT) {
		if ((*local)->st_baseobject == NULLDN) {
			LLOG(log_dsap, LLOG_TRACE, ("NULL Base in search ignored"));
			/* to stop poisoning... */
			return (DS_OK);
		}
		switch (find_entry((*local)->st_baseobject, &(arg->sra_common), (*local)->st_bind, NULLDNSEQ, FALSE, &(entryptr), error, &(di_tmp), OP_SEARCH)) {
		case DS_OK:
			/* Filled out entryptr - carry on */
			break;
		case DS_CONTINUE:
			/* Filled out di_p - what do we do with it ?? */
			subtask_refer(arg, local, refer, ismanager, di_tmp);
			return (DS_OK);

		case DS_X500_ERROR:
			/* Filled out error - what do we do with it ?? */
			/* The only problem can be alias error etc */
			/* to stop poisoning return OK */
			log_ds_error(error);
			ds_error_free(error);
			return (DS_OK);
		default:
			/* SCREAM */
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_search() - find_entry failed 2"));
			return (DS_ERROR_LOCAL);
		}
		/* must be an alias or referral task... */
		if ( (*local)->st_baseptr == NULLENTRY )
			(*local)->st_baseptr = entryptr;
	} else {
		switch (find_child_entry((*local)->st_baseobject, &(arg->sra_common), (*local)->st_bind, NULLDNSEQ, FALSE, &(entryptr), error, &(di_tmp))) {
		case DS_OK:
			/* Filled out entryptr - carry on */
			break;
		case DS_CONTINUE:
			/* Filled out di_p - what do we do with it ?? */
			subtask_refer(arg, local, refer, ismanager, di_tmp);
			return (DS_OK);

		case DS_X500_ERROR:
			/* Filled out error - what do we do with it ?? */
			/* The only problem can be alias error etc */
			/* to stop poisoning return OK */
			log_ds_error(error);
			return (DS_OK);
		default:
			/* SCREAM */
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("do_ds_search() - find_child_entry failed 2"));
			return (DS_ERROR_LOCAL);
		}
		/* must be an alias or referral task... */
		if ( (*local)->st_baseptr == NULLENTRY )
			(*local)->st_baseptr = entryptr;

		if ((*local)->st_subset == SRA_WHOLESUBTREE) {
			if (entryptr->e_data != E_TYPE_CONSTRUCTOR) {
				if ((*local)->st_alias) {
					*saclerror = 0;
					if ((einfo = filterentry(arg, entryptr,
					    (*local)->st_bind, authtype, saclerror,
					    *local, 1)) != NULLENTRYINFO) {
						(*local)->st_size--;
						if (result->CSR_entries == NULLENTRYINFO)
							result->CSR_entries = einfo;
						else
							entryinfo_merge(result->CSR_entries, 
							einfo, 
							!arg->sra_hitalias);
					}

					/*
					 * if saclerror is set, some
					 * searchacl with the zeroifexceeded
					 * flag set has had its sizelimit
					 * exceeded.  we should return a
					 * security error in this case.
					 */

					if (*saclerror)
						return (NOTOK);
				}
			} else {
				do_base(entryptr, local);
			}
		}
	}

	*saclerror = 0;
	switch ((*local)->st_subset) {
	case SRA_BASEOBJECT:
		einfo = filterentry(arg, entryptr, (*local)->st_bind,
				    authtype, saclerror, *local, 1);
		break;
	case SRA_ONELEVEL:
	case SRA_WHOLESUBTREE:
		einfo = filterchildren(arg, entryptr, local, refer, ismanager,
				       authtype, saclerror);
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("search protocol error"));
		error->dse_type = DSE_SERVICEERROR;
		error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
		return (DS_X500_ERROR);
	}

	if (einfo != NULLENTRYINFO)
		if (result->CSR_entries == NULLENTRYINFO)
			result->CSR_entries = einfo;
		else
			entryinfo_merge(result->CSR_entries, einfo,
					!arg->sra_hitalias);

	result->CSR_common.cr_requestor = NULLDN;

	if (*saclerror)
		return (NOTOK);

	return (DS_OK);
}

/*
 * SEARCH CHILDREN
 */


/*
 * search_kid2 - called from search_kid via avl_apply to apply
 * a filter to each entry in a tree of sibling entries without
 * looking at levels below.
 */

static          search_kid2(e, ska)
Entry           e;
struct search_kid_arg *ska;
{
	struct ds_search_task *new_task;
	EntryInfo      *eptr;
	int		saclerror = 0;

	if (size < 0)
		return (NOTOK);

	if ((e->e_alias != NULLDN)
	    && (do_alias(ska->ska_arg, e, ska->ska_local) == OK))
		return (OK);

	eptr = filterentry( ska->ska_arg, e, (*ska->ska_local)->st_bind,
	    ska->ska_authtype, &saclerror, (*ska->ska_local), 1 );

	if ( saclerror ) {
		ska->ska_saclerror = saclerror;
		return( NOTOK );
	}

	if (eptr != NULLENTRYINFO && size != -1)
		if (*ska->ska_einfo == NULLENTRYINFO)
			*ska->ska_einfo = eptr;
		else
			entryinfo_merge(*ska->ska_einfo, eptr, 
					!ska->ska_arg->sra_hitalias);

	if ( size < 0 )
		return( NOTOK );

	if (!isleaf(e)) {
		new_task = st_alloc();
		new_task->st_save = NULL_ST;
		new_task->st_baseobject = get_copy_dn(e);
		new_task->st_originalbase = dn_cpy((*ska->ska_local)->st_originalbase);
		new_task->st_size = 0;
		new_task->st_alias = NULLDN;
		new_task->st_bind = (*ska->ska_local)->st_bind;
		new_task->st_subset = SRA_WHOLESUBTREE;
#ifdef TURBO_INDEX
		new_task->st_optimized = (*ska->ska_local)->st_optimized;
#endif
		new_task->st_next = (*ska->ska_local)->st_next;
		new_task->st_entryonly = FALSE;
		new_task->st_saclheader = (*ska->ska_local)->st_saclheader;
		new_task->st_saclrefcount++;
		new_task->st_ftypeheader = (*ska->ska_local)->st_ftypeheader;
		new_task->st_ftyperefcount++;
		new_task->st_baseptr = (*ska->ska_local)->st_baseptr;
		(*ska->ska_local)->st_next = new_task;
	}
	if (ska->ska_tmp > SEARCH_DELTA_SIZE) {
		if (timelimit <= (timenow = time((time_t *) 0)))
			return (NOTOK);
		ska->ska_tmp = 0;
		ska->ska_domore = FALSE;
	}
	ska->ska_tmp++;

	return (OK);
}

/*
 * search_kid - called from filterchildren via avl_apply to apply
 * a filter to each entry in a tree of sibling entries, looking also
 * at one level below.
 */

static          search_kid(e, ska)
Entry           e;
struct search_kid_arg *ska;
{
	EntryInfo      *eptr;
	int		saclerror = 0;
#ifdef TURBO_INDEX
	struct ds_search_task	*new_task;
	DN			dn;
	extern Avlnode		*subtree_index;
	extern int		idn_cmp();
#endif

	if (size < 0)
		return (NOTOK);

	if ((e->e_alias != NULLDN)
	    && (do_alias(ska->ska_arg, e, ska->ska_local) == OK))
		return (OK);

#ifdef TURBO_INDEX
	/*
	 * if this entry has an index associated with it, make a new
	 * search task for the subtree so the index gets searched.
	 * if we don't do this, the index could be skipped.
	 */

	dn = NULLDN;
	if ( (*ska->ska_local)->st_subset == SRA_WHOLESUBTREE && !isleaf( e )
	    && get_subtree_index( dn = get_copy_dn( e ) ) ) {
		new_task = st_alloc();
		new_task->st_save = NULL_ST;
		new_task->st_baseobject = dn;
		new_task->st_originalbase = dn_cpy((*ska->ska_local)->st_originalbase);
		new_task->st_size = 0;
		new_task->st_alias = NULLDN;
		new_task->st_bind = (*ska->ska_local)->st_bind;
		new_task->st_subset = SRA_WHOLESUBTREE;
		new_task->st_optimized = (*ska->ska_local)->st_optimized;
		new_task->st_next = (*ska->ska_local)->st_next;
		new_task->st_entryonly = FALSE;
		new_task->st_saclheader = (*ska->ska_local)->st_saclheader;
		new_task->st_saclrefcount++;
		new_task->st_ftypeheader = (*ska->ska_local)->st_ftypeheader;
		new_task->st_ftyperefcount++;
		new_task->st_baseptr = (*ska->ska_local)->st_baseptr;
		(*ska->ska_local)->st_next = new_task;

		/* go on to the next entry */
		return( OK );
	}
	if ( dn != NULLDN )
		dn_free( dn );
#endif

	eptr = filterentry( ska->ska_arg, e, (*ska->ska_local)->st_bind,
	    ska->ska_authtype, &saclerror, (*ska->ska_local), 1 );

	if ( saclerror ) {
		ska->ska_saclerror = saclerror;
		return( NOTOK );
	}

	if (eptr != NULLENTRYINFO && size != -1)
		if (*ska->ska_einfo == NULLENTRYINFO)
			*ska->ska_einfo = eptr;
		else
			entryinfo_merge(*ska->ska_einfo, eptr,
					!ska->ska_arg->sra_hitalias);

	if ( size < 0 )
		return( NOTOK );

	if (ska->ska_tmp > SEARCH_DELTA_SIZE) {
		if (timelimit <= (timenow = time((time_t *) 0)))
			return (NOTOK);
		ska->ska_tmp = 0;
		ska->ska_domore = FALSE;
	}
	ska->ska_tmp++;

	if ((*ska->ska_local)->st_subset == SRA_WHOLESUBTREE && (!isleaf(e))) {
		if (check_acl((*ska->ska_local)->st_bind, ACL_READ,
			      e->e_acl->ac_child, NULLDN) == OK) {
			if (((e->e_children != NULLAVL)
			     && (e->e_allchildrenpresent == FALSE))
			    || (e->e_children == NULLAVL)) {
				search_refer(ska->ska_arg, e, ska->ska_local,
					ska->ska_refer, ska->ska_ismanager);
/*
                        } else if (ska->ska_domore) {
*/
			} else {

				/*
				 * let other connections progress every
				 * SEARCH_DELTA_SIZE entries searched.
				 */
				if (!ska->ska_domore)
					(void) dsa_wait(0);

				ska->ska_tmp = 0;
				ska->ska_domore = TRUE;
				(void) avl_apply(e->e_children, search_kid2,
					 (caddr_t) ska, NOTOK, AVL_INORDER);

				if (timelimit <= (timenow = time((time_t *) 0)))
					return (NOTOK);

				if (ska->ska_domore)
					ska->ska_domore = (ska->ska_tmp <
						   (SEARCH_DELTA_SIZE / 5));
				if (!ska->ska_domore)
					(void) dsa_wait(0);
			}
/*
It doesn't seem to make much sense to do this here with the avl's...
unless there's some reason calling dsa_wait() won't do the job.
			} else {
                                new_task = st_alloc();
                                new_task->st_save = NULL_ST;
                                new_task->st_baseobject = get_copy_dn(e);
				new_task->st_originalbase = dn_cpy((*ska->ska_local)->st_originalbase);
                                new_task->st_size = 0;
                                new_task->st_alias = NULLDN;
				new_task->st_bind = (*ska->ska_local)->st_bind;
                                new_task->st_subset = SRA_WHOLESUBTREE;
#ifdef TURBO_INDEX
                                new_task->st_optimized =
                                    (*ska->ska_local)->st_optimized;
#endif
                                new_task->st_next = (*ska->ska_local)->st_next;
				new_task->st_entryonly = FALSE;
				new_task->st_saclheader = 
				    (*ska->ska_local)->st_saclheader;
				new_task->st_saclrefcount++;
				new_task->st_ftypeheader =
				    (*ska->ska_local)->st_ftypeheader;
				new_task->st_ftyperefcount++;
				new_task->st_baseptr =
				    (*ska->ska_local)->st_baseptr;
                                (*ska->ska_local)->st_next = new_task;
                        }
*/
		}
	}
	return (OK);
}


static EntryInfo *filterchildren(arg, entryptr, local, refer, ismanager,
				authtype, saclerror)
struct ds_search_arg *arg;
Entry           entryptr;
struct ds_search_task **local, **refer;
int             ismanager;
char		authtype;
int		*saclerror;
{
	EntryInfo      *einfo = NULLENTRYINFO;
	register int    tmp = 0;
	char            domore = TRUE;
	Avlnode        *ptr;
	struct search_kid_arg ska;
#ifdef TURBO_INDEX
	extern Avlnode *subtree_index;
	extern Avlnode *sibling_index;
	int             idn_cmp();
#endif

	DLOG(log_dsap, LLOG_DEBUG, ("search: filter children"));

	if (entryptr == NULLENTRY)
		return (NULLENTRYINFO);

	if (isleaf(entryptr))
		return (NULLENTRYINFO);

	if (check_acl((*local)->st_bind, ACL_READ, entryptr->e_acl->ac_child, (*local)->st_baseobject) == NOTOK) {
		return (NULLENTRYINFO);
	}
	if (entryptr->e_alias != NULLDN) {
		(void) do_alias(arg, entryptr, local);
		return (NULLENTRYINFO);
	}
	if ((ptr = entryptr->e_children) == NULLAVL
	    || entryptr->e_allchildrenpresent == FALSE) {
		search_refer(arg, entryptr, local, refer, ismanager);
		return (NULLENTRYINFO);
	}
	/* search everything at this level */
	ska.ska_einfo = &einfo;
	ska.ska_arg = arg;
	ska.ska_local = local;
	ska.ska_refer = refer;
	ska.ska_tmp = tmp;
	ska.ska_domore = domore;
	ska.ska_ismanager = ismanager;
	ska.ska_saclerror = 0;
	ska.ska_authtype = authtype;

#ifdef TURBO_INDEX
	/* non optimized filter */
	if ((*local)->st_optimized == 0) {
		(void) avl_apply(ptr, search_kid, (caddr_t) & ska, NOTOK, AVL_INORDER);

		/* optimized filter & subtree search & subtree indexed */
	} else if (arg->sra_subset == SRA_WHOLESUBTREE
		   && get_subtree_index((*local)->st_baseobject)) {
		(void) turbo_subtree_search(entryptr, &ska);

		/* optimized filter & sibling search & siblings indexed */
	} else if (arg->sra_subset == SRA_ONELEVEL
		   && get_sibling_index((*local)->st_baseobject)) {
		(void) turbo_sibling_search(entryptr, &ska);

		/* optimized filter, but no index to search */
	} else {
		(void) avl_apply(ptr, search_kid, (caddr_t) & ska, NOTOK, AVL_INORDER);
	}
#else
	(void) avl_apply(ptr, search_kid, (caddr_t) & ska, NOTOK, AVL_INORDER);
#endif

	tmp = ska.ska_tmp;
	domore = ska.ska_domore;
	*saclerror = ska.ska_saclerror;

	return (einfo);
}

/*
 * HANDLE ALIASES AND REFERRALS
 */

do_alias(arg, eptr, local)
struct ds_search_arg *arg;
Entry           eptr;
struct ds_search_task **local;
{
	struct ds_search_task *new_task;
	struct ds_search_task *st;
	DN              st_dn;
	int		i;

	if (!arg->sra_searchaliases)
		return NOTOK;

	arg->sra_hitalias = 1;

	/*
	 * skip creating a new search task for aliases that do not
	 * escape the scope of the search.  they will be picked up
	 * by another search task already in the works.
	 */

	if ( arg->sra_subset == SRA_WHOLESUBTREE ) {
		i = th_prefix( (*local)->st_originalbase, eptr->e_alias );
		if ( i <= 0 )
			return( OK );
	} else if ( arg->sra_subset == SRA_ONELEVEL ) {
		if ( th_prefix( (*local)->st_originalbase, eptr->e_alias )
		    == -2 )
			return( OK );
	}

	DLOG(log_dsap, LLOG_DEBUG, ("alias in search path"));

	/* Check we have not been here before... */
	for (st = (*local)->st_save; st != NULL_ST; st = st->st_next) {
		if (st->st_alias == NULLDN)
			st_dn = st->st_baseobject;
		else
			st_dn = st->st_alias;
		if (dn_cmp(eptr->e_alias, st_dn) == 0) {
			LLOG(log_dsap, LLOG_TRACE, ("local search - loop detected"));
			return OK;
		}
	}

	new_task = st_alloc();
	new_task->st_save = NULL_ST;
	new_task->st_baseobject = get_copy_dn(eptr);
	new_task->st_originalbase = dn_cpy((*local)->st_originalbase);
	new_task->st_size = 0;	/* fill in later */
	new_task->st_alias = dn_cpy(eptr->e_alias);
	new_task->st_bind = (*local)->st_bind;
	new_task->st_entryonly = FALSE;
	new_task->st_saclheader = (*local)->st_saclheader;
	new_task->st_saclrefcount++;
	new_task->st_ftypeheader = (*local)->st_ftypeheader;
	new_task->st_ftyperefcount++;
	new_task->st_baseptr = NULLENTRY;	/* will be filled in later */

	switch ((*local)->st_subset) {
	case SRA_ONELEVEL:
		new_task->st_entryonly = TRUE;
		/* fall */
	case SRA_BASEOBJECT:
		new_task->st_subset = SRA_BASEOBJECT;
		break;
	case SRA_WHOLESUBTREE:
		new_task->st_subset = SRA_WHOLESUBTREE;
		break;
	}

	new_task->st_next = (*local)->st_next;
	(*local)->st_next = new_task;

	return (OK);
}

static          do_base(eptr, local)
Entry           eptr;
struct ds_search_task **local;
{
	struct ds_search_task *new_task;

	DLOG(log_dsap, LLOG_DEBUG, ("Making baseobject search"));

	new_task = st_alloc();
	new_task->st_save = NULL_ST;
	new_task->st_baseobject = get_copy_dn(eptr);
	new_task->st_originalbase = dn_cpy((*local)->st_originalbase);
	new_task->st_size = 0;	/* fill in later */
	new_task->st_alias = NULLDN;
	new_task->st_bind = (*local)->st_bind;
	new_task->st_entryonly = TRUE;	/* If is a subtree search we are
					 * breaking protocol here */
	/* BUT... There is no other way to do it !!! */
	new_task->st_subset = SRA_BASEOBJECT;
	new_task->st_saclheader = (*local)->st_saclheader;
	new_task->st_saclrefcount++;
	new_task->st_ftypeheader = (*local)->st_ftypeheader;
	new_task->st_ftyperefcount++;
	new_task->st_baseptr = (*local)->st_baseptr;
	new_task->st_next = (*local)->st_next;
	(*local)->st_next = new_task;
}

search_refer(arg, entryptr, local, refer, ismanager)
struct ds_search_arg *arg;
Entry           entryptr;
struct ds_search_task **local, **refer;
int             ismanager;
{
	struct ds_search_task *new_task;
	struct DSError  error;
	struct di_block *di_tmp;
	DN              name;

	name = get_copy_dn(entryptr);

	switch (dsa_info_new(name, NULLDNSEQ, FALSE, entryptr, &(error), &(di_tmp))) {
	case DS_OK:
		/* A di_block ready for use */
		break;
	case DS_CONTINUE:
		/* A deferred di_block */
		break;
	case DS_X500_ERROR:
		/* An error */
		pslog(log_dsap, LLOG_EXCEPTIONS, "search_refer failed",
		     (IFP) dn_print, (caddr_t) name);
		log_ds_error(&(error));
		ds_error_free(&(error));
		dn_free(name);
		return;
	default:
		/* A local error - scream */
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("search_refer - dsa_info_new() failed"));
		dn_free(name);
		return;
	}

	DLOG(log_dsap, LLOG_DEBUG, ("referral in search path"));

	new_task = st_alloc();
	new_task->st_save = NULL_ST;
	new_task->st_baseobject = name;
	new_task->st_originalbase = dn_cpy((*local)->st_originalbase);
	new_task->st_subset = (*local)->st_subset;
	new_task->st_alias = NULLDN;
	new_task->st_bind = NULLDN;
	new_task->st_entryonly = (*local)->st_entryonly;
	new_task->st_saclheader = (*local)->st_saclheader;
	new_task->st_saclrefcount++;
	new_task->st_ftypeheader = (*local)->st_ftypeheader;
	new_task->st_ftyperefcount++;

	new_task->st_baseptr = (*local)->st_baseptr;
	if (ismanager) {
		if ((new_task->st_size = arg->sra_common.ca_servicecontrol.svc_sizelimit) == SVC_NOSIZELIMIT)
			new_task->st_size = big_size;
	} else if ((new_task->st_size = MIN(admin_size, arg->sra_common.ca_servicecontrol.svc_sizelimit)) == SVC_NOSIZELIMIT)
		new_task->st_size = admin_size;

	new_task->st_di = di_tmp;
	new_task->st_next = *refer;
	*refer = new_task;
}

/*
 * SEARCH ENTRY
 */

EntryInfo      *filterentry(arg, entryptr, binddn,
			    authtype, saclerror, local, dosacl)
struct ds_search_arg 	*arg;
register Entry  	entryptr;
DN              	binddn;
char			authtype;
int            		*saclerror;
struct ds_search_task 	*local;
char			dosacl;
{
	register EntryInfo	*einfo;

	DLOG(log_dsap, LLOG_DEBUG, ("search: filter entry"));

	if (check_filter(arg->sra_filter, entryptr, binddn) != OK) {
		DLOG(log_dsap, LLOG_DEBUG, ("none found"));
		return (NULLENTRYINFO);
	}

	if ( check_acl( binddn, ACL_READ, entryptr->e_acl->ac_entry, NULLDN )
	    == NOTOK )
		return( NULLENTRYINFO );

	/*
	 * Check that the base searchacl allows access.  Also
	 * check that all ancestor nodes (up to and including the
	 * search base in a subtree search, just the parent in a 
	 * single-level search) allow the search.  If not, return
	 * with saclerror set if a zero-if-exceeded flag was exceeded.
	 */

	if ( dosacl && check_ancestor_sacls( binddn, NULLDN, entryptr,
	    arg->sra_subset, local, authtype, saclerror ) == NOTOK ) {
		return( NULLENTRYINFO );
	}

	einfo = entryinfo_alloc();
	einfo->ent_dn = get_copy_dn(entryptr);
	einfo->ent_eptr = entryptr;

	einfo->ent_attr = eis_select(arg->sra_eis, entryptr, binddn, qctx && arg->sra_eis.eis_allattributes, einfo->ent_dn);

	einfo->ent_iscopy = entryptr->e_data;
	einfo->ent_age = (time_t) 0;
	einfo->ent_next = NULLENTRYINFO;
	size--;
	return (einfo);
}

/*
 * TEST FILTER AGAINST SINGLE ENTRY
 */

static          check_filter(fltr, entryptr, binddn)
register Filter fltr;
register Entry  entryptr;
DN              binddn;
{
	register int    i;

	DLOG(log_dsap, LLOG_DEBUG, ("in check filter"));
	switch (fltr->flt_type) {
	case FILTER_ITEM:
		return (check_filteritem(&fltr->FUITEM, entryptr, binddn));
	case FILTER_AND:
	case FILTER_OR:
		return (check_filterop(fltr->FUFILT, entryptr, fltr->flt_type, binddn));
	case FILTER_NOT:
		if ((i = check_filter(fltr->FUFILT, entryptr, binddn)) == OK)
			return NOTOK;
		else if (i == NOTOK)
			return OK;
		else
			return i;
	}
	/* NOTREACHED */
}

static          check_filterop(fltr, entryptr, op, binddn)
register Filter fltr;
register Entry  entryptr;
int             op;
DN              binddn;
{
	register Filter ptr;
	int             result;

	DLOG(log_dsap, LLOG_DEBUG, ("in filter op"));

	/* effect of applying logical operator to zero operands */
	if (op == FILTER_OR)
		result = NOTOK;
	else
		result = OK;

	for (ptr = fltr; ptr != NULLFILTER; ptr = ptr->flt_next)
		switch (check_filter(ptr, entryptr, binddn)) {
		case MAYBE:
/* Beware of 'Pathological NOT' here.
 * To comply with the December '88 X.500, should just drop through here.
 * For the security to work properly, also set result to MAYBE.
 */
			result = MAYBE;
			break;
		case OK:
			if (op == FILTER_OR) {
				DLOG(log_dsap, LLOG_DEBUG, ("or ok"));
				return (OK);
			}
			break;
		case NOTOK:
			if (op == FILTER_AND) {
				DLOG(log_dsap, LLOG_DEBUG, ("and not"));
				return (NOTOK);
			}
			break;
		case -2:
		default:
			return (-2);
		}


	return (result);
}

/*
 * CHECK FILTER ITEM AGAINST ENTRY
 */

static          check_filteritem(fitem, entryptr, binddn)
register struct filter_item *fitem;
register Entry  entryptr;
DN              binddn;
{
	register Attr_Sequence as;
	Attr_Sequence   ias = NULLATTR;
	AttributeType   at;
	Attr_Sequence   ptr;
	int             i, res;

	DLOG(log_dsap, LLOG_DEBUG, ("search: check filter item"));

	switch (fitem->fi_type) {
	case FILTERITEM_APPROX:
	case FILTERITEM_EQUALITY:
	case FILTERITEM_GREATEROREQUAL:
	case FILTERITEM_LESSOREQUAL:
		at = fitem->UNAVA.ava_type;
		break;
	case FILTERITEM_SUBSTRINGS:
		at = fitem->UNSUB.fi_sub_type;
		break;
	case FILTERITEM_PRESENT:
		if ((as = entry_find_type(entryptr, fitem->UNTYPE)) == NULLATTR)
			return NOTOK;
		else
			return OK;
	}

	if ((as = as_find_type(entryptr->e_attributes, at)) == NULLATTR) {
		if (entryptr->e_iattr) {
			for (ptr = entryptr->e_iattr->i_default; ptr != NULLATTR; ptr = ptr->attr_link) {
				if ((i = AttrT_cmp(ptr->attr_type, at)) <= 0) {
					if (i == 0)
						as = ptr;
					break;
				}
			}
			if (as == NULLATTR)
				for (ptr = entryptr->e_iattr->i_always; ptr != NULLATTR; ptr = ptr->attr_link) {
					if ((i = AttrT_cmp(ptr->attr_type, at)) <= 0) {
						if (i == 0)
							as = ptr;
						break;
					}
				}
			if (as == NULLATTR)
				return MAYBE;
		} else
			return MAYBE;
	} else {
		/* see if there is an 'always' attribute */
		if (entryptr->e_iattr) {
			for (ptr = entryptr->e_iattr->i_always; ptr != NULLATTR; ptr = ptr->attr_link) {
				if ((i = AttrT_cmp(ptr->attr_type, at)) <= 0) {
					if (i == 0)
						ias = ptr;
					break;
				}
			}
		}
	}

	if ( check_acl(binddn, ACL_COMPARE, as->attr_acl, NULLDN) != OK)
		return MAYBE;

	switch (fitem->fi_type) {
	case FILTERITEM_SUBSTRINGS:
		res = substr_search(fitem, as->attr_value);
		break;
	case FILTERITEM_APPROX:
		res = (int) (*fitem->fi_ifp) (fitem, as->attr_value);
		break;
	default:
		res = test_avs(fitem, as->attr_value, fitem->fi_type);
		break;
	}

	if ((res == OK) || (ias == NULLATTR))
		return res;

	if ( check_acl(binddn, ACL_COMPARE, ias->attr_acl, NULLDN) != OK)
		return MAYBE;

	switch (fitem->fi_type) {
	case FILTERITEM_SUBSTRINGS:
		res = substr_search(fitem, ias->attr_value);
		break;
	case FILTERITEM_APPROX:
		res = (int) (*fitem->fi_ifp) (fitem, ias->attr_value);
		break;
	default:
		res = test_avs(fitem, ias->attr_value, fitem->fi_type);
		break;
	}

	return res;
}

static          test_avs(fitem, avs, mode)
register struct filter_item *fitem;
register AV_Sequence avs;
register int    mode;
{

	for (; avs != NULLAV; avs = avs->avseq_next) {
		switch (((int) (*fitem->fi_ifp) (avs->avseq_av.av_struct, fitem->UNAVA.ava_value->av_struct))) {
		case 0:
			return (OK);
		case 1:
			if (mode == FILTERITEM_GREATEROREQUAL)
				return (OK);
			break;
		case -1:
			if (mode == FILTERITEM_LESSOREQUAL)
				return (OK);
			break;
		case 2:
			return (NOTOK);
		default:
			return (MAYBE);
		}
	}
	return (NOTOK);
}


/*
 * SUBSTRING MATCH
 */

static          substr_search(fitem, avs)
register struct filter_item *fitem;
register AV_Sequence avs;
{

	phoneflag = telephone_match(fitem->UNAVA.ava_type->oa_syntax);
	for (; avs != NULLAV; avs = avs->avseq_next)
		if (aux_substr_search(fitem, avs, fitem->UNSUB.fi_sub_match) == OK)
			return (OK);
	return (NOTOK);
}



static          aux_substr_search(fitem, avs, chrmatch)
struct filter_item *fitem;
AV_Sequence     avs;
char            chrmatch[];
{
	register AV_Sequence loopavs;
	register char  *compstr;
	char           *top;
	register char  *temp;
	char           *temp2;
	int             offset;

	compstr = (char *) avs->avseq_av.av_struct;
	top = compstr;
	if (fitem->UNSUB.fi_sub_initial != NULLAV) {
		temp = (char *) fitem->UNSUB.fi_sub_initial->avseq_av.av_struct;
		do {
			if (phoneflag) {
				while (*compstr == ' ' || *compstr == '-')
					compstr++;
				while (*temp == ' ' || *temp == '-')
					temp++;
			}
			if (chrmatch[*compstr++] != *temp++) {
				DLOG(log_dsap, LLOG_DEBUG, ("initial failure (%s, %s)", top, (char *) fitem->UNSUB.fi_sub_initial->avseq_av.av_struct));
				return (NOTOK);
			}
		} while (*temp != '\0');
	}
	for (loopavs = fitem->UNSUB.fi_sub_any; loopavs != NULLAV; loopavs = loopavs->avseq_next, compstr += offset)
		if ((offset = attr_substr(compstr, &loopavs->avseq_av, chrmatch)) == -1) {
			DLOG(log_dsap, LLOG_DEBUG, ("any failure (%s, %s)", top, (char *) loopavs->avseq_av.av_struct));
			return (NOTOK);
		}
	if (fitem->UNSUB.fi_sub_final != NULLAV) {
		temp = (char *) fitem->UNSUB.fi_sub_final->avseq_av.av_struct;
		temp2 = temp;
		while (*++compstr != '\0');	/* NO-OP */

		while (*temp != '\0') {
			if (phoneflag) {
				if (*temp == ' ' || *temp == '-') {
					temp++;
					continue;
				} else
					compstr--;
				while (compstr >= top &&
				       (*compstr == ' ' || *compstr == '-'))
					compstr--;
			} else
				compstr--;
			temp++;
		}

		if (compstr < top) {
			DLOG(log_dsap, LLOG_DEBUG, ("final too long failure (%s,%s)", top, temp2));
			return (NOTOK);
		}
		temp = temp2;
		while (*compstr != '\0') {
			if (phoneflag) {
				while (*compstr == ' ' || *compstr == '-')
					compstr++;
				while (*temp == ' ' || *temp == '-')
					temp++;
			}
			if (chrmatch[*compstr++] != *temp++) {
				/* free (top); */
				DLOG(log_dsap, LLOG_DEBUG, ("final failure (%s, %s)", top, temp2));
				return (NOTOK);
			}
		}
	}
	return (OK);
}

attr_substr(str1, av, chrmatch)
register char  *str1;
AttributeValue  av;
char            chrmatch[];
{
	register char  *str2;
	register int    count;
	char           *top, *top2;
	char            found = 0;

	top = str1;
	top2 = str2 = (char *) av->av_struct;

	while (*str1 != '\0') {
		if (phoneflag) {
			while (*str1 == ' ' || *str1 == '-')
				str1++;
			while (*str2 == ' ' || *str2 == '-')
				str2++;
		}
		if (chrmatch[*str1++] == *str2) {
			str2++;
			found = 1;
			break;
		}
	}

	if (found == 0)
		return (-1);

	for (count = 1; *str2; count++) {
		if (*str1 == '\0')
			return (-1);

		if (phoneflag) {
			while (*str1 == ' ' || *str1 == '-')
				str1++;
			while (*str2 == ' ' || *str2 == '-')
				str2++;
		}
		if (chrmatch[*str1++] != *str2++) {
			/* not found here, but may still be in the string !! */
			str1 -= count;
			str2 = top2;
			while (*str1 != '\0') {
				if (phoneflag) {
					while (*str1 == ' ' || *str1 == '-')
						str1++;
					while (*str2 == ' ' || *str2 == '-')
						str2++;
				}
				if (chrmatch[*str1++] == *str2) {
					str2++;
					break;
				}
			}
			count = 0;	/* for loop ++ will make it 1 !!! */
		}
	}
	return (str1 - top);
}


subtask_refer(arg, local, refer, ismanager, di)
struct ds_search_arg *arg;
struct ds_search_task **local, **refer;
int             ismanager;
struct di_block *di;
{
	/* turn query into a referral */
	struct ds_search_task *new_task;

	new_task = st_alloc();
	new_task->st_save = NULL_ST;
	new_task->st_baseobject = dn_cpy((*local)->st_baseobject);
	new_task->st_originalbase = dn_cpy((*local)->st_originalbase);
	new_task->st_subset = (*local)->st_subset;
	new_task->st_alias = dn_cpy((*local)->st_alias);
	new_task->st_bind = NULLDN;
	new_task->st_saclheader = (*local)->st_saclheader;
	new_task->st_saclrefcount++;
	new_task->st_ftypeheader = (*local)->st_ftypeheader;
	new_task->st_ftyperefcount++;
	new_task->st_baseptr = (*local)->st_baseptr;
	if ((*local)->st_bind != NULLDN) {
		LLOG(log_dsap, LLOG_NOTICE, ("Search consistency problem"));

		/*
		 * Doing a authenticed search, but need to go outside this
		 * DSA
		 */
		/* probably aliases to blame... */
	}
	new_task->st_entryonly = (*local)->st_entryonly;

	if (ismanager) {
		if ((new_task->st_size = arg->sra_common.ca_servicecontrol.svc_sizelimit) == SVC_NOSIZELIMIT)
			new_task->st_size = big_size;
	} else if ((new_task->st_size = MIN(admin_size, arg->sra_common.ca_servicecontrol.svc_sizelimit)) == SVC_NOSIZELIMIT)
		new_task->st_size = admin_size;

	new_task->st_di = di;
	new_task->st_next = *refer;
	*refer = new_task;
}

dsa_search_control(arg, result)
struct ds_search_arg *arg;
struct ds_search_result *result;
{
	extern DN       mydsadn;
	char            buffer[LINESIZE];
	Attr_Sequence   as;
	extern AttributeType at_control;
	int             i;

	if (big_size == 0)
		for (i = NBBY * sizeof big_size - 1; i > 0; i--)
			big_size <<= 1, big_size |= 1;

	if ((arg->sra_eis.eis_allattributes) ||
	    (arg->sra_eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY))
		return FALSE;

	if (arg->sra_eis.eis_select == NULLATTR)
		return FALSE;

	if (arg->sra_eis.eis_select->attr_link != NULLATTR)
		return FALSE;

	if (AttrT_cmp(at_control, arg->sra_eis.eis_select->attr_type) != 0)
		return FALSE;

	if (result->CSR_entries)
		entryinfo_free(result->CSR_entries, 0);

	(void) sprintf(buffer, "%d", big_size - size);

	as = as_comp_alloc();
	as->attr_acl = NULLACL_INFO;
	as->attr_type = at_control;
	as->attr_link = NULLATTR;
	if ((as->attr_value = str2avs(buffer, as->attr_type)) == NULLAV) {
		as_free(as);
		result->CSR_entries = NULLENTRYINFO;
		return FALSE;
	}
	result->CSR_entries = entryinfo_alloc();
	result->CSR_entries->ent_dn = dn_cpy(mydsadn);
	result->CSR_entries->ent_next = NULLENTRYINFO;
	result->CSR_entries->ent_age = (time_t) 0;
	result->CSR_entries->ent_iscopy = TRUE;
	result->CSR_entries->ent_attr = as;

	return TRUE;
}
