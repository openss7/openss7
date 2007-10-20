-- if-enc.py - manually-augmented InformationFramework module

-- $Header: /xtel/isode/isode/dsap/x500as/RCS/if.py,v 9.0 1992/06/16 12:14:33 isode Rel $
--
--
-- $Log: if.py,v $
-- Revision 9.0  1992/06/16  12:14:33  isode
-- Release 8.0
--
--

--
--				  NOTICE
--
--    Acquisition, use, and distribution of this module and related
--    materials are subject to the restrictions of a license agreement.
--    Consult the Preface in the User's Manual for the full terms of
--    this agreement.
--
--


IF
	{
	joint-iso-ccitt
	ds(5)
	modules(1)
	informationFramework(1)
	}

DEFINITIONS ::=

%{
#include "quipu/malloc.h"
%}

PREFIXES encode decode print

BEGIN

-- EXPORTS
--	AttributeType,
-- 	AttributeValue,
--	AttributeValueAssertion,
--	Attribute, 
-- 	RelativeDistinguishedName,
--	DistinguishedName,
--	Name;


AttributeType [[P oid_table_attr *]] ::=
    OBJECT IDENTIFIER [[ O oa_ot.ot_oid ]] [[ D dec_at ]] [[ F free_at_dummy ]]
					

AttributeValue [[P attrVal *]]
        ::= %D{ 
	    *parm = AttrV_alloc();
	%} ANY [[a av_struct ]] [[ E grab_pe1 ]] [[ F AttrV_free ]]

AttributeValueAssertion [[P AVA *]]
        ::= SEQUENCE {
                AttributeType [[p parm->ava_type]],
                AttributeValue [[p parm->ava_value]] [[ D dec_av1 ]] 
        }

Attribute [[P attrcomp *]] ::=
        SEQUENCE {
	    type AttributeType [[p parm->attr_type]],

	    values D-AValues [[ p attr_value ]] %D{

		   /* Decode and merge */
		   
		   AV_Sequence avs;
		   AV_Sequence avs_next;
		   AV_Sequence newavs = NULLAV;
		   extern pe_print ();
		   extern AttrT_print ();
		   extern LLog * log_dsap;
		   PE mype;

		   SET_HEAP ((*parm)->attr_type);

		   for (avs=(*parm)->attr_value; avs != NULLAV; avs = avs_next) {
		       avs_next = avs->avseq_next;   
		       avs->avseq_next = NULLAV;
		       mype = (PE)avs->avseq_av.av_struct;
		       if (AttrV_decode ((*parm)->attr_type,&avs->avseq_av) != OK) {
		                pslog (log_dsap,LLOG_EXCEPTIONS,
				      "AttrV_decode failed (1)",
				      AttrT_print, (caddr_t) (*parm)->attr_type);
#ifdef  DEBUG
               			pslog (log_dsap,LLOG_EXCEPTIONS,
				      "PE", pe_print, (caddr_t) mype);
				      
#endif
				RESTORE_HEAP;
				return NOTOK;
}

		       newavs = avs_merge (avs,newavs);
		   }
		   (*parm)->attr_value = newavs;
		   RESTORE_HEAP;
	    %}
        }

-- Dummy type introduced to avoid the ambiguity in the grammer for function
-- calls

D-AValues [[ P avseqcomp * ]] ::=
	SET OF <<avseq_next>>
	    AttributeValue [[a avseq_av.av_struct ]] [[ E grab_pe2 ]] 

RelativeDistinguishedName [[P rdncomp * ]]
        ::=
        SET OF [[ T rdncomp * $ * ]] <<rdn_next>>
	     -- AttributeValueAssertion [[p ava_temp]]
	    SEQUENCE [[ T rdncomp * $ * ]]
	    {
		    AttributeType [[p parm->rdn_at]],
		    AttributeValue [[a parm->rdn_av.av_struct ]]
			[[ E grab_pe3 ]]
	    } %D{
		   /* Decode and merge */
		   
		   RDN rdn;
		   RDN rdn_next;
		   RDN newrdn = NULLRDN;
		   extern AttrT_print ();
		   extern pe_print ();
		   extern LLog * log_dsap;
		   PE mype;
		   
		   for (rdn=(*parm); rdn != NULLRDN; rdn = rdn_next) {
		       rdn_next = rdn->rdn_next;
		       rdn->rdn_next = NULLRDN;
		       mype = (PE)rdn->rdn_av.av_struct;
		       if (AttrV_decode (rdn->rdn_at,&rdn->rdn_av) != OK) {
		                pslog (log_dsap,LLOG_EXCEPTIONS,
				      "AttrV_decode failed (2)",
				      AttrT_print, (caddr_t) rdn->rdn_at);
#ifdef  DEBUG
               			pslog (log_dsap,LLOG_EXCEPTIONS,
				      "PE", pe_print, (caddr_t) mype);
				      
#endif
				return NOTOK;
			}
		       newrdn = rdn_merge (newrdn,rdn);
		   }
		   *parm = newrdn;
	    %}

