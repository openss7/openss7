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

/* ryinitiator.c - remote association initiator */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/idist/RCS/ryinitiator.c,v 9.0 1992/06/16 14:38:53 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/idist/RCS/ryinitiator.c,v 9.0 1992/06/16 14:38:53 isode Rel
 *
 * Severely hacked to give embedded functionality for client.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science
 * 
 *
 * Log: ryinitiator.c,v
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 */

#include <stdio.h>
#include <varargs.h>
#include "Idist-types.h"
#include "Idist-ops.h"
#include "defs.h"

/*    DATA */

void adios(), advise(), ros_adios(), ros_advise(), acs_advise(), acs_adios();

char *getstring();

static char *myservice = "isode idist";

static char *mycontext = "isode idist";
static char *mypci = "isode idist pci";

extern char *isodeversion;

static int ryconnect();

/*    INITIATOR */

makeconn(thehost)
	char *thehost;
{
	int result;
	PE data;
	struct type_Idist_Initiate *initial;
	char *cp;
	char buf[BUFSIZ];
	char ruser[128], *rhost;
	static char lasthost[BUFSIZ];

	if (lasthost[0] != 0) {
		if (strcmp(thehost, lasthost) == 0)
			return 1;
		closeconn();
	}

	(void) strcpy(lasthost, thehost);

	if ((initial = (struct type_Idist_Initiate *)
	     malloc(sizeof *initial)) == NULL)
		adios("memory", "out of");

	initial->version = VERSION;

	if (cp = index(thehost, '@')) {
		rhost = cp + 1;
		(void) strncpy(ruser, thehost, cp - thehost);
		ruser[cp - thehost] = 0;
	} else {
		(void) strcpy(ruser, user);
		rhost = thehost;
	}
	if (!qflag)
		(void) printf("updating host %s\n", rhost);
	(void) sprintf(buf, "user (%s:%s): ", rhost, ruser);
	cp = getstring(buf);
	if (cp == NULL)
		cp = ruser;
	initial->user = str2qb(cp, strlen(cp), 1);

	(void) sprintf(buf, "password (%s:%s): ", rhost, cp);
	cp = getpassword(buf);

	initial->passwd = str2qb(cp, strlen(cp), 1);
	bzero(cp, strlen(cp));	/* in case of cores */

	if (encode_Idist_Initiate(&data, 1, 0, NULLCP, initial) == NOTOK) {
		advise(NULLCP, "Error encoding data");
		return 0;
	}
	data->pe_context = 3;	/* hack */

	result = ryconnect(rhost, data, myservice, mycontext, mypci);

	free_Idist_Initiate(initial);

	return result == OK ? 1 : 0;
}

static int ry_sd = NOTOK;

static int
ryconnect(thehost, data, theservice, thecontext, thepci)
	char *thehost, *theservice, *thecontext, *thepci;
	PE data;
{
	struct SSAPref sfs;
	register struct SSAPref *sf;
	register struct PSAPaddr *pa;
	struct AcSAPconnect accs;
	register struct AcSAPconnect *acc = &accs;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;
	AEI aei;
	OID ctx, pci;
	struct PSAPctxlist pcs;
	register struct PSAPctxlist *pc = &pcs;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	if ((aei = _str2aei(thehost, theservice, thecontext, 0, NULLCP, NULLCP))
	    == NULLAEI)
		adios(NULLCP, "unable to resolve service: %s", PY_pepy);
	if ((pa = aei2addr(aei)) == NULLPA)
		adios(NULLCP, "address translation failed");

	if ((ctx = ode2oid(thecontext)) == NULLOID)
		adios(NULLCP, "%s: unknown object descriptor", thecontext);
	if ((ctx = oid_cpy(ctx)) == NULLOID)
		adios(NULLCP, "out of memory");
	if ((pci = ode2oid(thepci)) == NULLOID)
		adios(NULLCP, "%s: unknown object descriptor", thepci);
	if ((pci = oid_cpy(pci)) == NULLOID)
		adios(NULLCP, "out of memory");
	pc->pc_nctx = 1;
	pc->pc_ctx[0].pc_id = 1;
	pc->pc_ctx[0].pc_asn = pci;
	pc->pc_ctx[0].pc_atn = NULLOID;

	if ((sf = addr2ref(PLocalHostName())) == NULL) {
		sf = &sfs;
		(void) bzero((char *) sf, sizeof *sf);
	}

	if (AcAssocRequest(ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
			   0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, &data, 1, NULLQOS, acc, aci)
	    == NOTOK)
		acs_adios(aca, "A-ASSOCIATE.REQUEST");

	if (acc->acc_result != ACS_ACCEPT) {
		int slen;
		char *str;

		if (acc->acc_ninfo > 0 && (str = prim2str(acc->acc_info[0], &slen)))
			adios(NULLCP, "association rejected: [%s] %*.*s",
			      AcErrString(acc->acc_result), slen, slen, str);
		else
			adios(NULLCP, "association rejected: [%s]", AcErrString(acc->acc_result));
	}

	ry_sd = acc->acc_sd;
	ACCFREE(acc);

	if (RoSetService(ry_sd, RoPService, roi) == NOTOK)
		ros_adios(rop, "set RO/PS fails");
	return OK;
}

