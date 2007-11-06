...

    if ((pz = is2paddr (argv[1], NULLCP, is)) == NULLPA)
	error ("address translation failed");
    sf = addr2ref (PLocalHostName ());

    settings = 0;
#define	dotoken(requires,shift,bit,type) \
{ \
    if (requirements & requires) \
	settings |= ST_INIT_VALUE << shift; \
}
    dotokens ();
#undef	dotoken

    if (PConnRequest (NULLSA, pz, NULLPC, NULLOID, 0, requirements,
	    SERIAL_NONE, settings, sf, NULLPEP, 0, NULLQOS, pc, pi) == NOTOK)
	error ("P-CONNECT.REQUEST: %s", PErrString (pa -> pa_reason));
    if (pc -> pc_result != PC_ACCEPT)
	error ("connection rejected by sink: %s",
		    PErrString (pc -> pc_result));	

    sd = pc -> pc_sd;
    requirements = pc -> pc_requirements;

#define dotoken(requires,shift,bit,type) \
{ \
    if (requirements & requires) \
	if ((pc -> pc_settings & (ST_MASK << shift)) \
		== ST_INIT_VALUE) \
	    owned |= bit; \
}
	dotokens ();
#undef	dotoken

    PCFREE (pc);

...
