/* ryinitiator.h - include file for the generic interactive initiator */

/* 
 * $Header: /xtel/isode/isode/others/lookup/RCS/ryinitiator.h,v 9.0 1992/06/16 12:42:23 isode Rel $
 *
 *
 * $Log: ryinitiator.h,v $
 * Revision 9.0  1992/06/16  12:42:23  isode
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


#include "rosy.h"


static struct dispatch {
    char   *ds_name;
    int	    ds_operation;

    IFP	    ds_argument;
    modtyp	*ds_mod;	/* pointer to module table */
    int		ds_ind;		/* Index into module table */

    IFP	    ds_result;
    IFP	    ds_error;

    char   *ds_help;
};


void	adios (), advise ();
void	acs_adios (), acs_advise ();
void	ros_adios (), ros_advise ();

int	ryinitiator ();