closeconn()
{
	struct AcSAPrelease acrs;
	register struct AcSAPrelease *acr = &acrs;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;

	if (ry_sd == NOTOK)
		return;

	if (AcRelRequest(ry_sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
		acs_adios(aca, "A-RELEASE.REQUEST");

	if (!acr->acr_affirmative) {
		(void) AcUAbortRequest(ry_sd, NULLPEP, 0, aci);
		adios(NULLCP, "release rejected by peer: %d", acr->acr_reason);
	}

	ACRFREE(acr);
}

/*  */

invoke(op, arg, mod, ind, rfx, efx)
	int op;
	modtyp *mod;			/* encoding/decoding table for Idist */
	int ind;			/* index of this type in tables */
	caddr_t arg;
	IFP rfx, efx;
{
	int result;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;
	extern int result_value;

	switch (result = RyStub(ry_sd, table_Idist_Operations, op,
				RyGenID(ry_sd), NULLIP, arg, rfx, efx, ROS_SYNC, roi)) {
	case NOTOK:		/* failure */
		if (ROS_FATAL(rop->rop_reason))
			ros_adios(rop, "STUB");
		ros_advise(rop, "STUB");
		break;

	case OK:		/* got a result/error response */
		break;

	case DONE:		/* got RO-END? */
		adios(NULLCP, "got RO-END.INDICATION");
		/* NOTREACHED */

	default:
		adios(NULLCP, "unknown return from RyStub=%d", result);
		/* NOTREACHED */
	}

	if (mod && ind >= 0 && arg)
		(void) fre_obj(arg, mod->md_dtab[ind], mod, 1);

	return result_value;
}

/*  */

/*  */

sighandler_t cleanup;

void
ros_adios(rop, event)
	register struct RoSAPpreject *rop;
	char *event;
{
	ros_advise(rop, event);

	cleanup();

	_exit(1);
}

void
ros_advise(rop, event)
	register struct RoSAPpreject *rop;
	char *event;
{
	char buffer[BUFSIZ];

	if (rop->rop_cc > 0)
		(void) sprintf(buffer, "[%s] %*.*s", RoErrString(rop->rop_reason),
			       rop->rop_cc, rop->rop_cc, rop->rop_data);
	else
		(void) sprintf(buffer, "[%s]", RoErrString(rop->rop_reason));

	advise(NULLCP, "%s: %s", event, buffer);
}

/*  */

void
acs_adios(aca, event)
	register struct AcSAPabort *aca;
	char *event;
{
	acs_advise(aca, event);

	cleanup();
	_exit(1);
}

void
acs_advise(aca, event)
	register struct AcSAPabort *aca;
	char *event;
{
	char buffer[BUFSIZ];

	if (aca->aca_cc > 0)
		(void) sprintf(buffer, "[%s] %*.*s",
			       AcErrString(aca->aca_reason),
			       aca->aca_cc, aca->aca_cc, aca->aca_data);
	else
		(void) sprintf(buffer, "[%s]", AcErrString(aca->aca_reason));

	advise(NULLCP, "%s: %s (source %d)", event, buffer, aca->aca_source);
}

/*  */

#ifndef	lint
void _advise();

void
adios(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	cleanup();

	va_end(ap);

	_exit(1);
}
#else
/* VARARGS */

void
adios(what, fmt)
	char *what, *fmt;
{
	adios(what, fmt);
}
#endif

#ifndef	lint
void
advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);
}

static void
_advise(ap)
	va_list ap;
{
	char buffer[BUFSIZ];

	xsprintf(buffer, ap);

	(void) fflush(stdout);

	(void) fprintf(stderr, "%s: ", myname);
	(void) fputs(buffer, stderr);
	(void) fputc('\n', stderr);

	(void) fflush(stderr);
}
#else
/* VARARGS */

void
advise(what, fmt)
	char *what, *fmt;
{
	advise(what, fmt);
}
#endif

#ifndef	lint
void
ryr_advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);
}
#else
/* VARARGS */

void
ryr_advise(what, fmt)
	char *what, *fmt;
{
	ryr_advise(what, fmt);
}
#endif

char *
getstring(prompt)
	char *prompt;
{
	static char buffer[BUFSIZ];
	char *cp;

	(void) fputs(prompt, stdout);
	(void) fflush(stdout);

	if (fgets(buffer, sizeof buffer, stdin) == NULL)
		return NULLCP;

	if (cp = index(buffer, '\n'))
		*cp = '\0';
	if (buffer[0] == '\0')
		return NULLCP;
	return buffer;
}
