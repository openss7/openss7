/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* ryresponder.c - generic idempotent responder */

#include <stdio.h>
#include <setjmp.h>
#include "ryresponder.h"
#include <isode/tsap.h>		/* for listening */

/* DATA */

int debug = 0;

static LLog _pgm_log = {
	"responder.log", NULLCP, NULLCP,
	LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE, LLOG_FATAL, -1,
	LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *pgm_log = &_pgm_log;

static char *myname = "ryresponder";

static jmp_buf toplevel;

static IFP startfnx;
static IFP stopfnx;

int ros_init(), ros_work(), ros_indication(), ros_lose();

extern int errno;

/* RESPONDER */

int
ryresponder(argc, argv, host, myservice, mycontext, dispatches, ops, start, stop)
	int argc;
	char **argv, *host, *myservice, *mycontext;
	struct dispatch *dispatches;
	struct RyOperation *ops;
	IFP start, stop;
{
	register struct dispatch *ds;
	AEI aei;
	struct TSAPdisconnect tds;
	struct TSAPdisconnect *td = &tds;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	if (myname = rindex(argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	isodetailor(myname, 0);
	if (debug = isatty(fileno(stderr)))
		ll_dbinit(pgm_log, myname);
	else {
		static char myfile[BUFSIZ];

		(void) sprintf(myfile, "%s.log", (strncmp(myname, "ros.", 4)
						  && strncmp(myname, "lpp.", 4))
			       || myname[4] == NULL ? myname : myname + 4);
		pgm_log->ll_file = myfile;
		ll_hdinit(pgm_log, myname);
	}

	advise(LLOG_NOTICE, NULLCP, "starting");

	if ((aei = _str2aei(host, myservice, mycontext, 0, NULLCP, NULLCP))
	    == NULLAEI)
		adios(NULLCP, "unable to resolve service: %s", PY_pepy);

	for (ds = dispatches; ds->ds_name; ds++)
		if (RyDispatch(NOTOK, ops, ds->ds_operation, ds->ds_vector, roi)
		    == NOTOK)
			ros_adios(rop, ds->ds_name);

	startfnx = start;
	stopfnx = stop;

	if (isodeserver(argc, argv, aei, ros_init, ros_work, ros_lose, td)
	    == NOTOK) {
		if (td->td_cc > 0)
			adios(NULLCP, "isodeserver: [%s] %*.*s",
			      TErrString(td->td_reason), td->td_cc, td->td_cc, td->td_data);
		else
			adios(NULLCP, "isodeserver: [%s]", TErrString(td->td_reason));
	}

	return 0;
}

static int
ros_init(vecp, vec)
	int vecp;
	char **vec;
{
	int reply, result, sd;
	struct AcSAPstart acss;
	register struct AcSAPstart *acs = &acss;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;
	register struct PSAPstart *ps = &acs->acs_start;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	if (AcInit(vecp, vec, acs, aci) == NOTOK) {
		acs_advise(aca, "initialization fails");
		return NOTOK;
	}
	advise(LLOG_NOTICE, NULLCP,
	       "A-ASSOCIATE.INDICATION: <%d, %s, %s, %s, %d>",
	       acs->acs_sd, oid2ode(acs->acs_context),
	       sprintaei(&acs->acs_callingtitle), sprintaei(&acs->acs_calledtitle), acs->acs_ninfo);

	sd = acs->acs_sd;

	for (vec++; *vec; vec++)
		advise(LLOG_EXCEPTIONS, NULLCP, "unknown argument \"%s\"", *vec);

	reply = startfnx ? (*startfnx) (sd, acs) : ACS_ACCEPT;

	result = AcAssocResponse(sd, reply, reply != ACS_ACCEPT
				 ? ACS_USER_NOREASON : ACS_USER_NULL,
				 NULLOID, NULLAEI,
				 NULLPA, NULLPC, ps->ps_defctxresult,
				 ps->ps_prequirements, ps->ps_srequirements,
				 SERIAL_NONE, ps->ps_settings, &ps->ps_connect, NULLPEP, 0, aci);

	ACSFREE(acs);

	if (result == NOTOK) {
		acs_advise(aca, "A-ASSOCIATE.RESPONSE");
		return NOTOK;
	}
	if (reply != ACS_ACCEPT)
		return NOTOK;

	if (RoSetService(sd, RoPService, roi) == NOTOK)
		ros_adios(rop, "set RO/PS fails");

	return sd;
}

static int
ros_work(fd)
	int fd;
{
	int result;
	caddr_t out;
	struct AcSAPindication acis;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	switch (setjmp(toplevel)) {
	case OK:
		break;

	default:
		if (stopfnx)
			(*stopfnx) (fd, (struct AcSAPfinish *) 0);
	case DONE:
		(void) AcUAbortRequest(fd, NULLPEP, 0, &acis);
		(void) RyLose(fd, roi);
		return NOTOK;
	}

	switch (result = RyWait(fd, NULLIP, &out, OK, roi)) {
	case NOTOK:
		if (rop->rop_reason == ROS_TIMER)
			break;
	case OK:
	case DONE:
		ros_indication(fd, roi);
		break;

	default:
		adios(NULLCP, "unknown return from RoWaitRequest=%d", result);
	}

	return OK;
}

static int
ros_indication(sd, roi)
	int sd;
	register struct RoSAPindication *roi;
{
	int reply, result;

	switch (roi->roi_type) {
	case ROI_INVOKE:
	case ROI_RESULT:
	case ROI_ERROR:
		adios(NULLCP, "unexpected indication type=%d", roi->roi_type);
		break;

	case ROI_UREJECT:
	{
		register struct RoSAPureject *rou = &roi->roi_ureject;

		if (rou->rou_noid)
			advise(LLOG_EXCEPTIONS, NULLCP,
			       "RO-REJECT-U.INDICATION/%d: %s", sd, RoErrString(rou->rou_reason));
		else
			advise(LLOG_EXCEPTIONS, NULLCP,
			       "RO-REJECT-U.INDICATION/%d: %s (id=%d)",
			       sd, RoErrString(rou->rou_reason), rou->rou_id);
	}
		break;

	case ROI_PREJECT:
	{
		register struct RoSAPpreject *rop = &roi->roi_preject;

		if (ROS_FATAL(rop->rop_reason))
			ros_adios(rop, "RO-REJECT-P.INDICATION");
		ros_advise(rop, "RO-REJECT-P.INDICATION");
	}
		break;

	case ROI_FINISH:
	{
		register struct AcSAPfinish *acf = &roi->roi_finish;
		struct AcSAPindication acis;
		register struct AcSAPabort *aca = &acis.aci_abort;

		advise(LLOG_NOTICE, NULLCP, "A-RELEASE.INDICATION/%d: %d", sd, acf->acf_reason);

		reply = stopfnx ? (*stopfnx) (sd, acf) : ACS_ACCEPT;

		result = AcRelResponse(sd, reply, ACR_NORMAL, NULLPEP, 0, &acis);

		ACFFREE(acf);

		if (result == NOTOK)
			acs_advise(aca, "A-RELEASE.RESPONSE");
		else if (reply != ACS_ACCEPT)
			break;
		longjmp(toplevel, DONE);
	}
		/* NOTREACHED */

	default:
		adios(NULLCP, "unknown indication type=%d", roi->roi_type);
	}
}

static int
ros_lose(td)
	struct TSAPdisconnect *td;
{
	if (td->td_cc > 0)
		adios(NULLCP, "TNetAccept: [%s] %*.*s",
		      TErrString(td->td_reason), td->td_cc, td->td_cc, td->td_data);
	else
		adios(NULLCP, "TNetAccept: [%s]", TErrString(td->td_reason));
}

...
