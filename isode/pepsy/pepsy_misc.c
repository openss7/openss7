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

/* pepy_misc.c - PE parser (yacc-based) misc routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/pepsy/RCS/pepsy_misc.c,v 9.0 1992/06/16 12:24:03 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/pepsy/RCS/pepsy_misc.c,v 9.0 1992/06/16 12:24:03 isode Rel
 *
 *
 * Log: pepsy_misc.c,v
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

#include <ctype.h>
#include <stdio.h>
#include "pepsydefs.h"

/*  Oid manipulation */

typedef struct oidlist {
	OID op_oid;
	char *op_name;
	struct oidlist *op_next;
} oidlist, *OP;

#define NULLOP ((OP) 0)

typedef struct symtable {
	char *sym_name;
	char *sym_module;
	OID sym_oid;
	int sym_type;
	struct symtable *sym_next;
} symtable, *SYM;

#define NULLSYM ((SYM)0)

static OP myoids;
static SYM symtab[MAX_TBLS];

OID
addoid(o1, o2)
	OID o1, o2;
{
	OID noid;

	if (o1 == NULLOID || o2 == NULLOID)
		return NULLOID;

	noid = (OID) calloc(1, sizeof(*noid));
	if (noid == NULLOID)
		myyerror("out of memory (%d needed)", sizeof(*noid));

	noid->oid_nelem = o1->oid_nelem + o2->oid_nelem;
	noid->oid_elements = (unsigned int *) calloc((unsigned) noid->oid_nelem,
						     sizeof(unsigned int));
	if (noid->oid_elements == NULL)
		myyerror("out of memory (%d needed)", noid->oid_nelem);

	bcopy((char *) o1->oid_elements, (char *) noid->oid_elements,
	      o1->oid_nelem * sizeof(unsigned int));
	bcopy((char *) o2->oid_elements,
	      (char *) &noid->oid_elements[o1->oid_nelem], o2->oid_nelem * sizeof(unsigned int));
	return noid;
}

defineoid(name, oid)
	char *name;
	OID oid;
{
	register char *p;
	register OP op;

	if (oid == NULLOID) {
		myyerror("Warning Null oid in defineoid");
		return;
	}
	for (op = myoids; op; op = op->op_next)
		if (strcmp(op->op_name, name) == 0) {
			if (oid_cmp(op->op_oid, oid) != 0) {
				p = new_string(sprintoid(oid));
				warning("OID name clash %s => %s & %s",
					name, p, sprintoid(op->op_oid));
				free(p);
			} else
				return;
		}
	op = (OP) calloc(1, sizeof *op);
	if (op == NULLOP)
		myyerror("out of memory (%d needed)", sizeof(*op));
	op->op_oid = oid_cpy(oid);
	op->op_name = new_string(name);
	op->op_next = myoids;
	myoids = op;
}

OID
oidlookup(name)
	char *name;
{
	OP op;

	for (op = myoids; op; op = op->op_next)
		if (strcmp(name, op->op_name) == 0)
			return oid_cpy(op->op_oid);

	warning("unknown Object Identifier '%s'", name);
	return NULLOID;
}

char *
oidname(oid)
	OID oid;
{
	OP op;

	for (op = myoids; op; op = op->op_next)
		if (oid_cmp(op->op_oid, oid) == 0)
			return op->op_name;

	return NULLCP;
}

OID
int2oid(n)
	int n;
{
	OID noid;

	noid = (OID) calloc(1, sizeof(*noid));
	if (noid == NULLOID)
		myyerror("out of memory (%d needed)", sizeof *noid);

	noid->oid_elements = (unsigned int *) calloc(1, sizeof(unsigned int));
	if (noid->oid_elements == NULL)
		myyerror("out of memory (%d needed)", sizeof(unsigned int));
	noid->oid_nelem = 1;
	noid->oid_elements[0] = n;
	return noid;
}

/*  */

addtable(name, lt, typ)
	char *name;
	int lt;
	int typ;			/* Does it allow implicit's to work or not */
{
	SYM sp;

	sp = (SYM) calloc(1, sizeof *sp);
	sp->sym_name = new_string(name);
	sp->sym_next = symtab[lt];
	sp->sym_type = typ;
	symtab[lt] = sp;
}

addtableref(name, id, lt)
	char *name;
	OID id;
	int lt;
{
	SYM sp;
	char *nm;
	OID oid;

	nm = name ? new_string(name) : NULLCP;
	oid = id ? oid_cpy(id) : NULLOID;

	for (sp = symtab[lt]; sp; sp = sp->sym_next)
		if (sp->sym_module == NULLCP && sp->sym_oid == NULLOID) {
			sp->sym_module = nm;
			sp->sym_oid = oid;
		}
}

