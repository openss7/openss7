/* inherit.c - inherit attribute */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/inherit.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/inherit.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: inherit.c,v $
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


/*
	SYNTAX:
		inherit ::= [<objectclass> '$'] ["ALWAYS $"] <attribute>
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/syntaxes.h"

static inherit_free (ptr)
InheritAttr ptr;
{
	oid_free (ptr->i_oid);
	as_free  (ptr->i_default);
	as_free  (ptr->i_always);

	free ((char *) ptr);
}


static InheritAttr inherit_cpy (a)
InheritAttr a;
{
InheritAttr result;

	result = (InheritAttr) smalloc (sizeof (*result));
	result->i_always  = as_cpy (a->i_always);
	result->i_default = as_cpy (a->i_default);
	result->i_oid	  = oid_cpy (a->i_oid);
	return (result);
}

static inherit_cmp (a,b)
InheritAttr a;
InheritAttr b;
{
int res;

	if (a == NULLINHERIT)
		if (b == NULLINHERIT)
			return (0);
		else 
			return (-1);

	if ( a->i_oid ) {
		if ( b->i_oid) {
			if ( (res = oid_cmp (a->i_oid,b->i_oid)) != 0) 
				return (res);
		} else
			return -1;
	} else if ( b->i_oid )
		return 1;

	if ( (res = as_cmp (a->i_always,b->i_always)) != 0)
		return (res);

	return (as_cmp (a->i_default,b->i_default));
}


static inherit_print (ps,inherit,format)
register PS ps;
InheritAttr inherit;
int format;
{
	if (format == READOUT) {
		if (inherit->i_oid) {
			if (inherit->i_always) {
				ps_printf (ps,"Object class %s, ",oid2name(inherit->i_oid, OIDPART));
				ps_print (ps,"ALWAYS:\n");
				as_print (ps,inherit->i_always,READOUT);
			}
			if (inherit->i_default) {
				if (inherit->i_always)
					ps_print (ps,"--------\t\t");
				ps_printf (ps,"Object class %s, ",oid2name(inherit->i_oid, OIDPART));
				ps_print (ps,"DEFAULT:\n");
				as_print (ps,inherit->i_default,READOUT);
			}
		} else {
			if (inherit->i_always) {
				ps_print (ps,"ALWAYS inherit:\n");
				as_print (ps,inherit->i_always,READOUT);
			}
			if (inherit->i_default) {
				if (inherit->i_always)
					ps_print (ps,"--------\t\t");
				ps_print (ps,"DEFAULT inheritance:\n");
				as_print (ps,inherit->i_default,READOUT);
			}
		}
		ps_print (ps,"--------");
			
	} else {
		if (inherit->i_oid)
			ps_printf (ps,"%s $ ",oid2name(inherit->i_oid, OIDPART));

		if (inherit->i_always) {
			ps_print (ps,"ALWAYS (\n");
			as_print (ps,inherit->i_always,EDBOUT);
			ps_print (ps,") ");
		}
		if (inherit->i_default) {
			ps_print (ps,"DEFAULT (\n");
			as_print (ps,inherit->i_default,EDBOUT);
			ps_print (ps,")");
		}
	}
}

static char * nextline = NULLCP;

setAttributeLine(str)
char * str;
{
	/* Recusion ? */

	if ((nextline = index (str,'\n')) != NULLCP) 
		*nextline++ = 0;
}

char * nextAttributeLine (str)
char * str;
{
char * ptr;

#ifdef TURBO_DISK
char * fgetnextline();
extern char fromfile;
#endif
char * getnextline();

	if (nextline) {
		if (nextline == NULLCP)
			return NULLCP;
		ptr = nextline;
		setAttributeLine (nextline);
		return ptr;
	}

#ifdef TURBO_DISK
	if (fromfile) {
		if ((ptr = fgetnextline ()) == NULLCP) {
			parse_error ("Inherit: EOF unexpected",NULLCP);
			return (NULLCP);
		}
	} else
#endif
		if ((ptr = getnextline ()) == NULLCP) {
			parse_error ("Inherit: EOF unexpected",NULLCP);
			return (NULLCP);
		}

	return ptr;

}

