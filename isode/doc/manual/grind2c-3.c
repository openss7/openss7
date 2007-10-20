...

static int  ros_finish (sd, rof)
int     sd;
struct AcSAPfinish *acf;
{
    struct RtSAPindication  rtis;
    register struct RtSAPabort   *rta = &rtis.rti_abort;

    if (RtCloseResponse (sd, ACR_NORMAL, NULLPE, &rtis) == NOTOK)
	error ("RT-CLOSE.RESPONSE: %s", RtErrString (rta -> rta_reason));

    ACFFREE (acf);

    exit (0);
}
