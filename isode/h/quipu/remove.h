/* remove.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/remove.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: remove.h,v $
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


#ifndef QUIPUREMOVE
#define QUIPUREMOVE

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_removeentry_arg {
    CommonArgs rma_common;
    DN rma_object;
};

#endif
