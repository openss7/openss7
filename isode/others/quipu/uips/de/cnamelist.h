/* template.c - your comments here */


/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/cnamelist.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: cnamelist.h,v $
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


struct cnamelist {
  char *shortname;
  char *longname;
  struct cnamelist *next;
};

#define NULLCNLIST (struct cnamelist *)NULL
#define cname_alloc()   (struct cnamelist *)smalloc(sizeof (struct cnamelist))

void addToCoList();
void addCoNode();
char * mapCoName();
