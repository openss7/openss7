#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/dn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/dn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: dn_str.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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
#include "quipu/name.h"

static char dn_alias;

extern int dn_cmp ();
extern int dn_free ();
extern char * SkipSpace ();

short syntax_dn = 0;

DN str2dn (str)
register char * str;
{
    register char *ptr;
    char *save,val, sep = '@';
    char * aliasptr;
    DN dn = NULLDN, newdn, tmpdn;
    RDN rdn;
    int	rfcformat = 0;
    char * alias2name ();

    if (str == NULLCP)
	return (NULLDN);

    if (*str == '<') {
	/* try 'rfc' string format instead */
	rfcformat = 1;
	str++;			/* remove leading bracket */
	if (isspace (*str))
	    str++;
	sep = ',';
    }

    if ((rfcformat == 0) && (*str == '@'))   /* Skip leading '@' signs for dish compat */
	str++;		

    while ( (ptr = index (str,sep)) != 0) {
	save = ptr++;
	if (isspace (*(save - 1))) {
	    save--;
	    if ( *(save - 1) == '=')
		save++;		/* Let "type= " pass by */
	}
	val = *save;
	*save = 0;


	if ((rfcformat == 0) && (dn == NULLDN)) {
	    /* try str as an alias */
	    if ((aliasptr = alias2name (SkipSpace(str))) != NULLCP) {
		dn_alias = TRUE;
		if ((newdn = str2dn(aliasptr)) == NULLDN) {
		    parse_error ("Invalid alias '%s'",aliasptr);
		    dn_free (dn);
		    return (NULLDN);
		}
		dn = newdn;
		*save = val;
		str = ptr;
		continue;	
	    }
	}

	if ((rdn = str2rdn (str)) == NULLRDN) {
	    dn_free (dn);
	    return (NULLDN);
	} 
	if (dn == NULLDN)
	    dn = dn_comp_new (rdn);
	else {
	    if (rfcformat == 0)
		dn_append (dn,dn_comp_new (rdn));
	    else {
		tmpdn = dn;
		dn = dn_comp_new (rdn);
		dn ->dn_parent = tmpdn;
	    }
	}
	*save = val;
	str = ptr;
    }


    /* try str as an alias */
    if ((rfcformat == 0) && (dn == NULLDN)) {
	if ((aliasptr = alias2name (SkipSpace(str))) != NULLCP) {
	    dn_alias = TRUE;
	    if ((newdn = str2dn(aliasptr)) == NULLDN) {
		parse_error ("Invalid alias '%s'",aliasptr);
		dn_free (dn);
		return (NULLDN);
	    }
	    return (newdn);
	}
    }

    if (rfcformat == 1) {
	register char * p;
	p = str;
	while (*p && *p != '>')
	    p++;
	if (*p == '>') {
	    *p-- = 0;
	    if (isspace (*p)) 
	     *p = 0;
	}
    }

    if ((rdn = str2rdn (str)) == NULLRDN) {
	dn_free (dn);
	return (NULLDN);
    }

    if (dn == NULLDN)
	dn = dn_comp_new (rdn);
    else {
	if (rfcformat == 0) 
	    dn_append (dn,dn_comp_new (rdn));
	else {
	    tmpdn = dn;
	    dn = dn_comp_new(rdn);
	    dn->dn_parent = tmpdn;
	}
    }
    return (dn);
}

DN str2dn_aux (str,alias)
char * str;
char *alias;
{
DN dn;
	dn_alias = FALSE;
	dn = str2dn (str);
	*alias = dn_alias;
	return (dn);
}

DN str2dnX (str)
register char * str;
{
register char * ptr;

	if ((ptr = rindex (str,'#')) != 0) {
		/* a bit or reverse compatability... */
		if (*++ptr != 0) {
			parse_error ("invalid # in '%s'",str);
			return (NULLDN);
		} else
			*--ptr = 0;
	}
	return (str2dn(str));
}

DN dn_dec (pe)
PE pe;
{
DN adn;
	
	if (decode_IF_DistinguishedName(pe,1,NULLIP,NULLVP,&adn) == NOTOK)
		return (NULLDN);

	return (adn);
}

PE dn_enc (dn)
DN dn;
{
PE ret_pe;

	(void) encode_IF_DistinguishedName (&ret_pe,0,0,NULLCP,dn);
	return (ret_pe);
}

dn_syntax ()
{
	syntax_dn = add_attribute_syntax ("dn",
		(IFP) dn_enc,	(IFP) dn_dec,
		(IFP) str2dnX,	(IFP) dn_print,
		(IFP) dn_cpy,	dn_cmp,
		dn_free,	NULLCP,
		NULLIFP,	TRUE );
}


	    
