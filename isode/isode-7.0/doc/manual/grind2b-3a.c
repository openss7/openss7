...

static int ros_indication (sd, roi)
int     sd;
register struct RoSAPindication *roi;
{
    switch (roi -> roi_type) {
	case ROI_INVOKE: 
	    ros_invoke (sd, &roi -> roi_invoke);
	    break;

	case ROI_RESULT: 
	    ros_result (sd, &roi -> roi_result);
	    break;

	case ROI_ERROR: 
	    ros_error (sd, &roi -> roi_error);
	    break;

	case ROI_UREJECT: 
	    ros_ureject (sd, &roi -> roi_ureject);
	    break;

	case ROI_PREJECT: 
	    ros_preject (sd, &roi -> roi_preject);
	    break;

	case ROI_FINISH: 
	    ros_finish (sd, &roi -> roi_finish);
	    break;

	default: 
	    error ("unknown indication type=%d", roi -> roi_type);
    }
}

...
