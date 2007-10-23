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

/* main.c - widget */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/main.c,v 9.0 1992/06/16 12:45:08 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/main.c,v 9.0 1992/06/16 12:45:08 isode Rel
 */

#include <signal.h>
#include <stdio.h>
#define IP _IP
#include <curses.h>
#undef OK
#include <varargs.h>

#include "widget.h"
#include "quipu/util.h"

extern char goto_path[], namestr[], passwd[];
extern char *myname;
extern WINDOW *Text;
extern WIDGET mainwdgts[];
extern WIDGET cnnctwdgts[];
extern int print_parse_errors;
extern char *oidtable, *tailfile, *myname;
extern char testing;

void quit(), die(), setsignals(), int_quit(), sd_quit(), read_args();
void user_tailor(), main_bind(), cnnct_bind(), interact(), help_init();

void exit();

main(argc, argv)
	unsigned int argc;
	char *argv[];
{
	print_parse_errors = FALSE;
	quipu_syntaxes();

#ifdef USE_PP
	pp_quipu_init(argv[0]);
#endif

	namestr[0] = '\0';
	passwd[0] = '\0';

	read_args(argc, &argv);
	dsap_init((int *) 0, &argv);

#ifdef USE_PP
	pp_quipu_run();
#endif

	initwidgets();
	setsignals();

	user_tailor();
	main_bind();
	cnnct_bind();
	help_init();
	interact();
	return (0);
}

void
read_args(argc, avptr)
	unsigned int argc;
	char ***avptr;
{
	register char **av;
	register char *cp;

	if (argc <= 1)
		return;

	av = *avptr;
	av++;

	while ((cp = *av) && (*cp == '-')) {
		switch (*++cp) {
		case 'u':
			if (*++av != NULLCP)
				(void) strcpy(namestr, *av);
			break;
		case 'p':
			if (*++av != NULLCP)
				(void) strcpy(passwd, *av);
			break;
		case 'T':
			if (*++av != NULLCP)
				oidtable = *av;
			break;
		case 'c':
			if (*++av != NULLCP)
				myname = *av;
			break;
		case 't':
			if (lexequ(*av, "-test") != 0) {
				if (*++av != NULLCP)
					tailfile = *av;
			} else {
				testing = TRUE;
			}
			break;
		}
		av++;
	}
}

void
setsignals()
{
	int i;

	for (i = 0; i < 18; i++)
		(void) signal(i, SIG_DFL);
}

void
eprint(str)
	char *str;
{
	tprint(str);
}

void
sd_quit()
{
	quit("\n", 0);
}

void
quit(error, sig)
	char *error;
	int sig;
{
	endwidgets();
	(void) ds_unbind();
	hide_picture();
	(void) printf(error);
	exit(sig);
}

void
int_quit(sig)
	int sig;
{
	quit("\n", sig);
}

advise(va_alist)
	va_dcl
{
	int code;
	va_list ap;
	extern LLog *log_dsap;

	va_start(ap);

	code = va_arg(ap, int);

	(void) _ll_log(log_dsap, code, ap);

	va_end(ap);
}
