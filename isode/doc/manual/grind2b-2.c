#include <stdio.h>
#include "generic.h"		/* defines OPERATIONS and ERRORS */
#include <isode/rosap.h>


int	ros_indication ();


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int	    result,
	    sd;
    struct AcSAPstart   acss;
    register struct AcSAPstart *acs = &acss;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort   *aca = &aci -> aci_abort;
    register struct PSAPstart *ps = &acs -> acs_start;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (AcInit (argc, argv, acs, aci) == NOTOK)
	error ("initialization fails: %s", AcErrString (aca -> aca_reason));

    sd = acs -> acs_sd;
    ACSFREE (acs);
    
/* read command line arguments here... */

    if (AcAssocResponse (sd, ACS_ACCEPT, ACS_USER_NULL, NULLOID, NULLAEI,
		&ps -> ps_called, NULLPC, ps -> ps_defctxresult,
		ps -> ps_prequirements, ps -> ps_srequirements,
		SERIAL_NONE, ps -> ps_settings, &ps -> ps_connect,
		NULLPEP, 0, aci) == NOTOK)
	error ("A-ASSOCIATE.RESPONSE: %s", AcErrString (aca -> aca_reason));

    if (RoSetService (sd, RoPService, roi) == NOTOK)
	error ("RoSetService: %s", RoErrString (rop -> rop_reason));

    if (RoSetIndications (sd, ros_indication, roi) == NOTOK)
	error ("RoSetIndications: %s", RoErrString (rop -> rop_reason));

    for (;;)
	pause ();
}
