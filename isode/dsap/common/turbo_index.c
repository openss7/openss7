/* turbo_index.c */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/turbo_index.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/turbo_index.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: turbo_index.c,v $
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


#include <stdio.h>
#include "quipu/config.h"
#include "quipu/attr.h"
#include "quipu/entry.h"
#include "quipu/turbo.h"
#include "logger.h"
#include "quipu/malloc.h"

#ifdef TURBO_INDEX

extern LLog * log_dsap;

AttributeType	*turbo_index_types;	/* array of attributes to optimize */
int		turbo_index_num;	/* number of attributes to optimize */
Avlnode		*subtree_index;		/* array of subtree indexes */
Avlnode		*sibling_index;		/* array of sibling indexes */
int		optimized_only;		/* only allow indexed searches */

char *strrev( s )
char	*s;
{
	char	*start, *rev, *rsave;
	int	len;

	len = strlen( start = s );
	rsave = rev = malloc( (unsigned) (len + 1) );
	for ( s += len; s != start; *rev++ = *--s )
		;	/* NULL */
	*rev = '\0';
	return( rsave );
}

static index_cmp( a, b )
Index_node	*a;
Index_node	*b;
{
	return( AttrV_cmp( (AttributeValue) a->in_value,
	    (AttributeValue) b->in_value ) );
}

static sindex_cmp( a, b )
Index_node	*a;
Index_node	*b;
{
	return( strcmp( (char *) a->in_value, (char *) b->in_value ) );
}

index_soundex_cmp( code, node )
char		*code;
Index_node	*node;
{
	return( strcmp( code, (char *) node->in_value ) );
}

index_soundex_prefix( code, node, len )
char		*code;
Index_node	*node;
int		len;
{
	return( strncmp( code, (char *) node->in_value, len ) );
}

substring_prefix_cmp( val, node, len )
char		*val;
Index_node	*node;
int		len;
{
	return(strncmp(val,
	    (char *) (((AttributeValue) node->in_value)->av_struct), len));
}

substring_prefix_tel_cmp( val, node, len )
char		*val;
Index_node	*node;
int		len;
{
	return(telncmp(val,
	    (char *) (((AttributeValue) node->in_value)->av_struct), len));
}

substring_prefix_case_cmp( val, node, len )
char		*val;
Index_node	*node;
int		len;
{
	return(lexnequ(val,
	    (char *) (((AttributeValue) node->in_value)->av_struct), len));
}

indexav_cmp( av, node )
AttributeValue	av;
Index_node	*node;
{
	return( AttrV_cmp( av, (AttributeValue) node->in_value ) );
}

Index_node *new_indexnode()
{
	Index_node	*new;

	new = (Index_node *) malloc( sizeof(Index_node) );
	new->in_value = (caddr_t) NULLAttrV;
	new->in_entries = (Entry *) 0;
	new->in_num = 0;
	new->in_max = 0;

	return( new );
}

static index_dup( node, dup )
Index_node	*node;
Index_node	*dup;
{
	int	j;
	int	low, mid, high;
	Entry	tmp1, tmp2;

	/* 
	 * Check for duplicates.  If there are over 20 elements we do
	 * a binary search, otherwise a simple linear one.  This is just
	 * a guess.  It seems to work pretty well, though.
	 */

	tmp1 = dup->in_entries[0];

	if ( node->in_num > 20 ) {
		low = 0;
		high = node->in_num - 1;
		mid = (low + high) / 2;
		while ( low < high ) {
			if ( node->in_entries[mid] < tmp1 )
				low = mid + 1;
			else if ( node->in_entries[mid] > tmp1 )
				high = mid - 1;
			else
				break;	/* found a duplicates */

			mid = (low + high) / 2;
		}

		if ( node->in_entries[mid] == tmp1 )
			return( NOTOK );
		else if ( node->in_entries[mid] < tmp1 )
			mid++;
	} else {
		for ( mid = 0; mid < node->in_num; mid++ ) {
			if ( node->in_entries[mid] == tmp1 )
				return( NOTOK );
			if ( node->in_entries[mid] > tmp1 )
				break;
		}
	}

	/*
	 * Realloc double the space we got last time.  This may waste
	 * some space in the index, but it speeds things up, works
	 * around the QUIPU_MALLOC problem, and cuts down on fragmentation.
	 */

	if (node->in_num >= node->in_max) {
		if (node->in_max == 0)
			node->in_max = 1;
		else
			node->in_max *= 2;
		node->in_entries =
		    (struct entry **) realloc((char *)node->in_entries, 
		    (unsigned) (sizeof(struct entry *) * node->in_max));
	}
	node->in_num++;

	tmp1 = dup->in_entries[0];
	for (j = mid; j < node->in_num; j++) {
		tmp2 = node->in_entries[j];
		node->in_entries[j] = tmp1;
		tmp1 = tmp2;
	}

	return( NOTOK );
}

