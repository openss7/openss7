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
    "Header: /xtel/isode/isode/others/quipu/uips/de/RCS/video.c,v 9.0 1992/06/16 12:45:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/video.c,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: video.c,v
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
#include "quipu/util.h"

#define LINES 24
#define COLS  80

static char *revVideoOn;
static char *resetVideo;
static char *cursorMotion = NULLCP;
static char *clLine = NULLCP;
static char *startLine = NULLCP;
static char *bellCode = NULLCP;
static char bp[1024];
static char buffer[1024];

extern int wanAccess;

char term[80];
int preferInvVideo = TRUE;
int inverseVideo;
int lines, cols;

char *getenv();
char *tgetstr();
char *tgoto();
char *TidyString();
char *checkSetTerm();
int output();
void turnInverseVideoOn();
void soundBell();

void
initVideo()
{
	char *cp;

	inverseVideo = FALSE;
	cp = getenv("TERM");
	if (cp == (char *) NULL)
		cp = "network";
	cp = checkSetTerm(cp, "network");
	(void) strcpy(term, cp);
}

char *
checkSetTerm(termtype, defterm)
	char *termtype, *defterm;
{
	char *cp;
	char buf[1024];
	int n, ret;

	for (;;) {
		if (lexequ(termtype, "l") == 0) {
			if (lines == 0) {	/* if no term size, set suitable defaults */
				lines = 24;
				cols = 80;
			}

			displayHelp("termtypes");
			(void) strcpy(termtype, defterm);
		} else {
			ret = tgetent(bp, termtype);
			if (ret == 1)
				break;	/* got a valid terminal type */
			(void) fprintf(stdout,
				       "\nDon't know anything about your terminal type (%s).\n",
				       termtype);
			(void) fprintf(stdout,
				       "If your terminal type is unknown to the system, pressing <CR> for\n");
			(void) fprintf(stdout,
				       "terminal type will accept a default terminal type (assumes 24 line screen).\n");
		}
		(void) fprintf(stdout,
			       "\nEnter your terminal type (or l or L to list possible terminal types): ");
		if (gets(buf) == NULLCP) {
			clearerr(stdin);
			return defterm;
		}
		termtype = TidyString(buf);
		if (strlen(termtype) == 0) {	/* default accepted */
			return defterm;
		}
	}

	cp = buffer;
	revVideoOn = tgetstr("mr", &cp);
	resetVideo = tgetstr("me", &cp);
	if (preferInvVideo == TRUE)
		turnInverseVideoOn();

	lines = tgetnum("li");
	if (lines < 1)
		lines = LINES;
	if (wanAccess == TRUE)
		if (lines > 24) {
			(void) fprintf(stdout, "\nYour terminal size is set to %d lines.\n", lines);
			(void) fprintf(stdout,
				       "If that seems OK, press <CR>, otherwise enter the correct number (24 is normal).\n");
			for (;;) {
				(void) fprintf(stdout, "Length of screen in lines: ");
				if (gets(buf) == NULLCP)
					clearerr(stdin);
				else if (strlen(buf) != 0) {
					n = atoi(buf);
					if (n > 0) {
						lines = n;
						break;
					} else
						continue;
				}
				break;	/* default screen length accepted */
			}
		}
	cols = tgetnum("co");
	if (cols == 0)
		cols = COLS;

	cursorMotion = tgetstr("cm", &cp);
	if (cursorMotion != NULLCP)
		startLine = tgoto(cursorMotion, 0, lines - 1);
	clLine = tgetstr("ce", &cp);
	bellCode = tgetstr("bl", &cp);
	return termtype;
}

void
soundBell()
{
	if (bellCode != NULLCP)
		tputs(bellCode, 1, output);
	else
		(void) putchar(7);
}

output(c)
	char c;
{
	(void) putchar(c);
}

void
turnInverseVideoOn()
{
	if ((revVideoOn == (char *) NULL) || (resetVideo == (char *) NULL))
		inverseVideo = FALSE;
	else
		inverseVideo = TRUE;
}

void
turnInverseVideoOff()
{
	inverseVideo = FALSE;
}

writeInverse(str)
	char *str;
{
	if (inverseVideo == TRUE)
		tputs(revVideoOn, 1, output);
	(void) fputs(str, stdout);
	if (inverseVideo == TRUE)
		tputs(resetVideo, 1, output);
}

clearLine()
{
	int i;

	if (startLine != NULLCP) {
		tputs(startLine, 1, output);
		if (clLine != NULLCP)
			tputs(clLine, 1, output);
		else
			for (i = 1; i < 80; i++)
				(void) putchar(' ');
		tputs(startLine, 1, output);
	} else
		(void) putchar('\n');
}