static char * getInheritAttrs (asptr, needsoc, str)
Attr_Sequence * asptr;
char		needsoc;
char * 		str;
{
Attr_Sequence as = NULLATTR;
Attr_Sequence as_combine ();
Attr_Sequence as_find_type();
static AttributeType octype = NULLAttrT;
char * ptr;

	if ((ptr = nextAttributeLine (str)) == NULLCP) {
		parse_error ("Inherit: EOF unexpected",NULLCP);
		return (NULLCP);
	}
	str = ptr;

	if (*ptr == ')') {
		parse_error ("Attributes missing",NULLCP);
		return (++ptr);
	}

	while ( *ptr != 0 ) {
		as = as_combine (as,ptr,TRUE);

		if ((ptr = nextAttributeLine (ptr)) == NULLCP)
			break;
		str = ptr;

		if (*ptr == ')') {
			if (needsoc && octype == NULLAttrT)
				octype = str2AttrT("objectClass");
			if (needsoc && as_find_type(as, octype) == NULLATTR) {
				parse_error("Required inherited attribute objectClass missing",NULLCP);
				as_free(as);
				return(NULLCP);
			}
			*asptr = as;
			return (++ptr);	

		}
	}

	parse_error ("Inherit: EOF unexpected (2)",NULLCP);
	return (NULLCP);
	
}

static InheritAttr str2inherit (str)
char * str;
{
InheritAttr	result;
char 		*ptr;
char		needsoc = FALSE;
char		failed = FALSE;

	setAttributeLine(str);

	result = (InheritAttr) smalloc (sizeof *result);
	result->i_oid = NULLOID;	
	result->i_always = NULLATTR;
	result->i_default = NULLATTR;

	if ( (ptr=index (str,'$')) != NULLCP) {
		*ptr-- = 0;
		if (isspace (*ptr)) 
			*ptr = 0;
		ptr++;
		ptr = SkipSpace (++ptr);

		if ((result->i_oid = name2oid (str)) == NULLOID)
			needsoc = TRUE;
	} else
		ptr = str;

	if (lexnequ (ptr,"ALWAYS",strlen("ALWAYS")) == 0) {
		ptr = SkipSpace (ptr+strlen("ALWAYS"));
		if (*ptr++ != '(') {
			parse_error ("Inherit: open bracket missing",NULLCP);
			failed = TRUE;
		}
		ptr = SkipSpace (ptr);
		if (*ptr != 0) {
			parse_error ("Inherit: extra data '%s'",ptr);
			failed = TRUE;
		}
		ptr = getInheritAttrs(&result->i_always, FALSE, ptr);
	}

	ptr = SkipSpace (ptr);
	if (lexnequ (ptr,"DEFAULT",strlen("DEFAULT")) == 0) {
		ptr = SkipSpace (ptr+strlen("DEFAULT"));
		if (*ptr++ != '(') {
			parse_error ("Inherit: open bracket missing",NULLCP);
			failed = TRUE;
		}
		ptr = SkipSpace (ptr);
		if (*ptr != 0) {
			parse_error ("Inherit: extra data '%s'",ptr);
			failed = TRUE;
		}
		ptr = getInheritAttrs(&result->i_default, needsoc, ptr);
	}

	if ((result->i_always == NULLATTR) 
		&& (result->i_default == NULLATTR)) {
		parse_error ("Inherited Attribute syntax incorrect",NULLCP);
		return NULLINHERIT;
	}

	ptr = SkipSpace (ptr);
	if (*ptr != 0) 
		parse_error ("Inherit: extra data '%s'",ptr);

	if (failed) {
		if (result->i_oid)
			oid_free (result->i_oid);
		if (result->i_always)
			as_free (result->i_always);
		if (result->i_default)
			as_free (result->i_default);
		if (result)
			free ((char *)result);
		return NULLINHERIT;
	}
	
	return result;
}

static PE inherit_enc (m)
InheritAttr m;
{
PE ret_pe;

        (void) encode_Quipu_InheritedAttribute (&ret_pe,0,0,NULLCP,m);

	return (ret_pe);
}

static InheritAttr inherit_dec (pe)
PE pe;
{
InheritAttr m;

	if (decode_Quipu_InheritedAttribute (pe,1,NULLIP,NULLVP,&m) == NOTOK) 
		return (NULLINHERIT);
	return (m);
}

inherit_syntax ()
{
extern short inherit_sntx;

	inherit_sntx = add_attribute_syntax ("InheritedAttribute",
		(IFP) inherit_enc,	(IFP) inherit_dec,
		(IFP) str2inherit,	inherit_print,
		(IFP) inherit_cpy,	inherit_cmp,
		inherit_free,		NULLCP,
		NULLIFP,		TRUE);
}
