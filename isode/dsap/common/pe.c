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

/* pe.c - General PE utility routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/pe.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/pe.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: pe.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/util.h"
#include "psap.h"
#include "quipu/attr.h"		/* for defn of READOUT */

extern LLog *log_dsap;

pe_print(ps, pe, format)
	PS ps;
	PE pe;
	int format;
{
	register char *ptr, *s;
	register int i, j;
	PS sps;
	static char hex[] = "0123456789abcdef";
	char buffer[LINESIZE];

	if (format == FILEOUT) {
		(void) pe2ps(ps, pe);
		return;
	}

	if ((format == READOUT) && (pe->pe_len >= LINESIZE)) {
		ps_print(ps, "ASN attribute too big to print here!");
		return;
	}

	if ((sps = ps_alloc(str_open)) == NULLPS)
		return;
	if (str_setup(sps, NULLCP, LINESIZE, 0) == NOTOK)
		return;

	if (format != READOUT)
		(void) ps_write(ps, (PElementData) "{ASN}", 5);

	(void) pe2ps(sps, pe);

	s = buffer;

	ptr = sps->ps_base;
	for (i = 0, j = 0; i < sps->ps_byteno; i++) {
/*
		ps_printf (sps2,fmt,*ptr++ & 255);
*/
		*s++ = hex[((*ptr & 255) / 16) % 16];
		*s++ = hex[(*ptr++ & 255) % 16];
		j += 2;
		if (j >= EDB_LINEWRAP) {
			*s++ = '\\';
			*s++ = '\n';
			(void) ps_write(ps, (PElementData) buffer, j);
			s = buffer;
			j = 0;
		}
	}
	(void) ps_write(ps, (PElementData) buffer, j);
	(void) ps_write(ps, (PElementData) "00", 2);
	ps_free(sps);

}

PE
asn2pe(str)
	char *str;
{
	char *ptr;
	char *pe_ptr;
	register int i, j;
	PS sps;
	void StripSpace();
	PE pe;
	extern char hex2nib[];

#ifdef oldcode
	int val;
#endif

	StripSpace(str);

	j = strlen(str);
	if (j % 2 == 1) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("asn2pe: not an even number of bytes `%s'", str));
		j--;
	}
	pe_ptr = (char *) smalloc(j + 10);
	ptr = pe_ptr;

	for (i = 0; i < j;) {
		if (!isascii(*str) || !isxdigit(*str)) {	/* skip bad chars */
			str++, i++;
			continue;
		}
		*ptr = hex2nib[*str++ & 0x7f] << 4;
		*ptr |= hex2nib[*str++ & 0x7f];
		*ptr++ &= 0xff;
		i += 2;
#ifdef oldcode
		(void) sscanf(str, "%2x", &val);

		*ptr++ = val & 0xff;
		str++;
		str++;
		i += 2;
#endif
	}

	if ((sps = ps_alloc(str_open)) == NULLPS)
		return (NULLPE);
	if (str_setup(sps, pe_ptr, j + 10, 0) == NOTOK)
		return (NULLPE);

	pe = ps2pe(sps);
	if (sps->ps_errno != PS_ERR_NONE) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s in ASN attribute ", ps_error(sps->ps_errno)));
		if (pe) {
			pe_free(pe);
			pe = NULLPE;
		}
	}

	free(pe_ptr);
	ps_free(sps);

	return (pe);
}
