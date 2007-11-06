...

static int  ros_work (fd)
int	fd;
{
    int	    result;
    struct AcSAPindication  acis;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    switch (setjmp (toplevel)) {
	case OK: 
	    break;

	default: 
	    (void) AcUAbortRequest (fd, NULLPEP, 0, &acis);
	    return NOTOK;
    }

    switch (result = RoWaitRequest (fd, OK, roi)) {
	case NOTOK: 
	    if (rop -> rop_reason == ROS_TIMER)
		break;
	case OK: 
	case DONE: 
	    ros_indication (fd, roi);
	    break;

	default: 
	    fatal (NULLCP, "unknown return from RoWaitRequest=%d", result);
    }

    return OK;
}

...
