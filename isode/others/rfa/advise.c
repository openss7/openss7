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
 * advise.c - log error messages
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/advise.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: advise.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/advise.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include <varargs.h>
#include "manifest.h"
#include "logger.h"

static LLog _pgm_log = {
	"rfa.log", NULLCP, NULLCP,
	LLOG_NOTICE | LLOG_FATAL | LLOG_EXCEPTIONS, LLOG_NOTICE, 100,
	LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *pgm_log = &_pgm_log;

void
initLog(myname)
	char *myname;
{
/*    if (isatty (fileno (stderr)))
        ll_dbinit (pgm_log, myname);
    else */  {

		static char myfile[BUFSIZ];

		(void) sprintf(myfile, "%s.log", (strncmp(myname, "ros.", 4)
						  && strncmp(myname, "lpp.", 4))
			       || myname[4] == NULL ? myname : myname + 4);
		pgm_log->ll_file = myfile;
		ll_hdinit(pgm_log, myname);
	}
}

#ifndef	lint
void
adios(va_alist)
	va_dcl
{
	char *what;
	va_list ap;

	va_start(ap);

	_ll_log(pgm_log, LLOG_FATAL, ap);

	va_end(ap);

	cleanup();

	_exit(1);
}
#else
/* VARARGS2 */

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
	int code;
	va_list ap;

	va_start(ap);

	code = va_arg(ap, int);

	_ll_log(pgm_log, code, ap);

	va_end(ap);
}

#else
/* VARARGS */

void
advise(code, what, fmt)
	char *what, *fmt;
	int code;
{
	advise(code, what, fmt);
}
#endif

#ifndef	lint
void
ryr_advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_ll_log(pgm_log, LLOG_NOTICE, ap);

	va_end(ap);
}
#else
/* VARARGS2 */
void
ryr_advise(what, fmt)
	char *what, *fmt;
{
	ryr_advise(what, fmt);
}
#endif
