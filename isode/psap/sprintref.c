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

/* sprintref.c - manage encoded session addresses */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/sprintref.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/sprintref.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: sprintref.c,v
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
#include "ssap.h"

/*  */

char *
sprintref(sr)
	struct SSAPref *sr;
{
	register char *cp;
	static char buffer[BUFSIZ];

	cp = buffer;
	*cp++ = '<';

	if (sr->sr_ulen) {
		if (sr->sr_ulen > 1 && *(sr->sr_udata + 1) + 2 == sr->sr_ulen)
			(void) sprintf(cp, "%*.*s", sr->sr_ulen - 2, sr->sr_ulen - 2,
				       sr->sr_udata + 2);
		else
			(void) sprintf(cp, "%*.*s", sr->sr_ulen, sr->sr_ulen, sr->sr_udata);
		cp += strlen(cp);
	}
	*cp++ = ',';

	if (sr->sr_clen) {
		if (sr->sr_clen > 1 && *(sr->sr_cdata + 1) + 2 == sr->sr_clen)
			(void) sprintf(cp, "%*.*s", sr->sr_clen - 2, sr->sr_clen - 2,
				       sr->sr_cdata + 2);
		else
			(void) sprintf(cp, "%*.*s", sr->sr_clen, sr->sr_clen, sr->sr_cdata);
		cp += strlen(cp);
	}
	*cp++ = ',';

	if (sr->sr_alen) {
		if (sr->sr_alen > 1 && *(sr->sr_adata + 1) + 2 == sr->sr_alen)
			(void) sprintf(cp, "%*.*s", sr->sr_alen - 2, sr->sr_alen - 2,
				       sr->sr_adata + 2);
		else
			(void) sprintf(cp, "%*.*s", sr->sr_alen, sr->sr_alen, sr->sr_adata);
		cp += strlen(cp);
	}
	*cp++ = ',';

	if (sr->sr_vlen) {
		if (sr->sr_vlen > 1 && *(sr->sr_vdata + 1) + 2 == sr->sr_vlen)
			(void) sprintf(cp, "%*.*s", sr->sr_vlen - 2, sr->sr_vlen - 2,
				       sr->sr_vdata + 2);
		else
			(void) sprintf(cp, "%*.*s", sr->sr_vlen, sr->sr_vlen, sr->sr_vdata);
		cp += strlen(cp);
	}
	*cp++ = '>';

	*cp = NULL;

	return buffer;
}