static indexav_free( node )
Index_node	*node;
{
	AttrV_free( (AttributeValue) node->in_value );
	free( (char *) node->in_entries );
	free( (char *) node );
}

static soundex_free( node )
Index_node	*node;
{
	free( (char *) node->in_value );
	free( (char *) node->in_entries );
	free( (char *) node );
}

static index_free( pindex )
Index	*pindex;
{
	dn_free( pindex->i_dn );
	(void) avl_free( pindex->i_root, indexav_free );
	(void) avl_free( pindex->i_sroot, soundex_free );
	free( (char *) pindex );
}

/* ARGSUSED */
static i_dup( a )
Index	*a;
{
	return( NOTOK );
}

static i_cmp( a, b )
Index	*a;
Index	*b;
{
	return( dn_order_cmp( a->i_dn, b->i_dn ) );
}

idn_cmp( a, b )
DN	a;
Index	*b;
{
	return( dn_order_cmp( a, b->i_dn ) );
}

/*
 * th_prefix - returns the following:
 *	-2	=>	b is an immediate child of a
 *      -1      =>      a is some other prefix of b
 *      0       =>      a and b are equal
 *      1       =>      b is a prefix of a
 *      2       =>      neither a nor b is a prefix of the other
 */

th_prefix( a, b )
DN      a;
DN      b;
{
        for ( ; a && b; a = a->dn_parent, b = b->dn_parent )
                if ( dn_comp_cmp( a, b ) == NOTOK )
                        return( 2 );    /* neither is prefix */

        if ( a == NULLDN && b == NULLDN )
                return( 0 );            /* they are equal */
        else if ( a == NULLDN && b->dn_parent == NULLDN )
                return( -2 );           /* b is a child is a */
        else if ( a == NULLDN )
                return( -1 );           /* a is a prefix of b */
        else
                return( 1 );            /* b is a prefix of a */
}

static Index *new_index( dn )
DN	dn;
{
	Index	*pindex;
	int	i;

	pindex = (Index *) malloc( (unsigned) (sizeof(Index) * turbo_index_num ));

	for ( i = 0; i < turbo_index_num; i++ ) {
		pindex[ i ].i_attr = turbo_index_types[ i ];
		pindex[ i ].i_count = 0;
		pindex[ i ].i_rcount = 0;
		pindex[ i ].i_scount = 0;
		pindex[ i ].i_root = NULLAVL;
		pindex[ i ].i_rroot = NULLAVL;
		pindex[ i ].i_sroot = NULLAVL;
		pindex[ i ].i_nonleafkids = (struct entry **) 0;
		pindex[ i ].i_nonlocalaliases = (struct entry **) 0;
		pindex[ i ].i_dn = dn_cpy( dn );
	}

	return( pindex );
}

#ifdef notdef

/* ARGSUSED */
static print_soundex_node( n, ps )
Index_node	*n;
int		ps;
{
	int	i;

	(void) printf( "\t(%s)\n",n->in_value );
	for ( i = 0; i < n->in_num; i++ )
		(void) printf( "\t\t%s\n",
		    n->in_entries[i]->e_name->rdn_av.av_struct);
	return( OK );
}

#endif /* not used */

/*
 * add_nonlocalalias - add entry e to the list of nonlocal aliases
 * kept with index index.
 */

static add_nonlocalalias( e, pindex )
Index		*pindex;
struct entry	*e;
{
	struct entry	**tmp;
	int		i;

	if ( pindex == NULLINDEX )
		return;

