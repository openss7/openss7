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

/* py_pp.c - generic pretty-printer */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/pepy/RCS/py_pp.c,v 9.0 1992/06/16 12:25:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/pepy/RCS/py_pp.c,v 9.0 1992/06/16 12:25:01 isode Rel
 *
 *
 * Log: py_pp.c,v
 * Revision 9.0  1992/06/16  12:25:01  isode
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

#define	ps_advise(ps, f) \
	advise (NULLCP, "%s: %s", (f), ps_error ((ps) -> ps_errno))

/*    DATA */

static char *myname = "pp";

static enum { ps2pp, pl2pp } mode = ps2pp;

void adios(), advise();

static int process();

/*  */

/* ARGSUSED */

int
PY_pp(argc, argv, envp, pfx)
	int argc;
	char **argv, **envp;
	IFP pfx;
{
	register int status = 0;
	register char *cp;
	register FILE *fp;

	if (myname = rindex(argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	for (argc--, argv++; cp = *argv; argc--, argv++)
		if (*cp == '-') {
			if (strcmp(cp + 1, "ps") == 0) {
				mode = ps2pp;
				continue;
			}
			if (strcmp(cp + 1, "pl") == 0) {
				mode = pl2pp;
				continue;
			}
			adios(NULLCP, "usage: %s [ -ps | -pl ] [ files... ]", myname);
		} else
			break;

	if (argc == 0)
		status = process("(stdin)", stdin, pfx);
	else
		while (cp = *argv++) {
			if ((fp = fopen(cp, "r")) == NULL) {
				advise(cp, "unable to read");
				status++;
				continue;
			}
			status += process(cp, fp, pfx);
			(void) fclose(fp);
		}

	return status;
}

/*  */

static int
process(file, fp, pfx)
	register char *file;
	register FILE *fp;
	IFP pfx;
{
	register PE pe;
	register PS ps;

	if ((ps = ps_alloc(std_open)) == NULLPS) {
		ps_advise(ps, "ps_alloc");
		return 1;
	}
	if (std_setup(ps, fp) == NOTOK) {
		advise(NULLCP, "%s: std_setup loses", file);
		return 1;
	}

	for (;;) {
		switch (mode) {
		case ps2pp:
			if ((pe = ps2pe(ps)) == NULLPE)
				if (ps->ps_errno) {
					ps_advise(ps, "ps2pe");
				      you_lose:;
					ps_free(ps);
					return 1;
				} else {
				      done:;
					ps_free(ps);
					return 0;
				}
			break;

		case pl2pp:
			if ((pe = pl2pe(ps)) == NULLPE)
				if (ps->ps_errno) {
					ps_advise(ps, "pl2pe");
					goto you_lose;
				} else
					goto done;
			break;
		}

		(void) (*pfx) (pe, 1, NULLIP, NULLVP, NULLCP);

		pe_free(pe);
	}
}

/*    ERRORS */

#include <varargs.h>

#ifndef	lint
void _advise();

static void
adios(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);

	_exit(1);
}
#else
/* VARARGS */

static void
adios(what, fmt)
	char *what, *fmt;
{
	adios(what, fmt);
}
#endif

#ifndef	lint
static void
advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);
}

static void
_advise(ap)
	va_list ap;
{
	char buffer[BUFSIZ];

	asprintf(buffer, ap);

	(void) fflush(stdout);

	(void) fprintf(stderr, "%s: ", myname);
	(void) fputs(buffer, stderr);
	(void) fputc('\n', stderr);

	(void) fflush(stderr);
}
#else
/* VARARGS */

static void
advise(what, fmt)
	char *what, *fmt;
{
	advise(what, fmt);
}
#endif
