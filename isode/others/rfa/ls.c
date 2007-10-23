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
 /*
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * ls.c : prepare file info in a "ls" style
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/ls.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: ls.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/ls.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include <sys/types.h>
#include <sys/stat.h>
#include "general.h"
#include "RFA-types.h"
#include "rfa.h"
#include "rfainfo.h"

struct pair {
	u_short val;
	char *s;
};

static struct pair ftype[] = { {S_IFIFO, "?"}, {S_IFCHR, "c"}, {S_IFDIR, "d"},
{S_IFBLK, "b"}, {S_IFREG, "-"}, {S_IFLNK, "l"},
{S_IFSOCK, "s"}, {0, NULL}
};

static struct pair facc[] = { {01, "--x"}, {02, "-w-"}, {03, "-wx"},
{04, "r--"}, {05, "r-x"}, {06, "rw-"},
{07, "rwx"}, {00, "---"}
};

static void
mode2str(m, mstr)
	u_short m;
	char *mstr;
{
	u_short v;
	struct pair *pp;

	v = m & S_IFMT;
	for (pp = ftype; pp->s; pp++)
		if (v == pp->val) {
			strcat(mstr, pp->s);
			break;
		}
	v = (u_short) (m & 0700) >> 6;
	for (pp = facc; pp->s; pp++)
		if (v == pp->val) {
			strcat(mstr, pp->s);
			break;
		}
	if (m & S_ISUID)
		*(mstr + strlen(mstr) - 1) = 's';
	v = (u_short) (m & 070) >> 3;
	for (pp = facc; pp->s; pp++)
		if (v == pp->val) {
			strcat(mstr, pp->s);
			break;
		}
	if (m & S_ISGID)
		*(mstr + strlen(mstr) - 1) = 's';
	v = m & 07;
	for (pp = facc; pp->s; pp++)
		if (v == pp->val) {
			strcat(mstr, pp->s);
			break;
		}
}

char *
shortTime(t)
	long *t;
{
	char *s;

	s = ctime(t);

	*(rindex(s, ':')) = '\0';

	s += 4;
	return s;
}

char *
rfa2ls(rfa)
	struct RfaInfo *rfa;
{
	static char buf[512], *bp;

	*buf = '\0';
	bp = buf;

	mode2str(rfa->ri_mode, bp);
	bp += strlen(bp);

	sprintf(bp, " %3.3s", status2str(rfa->ri_status));
	bp += strlen(bp);

	if (IS_LOCKED(rfa->ri_status)) {
		sprintf(bp, " lockby %-10s %8d", rfa->ri_lckname, rfa->ri_size);
	} else
		sprintf(bp, " %-8s %-8s %8d", rfa->ri_owner, rfa->ri_group, rfa->ri_size);
	bp += strlen(bp);

	sprintf(bp, " %12s", shortTime(&(rfa->ri_modTime)));
	bp += strlen(bp);
	sprintf(bp, " %s", rfa->ri_filename);
	if ((rfa->ri_mode & S_IFMT) == S_IFLNK) {
		bp += strlen(bp);
		sprintf(bp, " -> %s", rfa->ri_lnkName);
	}

	return buf;
}
