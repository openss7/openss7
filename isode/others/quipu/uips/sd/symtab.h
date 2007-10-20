/*
 * $Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/symtab.h,v 9.0 1992/06/16 12:45:08 isode Rel $
 */

/*
 * $Log: symtab.h,v $
 * Revision 9.0  1992/06/16  12:45:08  isode
 * Release 8.0
 *
 */

#ifndef SYMTAB
#define SYMTAB


typedef struct tab_entry {
  	char *val;
	char *name;
	struct tab_entry *next;
      } *table_entry;

#define NULLSYM (table_entry) 0

#endif
