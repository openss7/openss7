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

/* na2norm.c - normalize NSAPaddr */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/na2norm.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/na2norm.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: na2norm.c,v
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

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"
#include "internet.h"
#include "tailor.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */

/* Encoding based on

   	"An interim approach to use of Network Addresses",
	S.E. Kille, January 16, 1989

 */

struct NSAPaddr *
na2norm(na)
	register struct NSAPaddr *na;
{
	int ilen;
	register char *cp, *dp;
	char nsap[NASIZE * 2 + 1];
	register struct hostent *hp;
	register struct ts_interim *ts;
	static struct NSAPaddr nas;
	register struct NSAPaddr *ca = &nas;

	if (na->na_stack == NA_NSAP) {
		*ca = *na;	/* struct copy */
		return ca;
	}

	bzero((char *) ca, sizeof *ca);
	ca->na_stack = NA_NSAP;

	for (ts = ts_interim; ts->ts_name; ts++)
		if (ts->ts_subnet == na->na_community)
			break;
	if (!ts->ts_name) {
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ("unable to find community #%d", na->na_community));
		return NULLNA;
	}

	cp = nsap;
	switch (na->na_stack) {
	case NA_TCP:
		if ((hp = gethostbystring(na->na_domain)) == NULL) {
			SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
			     ("%s: unknown host", na->na_domain));
			return NULLNA;
		}
#define	s2a(b)	(((int) (b)) & 0xff)
		(void) sprintf(cp, "%03d%03d%03d%03d",
			       s2a(hp->h_addr[0]),
			       s2a(hp->h_addr[1]), s2a(hp->h_addr[2]), s2a(hp->h_addr[3]));
		cp += strlen(cp);
#undef	s2a

		if (na->na_port) {
			(void) sprintf(cp, "%05d", (int) ntohs(na->na_port));
			cp += strlen(cp);

			if (na->na_tset && na->na_tset != NA_TSET_TCP) {
				(void) sprintf(cp, "%05d", (int) na->na_tset);
				cp += strlen(cp);
			}
		}
		break;

	case NA_X25:
		if (na->na_community == SUBNET_INT_X25 && na->na_cudflen == 0 && na->na_pidlen == 0 && na->na_dte[0] != '0') {	/* SEK 
																   - 
																   X121 
																   form 
																 */
			/* should be more general */
			(void) sprintf(nsap, "36%14s", na->na_dte);
			ts = NULL;
			break;
		}

		if ((ilen = na->na_pidlen & 0xff))
			*cp++ = '1', dp = na->na_pid;
		else if ((ilen = na->na_cudflen & 0xff))
			*cp++ = '2', dp = na->na_cudf;
		else
			*cp++ = '0';
		if (ilen) {
			(void) sprintf(cp, "%01d", ilen);
			cp += strlen(cp);

			for (; ilen-- > 0; cp += 3)
				(void) sprintf(cp, "%03d", *dp++ & 0xff);
		}
		(void) strcpy(cp, na->na_dte);
		break;

	default:
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ("unknown address type 0x%x", na->na_stack));
		return NULLNA;
	}

	cp = nsap, dp = ca->na_address;
	if (ts) {
		bcopy(ts->ts_prefix, dp, ts->ts_length);
		dp += ts->ts_length;
	}
	while (*cp) {
		*dp = (*cp++ - '0') << 4;
		if (*cp)
			*dp++ |= (*cp++ - '0') & 0x0f;
		else
			*dp++ |= 0x0f;
	}
	ca->na_addrlen = dp - ca->na_address;

	return ca;
}

static inline void
dummy(void)
{
	(void) rcsid;
}
