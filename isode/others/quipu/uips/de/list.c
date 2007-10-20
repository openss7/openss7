/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/list.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/list.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: list.c,v $
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
#include "util.h"
#include "namelist.h"
#include "quipu/util.h"

void
addToList(lp, str)
struct namelist ** lp;
char *str;
{
  for (;*lp != NULLLIST; lp = &(*lp)->next) {};
  *lp = list_alloc();
  (*lp)->name = copy_string(str);
  (*lp)->next = NULLLIST;
}