	if ( pindex->i_nonlocalaliases == (struct entry **) 0 ) {
		pindex->i_nonlocalaliases = (struct entry **) malloc(
		    sizeof(struct entry *) * 2 );
		pindex->i_nonlocalaliases[ 0 ] = (struct entry *) 0;
	}

	/* first, check for duplicates */
	for ( i = 0, tmp = pindex->i_nonlocalaliases;
	    *tmp != (struct entry *) 0; 
	    tmp++, i++ ) {
		if ( *tmp == e )
			return;
	}

	pindex->i_nonlocalaliases = (struct entry **) realloc(
	    (char *)pindex->i_nonlocalaliases, (unsigned) sizeof(struct entry *) * (i + 2) );

	pindex->i_nonlocalaliases[ i ] = e;
	pindex->i_nonlocalaliases[ i + 1 ] = NULLENTRY;
}

/* 
 * addnonleafkids - add entry e to the list of nonlocal kids kept
 * in index index.
 */

static add_nonleafkid( e, pindex )
Index		*pindex;
struct entry	*e;
{
	struct entry	**tmp;
	int		i;

	if ( pindex == NULLINDEX )
		return;

	if ( pindex->i_nonleafkids == (struct entry **) 0 ) {
		pindex->i_nonleafkids = (struct entry **) malloc(sizeof(Entry));
		pindex->i_nonleafkids[ 0 ] = (Entry) 0;
	}

	/* first, check for duplicates */
	for ( i = 0, tmp = pindex->i_nonleafkids; *tmp != NULLENTRY;
	    tmp++, i++ ) {
		if ( *tmp == e ) {
			return;
		}
	}

	pindex->i_nonleafkids = (struct entry **) realloc(
	    (char *) pindex->i_nonleafkids,
	    (unsigned) (sizeof(struct entry *) * (i + 2)) );

	pindex->i_nonleafkids[ i ] = e;
	pindex->i_nonleafkids[ i + 1 ] = NULLENTRY;
}

/*
 * delete_nonleafkid - delete a reference to nonleaf child entry e
 * in index index.
 */

static delete_nonleafkid( e, pindex )
struct entry	*e;
Index		*pindex;
{
	int		i, j;
	struct entry	**tmp;

	if ( pindex->i_nonleafkids == (struct entry **) 0 ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Index has no non-leaf entries"));
		return;
	}

	for ( i = 0, tmp = pindex->i_nonleafkids; *tmp; tmp++, i++ )
		if ( *tmp == e ) break;

	if ( *tmp == NULLENTRY ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Cannot find non-leaf entry"));
		return;
	}

	for ( j = i + 1; pindex->i_nonleafkids[ j ]; j++ )
		pindex->i_nonleafkids[ j - 1 ] = pindex->i_nonleafkids[ j ];
	pindex->i_nonleafkids[ j - 1 ] = NULLENTRY;

	return;
}

/*
 * delete_nonlocalalias - delete a reference to nonlocal alias entry e
 * in index index.
 */

static delete_nonlocalalias( e, pindex )
struct entry	*e;
Index		*pindex;
{
	int		i, j;
	struct entry	**tmp;

	if ( pindex->i_nonlocalaliases == (struct entry **) 0 ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("index has no non-local aliases"));
		return;
	}

	for ( i = 0, tmp = pindex->i_nonlocalaliases; *tmp; tmp++, i++ )
		if ( *tmp == e ) break;

	if ( *tmp == (struct entry *) 0 ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Cannot find non-local alias"));
		return;
	}

	for ( j = i + 1; pindex->i_nonlocalaliases[ j ]; j++ )
		pindex->i_nonlocalaliases[ j - 1 ] =
		    pindex->i_nonlocalaliases[ j ];
	pindex->i_nonlocalaliases[ j - 1 ] = NULLENTRY;

	return;
}

/*
 * turbo_attr_insert -- mark entry e as having attribute value val in
 * index for attribute type attr.
 */

