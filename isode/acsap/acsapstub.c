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

/* acsapstub.c - application entity titles -- stub directory service */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapstub.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapstub.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 * RFC1085 (LPP) support contributed by the Wollongong Group, Inc.
 *
 *
 * Log: acsapstub.c,v
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#include "psap.h"
#include "isoaddrs.h"
#include "internet.h"
#include "tailor.h"

#define	fetch_aet	str2aet_seq

/*    DATA */

static struct isoentity ies;

/*  */

AEI
str2aei_stub(designator, qualifier)
	char *designator, *qualifier;
{
#ifdef	LPP
	register int n;
	int tcp_port, udp_port;
	register struct servent *sp;
#endif
	register char *cp;
	register struct NSAPaddr *na;
	register struct isoentity *ie = &ies;

	if (fetch_aet(designator, qualifier, &ies) == NOTOK) {
#ifdef	LPP
		if (lpp_aet(designator, qualifier, &ies) != NOTOK)
			goto found_it;
#endif

		return NULLAEI;
	}

	if (ie->ie_addr.pa_addr.sa_addr.ta_naddr <= 0) {
		register int i;
		register struct TSAPaddr *ta = &ie->ie_addr.pa_addr.sa_addr;
		struct PSAPaddr *pa;

		if ((pa = str2paddr(designator)) != NULLPA) {
			if (pa->pa_addr.sa_addr.ta_selectlen > 0)	/* full addr or just nsap? */
				ie->ie_addr = *pa;	/* struct copy */
			else {
				for (i = 0; i < pa->pa_addr.sa_addr.ta_naddr; i++)
					ta->ta_addrs[i] = pa->pa_addr.sa_addr.ta_addrs[i];
				ta->ta_naddr = pa->pa_addr.sa_addr.ta_naddr;
			}
			goto found_it;
		}
		na = ta->ta_addrs, ta->ta_naddr = 1;
		bzero((char *) na, sizeof *na);

		/* guess at what sort of address we have on our hands */

		if ((i = strlen(cp = designator)) > 6
		    && cp[0] == '4' && cp[2] == '0' && cp[3] == '0') {
			na->na_stack = NA_NSAP;
			na->na_community = ts_comm_nsap_default;
			na->na_addrlen = implode((u_char *) na->na_address, cp, strlen(cp));
			goto found_it;
		}

		if (i <= NSAP_DTELEN + 1)
			for (; *cp; cp++)
				if (!isxdigit((u_char) *cp))
					break;
		if (*cp) {
			register struct hostent *hp = gethostbystring(designator);

			if (index(cp, '@') || index(cp, '=') || index(cp, ','))
				return NULLAEI;

			na->na_stack = NA_TCP;
			na->na_community = ts_comm_tcp_default;
			if (hp) {
				struct in_addr h_in_addr;

#ifdef	h_addr
				register char **ap;
#endif

				(void) bcopy((char *) hp->h_addr, (char *) &h_in_addr,
					     sizeof(struct in_addr));
				(void) strcpy(na->na_domain, inet_ntoa(h_in_addr));
#ifdef	h_addr
				for (ap = hp->h_addr_list + 1; *ap; ap++) {
					ta->ta_naddr++, na++;

					if (ta->ta_naddr >= NTADDR)
						break;

					bzero((char *) na, sizeof *na);
					na->na_stack = NA_TCP;
					na->na_community = ts_comm_tcp_default;
					(void) strcpy(na->na_domain,
						      inet_ntoa(*(struct in_addr *) *ap));
				}
#endif
			} else
				/* hostname not in /etc/hosts or NIS */
				return (NULLAEI);
		} else {
			(void) strcpy(na->na_dte, designator);
			na->na_dtelen = strlen(na->na_dte);
#ifdef	BRIDGE_X25
			na->na_stack = bridgediscrim(na) ? NA_BRG : NA_X25;
#else
			na->na_stack = NA_X25;
#endif
			na->na_community = ts_comm_x25_default;
		}
	}
#ifdef	LPP
	for (na = ie->ie_addr.pa_addr.sa_addr.ta_addrs,
	     n = ie->ie_addr.pa_addr.sa_addr.ta_naddr; n > 0; na++, n--)
		if (na->na_stack == NA_TCP && na->na_port == 0) {
			cp = strncmp(qualifier, "isode ", 6) ? qualifier : qualifier + 6;
			if (sp = getservbyname(cp, "lpp"))
				tcp_port = udp_port = sp->s_port;
			else {
				tcp_port = (sp = getservbyname(cp, "tcp")) ? sp->s_port : 0;
				udp_port = (sp = getservbyname(cp, "udp")) ? sp->s_port : 0;
			}
			break;
		}

	for (na = ie->ie_addr.pa_addr.sa_addr.ta_addrs,
	     n = ie->ie_addr.pa_addr.sa_addr.ta_naddr; n > 0; na++, n--)
		if (na->na_stack == NA_TCP && na->na_port == 0) {
			register struct TSAPaddr *ta;

			if (tcp_port) {
				na->na_port = (u_short) tcp_port;
				na->na_tset = NA_TSET_TCP;

				if (udp_port
				    && (ta = &ie->ie_addr.pa_addr.sa_addr)->ta_naddr < NTADDR) {
					register struct NSAPaddr *ua =
					    ta->ta_addrs + ta->ta_naddr++;

					bzero((char *) ua, sizeof *ua);
					ua->na_stack = NA_TCP;
					ua->na_community = ts_comm_tcp_default;
					(void) strncpy(ua->na_domain, na->na_domain,
						       sizeof ua->na_domain);
					ua->na_port = (u_short) udp_port;
					ua->na_tset = NA_TSET_UDP;
				}
			} else if (udp_port) {
				na->na_port = (u_short) udp_port;
				na->na_tset = NA_TSET_UDP;
			}
		}
#endif

      found_it:;
	if (addr_log->ll_events & LLOG_DEBUG)
		_printent(ie);

	return oid2aei(&ie->ie_identifier);
}

