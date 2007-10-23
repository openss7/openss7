#include <stdio.h>
#include <isode/ssap.h>
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
    register struct SSAPaddr   *sz;
    struct SSAPref  sfs;
    register struct SSAPref *sf = &sfs;
    struct SSAPconnect  scs;
    register struct SSAPconnect *sc = &scs;
    struct SSAPrelease  srs;
    register struct SSAPrelease *sr = &srs;
    struct SSAPindication   sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;
    register struct isoservent *is;

    if ((is = getisoserventbyname ("sink", "ssap")) == NULL)
	error ("ssap/sink: unknown provider/entity pair");
    if (argc != 2)
	error ("usage: source \"host\"");

...
