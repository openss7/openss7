
#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/symtab.c,v 9.0 1992/06/16 12:45:08 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/symtab.c,v 9.0 1992/06/16 12:45:08 isode Rel $
 *
 * $Log: symtab.c,v $
 * Revision 9.0  1992/06/16  12:45:08  isode
 * Release 8.0
 *
 */


#include "general.h"
#include "symtab.h"

put_symbol_value(table, name, val)
table_entry table;
char *name;
char *val;
{
	if (!name) return;

  	while(table && strcmp(name, table->name)) {
		table = table->next;
	}
	if (table) {
		free(table->val);
		if (val) {
		  	table->val = 
			  (char *) malloc((unsigned) strlen(val) + 1);
			(void) strcpy(table->val, val);
		} else
		  	table->val = (char *) 0;
	} else {
		table = (table_entry) malloc(sizeof(table_entry));
		table->next = NULLSYM;
		table->name = (char *) malloc((unsigned) strlen(name) + 1);
		(void) strcpy(table->name, name);
                if (val) {
                        table->val = 
			  (char *) malloc((unsigned) strlen(val) + 1);
                        (void) strcpy(table->val, val);
	        } else
                        table->val = 0;
	}
}
		
char *
get_symbol_value(table, name)
table_entry table;
char *name;
{
  	while(table && strcmp(name, table->name)) table = table->next;
  	if (table)
	  	return table->val;
	return (char *) 0;
}
	  	

free_table(table)
table_entry table;
{
  	table_entry  entry;

  	while(table) {
	  	if (table->val)
		  	free(table->val);
		free(table->name);
		entry = table;
		table = table->next;
		free((char *) entry);
	}
}

