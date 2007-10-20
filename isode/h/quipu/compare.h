/* compare.h - */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/compare.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: compare.h,v $
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


#ifndef QUIPUCOMP
#define QUIPUCOMP

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_compare_arg {
    CommonArgs cma_common;
    DN cma_object;
    AVA cma_purported;
};

struct ds_compare_result {
    CommonResults cmr_common;
    DN cmr_object;
    char cmr_matched;           /* set to TRUE or FALSE                 */
    char cmr_iscopy;            /* values defined in entrystruct        */
    char cmr_pepsycopy;
    time_t cmr_age;
};

#endif
