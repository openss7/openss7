#include <isode/acsap.h>

...

register struct PSAPaddr *pa;
register AEI aei;

...

char *qualifier = "filestore";
char *context = "iso ftam";

if ((aei = _str2aei ("hubris, cs, ucl, gb", qualifier,
		     context, isatty (fileno (stdin)),
		     NULLCP, NULLCP))
	== NULLAEI)
    error ("unable to resolve service: %s", PY_pepy);
if ((pa = aei2addr (aei)) == NULLPA)
    error ("address translation failed");

...
