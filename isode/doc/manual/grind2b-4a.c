#include <stdio.h>
#include "generic.h"		/* defines OPERATIONS and ERRORS */
#include <isode/rosap.h>


static char *myservice = "service";

static char *mycontext = "iso service";
static char *mypci = "service pci version m.n";


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int	    sd;
    struct SSAPref sfs;
    register struct SSAPref *sf;
    register struct PSAPaddr *pa;
    struct AcSAPconnect accs;
    register struct AcSAPconnect   *acc = &accs;
    struct AcSAPrelease acrs;
    register struct AcSAPrelease   *acr = &acrs;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort *aca = &aci -> aci_abort;
    struct RoSAPindication  rois;
    register struct RoSAPpreject   *rop = &rois.roi_preject;
    register AEI aei;
    register OID ctx, pci;
    struct PSAPctxlist pcs;
    register struct PSAPctxlist *pc = &pcs;

    if ((aei = _str2aei (argv[1], myservice, mycontext, 0,
			 NULLCP, NULLCP)) == NULLAEI)
	error ("unable to resolve service: %s", PY_pepy);
    if ((pa = aei2addr (aei)) == NULLPA)
	error ("address translation failed");
    if ((ctx = ode2oid (mycontext)) == NULLOID)
	error ("%s: unknown object descriptor", mycontext);
    if ((ctx = oid_cpy (ctx)) == NULLOID)
	error ("oid_cpy");
    if ((pci = ode2oid (mypci)) == NULLOID)
	error ("%s: unknown object descriptor", mypci);
    if ((pci = oid_cpy (pci)) == NULLOID)
	error ("oid_cpy");
    pc -> pc_nctx = 1;
    pc -> pc_ctx[0].pc_id = 1;
    pc -> pc_ctx[0].pc_asn = pci;
    pc -> pc_ctx[0].pc_atn = NULLOID;

...
