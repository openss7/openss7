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

/* str2sel.c - string to selector */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/str2sel.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/str2sel.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: str2sel.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "tailor.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */

#define	QUOTE	'\\'

/* STR2SEL */

int
str2sel(s, quoted, sel, n)
	char *s, *sel;
	int quoted, n;
{
	int i, r;
	register char *cp;

	(void) rcsid;
	if (*s == '\0')
		return 0;

	if (quoted <= 0) {
		for (cp = s; *cp; cp++)
			if (!isxdigit((unsigned char) *cp))
				break;

		if (*cp == '\0' && (i = (cp - s)) >= 2 && (i & 0x01) == 0) {
			if (i > (r = n * 2))
				i = r;
			i = implode((unsigned char *) sel, s, i);
			if ((r = (n - i)) > 0) {
#ifdef HAVE_MEMSET
				memset(sel + i, 0, r);
#else				/* HAVE_MEMSET */
#ifdef HAVE_BZERO
				bzero(sel + i, r);
#else				/* HAVE_BZERO */
#error need memset or bzero
#endif				/* HAVE_BZERO */
#endif				/* HAVE_MEMSET */
			}
			return i;
		}
		if (*s == '#') {	/* gosip style, network byte-order */
			i = atoi(s + 1);
			sel[0] = (i >> 8) & 0xff;
			sel[1] = i & 0xff;

			return 2;
		}

		DLOG(compat_log, LLOG_EXCEPTIONS, ("invalid selector \"%s\"", s));
	}

	for (cp = sel; *s && n > 0; cp++, s++, n--)
		if (*s != QUOTE)
			*cp = *s;
		else
			switch (*++s) {
			case 'b':
				*cp = '\b';
				break;
			case 'f':
				*cp = '\f';
				break;
			case 'n':
				*cp = '\n';
				break;
			case 'r':
				*cp = '\r';
				break;
			case 't':
				*cp = '\t';
				break;

			case 0:
				s--;
			case QUOTE:
				*cp = QUOTE;
				break;

			default:
				if (!isdigit((unsigned char) *s)) {
					*cp++ = QUOTE;
					*cp = *s;
					break;
				}
				r = *s != '0' ? 10 : 8;
				for (i = 0; isdigit((unsigned char) *s); s++)
					i = i * r + *s - '0';
				s--;
				*cp = toascii(i);
				break;
			}
	if (n > 0)
		*cp = '\0';

	return (cp - sel);
}