print_expimp()
{
	SYM sp;
	int ind;
	OID oid;
	char *p;

	if (sp = symtab[TBL_EXPORT])
		(void) printf("\nEXPORTS\n");

	for (ind = 0; sp; sp = sp->sym_next) {
		if (ind == 0) {
			(void) putchar('\t');
			ind = 8;
		}
		(void) printf("%s", sp->sym_name);
		ind += strlen(sp->sym_name);
		if (sp->sym_next) {
			(void) putchar(',');
			ind++;
		} else
			(void) putchar(';');
		if (ind > 72) {
			(void) putchar('\n');
			ind = 0;
		} else {
			(void) putchar(' ');
			ind++;
		}
	}
	(void) putchar('\n');

	if (sp = symtab[TBL_IMPORT]) {
		(void) printf("\nIMPORTS\n");
		p = sp->sym_module;
		oid = sp->sym_oid;
	}
	for (ind = 0; sp; sp = sp->sym_next) {
		if (ind == 0) {
			(void) putchar('\t');
			ind = 8;
		}
		(void) printf("%s", sp->sym_name);
		ind += strlen(sp->sym_name);
		if (sp->sym_next) {
			if (strcmp(p, sp->sym_next->sym_module) == 0) {
				(void) putchar(',');
				ind++;
				if (ind > 72) {
					(void) putchar('\n');
					ind = 0;
				} else {
					(void) putchar(' ');
					ind++;
				}
			} else {
				if (ind != 8)
					(void) printf("\n\t\t");
				else
					(void) putchar('\t');
				(void) printf("FROM %s", p);
				if (oid)
					(void) printf(" %s", oidprint(oid));
				(void) printf("\n\t");
				ind = 8;
				p = sp->sym_next->sym_module;
				oid = sp->sym_next->sym_oid;
			}
		} else {
			if (ind != 8)
				(void) printf("\n\t\t");
			else
				(void) putchar('\t');
			(void) printf("FROM %s", p);
			if (oid)
				(void) printf(" %s", oidprint(oid));
			(void) printf(";\n");
		}
	}
}

check_impexp(yp)
	YP yp;
{
	SYM sp;

	for (sp = symtab[TBL_EXPORT]; sp; sp = sp->sym_next)
		if (strcmp(sp->sym_name, yp->yp_identifier) == 0) {
			yp->yp_flags |= YP_EXPORTED;
			break;
		}

	for (sp = symtab[TBL_IMPORT]; sp; sp = sp->sym_next)
		if (strcmp(sp->sym_name, yp->yp_identifier) == 0) {
			if (yp->yp_flags & YP_EXPORTED)
				myyerror("Warning: %s imported & exported!", yp->yp_identifier);
			yp->yp_module = sp->sym_module;
			yp->yp_modid = sp->sym_oid;
/*	    yp -> yp_flags |= YP_IMPORTED;	*/
		}
}
static struct oidtbl {
	char *oid_name;
	int oid_value;
} oidtable[] = {
	/* Top level OIDS */
"ccitt", 0, "iso", 1, "joint-iso-ccitt", 2, NULL,};

initoidtbl()
{
	struct oidtbl *op;
	OID oid;

	for (op = oidtable; op->oid_name; op++) {
		defineoid(op->oid_name, oid = int2oid(op->oid_value));
		oid_free(oid);
	}
}

char *
oidprint(oid)
	OID oid;
{
	static char buf[BUFSIZ];
	char *cp;
	char *p;
	OID o2;
	unsigned int *ip;
	int i;

	if (oid == NULLOID)
		return "";

	(void) strcpy(buf, "{ ");
	cp = buf + strlen(buf);

	i = oid->oid_nelem;
	ip = oid->oid_elements;

	p = oidname(o2 = int2oid((int) *ip));
	oid_free(o2);
	if (p) {
		i--;
		ip++;
		(void) sprintf(cp, "%s ", p);
		cp += strlen(cp);
	}

	for (; i > 0; i--) {
		(void) sprintf(cp, "%d ", *ip++);
		cp += strlen(cp);
	}

	(void) strcat(cp, " }");
	return buf;
}

/*
 * look at import list and return any clue found as to handling implicit tags
 * on that type
 */
chkil(id)
	char *id;
{
	SYM sy;

	for (sy = symtab[TBL_IMPORT]; sy; sy = sy->sym_next)
		if (strcmp(sy->sym_name, id) == 0)
			break;
	if (sy)
		return (sy->sym_type);

	return (ER_UNKNOWN);

}
