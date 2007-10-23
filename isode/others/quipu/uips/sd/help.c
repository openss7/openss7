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

/* help.c - Display of assorted help texts */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/help.c,v 9.0 1992/06/16 12:45:08 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/help.c,v 9.0 1992/06/16 12:45:08 isode Rel
 */

#include "sequence.h"
#include <stdio.h>
#include "quipu/util.h"
#include "tailor.h"

#define BUFLEN 1024
#define TEXT 1

#ifdef lint
#define ETCDIR "/etc"
#endif

#define  HELPDIR "sd/helpdir/"

extern str_seq textseq;
extern str_seq curr_dnseq;
extern int text_state;
extern int current_entry, entry_number, display_entry;

void tprint(), scrollbar();

void get_help();

void
help_cncs()
{
	get_help("help", "   Press <KEY> to get detailed help.\n");
}

void
help_init()
{
	get_help("help", "   SD X.500 Directory Agent - Concise Help\n");
}

void
help_up()
{
	get_help("widen", "   The \"Widen Area\" Function.\n");
}

void
help_back()
{
	get_help("look", "   The \"Look Back\" Function.\n");
}

void
help_number()
{
	get_help("number", "   The \"Go To Number\" Function.\n");
}

void
help_srch()
{
	get_help("search", "   The \"Search\" Function");
}

void
help_list()
{
	get_help("list", "   The \"List\" Function");
}

void
get_help(filename, line)
	char *filename, *line;
{
	FILE *helpfp;
	char helpbuf[BUFLEN];
	char filebuf[BUFLEN];
	char *str;

	text_state = TEXT;
	entry_number = 0;
	display_entry = current_entry = 1;
	free_seq(curr_dnseq);
	free_seq(textseq);
	curr_dnseq = 0;
	textseq = 0;

	(void) strcpy(filebuf, HELPDIR);
	(void) strcat(filebuf, filename);
	(void) strcpy(helpbuf, isodefile(filebuf, 0));

	if ((helpfp = fopen(helpbuf, "r")) == (FILE *) NULL) {
		tprint("Can't open help file '%s'.\n", helpbuf);
		return;
	}

	if (line) {
		add_seq(&textseq, line);
		entry_number++;
	}

	while (fgets(filebuf, BUFLEN, helpfp) != (char *) NULL) {
		str = filebuf;
		while (*str != '\n' && *str != '\0')
			str++;
		*str = '\0';
		add_seq(&textseq, filebuf);
		entry_number++;
	}

	(void) fclose(helpfp);
	scrollbar('\0');
}
