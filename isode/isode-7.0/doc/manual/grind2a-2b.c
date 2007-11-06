...

static int  ros_init (vecp, vec)
int	vecp;
char  **vec;
{
    int	    sd;
    struct AcSAPstart   acss;
    register struct AcSAPstart *acs = &acss;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort   *aca = &aci -> aci_abort;
    register struct PSAPstart *ps = &acs -> acs_start;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (AcInit (vecp, vec, acs, aci) == NOTOK) {
	warn ("initialization fails: %s", AcErrString (aca -> aca_reason));
	return NOTOK;
    }

    sd = acs -> acs_sd;
    ACSFREE (acs);
    
/* read command line arguments here... */

    if (AcAssocResponse (sd, ACS_ACCEPT, ACS_USER_NULL, NULLOID, NULLAEI,
		&ps -> ps_called, NULLPC, ps -> ps_defctxresult,
		ps -> ps_prequirements, ps -> ps_srequirements,
		SERIAL_NONE, ps -> ps_settings, &ps -> ps_connect,
		NULLPEP, 0, aci) == NOTOK) {
	warn ("A-ASSOCIATE.RESPONSE: %s", AcErrString (aca -> aca_reason));
	return NOTOK;
    }

    if (RoSetService (sd, RoPService, roi) == NOTOK)
	fatal ("RoSetService: %s", RoErrString (rop -> rop_reason));

    return sd;
}

...
