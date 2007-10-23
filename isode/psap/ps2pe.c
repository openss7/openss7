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

/* ps2pe.c - presentation stream to presentation element */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/ps2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/ps2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: ps2pe.c,v
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

PE
ps2pe_aux(ps, top, all)
	register PS ps;
	int top, all;
{
	register PElementLen len;
	PElementClass class;
	PElementForm form;
	PElementID id;
	register PE pe;
	PE rpe = NULLPE;		/* for lint */

	if (top && ps_prime(ps, 0) == NOTOK)
		return NULLPE;

	if (ps_read_id(ps, top, &class, &form, &id) == NOTOK)
		return NULLPE;
	if ((pe = pe_alloc(class, form, id)) == NULLPE)
		return ps_seterr(ps, PS_ERR_NMEM, NULLPE);
	if (ps_read_len(ps, &pe->pe_len) == NOTOK)
		goto you_lose;

	if (all == 0)
		return pe;
	len = pe->pe_len;
	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if (len == PE_LEN_INDF) {
			rpe = ps_seterr(ps, PS_ERR_INDF, rpe);
			goto you_lose;
		}
		if (len > 0) {
			if (ps->ps_inline) {	/* "ultra-efficiency"... */
				if (ps->ps_base == NULLCP || ps->ps_cnt < len) {
					rpe = ps_seterr(ps, PS_ERR_EOF, rpe);
					goto you_lose;
				}
				pe->pe_inline = 1;
				pe->pe_prim = (PElementData) ps->ps_ptr;
				ps->ps_ptr += len, ps->ps_cnt -= len;
				ps->ps_byteno += len;
			} else {
				if ((pe->pe_prim = PEDalloc(len)) == NULLPED) {
					pe = ps_seterr(ps, PS_ERR_NMEM, rpe);
					goto you_lose;
				}
				if (ps_read(ps, pe->pe_prim, len) == NOTOK) {
#ifdef	DEBUG
					SLOG(psap_log, LLOG_DEBUG, NULLCP,
					     ("error reading primitive, %d bytes: %s",
					      len, ps_error(ps->ps_errno)));
#endif
					goto you_lose;
				}
			}
		}
		break;

	case PE_FORM_CONS:
		if (len != 0 && ps_read_cons(ps, &pe->pe_cons, len) == NOTOK)
			goto you_lose;
		break;
	}

	if (top && ps_prime(ps, -1) == NOTOK)
		goto you_lose;

	return pe;

      you_lose:;
#ifdef	DEBUG
	if (psap_log->ll_events & LLOG_DEBUG) {
		LLOG(psap_log, LLOG_PDUS, ("PE read thus far"));
		pe2text(psap_log, pe, 1, NOTOK);
	}
#endif

	pe_free(pe);
	return NULLPE;
}

/*  */

static int pe_id_overshift = PE_ID_MASK << (PE_ID_BITS - PE_ID_SHIFT);

int
ps_read_id(ps, top, class, form, id)
	register PS ps;
	int top;
	register PElementClass *class;
	register PElementForm *form;
	register PElementID *id;
{
	byte c, d;
	register PElementID j;

	if (ps_read(ps, &c, 1) == NOTOK) {
		if (top && ps->ps_errno == PS_ERR_EOF)
			ps->ps_errno = PS_ERR_NONE;
		else {
#ifdef	DEBUG
			SLOG(psap_log, LLOG_DEBUG, NULLCP,
			     ("error reading initial octet: %s", ps_error(ps->ps_errno)));
#endif
		}

		return NOTOK;
	}

	*class = ((int) (c & PE_CLASS_MASK)) >> PE_CLASS_SHIFT;
	*form = ((int) (c & PE_FORM_MASK)) >> PE_FORM_SHIFT;
	j = (c & PE_CODE_MASK);

	if (j == PE_ID_XTND)
		for (j = 0;; j <<= PE_ID_SHIFT) {
			if (ps_read(ps, &d, 1) == NOTOK) {
				if (ps->ps_errno == PS_ERR_EOF)
					ps->ps_errno = PS_ERR_EOFID;
				return NOTOK;
			}

			j |= d & PE_ID_MASK;
			if (!(d & PE_ID_MORE))
				break;
			if (j & pe_id_overshift)
				return ps_seterr(ps, PS_ERR_OVERID, NOTOK);
		}
	*id = j;
	DLOG(psap_log, LLOG_DEBUG, ("class=%d form=%d id=%d", *class, *form, *id));

	return OK;
}

/*  */

int
ps_read_len(ps, len)
	register PS ps;
	register PElementLen *len;
{
	register int i;
	register PElementLen j;
	byte c;

	if (ps_read(ps, &c, 1) == NOTOK) {
#ifdef	DEBUG
		SLOG(psap_log, LLOG_DEBUG, NULLCP,
		     ("error reading initial length octet: %s", ps_error(ps->ps_errno)));
#endif

		return NOTOK;
	}

	if ((i = c) & PE_LEN_XTND) {
		if ((i &= PE_LEN_MASK) > sizeof(PElementLen))
			return ps_seterr(ps, PS_ERR_OVERLEN, NOTOK);

		if (i) {
			for (j = 0; i-- > 0;) {
				if (ps_read(ps, &c, 1) == NOTOK) {
					if (ps->ps_errno == PS_ERR_EOF)
						ps->ps_errno = PS_ERR_EOFLEN;
					return NOTOK;
				}

				j = (j << 8) | (c & 0xff);
			}
			*len = j;
		} else
			*len = PE_LEN_INDF;
	} else
		*len = i;
#ifdef	DEBUG
	SLOG(psap_log, LLOG_DEBUG, NULLCP, ("len=%d", *len));
#endif

	return OK;
}

/*  */

int
ps_read_cons(ps, pe, len)
	register PS ps;
	register PE *pe;
	register PElementLen len;
{
	register int cc;
	register PE p, q;

	cc = ps->ps_byteno + len;

	if ((p = ps2pe_aux(ps, 0, 1)) == NULLPE) {
	      no_cons:;
#ifdef	DEBUG
		if (len == PE_LEN_INDF)
			LLOG(psap_log, LLOG_DEBUG,
			     ("error building indefinite constructor, %s", ps_error(ps->ps_errno)));
		else
			LLOG(psap_log, LLOG_DEBUG,
			     ("error building constructor, stream at %d, wanted %d: %s",
			      ps->ps_byteno, cc, ps_error(ps->ps_errno)));
#endif

		return NOTOK;
	}
	*pe = p;

	if (len == PE_LEN_INDF) {
		if (p->pe_class == PE_CLASS_UNIV && p->pe_id == PE_UNIV_EOC) {
			pe_free(p);
			*pe = NULLPE;
			return OK;
		}
		for (q = p; p = ps2pe_aux(ps, 0, 1); q = q->pe_next = p) {
			if (p->pe_class == PE_CLASS_UNIV && p->pe_id == PE_UNIV_EOC) {
				pe_free(p);
				return OK;
			}
		}

		goto no_cons;
	}

	for (q = p;; q = q->pe_next = p) {
		if (cc < ps->ps_byteno)
			return ps_seterr(ps, PS_ERR_LEN, NOTOK);
		if (cc == ps->ps_byteno)
			return OK;
		if ((p = ps2pe_aux(ps, 0, 1)) == NULLPE)
			goto no_cons;
	}
}