RDNSequence [[P dncomp *]] ::=
        SEQUENCE OF [[ T dncomp * $ * ]]
        <<dn_parent>>
                RelativeDistinguishedName [[p dn_rdn]]

DistinguishedName [[P dncomp *]]
        ::=
        RDNSequence [[p *]]

Name [[P dncomp *]]
        ::=
-- Remove choice for efficiency - ASN.1 is the same
--        CHOICE <D<0>><E<1>>
--        {
                RDNSequence [[p *]]
--        }

END

%{

PE grab_pe();

grab_pe1 (parm,ppe)
attrVal *parm;
PE *ppe;
{
	if (*ppe = grab_pe (parm))
		return OK;
	else
		return NOTOK;
}

grab_pe2 (parm,ppe)
avseqcomp * parm;
PE *ppe;
{
	if (*ppe = grab_pe (&parm->avseq_av))
		return OK;
	else
		return NOTOK;
}

grab_pe3 (parm,ppe)
rdncomp * parm;
PE *ppe;
{ 
  	if (*ppe = grab_pe (&parm->rdn_av))
		return OK;
	else
		return NOTOK;
}


dec_av1 (parm,pe)
AVA ** parm;
PE pe;
{
int res;
extern AttrT_print ();
extern pe_print ();
extern LLog * log_dsap;

	(*parm)->ava_value = AttrV_alloc ();
	(*parm)->ava_value->av_syntax = 0;
	(*parm)->ava_value->av_struct = (caddr_t) pe;
	pe->pe_refcnt++;

	if ((res = AttrV_decode ((*parm)->ava_type,(*parm)->ava_value)) != OK){
                pslog (log_dsap,LLOG_EXCEPTIONS,
		      "AttrV_decode failed (3)",
		      AttrT_print, (caddr_t) (*parm)->ava_type);
#ifdef  DEBUG
                pslog (log_dsap,LLOG_EXCEPTIONS,
		      "PE", pe_print, (caddr_t) pe);
#endif
        }
        return res;
}

dec_at (parm,pe)
oid_table_attr ** parm;
PE pe;
{
OID oid;
oid_table_attr * AttrT_decode_aux();

    if (oid = prim2oid (pe)) {
       oid = oid_cpy (oid);
       *parm = AttrT_decode_aux (oid);
       oid_free (oid);
    }
    return OK;
}

/* ARGSUSED */
free_at_dummy (parm)	/* Make sure nothing happens */
oid_table_attr * parm;
{
	return OK;
}


#ifndef lint

#undef encode_IF_AttributeType
int	encode_IF_AttributeType(pe, top, len, buffer, parm)
PE     *pe;
int	top,
	len;
char   *buffer;
struct type_IF_AttributeType *parm;
{
  return (enc_f(_ZAttributeTypeIF, &_ZIF_mod, pe, top, len, buffer,
		(char *) parm));
}

#undef decode_IF_AttributeType
int	decode_IF_AttributeType(pe, top, len, buffer, parm)
PE	pe;
int	top,
       *len;
char  **buffer;
struct type_IF_AttributeType **parm;
{
  return (dec_f(_ZAttributeTypeIF, &_ZIF_mod, pe, top, len, buffer,
		(char **) parm));
}

#undef encode_IF_AttributeValue
int	encode_IF_AttributeValue(pe, top, len, buffer, parm)
PE     *pe;
int	top,
	len;
char   *buffer;
struct type_IF_AttributeValue *parm;
{
  return (enc_f(_ZAttributeValueIF, &_ZIF_mod, pe, top, len, buffer,
		(char *) parm));
}

#undef decode_IF_AttributeValue
int	decode_IF_AttributeValue(pe, top, len, buffer, parm)
PE	pe;
int	top,
       *len;
char  **buffer;
struct type_IF_AttributeValue **parm;
{
  return (dec_f(_ZAttributeValueIF, &_ZIF_mod, pe, top, len, buffer,
		(char **) parm));
}

#undef encode_IF_DistinguishedName
int	encode_IF_DistinguishedName(pe, top, len, buffer, parm)
PE     *pe;
int	top,
	len;
char   *buffer;
struct type_IF_DistinguishedName *parm;
{
  return (enc_f(_ZDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer,
		(char *) parm));
}

#undef decode_IF_DistinguishedName
int	decode_IF_DistinguishedName(pe, top, len, buffer, parm)
PE	pe;
int	top,
       *len;
char  **buffer;
struct type_IF_DistinguishedName **parm;
{
  return (dec_f(_ZDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer,
		(char **) parm));
}
#undef encode_IF_Name
int	encode_IF_Name(pe, top, len, buffer, parm)
PE     *pe;
int	top,
	len;
char   *buffer;
struct type_IF_Name *parm;
{
  return (enc_f(_ZNameIF, &_ZIF_mod, pe, top, len, buffer,
		(char *) parm));
}

#undef decode_IF_Name
int	decode_IF_Name(pe, top, len, buffer, parm)
PE	pe;
int	top,
       *len;
char  **buffer;
struct type_IF_Name **parm;
{
  return (dec_f(_ZNameIF, &_ZIF_mod, pe, top, len, buffer,
		(char **) parm));
}


#endif

%}
