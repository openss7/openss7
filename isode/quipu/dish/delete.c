/* delete.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/delete.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/dish/RCS/delete.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: delete.c,v $
 * Revision 9.0  1992/06/16  12:35:39  isode
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


#include "quipu/util.h"
#include "quipu/remove.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"


extern DN       dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern	char	frompipe;
extern	PS	opt, rps;

call_delete (argc, argv)
int             argc;
char          **argv;
{
	DN              dnptr,
	                trail = NULLDN; 
	struct ds_removeentry_arg remove_arg;
	struct DSError  error;

	if ((argc = service_control (OPT, argc, argv, &remove_arg.rma_common)) == -1)
		return;

	if (argc > 1) 
		if (move (argv[1]) == OK)
			argc--;

	if (argc != 1) {
		ps_printf (OPT,"Unknown option %s\n",argv[1]);
		Usage (argv[0]);
		return;
	}
	remove_arg.rma_object = dn;

	if (rebind () != OK)
		return;

	/* Strong authentication */
	if (remove_arg.rma_common.ca_security != (struct security_parms *) 0)
	{
	extern struct SecurityServices *dsap_security;

	remove_arg.rma_common.ca_sig =
		(dsap_security->serv_sign)((caddr_t)&remove_arg, 
			_ZRemoveEntryArgumentDataDAS, &_ZDAS_mod);
	}

	while (ds_removeentry (&remove_arg, &error) != DS_OK) {
		if (dish_error (OPT, &error) == 0)
			return;
		remove_arg.rma_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	} 
		
	ps_print (RPS, "Removed ");
	dn_print (RPS, dn, EDBOUT);
	delete_cache (dn);
	for (dnptr = dn; dnptr->dn_parent != NULLDN; dnptr = dnptr->dn_parent)
		trail = dnptr;

	if (trail != NULLDN) 
		trail->dn_parent = NULLDN;
	else
		dn = NULLDN;

	dn_comp_free (dnptr);
	ps_print (RPS, "\n");
}