static turbo_attr_insert( pindex, e, attr, values )
Index		*pindex;
Entry		e;
AttributeType	attr;
AV_Sequence	values;
{
	int		i;
	int		substr;
	AV_Sequence	av;
	AttributeValue	save;
	Index_node	*imem;
	char		*word, *code, *savestr;
	char		*first_word(), *next_word();
	IFP		approxfn();
	int		soundex_match();

	/* find the appropriate index */
	for ( i = 0; i < turbo_index_num; i++ )
		if ( AttrT_cmp( pindex[ i ].i_attr, attr ) == 0 )
			break;

	if ( i == turbo_index_num ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("turbo_attr_insert: cannot find optimized attribute") );
		return;
	}

	substr = sub_string( attr->oa_syntax );
	/* insert all values */
	for ( av = values; av != NULLAV; av = av->avseq_next ) {
		imem = (Index_node *) malloc( sizeof(Index_node) );
		imem->in_value = (caddr_t) AttrV_cpy( &av->avseq_av );
		imem->in_entries = (struct entry **) malloc( sizeof(struct
		    entry *) );
		imem->in_entries[ 0 ] = (struct entry *) e;
		imem->in_num = 1;
		imem->in_max = 1;

		/*
		 * Now we insert the entry into the appropriate index.
		 * If the attribute has a soundex approximate matching
		 * function, we insert the entry into the appropriate
		 * soundex index for that attribute.
		 */

		/* a return of OK means it was the first one inserted */
		if ( avl_insert( &pindex[ i ].i_root, (caddr_t) imem, index_cmp,
		    index_dup ) == OK ) {
			pindex[ i ].i_count++;
			imem = NULLINDEXNODE;

		} else if ( substr ) {
			save = AttrV_cpy( &av->avseq_av );
			savestr = (char *) save->av_struct;
			save->av_struct = strrev( savestr );
			AttrV_free( (AttributeValue) imem->in_value );
			free( savestr );
			imem->in_value = (caddr_t) save;
		} else {
			AttrV_free( (AttributeValue) imem->in_value );
			free( (char *) imem->in_entries );
			free( (char *) imem );
		}
		if ( substr && imem == NULLINDEXNODE ) {
			imem = (Index_node *) malloc( sizeof(Index_node) );

			save = AttrV_cpy( &av->avseq_av );
			savestr = (char *) save->av_struct;
			save->av_struct = strrev( savestr );
			free( savestr );
			imem->in_value = (caddr_t) save;
			imem->in_entries = (struct entry **) malloc(
			    sizeof(struct entry *) );
			imem->in_entries[ 0 ] = (struct entry *) e;
			imem->in_num = 1;
			imem->in_max = 1;
		}

		/* insert into the reverse index, if appropriate */
		if ( substr ) {
			if ( avl_insert( &pindex[ i ].i_rroot, (caddr_t) imem,
			    index_cmp, index_dup ) == OK ) {
				pindex[ i ].i_rcount++;
				imem = NULLINDEXNODE;

			} else {
				AttrV_free( (AttributeValue) imem->in_value );
				free( (char *) imem->in_entries );
				free( (char *) imem );
			}
		}

		if ( approxfn( attr->oa_syntax ) != soundex_match )
			continue;

		for ( word = first_word((char *) av->avseq_av.av_struct);
		    word; word = next_word( word ) ) {
			code = NULL;
			soundex( word, &code );
			imem = (Index_node *) malloc( sizeof(Index_node) );
			imem->in_value = (caddr_t) code;
			imem->in_entries = (struct entry **) malloc(
			    sizeof(struct entry *) );
			imem->in_entries[ 0 ] = (struct entry *) e;
			imem->in_num = 1;
			imem->in_max = 1;

			if ( avl_insert( &pindex[i].i_sroot, (caddr_t) imem, sindex_cmp,
			    index_dup ) == OK ) {
				pindex[ i ].i_scount++;
			} else {
				free( (char *) imem->in_value );
				free( (char *) imem->in_entries );
				free( (char *) imem );
			}
		}
	}
}

/*
 * turbo_add2index -- search through the given entry's attribute list for
 * attrs to optimize. if an attr to optimize is found, we add that attribute 
 * along with a pointer to the corresponding entry to the appropriate 
 * attribute index.
 */

