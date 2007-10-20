...

static int  ros_end (sd, roe)
int     sd;
struct RoSAPend *roe;
{
    struct RtSAPindication  rtis;
    register struct RtSAPindication *rti = &rtis;
    register struct RtSAPabort   *rta = &rti -> rti_abort;

    if (RtEndResponse (sd, rti) == NOTOK)
	error ("RT-END.RESPONSE: %s", RtErrString (rta -> rta_reason));

    exit (0);
}
