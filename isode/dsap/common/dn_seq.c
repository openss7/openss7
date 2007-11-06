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

/* dn_seq.c - General Directory Name Sequence routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/dn_seq.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/dn_seq.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: dn_seq.c,v
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
#include "quipu/entry.h"

dn_seq_free(dnseq)
	struct dn_seq *dnseq;
{
	struct dn_seq *ptr;
	struct dn_seq *next;

	for (ptr = dnseq; ptr != NULLDNSEQ; ptr = next) {
		next = ptr->dns_next;
		dn_free(ptr->dns_dn);
		free((char *) ptr);
	}

}

struct dn_seq *
dn_seq_cpy(dnseq)
	struct dn_seq *dnseq;
{
	register struct dn_seq *ptr;
	struct dn_seq *ptr2;
	struct dn_seq *result = NULLDNSEQ;

	for (ptr = dnseq; ptr != NULLDNSEQ; ptr = ptr->dns_next) {
		ptr2 = dn_seq_alloc();
		ptr2->dns_next = result;
		result = ptr2;
		result->dns_dn = dn_cpy(ptr->dns_dn);
	}
	return (result);
}

check_dnseq(dnseq, who)
	struct dn_seq *dnseq;
	DN who;
{
	register struct dn_seq *ptr;

	for (ptr = dnseq; ptr != NULLDNSEQ; ptr = ptr->dns_next) {
		if (dn_cmp(who, ptr->dns_dn) == OK)
			return (OK);
	}

	return (NOTOK);
}

dn_seq_cmp(a, b)
	struct dn_seq *a, *b;
{
	struct dn_seq *dns1;
	struct dn_seq *dns2;

	if ((a == NULLDNSEQ) && (b == NULLDNSEQ))
		return (0);

	if (a == NULLDNSEQ)
		return (-1);

	if (b == NULLDNSEQ)
		return (1);

	for (dns1 = a; dns1 != NULLDNSEQ; dns1 = dns1->dns_next) {
		for (dns2 = b; dns2 != NULLDNSEQ; dns2 = dns2->dns_next) {
			if (dn_cmp(dns1->dns_dn, dns2->dns_dn) == 0)
				break;
		}
		if (dns2 == NULLDNSEQ)
			return (1);
	}

	for (dns2 = b; dns2 != NULLDNSEQ; dns2 = dns2->dns_next) {
		for (dns1 = a; dns1 != NULLDNSEQ; dns1 = dns1->dns_next) {
			if (dn_cmp(dns1->dns_dn, dns2->dns_dn) == 0)
				break;
		}
		if (dns1 == NULLDNSEQ)
			return (-1);
	}

	return (0);
}

check_dnseq_prefix(dnseq, who)
	struct dn_seq *dnseq;
	DN who;
{
	struct dn_seq *ptr;

	for (ptr = dnseq; ptr != NULLDNSEQ; ptr = ptr->dns_next) {
		if (dn_cmp_prefix(ptr->dns_dn, who) == OK)
			return (OK);
	}

	return (NOTOK);
}

dn_seq_print(ps, dnseq, format)
	PS ps;
	struct dn_seq *dnseq;
	int format;
{
	if (dnseq == NULLDNSEQ)
		return;

	dn_print(ps, dnseq->dns_dn, EDBOUT);
	for (dnseq = dnseq->dns_next; dnseq != NULLDNSEQ; dnseq = dnseq->dns_next) {
		if (format == READOUT)
			ps_print(ps, " AND ");
		else
			ps_print(ps, "$");
		dn_print(ps, dnseq->dns_dn, EDBOUT);
	}
}

struct dn_seq *
str2dnseq(str)
	register char *str;
{
	register char *ptr;
	register char *save, val;
	struct dn_seq *dns = NULLDNSEQ;
	struct dn_seq *newdns;

	while ((ptr = index(str, '$')) != 0) {
		save = ptr++;
		save--;
		if (!isspace(*save))
			save++;
		val = *save;
		*save = 0;
		newdns = dn_seq_alloc();
		if ((newdns->dns_dn = str2dn(str)) == NULLDN) {
			dn_seq_free(dns);
			free((char *) newdns);
			return (NULLDNSEQ);
		}
		newdns->dns_next = dns;
		dns = newdns;
		*save = val;
		str = ptr;
	}

	if ((ptr = rindex(str, '#')) != 0) {
		/* a bit or reverse compatability... */
		if (*++ptr != 0) {
			parse_error("invalid # in sequence '%s'", str);
			dn_seq_free(dns);
			return (NULLDNSEQ);
		} else
			*--ptr = 0;
	}

	newdns = dn_seq_alloc();
	if ((newdns->dns_dn = str2dn(str)) == NULLDN) {
		dn_seq_free(dns);
		free((char *) newdns);
		return (NULLDNSEQ);
	}
	newdns->dns_next = dns;
	dns = newdns;

	return (dns);
}

int
dn_in_dnseq(dn, dnseq)
	DN dn;
	struct dn_seq *dnseq;
{
	struct dn_seq *ptr;
	register int i = 1;

	for (ptr = dnseq; ptr != NULLDNSEQ; ptr = ptr->dns_next, i++) {
		if (dn_cmp(dn, ptr->dns_dn) == 0)
			break;
	}

	if (ptr == NULLDNSEQ)
		return (FALSE);

	return (i);
}

struct dn_seq *
dn_seq_push(dn, dnseq)
	DN dn;
	struct dn_seq *dnseq;
{
	struct dn_seq *ret;

	ret = dn_seq_alloc();

	ret->dns_dn = dn_cpy(dn);
	ret->dns_next = dnseq;

	return (ret);
}

struct dn_seq *
dn_seq_pop(dnseq)
	struct dn_seq *dnseq;
{
	struct dn_seq *ret;

	if (dnseq == NULLDNSEQ)
		return (NULLDNSEQ);

	ret = dnseq->dns_next;

	dn_free(dnseq->dns_dn);
	free((char *) dnseq);

	return (ret);
}