/*  */

#ifdef	LPP
static unsigned int elements[NELEM + 1];
static char objent[BUFSIZ];

static int
lpp_aet(designator, qualifier, ie)
	char *designator, *qualifier;
	register struct isoentity *ie;
{
	register int nelem;
	int tcp_port, udp_port;
	char *cp;
	register struct servent *sp;
	register OID oid = &ie->ie_identifier;
	register struct PSAPaddr *pa = &ie->ie_addr;
	register struct TSAPaddr *ta = &pa->pa_addr.sa_addr;
	register struct NSAPaddr *na;

	cp = strncmp(qualifier, "isode ", 6) ? qualifier : qualifier + 6;
	if (sp = getservbyname(cp, "lpp"))
		tcp_port = udp_port = sp->s_port;
	else if ((tcp_port = (sp = getservbyname(cp, "tcp")) ? sp->s_port : 0)
		 == 0 && (udp_port = (sp = getservbyname(cp, "udp")) ? sp->s_port : 0) == 0)
		return NOTOK;

	nelem = 0;
	elements[nelem++] = 1, elements[nelem++] = 17, elements[nelem++] = 4,
	    elements[nelem++] = 0, elements[nelem++] = 0;

	oid->oid_elements = elements;
	oid->oid_nelem = nelem;
	(void) sprintf(ie->ie_descriptor = objent, "%s-%s", designator, qualifier);

	(void) bzero((char *) pa, sizeof *pa);
	na = ta->ta_addrs;
	if (tcp_port) {
		na->na_stack = NA_TCP;
		na->na_community = ts_comm_tcp_default;
		(void) strncpy(na->na_domain, designator, sizeof na->na_domain);
		na->na_port = (u_short) tcp_port;
		na->na_tset = NA_TSET_TCP;
		na++;
	}
	if (udp_port) {
		na->na_stack = NA_TCP;
		na->na_community = ts_comm_tcp_default;
		(void) strncpy(na->na_domain, designator, sizeof na->na_domain);
		na->na_port = (u_short) udp_port;
		na->na_tset = NA_TSET_UDP;
		na++;
	}
	ta->ta_naddr = na - ta->ta_addrs;

	return OK;
}
#endif

/*  */

struct PSAPaddr *
aei2addr_stub(aei)
	AEI aei;
{
	register struct isoentity *ie = &ies;
	register struct PSAPaddr *pa;

	if (aei->aei_ap_title ? oid_cmp(&ie->ie_identifier, prim2oid(aei->aei_ap_title))
	    : ie->ie_identifier.oid_nelem > 0) {
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ("aei2addr_stub cache miss on %s", sprintaei(aei)));

		return NULLPA;
	}
	pa = &ie->ie_addr;
	if (addr_log->ll_events & LLOG_DEBUG)
		_printent(ie);

	return (pa->pa_addr.sa_addr.ta_naddr > 0 ? pa : NULLPA);
}

/*  */

#ifdef	LPP
/* ARGSUSED */

AEI
str2aei_dse(designator, qualifier, context, interactive)
	char *designator, *qualifier, *context;
	int interactive;
{
	SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("DSE lookup not supported for LPP"));
	return NULLAEI;
}

/* ARGSUSED */

struct PSAPaddr *
aei2addr_dse(aei)
	AEI aei;
{
	SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP, ("DSE lookup not supported for LPP"));

	return NULLPA;
}
#endif
