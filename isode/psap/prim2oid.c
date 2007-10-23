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

/* prim2oid.c - presentation element to object identifier */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/prim2oid.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/prim2oid.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: prim2oid.c,v
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

static int once_only = 1;
static OIDentifier oid;

/*  */

OID
prim2oid(pe)
	register PE pe;
{
	register unsigned int i, *ip;
	register PElementData dp, ep;
	register OID o = &oid;

	if (once_only) {
		bzero((char *) o, sizeof *o);
		once_only = 0;
	}

	if (pe->pe_form != PE_FORM_PRIM || (dp = pe->pe_prim) == NULLPED || pe->pe_len == 0)
		return pe_seterr(pe, PE_ERR_PRIM, NULLOID);
	ep = dp + pe->pe_len;

	if (o->oid_elements) {
		free((char *) o->oid_elements);
		o->oid_elements = NULL;
	}

	for (i = 1; dp < ep; i++) {	/* another whacko OSI encoding... */
		if (*dp == 0x80)
			return pe_seterr(pe, PE_ERR_OID, NULLOID);

		while (*dp++ & 0x80)
			if (dp > ep)
				return pe_seterr(pe, PE_ERR_OID, NULLOID);
	}

	if ((ip = (unsigned int *) malloc((i + 1) * sizeof *ip)) == NULL)
		return pe_seterr(pe, PE_ERR_NMEM, NULLOID);
	o->oid_elements = ip, o->oid_nelem = i;

	for (dp = pe->pe_prim; dp < ep;) {
		i = 0;
		do {
			i <<= 7;
			i |= *dp & 0x7f;
		} while (*dp++ & 0x80);

		if (ip != o->oid_elements)
			*ip++ = i;
		else if (i < 40)
			*ip++ = 0, *ip++ = i;
		else if (i < 80)
			*ip++ = 1, *ip++ = i - 40;
		else
			*ip++ = 2, *ip++ = i - 80;
	}

	return o;
}

/*  */

#ifdef DEBUG
free_static_oid()
{
	if (!once_only && oid.oid_elements) {
		free((char *) oid.oid_elements);
		oid.oid_elements = NULL;
	}
}
#endif
