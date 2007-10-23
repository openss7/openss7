...

    int	    result;
    struct type_MODULE_argument *arg;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject *rop = &roi -> roi_preject;

    /* allocate and initialize ``arg'' here... */

    switch (result = stub_MODULE_operation (sd, RyGenID (sd), arg, do_result,
					    do_error, ROS_ASYNC, roi)) {
	case NOTOK:		/* local failure */
	    if (ROS_FATAL (rop -> rop_reason))
		ros_adios (rop, "STUB");
	    ros_advise (rop, "STUB");
	    break;

	case OK:
	    break;

...