turbo_add2index( e )
Entry	e;		/* the entry these attrs belong to */
{
	Entry		parent;
	Attr_Sequence	as, entry_find_type();
	DN		pdn, dn, tmpdn, prevdn, savedn;
	int		pcmp, nonleaf;
	Index		*subindex;
	Index		*sibindex;
	extern AttributeType at_masterdsa, at_slavedsa;

	if ( sibling_index == (Avlnode *) 0 && subtree_index == (Avlnode *) 0 )
		return;

	nonleaf = !e->e_leaf;

	parent = e->e_parent;
	pdn = get_copy_dn( parent );
	dn = get_copy_dn( e );

	sibindex = get_sibling_index( pdn );

	/* for each attribute in the entry... */
	for ( as = e->e_attributes; as != NULLATTR; as = as->attr_link ) {
		if ( turbo_isoptimized( as->attr_type ) == 0 )
			continue;
		SET_HEAP( as->attr_type );

		/* sibling index */
		if ( sibindex ) {
			(void) turbo_attr_insert( sibindex, e, as->attr_type,
			    as->attr_value );
		}

		savedn = NULLDN;
		while ( dn->dn_parent != NULLDN ) {
			if ( subindex = get_subtree_index( dn ) ) {
				(void) turbo_attr_insert( subindex, e,
				    as->attr_type, as->attr_value );
			}

			for ( prevdn = NULLDN, tmpdn = dn;
			    tmpdn->dn_parent != NULLDN;
			    prevdn = tmpdn, tmpdn = tmpdn->dn_parent )
				;	/* NULL */
			tmpdn->dn_parent = savedn;
			savedn = tmpdn;
			prevdn->dn_parent = NULLDN;
		}
		dn->dn_parent = savedn;
	}
	RESTORE_HEAP;

	/* now add references in nonleafkids and nonlocalaliases... */
	if ( sibindex )
		if ( e->e_alias && th_prefix( sibindex->i_dn, e->e_alias ) != -2 )
			add_nonlocalalias( e, sibindex );

	if ( nonleaf == 0 && e->e_alias == NULLDN ) {
		dn_free( dn );
		dn_free( pdn );
		return;
	}

	/* could be a subtree index with all parents & this node */
	savedn = NULLDN;
	while ( dn->dn_parent != NULLDN ) {
		if ( subindex = get_subtree_index( dn ) ) {
			if ( e->e_alias ) {
				pcmp = th_prefix( subindex->i_dn, e->e_alias );
				if ( pcmp > 0 )
					add_nonlocalalias(e, subindex);
			}

			if ( nonleaf )
				add_nonleafkid(e, subindex);
		}
		for ( prevdn = NULLDN, tmpdn = dn;
		    tmpdn->dn_parent != NULLDN;
		    prevdn = tmpdn, tmpdn = tmpdn->dn_parent )
			;	/* NULL */
		tmpdn->dn_parent = savedn;
		savedn = tmpdn;
		prevdn->dn_parent = NULLDN;
	}
	dn->dn_parent = savedn;

	dn_free( dn );
	dn_free( pdn );

	return;
}

/*
 * turbo_attr_delete -- delete entry e from index for values of attribute
 * attr.
 */

