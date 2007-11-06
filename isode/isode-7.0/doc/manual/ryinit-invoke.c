...

    int	    result,
	    response;
    struct type_MODULE_argument *arg;
    caddr_t out;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    /* allocate and initialize ``arg'' here... */

    switch (result = op_MODULE_operation (sd, arg, &out, &response, roi)) {
	case NOTOK:		/* failure */
	    if (ROS_FATAL (rop -> rop_reason))
		ros_adios (rop, "OPERATION");
	    ros_advise (rop, "OPERATION");
	    break;

	case OK:		/* got a result/error response */
	    switch (response) {
		case RY_RESULT:	/* got a result */
		    {
			register struct type_MODULE_result *res =
				    (struct type_MODULE_result *) out;

/* do something with ``res'' here... */

			free_MODULE_result (res);
			break;
		    }
	
		case error_MODULE_error1:
		    {
			register struct type_MODULE_parameter *parm =
				    (struct type_MODULE_parameter *) out;

/* do something with ``parm'' here... */

			free_MODULE_parameter (parm);
			break;
		    }

		/* other errors are handled here... */

		default:
		    adios (NULLCP, "unknown result/error returned (%d)",
				response);
		    /* NOTREACHED */
	    }
	    break;

	case DONE:		/* got RO-END? */
	    adios (NULLCP, "got RO-END.INDICATION");
	    /* NOTREACHED */

	default:
	    adios (NULLCP, "unknown return from RyOperation=%d", result);
	    /* NOTREACHED */
    }

    free_MODULE_argument (arg);

...
