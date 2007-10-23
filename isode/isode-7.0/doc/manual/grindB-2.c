#include <stdio.h>
#include "generic.h"		/* defines OPERATIONS and ERRORS */
#include <isode/rosap.h>
#include <isode/isoservent.h>


int	ros_indications ();


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int	    result,
	    sd;
    struct RoSAPstart   ross;
    register struct RoSAPstart *ros = &ross;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (RoInit (argc, argv, ros, roi) == NOTOK)
	error ("initialization fails: %s", RoErrString (rop -> rop_reason));

    sd = ros -> ros_sd;
/*  do authentication using ros -> ros_data here... */
    ROSFREE (ros);
    
/* read command line arguments here... */

/* could use ROS_VALIDATE or ROS_BUSY instead and then exit */

    if (RoBeginResponse (sd, ROS_ACCEPT, NULLPE, roi) == NOTOK)
	error ("RO-BEGIN.RESPONSE: %s", RoErrString (rop -> rop_reason));

    if (RoSetIndications (sd, ros_indication, roi) == NOTOK)
	error ("RoSetIndications: %s", RoErrString (rop -> rop_reason));

    for (;;)
	pause ();
}
