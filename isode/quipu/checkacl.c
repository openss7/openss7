/* checkacl.c - routines to check search and list acl's */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/checkacl.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/checkacl.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: checkacl.c,v $
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



#include "quipu/attrvalue.h"
#include "logger.h"
#include "quipu/entry.h"
#include "quipu/ds_search.h"
#include "quipu/util.h"
#include <stdio.h>

extern int 	selector_rank[];
extern LLog	*log_dsap;

static common_prefix_len( a, b )
char	*a;
char	*b;
{
	int	i;

	for ( i = 0; *a && *a++ == *b++; i++ )
		;	/* NULL */

	return( i );
}

/*
 * Keep a list of types that appear in a search filter.  For each type,
 * information is kept on the min length of substring/soundex filters and
 * a running total of the minimum common prefix of <= and >= filters.
 * This information is used later when checking search acls.
 */

ftype_add( l, type, len, inequstr )
Ftypelist	*l;
AttributeType	type;
int		len;
char		*inequstr;
{
	Ftypelist	new, tmp;
	int		i, plen;

	if ( *l == NULLFTL ) {
		*l = (Ftypelist) smalloc( sizeof(Ftype) );
		(*l)->ft_type = type;
		(*l)->ft_len = len;
		(*l)->ft_numstrs = 0;
		(*l)->ft_inequstrs = (char **) 0;
		(*l)->ft_next = NULLFTL;

		if ( inequstr != NULL ) {
			(*l)->ft_inequstrs = (char **) smalloc(sizeof(char *));
			(*l)->ft_numstrs = 1;
			(*l)->ft_inequstrs[0] = inequstr;
		}

		return;
	}

	for ( tmp = *l; tmp != NULLFTL; tmp = tmp->ft_next ) {
		/*
		 * If we found the type, first check to see if the length of
		 * the substring in this filter component is less than the
		 * smallest we've found so far.  If so, update the smallest.
		 * Next, if the component is <= or >= with a string, inequstr
		 * will be set to the value in the filter.  In this case, we
		 * have to go through all previouis values (saved in
		 * ft_inequstrs) and possibly compute a new largest common
		 * prefix length (same number as ft_len).
		 */

		if ( AttrT_cmp( tmp->ft_type, type ) == 0 ) {
			/* substring length shorter? */
			if ( len >= 0 && (len < tmp->ft_len
			    || tmp->ft_len == -1) ) {
				tmp->ft_len = len;
			}

			/* check the >= or <= string prefix */
			if ( inequstr != NULL ) {
				/* record the new value */
				tmp->ft_numstrs++;
				if ( tmp->ft_numstrs > 1 ) {
					tmp->ft_inequstrs = (char **)
					    realloc( (char *) tmp->ft_inequstrs,
					    (unsigned)(tmp->ft_numstrs * sizeof(char *)));
				} else {
					tmp->ft_inequstrs = (char **)
					    smalloc( sizeof(char *) );
				}
				tmp->ft_inequstrs[tmp->ft_numstrs - 1] =
				    inequstr;

				/* see if len needs updating */
				for ( i = 0; i < tmp->ft_numstrs - 1; i++ ) {
					if ( (plen = common_prefix_len(
					    inequstr, tmp->ft_inequstrs[i] ))
					    < tmp->ft_len
					    || tmp->ft_len == -1) {
						tmp->ft_len = plen;
					}
				}
			}
			return;
		}
	}

	new = (Ftypelist) smalloc( sizeof(Ftype) );
	new->ft_type = type;
	new->ft_len = len;
	new->ft_numstrs = 0;
	new->ft_inequstrs = (char **) 0;
	if ( inequstr != NULL ) {
		new->ft_inequstrs = (char **) smalloc(sizeof(char *));
		new->ft_numstrs = 1;
		new->ft_inequstrs[0] = inequstr;
	}
	new->ft_next = (*l)->ft_next;
	(*l) = new;

	return;
}

ftype_free( ft )
Ftypelist	ft;
{
	Ftypelist	next = ft;

	for ( ; next != NULLFTL; ft = next ) {
		next = ft->ft_next;
		if ( ft->ft_inequstrs != NULL )
			free( (char *) ft->ft_inequstrs );
		free( (char *) ft );
	}
}

rc_free( rc )
struct result_count	*rc;
{
	if ( rc->rc_types != NULLTYPEDATA )
		free( (char *) rc->rc_types );
	free( (char *) rc );
}

