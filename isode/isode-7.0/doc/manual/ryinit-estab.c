...

#define	myservice	"servicestore"
#define	mycontext	"local service"
#define	mypci		"local service pci"

...

    int	    sd;
    struct SSAPref sfs;
    register struct SSAPref *sf;
    register struct PSAPaddr *pa;
    struct AcSAPconnect accs;
    register struct AcSAPconnect   *acc = &accs;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort *aca = &aci -> aci_abort;
    AEI	    aei;
    OID	    ctx,
	    pci;
    struct PSAPctxlist pcs;
    register struct PSAPctxlist *pc = &pcs;
    struct RoSAPindication rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject *rop = &roi -> roi_preject;

    if ((aei = _str2aei (argv[1], myservice, mycontext,
			 isatty (fileno (stdin)), NULLCP, NULLCP))
	    == NULLAEI)
	adios (NULLCP, "unable to resolve service: %s", PY_pepy);
    if ((pa = aei2addr (aei)) == NULLPA)
	adios (NULLCP, "address translation failed");

    if ((ctx = ode2oid (mycontext)) == NULLOID)
	adios (NULLCP, "%s: unknown object descriptor", mycontext);
    if ((ctx = oid_cpy (ctx)) == NULLOID)
	adios (NULLCP, "out of memory");
    if ((pci = ode2oid (mypci)) == NULLOID)
	adios (NULLCP, "%s: unknown object descriptor", mypci);
    if ((pci = oid_cpy (pci)) == NULLOID)
	adios (NULLCP, "out of memory");
    pc -> pc_nctx = 1;
    pc -> pc_ctx[0].pc_id = 1;
    pc -> pc_ctx[0].pc_asn = pci;
    pc -> pc_ctx[0].pc_atn = NULLOID;

    if ((sf = addr2ref (PLocalHostName ())) == NULL) {
	sf = &sfs;
	(void) bzero ((char *) sf, sizeof *sf);
    }

    if (AcAssocRequest (ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
		0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, NULLQOS,
		acc, aci)
	    == NOTOK)
	acs_adios (aca, "A-ASSOCIATE.REQUEST");

    if (acc -> acc_result != ACS_ACCEPT)
	adios (NULLCP, "association rejected: [%s]",
		AcErrString (acc -> acc_result));

    sd = acc -> acc_sd;
    ACCFREE (acc);

    if (RoSetService (sd, RoPService, roi) == NOTOK)
	ros_adios (rop, "set RO/PS fails");

...
