/* searchacl.c - Search and List Access Control routines */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/searchacl.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/searchacl.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: searchacl.c,v $
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


/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "cmd_srch.h"
#include "quipu/syntaxes.h"
#include "logger.h"

/*
 * These routines implement the searchACLSyntax.  The EDB representation
 * of this syntax is as follows:
 *
 * SearchACLSyntax ::= <accessselector> '#' <access> '#' <attrs> '#' <scope> 
 *			[ '#' <max-results> '#' <partialresults> 
 *			[ '#' <minkeylen> ]]
 *
 *
 * <accessselector> ::= 'group' '#' <dnseq>	|
 *			'prefix' '#' <dnseq> 	|
 *			'self'		 	|
 *			'other'
 *
 * <access> ::= 'search' | 'nosearch'
 *
 * <attrs> ::= 'default' | <attr-list>
 *
 * <attr-list> ::= attributetype | attributetype <attr-list>
 *
 * <scope> ::= <singlescope> | <singlescope> '$' <scope>
 *
 * <singlescope> ::= 'subtree' | 'singlelevel' | 'baseobject'
 *
 * <max-results> ::= integer
 *
 * <partialresults> ::= 'partialresults' | 'nopartialresults'
 *
 * <minkeylen> ::= integer
 *
 */

short sacl_sntx = -1;
short lacl_sntx = -1;

extern LLog	*log_dsap;

static Saclinfo sacl_alloc()
{
	Saclinfo	new;

	new = (Saclinfo) smalloc( sizeof(saclinfo) );
	bzero( (char *) new, sizeof(saclinfo) );
	new->sac_access = SACL_SEARCHABLE;
	new->sac_minkeylength = SACL_NOMINLENGTH;
	new->sac_zeroifexceeded = TRUE;
	new->sac_scope = SACL_SUBTREE | SACL_SINGLELEVEL;

	return( new );
}

sacl_free( p )
Saclinfo p;
{
	if ( p->sac_tmpbstr )
		free( p->sac_tmpbstr );
	if ( p->sac_name != NULLDNSEQ )
		dn_seq_free( p->sac_name );
	if ( p->sac_types != NULLOIDSEQ )
		oid_seq_free( p->sac_types );
	free( (char *) p );
}

lacl_free( p )
Saclinfo p;
{
	if ( p->sac_tmpbstr )
		free( p->sac_tmpbstr );
	if ( p->sac_name != NULLDNSEQ )
		dn_seq_free( p->sac_name );
	free( (char *) p );
}

/* 
 * selector_rank is used to rank acl selectors from most to least specific.
 */

int selector_rank[] = {
	-1,	/* error */
	1,	/* ACL_ENTRY	*/
	4,	/* ACL_OTHER	*/
	3,	/* ACL_PREFIX	*/
	2	/* ACL_GROUP	*/
};

sacl_cmp( a, b )
Saclinfo a;
Saclinfo b;
{
	int	i;

	if ( a == NULLSACL && b == NULLSACL )
		return( 0 );

	if ( a == NULLSACL )
		return( -1 );

	if ( b == NULLSACL )
		return( 1 );

	if ( (i = acl_info_cmp( &a->sac_info, &b->sac_info )) != 0 )
		return( i );

	if ( a->sac_scope != b->sac_scope )
		return( a->sac_scope < b->sac_scope ? -1 : 1 );

	if ( a->sac_access != b->sac_access )
		return( a->sac_access < b->sac_access ? -1 : 1 );

	if ( (i = oid_seq_cmp( a->sac_types, b->sac_types )) != 0 )
		return( i );

	if ( a->sac_access == SACL_UNSEARCHABLE )
		return( 0 );

	if ( a->sac_maxresults != b->sac_maxresults )
		return( a->sac_maxresults < b->sac_maxresults ? -1 : 1 );

	if ( a->sac_minkeylength != b->sac_minkeylength )
		return( a->sac_minkeylength < b->sac_minkeylength ? -1 : 1 );

	if ( a->sac_zeroifexceeded != b->sac_zeroifexceeded )
		return( a->sac_zeroifexceeded < b->sac_zeroifexceeded ?
		    -1 : 1 );

	return( 0 );
}

