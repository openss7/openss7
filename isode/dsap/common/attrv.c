/* attrv.c - Attribute Value routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attrv.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/attrv.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attrv.c,v $
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


/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/ds_error.h"
#include "quipu/malloc.h"
#include "cmd_srch.h"
#include <sys/stat.h>

extern int oidformat;
extern struct PSAPaddr * psap_cpy ();
extern str2asn (), pe_print() ;
AttributeValue str2file ();
extern LLog * log_dsap;
PE asn2pe();
int quipu_pe_cmp();
char t61_flag;
char crypt_flag;
extern char dsa_mode;
extern int file_cmp ();
extern PE grab_filepe ();

static short num_syntax = 1;
static sntx_table syntax_table [MAX_AV_SYNTAX] = { {
	"ASN", 		/* ASN - default type */
	(IFP)pe_cpy,	/* default encode */ 
	NULLIFP,	/* no decoding needed */
	NULLIFP,	/* default parse */
	NULLIFP, 	/* default print */
	(IFP)pe_cpy,	/* default copy */
#ifdef STRICT_X500
	NULLIFP,	/* Not allowed if syntax unknown (X.501 9.6.2) */
#else
	quipu_pe_cmp,	/* default compare */
#endif
	pe_free,	/* default free */
	NULLCP,		/* no pe_printer */
	NULLIFP,	/* NO approx matching */
	TRUE,		/* one per line */
} };
	

short add_attribute_syntax (sntx,enc,dec,parse,print,cpy,cmp,sfree,print_pe,approx,multiline)
char *	sntx;
IFP	enc,dec,parse,print,cpy,cmp,sfree,approx;
char *  print_pe;
char	multiline;
{
	if (num_syntax >= MAX_AV_SYNTAX)
		return (-1); 

	syntax_table[num_syntax].s_sntx    = sntx;
	set_attribute_syntax (num_syntax,enc,dec,parse,print,cpy,cmp,sfree,print_pe,approx,multiline);
	
	return (num_syntax++);
}

set_attribute_syntax (sntx,enc,dec,parse,print,cpy,cmp,sfree,print_pe,approx,multiline)
short sntx;
IFP	enc,dec,parse,print,cpy,cmp,sfree,approx;
char *  print_pe;
char	multiline;
{
        if (sntx > num_syntax)
	    return;

	syntax_table[sntx].s_encode  = enc;
	syntax_table[sntx].s_decode  = dec;
	syntax_table[sntx].s_parse   = parse;
	syntax_table[sntx].s_print   = print;
	syntax_table[sntx].s_copy    = cpy;
	syntax_table[sntx].s_compare = cmp;
	syntax_table[sntx].s_free    = sfree;
	syntax_table[sntx].s_pe_print= print_pe;
	syntax_table[sntx].s_approx  = approx;
	syntax_table[sntx].s_multiline  = multiline;
}

set_av_pe_print (sntx,print_pe)
short sntx;
char *  print_pe;
{
        if (sntx >= num_syntax)
	    return;

	syntax_table[sntx].s_pe_print= print_pe;
}

set_av_printer (sntx,print)
short sntx;
IFP print;
{
        if (sntx >= num_syntax)
	    return;

	syntax_table[sntx].s_print = print;
}

short modify_av_printer (at,sntx,print)
AttributeType at;
char * sntx;
IFP print;
{
short nstx;

	nstx  = add_attribute_syntax(sntx,
	    syntax_table[at->oa_syntax].s_encode,
	    syntax_table[at->oa_syntax].s_decode,
	    syntax_table[at->oa_syntax].s_parse,
	    print,
	    syntax_table[at->oa_syntax].s_copy,
	    syntax_table[at->oa_syntax].s_compare,
	    syntax_table[at->oa_syntax].s_free,
	    syntax_table[at->oa_syntax].s_pe_print,
	    syntax_table[at->oa_syntax].s_approx,
	    syntax_table[at->oa_syntax].s_multiline);

	at->oa_syntax = nstx;
	return nstx;
}

split_attr (as)
Attr_Sequence as;
{
	if ((as->attr_type == NULLTABLE_ATTR)
	  ||(as->attr_type->oa_syntax >= AV_WRITE_FILE))
		return (TRUE);
	else 
		return (syntax_table[as->attr_type->oa_syntax].s_multiline);
}

