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

/* prim2time.c - presentation element to time string */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/prim2time.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/prim2time.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: prim2time.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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

/* LINTLIBRARY */

#include <ctype.h>
#include <stdio.h>
#include "psap.h"

/*    DATA */

#define	YEAR(y)		((y) >= 100 ? (y) : (y) + 1900)

static long get_usec();

/*  */

UTC
prim2time(pe, generalized)
	register PE pe;
	int generalized;
{
	int len;
	register char *cp;
	register UTC u;

	UTC(*aux) ();

	aux = generalized ? str2gent : str2utct;
	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if (pe->pe_prim == NULLPED)
			return pe_seterr(pe, PE_ERR_PRIM, NULLUTC);
		u = (*aux) ((char *) pe->pe_prim, (int) pe->pe_len);
		break;

	case PE_FORM_CONS:
		if ((cp = prim2str(pe, &len)) == NULLCP)
			return NULLUTC;
		u = len ? (*aux) (cp, len) : NULLUTC;
		free(cp);
		break;
	}

	return (u ? u : pe_seterr(pe, generalized ? PE_ERR_GENT : PE_ERR_UTCT, NULLUTC));
}

/*  */

UTC
str2utct(cp, len)
	register char *cp;
	register int len;
{
	int year, hours, mins;
	register int zone;
	static UTCtime ut;
	register UTC u = &ut;

	bzero((char *) u, sizeof *u);

	if (sscanf(cp, "%2d%2d%2d%2d%2d", &year, &u->ut_mon,
		   &u->ut_mday, &u->ut_hour, &u->ut_min) != 5)
		return NULLUTC;
	cp += 10, len -= 10;
	u->ut_year = YEAR(year);

	if (len > 0 && isdigit((u_char) *cp)) {
		if (sscanf(cp, "%2d", &u->ut_sec) != 1)
			return NULLUTC;
		u->ut_flags |= UT_SEC;
		cp += 2, len -= 2;
	}

	if (len > 0) {
		switch (*cp) {
		case 'Z':
			cp++, len--;
			break;

		case '+':
		case '-':
			if (sscanf(cp + 1, "%2d%2d", &hours, &mins) != 2)
				return NULLUTC;
			zone = hours * 60 + mins;
			u->ut_zone = *cp == '+' ? zone : -zone;
			cp += 5, len -= 5;
			break;

		default:
			return NULLUTC;
		}
		u->ut_flags |= UT_ZONE;
	}
	if (len != 0)
		return NULLUTC;

	return u;
}

/*  */

UTC
str2gent(cp, len)
	char *cp;
	int len;
{
	int hours, mins;
	long usec;
	register int zone;
	static UTCtime ut;
	register UTC u = &ut;

	bzero((char *) u, sizeof *u);

	if (sscanf(cp, "%4d%2d%2d%2d", &u->ut_year, &u->ut_mon, &u->ut_mday, &u->ut_hour) != 4)
		return NULLUTC;
	cp += 10, len -= 10;

	if (len > 0)
		switch (*cp) {
		case '.':
		case ',':
			cp++, len--;
			if ((usec = get_usec(&cp, &len)) < 0)
				return NULLUTC;
			u->ut_min = (u->ut_sec = usec / 1000000) / 60;
			u->ut_sec %= 60;
			u->ut_usec = usec % 1000000;
			u->ut_flags |= UT_SEC | UT_USEC;
			goto get_zone;

		default:
			if (isdigit((u_char) *cp)) {
				if (sscanf(cp, "%2d", &u->ut_min) != 1)
					return NULLUTC;
				cp += 2, len -= 2;
			}
			break;
		}

	if (len > 0)
		switch (*cp) {
		case '.':
		case ',':
			cp++, len--;
			if ((usec = get_usec(&cp, &len)) < 0)
				return NULLUTC;
			if ((u->ut_sec = usec / 1000000) >= 60)
				return NULLUTC;
			u->ut_usec = usec % 1000000;
			u->ut_flags |= UT_SEC | UT_USEC;
			goto get_zone;

		default:
			if (isdigit((u_char) *cp)) {
				if (sscanf(cp, "%2d", &u->ut_sec) != 1)
					return NULLUTC;
				u->ut_flags |= UT_SEC;
				cp += 2, len -= 2;
			}
			break;
		}

	if (len > 0)
		switch (*cp) {
		case '.':
		case ',':
			cp++, len--;
			if ((usec = get_usec(&cp, &len)) < 0)
				return NULLUTC;
			if ((u->ut_usec = usec) >= 1000000)
				return NULLUTC;
			u->ut_flags |= UT_USEC;
			goto get_zone;

		default:
			break;
		}

      get_zone:;
	if (len > 0) {
		switch (*cp) {
		case 'Z':
			cp++, len--;
			break;

		case '+':
		case '-':
			if (sscanf(cp + 1, "%2d%2d", &hours, &mins) != 2)
				return NULLUTC;
			zone = hours * 60 + mins;
			u->ut_zone = *cp == '+' ? zone : -zone;
			cp += 5, len -= 5;
			break;

		default:
			return NULLUTC;
		}
		u->ut_flags |= UT_ZONE;
	}
	if (len != 0)
		return NULLUTC;

	return u;
}

/*  */

/* not perfect, but what is? */

static long
get_usec(cp, len)
	char **cp;
	int *len;
{
	register int j;
	register long i;
	register char *dp;

	i = 0L;
	for (dp = *cp, j = 0; isdigit((u_char) *dp); dp++, j++)
		if (j < 6)
			i = i * 10L + (long) (*dp - '0');

	*cp = dp, *len -= j;

	while (j++ < 6)
		i *= 10L;

	return i;
}
