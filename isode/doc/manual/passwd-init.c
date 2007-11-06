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

/* lookup.c - password lookup service -- initiator */

#include <stdio.h>
#include "ryinitiator.h"	/* for generic interctive initiators */
#include "PasswordLookup-ops.h"	/* operation definitions */
#include "PasswordLookup-types.h"	/* type definitions */

/* DATA */

static char *myservice = "passwdstore";

static char *mycontext = "isode passwd lookup demo";
static char *mypci = "isode passwd lookup demo pci";

					/* ARGUMENTS */
int do_lookupUser(), do_lookupUID(), do_help(), do_quit();

					/* RESULTS */
int lookup_result();

					/* ERRORS */
int lookup_error();

static struct dispatch dispatches[] = {
	"lookupUser", operation_PasswordLookup_lookupUser,
	do_lookupUser, free_PasswordLookup_UserName,
	lookup_result, lookup_error,
	"find user by name",

	"lookupUID", operation_PasswordLookup_lookupUID,
	do_lookupUID, free_PasswordLookup_UserID,
	lookup_result, lookup_error,
	"find user by id",

	"help", 0,
	do_help, NULLIFP,
	NULLIFP, NULLIFP,
	"print this information",

	"quit", 0,
	do_quit, NULLIFP,
	NULLIFP, NULLIFP,
	"terminate the association and exit",

	NULL
};

/* MAIN */

/* ARGSUSED */

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	(void) ryinitiator(argc, argv, myservice, mycontext, mypci,
			   table_PasswordLookup_Operations, dispatches, do_quit);

	exit(0);		/* NOTREACHED */
}

/* ARGUMENTS */

/* ARGSUSED */

static int
do_lookupUser(sd, ds, args, arg)
	int sd;
	struct dispatch *ds;
	char **args;
	register struct type_PasswordLookup_UserName **arg;
{
	char *cp;

	if ((cp = *args++) == NULL) {
		advise(NULLCP, "usage: lookupUser username");
		return NOTOK;
	}

	if ((*arg = str2qb(cp, strlen(cp), 1)) == NULL)
		adios(NULLCP, "out of memory");

	return OK;
}

/*  */

/* ARGSUSED */

static int
do_lookupUID(sd, ds, args, arg)
	int sd;
	struct dispatch *ds;
	char **args;
	register struct type_PasswordLookup_UserID **arg;
{
	char *cp;

	if ((cp = *args++) == NULL) {
		advise(NULLCP, "usage: lookupUID userid");
		return NOTOK;
	}

	if ((*arg = (struct type_PasswordLookup_UserID *) calloc(1, sizeof **arg))
	    == NULL)
		adios(NULLCP, "out of memory");

	(*arg)->parm = atoi(cp);

	return OK;
}

/*  */

/* ARGSUSED */

static int
do_help(sd, ds, args, dummy)
	int sd;
	register struct dispatch *ds;
	char **args;
	caddr_t *dummy;
{
	printf("\nCommands are:\n");
	for (ds = dispatches; ds->ds_name; ds++)
		printf("%s\t%s\n", ds->ds_name, ds->ds_help);

	return NOTOK;
}

/*  */

/* ARGSUSED */

static int
do_quit(sd, ds, args, dummy)
	int sd;
	struct dispatch *ds;
	char **args;
	caddr_t *dummy;
{
	struct AcSAPrelease acrs;
	register struct AcSAPrelease *acr = &acrs;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;

	if (AcRelRequest(sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
		acs_adios(aca, "A-RELEASE.REQUEST");

	if (!acr->acr_affirmative) {
		(void) AcUAbortRequest(sd, NULLPEP, 0, aci);
		adios(NULLCP, "release rejected by peer: %d", acr->acr_reason);
	}

	ACRFREE(acr);

	exit(0);
}

/* RESULTS */

/* ARGSUSED */

static int
lookup_result(sd, id, dummy, result, roi)
	int sd, id, dummy;
	register struct type_PasswordLookup_Passwd *result;
	struct RoSAPindication *roi;
{
	print_qb(result->name);
	putchar(':');
	print_qb(result->passwd);
	printf(":%d:%d:", result->uid->parm, result->gid->parm);
	print_qb(result->gecos);
	putchar(':');
	print_qb(result->dir);
	putchar(':');
	print_qb(result->shell);
	putchar('\n');

	return OK;
}

static
print_qb(q)
	register struct qbuf *q;
{
	register struct qbuf *p;

	if (q == NULL)
		return;

	for (p = q->qb_forw; p != q; p = p->qb_forw)
		printf("%*.*s", p->qb_len, p->qb_len, p->qb_data);
}

/* ERRORS */

/* ARGSUSED */

static int
lookup_error(sd, id, error, parameter, roi)
	int sd, id, error;
	caddr_t parameter;
	struct RoSAPindication *roi;
{
	register struct RyError *rye;

	if (error == RY_REJECT) {
		advise(NULLCP, "%s", RoErrString((int) parameter));
		return OK;
	}

	if (rye = finderrbyerr(table_PasswordLookup_Errors, error))
		advise(NULLCP, "%s", rye->rye_name);
	else
		advise(NULLCP, "Error %d", error);

	return OK;
}