static turbo_attr_delete( pindex, e, attr, values )
Index		*pindex;
Entry		e;
AttributeType	attr;
AV_Sequence	values;
{
	int		i, j, k;
	AV_Sequence	av;
	Index_node	*node, *imem;
	struct entry	**p;
	char		*code, *word;
	char		*first_word(), *next_word();

	/* find the appropriate index */
	for ( i = 0; i < turbo_index_num; i++ )
		if ( AttrT_cmp( pindex[ i ].i_attr, attr ) == 0 )
			break;

	if ( i == turbo_index_num ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("turbo_attr_delete: cannot find optimized attribute") );
		return;
	}

	/* delete all values */
	for ( av = values; av != NULLAV; av = av->avseq_next ) {
		node = (Index_node *) avl_find( pindex[ i ].i_root, 
		    (caddr_t) &av->avseq_av, (IFP)indexav_cmp );

		if ( node == NULLINDEXNODE ) {
			LLOG( log_dsap, LLOG_EXCEPTIONS, ("Optimized attribute value not found! (%s)\n", attr->oa_ot.ot_name) );
			continue;
		}

		/* find the entry we want to delete */
		p = node->in_entries;
		for ( j = 0; j < node->in_num; j++, p++ )
			if ( *p == (struct entry *) e )
				break;

		if ( j == node->in_num ) {
			LLOG( log_dsap, LLOG_EXCEPTIONS, ("Optimized av entry not found") );
			continue;
		}

		if ( --(node->in_num) == 0 ) {
			imem = (Index_node *) avl_delete( &pindex[ i ].i_root,
			    (caddr_t) &av->avseq_av, indexav_cmp );
			( void ) AttrV_free( (AttributeValue) imem->in_value );
			( void ) free( (char *) imem->in_entries );
			( void ) free( (char *) imem );
			pindex[ i ].i_count--;
		} else {
			for ( k = j; k < node->in_num; k++ )
				node->in_entries[ k ] =
				    node->in_entries[ k + 1 ];
			node->in_entries = (struct entry **)
			    realloc( (char *) node->in_entries, (unsigned) node->in_num
			    * sizeof(struct entry *) );
		}

		/* if there's a soundex index, delete from that too */
		if ( pindex[i].i_sroot == NULLAVL )
			continue;

		for ( word = first_word((char *)av->avseq_av.av_struct);
		    word != NULL; word = next_word( word ) ) {
			code = NULL;
			soundex( word, &code );

			/*
			 * not finding the node is ok if the entry happens
			 * to be the only one with this code and was deleted
			 * on a previous pass through this loop.  we hope.
			 */

			if ((imem = (Index_node *) avl_find(pindex[i].i_sroot,
			    code, index_soundex_cmp)) == NULLINDEXNODE) {
				free(code);
				continue;
			}

			/* find the entry */
			p = imem->in_entries;
			for ( j = 0; j < imem->in_num; j++, p++ )
				if ( *p == (struct entry *) e )
					break;

			/*
			 * not finding the entry is this is ok for the soundex
			 * index since an entry can appear more than once and
			 * might have already been deleted on a previous pass
			 */

			if ( j == imem->in_num )
				continue;

			if ( --(imem->in_num) == 0 ) {
				imem = (Index_node *)
				    avl_delete( &pindex[ i ].i_sroot,
				    (caddr_t) code, index_soundex_cmp );

				free( (char *) imem->in_value );
				free( (char *) imem->in_entries );
				free( (char *) imem );
			} else {
				for ( k = j; k < imem->in_num; k++ )
					imem->in_entries[ k ] =
					    imem->in_entries[ k+1 ];

				imem->in_entries = (struct entry **)
				    realloc( (char *) imem->in_entries,
				    (unsigned) imem->in_num * sizeof(struct entry *) );
			}
			free(code);
		}
	}
}

/*
 * turbo_index_delete -- delete attribute index entries for the given
 * entry from the attribute index associated with the entry's parent
 * node.
 */

turbo_index_delete( e )
Entry	e;
{
	Entry		parent;
	Attr_Sequence	as;
	DN		pdn, dn, tmpdn, prevdn, savedn;
	Index		*subindex;
	Index		*sibindex;
	int		pcmp, nonleaf;

	if ( subtree_index == NULLAVL && sibling_index == NULLAVL )
		return;

	nonleaf = (! isleaf(e));

	parent = e->e_parent;
	pdn = get_copy_dn( parent );
	dn = get_copy_dn( e );

	sibindex = get_sibling_index( pdn );

	/* for each attribute in the entry... */
	for ( as = e->e_attributes; as != NULLATTR; as = as->attr_link ) {
		if ( turbo_isoptimized( as->attr_type ) == 0 )
			continue;

		/* sibling index */
		if ( sibindex ) {
			(void) turbo_attr_delete( sibindex, e, as->attr_type,
			    as->attr_value );
		}

		savedn = NULLDN;
		while ( dn->dn_parent != NULLDN ) {
			if ( subindex = get_subtree_index( dn ) ) {
				(void) turbo_attr_delete( subindex, e,
				    as->attr_type, as->attr_value );
			}

			for ( prevdn = NULLDN, tmpdn = dn;
			    tmpdn->dn_parent != NULLDN;
			    prevdn = tmpdn, tmpdn = tmpdn->dn_parent )
				;	/* NULL */
			tmpdn->dn_parent = savedn;
			savedn = tmpdn;
			prevdn->dn_parent = NULLDN;
		}
		dn->dn_parent = savedn;
	}

	/* now delete references in nonleafkids and nonlocalaliases... */
	if ( sibindex && e->e_alias
	    && th_prefix( sibindex->i_dn, e->e_alias ) != -2 )
		delete_nonlocalalias( e, sibindex );

	if ( nonleaf == 0 && e->e_alias == NULLDN ) {
		dn_free( pdn );
		dn_free( dn );
		return;
	}

	/* could be a subtree index with all parents & this node */
	savedn = NULLDN;
	while ( dn->dn_parent != NULLDN ) {
		if ( subindex = get_subtree_index( dn ) ) {
			if ( e->e_alias ) {
				pcmp = th_prefix( subindex->i_dn, e->e_alias );
				if ( pcmp > 0 )
					delete_nonlocalalias( e, subindex );
			}

			if ( nonleaf )
				delete_nonleafkid( e, subindex );
		}
		for ( prevdn = NULLDN, tmpdn = dn;
		    tmpdn->dn_parent != NULLDN;
		    prevdn = tmpdn, tmpdn = tmpdn->dn_parent )
			;	/* NULL */
		tmpdn->dn_parent = savedn;
		savedn = tmpdn;
		prevdn->dn_parent = NULLDN;
	}
	dn->dn_parent = savedn;

	dn_free( pdn );
	dn_free( dn );

	return;
}

