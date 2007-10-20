/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/mapphone.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: mapphone.h,v $
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

struct mapphonelist {
  char *mapfrom;
  char *mapto;
  struct mapphonelist *next;
};

#define NULLPHLIST (struct mapphonelist *)NULL
#define mapphone_alloc()   (struct mapphonelist *)smalloc(sizeof (struct mapphonelist))

void addToPhoneList();
void addPhoneNode();
char * mapPhone();
