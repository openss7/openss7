/* turbo_search.c - DSA search of the directory using attribute index */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/turbo_search.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/turbo_search.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: turbo_search.c,v $
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
#include "quipu/entry.h"
#include "quipu/list.h"
#include "quipu/ds_search.h"
#include "quipu/turbo.h"
#include "config.h"

#define new_indexnode() (Index_node *) calloc (1, sizeof (Index_node))

#ifdef TURBO_INDEX

extern LLog    		*log_dsap;
extern int		size;
extern char		qctx;
extern AttributeType	*turbo_index_types;
extern int		turbo_index_num;
extern Avlnode		*subtree_index;
extern Avlnode		*sibling_index;
int			idn_cmp();

static struct search_kid_arg	*g_ska;
static int			g_toplevel;
static int			g_count;
static int			g_stopearly;
static int			g_size_normalizer;

static EntryInfo	*turbo_filterkids();
static EntryInfo	*turbo_item();
static EntryInfo	*turbo_and();
static EntryInfo	*turbo_or();
static EntryInfo	*eis_union();
static void		subtree_refer();

Attr_Sequence	eis_select();
EntryInfo	*filterentry();

optimized_filter( f )
Filter	f;
{
	struct filter_item	*fi;

        switch ( f->flt_type ) {
        case FILTER_ITEM:
		fi = &f->FUITEM;
		switch ( fi->fi_type ) {
		case FILTERITEM_GREATEROREQUAL:
		case FILTERITEM_LESSOREQUAL:
			return( FALSE );
		case FILTERITEM_SUBSTRINGS:
			if ( (fi->UNSUB.fi_sub_initial == NULLAV
			    && fi->UNSUB.fi_sub_final == NULLAV)
			    || turbo_isoptimized( fi->UNSUB.fi_sub_type ) == 0 )
				return( FALSE );
			/* FALL */
		case FILTERITEM_APPROX:
		case FILTERITEM_EQUALITY:
		case FILTERITEM_PRESENT:
			return( turbo_isoptimized( fi->UNAVA.ava_type ) );
		}
        case FILTER_AND:
		for ( f = f->FUFILT; f != NULLFILTER; f = f->flt_next ) {
			if ( optimized_filter( f ) )
				return( TRUE );
		}
                return( FALSE );
        case FILTER_OR:
		for ( f = f->FUFILT; f != NULLFILTER; f = f->flt_next ) {
			if ( !optimized_filter( f ) )
				return( FALSE );
		}
                return( TRUE );
        case FILTER_NOT:
                return( FALSE );
        default:
                LLOG(log_dsap, LLOG_EXCEPTIONS,
		    ("optimized_filter - unknown filter type"));
                return( FALSE );
        }
        /* NOTREACHED */
}

/* ARGSUSED */
static apply_sacl( list, e, ska )
EntryInfo		**list;
Entry			e;
struct search_kid_arg	*ska;
{
	EntryInfo	*p, *prev, *next;
	int		saclerror = 0;

	prev = next = NULLENTRYINFO;
	for ( p = *list; p != NULLENTRYINFO; p = next ) {
		next = p->ent_next;

		if ( check_ancestor_sacls( (*ska->ska_local)->st_bind, NULLDN,
		    p->ent_eptr, ska->ska_arg->sra_subset, *ska->ska_local,
		    ska->ska_authtype, &saclerror ) == NOTOK ) {
			/* zero-results... exceeded. nothing to be returned */
			if ( saclerror < 0 )
				break;

			/* 
			 * otherwise run of the mill sacl size limit
			 * exceeded -- don't return this entry.
			 */

			p->ent_next = NULLENTRYINFO;
			entryinfo_comp_free( p, 0 );

			if ( prev == NULLENTRYINFO ) {
				*list = next;
			} else {
				prev->ent_next = next;
			}

			continue;	/* prev remains the same */
		}

		prev = p;
	}

	ska->ska_saclerror = saclerror;
}

