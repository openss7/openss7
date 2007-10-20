...

static int  ros_finish (sd, acf)
int     sd;
struct AcSAPfinish *acf;
{
    struct AcSAPindication  acis;
    register struct AcSAPabort *aca = &acis.aci_abort;

    ACFFREE (acf);

    if (AcRelResponse (sd, ACS_ACCEPT, ACR_NORMAL, NULLPEP, 0, &acis) == NOTOK)
	error ("A-RELEASE.RESPONSE: %s", AcErrString (aca -> aca_reason));

    error ("association released");
}
