/* unixd.c - daemon for UNIX MIB */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/unixd.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/unixd.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: unixd.c,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
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


#include <signal.h>
#include <stdio.h>
#include <varargs.h>
#include "smux.h"
#include "objects.h"
#include "sys.file.h"
#include "tailor.h"

/*    DATA */

int	debug = 0;
static	int	nbits = FD_SETSIZE;

static LLog	_pgm_log = {
    "unixd.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
    LLOG_FATAL, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
static	LLog   *pgm_log = &_pgm_log;

static	char   *myname = "unixd";


static	int	smux_fd = NOTOK;
static	int	rock_and_roll = 0;
static	int	got_at_least_one = 0;
static	int	dont_bother_anymore = 0;


int	init_users (), sync_users ();	/* users group */
int	init_print (), sync_print ();	/* print group */

static struct triple {
    char   *t_tree;
    OID	    t_name;
    int	    t_access;
    IFP	    t_init;
    IFP	    t_sync;
}	triples[] = {
    "users", NULL, readWrite, init_users, sync_users,
    "print", NULL, readWrite, init_print, sync_print,

    NULL
};

static struct triple *tc;


static	struct smuxEntry *se = NULL;


static	fd_set	ifds;
static	fd_set	ofds;


void	adios (), advise ();

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int	    nfds;

    arginit (argv);
    envinit ();

    FD_ZERO (&ifds);
    FD_ZERO (&ofds);
    nfds = 0;

/* set fd's for other purposes here... */

    for (;;) {
	int	n,
		secs;
	fd_set	rfds,
		wfds;

	secs = NOTOK;

	rfds = ifds;	/* struct copy */
	wfds = ofds;	/*   .. */

	if (smux_fd == NOTOK && !dont_bother_anymore)
	    secs = 5 * 60L;
	else
	    if (rock_and_roll)
		FD_SET (smux_fd, &rfds);
	    else
		FD_SET (smux_fd, &wfds);
	if (smux_fd >= nfds)
	    nfds = smux_fd + 1;

	if ((n = xselect (nfds, &rfds, &wfds, NULLFD, secs)) == NOTOK)
	    adios ("failed", "xselect");

/* check fd's for other purposes here... */

	if (smux_fd == NOTOK && !dont_bother_anymore) {
	    if (n == 0) {
		if ((smux_fd = smux_init (debug)) == NOTOK)
		    advise (LLOG_EXCEPTIONS, NULLCP, "smux_init: %s [%s]",
			    smux_error (smux_errno), smux_info);
		else
		    rock_and_roll = 0;
	    }
	}
	else
	    if (rock_and_roll) {
		if (FD_ISSET (smux_fd, &rfds))
		    doit_smux ();
	    }
	    else
		if (FD_ISSET (smux_fd, &wfds))
		    start_smux ();
    }
}

/*    MISCELLANY */

static	arginit (vec)
char	**vec;
{
    register char  *ap;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;
    if (strncmp (myname, "smux.", 5) == 0 && myname[5] != NULL)
	myname += 5;

    isodetailor (myname, 0);
    ll_hdinit (pgm_log, myname);

    for (vec++; ap = *vec; vec++) {
	if (*ap == '-')
	    switch (*++ap) {
		case 'd':
		    debug++;
		    continue;

		default: 
		    adios (NULLCP, "-%s: unknown switch", ap);
	    }

	adios (NULLCP, "usage: %s [switches]", myname);
    }
}

/*  */

static  envinit () {
    int     i,
            sd;
    char    file[BUFSIZ];
    FILE   *fp;

    nbits = getdtablesize ();

    if (debug == 0 && !(debug = isatty (2))) {
	for (i = 0; i < 5; i++) {
	    switch (fork ()) {
		case NOTOK: 
		    sleep (5);
		    continue;

		case OK: 
		    break;

		default: 
		    _exit (0);
	    }
	    break;
	}

	(void) chdir ("/");

	if ((sd = open ("/dev/null", O_RDWR)) == NOTOK)
	    adios ("/dev/null", "unable to read");
	if (sd != 0)
	    (void) dup2 (sd, 0), (void) close (sd);
	(void) dup2 (0, 1);
	(void) dup2 (0, 2);

#ifdef	SETSID
	if (setsid () == NOTOK)
	    advise (LLOG_EXCEPTIONS, "failed", "setsid");
#endif
#ifdef	TIOCNOTTY
	if ((sd = open ("/dev/tty", O_RDWR)) != NOTOK) {
	    (void) ioctl (sd, TIOCNOTTY, NULLCP);
	    (void) close (sd);
	}
#else
#ifdef	SYS5
	(void) setpgrp ();
	(void) signal (SIGINT, SIG_IGN);
	(void) signal (SIGQUIT, SIG_IGN);
#endif
#endif
    }
    else
	ll_dbinit (pgm_log, myname);

#ifndef	sun		/* damn YP... */
    for (sd = 3; sd < nbits; sd++)
	if (pgm_log -> ll_fd != sd)
	    (void) close (sd);
#endif

    (void) signal (SIGPIPE, SIG_IGN);

    ll_hdinit (pgm_log, myname);

    mibinit ();

    (void) sprintf (file, "/etc/%s.pid", myname);
    if (fp = fopen (file, "w")) {
	(void) fprintf (fp, "%d\n", getpid ());
	(void) fclose (fp);
    }
    
    advise (LLOG_NOTICE, NULLCP, "starting");
}

