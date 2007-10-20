
/*
 ****************************************************************
 *                                                              *
 *  ISODECL  -  HULA project for connectionless ISODE           *
 *                                             			*
 *  module:  	addtestc.c                                      *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements the server for the simple add service. *
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
/* HULA */
/* addtestd.c - trivial add network service -- responder */

#include <ctype.h>
#include <stdio.h>
#include "ryresponder.h"	/* for generic idempotent responders */

#include "ADD-ops.h"	        /* ADD operation definitions */
#include "ADD-types.h"	        /* ADD type definitions */

/*    DATA */

static char *myservice = "addtest";
static char *mycontext = "addtest context";
static char *mypci     = "addtest pci";

					/* OPERATIONS */
int	op_addit ();

static struct dispatch dispatches[] = {
    "addit", operation_ADD_addit, op_addit,
    NULL
};

					/* TYPES */
extern int  errno;
char	*getlocalhost();


/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    ryresponder (argc, argv, getlocalhost(), myservice, mycontext, mypci,
		  dispatches, table_ADD_Operations, NULLIFP, NULLIFP);

    exit (0);			/* NOTREACHED */
}



/*    OPERATIONS */

/* ARGSUSED */

static int  op_addit (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
    register struct type_ADD_Sum *psum;
    register struct type_ADD_Addends *paddends;

    if (rox -> rox_nolinked == 0) {
	advise (NULLCP, LOG_INFO,
		"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
		sd, ryo -> ryo_name, rox -> rox_linkid);
	return ureject (sd, ROS_IP_LINKED, rox, roi);
    }
    if (debug)
	advise (NULLCP, LOG_DEBUG, "RO-INVOKE.INDICATION/%d: %s",
		sd, ryo -> ryo_name);

    if ((psum = (struct type_ADD_Sum *) calloc (1, sizeof *psum)) == NULL
	|| in == NULL )
	return error (sd, error_ADD_cantadd, (caddr_t) NULL,
		    rox, roi);

    paddends = (struct type_ADD_Addends *)in;
    psum->parm = paddends->addend1 + paddends->addend2;

    if (RyDsResult (sd, rox -> rox_id, (caddr_t)psum, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "RESULT");

    free_ADD_Sum (psum);

    return OK;
}


/*    ERROR */

static int  error (sd, err, param, rox, roi)
int	sd,
	err;
caddr_t	param;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "ERROR");

    return OK;
}

/*    U-REJECT */

static int  ureject (sd, reason, rox, roi)
int	sd,
	reason;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsUReject (sd, rox -> rox_id, reason, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "U-REJECT");

    return OK;
}