/*
 * turbo_sibling_search - search a sibling index
 */

turbo_sibling_search( e, ska )
Entry			e;
struct search_kid_arg	*ska;
{
	EntryInfo		*list;
	Entry			*tmp;
	DN			dn;
	Index			*pindex;

	*ska->ska_einfo = NULLENTRYINFO;

	if ( e->e_leaf )
		return;

	if ( e->e_children == NULLAVL ) {
		search_refer( ska->ska_arg, e, ska->ska_local, ska->ska_refer,
		    ska->ska_ismanager );
		return;
	}

	dn = get_copy_dn( e );
	if ( (pindex = get_sibling_index( dn )) == NULLINDEX ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("Cannot find sibling index") );
		dn_free( dn );
		return;
	}
	dn_free( dn );

	g_size_normalizer = 1;
	list = turbo_filterkids( e, ska->ska_arg->sra_filter, ska, pindex, 1 );

	apply_sacl( &list, e, ska );

	/* security error coming back */
	if ( ska->ska_saclerror < 0 ) {
		entryinfo_free( list, 0 );
		return;
	}

	if ( *ska->ska_einfo == NULLENTRYINFO )
		*ska->ska_einfo = list;
	else if ( list != NULLENTRYINFO )
		entryinfo_append( *ska->ska_einfo, list );

	/* sizelimit already exceeded */
	if ( ska->ska_saclerror > 0 )
		return;

	if ( ska->ska_arg->sra_searchaliases && pindex->i_nonlocalaliases
	    != (Entry *) 0 ) {
		for ( tmp = pindex->i_nonlocalaliases; *tmp; tmp++ )
			(void) do_alias( ska->ska_arg, *tmp, ska->ska_local );
	}
}

/*
 * turbo_subtree_search - search a subtree index
 */

turbo_subtree_search( e, ska )
Entry			e;
struct search_kid_arg	*ska;
{
	EntryInfo	*list;
	Entry		*tmp;
	DN		dn;
	Index		*pindex;

	dn = get_copy_dn( e );
	if ( (pindex = get_subtree_index( dn )) == NULLINDEX ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("Cannot find subtree index") );
		dn_free( dn );
		return;
	}
	dn_free( dn );

	g_size_normalizer = 1;
	list = turbo_filterkids( e, ska->ska_arg->sra_filter, ska, pindex, 1 );

	apply_sacl( &list, e, ska );

	/* security error coming back */
	if ( ska->ska_saclerror < 0 ) {
		entryinfo_free( list, 0 );
		return;
	}

	if ( *ska->ska_einfo == NULLENTRYINFO )
		*ska->ska_einfo = list;
	else if ( list != NULLENTRYINFO )
		entryinfo_append( *ska->ska_einfo, list );

	/* sizelimit already exceeded */
	if ( ska->ska_saclerror > 0 )
		return;

	/*
	 * at this point, anything held locally below this point has been
	 * searched.  we now search through the nonleaf children recursively
	 * for one whose children are not held locally, referring any that
	 * we find.  next, we search through the list of nonlocal aliases
	 * searching for one that matches (if dereferencing is allowed).
	 */

	if ( pindex->i_nonleafkids != (Entry *) 0 )
		subtree_refer( pindex, ska );

	if ( ska->ska_arg->sra_searchaliases && pindex->i_nonlocalaliases
	    != (Entry *) 0 ) {
		for ( tmp = pindex->i_nonlocalaliases; *tmp; tmp++ ) {
			int	i;

			i = th_prefix( (*ska->ska_local)->st_originalbase,
			    (*tmp)->e_alias );
			if ( i > 0 ) {
				(void) do_alias( ska->ska_arg, *tmp,
				    ska->ska_local );
			}
		}
	}
}

static void subtree_refer( pindex, ska )
Index			*pindex;
struct search_kid_arg	*ska;
{
	Entry	*tmp;