IFP approxfn (x)
short x;
{
        if (x >= num_syntax)
	    return NULLIFP;

	return (syntax_table[x].s_approx);
}

char * syntax2str (sntx)
short sntx;
{
        if (sntx >= num_syntax)
	    return NULLCP;

	return (syntax_table[sntx].s_sntx);
}

short str2syntax (str)
char * str;
{
register sntx_table * ptr;
register int i;

	for (i=0, ptr = &syntax_table[0] ; i<num_syntax; i++,ptr++) 
		if ( lexequ (ptr->s_sntx,str) == 0)
			return (i);

	return (0);
}

sntx_table * get_syntax_table (x)
short x;
{
        if (x >= num_syntax)
	    return (sntx_table * )NULL;

	return ( &syntax_table[x]);
}

AttrV_free (x)
AttributeValue x;
{
	AttrV_free_aux (x);
	free ((char *)  x);
}

AttrV_free_aux (x)
register AttributeValue x;
{
	if (x == NULLAttrV)
		return;

	if (x->av_syntax == AV_FILE) {
		file_free ((struct file_syntax *)x->av_struct);
		return;
	}

	if (( x->av_syntax < AV_WRITE_FILE ) 
		&& (syntax_table[x->av_syntax].s_free != NULLIFP)
		&& (x->av_struct != NULL))
			(*syntax_table[x->av_syntax].s_free) (x->av_struct);
}

PE      grab_pe(av)
AttributeValue  av;
{
PE ret_pe = NULLPE;

	if (av->av_syntax == AV_FILE) 
		ret_pe = grab_filepe (av);
	else if (syntax_table[av->av_syntax].s_encode != NULLIFP)
		ret_pe = (PE)((*syntax_table[av->av_syntax].s_encode) (av->av_struct));

	return (ret_pe);
}

AttrV_decode(x,y)
register AttributeType x;
register AttributeValue y;
{
int y_syntax;

	if (x == NULLAttrT)
		return (NOTOK);

	if ((y == NULLAttrV) || (y->av_syntax != 0))
		return (OK);

	if (y->av_struct == NULL)
		return (OK);

	y->av_syntax = x->oa_syntax;
	if ( (y_syntax = y->av_syntax) >= AV_WRITE_FILE ) 
		y_syntax = y->av_syntax - AV_WRITE_FILE;

	ATTRIBUTE_HEAP;

	if (syntax_table[y_syntax].s_decode != NULLIFP) {
		PE oldpe = (PE)y->av_struct;
		if ((y->av_struct = (caddr_t)((*syntax_table[y_syntax].s_decode) (oldpe))) == NULL) {
			RESTORE_HEAP;
			y->av_struct = (caddr_t)oldpe;
			y->av_syntax = 0;
			return (NOTOK);
		}
		pe_free (oldpe);
	} 

	if (((y->av_syntax == 0) && (((PE)y->av_struct)->pe_len > LINESIZE))
			/* The PE is too long for EDB file */
	   ||( y->av_syntax >= AV_WRITE_FILE ))
			/* Told to write it to a file */
		file_decode (y);

	RESTORE_HEAP;

	return (OK);
}

static strip_header(str)
char ** str;
{
register char * ptr, *save, val;
int syntax;
static CMD_TABLE cmd_syntax [] = {
        "FILE",		1,
	"ASN",		2,
	"T.61",		3,
	"CRYPT",	4,
        0,              0,
        } ;


	if (*str == NULLCP)
		return (0);

	ptr = SkipSpace (*str);
	if (*ptr == 0) {
		if ( ptr == (*str)+1) 	/* single space */
			return 5;
		return (0);
	}
	*str = ptr;

	t61_flag = FALSE;
	crypt_flag = FALSE;

	if (*ptr++ == '{') {
		/* look for syntax */	
		if (( *str = index (ptr,'}')) == 0) {
			parse_error ("syntax close bracket missing '%s'",--ptr);
			return (0);
		}
                save = *str;
                val = **str;

		*(*str)++ = 0;
		*str = SkipSpace (*str);

		if ((syntax = cmd_srch (ptr,cmd_syntax)) == 0) {
			parse_error ("unknown syntax '%s'",ptr);
			return (0);
		}
                *save = val;

		if ((syntax == 4) && ! dsa_mode) {
			parse_error ("{CRYPT} not allowed",ptr);
			return (0);
		}
		return (syntax);

	}
	return (5);
}

