...

    if ((sf = addr2ref (PLocalHostName ())) == NULL) {
	sf = &sfs;
	(void) bzero ((char *) sf, sizeof *sf);
    }
    
/* read command line arguments here... */

    if (AcAssocRequest (ctx, NULLAEI, NULLAEI, NULLPA, pa, pc, NULLOID,
		0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, NULLQOS,
		acc, aci)
	    == NOTOK)
	error ("A-ASSOCIATE.REQUEST: %s", AcErrString (aca -> aca_reason));

    if (acc -> acc_result != ACS_ACCEPT)
	error ("association rejected: %s", AcErrString (aca -> aca_reason));

    sd = acc -> acc_sd;
    ACCFREE (acc);

    if (RoSetService (sd, RoPService, &rois) == NOTOK)
	error ("RoSetService: %s", RoErrString (rop -> rop_reason));

    invoke (sd);		/* invoke the operations, etc. */
    
    if (AcRelRequest (sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
	error ("A-RELEASE.REQUEST: %s", AcErrString (aca -> aca_reason));

    if (!acr -> acr_affirmative) {
	(void) AcUAbortRequest (sd, NULLPEP, 0, aci);
	error ("release rejected by peer: %d", acr -> acr_reason);
    }

    ACRFREE (acr);

    exit (0);
}
