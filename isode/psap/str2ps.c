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

/* str2ps.c - string-backed abstraction for PStreams */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/str2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/str2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: str2ps.c,v
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

/*  */

/* ARGSUSED */

static int
str_read(ps, data, n, in_line)
	register PS ps;
	PElementData data;
	PElementLen n;
	int in_line;
{
	register int cc;

	if (ps->ps_base == NULLCP || (cc = ps->ps_cnt) <= 0)
		return 0;
	if (cc > n)
		cc = n;

	bcopy(ps->ps_ptr, (char *) data, cc);
	ps->ps_ptr += cc, ps->ps_cnt -= cc;

	return cc;
}

/* ARGSUSED */

static int
str_write(ps, data, n, in_line)
	register PS ps;
	PElementData data;
	PElementLen n;
	int in_line;
{
	register int cc;
	register char *cp;

	if (ps->ps_base == NULLCP) {
		if ((cp = malloc((unsigned) (cc = n + BUFSIZ))) == NULLCP)
			return ps_seterr(ps, PS_ERR_NMEM, NOTOK);
		ps->ps_base = ps->ps_ptr = cp;
		ps->ps_bufsiz = ps->ps_cnt = cc;
	} else if (ps->ps_cnt < n) {
		register int curlen = ps->ps_ptr - ps->ps_base;

		if (ps->ps_inline) {
			n = ps->ps_cnt;
			goto partial;
		}

		if ((cp = realloc(ps->ps_base, (unsigned) (ps->ps_bufsiz + (cc = n + BUFSIZ))))
		    == NULLCP)
			return ps_seterr(ps, PS_ERR_NMEM, NOTOK);
		ps->ps_ptr = (ps->ps_base = cp) + curlen;
		ps->ps_bufsiz += cc, ps->ps_cnt += cc;
	}
      partial:;

	bcopy((char *) data, ps->ps_ptr, n);
	ps->ps_ptr += n, ps->ps_cnt -= n;

	return n;
}

static int
str_close(ps)
	register PS ps;
{
	if (ps->ps_base && !ps->ps_inline)
		free(ps->ps_base);

	return OK;
}

/*  */

int
str_open(ps)
	register PS ps;
{
	ps->ps_readP = str_read;
	ps->ps_writeP = str_write;
	ps->ps_closeP = str_close;

	return OK;
}

int
str_setup(ps, cp, cc, in_line)
	register PS ps;
	register char *cp;
	register int cc;
	int in_line;
{
	register char *dp;

	if (in_line) {
		ps->ps_inline = 1;
		ps->ps_base = ps->ps_ptr = cp;
		ps->ps_bufsiz = ps->ps_cnt = cc;
	} else if (cc > 0) {
		if ((dp = malloc((unsigned) (cc))) == NULLCP)
			return ps_seterr(ps, PS_ERR_NMEM, NOTOK);
		ps->ps_base = ps->ps_ptr = dp;
		if (cp != NULLCP)
			bcopy(cp, dp, cc);
		ps->ps_bufsiz = ps->ps_cnt = cc;
	}

	return OK;
}
