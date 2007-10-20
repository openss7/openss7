/* mine.h */

/* 
 * $Header: /xtel/isode/isode/pepsy/RCS/mine.h,v 9.0 1992/06/16 12:24:03 isode Rel $
 *
 *
 * $Log: mine.h,v $
 * Revision 9.0  1992/06/16  12:24:03  isode
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


#define TABLESIZE 29

typedef struct ID_TABLE {
	char	*h_value;
	char	*r_value;
	int	def_bit;
	int	def_value;
	int	count;
	struct ID_TABLE	*next;
	} id_entry;

typedef struct S_TABLE {
	char	*name;
	char	*type;
	struct S_TABLE	*parent;
	char	*field;
	int	defined;
	struct S_TABLE *next;
	} s_table;

extern	id_entry	*id_table[];

extern char *c_flags();
