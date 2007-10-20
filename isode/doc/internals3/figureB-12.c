...

static int  op_name (sd, ryo, rox, arg, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
register struct type_MODULE_argument *arg;
struct RoSAPindication *roi;
{
    int	    won;

/* this check should *not* be made if linking of operations is expected */
    if (rox -> rox_nolinked == 0) {
	if (RyDsUReject (sd, rox -> rox_id, ROS_IP_LINKED, ROS_NOPRIO, roi)
	        == NOTOK)
	    ros_adios (&roi -> roi_preject, "U-REJECT");

	return OK;
    }
    advise (NULLCP, LOG_INFO, "RO-INVOKE.INDICATION/%d: %s",
	    sd, ryo -> ryo_name);