/*
 * turbo_isoptimized -- return TRUE if attr is to be optimized, FALSE
 * otherwise.
 */

turbo_isoptimized( attr )
AttributeType	attr;
{
	int	i;

	for ( i = 0; i < turbo_index_num; i++ ) {
		if ( AttrT_cmp( attr, turbo_index_types[ i ] ) == 0 )
			return( 1 );
	}

	return( 0 );
}

/*
 * turbo_optimize -- add attribute attr to the list of attributes to be 
 * optimized this routine creates an empty index and arranges for the 
 * attribute to be optimized during loading.
 */

turbo_optimize( attr )
char	*attr;
{
	AttributeType	a;

	if ( (a = str2AttrT( attr )) == NULLAttrT ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Bad attribute type (%s)", attr));
		return;
	}

	if ( subtree_index || sibling_index )
		fatal( -99, "optimized attributes MUST be specified before subtree or sibling index" );

	if ( turbo_index_types == (AttributeType *) 0 )
		turbo_index_types = (AttributeType *) malloc(
		    sizeof(AttributeType *));
	else
		turbo_index_types = (AttributeType *) realloc(
		    (char *) turbo_index_types, (unsigned) (turbo_index_num + 1) *
		    sizeof(AttributeType *));

	if ( turbo_index_types == (AttributeType *) 0 )
		fatal(66, "turbo_optimize: malloc failed!\n");

	turbo_index_types[ turbo_index_num ] = AttrT_cpy(a);
	turbo_index_num++;

	return;
}

/*
 * index_subtree - arrange for the subtree starting at tree to be indexed.
 */

index_subtree( tree )
char	*tree;
{
	DN		dn, str2dn();
	Index		*pindex;

	if ( (dn = str2dn( tree )) == NULLDN ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("Invalid subtree (%s)\n", tree) );
		return;
	}
	if ( turbo_index_num == 0 ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("WARNING: cannot index subtree - no attributes have been optimized"));
		return;
	}

	pindex = new_index( dn );
	dn_free( dn );
	if ( avl_insert( &subtree_index, (caddr_t) pindex, i_cmp, i_dup ) == NOTOK ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Subtree index for %s already exists\n", tree));
		index_free( pindex );
	}

	return;
}

/*
 * index_siblings - arrange for the children of parent to be indexed.
 */

index_siblings( parent )
char	*parent;
{
	DN		dn, str2dn();
	Index		*pindex;

	if ( (dn = str2dn( parent )) == NULLDN ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("Invalid parent (%s)\n", parent) );
		return;
	}
	if ( turbo_index_num == 0 ) {
		LLOG( log_dsap, LLOG_EXCEPTIONS, ("WARNING: cannot index siblings - no attributes have been optimized"));
		return;
	}

	pindex = new_index( dn );
	dn_free( dn );
	if ( avl_insert( &sibling_index, (caddr_t) pindex, i_cmp, i_dup ) == NOTOK ) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Sibling index for %s already exists\n", parent));
		index_free( pindex );
	}

	return;
}

#endif /* turbo_index */
