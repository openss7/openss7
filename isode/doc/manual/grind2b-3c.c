...

static int  ros_invoke (sd, rox)
int     sd;
register struct RoSAPinvoke *rox;
{
    int     result;
    register struct dispatch   *ds;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    for (ds = dispatches; ds -> ds_operation != APDU_UNKNOWN; ds++)
	if (ds -> ds_operation == rox -> rox_op)
	    break;

    if (ds -> ds_operation == APDU_UNKNOWN) {
	if (RoURejectRequest (sd, &rox -> rox_id, ROS_IP_UNRECOG,
		    ROS_NOPRIO, roi) == NOTOK)
	    error ("RO-U-REJECT.REQUEST: %s", RoErrString (rop -> rop_reason));
	goto out;
    }

    if (rox -> rox_nolinked == 0) {
	if (RoURejectRequest (sd, &rox -> rox_id, ROS_IP_LINKED,
		    ROS_NOPRIO, roi) == NOTOK)
	    error ("RO-U-REJECT.REQUEST: %s", RoErrString (rop -> rop_reason));
	goto out;
    }

    switch (result = (*ds -> ds_vector) (rox)) {
	case ERROR_UNKNOWN: 
	    if (RoResultRequest (sd, rox -> rox_id, rox -> rox_op,
			rox -> rox_args, ROS_NOPRIO, roi) == NOTOK)
		error ("RO-RESULT.REQUEST: %s",
			RoErrString (rop -> rop_reason));
	    break;

	default: 
	    if (RoErrorRequest (sd, rox -> rox_id, result, rox -> rox_args,
			ROS_NOPRIO, roi) == NOTOK)
		error ("RO-ERROR.REQUEST: %s",
			RoErrString (rop -> rop_reason));
	    break;

	case ERROR_MISTYPED: 
	    if (RoURejectRequest (sd, &rox -> rox_id, ROS_IP_MISTYPED,
			ROS_NOPRIO, roi) == NOTOK)
		error ("RO-U-REJECT.REQUEST: %s",
			RoErrString (rop -> rop_reason));
	    break;
    }

out: ;
    ROXFREE (rox);
}

...
