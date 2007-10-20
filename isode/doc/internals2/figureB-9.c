...

    int	    result,
	    response;
    struct type_MODULE_argument *arg;
    caddr_t out;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject *rop = &roi -> roi_preject;

    /* allocate and initialize ``arg'' here... */

    switch (result = op_MODULE_operation (sd, arg, &out, &response, roi)) {
	case NOTOK:		/* failure */
	    if (ROS_FATAL (rop -> rop_reason))
		ros_adios (rop, "OPERATION");
	    ros_advise (rop, "OPERATION");
	    break;
