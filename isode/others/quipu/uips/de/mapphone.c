/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/mapphone.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/mapphone.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: mapphone.c,v $
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
#include "mapphone.h"
#include "quipu/util.h"

extern struct mapphonelist * mapphonelp;
static struct mapphonelist * tailplp;

void
addToPhoneList(str)
char *str;
{
char * cp;

  cp = index(str, ':');
  if (cp == NULLCP)
  {
    (void) fprintf(stderr, "log an error message about parsing of phone numbermappings...\n");
    return;
  }
  *cp = '\0';
  cp++;
  addPhoneNode(str, cp);
}

void
addPhoneNode(from, to)
char * from, * to;
{
struct mapphonelist * mplp;

  mplp = mapphone_alloc();
  if (mapphonelp != NULLPHLIST)
  {
    tailplp->next = mplp;
    tailplp = mplp;
  }
  else
    mapphonelp = tailplp = mplp;
  mplp->mapfrom = copy_string(from);
  mplp->mapto = copy_string(to);
  mplp->next = NULLPHLIST;
}

char *
mapPhone(from)
char * from;
{
struct mapphonelist * mplp;
static char tophone[LINESIZE];

  for (mplp = mapphonelp; mplp != NULLPHLIST; mplp = mplp->next)
    if (strncmp(from, mplp->mapfrom, strlen(mplp->mapfrom)) == 0)
    {
      (void) strcpy(tophone, mplp->mapto);
      (void) strcat(tophone, from + strlen(mplp->mapfrom));
      return tophone;
    }
  return from;
}