static oid_in_seq( at, seq )
AttributeType	at;
struct oid_seq	*seq;
{
	for ( ; seq != NULLOIDSEQ; seq = seq->oid_next ) {
		if ( oid_cmp( at->oa_ot.ot_oid, seq->oid_oid ) == 0 ) {
			return( 1 );
		}
		if ( oid_cmp( at->oa_ot.ot_aliasoid, seq->oid_oid ) == 0 ) {
			return( 1 );
		}
	}

	return( 0 );
}

static sacl_match( binddn, selfdn, s )
DN		binddn;
DN		selfdn;
Saclinfo	s;
{
	switch ( s->sac_selector ) {
	case ACL_ENTRY:
		if ( selfdn != NULLDN && dn_cmp( binddn, selfdn ) == OK )
			return( 1 );
		break;

	case ACL_GROUP:
		if ( check_dnseq( s->sac_name, binddn ) == OK )
			return( 1 );
		break;

	case ACL_PREFIX:
		if ( check_dnseq_prefix( s->sac_name, binddn ) == OK )
			return( 1 );
		break;

	case ACL_OTHER:
		return( 1 );

	default:
		/* trouble */
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("sacl_match: unknown type") );
		return( 0 );
	}

	return( 0 );
}

static check_base_sacl( binddn, selfdn, e, local, authtype )
DN			binddn;
DN			selfdn;
Entry			e;
struct ds_search_task	*local;
char			authtype;
{
	AV_Sequence		avs;
	Ftypelist		ft;
	Saclinfo		s, save;

	/* check auth policy allow us to believe binddn */
	if ( e->e_authp != NULLAUTHP ) {
		binddn = (authtype % 3) >= e->e_authp->ap_listandsearch ?
		    binddn : NULLDN;
	}

	/* for each type in the filter */
	for ( ft = (Ftypelist) local->st_ftypes; ft != NULLFTL;
	    ft = ft->ft_next ) {

		/*
		 * find the most restrictive sacl that applies to this
		 * type and who, then check that it allows access.
		 */

		save = NULLSACL;
		for ( avs = e->e_sacl; avs != NULLAV; avs = avs->avseq_next ) {
			s = (Saclinfo) avs->avseq_av.av_struct;

			/* right scope? */
			if ( (s->sac_scope & SACL_BASEOBJECT) == 0 )
				continue;

			/* right type? */
			if ( s->sac_types != NULLOIDSEQ &&
			    oid_in_seq( ft->ft_type, s->sac_types ) == 0 )
				continue;

			/* right who? */
			if ( ! sacl_match( binddn, selfdn, s ) )
				continue;

			/*
			 * We found a sacl that matches.  There may be
			 * others that match.  We want to end up with the
			 * most restrictive one, so save this one if it's
			 * more restrictive than any found so far.
			 */

			/* first match */
			if ( save == NULLSACL ) {
				save = s;

			/* more specific who match */
			} else if ( selector_rank[s->sac_selector] <
			    selector_rank[save->sac_selector] ) {
				save = s;

			/* same who - more specific attribute */
			} else if ( selector_rank[s->sac_selector] ==
			    selector_rank[save->sac_selector]
			    && save->sac_types == NULLOIDSEQ ) {
				save = s;
			}
		}

		/* no sacls applicable to this type */
		if ( save == NULLSACL )
			continue;

		/* we found an applicable sacl. check it allows access */
		if ( save->sac_access == SACL_UNSEARCHABLE )
			return( NOTOK );

		if ( ft->ft_len >= 0 && ft->ft_len < save->sac_minkeylength )
			return( NOTOK );
	}

	return( OK );
}

static struct result_count *make_rc( binddn, selfdn, e, scope, local )
DN			binddn;
DN			selfdn;
Entry			e;
int			scope;
struct ds_search_task	*local;
{
	struct result_count	*rc;
	AV_Sequence		avs;
	Ftypelist		ft;
	Saclinfo		s, save;
	Typedata		td;

	rc = (struct result_count *) smalloc( sizeof(struct result_count) );
	(void) bzero( (char *) rc, sizeof(struct result_count) );
	rc->rc_base = e;

	save = NULLSACL;

	/* 
	 * For each type that appears in the filter, look through the sacls
	 * and find the most restrictive one that applies to this type and
	 * scope.  If an applicable sacl is found, add its restrictions to
	 * the list of restrictions being kept in the result count.
	 */