	for ( tmp = pindex->i_nonleafkids; *tmp != NULLENTRY; tmp++ ) {
		if ( ((*tmp)->e_children != NULLAVL
		    && (*tmp)->e_allchildrenpresent == FALSE)
		    || (*tmp)->e_children == NULLAVL ) {
			if ( check_acl( (*ska->ska_local)->st_bind, ACL_READ,
			    (*tmp)->e_acl->ac_child, NULLDN ) == OK ) {
                                search_refer( ska->ska_arg, *tmp, 
				    ska->ska_local, ska->ska_refer, 
				    ska->ska_ismanager);
			}
		}
	}
}

static EntryInfo *turbo_filterkids( e, f, ska, pindex, toplevel )
Entry			e;
Filter			f;
struct search_kid_arg	*ska;
Index			*pindex;
int			toplevel;
{

	if ( e == NULLENTRY || f == NULLFILTER ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("bad turbo_filterkids pars") );
		return( NULLENTRYINFO );
	}

	switch ( f->flt_type ) {
	case FILTER_ITEM:
		return( turbo_item( e, &f->FUITEM, ska, pindex, toplevel ) );
	case FILTER_AND:
		return( turbo_and( e, f->FUFILT, ska, pindex, toplevel ) );
	case FILTER_OR:
		return( turbo_or( e, f->FUFILT, ska, pindex, toplevel ) );
	default:
		LLOG( log_dsap, LLOG_EXCEPTIONS,
		    ("turbo_filterkids: cannot filter type (%d)",
		    f->flt_type) );
		return( NULLENTRYINFO );
	}
	/* NOT REACHED */
}

static eis_merge( ei, eilist, toplevel )
EntryInfo	*ei;
EntryInfo	**eilist;
int		toplevel;
{
	int		cmp;
	EntryInfo	*eitmp;

	/* 
	 * add this entry to the result list. the list is
	 * ordered here to make it easier to do and's and 
	 * or's later on.
	 */

	if ( *eilist == NULLENTRYINFO ) {
		*eilist = ei;
		if ( toplevel )
			size--;
		return( OK );
	} else if ((cmp = dn_order_cmp(ei->ent_dn, (*eilist)->ent_dn)) == 0) {
		entryinfo_free(ei, 0);
		return( NOTOK );
	} else if ( cmp < 0 ) {
		ei->ent_next = *eilist;
		*eilist = ei;
		if ( toplevel )
			size--;
		return( OK );
	}

	eitmp = *eilist;
	cmp = -1;
	while ( eitmp->ent_next != NULLENTRYINFO ) {
		if ((cmp = dn_order_cmp(ei->ent_dn, eitmp->ent_next->ent_dn))
		    <= 0)
			break;
		eitmp = eitmp->ent_next;
	}

	if ( cmp == 0 ) {
		entryinfo_free(ei, 0);
		return( NOTOK );
	}

	ei->ent_next = eitmp->ent_next;
	eitmp->ent_next = ei;

	if ( toplevel )
		size--;

	return( OK );
}

static entry_collect( node, eilist )
Index_node	*node;
EntryInfo	**eilist;
{
	int		i;
	EntryInfo	*ei;
	int		saclerror;

	for ( i = 0; i < node->in_num; i++ ) {
		if ( g_ska->ska_arg->sra_searchaliases &&
		    node->in_entries[i]->e_alias != NULLDN )
			continue;

		if ( (ei = filterentry( g_ska->ska_arg, node->in_entries[i],
		    (*g_ska->ska_local)->st_bind, g_ska->ska_authtype,
		    &saclerror, *g_ska->ska_local, 0 )) == NULLENTRYINFO ) {
			continue;
		}

		/*
		 * size will have been decremented by filterentry, so
		 * we need to undo this here if the entry was a duplicate,
		 * or we are not at the top level.
		 */

		if ( eis_merge( ei, eilist, 0 ) != OK || g_toplevel == 0 )
			size++;

		if ( size <= 0 ) {
			size--;
			return( NOTOK );
		}
	}
	return( OK );
}

