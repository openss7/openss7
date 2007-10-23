...

static int  ros_end (sd, roe)
int     sd;
struct RoSAPend *roe;
{
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (RoEndResponse (sd, roi) == NOTOK)
	error ("RO-END.RESPONSE: %s", RoErrString (rop -> rop_reason));

    exit (0);
}
