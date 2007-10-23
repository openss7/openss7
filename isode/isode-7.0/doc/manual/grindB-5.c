#include <stdio.h>
#include <isode/rtsap.h>
#include <isode/isoservent.h>


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
    struct RtSAPindication  rtis;
    register struct RtSAPindication *rti = &rtis;
    register struct RtSAPabort   *rta = &rti -> rti_abort;

    if (RtInit (argc, argv, rts, rti) == NOTOK)
	error ("initialization fails: %s", RtErrString (rta -> rta_reason));

    sd = rts -> rts_sd;
/*  do authentication using rts -> rts_data here... */
    RTSFREE (rts);
    
/* read command line arguments here... */

/* could use RTS_BUSY, RTS_VALIDATE, or RTS_MODE instead and then exit */

    if (RtBeginResponse (sd, RTS_ACCEPT, NULLPE, rti) == NOTOK)
	error ("RT-BEGIN.RESPONSE: %s", RtErrString (rta -> rta_reason));

    if (RoSetService (sd, RoPService, roi) == NOTOK)
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
