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

/* gtime.c - inverse gmtime */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/gtime.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/gtime.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: gtime.c,v
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

#include <stdio.h>
#include "psap.h"
#ifdef	OSX
#include <sys/time.h>
#endif
#ifdef	notdef
#include <sys/timeb.h>
#endif

/*    DATA */

/* gtime(): the inverse of localtime().
	This routine was supplied by Mike Accetta at CMU many years ago.
 */

#ifdef masscomp
extern int dmsize[];
#else
int dmsize[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
#endif

#define	dysize(y)	\
	(((y) % 4) ? 365 : (((y) % 100) ? 366 : (((y) % 400) ? 365 : 366)))

#define	YEAR(y)		((y) >= 100 ? (y) : (y) + 1900)

/*  */

long
gtime(tm)
	register struct tm *tm;
{
	register int i, sec, mins, hour, mday, mon, year;
	register long result;

#ifdef	notdef
	long local;
	struct timeb tb;
#endif

	if ((sec = tm->tm_sec) < 0 || sec > 59
	    || (mins = tm->tm_min) < 0 || mins > 59
	    || (hour = tm->tm_hour) < 0 || hour > 24
	    || (mday = tm->tm_mday) < 1 || mday > 31 || (mon = tm->tm_mon + 1) < 1 || mon > 12)
		return ((long) NOTOK);
	if (hour == 24) {
		hour = 0;
		mday++;
	}
	year = YEAR(tm->tm_year);

	result = 0L;
	for (i = 1970; i < year; i++)
		result += dysize(i);
	if (dysize(year) == 366 && mon >= 3)
		result++;
	while (--mon)
		result += dmsize[mon - 1];
	result += mday - 1;
	result = 24 * result + hour;
	result = 60 * result + mins;
	result = 60 * result + sec;

#ifdef	notdef
	(void) ftime(&tb);
	result += 60 * tb.timezone;
	local = result;
	if ((tm = localtime(&local)) && tm->tm_isdst)
		result -= 60 * 60;
#endif

	return result;
}
