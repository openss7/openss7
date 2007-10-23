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

/* pe2uvec.c - write a PE to a udvec */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/pe2uvec.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/pe2uvec.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: pe2uvec.c,v
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
#include "tailor.h"

/*  */

int
pe2uvec(pe, uv)
	register PE pe;
	struct udvec **uv;
{
	int cc;

#ifdef	DEBUG
	int len;
#endif
	register PS ps;

	*uv = NULL;

	if ((ps = ps_alloc(uvec_open)) == NULLPS)
		return NOTOK;
	cc = ps_get_abs(pe) - ps_get_plen(pe);
#ifdef	DEBUG
	len = ps->ps_byteno;
#endif
	if (uvec_setup(ps, cc) == NOTOK || pe2ps_aux(ps, pe, 0) == NOTOK) {
		ps_free(ps);
		return NOTOK;
	}

	*uv = ps->ps_head;
#ifdef	DEBUG
	len = ps->ps_byteno - len;
#endif

	ps->ps_head = NULL;
	ps->ps_extra = NULL;
	ps_free(ps);

#ifdef	DEBUG
	if (psap_log->ll_events & LLOG_PDUS) {
		register int i, j, k;
		register struct udvec *vv;

		i = j = k = 0;
		for (vv = *uv; vv->uv_base; vv++, i++)
			if (vv->uv_inline)
				j++, k += vv->uv_len;

		LLOG(psap_log, LLOG_PDUS,
		     ("PE written in %d elements, %d inline (%d octet%s)",
		      i, j, k, k != 1 ? "s" : ""));
		pe2text(psap_log, pe, 0, len);
	}
#endif

	return OK;
}