static lacl_cmp( a, b )
Listacl	a;
Listacl	b;
{
	int	i;

	if ( a == NULLSACL && b == NULLSACL )
		return( 0 );

	if ( a == NULLSACL )
		return( -1 );

	if ( b == NULLSACL )
		return( 1 );

	if ( (i = acl_info_cmp( &a->sac_info, &b->sac_info )) != 0 )
		return( i );

	if ( a->sac_scope != b->sac_scope )
		return( a->sac_scope < b->sac_scope ? -1 : 1 );

	if ( a->sac_maxresults != b->sac_maxresults )
		return( a->sac_maxresults < b->sac_maxresults ? -1 : 1 );

	return( 0 );
}

static Saclinfo sacl_cpy( sacl )
Saclinfo sacl;
{
	Saclinfo new;

	new = sacl_alloc();
	*new = *sacl;	/* struct copy */
	if ( sacl->sac_name != NULLDNSEQ )
		new->sac_name = dn_seq_cpy( sacl->sac_name );
	new->sac_tmpbstr = NULL;
	if ( sacl->sac_types != NULLOIDSEQ )
		new->sac_types = oid_seq_cpy( sacl->sac_types );

	return( new );
}

static Listacl lacl_cpy( lacl )
Listacl	lacl;
{
	return( sacl_cpy( lacl ) );
}

static Saclinfo sacl_decode( pe )
PE	pe;
{
	Saclinfo acl;

	if ( decode_Quipu_SearchACLSyntax( pe, 1, NULLIP, NULLVP, &acl )
	    == NOTOK ) {
		return( NULLSACL );
	}

	return( acl );
}

static PE sacl_enc( acl )
Saclinfo acl;
{
	PE ret_pe;

	(void) encode_Quipu_SearchACLSyntax( &ret_pe, 0, 0, NULLCP, acl );

	return( ret_pe );
}