static build_indexnode( node, bignode )
Index_node	*node;
Index_node	*bignode;
{
	int	i, j;
	int	low, mid, high;
	int	dup;
	Entry	tmp1, tmp2;

	for (i = 0; i < node->in_num; i++) {
		tmp1 = node->in_entries[i];

		if ( bignode->in_num > 20 ) {
			low = 0;
			high = bignode->in_num - 1;
			mid = (low + high) / 2;
			while ( low < high ) {
				if ( bignode->in_entries[mid] < tmp1 )
					low = mid + 1;
				else if ( bignode->in_entries[mid] > tmp1 )
					high = mid - 1;
				else
					break;  /* found a duplicate */

				mid = (low + high) / 2;
			}

			if ( bignode->in_entries[mid] == tmp1 )
				continue;
			else if ( bignode->in_entries[mid] < tmp1 )
				mid++;
		} else {
			dup = 0;
			for ( mid = 0; mid < bignode->in_num; mid++ ) {
				if ( bignode->in_entries[mid] == tmp1 ) {
					dup = 1;
					break;
				}
				if ( bignode->in_entries[mid] > tmp1 )
					break;
			}
			if ( dup )
				continue;
		}

		/*
		 * Realloc double the space we got last time.  This may waste
		 * some space in the index, but it speeds things up, and cuts
		 * down on fragmentation.
		 */

		if (bignode->in_num >= bignode->in_max) {
			if (bignode->in_max == 0) {
				bignode->in_max = 1;
				bignode->in_entries =
				    (struct entry **) malloc( (unsigned)
				    sizeof(struct entry *) );
				bignode->in_entries[0] = tmp1;
			} else {
				bignode->in_max *= 2;
				bignode->in_entries =
				    (struct entry **) realloc((char *)
				    bignode->in_entries, (unsigned)
				    (sizeof(struct entry *) * bignode->in_max));
			}
		}
		bignode->in_num++;

		tmp1 = node->in_entries[i];
		for (j = mid; j < bignode->in_num; j++) {
			tmp2 = bignode->in_entries[j];
			bignode->in_entries[j] = tmp1;
			tmp1 = tmp2;
		}

		if (g_stopearly && --g_count == 0) {
			return(NOTOK);
		}
	}

	return(OK);
}

