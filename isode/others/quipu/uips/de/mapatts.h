/* template.c - your comments here */


/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/mapatts.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: mapatts.h,v $
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


struct mapnamelist {
  char *tablename;
  char *nicename;
  struct mapnamelist *next;
};

#define NULLMNLIST (struct mapnamelist *)NULL
#define mapname_alloc()   (struct mapnamelist *)smalloc(sizeof (struct mapnamelist))

void addToAttList();
void addAttNode();
char * mapAttName();
