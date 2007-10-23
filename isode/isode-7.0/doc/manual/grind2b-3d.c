...

static int  ros_result (sd, ror)
int     sd;
register struct RoSAPresult *ror;
{
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (RoURejectRequest (sd, &ror -> ror_id, ROS_RRP_UNRECOG, ROS_NOPRIO, roi)
	    == NOTOK)
	error ("RO-U-REJECT.REQUEST: %s", RoErrString (rop -> rop_reason));

    RORFREE (ror);
}


static int  ros_error (sd, roe)
int     sd;
register struct RoSAPerror *roe;
{
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (RoURejectRequest (sd, &roe -> roe_id, ROS_REP_UNRECOG, ROS_NOPRIO, roi)
	    == NOTOK)
	error ("RO-U-REJECT.REQUEST: %s", RoErrString (rop -> rop_reason));

    ROEFREE (roe);
}


static int  ros_ureject (sd, rou)
int     sd;
register struct RoSAPureject *rou;
{
/* handle rejection here... */
}


static int  ros_preject (sd, rop)
int     sd;
register struct RoSAPpreject *rop;
{
    if (ROS_FATAL (rop -> rop_reason))
	error ("RO-REJECT-P.INDICATION: %s", RoErrString (rop -> rop_reason));

/* handle temporary failure here... */
}

...
