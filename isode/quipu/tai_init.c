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

/* tai_init.c - */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/tai_init.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/tai_init.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: tai_init.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "tailor.h"

extern char *dsatailfile;

#define MAXTAIARGS      100

extern LLog *log_dsap;

dsa_tai_init(name)
	char *name;
{
	FILE *fp;
	char buf[BUFSIZ];
	char *cp;

	isodetailor(name, 0);

	if ((fp = fopen(cp = isodefile(dsatailfile, 0), "r")) == (FILE *) NULL) {
		LLOG(log_dsap, LLOG_FATAL, ("Cannot open tailor file '%s'", cp));
		fatal(-46, "Cannot open quiputailor");
	}

	while (fgets(buf, sizeof(buf), fp) != NULLCP)
		if ((*buf != '#') && (*buf != '\n'))
			/* not a comment or blank */
			if (dsa_tai_string(buf) == NOTOK)
				LLOG(log_dsap, LLOG_EXCEPTIONS, ("tai_string failed %s", buf));

	(void) fclose(fp);
	isodexport(NULLCP);
	return OK;
}

dsa_tai_string(str)
	char *str;
{
	char *args[MAXTAIARGS];
	char *p;
	int ac;

	if ((p = index(str, '\n')) != NULLCP)
		*p = '\0';

	if ((ac = sstr2arg(str, MAXTAIARGS, args, " \t,")) == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("too many tailor parameters"));
		return (NOTOK);
	}
	if (ac <= 1) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("no option set", str));
		return (NOTOK);
	}
	return (dsa_sys_tai(ac, args));

}
