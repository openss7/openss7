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

/* template.c - your comments here */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/de/RCS/pager.c,v 9.0 1992/06/16 12:45:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/pager.c,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: pager.c,v
 * Revision 9.0  1992/06/16  12:45:59  isode
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

#include <stdio.h>
#include <varargs.h>
#include <sys/types.h>
#include <termios.h>
#include "quipu/util.h"
#include "demanifest.h"

extern int lines, cols;

static int redisplay = FALSE;
static int numOK;
static int pagerNumber = -1;
static int wrapLines = FALSE;

int lineno;
int discardInput;

testRedisplay()
{
	if (redisplay)
		return TRUE;
	else
		return FALSE;
}

setRedisplay()
{
	redisplay = TRUE;
}

pagerOn(number)
	int number;
{
	lineno = 0;
	discardInput = FALSE;
	pagerNumber = -1;
	if (number == NUMBER_ALLOWED)
		numOK = TRUE;
	else
		numOK = FALSE;
}

linewrapOn()
{
	wrapLines = TRUE;
}

linewrapOff()
{
	wrapLines = FALSE;
}

isWrapOn()
{
	return wrapLines;
}

resetprint(va_alist)
	va_dcl
{
	va_list ap;
	char buf[BUFSIZ];

	va_start(ap);
	_asprintf(buf, NULLCP, ap);
	(void) fputs(buf, stdout);
	pagerOn(numOK);
	redisplay = TRUE;
	va_end(ap);
}

pageprint(va_alist)
	va_dcl
{				/* no ; */
	va_list ap;
	char buf[BUFSIZ];
	int i, c;
	static int charsInLine = 0;

	va_start(ap);
	redisplay = FALSE;
	if (discardInput == TRUE)
		return;
	_asprintf(buf, NULLCP, ap);
	for (i = 0; buf[i] != '\0'; i++) {
		if (buf[i] == '\n') {
			charsInLine = 0;
			(void) putchar(buf[i]);
			lineno++;
			if (lineno >= lines - 1) {
				putPagePrompt();
				c = getPagerInput();
				switch (c) {
				case '\n':
					lineno--;
					break;
				case ' ':
					lineno = 1;
					break;
				default:
					break;
				}
				if ((pagerNumber == -1) && ((c == VEOF) || (c == VKILL)))
					(void) kill(getpid(), 2);	/* let the signal handling
									   sort it out */
				if (((pagerNumber == -1) && (c == 'q')) || (pagerNumber != -1)) {
					discardInput = TRUE;
					break;
				}
			}
		} else
			/* optionally discard chars that would cause line wrapping */
		if (isWrapOn() || (charsInLine < (cols - 1))) {
			(void) putchar(buf[i]);
			charsInLine++;
			if (isWrapOn()) {
				if (charsInLine == cols) {
					charsInLine = 0;
					lineno++;
				}
			}
		}
	}
	va_end(ap);
}

putPagePrompt()
{
	writeInverse("SPACE for next screen; q to quit pager");
	if (numOK == TRUE)
		writeInverse("; or the number of the entry");
	writeInverse(": ");
}

int
getPagerInput()
{
	int c, i;
	char numstr[LINESIZE];

	pagerNumber = -1;
	setRawMode();
	c = 'Z';
	while (!((c == 'q') || (c == ' ') || (c == '\n')
		 || (c == VKILL) || (c == VEOF))) {
		if ((numOK == TRUE) && isdigit(c)) {
			for (i = 0; isdigit(c); i++) {
				(void) putchar(c);
				numstr[i] = c;
				c = getchar();
			}
			if (c == '\n') {
				numstr[i] = '\0';
				c = pagerNumber = atoi(numstr);
				break;
			} else {
				clearLine();
				putPagePrompt();
				c = getchar();
				continue;
			}
		} else
			c = getchar();
	}
	unsetRawMode();
	clearLine();
	return c;
}

int
getpnum()
{
	return pagerNumber;
}
