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


/*
 ****************************************************************
 *                                                              *
 *  ISODECL  -  HULA project for connectionless ISODE           *
 *                                             			*
 *  module:  	addtestc.c                                      *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements the client for the simple add service. *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *      main ()				                        *
 *                                                              *
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*    
 ****************************************************************
*/
/* addtestc.c - add 2 numbers network trivial service -- initiator */

#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include "ryinitiator.h"	/* for generic interactive initiators */

#include "ADD-ops.h"		/* ADD operation definitions */
#include "ADD-types.h"		/* ADD type definitions */

#ifdef	SYS5
struct passwd *getpwuid();
#endif

/*    DATA */

static char *myservice = "addtest";
static char *mycontext = "addtest context";
static char *mypci = "addtest pci";

extern int length;

					/* ARGUMENTS */
int addit_arg(), do_help(), do_quit();

					/* RESULTS */
int addit_result();

					/* ERRORS */
int addtest_error();

char *getlocalhost();

static struct dispatch dispatches[] = {

	"addtest", operation_ADD_addit,
	addit_arg, free_ADD_Addends,
	addit_result, addtest_error,
	"the sum of two addends",

	"help", 0,
	do_help, NULLIFP,
	NULLIFP, NULLIFP,
	"print this information",

	"quit", 0,
	do_quit, NULLIFP,
	NULLIFP, NULLIFP,
	"terminate the virtual association and exit",

	NULL
};

char *ctime();

/*    MAIN */

/* ARGSUSED */

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	ryinitiator(argc, argv, myservice, mycontext, mypci,
		    table_ADD_Operations, dispatches, do_quit);

	exit(0);		/* NOTREACHED */
}

/*    ARGUMENTS */

/* ARGSUSED */

static int
addit_arg(sd, ds, args, ppaddends)
	int sd;
	struct dispatch *ds;
	char **args;
	struct type_ADD_Addends **ppaddends;
{
	struct type_ADD_Addends *paddends;

	if ((paddends = (struct type_ADD_Addends *) calloc(1, sizeof *paddends))
	    == NULL)
		adios(NULLCP, "out of memory");
	paddends->addend1 = 3;
	paddends->addend2 = 2;
	*ppaddends = paddends;
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
	struct AcSAPindication acis;
	struct AcSAPindication *aci = &acis;

/* should do the AcUnitDataUnbind(); */
	if (AcUnitDataUnbind(sd, aci) != OK)
		printf("\n could not unbind\n");
	exit(0);
}

/*    RESULTS */

/* ARGSUSED */

static int
addit_result(sd, id, dummy, result, roi)
	int sd, id, dummy;
	register struct type_ADD_Sum *result;
	struct RoSAPindication *roi;
{
	printf("\n sum = %d\n", result->parm);
	printf("\n");
	return OK;
}

/*    ERRORS */

/* ARGSUSED */

static int
addtest_error(sd, id, error, parameter, roi)
	int sd, id, error;
	char *parameter;		/* ??? */
	struct RoSAPindication *roi;
{
	register struct RyError *rye;

	if (error == RY_REJECT) {
		advise(NULLCP, "%s", RoErrString((int) parameter));
		return OK;
	}

	if (rye = finderrbyerr(table_ADD_Errors, error))
		advise(NULLCP, "%s", rye->rye_name);
	else
		advise(NULLCP, "Error %d", error);

/*
    if (parameter)
	print_ia5list(parameter);
*/

	return OK;
}
