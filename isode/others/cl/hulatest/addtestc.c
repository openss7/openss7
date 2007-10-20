
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

#include "ADD-ops.h"            /* ADD operation definitions */
#include "ADD-types.h"		/* ADD type definitions */


#ifdef	SYS5
struct passwd *getpwuid ();
#endif

/*    DATA */

static char *myservice = "addtest";	
static char *mycontext = "addtest context";
static char *mypci =     "addtest pci";


extern int length;

					/* ARGUMENTS */
int	addit_arg (), do_help (), do_quit ();
					/* RESULTS */
int	addit_result ();
					/* ERRORS */
int	addtest_error ();

char    *getlocalhost();


static struct dispatch dispatches[] = {

    "addtest",	operation_ADD_addit,
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


char   *ctime ();

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    ryinitiator (argc, argv, myservice, mycontext, mypci,
		 table_ADD_Operations, dispatches, do_quit);

    exit (0);			/* NOTREACHED */
}


/*    ARGUMENTS */

/* ARGSUSED */

static int  addit_arg (sd, ds, args, ppaddends)
int	sd;
struct dispatch *ds;
char  **args;
struct type_ADD_Addends **ppaddends;
{
    struct type_ADD_Addends *paddends;

    if ((paddends = (struct type_ADD_Addends *) calloc (1, sizeof *paddends))
	    == NULL)
	adios (NULLCP, "out of memory");
    paddends->addend1 = 3;
    paddends->addend2 = 2;
    *ppaddends = paddends; 
    return OK;
}

/*  */

/* ARGSUSED */

static int  do_help (sd, ds, args, dummy)
int	sd;
register struct dispatch *ds;
char  **args;
caddr_t *dummy;
{
    printf ("\nCommands are:\n");
    for (ds = dispatches; ds -> ds_name; ds++)
	printf ("%s\t%s\n", ds -> ds_name, ds -> ds_help);

    return NOTOK;
}

/*  */

/* ARGSUSED */

static int  do_quit (sd, ds, args, dummy)
int	sd;
struct dispatch *ds;
char  **args;
caddr_t *dummy;
{
struct AcSAPindication acis;
struct AcSAPindication *aci = &acis;

/* should do the AcUnitDataUnbind(); */
    if (AcUnitDataUnbind ( sd, aci ) != OK )
	printf ("\n could not unbind\n");
    exit (0);
}



/*    RESULTS */

/* ARGSUSED */

static int  addit_result (sd, id, dummy, result, roi)
int	sd,
    	id,
    	dummy;
register struct type_ADD_Sum *result;
struct RoSAPindication *roi;
{
    printf ("\n sum = %d\n", result->parm ); 
    printf ("\n");
    return OK;
}


/*    ERRORS */

/* ARGSUSED */

static int  addtest_error (sd, id, error, parameter, roi)
int	sd,
	id,
    	error;
char *parameter; /*???*/
struct RoSAPindication *roi;
{    
    register struct RyError *rye;

    if (error == RY_REJECT) {
	advise (NULLCP, "%s", RoErrString ((int) parameter));
	return OK;
    }

    if (rye = finderrbyerr (table_ADD_Errors, error))
	advise (NULLCP, "%s",  rye -> rye_name);
    else
	advise (NULLCP, "Error %d", error);

/*
    if (parameter)
	print_ia5list(parameter);
*/

    return OK;
}
