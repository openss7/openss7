/* do something with ``arg'' here and decide if won... */

    if (won) {
	register struct type_MODULE_result *res;

/* allocate and initialize the result ``res'' here... */

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) res, ROS_NOPRIO, roi)
		== NOTOK)
	    ros_adios (&roi -> roi_preject, "RESULT");

	free_MODULE_result (res);
    }
