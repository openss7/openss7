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

/* sstr2arg: convert string into argument list */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/sstr2arg.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/sstr2arg.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: sstr2arg.c,v
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

#include <stdio.h>
#include "manifest.h"
#include "general.h"
#include <errno.h>
extern int errno;

/*  */

/*
   stash a pointer to each field into the passed array. any common seperators
   split the words.  extra white-space between fields is ignored.

   specially-interpreted characters:
	double-quote, backslash (preceding a special char with a backslash
	removes its interpretation.  A backslash not followed by a special is
	used to preface an octal specification for one character a string begun
	with double-quote has only double-quote and backslash as special
	characters.

*/

sstr2arg(srcptr, maxpf, argv, dlmstr)
	register char *srcptr;		/* source data */
	int maxpf;			/* maximum number of permitted fields */
	char *argv[];			/* where to put the pointers */
	char *dlmstr;			/* Delimiting character */
{
	char gotquote;			/* currently parsing quoted string */
	register int ind;
	register char *destptr;
	char idex[256];

	if (srcptr == 0)
		return (NOTOK);

	bzero(idex, sizeof idex);
	for (destptr = dlmstr; *destptr; destptr++)
		idex[*destptr] = 1;

	for (ind = 0, maxpf -= 2;; ind++) {
		if (ind >= maxpf)
			return (NOTOK);

		/* Skip leading white space */
		for (; *srcptr == ' ' || *srcptr == '\t'; srcptr++) ;

		argv[ind] = srcptr;
		destptr = srcptr;

		for (gotquote = 0;;) {

			if (idex[*srcptr]) {
				if (gotquote) {	/* don't interpret the char */
					*destptr++ = *srcptr++;
					continue;
				}

				srcptr++;
				*destptr = '\0';
				goto nextarg;
			} else {
				switch (*srcptr) {
				default:	/* just copy it */
					*destptr++ = *srcptr++;
					break;

				case '\"':	/* beginning or end of string */
					gotquote = (gotquote) ? 0 : 1;
					srcptr++;	/* just toggle */
					break;

				case '\\':	/* quote next character */
					srcptr++;	/* skip the back-slash */
					switch (*srcptr) {
						/* Octal character */
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						*destptr = '\0';
						do
							*destptr =
							    (*destptr << 3) | (*srcptr++ - '0');
						while (*srcptr >= '0' && *srcptr <= '7');
						destptr++;
						break;
						/* C escape char */
					case 'b':
						*destptr++ = '\b';
						srcptr++;
						break;
					case 'n':
						*destptr++ = '\n';
						srcptr++;
						break;
					case 'r':
						*destptr++ = '\r';
						srcptr++;
						break;
					case 't':
						*destptr++ = '\t';
						srcptr++;
						break;

						/* Boring -- just copy ASIS */
					default:
						*destptr++ = *srcptr++;
					}
					break;

				case '\0':
					*destptr = '\0';
					ind++;
					argv[ind] = (char *) 0;
					return (ind);
				}
			}
		}
	      nextarg:
		continue;
	}
}
