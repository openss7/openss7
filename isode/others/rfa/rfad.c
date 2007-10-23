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

/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfad.c : responder for RFA commands
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/rfad.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: rfad.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/rfad.c,v 9.0 1992/06/16 12:47:25 isode Rel";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <varargs.h>
#include "RFA-types.h"		/* type definitions */
#include "RFA-ops.h"		/* operation definitions */
#include "ryresponder.h"
#include "tsap.h"
#include "rfa.h"

static char *myservice = "rfa";
static char *mycontext = "rfa";

extern struct type_RFA_QueryResult *query();
extern struct type_RFA_FileList *do_listcdir();
extern int op_init();
extern int ros_init(), ros_work(), ros_indication(), ros_lose();
extern IFP startfnx, stopfnx;
extern char *isodetcpath;

extern int op_getFileData(), op_requestMaster(), op_listDir(), op_syncTime();
static struct dispatch dispatches[] = {
	"getFileData", operation_RFA_getFileData, op_getFileData,
	"requestMaster", operation_RFA_requestMaster, op_requestMaster,
	"listDir", operation_RFA_listDir, op_listDir,
	"syncTime", operation_RFA_syncTime, op_syncTime,
	NULL
};

char target[BUFSIZ];
char *host;
int groupid, userid;
char homedir[BUFSIZ];

/*   MAIN */

main(argc, argv)
	int argc;
	char **argv;
{
	int initiate();
	char buf[BUFSIZ];
	register struct dispatch *ds;
	AEI aei;
	struct TSAPdisconnect tds;
	struct TSAPdisconnect *td = &tds;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;
	char *myname;

	host = getlocalhost();
	if (myname = rindex(argv[0], '/'))
		myname++;
	else
		myname = argv[0];

   /*--- isode initialization and tailoring ---*/
	sprintf(buf, "HOME=%s", RFA_TAILDIR);
	putenv(buf);
	isodetailor(myname, 1);
	(void) initLog(myname);

    /*--- rfa tailoring ---*/
	sprintf(buf, "%s/rfatailor", isodetcpath);
	if (tailor(buf) != OK)
		advise(LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	sprintf(buf, "%s/rfatailor", RFA_TAILDIR);
	if (tailor(buf) != OK)
		advise(LLOG_EXCEPTIONS, NULLCP, rfaErrStr);

    /*--- get application entity identifier for rfa service ---*/
	if ((aei = _str2aei(host, myservice, mycontext, 0, NULLCP, NULLCP)) == NULLAEI)
		adios(NULLCP, "%s-%s: unknown application-entity", host, myservice);

    /*--- register operation to serve ---*/
	advise(LLOG_EXCEPTIONS, NULLCP, "doing");
	for (ds = dispatches; ds->ds_name; ds++)
		if (RyDispatch(NOTOK, table_RFA_Operations, ds->ds_operation,
			       ds->ds_vector, roi) == NOTOK)
			ros_adios(rop, ds->ds_name);

	advise(LLOG_EXCEPTIONS, NULLCP, "done");

	startfnx = initiate;
	stopfnx = NULLIFP;

	advise(LLOG_NOTICE, NULLCP, "starting");
	if (isodeserver(argc, argv, aei, ros_init, ros_work, ros_lose, td)
	    == NOTOK) {
		if (td->td_cc > 0)
			adios(NULLCP, "isodeserver [%s] %*.*s",
			      TErrString(td->td_reason), td->td_cc, td->td_cc, td->td_data);
		else
			adios(NULLCP, "isodeserver: [%s]", TErrString(td->td_reason));
	}

	exit(0);
}

cleanup()
{
}

/* ARGSUSED */
initiate(sd, acs, pe)
	int sd;
	struct AcSAPstart *acs;
	PE *pe;
{
	struct type_RFA_Initiate *initial;
	char *cp, *crypt();
	struct passwd *pw;

	*pe = NULLPE;
	if (acs->acs_ninfo != 1)
		return init_lose(ACS_PERMANENT, pe, "No Bind data");

	if (decode_RFA_Initiate(acs->acs_info[0], 1, NULLIP, NULLVP, &initial) == NOTOK)
		return init_lose(ACS_PERMANENT, pe, "Can't parse Bind data");

	user = qb2str(initial->user);

	advise(LLOG_NOTICE, NULLCP, "Bind of user %s", user);

	if (baduser(NULLCP, user)) {
		advise(LLOG_EXCEPTIONS, NULLCP, "Bad listed user '%s'", user);
		return init_lose(ACS_PERMANENT, pe, "Bad user/password");
	}

	if ((pw = getpwnam(user)) == NULL) {
		advise(LLOG_EXCEPTIONS, NULLCP, "Unknown user '%s'", user);
		return init_lose(ACS_PERMANENT, pe, "Bad user/password");

	}

	userid = pw->pw_uid;
	groupid = pw->pw_gid;
	(void) strcpy(homedir, pw->pw_dir);

	cp = qb2str(initial->password);

	if (pw->pw_passwd == NULL || strcmp(crypt(cp, pw->pw_passwd), pw->pw_passwd) != 0) {
		advise(LLOG_NOTICE, NULLCP, "Password mismatch for %s", user);
		return init_lose(ACS_PERMANENT, pe, "Bad user/password");

	}
	bzero(cp, strlen(cp));	/* in case of cores */
	free(cp);

	free_RFA_Initiate(initial);

	if (chdir(homedir) == -1) {
		advise(LLOG_NOTICE, NULLCP, "Can't set home directory to '%s'", homedir);
		return init_lose(ACS_PERMANENT, pe, "No home directory");
	}

	if (initUserId(userid, groupid, user) != OK) {
		advise(LLOG_NOTICE, NULLCP, "%s\n", rfaErrStr);
		return init_lose(ACS_PERMANENT, pe, rfaErrStr);
	}
	return ACS_ACCEPT;
}

init_lose(type, pe, str)
	int type;
	PE *pe;
	char *str;
{
	*pe = ia5s2prim(str, strlen(str));
	(*pe)->pe_context = 3;	/* magic!! - don't ask me why */
	return type;
}
