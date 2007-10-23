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
 * llock.c : do a local lock if possible
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/llock.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: llock.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/llock.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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

#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "general.h"
#include "logger.h"
#include "rfa.h"
#include "rfainfo.h"

extern char *fsBase;
extern char *strtok();
extern char *getRfaContext();
extern char *isodetcpath;
extern int commandMode;
extern char *sys_errname();

#ifdef __STDC__
extern char *strdup(const char *);
#else
extern char *strdup();
#endif

char *myname;
int connected = 0;
FILE *err, *out;
char cwd_remote[512];
int interactive = 1;
int retcode;

cleanup()
{
}

/*--------------------------------------------------------------*/
/*  errMsg                                                      */
/*--------------------------------------------------------------*/
char *
errMsg(type)
	int type;
{
	if (type == NOTOK)
		return rfaErrStr;
	else
		return sys_errname(errno);
}

/*--------------------------------------------------------------*/
/*  getLocalFileRfaInfo						*/
/*--------------------------------------------------------------*/
int
getLocalRfaInfo(fn, rfap, rfalp, reg)
	char **fn;
	struct RfaInfo **rfap, **rfalp;
	int reg;
{
	int rc;

    /*--- expand symbolic links in fn ---*/
	if ((*fn = getRfaContext(cwd_remote, *fn)) == NULL) {
		fprintf(err,
			"*** local file access error : not within RFA subtree %s ***\n", fsBase);
		return (NOTOK_OUTOFSUBTREE);
	}
	if ((*fn = expandSymLinks(*fn)) == NULL) {
		fprintf(err, "*** local file access error : %s ***\n", rfaErrStr);
		return (NOTOK_OUTOFSUBTREE);
	}

    /*--- get file Info ---*/
	if ((rc = getRfaInfoList(dirname(*fn), rfalp, basename(*fn), 1)) != OK) {
		fprintf(err,
			"*** local file access error : can't read %s/.rfainfo (%s) ***\n",
			dirname(*fn), errMsg(rc));
		return (NOTOK_FILEACCESS);
	}

	if ((*rfap = findRfaInfo(basename(*fn), *rfalp)) == NULL) {
		releaseRfaInfoList(*fn, *rfalp);
		*rfalp = NULL;
		fprintf(err, "*** local file access error : %s does not exist ***\n", *fn);
		return (NOTOK_FILEACCESS);
	}

    /*--- check if regular file ---*/
	if (reg)
		if (((*rfap)->ri_mode & S_IFMT) != S_IFREG) {
			releaseRfaInfoList(*fn, *rfalp);
			fprintf(err, " *** status error : not a regular file ***\n");
			*rfalp = NULL;
			return NOTOK_NOTREGULAR;
		}

	return OK;
}

/*--------------------------------------------------------------*/
/*  unlockFile							*/
/*--------------------------------------------------------------*/
do_lunlock(fn)
	char *fn;
{
	int rc;
	struct RfaInfo *rfalist, *rfa;

    /*--- get file Info ---*/
	if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 1)) != OK)
		return rc;

    /*--- check if we are master ---*/
	if (IS_SLAVE(rfa->ri_status)) {
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(err, " *** local file %s is slave version ***\n", fn);
		return NOTOK_IS_SLAVE;
	}
	if (!IS_LOCKED(rfa->ri_status)) {
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(err, " *** local file %s not locked ***\n", fn);
		return NOTOK_NOTLOCKED;
	}
	SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);
	rfa->ri_lcksince = NULL;
	free(rfa->ri_lckname);
	rfa->ri_lckname = "NONE";

	if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
		return NOTOK_FILEACCESS;
	}
	releaseRfaInfoList(dirname(fn), rfalist);
	return OK;
}

/*--------------------------------------------------------------*/
/*  lockFile							*/
/*--------------------------------------------------------------*/
do_llock(fn)
	char *fn;
{
	int res, rc;
	struct RfaInfo *rfalist, *rfa;

    /*--- get file Info ---*/
	if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 1)) != OK)
		return rc;

    /*--- check if we are master ---*/
	if (IS_MASTER(rfa->ri_status) || IS_UNREGISTERED(rfa->ri_status)) {
		if (IS_LOCKED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, " *** file already locked by %s since %s ***\n",
				rfa->ri_lckname, shortTime(&(rfa->ri_lcksince)));
			return NOTOK_LOCKED;
		}
		SET_LOCKINFO(rfa->ri_status, RI_LOCKED);
		rfa->ri_lckname = strdup(getenv("USER"));
		(void) time(&(rfa->ri_lcksince));
		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err,
				"*** local file access error : can't write rfainfo (%s) ***\n",
				errMsg(rc));
			return NOTOK_FILEACCESS;
		}
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "locked file %s\n", fn);
		return OK;
	}

    /*-- here we will need help from Big-Brother "rfa" --*/
	return NOTOK_LOCAL_LOCK;
}

main(ac, av)
	int ac;
	char **av;
{
	char c, *cwd, buf[BUFSIZ];
	char *cmd = NULL;
	int rc;

	myname = av[0];

	out = stdout;
	err = stderr;

	initLog(myname);

	if (ac < 2) {
		fprintf(stderr, "USAGE: %s filename [-q]\n", basename(myname));
		exit(1);
	}
	if (ac > 2 && strcmp(av[2], "-q") == 0)
		interactive = 0;

    /*--- rfa tailoring ---*/
	sprintf(buf, "%s/rfatailor", isodetcpath);
	if (tailor(buf) != OK) {
		fprintf(stderr, "*** tailoring %s - %s ***\n", buf, rfaErrStr);
		exit(1);
	}
	sprintf(buf, "%s/rfatailor", RFA_TAILDIR);
	if (tailor(buf) != OK) {
		fprintf(stderr, "*** tailoring %s - %s ***\n", buf, rfaErrStr);
		exit(1);
	}
	sprintf(buf, "%s/.rfarc", getenv("HOME"));
	if (tailor(buf) != OK) {
		fprintf(stderr, "*** tailoring %s - %s ***\n", buf, rfaErrStr);
		exit(1);
	}

    /*--- init cwd ---*/
	(void) getwd(buf);
	if (strncmp(buf, fsBase, strlen(fsBase)) == 0)
		strcpy(cwd_remote, buf + strlen(fsBase));

	interactive = 0;
	commandMode = 1;
	if (strcmp(basename(myname), "llock") == 0)
		rc = do_llock(av[1]);
	else
		rc = do_lunlock(av[1]);

	if (interactive && (rc == 2))
		fprintf(stderr, "*** can't LOCK locally, must use RFA command ***\n");

	exit(rc);
}