/* ARGSUSED */
static EntryInfo *turbo_item( e, f, ska, pindex, toplevel )
Entry			e;
struct filter_item	*f;
struct search_kid_arg	*ska;
Index			*pindex;
int			toplevel;
{
	int		i;
	int		len;
	int		phoneflag;
	Index_node	*node;
	EntryInfo	*eilist;
	Avlnode		*theindex;
	char		*thestring;
	char		*word, *code, *small;
	char		*next_word(), *first_word(), *strrev();
	int		index_soundex_cmp(), index_soundex_prefix();
	int		substring_prefix_cmp(), substring_prefix_case_cmp();
	int		substring_prefix_tel_cmp();
	int		indexav_cmp(), build_indexnode();

	if ( pindex == NULLINDEX ) {
		return( NULLENTRYINFO );
	}

	for ( i = 0; i < turbo_index_num; i++ ) {
		if ( AttrT_cmp( pindex[ i ].i_attr, f->UNAVA.ava_type )
		    == 0 )
			break;
	}

	if ( i == turbo_index_num ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("can't find index") );
		return( NULLENTRYINFO );
	}

	eilist = NULLENTRYINFO;
	g_ska = ska;
	switch ( f->fi_type ) {
	case FILTERITEM_EQUALITY:
		node = (Index_node *) avl_find( pindex[ i ].i_root,
		    (caddr_t) f->UNAVA.ava_value, indexav_cmp );

		if ( node == ((Index_node *) 0) )
			break;

		g_toplevel = toplevel;
		(void) entry_collect( node, &eilist );
		break;

	case FILTERITEM_APPROX:
		g_toplevel = 0;
		g_count = 0;
		small = NULL;
		for ( word = first_word((char *) f->UNAVA.ava_value->av_struct);
		    word; word = next_word( word ) ) {
			g_count++;
			code = NULL;
			soundex( word, &code );

			if (small == NULL || (int)strlen(code) > (int)strlen(small))
				small = code;
			else
				free(code);
		}

		if (small == NULL)
			break;
#ifdef SOUNDEX_PREFIX
		/*
		 * now traverse the index (smartly) and build a giant
		 * Index_node to be used below
		 */

		node = new_indexnode();
		g_stopearly = 0;
		(void) avl_prefixapply(pindex[i].i_sroot,
		    (caddr_t) small, build_indexnode, (caddr_t) node,
		    index_soundex_prefix, (caddr_t)strlen(small), NOTOK);
#else
		node = (Index_node *) avl_find( pindex[ i ].i_sroot,
		    (caddr_t) small, index_soundex_cmp );
#endif

		/* we found nothing */
		if (node->in_num == 0) {
			free((char *) node);
			break;
		}

		/*
		 * now we build the result list by applying the filter
		 * to the node we found above.
		 */

		g_toplevel = toplevel;
		(void) entry_collect(node, &eilist);

#ifdef SOUNDEX_PREFIX
		free((char *) node->in_entries);
		free((char *) node);
#endif
		break;

	case FILTERITEM_SUBSTRINGS:
		if (f->UNSUB.fi_sub_initial == NULLAV &&
		    f->UNSUB.fi_sub_final == NULLAV) {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("turbo_item: non-optimized substring filter"));
			break;
		}

		phoneflag = telephone_match(f->UNSUB.fi_sub_type->oa_syntax);

		/* no initial substring so use final */
		if (f->UNSUB.fi_sub_initial == NULLAV) {
			theindex = pindex[i].i_rroot;
			thestring =
			    strrev(f->UNSUB.fi_sub_final->avseq_av.av_struct);

		/* no final substring so use initial */
		} else if (f->UNSUB.fi_sub_final == NULLAV) {
			theindex = pindex[i].i_root;
			thestring =
			    strdup(f->UNSUB.fi_sub_initial->avseq_av.av_struct);

		/* otherwise, use whichever is longest */
		} else {
			int	flen, ilen;

			if (phoneflag) {
				flen = telstrlen(f->UNSUB.fi_sub_final->avseq_av.av_struct);
				ilen = telstrlen(f->UNSUB.fi_sub_initial->avseq_av.av_struct);
			} else {
				flen = strlen(f->UNSUB.fi_sub_final->avseq_av.av_struct);
				ilen = strlen(f->UNSUB.fi_sub_initial->avseq_av.av_struct);
			}
			thestring = (flen > ilen ?
			    strrev(f->UNSUB.fi_sub_final->avseq_av.av_struct) :
			    strdup(f->UNSUB.fi_sub_initial->avseq_av.av_struct));
			theindex = (flen > ilen ? pindex[i].i_rroot :
			    pindex[i].i_root);
		}
		if (phoneflag)
			len = telstrlen(thestring);
		else
			len = strlen(thestring);

		node = new_indexnode();
		g_stopearly = toplevel;	/* signifies top level or OR filter */
		g_count = size * g_size_normalizer;
		if (case_exact_match(f->UNSUB.fi_sub_type->oa_syntax)) {
			if (phoneflag)
				(void) avl_prefixapply(theindex, thestring,
				    build_indexnode, (caddr_t) node,
				    substring_prefix_tel_cmp, (caddr_t)len, 
				    NOTOK);
			else
				(void) avl_prefixapply(theindex, thestring,
				    build_indexnode, (caddr_t) node,
				    substring_prefix_cmp, (caddr_t)len, NOTOK);
		} else {
			(void) avl_prefixapply(theindex, thestring,
			    build_indexnode, (caddr_t) node,
			    substring_prefix_case_cmp, (caddr_t)len, NOTOK);
		}

		if (node->in_num == 0) {
			free((char *) node);
			break;
		}

		g_toplevel = (toplevel == 1); /* > 1 => or search */
		(void) entry_collect(node, &eilist);

		free((char *) node->in_entries);
		free((char *) node);
		free(thestring);
		break;

	case FILTERITEM_PRESENT:
		g_toplevel = toplevel;
		(void) avl_apply( pindex[ i ].i_root, entry_collect,
		    (caddr_t) &eilist, NOTOK, AVL_INORDER );
		break;

	default:	/* handle other cases some day */
		LLOG( log_dsap, LLOG_EXCEPTIONS,
		    ("turbo_item: filter type %d not supported\n",
		    f->fi_type) );
		break;
	}

	return( eilist );
}

