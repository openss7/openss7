/* query.h - main information structure */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/query.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: query.h,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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


#ifndef _query_h
#define _query_h

#include "quipu/util.h"

struct query {
        char defvalue [LINESIZE];
        char entered [LINESIZE];
        char displayed [LINESIZE];
        struct namelist * lp;
        int listlen;
};

#endif _query_h
