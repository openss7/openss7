/* ryresponder.c - generic idempotent responder */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/ryresponder.c,v 9.0 1992/06/16 14:38:53 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/idist/RCS/ryresponder.c,v 9.0 1992/06/16 14:38:53 isode Rel $
 *
 *
 * $Log: ryresponder.c,v $
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 * 
 */

#include "Idist-types.h"
#include "Idist-ops.h"
#include <stdio.h>
#include <setjmp.h>
#include <varargs.h>
#include "ryresponder.h"
#include "tsap.h"		/* for listening */

/*    DATA */

int	debug = 0;

static LLog _pgm_log = {
    "responder.log", NULLCP, NULLCP,
    LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE, LLOG_FATAL, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *pgm_log = &_pgm_log;

static char *myname = "ryresponder";

static jmp_buf toplevel;


static IFP	startfnx;
static IFP	stopfnx;

int	ros_init (), ros_work (), ros_indication (), ros_lose ();

SFD cleanup ();

extern int  errno;

/*    RESPONDER */

int	ryresponder (argc, argv, host, myservice, mycontext, dispatches, ops, start, stop)
int	argc;
char  **argv,
       *host,
       *myservice,
       *mycontext;
struct dispatch *dispatches;
struct RyOperation *ops;
IFP	start,
	stop;
{
    register struct dispatch   *ds;
    AEI	    aei;
    struct TSAPdisconnect   tds;
    struct TSAPdisconnect  *td = &tds;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (myname = rindex (argv[0], '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = argv[0];

    isodetailor (myname, 0);

    if (debug = isatty (fileno (stderr)))
        ll_dbinit (pgm_log, myname);
    else {
        static char  myfile[BUFSIZ];

        (void) sprintf (myfile, "%s.log",
                        (strncmp (myname, "ros.", 4)                    
                                    && strncmp (myname, "lpp.", 4))
                                || myname[4] == NULL
                            ? myname : myname + 4);
        pgm_log -> ll_file = myfile;
        ll_hdinit (pgm_log, myname);
    }

    advise (LLOG_NOTICE, NULLCP, "starting");

    if ((aei = _str2aei (host, myservice, mycontext, 0, NULLCP, NULLCP))
	    == NULLAEI)
	adios (NULLCP, "unable to resolve service: %s", PY_pepy);

    for (ds = dispatches; ds -> ds_name; ds++)
	if (RyDispatch (NOTOK, ops, ds -> ds_operation, ds -> ds_vector, roi)
		== NOTOK)
	    ros_adios (rop, ds -> ds_name);

    startfnx = start;
    stopfnx = stop;

    if (isodeserver (argc, argv, aei, ros_init, ros_work, ros_lose, td)
	    == NOTOK) {
	if (td -> td_cc > 0)
	    adios (NULLCP, "isodeserver: [%s] %*.*s",
		    TErrString (td -> td_reason),
		    td -> td_cc, td -> td_cc, td -> td_data);
	else
	    adios (NULLCP, "isodeserver: [%s]",
		    TErrString (td -> td_reason));
    }

    return 0;
}

/*  */

static int  ros_init (vecp, vec)
int	vecp;
char  **vec;
{
    int	    reply,
	    result,
	    sd;
    struct AcSAPstart   acss;
    register struct AcSAPstart *acs = &acss;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort   *aca = &aci -> aci_abort;
    register struct PSAPstart *ps = &acs -> acs_start;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;
    PE pe[1];

    if (AcInit (vecp, vec, acs, aci) == NOTOK) {
	acs_advise (aca, "initialization fails");
	return NOTOK;
    }
    advise (LLOG_NOTICE, NULLCP,
		"A-ASSOCIATE.INDICATION: <%d, %s, %s, %s, %d>",
		acs -> acs_sd, oid2ode (acs -> acs_context),
		sprintaei (&acs -> acs_callingtitle),
		sprintaei (&acs -> acs_calledtitle), acs -> acs_ninfo);

    sd = acs -> acs_sd;

    for (vec++; *vec; vec++)
	advise (LLOG_EXCEPTIONS, NULLCP, "unknown argument \"%s\"", *vec);

    reply = startfnx ? (*startfnx) (sd, acs, &pe[0]) : ACS_ACCEPT;

    result = AcAssocResponse (sd, reply, 
		reply != ACS_ACCEPT ? ACS_USER_NOREASON : ACS_USER_NULL,
		NULLOID, NULLAEI, NULLPA, NULLPC, ps -> ps_defctxresult,
		ps -> ps_prequirements, ps -> ps_srequirements, SERIAL_NONE,
		ps -> ps_settings, &ps -> ps_connect, pe,
		reply != ACS_ACCEPT ? 1 : 0, aci);

    ACSFREE (acs);

    if (result == NOTOK) {
	acs_advise (aca, "A-ASSOCIATE.RESPONSE");
	return NOTOK;
    }
    if (reply != ACS_ACCEPT)
	return NOTOK;

    if (RoSetService (sd, RoPService, roi) == NOTOK)
	ros_adios (rop, "set RO/PS fails");

    return sd;
}

/*  */

static int  ros_work (fd)
int	fd;
{
    int	    result;
    caddr_t out;
    struct AcSAPindication  acis;
    struct RoSAPindication  rois;
    register struct RoSAPindication *roi = &rois;
    register struct RoSAPpreject   *rop = &roi -> roi_preject;

    switch (setjmp (toplevel)) {
	case OK: 
	    break;

	default: 
	    if (stopfnx)
		(*stopfnx) (fd, (struct AcSAPfinish *) 0);
	case DONE:
	    (void) AcUAbortRequest (fd, NULLPEP, 0, &acis);
	    (void) RyLose (fd, roi);
	    return NOTOK;
    }

    switch (result = RyWait (fd, NULLIP, &out, OK, roi)) {
	case NOTOK: 
	    if (rop -> rop_reason == ROS_TIMER)
		break;
	case OK: 
	case DONE: 
	    ros_indication (fd, roi);
	    break;

	default: 
	    adios (NULLCP, "unknown return from RoWaitRequest=%d", result);
    }

    return OK;
}

/*  */

static int ros_indication (sd, roi)
int	sd;
register struct RoSAPindication *roi;
{
    int	    reply,
	    result;

    switch (roi -> roi_type) {
	case ROI_INVOKE: 
	case ROI_RESULT: 
	case ROI_ERROR: 
	    adios (NULLCP, "unexpected indication type=%d", roi -> roi_type);
	    break;

	case ROI_UREJECT: 
	    {
		register struct RoSAPureject   *rou = &roi -> roi_ureject;

		if (rou -> rou_noid)
		    advise (LLOG_EXCEPTIONS, NULLCP, "RO-REJECT-U.INDICATION/%d: %s",
			    sd, RoErrString (rou -> rou_reason));
		else
		    advise (LLOG_EXCEPTIONS, NULLCP,
			    "RO-REJECT-U.INDICATION/%d: %s (id=%d)",
			    sd, RoErrString (rou -> rou_reason),
			    rou -> rou_id);
	    }
	    break;

	case ROI_PREJECT: 
	    {
		register struct RoSAPpreject   *rop = &roi -> roi_preject;

		if (ROS_FATAL (rop -> rop_reason))
		    ros_adios (rop, "RO-REJECT-P.INDICATION");
		ros_advise (rop, "RO-REJECT-P.INDICATION");
	    }
	    break;

	case ROI_FINISH: 
	    {
		register struct AcSAPfinish *acf = &roi -> roi_finish;
		struct AcSAPindication  acis;
		register struct AcSAPabort *aca = &acis.aci_abort;

		advise (LLOG_NOTICE, NULLCP, "A-RELEASE.INDICATION/%d: %d",
			sd, acf -> acf_reason);

		reply = stopfnx ? (*stopfnx) (sd, acf) : ACS_ACCEPT;

		result = AcRelResponse (sd, reply, ACR_NORMAL, NULLPEP, 0,
			    &acis);

		ACFFREE (acf);

		if (result == NOTOK)
		    acs_advise (aca, "A-RELEASE.RESPONSE");
		else
		    if (reply != ACS_ACCEPT)
			break;
		longjmp (toplevel, DONE);
	    }
	/* NOTREACHED */

	default: 
	    adios (NULLCP, "unknown indication type=%d", roi -> roi_type);
    }
}

/*  */

static int  ros_lose (td)
struct TSAPdisconnect *td;
{
    if (td -> td_cc > 0)
	adios (NULLCP, "TNetAccept: [%s] %*.*s",
		TErrString (td -> td_reason), td -> td_cc, td -> td_cc,
		td -> td_data);
    else
	adios (NULLCP, "TNetAccept: [%s]", TErrString (td -> td_reason));
}

/*    ERRORS */

void	ros_adios (rop, event)
register struct RoSAPpreject *rop;
char   *event;
{
    ros_advise (rop, event);

    cleanup ();

    longjmp (toplevel, NOTOK);
}


void	ros_advise (rop, event)
register struct RoSAPpreject *rop;
char   *event;
{
    char    buffer[BUFSIZ];

    if (rop -> rop_cc > 0)
	(void) sprintf (buffer, "[%s] %*.*s", RoErrString (rop -> rop_reason),
		rop -> rop_cc, rop -> rop_cc, rop -> rop_data);
    else
	(void) sprintf (buffer, "[%s]", RoErrString (rop -> rop_reason));

    advise (LLOG_EXCEPTIONS, NULLCP, "%s: %s", event, buffer);
}

/*  */

void	acs_advise (aca, event)
register struct AcSAPabort *aca;
char   *event;
{
    char    buffer[BUFSIZ];

    if (aca -> aca_cc > 0)
	(void) sprintf (buffer, "[%s] %*.*s",
		AcErrString (aca -> aca_reason),
		aca -> aca_cc, aca -> aca_cc, aca -> aca_data);
    else
	(void) sprintf (buffer, "[%s]", AcErrString (aca -> aca_reason));

    advise (LLOG_EXCEPTIONS, NULLCP, "%s: %s (source %d)", event, buffer,
		aca -> aca_source);
}

/*  */

#ifndef	lint

void	adios (va_alist)
va_dcl
{
    char   *what;
    va_list ap;

    va_start (ap);

    _ll_log (pgm_log, LLOG_FATAL, ap);

    va_end (ap);

    cleanup ();

    _exit (1);
}
#else
/* VARARGS2 */

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



#ifndef	lint
void	ryr_advise (va_alist)
va_dcl
{
    va_list ap;

    va_start (ap);

    _ll_log (pgm_log, LLOG_NOTICE, ap);

    va_end (ap);
}
#else
/* VARARGS2 */
void	ryr_advise (what, fmt)
char   *what,
       *fmt;
{
    ryr_advise (what, fmt);
}
#endif

#ifdef lint
/* VARARGS1 */
note (fmt)
char	*fmt;
{
	note (fmt);
}

#else
note (va_alist)
va_dcl
{
	char	buffer[BUFSIZ];
	struct type_Idist_IA5List **ia5p;
	va_list	ap;

	va_start (ap);

	_asprintf (buffer, NULLCP, ap);

	addtoia5 (buffer, strlen (buffer));

	va_end (ap);
}
#endif

#ifdef lint
/* VARARGS2 */
nadvise (what, fmt)
char	*what, *fmt;
{
	nadvise (what, fmt);
}
#else
nadvise (va_alist)
va_dcl
{
	va_list ap;
	char	buf[BUFSIZ], *cp;
	extern	char *host;

	va_start (ap);

	(void) sprintf (buf, "%s (%s): ", myname, host);
	cp = buf + strlen (buf);

	asprintf (cp, ap);

	addtoia5 (buf, strlen (buf));

	va_end (ap);
}
#endif
