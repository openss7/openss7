/* name.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/name.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: name.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
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


#ifndef QUIPUNAME
#define QUIPUNAME

#include "quipu/attr.h"

typedef struct ava {            /* represents AttributeValueAssertion */
    AttributeType ava_type;
    AttributeValue ava_value;
}ava, AVA;

typedef struct rdncomp {        /* RDN is sequence of attribute value   */
				/* assertions                           */
				/* represents RelativeDistinguishedName */
    attrType       	rdn_at;
    attrVal      	rdn_av;
    struct rdncomp      *rdn_next;
} rdncomp, *RDN;

#define NULLRDN ((RDN) 0)
#define rdn_comp_alloc()          (RDN) smalloc(sizeof(rdncomp))
RDN  rdn_comp_new ();
RDN  rdn_comp_cpy ();
RDN  str2rdn();
RDN  rdn_cpy ();
RDN  rdn_merge ();

typedef struct dncomp {         /* DN is sequence of RDNs.              */
				/* represents RDNSequence which is      */
				/* equivalent to DistinguishedName      */
    RDN                 dn_rdn;
    struct dncomp       *dn_parent;
} dncomp, *DN;

#define NULLDN ((DN) 0)

#define dn_comp_alloc()        (DN) smalloc(sizeof(dncomp))
#define dn_comp_print(x,y,z)   if (y!=NULLDN) rdn_print(x,y->dn_rdn,z)
#define dn_comp_fill(x,y)     x -> dn_rdn = y
#define dn_comp_cmp(x,y)      ((rdn_cmp (x->dn_rdn ,y->dn_rdn) == OK) ? OK : NOTOK )

DN  dn_comp_new ();
DN  dn_comp_cpy ();
DN  dn_cpy ();
DN  str2dn ();

char *dn2str ();
char *dn2ufn ();
char *dn2rfc ();

void dn_print ();
void dn_rfc_print ();
void ufn_dn_print ();
void ufn_rdn_print ();

int ufn_dn_print_aux ();

extern int ufn_indent;

#endif
