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

/* pa2str.c - pretty-print PSAPaddr */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/pa2str.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/pa2str.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: pa2str.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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

#include <ctype.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

/*    Presentation Address to String */

char *
pa2str(px)
	register struct PSAPaddr *px;
{
	register char *bp;
	struct PSAPaddr pas;
	register struct PSAPaddr *pa = &pas;
	register struct TSAPaddr *ta = &pa->pa_addr.sa_addr;
	static char buffer[BUFSIZ];

	bp = buffer;

	*pa = *px;		/* struct copy */
	if (ta->ta_selectlen > 0 && ta->ta_selectlen < sizeof ta->ta_selector) {
		register char *dp, *ep;
		register struct TSAPaddr *tz;
		register int n, m;

		/* does this look like an encoded TSEL? */
		m = ta->ta_selectlen;
		n = ta->ta_selector[0];
		if (m > 4 && ta->ta_selector[0] == ta->ta_selector[1] && n > 2 && n <= m - 2) {	/* encoded! 
												 */
			tz = &px->pa_addr.sa_addr;
			bzero((char *) ta, sizeof *ta);
			if ((ta->ta_selectlen = m - n - 2) > 0)
				bcopy(&tz->ta_selector[n + 2], ta->ta_selector, ta->ta_selectlen);
			if (norm2na(&tz->ta_selector[2], n, ta->ta_addrs) != OK) {
				*pa = *px;
				goto normal;
			}
			ta->ta_naddr = 1;
			goto bridge;
		}
		for (ep = (dp = ta->ta_selector) + ta->ta_selectlen, *ep = NULL; dp < ep; dp++)
			if (!isprint((u_char) *dp) && *dp != ' ')
				break;
		if (dp >= ep && (tz = str2taddr(ta->ta_selector))) {
			pa->pa_addr.sa_addr = *tz;	/* struct copy */
		      bridge:
			(void) sprintf(bp, "%s through TS bridge at ", paddr2str(pa, NULLNA));
			bp += strlen(bp);

			bzero((char *) pa, sizeof *pa);
			*ta = px->pa_addr.sa_addr;	/* struct copy */
			ta->ta_selectlen = 0;
		}
	}
      normal:
	(void) strcpy(bp, paddr2str(pa, NULLNA));

	return buffer;
}
