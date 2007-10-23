...

static int  op_name (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
    int	    result,
	    won;
    register struct type_MODULE_argument *arg =
		(struct type_MODULE_argument *) in;

/* this check should *not* be made if linking of operations is expected */
    if (rox -> rox_nolinked == 0) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
		sd, ryo -> ryo_name, rox -> rox_linkid);
	return ureject (sd, ROS_IP_LINKED, rox, roi);
    }
    if (debug)
	advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
		sd, ryo -> ryo_name);

/* do something with ``arg'' here and decide if won... */

    if (won) {
	register struct type_MODULE_result *res;

/* allocate and initialize the result ``res'' here... */

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) res, ROS_NOPRIO, roi)
		== NOTOK)
	    ros_adios (&roi -> roi_preject, "RESULT");
	result = OK;

	free_MODULE_result (res);
    }
    else {
	struct type_MODULE_parameter *parm;

/* allocate and initialize ``parm'' here... */

	result = error (sd, error_MODULE_error1, (caddr_t) parm, rox, roi);

	free_MODULE_parameter (parm);
    }

/* ``arg'' will be automatically free'd when op_name returns */

    return result;
}

...

static int  error (sd, err, param, rox, roi)
int	sd,
	err;
caddr_t	param;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "ERROR");

    return OK;
}


static int  ureject (sd, reason, rox, roi)
int	sd,
	reason;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsUReject (sd, rox -> rox_id, reason, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "U-REJECT");

    return OK;
}

...
