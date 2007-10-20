...

int	do_result (), do_error ();

...

    int	    result;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    /* allocate and initialize ``arg'' here... */

    switch (result = stub_MODULE_operation (sd, RyGenID (sd), arg, do_result,
					    do_error, ROS_SYNC, roi)) {
	case NOTOK:		/* failure */
	    if (ROS_FATAL (rop -> rop_reason))
		ros_adios (rop, "STUB");
	    ros_advise (rop, "STUB");
	    break;

	case OK:
	    break;

	case DONE:		/* got RO-END? */
	    adios (NULLCP, "got RO-END.INDICATION");
	    /* NOTREACHED */

	default:
	    adios (NULLCP, "unknown return from RyStub=%d", result);
	    /* NOTREACHED */
    }

    free_MODULE_argument (arg);

...

do_result (sd, id, dummy, result, roi)
int	sd;
int	id,
	dummy;
struct type_MODULE_result *result;
struct RoSAPindication *roi;
{
    /* do something with ``result'' here... */

    /* ``result'' will be automatically free'd when do_result returns */

    return OK;
}

...


do_error (sd, id, error, parameter, roi)
int	sd;
int	id,
	error;
caddr_t parameter;
struct RoSAPindication *roi;
{
    switch (error) {
	case error_MODULE_error1:
	{
	    register struct type_MODULE_parameter *parm =
			    (struct type_MODULE_parameter *) parameter;

	    /* do something with ``parm'' here... */

	    break;
	}

	/* other errors are handled here... */
	

	default:
	    adios (NULLCP, "unknown error returned (%d)", error);
	    /* NOTREACHED */
    }
    
    /* ``parameter'' will be automatically free'd when do_error returns */

    return OK;
}

...