	for ( ft = (Ftypelist) local->st_ftypes; ft != NULLFTL;
	    ft = ft->ft_next ) {
		/* for each set of sacls */
		for ( avs = e->e_sacl; avs != NULLAV; avs = avs->avseq_next ) {
			s = (Saclinfo) avs->avseq_av.av_struct;

			/* right scope? */
			if ( (s->sac_scope & scope) == 0 )
				continue;

			/* right type? */
			if ( s->sac_types != NULLOIDSEQ &&
			    oid_in_seq( ft->ft_type, s->sac_types ) == 0 )
				continue;

			/* right who? */
			if ( ! sacl_match( binddn, selfdn, s ) )
				continue;

			/*
			 * We found a sacl that matches.  There may be
			 * others that match.  We want to end up with the
			 * most restrictive one, so save this one if it's
			 * more restrictive than any found so far.
			 */

			/* first match */
			if ( save == NULLSACL ) {
				save = s;

			/* more specific who match */
			} else if ( selector_rank[s->sac_selector] <
			    selector_rank[save->sac_selector] ) {
				save = s;

			/* same who - more specific attribute */
			} else if ( selector_rank[s->sac_selector] ==
			    selector_rank[save->sac_selector]
			    && save->sac_types == NULLOIDSEQ ) {
				save = s;
			}
		}

		/* no sacls applicable to this type */
		if ( save == NULLSACL )
			continue;

		/* We have found an applicable sacl. Add it to the rc */
		td = (Typedata) smalloc( sizeof( typedata) );
		td->td_type = ft->ft_type;
		td->td_limit = save->sac_maxresults;
		td->td_nopartial = save->sac_zeroifexceeded;
		td->td_minkey = save->sac_minkeylength;
		td->td_access = save->sac_access;

		rc->rc_numtypes++;
		if ( rc->rc_types == NULLTYPEDATA ) {
			rc->rc_types = td;
		} else {
			rc->rc_types = (Typedata) realloc( (char *) rc->rc_types,
			    (unsigned)(rc->rc_numtypes * sizeof(typedata)) );
			rc->rc_types[rc->rc_numtypes-1] = *td;
			free( (char *) td );
		}
	}

	return( rc );
}

static int srch2sacl_scope[] = {
	SACL_BASEOBJECT,	/* SRA_BASEOBJECT 	*/
	SACL_SINGLELEVEL,	/* SRA_ONELEVEL 	*/
	SACL_SUBTREE		/* SRA_WHOLESUBTREE	*/
};

static entry_rc_cmp( e, rc )
Entry			e;
struct result_count	*rc;
{
	return( e < rc->rc_base ? -1 : e > rc->rc_base ? 1 : 0 );
}

static rc_cmp( a, b )
struct result_count	*a;
struct result_count	*b;
{
	return( a->rc_base < b->rc_base ? -1
	    : a->rc_base > b->rc_base ? 1 : 0 );
}

/*
 * check_one_sacl checks one sacl level to see if it allows access to the
 * given dn.  If it does, OK is returned.  If not, NOTOK is returned.
 * saclerror may be set if the sacl is of the zero-results-if-limit-exceeded
 * type and this entry has put it over the top.  This routine is called
 * repeatedly for each ancestor when checking an entry for a subtree search.
 * It's also called once to check the parent sacl during a single-level
 * search.
 */

check_one_sacl( binddn, selfdn, ancestor, scope, local, saclerror, authtype )
DN			binddn;
DN			selfdn;
Entry			ancestor;
int			scope;
struct ds_search_task	*local;
int			*saclerror;
char			authtype;
{
	struct result_count	*rc;
	Typedata		td;
	Ftypelist		ft;
	int			i;

	/* check auth policy allow us to believe binddn */
	if ( ancestor->e_authp != NULLAUTHP ) {
		binddn = authtype >= ancestor->e_authp->ap_listandsearch ?
		    binddn : NULLDN;
	}

	if ( (rc = (struct result_count *) avl_find( (Avlnode *)local->st_sacls,
	    (caddr_t) ancestor, entry_rc_cmp )) == (struct result_count *) 0 ) {

		/* no running total - make one, possibly a dummy */
		rc = make_rc( binddn, selfdn, ancestor, scope, local );

		(void) avl_insert( (Avlnode **)&local->st_sacls, (caddr_t) rc,
		    rc_cmp, avl_dup_error );
	}

	rc->rc_count++;

	for ( ft = (Ftypelist) local->st_ftypes; ft != NULLFTL;
	    ft = ft->ft_next ) {
		for ( i = 0; i < rc->rc_numtypes; i++ ) {
			if ( AttrT_cmp( rc->rc_types[i].td_type,
			    ft->ft_type ) == 0 )
				break;
		}
		if ( i == rc->rc_numtypes ) {
			continue;
		}

		td = &(rc->rc_types[i]);

		if ( td->td_access == SACL_UNSEARCHABLE ) {
			rc->rc_count--;
			return( NOTOK );
		}

		if ( rc->rc_count > td->td_limit ) {
			if ( td->td_nopartial )
				*saclerror = -1;	/* security error */
			else
				*saclerror = 1;		/* limit exceeded */
			rc->rc_count--;
			return( NOTOK );
		}

		if ( ft->ft_len >= 0 && ft->ft_len < td->td_minkey ) {
			*saclerror = -1;
			rc->rc_count--;
			return( NOTOK );
		}
	}

	return( OK );
}