/* ARGSUSED */
static EntryInfo *turbo_and( e, f, ska, pindex, toplevel )
Entry			e;
Filter			f;
struct search_kid_arg	*ska;
Index			*pindex;
int			toplevel;
{
	for ( ; f != NULLFILTER; f = f->flt_next )
		if ( optimized_filter( f ) )
			break;

	if ( f == NULLFILTER ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS,
		    ("turbo_and - filter has no optimized component") );
		return( NULLENTRYINFO );
	}

	/*
	 * the level below takes care of making sure that the entries
	 * selected match every component of the filter, not just this
	 * one.
	 */

	return( turbo_filterkids( e, f, ska, pindex, 1 ) );
}

static EntryInfo *turbo_or( e, f, ska, pindex, toplevel )
Entry			e;
Filter			f;
struct search_kid_arg	*ska;
Index			*pindex;
int			toplevel;
{
	EntryInfo	*result = NULLENTRYINFO;
	EntryInfo	*tmp;
	Filter		nextf;

	if (toplevel)
		toplevel = 2;
	g_size_normalizer = 1;
	nextf = f;
	while ( nextf != NULLFILTER ) {
		g_size_normalizer++;
		tmp = turbo_filterkids( e, nextf, ska, pindex, toplevel );
		if ( tmp != NULLENTRYINFO )
			result = eis_union( result, tmp, toplevel );
		if ( size <= 0 )
			break;
		nextf = nextf->flt_next;
	}

	return( result );
}

static EntryInfo *eis_union( a, b, toplevel )
EntryInfo	*a;
EntryInfo	*b;
int		toplevel;
{
	EntryInfo	*result, *rtail;
	EntryInfo	*next;

	result = NULLENTRYINFO;
	if ( a == NULLENTRYINFO ) {
		while ( b != NULLENTRYINFO ) {
			next = b->ent_next;
			if ( result == NULLENTRYINFO ) {
				result = b;
				rtail = result;
			} else {
				rtail->ent_next = b;
				rtail = rtail->ent_next;
			}
			b = next;
			if ( toplevel && --size == 0 ) {
				size--;
				break;
			}
		}

		entryinfo_free(b, 0);

		if ( rtail != NULLENTRYINFO )
			rtail->ent_next = NULLENTRYINFO;

		return( result );
	}

	if ( b == NULLENTRYINFO ) {
		while ( a != NULLENTRYINFO ) {
			next = a->ent_next;
			if ( result == NULLENTRYINFO ) {
				result = a;
				rtail = result;
			} else {
				rtail->ent_next = a;
				rtail = rtail->ent_next;
			}
			a = next;
			if ( toplevel && --size == 0 ) {
				size--;
				break;
			}
		}

		entryinfo_free(a, 0);

		if ( rtail != NULLENTRYINFO )
			rtail->ent_next = NULLENTRYINFO;

		return( result );
	}

	result = a;
	while (b != NULLENTRYINFO) {
		next = b->ent_next;
		b->ent_next = NULLENTRYINFO;
		(void) eis_merge(b, &result, toplevel);
		b = next;

		if (toplevel && size <= 0)
			break;
	}

	entryinfo_free(b, 0);

	return( result );
}

#endif /* TURBO_INDEX */
