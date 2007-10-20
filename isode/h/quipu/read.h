/* read.h - read operation */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/read.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: read.h,v $
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


#ifndef QUIPUREAD
#define QUIPUREAD

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

	/* THIS SECTION GIVES OPERATION PARAMETERS */

struct ds_read_arg {
    CommonArgs rda_common;
    DN rda_object;
    EntryInfoSelection rda_eis;
};

struct ds_read_result {
    CommonResults rdr_common;
    EntryInfo rdr_entry;
};

#endif
