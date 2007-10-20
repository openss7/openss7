#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_print.c,v $
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
#include "quipu/attrvalue.h"

IFP oc_avsprint = NULLIFP;
extern short oc_sntx;
extern int avs_count;

#ifndef avs_comp_print
avs_comp_print (ps,avs,format)
AV_Sequence  avs;
PS   ps;
int  format;
{
	AttrV_print (ps,&avs->avseq_av,format);
}
#endif

avs_print (ps,avs,format)
AV_Sequence  avs;
PS   ps;
int  format;
{
	if (avs == NULLAV) {
		ps_print (ps,"\n");
		return;
	}

	if ((format == READOUT) && (avs->avseq_av.av_syntax == oc_sntx))
		(*oc_avsprint)(ps,avs,format);
	else 
		avs_print_aux (ps,avs,format," & ");
	ps_print (ps,"\n");
}

avs_print_aux (ps,avs,format,sep)
AV_Sequence  avs;
PS   ps;
int  format;
char *sep;
{
register AV_Sequence eptr;

	if (avs == NULLAV)
		return;

	avs_count = 1;
	avs_comp_print (ps,avs,format);
	avs_count++;

		for(eptr = avs->avseq_next; eptr != NULLAV; eptr = eptr->avseq_next, avs_count++) {
			ps_print (ps,sep);
			AttrV_print (ps,&eptr->avseq_av,format);
			}
}

