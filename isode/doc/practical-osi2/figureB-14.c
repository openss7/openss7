    else {
	struct type_MODULE_parameter *parm;

/* allocate and initialize ``parm'' here... */

	if (RyDsError (sd, rox -> rox_id, error_MODULE_error1, (caddr_t) parm,
		       ROS_NOPRIO, roi) == NOTOK)
	    ros_adios (&roi -> roi_preject, "ERROR");

	free_MODULE_parameter (parm);
    }

    /* ``arg'' will be automatically free'd when op_name returns */

    return OK;
}