static Saclinfo str2sacl( str )
char	*str;
{
	Saclinfo	new;
	char		save, *s, *tmp;
	int		scope;
	struct oid_seq	*str2oidseq();

	if ( (s = index( str, '#' )) == NULL ) {
		parse_error( "# missing in search acl '%s'", str );
		return( NULLSACL );
	}
	new = sacl_alloc();

	save = *s;
	*s = '\0';
	while ( isspace( *str ) )
		str++;

	/* get access selector */
	if ( lexnequ( str, "group", 5 ) == 0 ) {
		new->sac_selector = ACL_GROUP;
	} else if ( lexnequ( str, "prefix", 6 ) == 0 ) {
		new->sac_selector = ACL_PREFIX;
	} else if ( lexnequ( str, "other", 5 ) == 0 ) {
		new->sac_selector = ACL_OTHER;
	} else if ( lexnequ( str, "self", 4 ) == 0 ) {
		new->sac_selector = ACL_ENTRY;
	} else {
		parse_error( "unknown access selector '%s'", str );
		free( (char *) new );
		return( NULLSACL );
	}

	if ( new->sac_selector == ACL_GROUP
	    || new->sac_selector == ACL_PREFIX) {
		/* skip past next # */
		*s++ = save;
		str = s;
		if ( (s = index( str, '#' )) == NULL ) {
			parse_error( "2nd # missing in search acl '%s'", str );
			sacl_free( new );
			return( NULLSACL );
		}
		save = *s;
		*s = '\0';
		while ( isspace( *str ) )
			str++;

		str = TidyString(str);
		if ( (new->sac_name = str2dnseq( str )) == NULLDNSEQ ) {
			free( (char *) new );
			return( NULLSACL );
		}
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		s--;
		if ( !isspace(*s)) {
			s++;
			*s = '\0';
		} else {
			*s = '\0';
			s++;
		}
	}
	while ( isspace( *str ) )
		str++;

	/* get access */
	if ( lexnequ( str, "search", 6 ) == 0 ) {
		new->sac_access = SACL_SEARCHABLE;
	} else if ( lexnequ( str, "nosearch", 8 ) == 0 ) {
		new->sac_access = SACL_UNSEARCHABLE;
	} else {
		parse_error( "expecting [no]search, found '%s'", str );
		sacl_free( new );
		return( NULLSACL );
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		s--;
		if ( !isspace(*s)) {
			*s = '\0';
			s++;
		} else {
			*s = '\0';
			s++;
		}
	}
	while ( isspace( *str ) )
		str++;

	/* attribute list */
	if ( lexnequ( str, "default", 7 ) == 0 ) {
		new->sac_types = NULLOIDSEQ;
	} else {
		FAST_TIDY(str);
		if ( (new->sac_types = str2oidseq( str )) == NULLOIDSEQ ) {
			sacl_free( new );
			return( NULLSACL );
		}
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		s--;
		if ( !isspace(*s)) {
			*s = '\0';
			s++;
		} else {
			*s = '\0';
			s++;
		}
	}
	while ( isspace( *str ) )
		str++;

	/* get scope */
	scope = 0;
	while ( str != NULL && *str != '\0' ) {
		if ( lexnequ( str, "subtree", 7 ) == 0 ) {
			scope |= SACL_SUBTREE;
		} else if ( lexnequ( str, "single", 6 ) == 0 ) {
			scope |= SACL_SINGLELEVEL;
		} else if ( lexnequ( str, "base", 4 ) == 0 ) {
			scope |= SACL_BASEOBJECT;
		} else {
			parse_error( "unknown scope in search acl '%s'", str );
			sacl_free( new );
			return( NULLSACL );
		}
		tmp = str;
		if ( (str = index( tmp, '$' )) || (str = index( tmp, ' ' )) ) {
			str++;
			while ( isspace( *str ) )
				str++;
		}
	}
	if ( scope != 0 )
		new->sac_scope = scope;

	if ( new->sac_access == SACL_UNSEARCHABLE ) {
		if ( s != NULL ) {
			parse_error( "extra junk after nosearch '%s'", s+1 );
			sacl_free( new );
			return( NULLSACL );
		} else {
			return( new );
		}
	}

	if ( s == NULL ) {
		/*
		 * if this is only a baseobject sacl, it doesn't need
		 * anything more.
		 */
		if ( new->sac_scope == SACL_BASEOBJECT )
			return( new );

		parse_error( "max-results missing from searchacl", NULLCP );
		sacl_free( new );
		return( NULLSACL );
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		s--;
		if ( !isspace(*s)) {
			*s = '\0';
			s++;
		} else {
			*s = '\0';
			s++;
		}
	}
	while ( isspace( *str ) )
		str++;

	/* max results */
	new->sac_maxresults = atoi( str );
	if ( new->sac_maxresults <= 0 ) {
		parse_error( "searchacl maxresults must be > 0" ,NULLCP);
		sacl_free( new );
		return( NULLSACL );
	}

	if ( s == NULL ) {
		parse_error( "[no]partialresults missing from searchacl" ,NULLCP);
		sacl_free( new );
		return( NULLSACL );
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		s--;
		if ( !isspace(*s)) {
			*s = '\0';
			s++;
		} else {
			*s = '\0';
			s++;
		}
	}
	while ( isspace( *str ) )
		str++;

	/* zero-results-if-limit-exceeded flag */
	if ( lexnequ( str, "partial", 7 ) == 0 )
		new->sac_zeroifexceeded = FALSE;
	else if ( lexnequ( str, "nopartial", 9 ) == 0 )
		new->sac_zeroifexceeded = TRUE;
	else {
		parse_error( "expecting [no]partialresults got '%s'", str );
		sacl_free( new );
		return( NULLSACL );
	}

	if ( s == NULL ) {
		return( new );	/* default to no min key length */
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	while ( isspace( *str ) )
		str++;

	/* min substring key length */
	new->sac_minkeylength = atoi( str );
	if ( new->sac_minkeylength < 0 ) {
		parse_error( "searchacl minkeylength must be >= 0",NULLCP );
		sacl_free( new );
		return( NULLSACL );
	}

	return( new );
}

static char	*sacl_scope[] = {
			"subtree",
			"singlelevel",
			"baseobject"
		};

sacl_print( ps, acl, format )
PS		ps;
Saclinfo	acl;
int		format;
{
	int		i, once;
	extern char	*acl_sel[];

	/* selector */
	if ( format == READOUT )
		ps_printf( ps, "%s ", acl_sel[ acl->sac_selector ] );
	else
		ps_printf( ps, "%s # ", acl_sel[ acl->sac_selector ] );
	if ( acl->sac_selector == ACL_GROUP
	   || acl->sac_selector == ACL_PREFIX ) {
		if ( format == READOUT )
			ps_printf( ps, "( " );
		dn_seq_print( ps, acl->sac_name, format );
		if ( format == READOUT )
			ps_printf( ps, " ) " );
		else
			ps_printf( ps, " # " );
	}

	/* access */
	if ( acl->sac_access == SACL_SEARCHABLE ) {
		if ( format == READOUT )
			ps_printf( ps, "can search " );
		else
			ps_printf( ps, "search # " );
	} else {
		if ( format == READOUT )
			ps_printf( ps, "cannot search " );
		else
			ps_printf( ps, "nosearch # " );
	}

	/* attributes */
	if ( acl->sac_types == NULLOIDSEQ ) {
		if ( format == READOUT )
			ps_printf( ps, "by the default" );
		else
			ps_printf( ps, "default" );
	} else {
		if ( format == READOUT )
			ps_printf( ps, "by the attributes ( " );
		oid_seq_print( ps, acl->sac_types, format );
		if ( format == READOUT )
			ps_printf( ps, " )" );
	}
	if ( format != READOUT )
		ps_printf( ps, " # " );

	/* scope */
	if ( format == READOUT )
		ps_printf( ps, " in the " );
	once = 0;
	for ( i = 0; i < 3; i++ ) {
		if ( acl->sac_scope & (1 << i) ) {
			if ( once ) {
				if ( format == READOUT )
					ps_printf( ps, " and " );
				else
					ps_printf( ps, "$" );
			}
			ps_printf( ps, "%s", sacl_scope[ i ] );
			once = 1;
		}
	}

	if ( acl->sac_access == SACL_UNSEARCHABLE )
		return;

	/* if it's only a baseobject sacl, nothing more to print */
	if ( acl->sac_scope == SACL_BASEOBJECT )
		return;

	/* max results */
	if ( format == READOUT )
		ps_printf( ps, "\n\t\t\t(limit %d matches",
		    acl->sac_maxresults );
	else
		ps_printf( ps, " # %d", acl->sac_maxresults );

	/* zero results flag */
	if ( format == READOUT ) {
		if ( acl->sac_zeroifexceeded )
			ps_printf( ps, ", none if limit is exceeded)" );
		else
			ps_printf( ps, ")" );
	} else
		ps_printf( ps, " # %s", acl->sac_zeroifexceeded ?
		    "nopartialresults" : "partialresults" );

	/* min key length */
	if ( acl->sac_minkeylength != SACL_NOMINLENGTH ) {
		if ( format == READOUT )
			ps_printf( ps,
			    ",\n\t\t\tminimum substring key length is %d",
			    acl->sac_minkeylength );
		else
			ps_printf( ps, " # %d", acl->sac_minkeylength );
	}
}

static Listacl lacl_decode( pe )
PE	pe;
{
	Listacl	acl;

	if ( decode_Quipu_ListACLSyntax( pe, 1, NULLIP, NULLVP, &acl )
	    == NOTOK ) {
		return( NULLLISTACL );
	}

	return( acl );
}

static PE lacl_enc( acl )
Listacl acl;
{
	PE ret_pe;

	(void) encode_Quipu_ListACLSyntax( &ret_pe, 0, 0, NULLCP, acl );

	return( ret_pe );
}

Listacl str2lacl( str )
char	*str;
{
	char	*s, save;
	Listacl	new;

	if ( (s = index( str, '#' )) == NULL ) {
		parse_error( "# missing in list acl '%s'", str );
		return( NULLLISTACL );
	}

	new = sacl_alloc();

	save = *s;
	*s = '\0';
	while ( isspace( *str ) )
		str++;

	/* get access selector */
	if ( lexnequ( str, "group", 5 ) == 0 ) {
		new->sac_selector = ACL_GROUP;
	} else if ( lexnequ( str, "prefix", 6 ) == 0 ) {
		new->sac_selector = ACL_PREFIX;
	} else if ( lexnequ( str, "other", 5 ) == 0 ) {
		new->sac_selector = ACL_OTHER;
	} else if ( lexnequ( str, "self", 4 ) == 0 ) {
		new->sac_selector = ACL_ENTRY;
	} else {
		parse_error( "unknown list acl selector type '%s'", str );
		free( (char *) new );
		return( NULLLISTACL );
	}

	if ( new->sac_selector == ACL_GROUP
	    || new->sac_selector == ACL_PREFIX) {
		/* skip past next # */
		*s++ = save;
		str = s;
		if ( (s = index( str, '#' )) == NULL ) {
			parse_error( "2nd # missing in list acl '%s'", str );
			sacl_free( new );
			return( NULLLISTACL );
		}
		save = *s;
		*s = '\0';
		while ( isspace( *str ) )
			str++;

		str = TidyString(str);
		if ( (new->sac_name = str2dnseq( str )) == NULLDNSEQ ) {
			free( (char *) new );
			return( NULLLISTACL );
		}
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		*s = '\0';
	}
	while ( isspace( *str ) )
		str++;

	/* get access */
	if ( lexnequ( str, "list", 4 ) == 0 ) {
		new->sac_maxresults = -1;
	} else if ( lexnequ( str, "nolist", 6 ) == 0 ) {
		new->sac_maxresults = 0;
	} else {
		parse_error( "expecting [no]list, found '%s'", str );
		sacl_free( new );
		return( NULLLISTACL );
	}

	if ( s == NULL ) {
		parse_error( "listacl scope missing",NULLCP );
		sacl_free( new );
		return( NULLLISTACL );
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		*s = '\0';
	}
	while ( isspace( *str ) )
		str++;

	/* get scope */
	if ( lexnequ( str, "entry", 5 ) == 0 ) {
		new->sac_scope = SACL_BASEOBJECT;

		return( new );
	} else if ( lexnequ( str, "children", 8 ) == 0 ) {
		new->sac_scope = SACL_SINGLELEVEL;

		if ( new->sac_maxresults == 0 ) {
			return( new );	/* nolist was specified */
		}
	} else {
		parse_error( "expecting entry or children, got '%s'", str );
		sacl_free( new );
		return( NULLLISTACL );
	}

	if ( s == NULL ) {
		return( new );
	}

	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) != NULL ) {
		save = *s;
		*s = '\0';
	}
	while ( isspace( *str ) )
		str++;

	/* get max results */
	new->sac_maxresults = atoi( str );

	return( new );
}