/*    MIB */

int	quantum = 0;


static  mibinit () {
    OT	    ot;

    if ((se = getsmuxEntrybyname ("unixd")) == NULL)
	adios (NULLCP, "no SMUX entry for \"%s\"", "unixd");

    if (readobjects ("unixd.defs") == NOTOK)
	adios (NULLCP, "readobjects: %s", PY_pepy);

    for (tc = triples; tc -> t_tree; tc++)
	if (ot = text2obj (tc -> t_tree)) {
	    tc -> t_name = ot -> ot_name;
	    (void) (*tc -> t_init) ();
	}
	else
	    advise (LLOG_EXCEPTIONS, NULLCP, "text2obj (\"%s\") fails",
		    tc -> t_tree);

    if ((smux_fd = smux_init (debug)) == NOTOK)
	advise (LLOG_EXCEPTIONS, NULLCP, "smux_init: %s [%s]",
		smux_error (smux_errno), smux_info);
    else
	rock_and_roll = 0;
}

/*  */

static	start_smux () {
    if (smux_simple_open (&se -> se_identity, "SMUX UNIX daemon",
			  se -> se_password, strlen (se -> se_password))
	    == NOTOK) {
	if (smux_errno == inProgress)
	    return;

	advise (LLOG_EXCEPTIONS, NULLCP, "smux_simple_open: %s [%s]",
		smux_error (smux_errno), smux_info);
losing: ;
	smux_fd = NOTOK;
	return;
    }
    advise (LLOG_NOTICE, NULLCP, "SMUX open: %s \"%s\"",
	    oid2ode (&se -> se_identity), se -> se_name);
    rock_and_roll = 1;

    for (tc = triples; tc -> t_tree; tc++)
	if (tc -> t_name) {
	    if (smux_register (tc -> t_name, -1, tc -> t_access) == NOTOK) {
		advise (LLOG_EXCEPTIONS, NULLCP, "smux_register: %s [%s]",
			smux_error (smux_errno), smux_info);
		goto losing;
	    }
	    advise (LLOG_NOTICE, NULLCP, "SMUX register: %s in=%d",
		    tc -> t_tree, -1);
	    break;
	}
}

/*  */

static	doit_smux () {
    struct type_SNMP_SMUX__PDUs *event;

    if (smux_wait (&event, NOTOK) == NOTOK) {
	if (smux_errno == inProgress)
	    return;

	advise (LLOG_EXCEPTIONS, NULLCP, "smux_wait: %s [%s]",
		smux_error (smux_errno), smux_info);
losing: ;
	smux_fd = NOTOK;
	return;
    }

    switch (event -> offset) {
	case type_SNMP_SMUX__PDUs_registerResponse:
	    if (!tc -> t_name)
		goto unexpected;
	    {
		struct type_SNMP_RRspPDU *rsp = event -> un.registerResponse;

		if (rsp -> parm == int_SNMP_RRspPDU_failure)
		    advise (LLOG_NOTICE, NULLCP,
			    "SMUX registration of %s failed",
			    tc -> t_tree);
		else {
		    advise (LLOG_NOTICE, NULLCP,
			    "SMUX register: %s out=%d",
			    tc -> t_tree, rsp -> parm);
		    got_at_least_one = 1;
		}
	    }
	    for (tc++; tc -> t_tree; tc++)
		if (tc -> t_name) {
		    if (smux_register (tc -> t_name, -1, tc -> t_access)
			    == NOTOK) {
			advise (LLOG_EXCEPTIONS, NULLCP,
				"smux_register: %s [%s]",
				smux_error (smux_errno), smux_info);
			goto losing;
		    }
		    advise (LLOG_NOTICE, NULLCP, "SMUX register: %s in=%d",
			    tc -> t_tree, -1);
		    break;
		}
	    if (!tc -> t_tree) {
		if (!got_at_least_one) {
		    dont_bother_anymore = 1;
		    (void) smux_close (goingDown);
		    goto losing;
		}
		if (smux_trap (int_SNMP_generic__trap_coldStart, 0,
			       (struct type_SNMP_VarBindList *) 0) == NOTOK) {
		    advise (LLOG_EXCEPTIONS, NULLCP, "smux_trap: %s [%s]",
			    smux_error (smux_errno), smux_info);
		    goto losing;
		}
	    }
	    break;

	case type_SNMP_SMUX__PDUs_get__request:
	case type_SNMP_SMUX__PDUs_get__next__request:
	case type_SNMP_SMUX__PDUs_set__request:
	    do_smux (event -> un.get__request, event -> offset);
	    break;

	case type_SNMP_SMUX__PDUs_commitOrRollback:
	    {
		register struct triple *tz;

		for (tz = triples; tz -> t_tree; tz++)
		    if (tz -> t_name)
			(void) (*tz -> t_sync) (event -> un.commitOrRollback
								    -> parm);
	    }
	    break;

	case type_SNMP_SMUX__PDUs_close:
	    advise (LLOG_NOTICE, NULLCP, "SMUX close: %s",
		    smux_error (event -> un.close -> parm));
	    goto losing;

	case type_SNMP_SMUX__PDUs_simple:
	case type_SNMP_SMUX__PDUs_registerRequest:
	case type_SNMP_SMUX__PDUs_get__response:
	case type_SNMP_SMUX__PDUs_trap:
unexpected: ;
	    advise (LLOG_EXCEPTIONS, NULLCP, "unexpectedOperation: %d",
		    event -> offset);
	    (void) smux_close (protocolError);
	    goto losing;

	default:
	    advise (LLOG_EXCEPTIONS, NULLCP, "badOperation: %d",
		    event -> offset);
	    (void) smux_close (protocolError);
	    goto losing;
    }
}

