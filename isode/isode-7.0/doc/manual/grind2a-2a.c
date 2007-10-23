#include <stdio.h>
#include "generic.h"		/* defines OPERATIONS and ERRORS */
#include <isode/rosap.h>
#include <isode/tsap.h>		/* for listening */
#include <setjmp.h>


static char *myservice = "service";
static char *mycontext = "context";

static jmp_buf toplevel;


int	ros_init (), ros_work (), ros_lose ();


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    AEI	    aei;
    struct TSAPdisconnect tds;
    register struct TSAPdisconnect *td = &tds;

    if ((aei = _str2aei (PLocalHostName (), myservice, mycontext, 0,
			 NULLCP, NULLCP)) == NULLAEI)
	fatal ("unable to resolve service: %s", PY_pepy);

    if (isodeserver (argc, argv, aei, ros_init, ros_work, ros_lose, td)
	    == NOTOK) {
	if (td -> td_cc > 0)
	    fatal ("isodeserver: [%s] %*.*s", TErrString (td -> td_reason),
		    td -> td_cc, td -> td_cc, td -> td_data);
	else
	    fatal ("isodeserver: [%s]", TErrString (td -> td_reason));
    }

    exit (0);
}

...
