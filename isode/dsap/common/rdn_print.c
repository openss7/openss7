#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_print.c,v $
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

rdn_comp_print (ps,rdn,format)
PS   ps;
RDN  rdn;
int  format;
{

	if (rdn!=NULLRDN) {
		AttrT_print (ps,rdn->rdn_at,format);
		ps_print (ps,"=");
		AttrV_print (ps,&rdn->rdn_av,format);
	}
	return;
}

rdn_print (ps,rdn,format)
RDN  rdn;
PS   ps;
int  format;
{
register RDN eptr;

	if (rdn ==  NULLRDN) {
		if (format == READOUT)
			ps_print  (ps,"NULL RDN");
		return;
	}

	rdn_comp_print (ps,rdn,format);

		for (eptr=rdn->rdn_next; eptr!=NULLRDN; eptr=eptr->rdn_next) {
			ps_print (ps,"%"); 
			rdn_comp_print (ps,eptr,format);
		}

}