str_at2AttrV_aux (str,at,rav)
char * str;
AttributeType at;
AttributeValue rav;
{
	if (at == NULLAttrT)
		return (NOTOK);

	switch (strip_header (&str)) {
		case 0:	/* error */
			return (NOTOK);
		case 1: /* FILE */
			return (str2file_aux (str,at,rav));
		case 2: /* ASN */
			(void) str2AttrV_aux (str,0,rav);
			if (AttrV_decode (at,rav) == NOTOK) {
				parse_error ("ASN attribute decode failed",NULLCP);
				return (NOTOK);
			}
			return (OK);
		case 3: /* T.61 */
			t61_flag = TRUE;
			return (str2AttrV_aux (SkipSpace(str),at->oa_syntax,rav));
		case 4:
			crypt_flag = TRUE;
			/* fall */
		case 5:
			return (str2AttrV_aux (str,at->oa_syntax,rav));
	}
	return (NOTOK);
}


AttributeValue str_at2AttrV (str,at)
char * str;
AttributeType at;
{
AttributeValue av;

	if (at == NULLAttrT)
		return (NULLAttrV);

	switch (strip_header(&str)) {
		case 0:	/* error */
			return (NULLAttrV);
		case 1: /* FILE */
			return (str2file (str,at));
		case 2: /* ASN */
			av = str2AttrV (str,0);
			if (AttrV_decode (at,av) == NOTOK) {
				parse_error ("ASN attribute decode failed (2)",NULLCP);
				return (NULLAttrV);
			}
			return (av);
		case 3: /* T.61 */
			t61_flag = TRUE;
			return (str2AttrV (str,at->oa_syntax));
		case 4:
			crypt_flag = TRUE;
			/* fall */
		case 5:
			return (str2AttrV (str,at->oa_syntax));
	}
	return (NULLAttrV);
}

AttributeValue str2AttrV (str,syntax)
char * str;
short syntax;
{
AttributeValue x;
short ns;

	x = AttrV_alloc();

	if ( (ns = syntax) >= AV_WRITE_FILE ) 
		ns = syntax - AV_WRITE_FILE;
	
	if (str2AttrV_aux (str,ns,x) == OK)
		return (x);
	free ((char *)x);
	return NULLAttrV;
}

str2AttrV_aux (str,syntax,x)
char * str;
short syntax;
AttributeValue x;
{

	if (str == NULLCP)
		return (NOTOK);

	if ((x->av_syntax = syntax) > AV_WRITE_FILE)
		x->av_syntax = syntax - AV_WRITE_FILE;
	x->av_struct = NULL;

	if (syntax_table[syntax].s_parse != NULLIFP) {
		if ((x->av_struct = (caddr_t)(*syntax_table[syntax].s_parse) (str)) == NULL) 
			return (NOTOK);
		if (t61_flag) {
			parse_error ("invalid use of {T.61}",NULLCP);
			return (NOTOK);
		}
		if (crypt_flag) {
			parse_error ("invalid use of {CRYPT}",NULLCP);
			return (NOTOK);
		}
	} else if ((x->av_struct = (caddr_t) asn2pe (str)) == NULL) {
		parse_error ("invalid ASN attribute",NULLCP);
		return NOTOK;
	}
	return (OK);
}

AttributeValue AttrV_cpy (x)
register AttributeValue x;
{
register AttributeValue y = NULLAttrV;

	y = AttrV_alloc();
	AttrV_cpy_aux (x,y);
	return (y);
}

AttrV_cpy_aux (x,y)
register AttributeValue x;
register AttributeValue y;
{
struct file_syntax * fileattr_cpy();

	y->av_syntax = x->av_syntax;

	if (x->av_struct == NULL) {
		y->av_struct = NULL;
		return;
	}

	if (x->av_syntax == AV_FILE) 
		y->av_struct = (caddr_t) fileattr_cpy ((struct file_syntax *)x->av_struct);
	else if (syntax_table[x->av_syntax].s_copy != NULLIFP) 
		y->av_struct = (caddr_t)(*syntax_table[x->av_syntax].s_copy) (x->av_struct);
	else 
		y->av_struct = NULL;
}

