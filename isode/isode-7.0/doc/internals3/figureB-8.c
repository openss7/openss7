...

    int	    result;
    struct type_MODULE_argument *arg;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;

    /* allocate and initialize ``arg'' here... */

    if ((result = stub_MODULE_operation (sd, RyGenID (sd), arg, do_result,
					 do_error, ROS_ASYNC, roi)) != OK)
	ros_lose (result, roi);

    free_MODULE_argument (arg);

...

