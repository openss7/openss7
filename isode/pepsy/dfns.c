/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* dfns.c */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/pepsy/RCS/dfns.c,v 9.0 1992/06/16 12:24:03 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/pepsy/RCS/dfns.c,v 9.0 1992/06/16 12:24:03 isode Rel
 *
 *
 * Log: dfns.c,v
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

#include <stdio.h>
#include "general.h"
#include "mine.h"
#include "pepsydefs.h"
#include "pass2.h"

id_entry *id_table[TABLESIZE];

#define my_error(mesg)	((void)fprintf(stderr, "%s\n",mesg),exit(1))

extern char *notidtoid(), *my_new_str(), *my_strcat();
extern char *proc_name(), *mymodule;

/*
 * Lookup the hash table (id_table) for the string t and insert it at
 * the start of the appropriate chain if it is not already there.
 * The argument flag indicates whether t is being defined (1) or used
 * (0).
 */
char *
proc_name(t, flag)
	char *t;
	int flag;
{
	int i;
	static int curr = 0;
	id_entry *ptr;

	i = hash_val(t);
	for (ptr = id_table[i]; ptr != NULL && strcmp(t, ptr->h_value); ptr = ptr->next) ;

	if (ptr == NULL) {
		if ((ptr = (id_entry *) malloc(sizeof(id_entry))) == NULL)
			my_error("proc_name: Out of memory");
		ptr->h_value = t;
		ptr->r_value = my_strcat(notidtoid(t), notidtoid(mymodule));
		ptr->count = 1;
		if (flag) {
			ptr->def_bit = flag;
			ptr->def_value = curr++;
		}
		ptr->next = id_table[i];
		id_table[i] = ptr;
	} else if (!ptr->def_bit)
		ptr->def_bit = flag;

	return ptr->r_value;
}

/*
 * output a sequence of #define statements (one for each value stored
 * in the hash table) to the file specified by fp
 */
out_final_defs(fp)
	FILE *fp;
{
	int j;
	id_entry *ptr;

	for (j = 0; j < TABLESIZE; j++)
		for (ptr = id_table[j]; ptr != NULL; ptr = ptr->next) {
			if (ptr->def_bit)
				(void) fprintf(fp, "#define %s%s\t%d\n", PREFIX, ptr->r_value,
					       ptr->def_value);
			else
				ferrs(0, "the identifier %s is used but not defined\n",
				      ptr->h_value);
			if (ptr->count > 1)	/* not used */
				(void) printf("The id %s has a count of %d\n", ptr->r_value,
					      ptr->count);
		}
}

/*
 * return a copy of the string s with '-' replaced by '_'
 */
char *
notidtoid(s)
	char *s;
{

	char *t, *r;

	t = my_new_str(s);
	for (r = t; *r != '\0'; r++)
		if (*r == '-')
			*r = '_';
	return t;
}

/*
 * return a copy of the string s
 */
char *
my_new_str(s)
	char *s;
{

	char *t;

	if ((t = (char *) malloc((unsigned) strlen(s) + 1)) == NULL)
		my_error("my_new_str: Out of memory");

	(void) strcpy(t, s);
	return t;
}

/*
 * return the concatenation of the strings s1 and s2
 */
char *
my_strcat(s1, s2)
	char *s1, *s2;
{
	char *s3, *s, *t;

	if (s1 == NULL || *s1 == '\0')
		return my_new_str(s2);

	if ((s3 = (char *) malloc((unsigned) (strlen(s1) + strlen(s2) + 1))) == NULL)
		my_error("my_strcat: Out of memory");

	for (s = s1, t = s3; *s != '\0'; s++, t++)
		*t = *s;
	(void) strcpy(t, s2);
	return s3;
}

/*
 * a simple hash function
 */
hash_val(s)
	char *s;
{
	int i, sum;
	char *t;

	sum = 0;
	for (i = 1, t = s; *t != '\0'; i++, t++)
		sum = sum + *t * i;
	return (sum % TABLESIZE);
}

/*
 * initialize the table id_table
 */
init()
{
	int i;

	for (i = 0; i <= TABLESIZE; i++)
		id_table[i] = NULL;
}

#define BUFSIZE		128
#define MAX(a, b)	((a) > (b) ? (a) : (b))

static char *buf = NULL;
static unsigned int len = 0;

/*
 * Return in a static buffer the two strings concatenated
 */
char *
concat(s1, s2)
	char *s1, *s2;
{
	int tot;

	tot = strlen(s1) + strlen(s2) + 1;

	if (tot > len) {
		len = MAX(BUFSIZE, tot);
		if (buf == NULL) {
			if ((buf = malloc(len)) == NULL)
				ferr(1, "concat:malloc failed\n");
		} else if ((buf = realloc(buf, len)) == NULL)
			ferr(1, "concat:realloc failed\n");
	}
	(void) strcpy(buf, s1);
	(void) strcat(buf, s2);

	return (buf);
}

/*
 * Generate a free call given the name of the parameter, the module
 * name, and the name of the type
 */
char *
gfree(module, id, parm)
	char *module;			/* name of module we are in (usually mymodule) */
	char *id;			/* name of type we want to free */
	char *parm;			/* name of the pointer to the data */
{
	char *p1 = notidtoid(module);
	char *p2 = notidtoid(id);
	int tot;

	tot = 3 * strlen(p1) + 2 * strlen(p2) + BUFSIZE;

	if (tot > len) {
		len = MAX(BUFSIZE, tot);
		if (buf == NULL) {
			if ((buf = malloc(len)) == NULL)
				ferr(1, "concat:malloc failed\n");
		} else if ((buf = realloc(buf, len)) == NULL)
			ferr(1, "concat:realloc failed\n");
	}
	(void) sprintf(buf, "(void) fre_obj((char *) %s, %s%s%s.md_dtab[%s%s%s], &%s%s%s, 1)",
		       parm, PREFIX, p1, MODTYP_SUFFIX, PREFIX, p2, p1, PREFIX, p1, MODTYP_SUFFIX);
	free(p1);
	free(p2);

	return (buf);
}
