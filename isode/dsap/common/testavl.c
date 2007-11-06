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

/* testavl.c - Test Tim Howes AVL code */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/testavl.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/testavl.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: testavl.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

#include <sys/types.h>
#include <stdio.h>

#include "manifest.h"
#include "quipu/util.h"
#include "quipu/attr.h"
#include "quipu/turbo.h"

main(argc, argv)
	int argc;
	char **argv;
{
	Avlnode *tree = NULLAVL;
	char command[10];
	char name[80];
	char *p;
	int free();

	printf("> ");
	while (fgets(command, sizeof(command), stdin) != NULL) {
		switch (*command) {
		case 'n':	/* new tree */
			(void) avl_free(tree, free);
			tree = NULLAVL;
			break;
		case 'p':	/* print */
			(void) myprint(tree);
			break;
		case 't':	/* traverse with first, next */
			printf("***\n");
			for (p = (char *) avl_getfirst(tree);
			     p != NULL; p = (char *) avl_getnext(tree, p))
				printf("%s\n", p);
			printf("***\n");
			break;
		case 'f':	/* find */
			printf("data? ");
			if (fgets(name, sizeof(name), stdin) == NULL)
				exit(0);
			name[strlen(name) - 1] = '\0';
			if ((p = (char *) avl_find(tree, name, strcmp))
			    == NULL)
				printf("Not found.\n\n");
			else
				printf("%s\n\n", p);
			break;
		case 'i':	/* insert */
			printf("data? ");
			if (fgets(name, sizeof(name), stdin) == NULL)
				exit(0);
			name[strlen(name) - 1] = '\0';
			if (avl_insert(&tree, strdup(name), strcmp, avl_dup_error) != OK)
				printf("\nNot inserted!\n");
			break;
		case 'd':	/* delete */
			printf("data? ");
			if (fgets(name, sizeof(name), stdin) == NULL)
				exit(0);
			name[strlen(name) - 1] = '\0';
			if (avl_delete(&tree, name, strcmp) == NULL)
				printf("\nNot found!\n");
			break;
		case 'q':	/* quit */
			exit(0);
			break;
		case '\n':
			break;
		default:
			printf("Commands: insert, delete, print, new, quit\n");
		}

		printf("> ");
	}
	/* NOTREACHED */
}

static
ravl_print(root, depth)
	Avlnode *root;
	int depth;
{
	int i;

	if (root == 0)
		return;

	ravl_print(root->avl_right, depth + 1);

	for (i = 0; i < depth; i++)
		printf("   ");
	printf("%s %d\n", root->avl_data, root->avl_bf);

	ravl_print(root->avl_left, depth + 1);
}

myprint(root)
	Avlnode *root;
{
	printf("********\n");

	if (root == 0)
		printf("\tNULL\n");
	else
		(void) ravl_print(root, 0);

	printf("********\n");
}
