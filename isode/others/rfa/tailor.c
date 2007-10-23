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
 * tailor.c - read rfatailor and .rfarc file
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/tailor.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: tailor.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/tailor.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include <ctype.h>
#include "general.h"
#include "logger.h"
#include "rfa.h"
#include "rfainfo.h"

extern char *fsBase;
extern char *user, *host, *passwd extern char *realpath();
extern LLog *pgm_log;

#ifdef __STDC__
extern char *strdup(const char *);
#else
extern char *strdup();
#endif

int default_transfer = RI_TR_REQ;
int doChown = 0;
int doChgrp = 1;
int doChmod = 1;
int doClearSUID = 1;
int doRmWidows = 0;
int doRfaExec = 1;
int timeSlave = 0;
int compLimit = COMPRESSLIMIT;
char *passwd = "rfa";
char *user = "rfa";
char *host = "localhost";
int backup = 0;

#define VERROR(v)	{ sprintf(errp,"\n\tinvalid tailor value '%s'",(v));\
			  errp += strlen(errp); \
			  err++; }

char *
cut(s)
	char *s;
{
	register char *s1;

	for (; *s && isspace(*s); s++) ;
	for (; *s && (*s == '"'); s++) ;
	for (s1 = s + strlen(s) - 1; (s1 != s) && isspace(*s1); s1--) ;
	for (; *s1 && (*s1 == '"'); s1--) ;
	if (*s1)
		*(s1 + 1) = '\0';
	return s;
}

int
tailor(fn)
	char *fn;
{
	FILE *f;
	char buf[BUFSIZ];
	char *errp, *v, *o, *index();
	int err = 0;

	if ((f = fopen(fn, "r")) == NULL)
		return OK;

	errp = rfaErrStr;
	while (fgets(buf, BUFSIZ, f)) {
		for (v = buf; isspace(*v); v++) ;
		if (*v == '\0')
			continue;
		if (*buf == '#')
			continue;
		if ((v = index(buf, '=')) == NULL) {
			sprintf(rfaErrStr, "\n\tinvalid tailor string '%s'", cut(buf));
			fclose(f);
			return NOTOK_LOCAL_ERROR;
		}
		*v = '\0';
		v = cut(v + 1);
		o = cut(buf);

	/*--- USER ---*/
		if (!strcasecmp(o, "user")) {
			user = strdup(v);
		} else
	/*--- PASSWORD ---*/
		if (!strcasecmp(o, "password")) {
			passwd = strdup(v);
		} else
	/*--- HOST ---*/
		if (!strcasecmp(o, "host")) {
			host = strdup(v);
		} else
	/*--- ROOT ---*/
		if (!strcasecmp(o, "root")) {
			char buf[BUFSIZ];

			if (realpath(v, buf) == NULLCP) {
				sprintf(errp, "\n\tinvalid local root '%s'", cut(v));
				errp += strlen(errp);
				err++;
			}
			fsBase = strdup(buf);
		} else
	/*--- COMPRESS ---*/
		if (!strcasecmp(o, "compress")) {
			if ((compLimit = atoi(v)) == 0)
				VERROR(v);
		} else
	/*--- BACKUP ---*/
		if (!strcasecmp(o, "backup")) {

			if (!strcasecmp(v, "on"))
				backup = 1;
			else if (!strcasecmp(v, "off"))
				backup = 0;
			else
				VERROR(v);
		} else
	/*--- CHGRP ---*/
		if (!strcasecmp(o, "chgrp")) {

			if (!strcasecmp(v, "on"))
				doChgrp = 1;
			else if (!strcasecmp(v, "off"))
				doChgrp = 0;
			else
				VERROR(v);
		} else
	/*--- TIME ---*/
		if (!strcasecmp(o, "time")) {

			if (!strcasecmp(v, "slave"))
				timeSlave = 1;
			else if (!strcasecmp(v, "master"))
				timeSlave = 0;
			else
				VERROR(v);
		} else
	/*--- CHOWN ---*/
		if (!strcasecmp(o, "chown")) {
			if (!strcasecmp(v, "on")) {
				if (geteuid() != 0) {
					sprintf(errp,
						"\n\tWARNING: must run as root for <chown> option");
					errp += strlen(errp);
					doChown = 0;
				} else
					doChown = 1;
			} else if (!strcasecmp(v, "off"))
				doChown = 0;
			else
				VERROR(v);
		} else
	/*--- CHMOD ---*/
		if (!strcasecmp(o, "chmod")) {
			if (!strcasecmp(v, "on"))
				doChmod = 1;
			else if (!strcasecmp(v, "off"))
				doChmod = 0;
			else
				VERROR(v);
		} else
	/*--- STRIP SUID ---*/
		if (!strcasecmp(o, "clearsuid")) {
			if (!strcasecmp(v, "on"))
				doClearSUID = 1;
			else if (!strcasecmp(v, "off"))
				doClearSUID = 0;
			else
				VERROR(v);
		} else
	/*--- REMOVE SLAVES ---*/
		if (!strcasecmp(o, "rmslaves")) {
			if (!strcasecmp(v, "on"))
				doRmWidows = 1;
			else if (!strcasecmp(v, "off"))
				doRmWidows = 0;
			else
				VERROR(v);
		} else
	/*--- LOGDEBUG ---*/
		if (!strcasecmp(o, "debuglog")) {
			if (!strcasecmp(v, "on"))
				pgm_log->ll_events |= LLOG_DEBUG | LLOG_TRACE | LLOG_NOTICE;
			else if (!strcasecmp(v, "off"))
				pgm_log->ll_events = LLOG_EXCEPTIONS | LLOG_FATAL;
			else
				VERROR(v);

		} else
	/*--- RFA EXEC ---*/
		if (!strcasecmp(o, "rfaexec")) {
			if (!strcasecmp(v, "on"))
				doRfaExec = 1;
			else if (!strcasecmp(v, "off"))
				doRfaExec = 0;
			else
				VERROR(v);
		} else
	/*--- TRANSFER ---*/
		if (!strcasecmp(o, "transfer")) {
			if (!strcasecmp(v, "request"))
				default_transfer = RI_TR_REQ;
			else if (!strcasecmp(v, "auto"))
				default_transfer = RI_TR_AUTO;
			else
				VERROR(v);

		} else {
			sprintf(errp, "\n\tinvalid tailor option '%s'", o);
			errp += strlen(errp);
			err++;
		}
	}
	fclose(f);
	if (err)
		return NOTOK;
	return OK;
}
