/* isore.c - help out ISODE TSAP programs */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/support/RCS/isore.c,v 9.0 1992/06/16 12:40:09 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/support/RCS/isore.c,v 9.0 1992/06/16 12:40:09 isode Rel $
 *
 *
 * $Log: isore.c,v $
 * Revision 9.0  1992/06/16  12:40:09  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include <signal.h>
#include "general.h"
#include "manifest.h"

/*    MAIN */

static SFD	EMTser ();


/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int	    fd,
	    mask,
    	    nfds,
    	    ppid;
    fd_set  ifds,
    	    rfds;

    if (argc != 4)
	exit (1);
    if ((nfds = atoi (argv[1])) < 0
	    || sscanf (argv[2], "0x%x", &mask) != 1
	    || (ppid = atoi (argv[3])) < 0)
	exit (2);

    FD_ZERO (&rfds);
    for (fd = 0; fd < nfds; fd++)
	if (mask & (1 << fd))
	    FD_SET (fd, &rfds);

    (void) signal (SIGEMT, EMTser);

    for (;;) {
	ifds = rfds;
	switch (xselect (nfds, &ifds, NULLFD, NULLFD, NOTOK)) {
	    case NOTOK: 
		(void) fprintf (stderr, "NOTOK\n");
		break;

	    case OK: 
		(void) fprintf (stderr, "OK\n");
		break;

	    default: 
		(void) kill (ppid, SIGEMT);
		(void) sigpause (0);
		break;
	}
    }
}

/*    SIGNALS */

/* ARGSUSED */
#ifdef SVR4
static  SFD EMTser (sig)
int	sig;
#else
static  SFD EMTser (sig, code, sc)
int	sig;
long	code;
struct sigcontext *sc;
#endif
{
#ifndef	BSDSIGS
    (void) signal (SIGEMT, EMTser);
#endif
}
