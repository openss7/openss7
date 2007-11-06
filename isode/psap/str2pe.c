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

/* str2pe.c - create an Inline CONStructor PElement */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/str2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/str2pe.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: str2pe.c,v
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

static PElementLen str_get_len();
static int str_get_start();

/*  */

#define	seterr(e,v)		(*result = (e), (v))

PE
str2pe(s, len, advance, result)
	char *s;
	int len, *advance, *result;
{
#ifdef	DEBUG
	int n = len;
#endif
	char *sp;
	PElementClass class;
	PElementForm form;
	PElementID id;
	PElementLen plen;
	register PE pe;

	*result = PS_ERR_NONE;

	sp = s;
	if (str_get_start(&sp, &len, &class, &form, &id, &plen, result) == NOTOK)
		return NULLPE;

	if (form == PE_FORM_CONS)
		form = PE_FORM_ICONS;

	if (plen == PE_LEN_INDF && (plen = str_get_len(sp, len, result)) == PE_LEN_INDF)
		return NULLPE;

	if ((pe = pe_alloc(class, form, id)) == NULLPE)
		return seterr(PS_ERR_NMEM, NULLPE);

	pe->pe_ilen = sp - s;
	if (form == PE_FORM_ICONS) {
		pe->pe_len = pe->pe_ilen + plen;
		pe->pe_prim = (PElementData) s;
	} else if (pe->pe_len = plen)
		pe->pe_prim = (PElementData) sp;

	pe->pe_inline = 1;

	if (advance)
		*advance = pe->pe_ilen + plen;

#ifdef	DEBUG
	if (psap_log->ll_events & LLOG_PDUS)
		pe2text(psap_log, pe, 1, n);
#endif

	return pe;
}

/*  */

static int
str_get_start(sp, n, class, form, id, plen, result)
	char **sp;
	int *n, *result;
	PElementClass *class;
	PElementForm *form;
	PElementID *id;
	PElementLen *plen;
{
	register int i, len;
	register char *s;
	byte c, d;
	register PElementID jd;
	register PElementLen qlen;

	s = *sp, len = *n;
	if (len-- <= 0)
		return seterr(PS_ERR_EOF, NOTOK);
	c = *s++;

	*class = ((int) (c & PE_CLASS_MASK)) >> PE_CLASS_SHIFT;
	*form = ((int) (c & PE_FORM_MASK)) >> PE_FORM_SHIFT;
	if ((jd = (c & PE_CODE_MASK)) == PE_ID_XTND)
		for (jd = 0;; jd <<= PE_ID_SHIFT) {
			if (len-- <= 0)
				return seterr(PS_ERR_EOFID, NOTOK);
			d = *s++;

			jd |= d & PE_ID_MASK;
			if (!(d & PE_ID_MORE))
				break;
			if (jd & (PE_ID_MASK << (PE_ID_SHIFT - 1)))
				return seterr(PS_ERR_OVERID, NOTOK);
		}
	*id = jd;
#ifdef	DEBUG
	SLOG(psap_log, LLOG_DEBUG, NULLCP, ("class=%d form=%d id=%d", *class, *form, *id));
#endif

	if (len-- <= 0)
		return seterr(PS_ERR_EOFLEN, NOTOK);
	c = *s++;

	if ((i = c) & PE_LEN_XTND) {
		if ((i &= PE_LEN_MASK) > sizeof(PElementLen))
			return seterr(PS_ERR_OVERLEN, NOTOK);

		if (i) {
			for (qlen = 0; i-- > 0;) {
				if (len-- <= 0)
					return seterr(PS_ERR_EOFLEN, NOTOK);
				c = *s++;

				qlen = (qlen << 8) | (c & 0xff);
			}

			*plen = qlen;
		} else if (*form == PE_FORM_PRIM)
			return seterr(PS_ERR_INDF, NOTOK);
		else
			*plen = PE_LEN_INDF;
	} else
		*plen = i;
#ifdef	DEBUG
	SLOG(psap_log, LLOG_DEBUG, NULLCP, ("len=%d", *plen));
#endif

	*sp = s, *n = len;

	return OK;
}

/*  */

static PElementLen
str_get_len(s, len, result)
	char *s;
	int len, *result;
{
	char *sp;
	PElementClass class;
	PElementForm form;
	PElementID id;
	PElementLen plen;

	for (sp = s;;) {
		if (str_get_start(&sp, &len, &class, &form, &id, &plen, result)
		    == NOTOK)
			return PE_LEN_INDF;

		if (class == PE_CLASS_UNIV && id == PE_UNIV_EOC)
			return ((PElementLen) (sp - s));

		if (plen == PE_LEN_INDF && (plen = str_get_len(sp, len, result)) == PE_LEN_INDF)
			return PE_LEN_INDF;

		sp += plen, len -= plen;
	}
}
