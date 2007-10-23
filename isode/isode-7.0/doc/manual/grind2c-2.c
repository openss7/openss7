#include <stdio.h>
#include <isode/rtsap.h>


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int	    result,
	    sd;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;
    struct RtSAPstart   rtss;
    register struct RtSAPstart *rts = &rtss;
    struct AcSAPstart *acs = &rts -> rts_start;
    strut PSAPstart *ps = &acs -> acs_start;
    struct RtSAPindication  rtis;
    register struct RtSAPindication *rti = &rtis;
    register struct RtSAPabort   *rta = &rti -> rti_abort;

    if (RtInit (argc, argv, rts, rti) == NOTOK)
	error ("initialization fails: %s", RtErrString (rta -> rta_reason));

    sd = rts -> rts_sd;
    RTSFREE (rts);
    
/* read command line arguments here... */

    if (RtOpenResponse (sd, ACS_ACCEPT, NULLOID, NULLAEI,
		&ps -> ps_called, NULLPC, ps -> ps_defctxresult,
		NULLPE, rti) == NOTOK)
	error ("RT-OPEN.RESPONSE: %s", RtErrString (rta -> rta_reason));

    if (RoSetService (sd, RoRtService, roi) == NOTOK)
	error ("RoSetService: %s", RoErrString (rop -> rop_reason));

    for (;;)
	switch (result = RoWaitRequest (sd, NOTOK, roi)) {
	    case NOTOK: 
	    case OK: 
	    case DONE: 
		ros_indication (sd, roi);
		break;

	    default: 
		error ("unknown return from RoWaitRequest=%d", result);
	}
}