/*
 * check_ancestor_sacls is called when an entry e has been found that
 * matches a search filter.  First it always checks e's base sacl, if
 * there is one.  Next, for a single level search, it checks e's parent
 * node sacl.  For a subtree search, it checks all of e's ancestors sacls
 * up to and including the base node of the search.
 */

check_ancestor_sacls( binddn, selfdn, e, scope, local, authtype, saclerror )
DN			binddn;
DN			selfdn;
Entry			e;
int			scope;
struct ds_search_task	*local;
char			authtype;
int			*saclerror;
{
	Entry			stop, ancestor;
	extern Entry		database_root;

	if ( manager( binddn ) )
		return( OK );

	scope = srch2sacl_scope[ scope ];	/* convert the scope */

	/* base object sacls are always checked */
	if ( check_base_sacl( binddn, selfdn, e, local, authtype ) == NOTOK )
		return( NOTOK );

	if ( scope == SACL_BASEOBJECT )
		return( OK );

	/* check single level sacl if appropriate */
	if ( scope == SACL_SINGLELEVEL ) {
		return ( check_one_sacl( binddn, selfdn, e->e_parent, scope,
		    local, saclerror, authtype ) );
	}

	/*
	 * It's a subtree search.  Walk back up the tree, checking
	 * ancestor sacls as we go.
	 */

	stop = local->st_baseptr->e_parent;
	for ( ancestor = e; ancestor != database_root && ancestor != stop;
	    ancestor = ancestor->e_parent ) {
		if ( ancestor->e_sacl == NULLAV )
			continue;

		if ( check_one_sacl( binddn, selfdn, ancestor, scope, local,
		    saclerror, authtype ) == NOTOK ) {
			return( NOTOK );
		}
	}

	if ( ancestor == database_root )
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("trouble in check_ancestor"));

	return( OK );
}

check_lacl( binddn, selfdn, avs, scope, sizelimit )
DN		binddn;		/* the dn requesting access 		  */
DN		selfdn;		/* the dn of the entry containing the acl */
AV_Sequence	avs;		/* the acl protecting the entry 	  */
int		scope;		/* scope of acl's we want		  */
int		*sizelimit;	/* size limit on acl we found		  */
{
	Listacl		save, tmp, l;
	AV_Sequence	av;

	*sizelimit = -1;
	tmp = save = NULLLISTACL;
	for ( av = avs; av != NULLAV; av = av->avseq_next ) {
		l = (Listacl) av->avseq_av.av_struct;

		/* make sure this is the right kind */
		if ( l->sac_scope != scope )
			continue;

		tmp = NULLLISTACL;
		switch ( l->sac_selector ) {
		case ACL_ENTRY:
			if ( dn_cmp( binddn, selfdn ) == OK )
				tmp = l;
			break;

		case ACL_GROUP:
			if ( check_dnseq( l->sac_name, binddn ) == OK )
				tmp = l;
			break;

		case ACL_PREFIX:
			if ( check_dnseq_prefix( l->sac_name, binddn ) == OK )
				tmp = l;
			break;

		case ACL_OTHER:
			tmp = l;
			break;
		default:
			/* trouble */
			LLOG( log_dsap, LLOG_EXCEPTIONS,
			    ("check_lacl: unknown type") );
			return( NOTOK );
		}
		if ( tmp == NULLLISTACL )
			continue;

		/* if it's more restrictive than the current one, save it */
		if ( save != NULLLISTACL ) {
			if ( selector_rank[ tmp->sac_selector ]
			    < selector_rank[ save->sac_selector ] )
				save = tmp;
		} else {
			save = tmp;
		}
	}

	if ( save == NULLLISTACL )
		return( OK );	/* no acl applicable - access allowed */

	*sizelimit = save->sac_maxresults;

	if ( *sizelimit == 0 )
		return( NOTOK );
	else
		return( OK );
}