AttrV_cpy_enc (x,y)
register AttributeValue x;
register AttributeValue y;
{
struct file_syntax * fileattr_cpy();

	/* Encode a copy of x into y */

	y->av_syntax = 0;

	if (x->av_struct == NULL) {
		y->av_struct = NULL;
		return;
	}

	if (x->av_syntax == AV_FILE) 
		y->av_struct = (caddr_t) grab_filepe (x);
	else if (syntax_table[x->av_syntax].s_encode != NULLIFP) 
		y->av_struct = (caddr_t)(*syntax_table[x->av_syntax].s_encode) (x->av_struct);
	else 
		y->av_struct = NULL;

	return;
}


rdn_cmp (a,b)
register RDN  a,b;
{
register int i;

	for (; (a != NULLRDN) && (b != NULLRDN) ; a = a->rdn_next, b = b->rdn_next) {
		if (a->rdn_at != b->rdn_at) 
			return ((a->rdn_at > b->rdn_at) ? 1 : -1);
		
		if (syntax_table[a->rdn_av.av_syntax].s_compare == NULLIFP) 
			return (2); /* can't compare */
		else 
			if (( i = (*syntax_table[a->rdn_av.av_syntax].s_compare) (a->rdn_av.av_struct,b->rdn_av.av_struct)) != 0)
				return i;
	}

	if ( (a == NULLRDN) && (b == NULLRDN) )  {
		return 0;
	} else {
		return ( a  ?  1  : -1);
	}

}

rdn_cmp_reverse (a,b)
register RDN  a,b;
{
register int i;

	for (; (a != NULLRDN) && (b != NULLRDN) ; a = a->rdn_next, b = b->rdn_next) {
		if (a->rdn_at != b->rdn_at) 
			return ((a->rdn_at > b->rdn_at) ? -1 : 1);
		
		if (syntax_table[a->rdn_av.av_syntax].s_compare == NULLIFP) 
			return (2); /* can't compare */
		else 
			if (( i = (*syntax_table[a->rdn_av.av_syntax].s_compare) (a->rdn_av.av_struct,b->rdn_av.av_struct)) != 0)
				return i;
	}

	if ( (a == NULLRDN) && (b == NULLRDN) )  {
		return 0;
	} else {
		return ( a  ?  1  : -1);
	}

}

AttrV_cmp (x,y)
register AttributeValue x,y;
{
	if (x->av_syntax != y->av_syntax) 
		return (-2);

	if (x->av_syntax == AV_FILE)
		return (file_cmp ((struct file_syntax *)x->av_struct,(struct file_syntax *)y->av_struct));

	if (syntax_table[x->av_syntax].s_compare != NULLIFP) 
		return ((*syntax_table[x->av_syntax].s_compare) (x->av_struct,y->av_struct));
	else 
		return (2); /* can't compare */
}

IFP av_cmp_fn (syntax)
int syntax;
{
	if ( syntax >= AV_WRITE_FILE )
		return NULLIFP;

	if (syntax == AV_FILE)
		return (file_cmp);

	if (syntax_table[syntax].s_compare != NULLIFP) 
		return (syntax_table[syntax].s_compare);
	else
		return NULLIFP;
}

AttrV_print (ps,x,format)
PS ps;
AttributeValue x;
int format;
{
extern int ps_printf ();

	if (format == RDNOUT)
	    format = EDBOUT;

	if (x->av_syntax == AV_FILE) 
		fileattr_print (ps,x,format);
	else if ((format == READOUT) && (syntax_table[x->av_syntax].s_pe_print != NULLCP))
		exec_print (ps,x,syntax_table[x->av_syntax].s_pe_print);
	else if (syntax_table[x->av_syntax].s_print != NULLIFP) {
		if (x->av_struct != NULL)
			(*syntax_table[x->av_syntax].s_print) (ps,x->av_struct,format);
	} else if (format == READOUT) {
                vpushquipu (ps);
                vunknown ((PE)x->av_struct);
                vpopquipu ();
	} else 
		pe_print (ps,(PE)x->av_struct,format);
}