lacl_print( ps, acl, format )
PS	ps;
Listacl	acl;
int	format;
{
	extern char	*acl_sel[];

	/* selector */
	if ( format == READOUT )
		ps_printf( ps, "%s ", acl_sel[ acl->sac_selector ] );
	else
		ps_printf( ps, "%s # ", acl_sel[ acl->sac_selector ] );
	if ( acl->sac_selector == ACL_GROUP
	    || acl->sac_selector == ACL_PREFIX ){
		if ( format == READOUT )
			ps_printf( ps, "( " );
		dn_seq_print( ps, acl->sac_name, format );
		if ( format == READOUT )
			ps_printf( ps, " ) " );
		else
			ps_printf( ps, " # " );
	}

	/* access */
	if ( acl->sac_maxresults != 0 ) {
		if ( format == READOUT )
			ps_printf( ps, "can list " );
		else
			ps_printf( ps, "list # " );
	} else {
		if ( format == READOUT )
			ps_printf( ps, "cannot list " );
		else
			ps_printf( ps, "nolist # " );
	}

	/* scope */
	if ( acl->sac_scope == SACL_BASEOBJECT ) {
		if ( format == READOUT )
			ps_printf( ps, "the entry" );
		else
			ps_printf( ps, "entry" );
		return;
	} else {
		if ( format == READOUT )
			ps_printf( ps, "the children" );
		else
			ps_printf( ps, "children" );
	}

	if ( acl->sac_maxresults > 0 ) {
		if ( format == READOUT )
			ps_printf( ps, " (limit %d entries)",
			    acl->sac_maxresults );
		else
			ps_printf( ps, " # %d", acl->sac_maxresults );
	}

	return;
}

sacl_syntax()
{
	sacl_sntx = add_attribute_syntax ("SearchACLSyntax",
		(IFP) sacl_enc,	(IFP) sacl_decode,
		(IFP) str2sacl,	sacl_print,
		(IFP) sacl_cpy,	sacl_cmp,
		sacl_free,	NULLCP,
		NULLIFP,	TRUE);
}

lacl_syntax()
{
	lacl_sntx = add_attribute_syntax ("ListACLSyntax",
		(IFP) lacl_enc,	(IFP) lacl_decode,
		(IFP) str2lacl,	lacl_print,
		(IFP) lacl_cpy,	lacl_cmp,
		lacl_free,	NULLCP,
		NULLIFP,	TRUE);
}
