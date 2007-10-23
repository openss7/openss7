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

/* time.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/time.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/time.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: time.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

/*
	SYNTAX:
		time ::= 'yymmddhhmmssz'

		where yy   = year
		      mm   = month
		      dd   = day
		      hh   = hours
		      mm   = minutes
		      ss   = seconds
	              z    = timezone

	EXAMPLE:
		890602093221Z -> 09:32:21 GMT, on June 2nd 1989.
*/

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint();
extern int sfree();
extern int pstrcmp();

static UTC
qstr2utct(s, len)
	char *s;
	int len;
{
	UTC ut;

	if (len > 14 && strncmp(s, "1989", 4) == 0 && (ut = str2utct(s + 2, len - 2)))
		return ut;

	return str2utct(s, len);
}

#define	str2utct	qstr2utct

static PE
timeenc(x)
	char *x;
{
	PE ret_pe = NULLPE;

	/* Should switch to pepsy -> need to use qbufs! */

	(void) build_UNIV_UTCTime(&ret_pe, 0, 0, x, NULL);
	return (ret_pe);
}

static char *
timedec(pe)
	PE pe;
{
	char *x;

	if (parse_UNIV_UTCTime(pe, 0, 0, &x, NULL) == NOTOK)
		return (NULLCP);
	return (x);
}

utcprint(ps, xtime, format)
	PS ps;
	char *xtime;
	int format;
{
	UTC ut;

	if (format == READOUT && (ut = str2utct(xtime, strlen(xtime)))) {
		long mtime;

		mtime = gtime(ut2tm(ut));

		ps_printf(ps, "%-24.24s", ctime(&mtime));
	} else
		ps_printf(ps, "%s", xtime);
}

static
utccmp(a, b)
	char *a, *b;
{
	long a_time, mdiff;
	UTC ut;

	if ((ut = str2utct(a, strlen(a))) == NULL)
		return pstrcmp(a, b);
	a_time = gtime(ut2tm(ut));

	if ((ut = str2utct(b, strlen(b))) == NULL)
		return pstrcmp(a, b);

	return ((mdiff = a_time - gtime(ut2tm(ut))) == 0L ? 0 : mdiff > 0L ? 1 : -1);
}

time_syntax()
{
	(void) add_attribute_syntax("UTCTime",
				    (IFP) timeenc, (IFP) timedec,
				    (IFP) strdup, utcprint,
				    (IFP) strdup, utccmp, sfree, NULLCP, NULLIFP, FALSE);
}