/*  */

static	do_smux (pdu, offset)
register struct type_SNMP_GetRequest__PDU *pdu;
int	offset;
{
    int	    idx,
	    status;
    object_instance ois;
    register struct type_SNMP_VarBindList *vp;
    IFP	    method;

    quantum = pdu -> request__id;
    idx = 0;
    for (vp = pdu -> variable__bindings; vp; vp = vp -> next) {
	register OI	oi;
	register OT	ot;
	register struct type_SNMP_VarBind *v = vp -> VarBind;

	idx++;

	if (offset == type_SNMP_SMUX__PDUs_get__next__request) {
	    if ((oi = name2inst (v -> name)) == NULLOI
		    && (oi = next2inst (v -> name)) == NULLOI)
		goto no_name;

	    if ((ot = oi -> oi_type) -> ot_getfnx == NULLIFP)
		goto get_next;
	}
	else {
	    if ((oi = name2inst (v -> name)) == NULLOI)
		goto no_name;
	    ot = oi -> oi_type;
	    if ((offset == type_SNMP_SMUX__PDUs_get__request
			    ? ot -> ot_getfnx : ot -> ot_setfnx) == NULLIFP) {
no_name: ;
		pdu -> error__status = int_SNMP_error__status_noSuchName;
		goto out;
	    }
	}

try_again: ;
	switch (offset) {
	    case type_SNMP_SMUX__PDUs_get__request:
		if (!(method = ot -> ot_getfnx))
		    goto no_name;
		break;
		
	    case type_SNMP_SMUX__PDUs_get__next__request:
		if (!(method = ot -> ot_getfnx))
		    goto get_next;
		break;
		
	    case type_SNMP_SMUX__PDUs_set__request:
		if (!(method = ot -> ot_setfnx))
		    goto no_name;
		break;

	    default:
		goto no_name;
	}
		
	switch (status = (method) (oi, v, offset)) {
	    case NOTOK:	    /* get-next wants a bump */
get_next: ;
		oi = &ois;
		for (;;) {
		    if ((ot = ot -> ot_next) == NULLOT) {
			pdu -> error__status =
					    int_SNMP_error__status_noSuchName;
			goto out;
		    }
		    oi -> oi_name = (oi -> oi_type = ot) -> ot_name;
		    if (ot -> ot_getfnx)
			goto try_again;
		}

	    case int_SNMP_error__status_noError:
		break;

	    default:
		pdu -> error__status = status;
		goto out;
	}
    }
    idx = 0;

out: ;
    pdu -> error__index = idx;

    if (smux_response (pdu) == NOTOK) {
	advise (LLOG_EXCEPTIONS, NULLCP, "smux_response: %s [%s]",
		smux_error (smux_errno), smux_info);
	smux_fd = NOTOK;
    }
}

/*    ERRORS */

#ifndef	lint
void	adios (va_alist)
va_dcl
{
    va_list ap;

    va_start (ap);
    
    _ll_log (pgm_log, LLOG_FATAL, ap);

    va_end (ap);

    _exit (1);
}
#else
/* VARARGS */

void	adios (what, fmt)
char   *what,
       *fmt;
{
    adios (what, fmt);
}
#endif


#ifndef	lint
void	advise (va_alist)
va_dcl
{
    int	    code;
    va_list ap;

    va_start (ap);
    
    code = va_arg (ap, int);

    _ll_log (pgm_log, code, ap);

    va_end (ap);
}
#else
/* VARARGS */

void	advise (code, what, fmt)
char   *what,
       *fmt;
int	code;
{
    advise (code, what, fmt);
}
#endif
