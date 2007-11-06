#include <stdio.h>
#include <isode/psap2.h>
#include <isode/isoservent.h>


static int requirements = SR_HALFDUPLEX | SR_NEGOTIATED;

static int owned = 0;


/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int     sd,
	    settings;
    char    buffer[BUFSIZ];
    register struct PSAPaddr   *pz;
    register struct SSAPref *sf;
    struct PSAPconnect  pcs;
    register struct PSAPconnect *pc = &pcs;
    struct PSAPrelease  prs;
    register struct PSAPrelease *pr = &prs;
    struct PSAPindication   pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort *pa = &pi -> pi_abort;
    register struct isoservent *is;

    if ((is = getisoserventbyname ("sink", "psap")) == NULL)
	error ("psap/sink: unknown provider/entity pair");
    if (argc != 2)
	error ("usage: source \"host\"");

...
