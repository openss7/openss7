/* template.c - your comments here */


/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/namelist.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: namelist.h,v $
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


#include <stdio.h>
#include "quipu/attrvalue.h"
#include "util.h"

struct namelist {
  char *name;
  struct namelist *next;
  Attr_Sequence ats;
};

#define NULLLIST (struct namelist *)NULL
#define list_alloc()   (struct namelist *)smalloc(sizeof (struct namelist))
