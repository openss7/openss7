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

/* psaptest.c - test out -lpsap */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/psaptest.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/psaptest.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: psaptest.c,v
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

#include <stdio.h>
#include "psap.h"
#include "tailor.h"
#include <sys/stat.h>

/*  */

static enum {
	ps2pl, ps2ps, pl2pl, pl2ps
} mode;

static ps_die();

/*  */

/* ARGSUSED */

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	char *cp;
	register PE pe;
	register PS ps;

	argc--, argv++;
	if (argc != 1) {
	      usage:;
		(void) fprintf(stderr, "usage: psaptest [ps2pl | ps2ps | pl2pl | pl2ps]\n");
		exit(1);
	}

	if (strcmp(*argv, "version") == 0) {
		(void) printf("%s\n", psapversion);
		exit(0);
	}

	if ((cp = getenv("PSAPTEST")) && *cp) {
		psap_log->ll_events = atoi(cp);
		psap_log->ll_stat |= LLOGTTY;
	}

	if (strcmp(*argv, "string") == 0) {
		int onceonly = 1;
		register PE *pep;

		if ((pe = pe_alloc(PE_CLASS_UNIV, PE_FORM_CONS, PE_PRIM_OCTS))
		    == NULLPE) {
		      no_pe:;
			(void) fprintf(stderr, "pe_alloc: you lose\n");
			exit(1);
		}
		pep = &pe->pe_cons;
		for (;;) {
			int i;
			register PE p;

			if (onceonly) {
				struct stat st;

				if (fstat(fileno(stdin), &st) == NOTOK
				    || (st.st_mode & S_IFMT) != S_IFREG || (i = st.st_size) <= 0)
					i = BUFSIZ;

				onceonly = 0;
			} else
				i = BUFSIZ;

			if ((p = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_OCTS))
			    == NULL || (p->pe_prim = PEDalloc(p->pe_len = i)) == NULLPED)
				goto no_pe;

			switch (i = fread((char *) p->pe_prim, sizeof *p->pe_prim,
					  (int) p->pe_len, stdin)) {
			case NOTOK:
				perror("fread");
				exit(1);

			case OK:
				pe_free(p);
				mode = ps2ps;
				goto doit;

			default:
				p->pe_len = i;
				*pep = p, pep = &p->pe_next;
				break;
			}
		}
	}

	if (strcmp(*argv, "binary") == 0) {
		int i;
		char buffer[BUFSIZ], packet[BUFSIZ];

		while (fgets(buffer, sizeof buffer, stdin)) {
			if (*buffer == ' ')
				(void) strncpy(packet, buffer + 1, i = strlen(buffer) - 2);
			else
				i = implode((u_char *) packet, buffer, strlen(buffer) - 1);
			(void) fwrite(packet, sizeof *packet, i, stdout);
		}

		exit(0);
	}

	if (strcmp(*argv, "ps2pl") == 0)
		mode = ps2pl;
	else if (strcmp(*argv, "ps2ps") == 0)
		mode = ps2ps;
	else if (strcmp(*argv, "pl2pl") == 0)
		mode = pl2pl;
	else if (strcmp(*argv, "pl2ps") == 0)
		mode = pl2ps;
	else
		goto usage;

	for (;;) {
		if ((ps = ps_alloc(std_open)) == NULLPS) {
			(void) fprintf(stderr, "ps_alloc(stdin): you lose\n");
			exit(1);
		}
		if (std_setup(ps, stdin) == NOTOK)
			ps_die(ps, "std_setup(stdin)");

		switch (mode) {
		case ps2pl:
		case ps2ps:
			if ((pe = ps2pe(ps)) == NULLPE)
				if (ps->ps_errno)
					ps_die(ps, "ps2pe");
				else
					exit(0);
			break;

		case pl2ps:
		case pl2pl:
			if ((pe = pl2pe(ps)) == NULLPE)
				if (ps->ps_errno)
					ps_die(ps, "pl2pe");
				else
					exit(0);
			break;
		}

		ps_free(ps);

	      doit:;
		if ((ps = ps_alloc(std_open)) == NULLPS) {
			(void) fprintf(stderr, "ps_alloc(stdout): you lose\n");
			exit(1);
		}
		if (std_setup(ps, stdout) == NOTOK)
			ps_die(ps, "std_setup(stdout)");

		switch (mode) {
		case ps2ps:
		case pl2ps:
			if (pe2ps(ps, pe) == NOTOK)
				ps_die(ps, "pe2ps");
			break;

		case pl2pl:
		case ps2pl:
			if (pe2pl(ps, pe) == NOTOK)
				ps_die(ps, "pe2pl");
			break;
		}

		pe_free(pe);
		ps_free(ps);
	}
}

/*    ERRORS */

static
ps_die(ps, s)
	register PS ps;
	register char *s;
{
	(void) fprintf(stderr, "%s: %s\n", s, ps_error(ps->ps_errno));
	exit(1);
}
