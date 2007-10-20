/* 
 * $Header: /xtel/isode/isode/dsap/x500as/RCS/qu-cdefs.h,v 9.0 1992/06/16 12:14:33 isode Rel $
 *
 *
 * $Log: qu-cdefs.h,v $
 * Revision 9.0  1992/06/16  12:14:33  isode
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


#ifdef PEPYPATH

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/authen.h"

#else

#include <isode/quipu/util.h>
#include <isode/quipu/entry.h>
#include <isode/quipu/authen.h>

#endif

extern char * int2strb_alloc ();
