/* attr_sntx.c - Attribute Attribute syntax! */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attr_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/attr_sntx.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attr_sntx.c,v $
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
 *	SYNTAX:
 *
 *		attributeSyntax ::= '(' attributeSequence ')'	
 *
 *		Written by :-	Kuan Siew Weng	
 */

#include "quipu/util.h"
#include "quipu/common.h"
#include "psap.h"

#define AS_START_DELIMITER 	'('
#define AS_END_DELIMITER 	')'

extern int dn_cmp();
extern int dn_free();
extern PE dn_enc();
extern DN dn_dec();
extern DN str2dnX();
extern as_free(), as_cmp();
extern Attr_Sequence as_cpy();
Attr_Sequence attrSntx_dec();
char * find_nest();

static int indent = 0;
short as_sntx;
extern short oc_sntx;
extern IFP oc_hier;

#ifdef TURBO_DISK
char fromfile;
#endif
char *find_nest(str)
char *str;
{
	char *cp, *ptr1, *ptr2;

	if(!(cp = index(str,AS_START_DELIMITER)))
		return(NULL);
	ptr1 = ++cp;

	if(!(ptr2 = index(cp,AS_END_DELIMITER)))
		return(NULL);
	*ptr2 = NULL;

	while((ptr1 = index(ptr1,AS_START_DELIMITER)))
	{
		*ptr2 = AS_END_DELIMITER;
		ptr2++; ptr1++;
		if(!(ptr2 = index(ptr2,AS_END_DELIMITER)))
			return(NULL);
		*ptr2 = NULL;
	}
	*ptr2 = AS_END_DELIMITER;
	return(ptr2);
}

static	attrSntx_print (ps, a, format)
PS	ps;
Attr_Sequence a;
int	format;
{
	char buf[LINESIZE];
	Attr_Sequence  atl;
	AV_Sequence avs;
	extern int oidformat;
	register i;

	if (a) {
		if (format == READOUT) {
			indent++;
			for ( atl = a ; atl != NULL; atl = atl->attr_link) {
				(void) sprintf(buf,"%s",attr2name(atl->attr_type,oidformat));
				for (avs= atl->attr_value; avs != NULLAV; avs = avs->avseq_next) {
					ps_printf(ps,"\n");
					for ( i = 0; i< indent;i++)
						ps_printf(ps,"  ");
					ps_printf(ps,"%-21s - ",buf);
					avs_comp_print(ps,avs,format);
				}
			}
			indent--;
		}
		else {
			ps_printf(ps,"%c\n",AS_START_DELIMITER);
			as_print(ps,a,format);
			ps_printf(ps,"%c",AS_END_DELIMITER);
		}
	}
}

static Attr_Sequence str2attrSntx (str)
char   *str;
{
	char * ptr;
	Attr_Sequence as = NULLATTR, tas;
	char * getnextline();
#ifdef TURBO_DISK
	char * fgetnextline();
#endif

	if ( *str != AS_START_DELIMITER)
		parse_error ("Starting delimeter missing",NULLCP);


	for (;;) { /* break out */
		
#ifdef TURBO_DISK
		if (fromfile) {
			if ((ptr = fgetnextline ()) == NULLCP) {
				parse_error ("Attribute sntx EOF unexpected",NULLCP);
				return (NULLATTR);
			}
		} else
#endif
			if ((ptr = getnextline ()) == NULLCP) {
				parse_error ("Attribute sntx EOF unexpected(2)",NULLCP);
				return (NULLATTR);
			}

		if ( *ptr == AS_END_DELIMITER )
			break;

		if ((tas = str2as(ptr)) == NULLATTR) {
				parse_error ("attr sntx attr parse failed",NULLCP);
				return (NULLATTR);
			}
		as = as_merge (as, tas);
	}

	return as;
}

Attr_Sequence str2attrSeq(buf)
char * buf;
{
	char *cp, cp1[3];

	cp = smalloc(strlen(buf)+4);
	(void) sprintf(cp,"%c\n", AS_START_DELIMITER);
	(void) strcat(cp,buf);
	(void) sprintf(cp1,"%c\n",AS_END_DELIMITER);
	(void) strcat(cp,cp1);
	return(str2attrSntx(cp));
}
#define str2AttrList(buf)	str2attrSeq(buf)
		

static PE avs_enc(avs)
AV_Sequence avs;
{
	AV_Sequence avl;
	attrVal av;
	PE	    pe;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);

	for (avl = avs; avl; avl=avl->avseq_next) {
		AttrV_cpy_enc(&(avl->avseq_av),&av);
		(void) seq_add(pe,(PE) av.av_struct,-1);
	}
	
	return pe;
}

static AV_Sequence avs_dec(pe, at)
PE pe;
AttributeType at;
{
	AV_Sequence avl,av;
	PE r;

	avl = NULLAV;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		av = avs_comp_alloc();
		av->avseq_next = NULLAV;
		av->avseq_av.av_syntax = 0;
		av->avseq_av.av_struct = (caddr_t) pe_cpy(r);
		(void) AttrV_decode(at,&(av->avseq_av));
		avl = avs_merge(avl,av);
	}
	return avl;
}

static PE attr_enc(a)
Attr_Sequence a;
{
	PE pe, r;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);

	if ((r = oid2prim(a->attr_type->oa_ot.ot_oid)) == NULLPE) {
		pe_free(pe);
		return(NULLPE);
	}
	else
		(void) seq_add(pe,r,0);

	if ((r = avs_enc(a->attr_value)) == NULLPE) {
		pe_free(pe);
		return(NULLPE);
	}
	else
		(void) seq_add(pe,r,1);

	return(pe);
}

static Attr_Sequence attr_dec(pe)
PE pe;
{
	Attr_Sequence a;
	AttributeType at;
	PE r;

	a = NULLATTR;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		switch(r->pe_offset) {
		    case 0:		
			if (r->pe_form == PE_FORM_PRIM) {
				if (at = oid2attr(prim2oid(r))) {
					a = as_comp_new(at,NULLAV,NULLACL_INFO);
					if (((r=next_member(pe,r)) == NULLPE) ||
					    ((a->attr_value = avs_dec(r,at)) == NULLAV)) {
						as_free(a);
						return NULLATTR;
					}
	
				}
				else 
					return NULLATTR;
				break;
			}
		    default:
			if ((a = attr_dec(r)) == NULLATTR)
				return NULLATTR;
		}
	}
	return a;
}

/*
 * attrSntx_enc and attrSntx_dec must be defined as non static for tests
 */

PE attrSntx_enc (a)
Attr_Sequence a;
{
	Attr_Sequence atl;
	PE	    pe, r;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);

	for (atl = a; atl; atl=atl->attr_link) {
		if ((r = attr_enc(atl)) == NULLPE) {
			pe_free(pe);
			return(NULLPE);
		}
		else
			(void) seq_add(pe,r,-1);
	}
	return pe;
}

Attr_Sequence attrSntx_dec (pe)
PE	pe;
{
	Attr_Sequence a, atl;
	PE r;

	atl = NULLATTR;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		if (((a = attr_dec(r)) == NULLATTR) ||
		    ((atl = as_merge(atl,a)) == NULLATTR)) {
			as_free(atl);
			return(NULLATTR);
		}
	}
	return atl;
}

attribute_syntax ()
{
	as_sntx = add_attribute_syntax ("AttributeSyntax",
					(IFP) attrSntx_enc,
					(IFP) attrSntx_dec,
					(IFP) str2attrSntx,
					attrSntx_print,
					(IFP) as_cpy,
					as_cmp,
					as_free,
					NULLCP, NULLIFP, TRUE);

}
