...

    if ((sz = is2saddr (argv[1], NULLCP, is)) == NULLSA)
	error ("address translation failed");
    (void) bzero ((char *) sf, sizeof *sf);

    settings = 0;
#define	dotoken(requires,shift,bit,type) \
{ \
    if (requirements & requires) \
	settings |= ST_INIT_VALUE << shift; \
}
    dotokens ();
#undef	dotoken

    if (SConnRequest (sf, NULLSA, sz, requirements, settings, SERIAL_NONE,
	    NULLCP, 0, NULLQOS, sc, si) == NOTOK)
	error ("S-CONNECT.REQUEST: %s", SErrString (sa -> sa_reason));
    if (sc -> sc_result != SC_ACCEPT)
	error ("connection rejected by sink[%s]: %s",
		    SErrString (sa -> sa_reason),
		    SErrString (sc -> sc_result));	

    sd = sc -> sc_sd;
    requirements = sc -> sc_requirements;

#define dotoken(requires,shift,bit,type) \
{ \
    if (requirements & requires) \
	if ((sc -> sc_settings & (ST_MASK << shift)) == ST_INIT_VALUE) \
	    owned |= bit; \
}
	dotokens ();
#undef	dotoken

...
