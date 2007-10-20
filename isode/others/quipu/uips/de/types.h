/* template.c - your comments here */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/types.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: types.h,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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


#ifndef _query_types_h_
#define _query_types_h_

#include <stdio.h>

#define bool unsigned char
#define cardinal unsigned int

typedef enum {
  yes,
  no,
  unknown
  } known;

typedef enum {
  RQ_processing,
  RQ_results_returned,
  RQ_error_returned
  } request_state;

#endif
